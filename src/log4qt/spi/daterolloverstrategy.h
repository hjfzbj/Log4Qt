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

#ifndef LOG4QT_DATEROLLOVERSTRATEGY_H
#define LOG4QT_DATEROLLOVERSTRATEGY_H

#include "rolloverstrategy.h"

#include <QFutureSynchronizer>
#include <QString>

namespace Log4Qt
{

/*!
 * \brief The class DateRolloverStrategy performs date-based rotation
 *        of backup files.
 *
 * Two naming modes are supported:
 *
 * - \b Suffix (default): the active file is renamed by appending a date
 *   suffix. For example, \c app.log becomes \c app.log.2026-03-28.
 *   The appender re-opens the same base filename.
 *
 * - \b Embedded: the date is embedded in the filename between the
 *   basename and extension. For example, \c app.log becomes
 *   \c app_2026-03-28.log. The appender opens the new dated filename.
 *
 * The \c datePattern property controls how the date is formatted using
 * Qt's QDateTime::toString() format strings.
 *
 * The \c maxBackups property limits the number of backup files kept.
 * When set to 0 (the default), backups accumulate without limit.
 *
 * \note All the functions declared in this class are thread-safe.
 */
class LOG4QT_EXPORT DateRolloverStrategy : public RolloverStrategy
{
    Q_OBJECT

    /*!
     * The date format pattern for backup file names.
     * The default is "'.'yyyy-MM-dd".
     */
    Q_PROPERTY(QString datePattern READ datePattern WRITE setDatePattern)

    /*!
     * The naming mode: "Suffix" or "Embedded".
     * The default is "Suffix".
     */
    Q_PROPERTY(QString mode READ modeString WRITE setModeString)

    /*!
     * Maximum number of backup files to keep.
     * 0 means unlimited. The default is 0.
     */
    Q_PROPERTY(int maxBackups READ maxBackups WRITE setMaxBackups)

public:
    enum NamingMode
    {
        Suffix = 0,
        Embedded
    };
    Q_ENUM(NamingMode)

    explicit DateRolloverStrategy(QObject *parent = nullptr);

    QString datePattern() const;
    void setDatePattern(const QString &datePattern);

    NamingMode mode() const;
    void setMode(NamingMode mode);

    QString modeString() const;
    void setModeString(const QString &mode);

    int maxBackups() const;
    void setMaxBackups(int maxBackups);

    void activateOptions() override;
    QString rollover(const QString &fileName) override;

private:
    Q_DISABLE_COPY_MOVE(DateRolloverStrategy)

    QString buildBackupName(const QString &fileName, const QDateTime &dateTime) const;

    QString mDatePattern;
    NamingMode mMode;
    int mMaxBackups;
    QString mActiveSuffix;
    QFutureSynchronizer<void> mCleanupExecutors;
};

inline QString DateRolloverStrategy::datePattern() const
{
    return mDatePattern;
}

inline void DateRolloverStrategy::setDatePattern(const QString &datePattern)
{
    mDatePattern = datePattern;
}

inline DateRolloverStrategy::NamingMode DateRolloverStrategy::mode() const
{
    return mMode;
}

inline void DateRolloverStrategy::setMode(NamingMode mode)
{
    mMode = mode;
}

inline int DateRolloverStrategy::maxBackups() const
{
    return mMaxBackups;
}

inline void DateRolloverStrategy::setMaxBackups(int maxBackups)
{
    mMaxBackups = maxBackups;
}

} // namespace Log4Qt

#endif // LOG4QT_DATEROLLOVERSTRATEGY_H
