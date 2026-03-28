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

#include "spi/timebasedtriggeringpolicy.h"

#include "log4qtdefs.h"
#include "helpers/datetime.h"

namespace Log4Qt
{

TimeBasedTriggeringPolicy::TimeBasedTriggeringPolicy(QObject *parent) :
    TriggeringPolicy(parent),
    mDatePattern(u"'.'yyyy-MM-dd"_s),
    mFrequency(DAILY_ROLLOVER)
{
}

void TimeBasedTriggeringPolicy::activateOptions()
{
    computeFrequency();
    if (!mActiveDatePattern.isEmpty())
        computeRollOverTime();
}

bool TimeBasedTriggeringPolicy::isTriggeringEvent(const QString &fileName,
                                                    qint64 fileSize,
                                                    const LoggingEvent &event)
{
    Q_UNUSED(fileName)
    Q_UNUSED(fileSize)
    Q_UNUSED(event)

    if (mActiveDatePattern.isEmpty())
        return false;

    if (QDateTime::currentDateTime() > mRollOverTime)
    {
        computeRollOverTime();
        return true;
    }
    return false;
}

void TimeBasedTriggeringPolicy::computeFrequency()
{
    const DateTime startTime(QDate(1999, 1, 1), QTime(0, 0));
    const QString startString = startTime.toString(mDatePattern);
    mActiveDatePattern.clear();

    if (startString != static_cast<DateTime>(startTime.addSecs(60)).toString(mDatePattern))
        mFrequency = MINUTELY_ROLLOVER;
    else if (startString != static_cast<DateTime>(startTime.addSecs(60 * 60)).toString(mDatePattern))
        mFrequency = HOURLY_ROLLOVER;
    else if (startString != static_cast<DateTime>(startTime.addSecs(60 * 60 * 12)).toString(mDatePattern))
        mFrequency = HALFDAILY_ROLLOVER;
    else if (startString != static_cast<DateTime>(startTime.addDays(1)).toString(mDatePattern))
        mFrequency = DAILY_ROLLOVER;
    else if (startString != static_cast<DateTime>(startTime.addDays(7)).toString(mDatePattern))
        mFrequency = WEEKLY_ROLLOVER;
    else if (startString != static_cast<DateTime>(startTime.addMonths(1)).toString(mDatePattern))
        mFrequency = MONTHLY_ROLLOVER;
    else
        return;

    mActiveDatePattern = mDatePattern;
}

void TimeBasedTriggeringPolicy::computeRollOverTime()
{
    Q_ASSERT_X(!mActiveDatePattern.isEmpty(), "TimeBasedTriggeringPolicy::computeRollOverTime()", "No active date pattern");

    QDateTime now = QDateTime::currentDateTime();
    QDate nowDate = now.date();
    QTime nowTime = now.time();
    QDateTime start;

    switch (mFrequency)
    {
    case MINUTELY_ROLLOVER:
        start = QDateTime(nowDate, QTime(nowTime.hour(), nowTime.minute(), 0, 0));
        mRollOverTime = start.addSecs(60);
        break;
    case HOURLY_ROLLOVER:
        start = QDateTime(nowDate, QTime(nowTime.hour(), 0, 0, 0));
        mRollOverTime = start.addSecs(60 * 60);
        break;
    case HALFDAILY_ROLLOVER:
    {
        int hour = nowTime.hour() >= 12 ? 12 : 0;
        start = QDateTime(nowDate, QTime(hour, 0, 0, 0));
        mRollOverTime = start.addSecs(60 * 60 * 12);
        break;
    }
    case DAILY_ROLLOVER:
        start = QDateTime(nowDate, QTime(0, 0, 0, 0));
        mRollOverTime = start.addDays(1);
        break;
    case WEEKLY_ROLLOVER:
    {
        int day = nowDate.dayOfWeek();
        if (day == Qt::Sunday)
            day = 0;
        start = QDateTime(nowDate, QTime(0, 0, 0, 0)).addDays(-1 * day);
        mRollOverTime = start.addDays(7);
        break;
    }
    case MONTHLY_ROLLOVER:
        start = QDateTime(QDate(nowDate.year(), nowDate.month(), 1), QTime(0, 0, 0, 0));
        mRollOverTime = start.addMonths(1);
        break;
    }
}

} // namespace Log4Qt

#include "moc_timebasedtriggeringpolicy.cpp"
