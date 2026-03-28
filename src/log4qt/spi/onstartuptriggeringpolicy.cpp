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

#include "spi/onstartuptriggeringpolicy.h"

#include <QFileInfo>

namespace Log4Qt
{

OnStartupTriggeringPolicy::OnStartupTriggeringPolicy(QObject *parent) :
    TriggeringPolicy(parent)
{
}

bool OnStartupTriggeringPolicy::isTriggeringEvent(const QString &fileName,
                                                    qint64 fileSize,
                                                    const LoggingEvent &event)
{
    Q_UNUSED(fileName)
    Q_UNUSED(fileSize)
    Q_UNUSED(event)
    return false;
}

bool OnStartupTriggeringPolicy::isStartupTrigger(const QString &fileName, qint64 fileSize)
{
    Q_UNUSED(fileSize)
    QFileInfo fi(fileName);
    return fi.exists() && fi.size() > 0;
}

} // namespace Log4Qt

#include "moc_onstartuptriggeringpolicy.cpp"
