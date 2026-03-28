/******************************************************************************
 *
 * This file is part of Log4Qt library.
 *
 * Copyright (C) 2007 - 2026 Log4Qt contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#include "appenderskeleton.h"

#include "layout.h"
#include "loggingevent.h"
#include "logmanager.h"
#include "spi/filter.h"
#include "logger.h"

namespace Log4Qt
{

/*!
 * \brief The class RecursionGuardLocker controls a boolean flag.
 *
 * It is a helper class to control a boolean flag. The class sets the flag
 * on creation and resets it on destruction.
 */
class RecursionGuardLocker
{
public:
    explicit RecursionGuardLocker(bool *guard);
    ~RecursionGuardLocker();
private:
    Q_DISABLE_COPY_MOVE(RecursionGuardLocker)
private:
    bool *mGuard;
};

inline RecursionGuardLocker::RecursionGuardLocker(bool *guard)
{
    Q_ASSERT_X(guard != nullptr, "RecursionGuardLocker::RecursionGuardLocker()", "Pointer to guard bool must not be null");

    mGuard = guard;
    *mGuard = true;
}

inline RecursionGuardLocker::~RecursionGuardLocker()
{
    *mGuard = false;
}

AppenderSkeleton::AppenderSkeleton(QObject *parent) 
    : Appender(parent)
#if QT_VERSION < 0x050E00
    , mObjectGuard(QMutex::Recursive) // Recursive for doAppend()
#endif
    , mAppendRecursionGuard(false)
    , mThreshold(Level::NULL_INT)
{
    mIsActive.store(true, std::memory_order_relaxed);
    mIsClosed.store(false, std::memory_order_relaxed);
}

AppenderSkeleton::AppenderSkeleton(bool isActive,
                                   QObject *parent) 
    : Appender(parent)
#if QT_VERSION < 0x050E00
    , mObjectGuard(QMutex::Recursive) // Recursive for doAppend()
#endif
    , mAppendRecursionGuard(false)
    , mThreshold(Level::NULL_INT)
{
    mIsActive.store(isActive, std::memory_order_relaxed);
    mIsClosed.store(false, std::memory_order_relaxed);
}

AppenderSkeleton::AppenderSkeleton(bool isActive,
                                   const LayoutSharedPtr &layout,
                                   QObject *parent) 
    : Appender(parent)
#if QT_VERSION < 0x050E00
    , mObjectGuard(QMutex::Recursive) // Recursive for doAppend()
#endif
    , mAppendRecursionGuard(false)
    , mpLayout(layout)
    , mThreshold(Level::NULL_INT)
{
    mIsActive.store(isActive, std::memory_order_relaxed);
    mIsClosed.store(false, std::memory_order_relaxed);
}

AppenderSkeleton::~AppenderSkeleton()
{
    closeInternal();
}

void AppenderSkeleton::activateOptions()
{
    QMutexLocker locker(&mObjectGuard);

    if (requiresLayout() && !layout())
    {
        LogError e = LOG4QT_QCLASS_ERROR(QT_TR_NOOP("Activation of appender '%1' that requires layout and has no layout set"),
                                         AppenderActivateMissingLayoutError);
        e << name();
        logger()->error(e);
        return;
    }
    mIsActive.store(true, std::memory_order_relaxed);
}

void AppenderSkeleton::addFilter(const FilterSharedPtr &filter)
{
    if (!filter)
    {
        logger()->warn(u"Adding null Filter to Appender '%1'"_s, name());
        return;
    }

    QMutexLocker locker(&mObjectGuard);

    if (!mpTailFilter)
    {
        // filter list empty
        mpHeadFilter = filter;
        mpTailFilter = filter;
    }
    else
    {
        // append filter to the end of the filter list
        mpTailFilter->setNext(filter);
        mpTailFilter = filter;
    }
}

void AppenderSkeleton::clearFilters()
{
    QMutexLocker locker(&mObjectGuard);

    mpHeadFilter.reset();
}

void AppenderSkeleton::close()
{
    closeInternal();
}

void AppenderSkeleton::closeInternal()
{
    QMutexLocker locker(&mObjectGuard);

    mIsClosed.store(true, std::memory_order_relaxed);
    mIsActive.store(false, std::memory_order_relaxed);
}

void AppenderSkeleton::customEvent(QEvent *event)
{
    if (event->type() == LoggingEvent::eventId)
    {
        const auto logEvent = static_cast<LoggingEvent *>(event);
        doAppend(*logEvent);
        return ;
    }
    QObject::customEvent(event);
}

void AppenderSkeleton::doAppend(const LoggingEvent &event)
{
    // The mutex serialises concurrent access from multiple threads.
    // - e.g. two threads using the same logger
    // - e.g. two threads using different logger with the same appender
    //
    // A call from the same thread will pass the mutex (QMutex::Recursive)
    // and get to the recursion guard. The recursion guard blocks recursive
    // invocation and prevents a possible endless loop.
    // - e.g. an appender logs an error with a logger that uses it

    QMutexLocker locker(&mObjectGuard);

    if (mAppendRecursionGuard)
        return;

    RecursionGuardLocker recursion_locker(&mAppendRecursionGuard);

    if (!checkEntryConditions())
        return;
    if (!isAsSevereAsThreshold(event.level()))
        return;

    const auto  *filter = mpHeadFilter.data();
    while (filter)
    {
        Filter::Decision decision = filter->decide(event);
        if (decision == Filter::Accept)
            break;
        else if (decision == Filter::Deny)
            return;
        else
            filter = filter->next().data();
    }

    append(event);
}

bool AppenderSkeleton::checkEntryConditions() const
{
    if (!isActive())
    {
        LogError e = LOG4QT_QCLASS_ERROR(QT_TR_NOOP("Use of non activated appender '%1'"),
                                         AppenderNotActivatedError);
        e << name();
        logger()->error(e);
        return false;
    }
    if (isClosed())
    {
        LogError e = LOG4QT_QCLASS_ERROR(QT_TR_NOOP("Use of closed appender '%1'"),
                                         AppenderClosedError);
        e << name();
        logger()->error(e);
        return false;
    }
    if (requiresLayout() && !layout())
    {
        LogError e = LOG4QT_QCLASS_ERROR(QT_TR_NOOP("Use of appender '%1' that requires layout and has no layout set"),
                                         AppenderUseMissingLayoutError);
        e << name();
        logger()->error(e);
        return false;
    }

    return true;
}

void Log4Qt::AppenderSkeleton::setLayout(const LayoutSharedPtr &layout)
{
    QMutexLocker locker(&mObjectGuard);
    mpLayout = layout;
}

LayoutSharedPtr Log4Qt::AppenderSkeleton::layout() const
{
    QMutexLocker locker(&mObjectGuard);
    return mpLayout;
}

} // namespace Log4Qt

#include "moc_appenderskeleton.cpp"
