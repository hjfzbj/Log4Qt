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

#include "dailyfileappender.h"

#include "loggingevent.h"
#include "spi/daterolloverstrategy.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStringList>
#include <QtConcurrentRun>

#include <algorithm>

namespace Log4Qt
{

IDateRetriever::~IDateRetriever() = default;

QDate DefaultDateRetriever::currentDate() const
{
    return QDate::currentDate();
}

constexpr char defaultDatePattern[] = "_yyyy_MM_dd";

DailyFileAppender::DailyFileAppender(QObject *parent)
    : RollingFileAppender(parent)
    , mDateRetriever(std::make_shared<DefaultDateRetriever>())
    , mDatePattern(defaultDatePattern)
    , mKeepDays(0)
{
}

DailyFileAppender::DailyFileAppender(const LayoutSharedPtr &layout, const QString &fileName, const QString &datePattern, const int keepDays, QObject *parent)
    : RollingFileAppender(layout, fileName, parent)
    , mDateRetriever(std::make_shared<DefaultDateRetriever>())
    , mDatePattern(datePattern.isEmpty() ? defaultDatePattern : datePattern)
    , mKeepDays(keepDays)
{
}

QString DailyFileAppender::datePattern() const
{
    QMutexLocker locker(&mObjectGuard);
    return mDatePattern;
}

void DailyFileAppender::setDatePattern(const QString &datePattern)
{
    QMutexLocker locker(&mObjectGuard);
    mDatePattern = datePattern;
}

int DailyFileAppender::keepDays() const
{
    QMutexLocker locker(&mObjectGuard);
    return mKeepDays;
}

void DailyFileAppender::setKeepDays(const int keepDays)
{
    QMutexLocker locker(&mObjectGuard);
    mKeepDays = keepDays;
}

namespace
{

void deleteObsoleteFiles(
        QDate currentDate,
        const QString &datePattern,
        int keepDays,
        const QString &originalFilename)
{
    const QFileInfo fi(originalFilename);
    const QDir logDir(fi.absolutePath());
    const auto logFileNames(
                logDir.entryList(
                    QStringList(u"*."_s + fi.completeSuffix()),
                    QDir::NoSymLinks | QDir::Files));

    const QRegularExpression creationDateExtractor(
                fi.baseName() % u"(.*)"_s % u"."_s % fi.completeSuffix());

    const auto startOfLogging(currentDate.addDays(-keepDays));

    // Helper to check if file is obsolete
    auto isObsolete = [&](const QString& fileName) -> bool {
        // determine creation date from file name instead of using file attributes, since file might
        // have been moved around, modified by user etc.
        const auto match = creationDateExtractor.match(fileName);
        if (!match.hasMatch())
            return false;
        
        const auto creationDate = QDate::fromString(match.captured(1), datePattern);
        return creationDate.isValid() && creationDate < startOfLogging;
    };

    // Single-pass: filter and delete in one go
    std::for_each(logFileNames.begin(), logFileNames.end(),
                  [&](const QString& fileName) {
                      if (isObsolete(fileName)) {
                          QFile::remove(logDir.filePath(fileName));
                      }
                  });
}

}

void DailyFileAppender::activateOptions()
{
    QMutexLocker locker(&mObjectGuard);

    Q_ASSERT_X(mDateRetriever, "DailyFileAppender::activateOptions()", "No date retriever set");

    if (mOriginalFilename.isEmpty())
        mOriginalFilename = file();

    // Set up DateRolloverStrategy in Embedded mode for filename construction.
    // maxBackups=0 because DailyFileAppender manages its own date-based cleanup via keepDays.
    auto *strategy = new DateRolloverStrategy;
    strategy->setMode(DateRolloverStrategy::Embedded);
    strategy->setDatePattern(mDatePattern);
    strategy->setMaxBackups(0);
    strategy->setDateTimeProvider([retriever = mDateRetriever]() {
        return QDateTime(retriever->currentDate(), QTime(0, 0));
    });
    setRolloverStrategy(RolloverStrategySharedPtr(strategy));

    // Compute today's dated filename via the strategy
    mLastDate = mDateRetriever->currentDate();
    closeFile();
    setFile(strategy->rollover(mOriginalFilename));

    if (mKeepDays > 0 && !mOriginalFilename.isEmpty())
        deleteObsoleteFiles(mLastDate, mDatePattern, mKeepDays, mOriginalFilename);

    FileAppender::activateOptions();
}

void DailyFileAppender::append(const LoggingEvent &event)
{
    Q_ASSERT_X(mDateRetriever, "DailyFileAppender::append()", "No date retriever set");

    const auto currentDate(mDateRetriever->currentDate());

    if (currentDate != mLastDate)
    {
        mLastDate = currentDate;

        // Reset to base filename so the strategy receives the un-dated name
        setFile(mOriginalFilename);
        rollOver();

        // schedule check for obsolete files for asynchronous execution, destructor will wait for
        // completion of each executor
        if (mKeepDays > 0 && !mOriginalFilename.isEmpty())
        {
            mDeleteObsoleteFilesExecutors.addFuture(
                        QtConcurrent::run(
                            deleteObsoleteFiles,
                            currentDate, mDatePattern, mKeepDays, mOriginalFilename));
        }
    }
    FileAppender::append(event);
}

void DailyFileAppender::setDateRetriever(const std::shared_ptr<const IDateRetriever> &dateRetriever)
{
    QMutexLocker locker(&mObjectGuard);

    mDateRetriever = dateRetriever;
}

}

#include "moc_dailyfileappender.cpp"
