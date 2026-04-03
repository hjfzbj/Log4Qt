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

#include "helpers/datetime.h"

#include "helpers/initialisationhelper.h"

#include <ctime>

namespace Log4Qt
{

namespace
{

// Decomposes epoch milliseconds into a local-time struct tm and a sub-second ms
// component. Handles negative epoch values (pre-1970 timestamps) correctly.
// C++ truncates towards zero, so -1500 % 1000 == -500; adding 1000 gives the
// correct sub-second part (+500 ms) and adjusts secs to -2 (not -1).
inline void epochMsToLocalTm(qint64 msecs, struct tm &tm, int &ms)
{
    ms = static_cast<int>(msecs % 1000);
    if (ms < 0) ms += 1000;
    time_t secs = static_cast<time_t>((msecs - ms) / 1000);
#ifdef Q_OS_WIN
    localtime_s(&tm, &secs);
#else
    localtime_r(&secs, &tm);
#endif
}

// Manual ISO8601 formatting: "yyyy-MM-dd hh:mm:ss.zzz"
//
// ~500 ns vs ~15 µs for QDateTime::toString(): avoids QDateTime construction
// (timezone table lookup), format-string parsing, locale-aware formatting, and
// intermediate UTF-16 allocations.
QString manualISO8601(qint64 msecs)
{
    struct tm tm{};
    int ms{};
    epochMsToLocalTm(msecs, tm, ms);
    char buf[24]; // "yyyy-MM-dd hh:mm:ss.zzz\0"
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d.%03d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
             tm.tm_hour, tm.tm_min, tm.tm_sec, ms);
    return QString::fromLatin1(buf, 23);
}

// Manual ABSOLUTE formatting: "HH:mm:ss.zzz"
QString manualABSOLUTE(qint64 msecs)
{
    struct tm tm{};
    int ms{};
    epochMsToLocalTm(msecs, tm, ms);
    char buf[13]; // "HH:mm:ss.zzz\0"
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%03d",
             tm.tm_hour, tm.tm_min, tm.tm_sec, ms);
    return QString::fromLatin1(buf, 12);
}

// Thread-local cache for a single formatted timestamp.
// epochMs == -1 means the cache is empty.
struct TimestampCache
{
    qint64 epochMs = -1;
    QString str;
};

} // anonymous namespace

DateTime::DateTime() = default;

DateTime::~DateTime() = default;

DateTime::DateTime(const DateTime &other) = default;

// Static fast path: format epoch ms without constructing a QDateTime object for
// the named formats. Uses thread-local caches for ISO8601 and ABSOLUTE so that
// repeated calls within the same millisecond cost ~1 ns (qint64 compare + copy).
QString DateTime::formatMsecs(qint64 msecs, const QString &format)
{
    if (format.isEmpty())
        return QString();

    if (format == u"NONE"_s)
        return QString();
    if (format == u"RELATIVE"_s)
        return QString::number(msecs - InitialisationHelper::startTime());

    if (format == u"ISO8601"_s)
    {
        thread_local TimestampCache cache;
        if (cache.epochMs != msecs)
        {
            cache.epochMs = msecs;
            cache.str = manualISO8601(msecs);
        }
        return cache.str;
    }

    if (format == u"ABSOLUTE"_s)
    {
        thread_local TimestampCache cache;
        if (cache.epochMs != msecs)
        {
            cache.epochMs = msecs;
            cache.str = manualABSOLUTE(msecs);
        }
        return cache.str;
    }

    if (format == u"DATE"_s)
    {
        thread_local TimestampCache cache;
        if (cache.epochMs != msecs)
        {
            cache.epochMs = msecs;
            cache.str = QDateTime::fromMSecsSinceEpoch(msecs).toString(u"dd MM yyyy HH:mm:ss.zzz"_s);
        }
        return cache.str;
    }

    // Custom format string: fall back to full QDateTime construction.
    return QDateTime::fromMSecsSinceEpoch(msecs).toString(format);
}

QString DateTime::toString(const QString &format) const
{
    if (!isValid())
        return QString();
    return formatMsecs(toMSecsSinceEpoch(), format);
}

QString DateTime::formatDateTime(const QString &format) const
{
    return QDateTime::toString(format);
}

} // namespace Log4Qt
