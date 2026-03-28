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

#include "spi/compositetriggeringpolicy.h"

namespace Log4Qt
{

CompositeTriggeringPolicy::CompositeTriggeringPolicy(QObject *parent) :
    TriggeringPolicy(parent)
{
}

void CompositeTriggeringPolicy::addPolicy(const TriggeringPolicySharedPtr &policy)
{
    mPolicies.append(policy);
}

QList<TriggeringPolicySharedPtr> CompositeTriggeringPolicy::policies() const
{
    return mPolicies;
}

void CompositeTriggeringPolicy::activateOptions()
{
    for (const auto &policy : mPolicies)
        policy->activateOptions();
}

bool CompositeTriggeringPolicy::isTriggeringEvent(const QString &fileName,
                                                    qint64 fileSize,
                                                    const LoggingEvent &event)
{
    for (const auto &policy : mPolicies)
    {
        if (policy->isTriggeringEvent(fileName, fileSize, event))
            return true;
    }
    return false;
}

bool CompositeTriggeringPolicy::isStartupTrigger(const QString &fileName, qint64 fileSize)
{
    for (const auto &policy : mPolicies)
    {
        if (policy->isStartupTrigger(fileName, fileSize))
            return true;
    }
    return false;
}

} // namespace Log4Qt

#include "moc_compositetriggeringpolicy.cpp"
