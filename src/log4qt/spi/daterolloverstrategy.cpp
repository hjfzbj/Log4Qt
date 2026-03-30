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

#include "spi/daterolloverstrategy.h"

#include "log4qtdefs.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QtConcurrentRun>

#include <algorithm>

namespace
{

void deleteObsoleteFiles(
        Log4Qt::DateRolloverStrategy::NamingMode mode,
        int maxBackups,
        const QString &fileName)
{
    const QFileInfo fi(fileName);
    const QDir dir(fi.absolutePath());
    const QString base = fi.baseName();
    const QString ext = fi.completeSuffix();

    QString nameFilter;
    if (mode == Log4Qt::DateRolloverStrategy::Suffix)
        nameFilter = fi.fileName() + u"*"_s;
    else
        nameFilter = base + u"*"_s + (ext.isEmpty() ? u""_s : u"."_s + ext);

    QFileInfoList entries = dir.entryInfoList({nameFilter}, QDir::Files, QDir::Name);

    entries.removeIf([&](const QFileInfo &entry) {
        return entry.absoluteFilePath() == fi.absoluteFilePath();
    });

    if (entries.size() <= maxBackups)
        return;

    std::sort(entries.begin(), entries.end(), [](const QFileInfo &a, const QFileInfo &b) {
        return a.lastModified() > b.lastModified();
    });

    for (int i = maxBackups; i < entries.size(); ++i)
        QFile::remove(entries.at(i).absoluteFilePath());
}

} // anonymous namespace

namespace Log4Qt
{

DateRolloverStrategy::DateRolloverStrategy(QObject *parent) :
    RolloverStrategy(parent),
    mDatePattern(u"'.'yyyy-MM-dd"_s),
    mMode(Suffix),
    mMaxBackups(0),
    mDateTimeProvider([]() { return QDateTime::currentDateTime(); })
{
}

QString DateRolloverStrategy::modeString() const
{
    return mMode == Embedded ? u"Embedded"_s : u"Suffix"_s;
}

void DateRolloverStrategy::setModeString(const QString &mode)
{
    if (mode.compare(u"Embedded"_s, Qt::CaseInsensitive) == 0)
        mMode = Embedded;
    else
        mMode = Suffix;
}

void DateRolloverStrategy::setDateTimeProvider(DateTimeProvider provider)
{
    mDateTimeProvider = std::move(provider);
}

void DateRolloverStrategy::activateOptions()
{
    RolloverStrategy::activateOptions();
    mActiveSuffix = currentDateTime().toString(mDatePattern);
}

QDateTime DateRolloverStrategy::currentDateTime() const
{
    return mDateTimeProvider ? mDateTimeProvider() : QDateTime::currentDateTime();
}

QString DateRolloverStrategy::rollover(const QString &fileName)
{
    const auto dateTime = currentDateTime();

    if (mMode == Suffix)
    {
        // Use the active suffix to name the backup after the period it belongs to.
        // mActiveSuffix is set during activateOptions() and updated after each rollover.
        const QString backupName = fileName
            + (mActiveSuffix.isEmpty() ? dateTime.toString(mDatePattern) : mActiveSuffix);
        mActiveSuffix = dateTime.toString(mDatePattern);

        if (QFile::exists(backupName))
            removeFile(backupName);
        if (QFile::exists(fileName))
            renameFile(fileName, backupName);
        if (mMaxBackups > 0)
            mCleanupExecutors.addFuture(
                QtConcurrent::run(deleteObsoleteFiles, mMode, mMaxBackups, fileName));
        return fileName;
    }

    // Embedded mode: return the new dated filename for the current period
    mActiveSuffix = dateTime.toString(mDatePattern);
    const QString backupName = buildBackupName(fileName, dateTime);
    if (mMaxBackups > 0)
        mCleanupExecutors.addFuture(
            QtConcurrent::run(deleteObsoleteFiles, mMode, mMaxBackups, fileName));
    return backupName;
}

QString DateRolloverStrategy::buildBackupName(const QString &fileName,
                                               const QDateTime &dateTime) const
{
    const QString dateStr = dateTime.toString(mDatePattern);

    if (mMode == Suffix)
        return fileName + dateStr;

    // Embedded: insert date between basename and extension
    const QFileInfo fi(fileName);
    const QString dir = fi.absolutePath();
    const QString base = fi.baseName();
    const QString ext = fi.completeSuffix();

    if (ext.isEmpty())
        return dir + u"/"_s + base + dateStr;
    return dir + u"/"_s + base + dateStr + u"."_s + ext;
}

} // namespace Log4Qt

#include "moc_daterolloverstrategy.cpp"
