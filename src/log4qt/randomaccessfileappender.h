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

#ifndef LOG4QT_RANDOMACCESSFILEAPPENDER_H
#define LOG4QT_RANDOMACCESSFILEAPPENDER_H

#include "appenderskeleton.h"

#include <atomic>
#include <memory>

class QFile;

namespace Log4Qt
{

/*!
 * \brief The class RandomAccessFileAppender appends log events directly
 *        to a file using a pre-allocated byte buffer, bypassing QTextStream.
 *
 * Compared to FileAppender, this appender eliminates the QTextStream
 * abstraction layer. Formatted messages are encoded to UTF-8 and
 * accumulated in an in-memory byte buffer before being written to disk
 * via a single QFile::write() call per flush. The default buffer size is
 * 256 KB (262144 bytes), matching log4j 2's RandomAccessFileAppender.
 *
 * The default for \c immediateFlush is \c false to maximise throughput.
 * Set it to \c true when log entries must be visible immediately (e.g.
 * for debugging crashes).
 *
 * \note All the functions declared in this class are thread-safe.
 *
 * \note The ownership and lifetime of objects of this class are managed. See
 *       \ref Ownership "Object ownership" for more details.
 */
class LOG4QT_EXPORT RandomAccessFileAppender : public AppenderSkeleton
{
    Q_OBJECT

    /*!
     * The property holds whether the output is appended to an existing file.
     *
     * The default is false (truncate on open).
     *
     * \sa appendFile(), setAppendFile()
     */
    Q_PROPERTY(bool appendFile READ appendFile WRITE setAppendFile)

    /*!
     * The property holds the name of the log file.
     *
     * \sa file(), setFile()
     */
    Q_PROPERTY(QString file READ file WRITE setFile)

    /*!
     * The property holds the size of the in-memory byte buffer in bytes.
     *
     * The default is 262144 (256 KB).
     *
     * \sa bufferSize(), setBufferSize()
     */
    Q_PROPERTY(int bufferSize READ bufferSize WRITE setBufferSize)

    /*!
     * The property holds whether the buffer is flushed after every append.
     *
     * The default is false.
     *
     * \sa immediateFlush(), setImmediateFlush()
     */
    Q_PROPERTY(bool immediateFlush READ immediateFlush WRITE setImmediateFlush)

public:
    explicit RandomAccessFileAppender(QObject *parent = nullptr);
    RandomAccessFileAppender(const LayoutSharedPtr &layout,
                             const QString &fileName,
                             QObject *parent = nullptr);
    RandomAccessFileAppender(const LayoutSharedPtr &layout,
                             const QString &fileName,
                             bool append,
                             QObject *parent = nullptr);
    ~RandomAccessFileAppender() override;

private:
    Q_DISABLE_COPY_MOVE(RandomAccessFileAppender)

public:
    [[nodiscard]] inline bool appendFile() const;
    [[nodiscard]] QString file() const;
    [[nodiscard]] inline int bufferSize() const;
    [[nodiscard]] inline bool immediateFlush() const;

    inline void setAppendFile(bool append);
    void setFile(const QString &fileName);
    void setBufferSize(int bufferSize);
    inline void setImmediateFlush(bool immediateFlush);

    bool requiresLayout() const override;

    void activateOptions() override;
    void close() override;

protected:
    void append(const LoggingEvent &event) override;

    /*!
     * Tests if all entry conditions for using append() in this class are met.
     *
     * Checks that a file is open (AppenderNoOpenFileError), then delegates
     * to AppenderSkeleton::checkEntryConditions().
     */
    bool checkEntryConditions() const override;

    bool handleIoErrors() const;
    void flushBuffer();

    /*!
     * Opens the file for writing. Creates parent directories if needed and
     * expands Windows environment variables in the file path.
     *
     * Declared virtual to allow rolling subclasses to override.
     */
    virtual void openFile();
    void closeFile();

    bool removeFile(QFile &file) const;
    bool renameFile(QFile &file, const QString &fileName) const;

private:
    void closeInternal();

    std::atomic<bool> mAppendFile;
    std::atomic<int>  mBufferSize;
    std::atomic<bool> mImmediateFlush;
    QString           mFileName;      // guarded by mObjectGuard
    QByteArray        mByteBuffer;    // guarded by mObjectGuard
    std::unique_ptr<QFile> mFile;     // guarded by mObjectGuard
};

inline bool RandomAccessFileAppender::appendFile() const
{
    return mAppendFile.load(std::memory_order_relaxed);
}

inline int RandomAccessFileAppender::bufferSize() const
{
    return mBufferSize.load(std::memory_order_relaxed);
}

inline bool RandomAccessFileAppender::immediateFlush() const
{
    return mImmediateFlush.load(std::memory_order_relaxed);
}

inline void RandomAccessFileAppender::setAppendFile(bool append)
{
    mAppendFile.store(append, std::memory_order_relaxed);
}

inline void RandomAccessFileAppender::setImmediateFlush(bool immediateFlush)
{
    mImmediateFlush.store(immediateFlush, std::memory_order_relaxed);
}

} // namespace Log4Qt

#endif // LOG4QT_RANDOMACCESSFILEAPPENDER_H
