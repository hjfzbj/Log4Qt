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

#include "randomaccessfileappender.h"
#include "layout.h"
#include "loggingevent.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMutexLocker>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace Log4Qt
{

RandomAccessFileAppender::RandomAccessFileAppender(QObject *parent)
    : AppenderSkeleton(false, parent)
    , mAppendFile(false)
    , mBufferSize(256 * 1024)
    , mImmediateFlush(false)
{
}

RandomAccessFileAppender::RandomAccessFileAppender(const LayoutSharedPtr &layout,
                                                   const QString &fileName,
                                                   QObject *parent)
    : AppenderSkeleton(false, layout, parent)
    , mAppendFile(false)
    , mBufferSize(256 * 1024)
    , mImmediateFlush(false)
    , mFileName(fileName)
{
}

RandomAccessFileAppender::RandomAccessFileAppender(const LayoutSharedPtr &layout,
                                                   const QString &fileName,
                                                   bool append,
                                                   QObject *parent)
    : AppenderSkeleton(false, layout, parent)
    , mAppendFile(append)
    , mBufferSize(256 * 1024)
    , mImmediateFlush(false)
    , mFileName(fileName)
{
}

RandomAccessFileAppender::~RandomAccessFileAppender()
{
    closeInternal();
}

bool RandomAccessFileAppender::requiresLayout() const
{
    return true;
}

void RandomAccessFileAppender::activateOptions()
{
    QMutexLocker locker(&mObjectGuard);

    if (mFileName.isEmpty())
    {
        LogError e = LOG4QT_QCLASS_ERROR(QT_TR_NOOP("Activation of Appender '%1' that requires file and has no file set"),
                                         AppenderActivateMissingFileError);
        e << name();
        logger()->error(e);
        return;
    }

    closeFile();
    openFile();

    if (mFile && mFile->isOpen())
    {
        mByteBuffer.reserve(mBufferSize.load(std::memory_order_relaxed));
        AppenderSkeleton::activateOptions();
    }
}

void RandomAccessFileAppender::close()
{
    closeInternal();
    AppenderSkeleton::close();
}

void RandomAccessFileAppender::closeInternal()
{
    QMutexLocker locker(&mObjectGuard);

    if (isClosed())
        return;

    closeFile();
}

bool RandomAccessFileAppender::checkEntryConditions() const
{
    if (!mFile || !mFile->isOpen())
    {
        LogError e = LOG4QT_QCLASS_ERROR(QT_TR_NOOP("Use of appender '%1' without open file"),
                                         AppenderNoOpenFileError);
        e << name();
        logger()->error(e);
        return false;
    }

    return AppenderSkeleton::checkEntryConditions();
}

bool RandomAccessFileAppender::handleIoErrors() const
{
    if (mFile->error() == QFile::NoError)
        return false;

    LogError e = LOG4QT_QCLASS_ERROR(QT_TR_NOOP("Unable to write to file '%1' for appender '%2'"),
                                     AppenderWritingFileError);
    e << mFileName << name();
    e.addCausingError(LogError(mFile->errorString(), mFile->error()));
    logger()->error(e);
    return true;
}

void RandomAccessFileAppender::append(const LoggingEvent &event)
{
    const QByteArray encoded = layout()->format(event).toUtf8();

    if (mByteBuffer.size() + encoded.size() > mBufferSize.load(std::memory_order_relaxed))
        flushBuffer();

    mByteBuffer.append(encoded);

    if (mImmediateFlush.load(std::memory_order_relaxed))
        flushBuffer();
}

void RandomAccessFileAppender::flushBuffer()
{
    if (mByteBuffer.isEmpty())
        return;

    mFile->write(mByteBuffer);
    handleIoErrors();
    mByteBuffer.clear();
    // Note: clear() preserves the reserved capacity for reuse
}

void RandomAccessFileAppender::openFile()
{
    Q_ASSERT_X(!mFile, "RandomAccessFileAppender::openFile()", "Opening file without closing previous file");

    QFileInfo fileInfo(mFileName);
    QDir parentDir = fileInfo.dir();
    if (!parentDir.exists())
    {
        logger()->trace(u"Creating missing parent directory for file %1"_s, mFileName);
        QString dirName = parentDir.dirName();
        parentDir.cdUp();
        parentDir.mkdir(dirName);
    }

#ifdef Q_OS_WIN
    wchar_t buffer[MAX_PATH];
    if (ExpandEnvironmentStringsW(mFileName.toStdWString().c_str(), buffer, MAX_PATH))
        mFileName = QString::fromWCharArray(buffer);
#endif

    mFile = std::make_unique<QFile>(mFileName);
    QFile::OpenMode mode = QIODevice::WriteOnly;
    // No QIODevice::Text — we write raw UTF-8; the layout's endOfLine() already
    // returns the correct platform-specific line ending.
    // No QIODevice::Unbuffered — we manage the buffer ourselves.
    if (mAppendFile.load(std::memory_order_relaxed))
        mode |= QIODevice::Append;
    else
        mode |= QIODevice::Truncate;

    if (!mFile->open(mode))
    {
        LogError e = LOG4QT_QCLASS_ERROR(QT_TR_NOOP("Unable to open file '%1' for appender '%2'"),
                                         AppenderOpeningFileError);
        e << mFileName << name();
        e.addCausingError(LogError(mFile->errorString(), mFile->error()));
        logger()->error(e);
        mFile.reset();
        return;
    }
    logger()->debug(u"Opened file '%1' for appender '%2'"_s, mFile->fileName(), name());
}

void RandomAccessFileAppender::closeFile()
{
    if (mFile)
    {
        logger()->debug(u"Closing file '%1' for appender '%2'"_s, mFile->fileName(), name());
        flushBuffer();
    }
    mFile.reset();
    mByteBuffer.clear();
}

bool RandomAccessFileAppender::removeFile(QFile &file) const
{
    if (file.remove())
        return true;

    LogError e = LOG4QT_QCLASS_ERROR(QT_TR_NOOP("Unable to remove file '%1' for appender '%2'"),
                                     AppenderRemoveFileError);
    e << file.fileName() << name();
    e.addCausingError(LogError(file.errorString(), file.error()));
    logger()->error(e);
    return false;
}

bool RandomAccessFileAppender::renameFile(QFile &file, const QString &fileName) const
{
    logger()->debug(u"Renaming file '%1' to '%2'"_s, file.fileName(), fileName);
    if (file.rename(fileName))
        return true;

    LogError e = LOG4QT_QCLASS_ERROR(QT_TR_NOOP("Unable to rename file '%1' to '%2' for appender '%3'"),
                                     AppenderRenamingFileError);
    e << file.fileName() << fileName << name();
    e.addCausingError(LogError(file.errorString(), file.error()));
    logger()->error(e);
    return false;
}

QString RandomAccessFileAppender::file() const
{
    QMutexLocker locker(&mObjectGuard);
    return mFileName;
}

void RandomAccessFileAppender::setFile(const QString &fileName)
{
    QMutexLocker locker(&mObjectGuard);
    mFileName = fileName;
}

void RandomAccessFileAppender::setBufferSize(int bufferSize)
{
    QMutexLocker locker(&mObjectGuard);
    mBufferSize.store(bufferSize, std::memory_order_relaxed);
    if (mFile && mFile->isOpen())
        mByteBuffer.reserve(bufferSize);
}

} // namespace Log4Qt

#include "moc_randomaccessfileappender.cpp"
