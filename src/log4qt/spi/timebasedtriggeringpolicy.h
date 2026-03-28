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

#ifndef LOG4QT_TIMEBASEDTRIGGERINGPOLICY_H
#define LOG4QT_TIMEBASEDTRIGGERINGPOLICY_H

#include "triggeringpolicy.h"

#include <QDateTime>

namespace Log4Qt
{

/*!
 * \brief The class TimeBasedTriggeringPolicy triggers a rollover based on
 *        a time/date pattern.
 *
 * The date pattern determines the rollover frequency. The policy triggers
 * when the current time passes the computed rollover time.
 *
 * Supported frequencies (determined automatically from the pattern):
 * - MINUTELY: pattern changes every minute
 * - HOURLY: pattern changes every hour
 * - HALFDAILY: pattern changes every 12 hours
 * - DAILY: pattern changes every day
 * - WEEKLY: pattern changes every week
 * - MONTHLY: pattern changes every month
 *
 * \note All the functions declared in this class are thread-safe.
 */
class LOG4QT_EXPORT TimeBasedTriggeringPolicy : public TriggeringPolicy
{
    Q_OBJECT

    /*!
     * The date pattern used to determine the rollover frequency.
     * The default is "'.'yyyy-MM-dd" (daily).
     */
    Q_PROPERTY(QString datePattern READ datePattern WRITE setDatePattern)

public:
    enum Frequency
    {
        MINUTELY_ROLLOVER = 0,
        HOURLY_ROLLOVER,
        HALFDAILY_ROLLOVER,
        DAILY_ROLLOVER,
        WEEKLY_ROLLOVER,
        MONTHLY_ROLLOVER
    };
    Q_ENUM(Frequency)

    explicit TimeBasedTriggeringPolicy(QObject *parent = nullptr);

    QString datePattern() const;
    void setDatePattern(const QString &datePattern);

    Frequency frequency() const;

    void activateOptions() override;

    bool isTriggeringEvent(const QString &fileName,
                           qint64 fileSize,
                           const LoggingEvent &event) override;

private:
    Q_DISABLE_COPY_MOVE(TimeBasedTriggeringPolicy)

    void computeFrequency();
    void computeRollOverTime();

    QString mDatePattern;
    QString mActiveDatePattern;
    Frequency mFrequency;
    QDateTime mRollOverTime;
};

inline QString TimeBasedTriggeringPolicy::datePattern() const
{
    return mDatePattern;
}

inline void TimeBasedTriggeringPolicy::setDatePattern(const QString &datePattern)
{
    mDatePattern = datePattern;
}

inline TimeBasedTriggeringPolicy::Frequency TimeBasedTriggeringPolicy::frequency() const
{
    return mFrequency;
}

} // namespace Log4Qt

#endif // LOG4QT_TIMEBASEDTRIGGERINGPOLICY_H
