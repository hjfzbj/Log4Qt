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

#include "asyncappender.h"
#include "loggingevent.h"
#include "helpers/dispatcher.h"

#include <QCoreApplication>
#include <QReadLocker>
#include <QThread>

namespace Log4Qt
{

AsyncAppender::AsyncAppender(QObject *parent) 
    : AppenderSkeleton(parent)
{}

AsyncAppender::~AsyncAppender()
{
    closeInternal();
}

bool AsyncAppender::requiresLayout() const
{
    return false;
}

void AsyncAppender::activateOptions()
{
    if (mpThread)
        return;

    mpThread = std::make_unique<QThread>();
    mpDispatcher = std::make_unique<Dispatcher>();
    mpDispatcher->setAsyncAppender(this);

    mpDispatcher->moveToThread(mpThread.get());
    mpThread->start();
}

void AsyncAppender::close()
{
    closeInternal();
    AppenderSkeleton::close();
}

void AsyncAppender::closeInternal()
{
    QMutexLocker locker(&mObjectGuard);

    if (isClosed())
        return;

    if (mpThread)
    {
        if (mpDispatcher)
            mpDispatcher->setAsyncAppender(nullptr);
        
        mpThread->quit();
        mpThread->wait();
        
        // Automatic cleanup via unique_ptr destructors
        mpDispatcher.reset();
        mpThread.reset();
    }
}

void AsyncAppender::callAppenders(const LoggingEvent &event) const
{
    QReadLocker locker(&mAppenderGuard);

    for (const auto& appender : mAppenders)
        appender->doAppend(event);
}

void AsyncAppender::append(const LoggingEvent &event)
{
    if (mpDispatcher)
        qApp->postEvent(mpDispatcher.get(), new LoggingEvent(event));
}

bool AsyncAppender::checkEntryConditions() const
{
    if (mpThread && !mpThread->isRunning())
    {
        LogError e =
            LOG4QT_QCLASS_ERROR(QT_TR_NOOP("Use of appender '%1' without a running dispatcher thread"),
                                AppenderAsncDispatcherNotRunning);
        e << name();
        logger()->error(e);
        return false;
    }

    return AppenderSkeleton::checkEntryConditions();
}

} // namespace Log4Qt

#include "moc_asyncappender.cpp"
