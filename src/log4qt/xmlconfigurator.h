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

#ifndef LOG4QT_XMLCONFIGURATOR_H
#define LOG4QT_XMLCONFIGURATOR_H

#include "log4qt.h"

class QXmlStreamReader;

namespace Log4Qt
{

class Properties;
class LoggerRepository;

/*!
 * \brief The class XmlConfigurator allows the configuration of the
 *        package from an XML file.
 *
 * The XML file mirrors the existing \c .properties format in a
 * structured XML tree. Nested elements produce dot-separated keys
 * and the \c class attribute on an element maps to the element's
 * flat property value (the class name). For example:
 *
 * \code{.xml}
 * <?xml version="1.0" encoding="UTF-8"?>
 * <log4j>
 *     <rootLogger>ALL, console</rootLogger>
 *     <appender>
 *         <console class="org.apache.log4j.ConsoleAppender">
 *             <target>STDOUT_TARGET</target>
 *             <layout class="org.apache.log4j.TTCCLayout">
 *                 <dateFormat>ISO8601</dateFormat>
 *             </layout>
 *         </console>
 *     </appender>
 *     <logger>
 *         <MyApp>ERROR, console</MyApp>
 *     </logger>
 *     <additivity>
 *         <MyApp>false</MyApp>
 *     </additivity>
 * </log4j>
 * \endcode
 *
 * Flattening rules:
 * - Nested elements produce dot-separated keys:
 *   \c <a><b>c</b></a> becomes \c a.b=c
 * - \c class attribute sets the element's key value:
 *   \c <console class="Foo"> becomes \c appender.console=Foo
 * - Variable substitution (\c ${varname}) works in text values
 *   and is resolved by OptionConverter::findAndSubst() after
 *   flattening
 *
 * The XML file is flattened into a Properties object and then
 * delegated to PropertyConfigurator for the actual configuration.
 * This reuses all existing parsing, factory, and type-conversion
 * logic with zero duplication.
 *
 * During automatic initialization the LogManager searches for
 * \c log4qt.xml after \c log4qt.json, so \c .properties and
 * \c .json files take priority for backward compatibility.
 * The \c Configuration setting also accepts \c .xml files.
 *
 * \note All the functions declared in this class are thread-safe.
 *
 * \sa PropertyConfigurator, JsonConfigurator, LogManager::startup()
 */
class LOG4QT_EXPORT XmlConfigurator
{
public:
    XmlConfigurator();

private:
    Q_DISABLE_COPY_MOVE(XmlConfigurator)

public:
    /*!
     * Reads the configuration from the XML file \a configFileName
     * and configures the \a loggerRepository.
     *
     * \sa ConfiguratorHelper::configureError()
     */
    bool doConfigure(const QString &configFileName,
                     LoggerRepository *loggerRepository = nullptr);

    /*!
     * Reads the configuration from the XML file \a configFilename
     * and configures the default LoggerRepository.
     *
     * \sa ConfiguratorHelper::configureError()
     */
    static bool configure(const QString &configFilename);

    /*!
     * Reads the configuration from the XML file \a configFilename,
     * configures the default LoggerRepository, and watches the file
     * for changes.
     *
     * \sa ConfiguratorHelper::configureError(),
     *     ConfiguratorHelper::configurationFile()
     */
    static bool configureAndWatch(const QString &configFilename);

private:
    static bool xmlToProperties(const QString &file, Properties &properties);
    static void flattenXmlElement(QXmlStreamReader &xml,
                                  const QString &prefix,
                                  Properties &properties);
};

inline XmlConfigurator::XmlConfigurator()
{}

} // namespace Log4Qt

#endif // LOG4QT_XMLCONFIGURATOR_H
