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

#include "hierarchy.h"

#include "logger.h"
#include "helpers/optionconverter.h"

#include <algorithm>

namespace Log4Qt
{

LOG4QT_DECLARE_STATIC_LOGGER(static_logger, ::LoggerRepository)

Hierarchy::Hierarchy() 
    : mObjectGuard(QReadWriteLock::Recursive)
    , mThreshold(Level::NULL_INT)
    , mRootLogger(logger(QString()))
{}

Hierarchy::~Hierarchy()
{
    static_logger()->warn(u"Unexpected destruction of Hierarchy"_s);
}

bool Hierarchy::exists(const QString &name) const
{
    QReadLocker locker(&mObjectGuard);

    return mLoggers.contains(name);
}

Logger *Hierarchy::logger(const QString &name)
{
    QWriteLocker locker(&mObjectGuard);

    return createLogger(name);
}

QList<Logger *> Hierarchy::loggers() const
{
    QReadLocker locker(&mObjectGuard);

    return mLoggers.values();
}

void Hierarchy::setThreshold(QStringView threshold)
{
    setThreshold(Level::fromString(threshold));
}

void Hierarchy::resetConfiguration()
{
    QWriteLocker locker(&mObjectGuard);

    // Reset all loggers.
    // Leave log, qt and root logger to the last to allow debugging of shutdown.

    Logger *p_logging_logger = logger(u""_s);
    Logger *p_qt_logger = logger(u"Qt"_s);
    Logger *p_root_logger = rootLogger();

    // Define predicate for regular (non-special) loggers
    auto isRegularLogger = [=](Logger* logger) {
        return logger != p_logging_logger && 
               logger != p_qt_logger && 
               logger != p_root_logger;
    };

    // Reset all regular loggers
    auto loggers = mLoggers.values();
    std::for_each(loggers.begin(), loggers.end(), 
                  [&](Logger* logger) {
                      if (isRegularLogger(logger)) {
                          resetLogger(logger, Level::NULL_INT);
                      }
                  });
    
    // Reset special loggers
    resetLogger(p_qt_logger, Level::NULL_INT);
    resetLogger(p_logging_logger, Level::NULL_INT);
    resetLogger(p_root_logger, Level::DEBUG_INT);
}

void Hierarchy::shutdown()
{
    static_logger()->debug(u"Shutting down Hierarchy"_s);
    resetConfiguration();
}

Logger *Hierarchy::createLogger(const QString &orgName)
{
    static const auto name_separator = u"::"_s;

    QString name(OptionConverter::classNameJavaToCpp(orgName));

    Logger *logger = mLoggers.value(name, nullptr);
    if (logger != nullptr)
        return logger;

    if (name.isEmpty())
    {
        logger = new Logger(this, Level::DEBUG_INT, u"root"_s, nullptr);
        mLoggers.insert(QString(), logger);
        return logger;
    }
    QString parent_name;
    int index = name.lastIndexOf(name_separator);
    if (index >= 0)
        parent_name = name.left(index);

    logger = new Logger(this, Level::NULL_INT, name, createLogger(parent_name));
    mLoggers.insert(name, logger);
    return logger;
}

void Hierarchy::resetLogger(Logger *logger, Level level) const
{
    logger->removeAllAppenders();
    logger->setAdditivity(true);
    logger->setLevel(level);
}

} // namespace Log4Qt
