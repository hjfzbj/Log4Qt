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

#include "timestampprovider.h"

#include <atomic>

namespace Log4Qt
{

// Thread-local cached timestamp
static thread_local qint64 s_cachedTimestamp = 0;

// Thread-local monotonic counter for cache expiry
static thread_local qint64 s_lastCounterValue = 0;

// Global cache window setting (in milliseconds) - atomic because it is
// written by setCacheWindow() and read concurrently from every logging thread.
static std::atomic<qint64> s_cacheWindowMs{1};

// Monotonic timer for cache validation
static QElapsedTimer s_elapsedTimer  = []() {
    QElapsedTimer timer;
    timer.start();
    return timer;
}();


qint64 TimestampProvider::currentMSecsSinceEpoch()
{
    const qint64 cacheWindow = s_cacheWindowMs.load(std::memory_order_relaxed);

    // If caching is disabled, always get fresh timestamp
    if (cacheWindow <= 0)
    {
        return QDateTime::currentMSecsSinceEpoch();
    }

    // Get monotonic counter value (very fast, no system call).
    // Guard against use before static initialization (e.g. from another
    // translation unit's static initializer).
    if (Q_UNLIKELY(!s_elapsedTimer.isValid()))
        s_elapsedTimer.start();
    qint64 currentCounter = s_elapsedTimer.elapsed();

    // Check if cached value is still valid
    qint64 elapsedSinceLastUpdate = currentCounter - s_lastCounterValue;

    if (s_cachedTimestamp == 0 || elapsedSinceLastUpdate < 0 || elapsedSinceLastUpdate >= cacheWindow)
    {
        // Cache expired or first call - update with system call
        s_cachedTimestamp = QDateTime::currentMSecsSinceEpoch();
        s_lastCounterValue = currentCounter;
    }
    
    return s_cachedTimestamp;
}

void TimestampProvider::setCacheWindow(qint64 cacheWindowMs)
{
    s_cacheWindowMs.store(cacheWindowMs, std::memory_order_relaxed);
}

qint64 TimestampProvider::cacheWindow()
{
    return s_cacheWindowMs.load(std::memory_order_relaxed);
}

} // namespace Log4Qt
