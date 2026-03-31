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

#include "spi/sizebasedtriggeringpolicy.h"

#include "helpers/optionconverter.h"

#include <QIODevice>

namespace Log4Qt
{

SizeBasedTriggeringPolicy::SizeBasedTriggeringPolicy(QObject *parent) :
    TriggeringPolicy(parent),
    mMaximumFileSize(10 * 1024 * 1024)
{
}

void SizeBasedTriggeringPolicy::setMaxFileSize(const QString &maxFileSize)
{
    bool ok;
    qint64 size = OptionConverter::toFileSize(maxFileSize, &ok);
    if (ok)
        setMaximumFileSize(size);
}

bool SizeBasedTriggeringPolicy::isTriggeringEvent(QIODevice *activeFile,
                                                   const LoggingEvent &event)
{
    Q_UNUSED(event)
    return activeFile && activeFile->size() > mMaximumFileSize;
}

} // namespace Log4Qt

#include "moc_sizebasedtriggeringpolicy.cpp"
