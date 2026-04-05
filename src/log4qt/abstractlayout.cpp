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

#include "log4qtdefs.h"
#include "abstractlayout.h"


namespace Log4Qt
{

AbstractLayout::AbstractLayout(QObject *parent) :
    QObject(parent)
{}

AbstractLayout::~AbstractLayout() = default;

QString AbstractLayout::contentType() const
{
    return u"text/plain"_s;
}

void AbstractLayout::activateOptions()
{
}

bool AbstractLayout::requiresLocation() const
{
    return false;
}

QString AbstractLayout::endOfLine()
{
    // There seams to be no function in Qt for this. MinGW enter '\r\n' automatically
    return u"\n"_s;
}

} // namespace Log4Qt

#include "moc_abstractlayout.cpp"
