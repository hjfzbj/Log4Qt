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

#include "loggingevent.h"

#include "helpers/datetime.h"
#include "helpers/initialisationhelper.h"
#include "helpers/timestampprovider.h"
#include "logger.h"
#include "mdc.h"
#include "ndc.h"

#include <QBuffer>
#include <QByteArray>
#include <QDataStream>
#include <QMutex>
#include <QPointer>
#include <QProperty>
#include <QThread>

namespace Log4Qt
{

LoggingEvent::LoggingEvent() :
    QEvent(eventId),
    mLevel(Level::NULL_INT),
    mLogger(nullptr),
    mMessage(),
    mNdc(NDC::peek()),
    mProperties(MDC::context()),
    mSequenceNumber(nextSequenceNumber()),
    mThreadName(),
    mTimeStamp(TimestampProvider::currentMSecsSinceEpoch())
{
    setThreadNameToCurrent();
}

LoggingEvent::~LoggingEvent() = default;

LoggingEvent::LoggingEvent(const Logger *logger,
                           Level level,
                           const QString &message) :
    QEvent(eventId),
    mLevel(level),
    mLogger(logger),
    mMessage(message),
    mNdc(NDC::peek()),
    mProperties(MDC::context()),
    mSequenceNumber(nextSequenceNumber()),
    mThreadName(),
    mTimeStamp(TimestampProvider::currentMSecsSinceEpoch())
{
    setThreadNameToCurrent();
}

LoggingEvent::LoggingEvent(const Logger *logger,
                           Level level,
                           const QString &message,
                           const MessageContext &context,
                           const QString &categoryName) :
       QEvent(eventId),
       mLevel(level),
       mLogger(logger),
       mMessage(message),
       mNdc(NDC::peek()),
       mProperties(MDC::context()),
       mSequenceNumber(nextSequenceNumber()),
       mThreadName(),
       mTimeStamp(TimestampProvider::currentMSecsSinceEpoch()),
       mContext(context),
       mCategoryName(categoryName)
{
    setThreadNameToCurrent();
}


LoggingEvent::LoggingEvent(const Logger *logger,
                           Level level,
                           const QString &message,
                           qint64 timeStamp) :
    QEvent(eventId),
    mLevel(level),
    mLogger(logger),
    mMessage(message),
    mNdc(NDC::peek()),
    mProperties(MDC::context()),
    mSequenceNumber(nextSequenceNumber()),
    mThreadName(),
    mTimeStamp(timeStamp)
{
    setThreadNameToCurrent();
}


LoggingEvent::LoggingEvent(const Logger *logger,
                           Level level,
                           const QString &message,
                           const QString &ndc,
                           const QHash<QString, QString> &properties,
                           const QString &threadName,
                           qint64 timeStamp) :
    QEvent(eventId),
    mLevel(level),
    mLogger(logger),
    mMessage(message),
    mNdc(ndc),
    mProperties(properties),
    mSequenceNumber(nextSequenceNumber()),
    mThreadName(threadName),
    mTimeStamp(timeStamp)
{
}

LoggingEvent::LoggingEvent(const Logger *logger,
                           Level level,
                           const QString &message,
                           const QString &ndc,
                           const QHash<QString, QString> &properties,
                           qint64 timeStamp,
                           const MessageContext &context,
                           const QString &categoryName)
    :
       QEvent(eventId),
       mLevel(level),
       mLogger(logger),
       mMessage(message),
       mNdc(ndc),
       mProperties(properties),
       mSequenceNumber(nextSequenceNumber()),
       mThreadName(),
       mTimeStamp(timeStamp),
       mContext(context),
       mCategoryName(categoryName)
{
    setThreadNameToCurrent();
}

LoggingEvent::LoggingEvent(const Logger *logger,
                           Level level,
                           const QString &message,
                           const QString &ndc,
                           const QHash<QString, QString> &properties,
                           const QString &threadName,
                           qint64 timeStamp,
                           const MessageContext &context,
                           const QString &categoryName)
    :
       QEvent(eventId),
       mLevel(level),
       mLogger(logger),
       mMessage(message),
       mNdc(ndc),
       mProperties(properties),
       mSequenceNumber(nextSequenceNumber()),
       mThreadName(threadName),
       mTimeStamp(timeStamp),
       mContext(context),
       mCategoryName(categoryName)
{
}

// Move-enabled constructors for zero-copy construction
LoggingEvent::LoggingEvent(const Logger *logger,
                           Level level,
                           QString &&message) :
    QEvent(eventId),
    mLevel(level),
    mLogger(logger),
    mMessage(std::move(message)),
    mNdc(NDC::peek()),
    mProperties(MDC::context()),
    mSequenceNumber(nextSequenceNumber()),
    mThreadName(),
    mTimeStamp(TimestampProvider::currentMSecsSinceEpoch())
{
    setThreadNameToCurrent();
}

LoggingEvent::LoggingEvent(const Logger *logger,
                           Level level,
                           QString &&message,
                           const MessageContext &context,
                           QString &&categoryName) :
    QEvent(eventId),
    mLevel(level),
    mLogger(logger),
    mMessage(std::move(message)),
    mNdc(NDC::peek()),
    mProperties(MDC::context()),
    mSequenceNumber(nextSequenceNumber()),
    mThreadName(),
    mTimeStamp(TimestampProvider::currentMSecsSinceEpoch()),
    mContext(context),
    mCategoryName(std::move(categoryName))
{
    setThreadNameToCurrent();
}

QString LoggingEvent::loggename() const
{
    if (mLogger)
        return mLogger->name();
    return QString();
}

QString LoggingEvent::toString() const
{
    return level().toString() % QLatin1Char(':') % message();
}

qint64 LoggingEvent::sequenceCount()
{
    return msSequenceCount;
}

qint64 LoggingEvent::startTime()
{
    return InitialisationHelper::startTime();
}

void LoggingEvent::setThreadNameToCurrent()
{
    static thread_local QString cachedName;

    // Flag set by QBindable notifier when objectName changes.
    // The cached name is re-read lazily on the next call.
    static thread_local bool nameChanged = true;
    static thread_local QPropertyNotifier notifier = []() -> QPropertyNotifier {
        QThread *thread = QThread::currentThread();
        if (!thread)
            return {};
        return thread->bindableObjectName().addNotifier([wp = QPointer<QThread>(thread)]() {
            if (wp && QThread::currentThread() == wp.data())
                nameChanged = true;
        });
    }();

    if (nameChanged)
    {
        if (const QThread *thread = QThread::currentThread())
            cachedName = thread->objectName();
        nameChanged = false;
    }

    if (cachedName.isEmpty())
    {
        static thread_local const QString cachedPtrName =
            u"0x%1"_s.arg(reinterpret_cast<quintptr>(
                QThread::currentThread()), QT_POINTER_SIZE * 2, 16, QChar('0'));
        mThreadName = cachedPtrName;
    }
    else
        mThreadName = cachedName;
}

qint64 LoggingEvent::nextSequenceNumber()
{
    return ++msSequenceCount;
}

MessageContext LoggingEvent::context() const
{
    return mContext;
}

void LoggingEvent::setContext(const MessageContext &context)
{
    mContext = context;
}

QString LoggingEvent::categoryName() const
{
    return mCategoryName;
}

void LoggingEvent::setCategoryName(const QString &categoryName)
{
    mCategoryName = categoryName;
}

std::atomic<qint64> LoggingEvent::msSequenceCount {0};
const QEvent::Type LoggingEvent::eventId = static_cast<QEvent::Type>(QEvent::registerEventType());

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &out, const LoggingEvent &loggingEvent)
{
    // version
    quint16 version = 0;
    out << version;
    // version 0 data
    out << loggingEvent.mLevel
           << loggingEvent.loggename()
           << loggingEvent.mMessage
           << loggingEvent.mNdc
           << loggingEvent.mProperties
           << loggingEvent.mSequenceNumber
           << loggingEvent.mThreadName
           << loggingEvent.mTimeStamp;

    return out;
}


QDataStream &operator>>(QDataStream &in, LoggingEvent &loggingEvent)
{
    // version
    quint16 version;
    in >> version;
    // Version 0 data
    QString logger;
    in >> loggingEvent.mLevel
       >> logger
       >> loggingEvent.mMessage
       >> loggingEvent.mNdc
       >> loggingEvent.mProperties
       >> loggingEvent.mSequenceNumber
       >> loggingEvent.mThreadName
       >> loggingEvent.mTimeStamp;
    if (logger.isEmpty())
        loggingEvent.mLogger = nullptr;
    else
        loggingEvent.mLogger = Logger::logger(logger);

    return in;
}
#endif // QT_NO_DATASTREAM

} // namespace Log4Qt
