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

#include "spi/defaultrolloverstrategy.h"

#include <QFile>

namespace Log4Qt
{

DefaultRolloverStrategy::DefaultRolloverStrategy(QObject *parent) :
    RolloverStrategy(parent),
    mMinIndex(defaultMinIndex),
    mMaxIndex(defaultMaxIndex)
{
}

QString DefaultRolloverStrategy::rollover(const QString &fileName)
{
    // Delete the oldest backup file
    removeFile(fileName + QLatin1Char('.') + QString::number(mMaxIndex));

    // Shift numbered backups: .N -> .N+1
    for (int i = mMaxIndex - 1; i >= mMinIndex; i--)
    {
        const QString source = fileName + QLatin1Char('.') + QString::number(i);
        const QString target = fileName + QLatin1Char('.') + QString::number(i + 1);
        QFile f(source);
        if (f.exists())
            renameFile(source, target);
    }

    // Rename base file to .minIndex (may not exist on first startup)
    QFile base(fileName);
    if (base.exists())
        renameFile(fileName, fileName + QLatin1Char('.') + QString::number(mMinIndex));

    return fileName;
}

} // namespace Log4Qt

#include "moc_defaultrolloverstrategy.cpp"
