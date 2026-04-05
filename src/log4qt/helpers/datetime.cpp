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

namespace Log4Qt
{

namespace
{

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

// Static fast path: formats epoch ms without constructing a QDateTime for the
// named formats. Named formats are cached thread-locally (keyed by epoch ms)
// so that repeated calls within the same millisecond cost only a qint64
// comparison and a string copy (~1 ns). On a cache miss, formatting is
// delegated to QDateTime::toString() as usual.
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
            cache.str = QDateTime::fromMSecsSinceEpoch(msecs).toString(u"yyyy-MM-dd hh:mm:ss.zzz"_s);
        }
        return cache.str;
    }

    if (format == u"ABSOLUTE"_s)
    {
        thread_local TimestampCache cache;
        if (cache.epochMs != msecs)
        {
            cache.epochMs = msecs;
            cache.str = QDateTime::fromMSecsSinceEpoch(msecs).toString(u"HH:mm:ss.zzz"_s);
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
