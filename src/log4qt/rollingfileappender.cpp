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

#include "rollingfileappender.h"

#include "layout.h"
#include "loggingevent.h"
#include "spi/compositetriggeringpolicy.h"
#include "spi/defaultrolloverstrategy.h"

#include <QFileInfo>

namespace Log4Qt
{

RollingFileAppender::RollingFileAppender(QObject *parent) :
    FileAppender(parent)
{
}

RollingFileAppender::RollingFileAppender(const LayoutSharedPtr &layout,
        const QString &fileName,
        QObject *parent) :
    FileAppender(layout, fileName, parent)
{
}

RollingFileAppender::RollingFileAppender(const LayoutSharedPtr &layout,
        const QString &fileName,
        bool append,
        QObject *parent) :
    FileAppender(layout, fileName, append, parent)
{
}

void RollingFileAppender::setTriggeringPolicy(const TriggeringPolicySharedPtr &policy)
{
    QMutexLocker locker(&mObjectGuard);
    mTriggeringPolicy = policy;
}

void RollingFileAppender::addTriggeringPolicy(const TriggeringPolicySharedPtr &policy)
{
    QMutexLocker locker(&mObjectGuard);

    if (!mTriggeringPolicy)
    {
        mTriggeringPolicy = policy;
    }
    else if (auto *composite = qobject_cast<CompositeTriggeringPolicy *>(mTriggeringPolicy.data()))
    {
        composite->addPolicy(policy);
    }
    else
    {
        auto *comp = new CompositeTriggeringPolicy;
        comp->addPolicy(mTriggeringPolicy);
        comp->addPolicy(policy);
        mTriggeringPolicy = TriggeringPolicySharedPtr(comp);
    }
}

void RollingFileAppender::setRolloverStrategy(const RolloverStrategySharedPtr &strategy)
{
    QMutexLocker locker(&mObjectGuard);
    mRolloverStrategy = strategy;
}

void RollingFileAppender::activateOptions()
{
    QMutexLocker locker(&mObjectGuard);

    // Default strategy if none set
    if (!mRolloverStrategy)
        mRolloverStrategy = RolloverStrategySharedPtr(new DefaultRolloverStrategy);

    if (mTriggeringPolicy)
        mTriggeringPolicy->activateOptions();
    mRolloverStrategy->activateOptions();

    // Check startup trigger BEFORE opening the file — openFile() may truncate
    bool startupRollover = false;
    if (mTriggeringPolicy)
    {
        qint64 fileSize = 0;
        QFileInfo fi(file());
        if (fi.exists())
            fileSize = fi.size();
        startupRollover = mTriggeringPolicy->isStartupTrigger(file(), fileSize);
    }

    FileAppender::activateOptions();

    if (startupRollover)
        rollOver();
}

void RollingFileAppender::append(const LoggingEvent &event)
{
    FileAppender::append(event);
    if (mTriggeringPolicy)
    {
        qint64 fileSize = writer()->device()->size();
        if (mTriggeringPolicy->isTriggeringEvent(file(), fileSize, event))
            rollOver();
    }
}

void RollingFileAppender::rollOver()
{
    logger()->debug(u"Rolling over with strategy %1"_s,
                    QLatin1String(mRolloverStrategy->metaObject()->className()));

    closeFile();
    QString nextFile = mRolloverStrategy->rollover(file());
    if (nextFile != file())
        setFile(nextFile);
    FileAppender::openFile();
}

} // namespace Log4Qt

#include "moc_rollingfileappender.cpp"
