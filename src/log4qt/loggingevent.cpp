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

LoggingEvent::Data::Data() :
    mLevel(Level::NULL_INT),
    mLogger(nullptr),
    mMessage(),
    mNdc(NDC::peek()),
    mProperties(MDC::context()),
    mSequenceNumber(nextSequenceNumber()),
    mThreadName(),
    mTimeStamp(TimestampProvider::currentMSecsSinceEpoch())
{
}

LoggingEvent::Data::Data(const Logger *logger,
                         Level level,
                         QString message) :
    mLevel(level),
    mLogger(logger),
    mMessage(std::move(message)),
    mNdc(NDC::peek()),
    mProperties(MDC::context()),
    mSequenceNumber(nextSequenceNumber()),
    mThreadName(),
    mTimeStamp(TimestampProvider::currentMSecsSinceEpoch())
{
}

LoggingEvent::Data::Data(const Logger *logger,
                         Level level,
                         QString message,
                         const MessageContext &context,
                         QString categoryName) :
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
}

LoggingEvent::Data::Data(const Logger *logger,
                         Level level,
                         QString message,
                         const QString &ndc,
                         const QHash<QString, QString> &properties,
                         const QString &threadName,
                         qint64 timeStamp,
                         const MessageContext &context,
                         const QString &categoryName) :
    mLevel(level),
    mLogger(logger),
    mMessage(std::move(message)),
    mNdc(ndc),
    mProperties(properties),
    mSequenceNumber(nextSequenceNumber()),
    mThreadName(threadName),
    mTimeStamp(timeStamp),
    mContext(context),
    mCategoryName(categoryName)
{
}

LoggingEvent::LoggingEvent() :
    QEvent(eventId),
    d(new Data)
{
    setThreadNameToCurrent();
}

LoggingEvent::~LoggingEvent() = default;

LoggingEvent::LoggingEvent(const LoggingEvent &other) :
    QEvent(other),
    d(other.d)
{
}

LoggingEvent &LoggingEvent::operator=(const LoggingEvent &other)
{
    if (this != &other)
    {
        QEvent::operator=(other);
        d = other.d;
    }
    return *this;
}

LoggingEvent::LoggingEvent(const Logger *logger,
                           Level level,
                           const QString &message) :
    QEvent(eventId),
    d(new Data(logger, level, message))
{
    setThreadNameToCurrent();
}

LoggingEvent::LoggingEvent(const Logger *logger,
                           Level level,
                           const QString &message,
                           const MessageContext &context,
                           const QString &categoryName) :
    QEvent(eventId),
    d(new Data(logger, level, message, context, categoryName))
{
    setThreadNameToCurrent();
}


LoggingEvent::LoggingEvent(const Logger *logger,
                           Level level,
                           const QString &message,
                           qint64 timeStamp) :
    QEvent(eventId),
    d(new Data(logger, level, message))
{
    d->mTimeStamp = timeStamp;
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
    d(new Data(logger, level, message, ndc, properties, threadName, timeStamp, MessageContext(), QString()))
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
       d(new Data(logger, level, message, ndc, properties, QString(), timeStamp, context, categoryName))
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
       d(new Data(logger, level, message, ndc, properties, threadName, timeStamp, context, categoryName))
{
}

// Move-enabled constructors for zero-copy construction
LoggingEvent::LoggingEvent(const Logger *logger,
                           Level level,
                           QString &&message) :
    QEvent(eventId),
    d(new Data(logger, level, std::move(message)))
{
    setThreadNameToCurrent();
}

LoggingEvent::LoggingEvent(const Logger *logger,
                           Level level,
                           QString &&message,
                           const MessageContext &context,
                           QString &&categoryName) :
    QEvent(eventId),
    d(new Data(logger, level, std::move(message), context, std::move(categoryName)))
{
    setThreadNameToCurrent();
}

QString LoggingEvent::loggename() const
{
    if (d->mLogger)
        return d->mLogger->name();
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
    static thread_local QString cachedPtrName;

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
        {
            cachedName = thread->objectName();
            cachedPtrName = u"0x%1"_s.arg(reinterpret_cast<quintptr>(thread),
                                           QT_POINTER_SIZE * 2, 16, QChar('0'));
        }
        nameChanged = false;
    }

    d->mThreadName = cachedName.isEmpty() ? cachedPtrName : cachedName;
}

qint64 LoggingEvent::nextSequenceNumber()
{
    return ++msSequenceCount;
}

Level LoggingEvent::level() const
{
    return d->mLevel;
}

const Logger *LoggingEvent::logger() const
{
    return d->mLogger;
}

QString LoggingEvent::message() const
{
    return d->mMessage;
}

QHash<QString, QString> LoggingEvent::mdc() const
{
    return d->mProperties;
}

QString LoggingEvent::ndc() const
{
    return d->mNdc;
}

QHash<QString, QString> LoggingEvent::properties() const
{
    return d->mProperties;
}

qint64 LoggingEvent::sequenceNumber() const
{
    return d->mSequenceNumber;
}

QString LoggingEvent::threadName() const
{
    return d->mThreadName;
}

qint64 LoggingEvent::timeStamp() const
{
    return d->mTimeStamp;
}

QString LoggingEvent::property(const QString &key) const
{
    return d->mProperties.value(key);
}

QStringList LoggingEvent::propertyKeys() const
{
    return QStringList(d->mProperties.keys());
}

void LoggingEvent::setProperty(const QString &key, const QString &value)
{
    d->mProperties.insert(key, value);
}

MessageContext LoggingEvent::context() const
{
    return d->mContext;
}

void LoggingEvent::setContext(const MessageContext &context)
{
    d->mContext = context;
}

QString LoggingEvent::categoryName() const
{
    return d->mCategoryName;
}

void LoggingEvent::setCategoryName(const QString &categoryName)
{
    d->mCategoryName = categoryName;
}

int LoggingEvent::lineNumber() const
{
    return d->mContext.line;
}

void LoggingEvent::setLineNumber(int lineNumber)
{
    d->mContext.line = lineNumber;
}

QString LoggingEvent::fileName() const
{
    return QString::fromUtf8(d->mContext.file);
}

void LoggingEvent::setFileName(const QString &fileName)
{
    Q_UNUSED(fileName)
    // MessageContext stores const char*, so we can't easily set it from QString
    // without ownership management. Existing MessageContext seems to assume
    // static strings for file/function.
}

QString LoggingEvent::functionName() const
{
    return QString::fromUtf8(d->mContext.function);
}

void LoggingEvent::setMethodName(const QString &functionName)
{
    Q_UNUSED(functionName)
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
    out << loggingEvent.d->mLevel
           << loggingEvent.loggename()
           << loggingEvent.d->mMessage
           << loggingEvent.d->mNdc
           << loggingEvent.d->mProperties
           << loggingEvent.d->mSequenceNumber
           << loggingEvent.d->mThreadName
           << loggingEvent.d->mTimeStamp;

    return out;
}


QDataStream &operator>>(QDataStream &in, LoggingEvent &loggingEvent)
{
    // version
    quint16 version;
    in >> version;
    // Version 0 data
    QString logger;
    in >> loggingEvent.d->mLevel
       >> logger
       >> loggingEvent.d->mMessage
       >> loggingEvent.d->mNdc
       >> loggingEvent.d->mProperties
       >> loggingEvent.d->mSequenceNumber
       >> loggingEvent.d->mThreadName
       >> loggingEvent.d->mTimeStamp;
    if (logger.isEmpty())
        loggingEvent.d->mLogger = nullptr;
    else
        loggingEvent.d->mLogger = Logger::logger(logger);

    return in;
}
#endif // QT_NO_DATASTREAM

} // namespace Log4Qt
