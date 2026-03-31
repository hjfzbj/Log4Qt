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

#include "spi/crontriggeringpolicy.h"

#include "log4qtdefs.h"

namespace Log4Qt
{

CronTriggeringPolicy::CronTriggeringPolicy(QObject *parent) :
    TriggeringPolicy(parent),
    mSchedule(u"0 0 0 * * ?"_s)
{
}

void CronTriggeringPolicy::activateOptions()
{
    mCronExpression = CronExpression(mSchedule);
    if (!mCronExpression.isValid())
    {
        // Leave mNextFireTime invalid so isTriggeringEvent always returns false
        mNextFireTime = QDateTime();
        return;
    }
    computeNextFireTime();
}

bool CronTriggeringPolicy::isTriggeringEvent(QIODevice *activeDevice,
                                               const LoggingEvent &event)
{
    Q_UNUSED(activeDevice)
    Q_UNUSED(event)

    if (!mNextFireTime.isValid())
        return false;

    if (QDateTime::currentDateTime() >= mNextFireTime)
    {
        computeNextFireTime();
        return true;
    }
    return false;
}

void CronTriggeringPolicy::computeNextFireTime()
{
    mNextFireTime = mCronExpression.nextFireTime(QDateTime::currentDateTime());
}

} // namespace Log4Qt

#include "moc_crontriggeringpolicy.cpp"
