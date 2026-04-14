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

#include "spi/headerfooterprovider.h"

#include "helpers/patternformatter.h"
#include "loggingevent.h"

namespace Log4Qt
{

// ---------------------------------------------------------------------------
// HeaderFooterProvider
// ---------------------------------------------------------------------------

HeaderFooterProvider::HeaderFooterProvider(QObject *parent)
    : QObject(parent)
{}

HeaderFooterProvider::~HeaderFooterProvider() = default;

QString HeaderFooterProvider::header() const
{
    return {};
}

QString HeaderFooterProvider::footer() const
{
    return {};
}

void HeaderFooterProvider::activateOptions()
{}

// ---------------------------------------------------------------------------
// PatternHeaderFooterProvider
// ---------------------------------------------------------------------------

PatternHeaderFooterProvider::PatternHeaderFooterProvider(QObject *parent)
    : HeaderFooterProvider(parent)
{}

PatternHeaderFooterProvider::~PatternHeaderFooterProvider() = default;

void PatternHeaderFooterProvider::setHeaderPattern(const QString &pattern)
{
    mHeaderPattern = pattern;
    mHeaderFormatter = pattern.isEmpty()
                           ? nullptr
                           : std::make_unique<PatternFormatter>(pattern);
}

void PatternHeaderFooterProvider::setFooterPattern(const QString &pattern)
{
    mFooterPattern = pattern;
    mFooterFormatter = pattern.isEmpty()
                           ? nullptr
                           : std::make_unique<PatternFormatter>(pattern);
}

QString PatternHeaderFooterProvider::header() const
{
    if (mHeaderFormatter)
        return mHeaderFormatter->format(LoggingEvent{});
    return {};
}

QString PatternHeaderFooterProvider::footer() const
{
    if (mFooterFormatter)
        return mFooterFormatter->format(LoggingEvent{});
    return {};
}

} // namespace Log4Qt

#include "moc_headerfooterprovider.cpp"
