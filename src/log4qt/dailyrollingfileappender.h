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

#ifndef LOG4QT_DAILYROLLINGFILEAPPENDER_H
#define LOG4QT_DAILYROLLINGFILEAPPENDER_H

#include "rollingfileappender.h"

#include <QDate>
#include <QFutureSynchronizer>
#include <memory>
#include <QString>

namespace Log4Qt
{

class LOG4QT_EXPORT IDateRetriever
{
public:
    virtual ~IDateRetriever();
    virtual QDate currentDate() const = 0;
};

class LOG4QT_EXPORT DefaultDateRetriever final : public IDateRetriever
{
public:

    /**
     * Return the current date, as reported by the system clock.
     */
    QDate currentDate() const override;
};

/*!
 * \brief The class DailyRollingFileAppender extends FileAppender so that the
 * a log file is created for each day
 */
class LOG4QT_EXPORT DailyRollingFileAppender : public RollingFileAppender
{
    Q_OBJECT

    //! The property holds the date pattern used by the appender.
    Q_PROPERTY(QString datePattern READ datePattern WRITE setDatePattern)

    /**
     * Number of days that old log files will be kept on disk.
     * Set to a positive value to enable automatic deletion. Per default, all files are kept. Check
     * for obsolete files happens once a day.
     */
    Q_PROPERTY(int keepDays READ keepDays WRITE setKeepDays)

public:
    explicit DailyRollingFileAppender(QObject *parent = nullptr);
    DailyRollingFileAppender(const LayoutSharedPtr &layout, const QString &fileName, const QString &datePattern = QString(), int keepDays = 0, QObject *parent = nullptr);

    [[nodiscard]] QString datePattern() const;
    void setDatePattern(const QString &datePattern);

    [[nodiscard]] int keepDays() const;
    void setKeepDays(int keepDays);

    void activateOptions() override;

    void setDateRetriever(const std::shared_ptr<const IDateRetriever> &dateRetriever);

protected:
    void append(const LoggingEvent &event) override;

private:
    Q_DISABLE_COPY_MOVE(DailyRollingFileAppender)

    std::shared_ptr<const IDateRetriever> mDateRetriever;

    QString mDatePattern;
    QDate mLastDate;
    int mKeepDays;
    QString mOriginalFilename;

    QFutureSynchronizer<void> mDeleteObsoleteFilesExecutors;
};

}

#endif // LOG4QT_DAILYROLLINGFILEAPPENDER_H
