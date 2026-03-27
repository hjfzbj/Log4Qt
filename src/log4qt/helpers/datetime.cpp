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

DateTime::DateTime() = default;

DateTime::~DateTime() = default;

DateTime::DateTime(const DateTime &other) = default;

QString DateTime::toString(const QString &format) const
{
    if (format.isEmpty())
        return QString();
    if (!isValid())
        return QString();

    if (format == u"NONE"_s)
        return QString();
    if (format == u"RELATIVE"_s)
        return QString::number(toMSecsSinceEpoch() - InitialisationHelper::startTime());
    if (format == u"ISO8601"_s)
        return formatDateTime(u"yyyy-MM-dd hh:mm:ss.zzz"_s);
    if (format == u"ABSOLUTE"_s)
        return formatDateTime(u"HH:mm:ss.zzz"_s);
    if (format == u"DATE"_s)
        return formatDateTime(u"dd MM yyyy HH:mm:ss.zzz"_s);

    return formatDateTime(format);
}

QString DateTime::formatDateTime(const QString &format) const
{
    return QDateTime::toString(format);
}

} // namespace Log4Qt
