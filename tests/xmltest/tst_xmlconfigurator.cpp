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

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QtTest>

#include "log4qt/consoleappender.h"
#include "log4qt/dailyfileappender.h"
#include "log4qt/helpers/configuratorhelper.h"
#include "log4qt/helpers/properties.h"
#include "log4qt/xmlconfigurator.h"
#include "log4qt/logger.h"
#include "log4qt/logmanager.h"
#include "log4qt/ttcclayout.h"

using namespace Log4Qt;

class XmlConfiguratorTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void cleanup();
    void testFlattenSimpleKeys();
    void testFlattenNestedElements();
    void testFlattenClassAttribute();
    void testFlattenBooleanStrings();
    void testFlattenFullConfig();
    void testConfigureFromFile();
    void testRealWorldConfig();
    void testConfigureMissingFile();
    void testConfigureMalformedXml();

private:
    void writeXmlFile(const QString &path, const QByteArray &content);
};

void XmlConfiguratorTest::cleanup()
{
    LogManager::resetConfiguration();
}

void XmlConfiguratorTest::writeXmlFile(const QString &path, const QByteArray &content)
{
    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    file.write(content);
    file.close();
}

void XmlConfiguratorTest::testFlattenSimpleKeys()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.path() + "/test.xml";
    writeXmlFile(file, R"(<?xml version="1.0" encoding="UTF-8"?>
<log4j>
    <rootLogger>ALL</rootLogger>
</log4j>
)");

    QVERIFY(XmlConfigurator::configure(file));
    QCOMPARE(LogManager::rootLogger()->level(), Level::ALL_INT);
}

void XmlConfiguratorTest::testFlattenNestedElements()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.path() + "/test.xml";
    writeXmlFile(file, R"(<?xml version="1.0" encoding="UTF-8"?>
<log4j>
    <rootLogger>DEBUG, console</rootLogger>
    <appender>
        <console class="org.apache.log4j.ConsoleAppender">
            <target>STDOUT_TARGET</target>
            <layout class="org.apache.log4j.TTCCLayout">
                <dateFormat>ISO8601</dateFormat>
            </layout>
        </console>
    </appender>
</log4j>
)");

    QVERIFY(XmlConfigurator::configure(file));

    Logger *root = LogManager::rootLogger();
    QCOMPARE(root->level(), Level::DEBUG_INT);

    // Verify the appender was created
    auto appenders = root->appenders();
    bool foundConsole = false;
    for (const auto &appender : appenders)
    {
        if (appender->name() == u"console"_s)
        {
            foundConsole = true;
            QVERIFY(qobject_cast<ConsoleAppender *>(appender.data()));
            QVERIFY(appender->layout());
            QVERIFY(qobject_cast<TTCCLayout *>(appender->layout().data()));
        }
    }
    QVERIFY(foundConsole);
}

void XmlConfiguratorTest::testFlattenClassAttribute()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.path() + "/test.xml";
    writeXmlFile(file, R"(<?xml version="1.0" encoding="UTF-8"?>
<log4j>
    <rootLogger>WARN, myapp</rootLogger>
    <appender>
        <myapp class="org.apache.log4j.ConsoleAppender">
            <layout class="org.apache.log4j.SimpleLayout" />
        </myapp>
    </appender>
</log4j>
)");

    QVERIFY(XmlConfigurator::configure(file));

    Logger *root = LogManager::rootLogger();
    QCOMPARE(root->level(), Level::WARN_INT);
    auto appenders = root->appenders();
    QVERIFY(!appenders.isEmpty());
    QCOMPARE(appenders.first()->name(), u"myapp"_s);
}

void XmlConfiguratorTest::testFlattenBooleanStrings()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.path() + "/test.xml";
    writeXmlFile(file, R"(<?xml version="1.0" encoding="UTF-8"?>
<log4j>
    <rootLogger>DEBUG, console</rootLogger>
    <appender>
        <console class="org.apache.log4j.ConsoleAppender">
            <layout class="org.apache.log4j.TTCCLayout">
                <contextPrinting>false</contextPrinting>
            </layout>
        </console>
    </appender>
</log4j>
)");

    QVERIFY(XmlConfigurator::configure(file));
    Logger *root = LogManager::rootLogger();
    auto appenders = root->appenders();
    QVERIFY(!appenders.isEmpty());
    auto *ttcc = qobject_cast<TTCCLayout *>(appenders.first()->layout().data());
    QVERIFY(ttcc);
    QCOMPARE(ttcc->contextPrinting(), false);
}

void XmlConfiguratorTest::testFlattenFullConfig()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.path() + "/test.xml";
    writeXmlFile(file, R"(<?xml version="1.0" encoding="UTF-8"?>
<log4j>
    <rootLogger>ALL, console</rootLogger>
    <appender>
        <console class="org.apache.log4j.ConsoleAppender">
            <target>STDOUT_TARGET</target>
            <layout class="org.apache.log4j.TTCCLayout">
                <dateFormat>ISO8601</dateFormat>
            </layout>
        </console>
    </appender>
    <logger>
        <MyApp>ERROR, console</MyApp>
    </logger>
    <additivity>
        <MyApp>false</MyApp>
    </additivity>
</log4j>
)");

    QVERIFY(XmlConfigurator::configure(file));

    Logger *root = LogManager::rootLogger();
    QCOMPARE(root->level(), Level::ALL_INT);

    Logger *myApp = LogManager::logger(u"MyApp"_s);
    QCOMPARE(myApp->level(), Level::ERROR_INT);
    QCOMPARE(myApp->additivity(), false);
}

void XmlConfiguratorTest::testConfigureFromFile()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.path() + "/test.xml";
    writeXmlFile(file, R"(<?xml version="1.0" encoding="UTF-8"?>
<log4j>
    <rootLogger>TRACE, a1</rootLogger>
    <appender>
        <a1 class="org.apache.log4j.ConsoleAppender">
            <layout class="org.apache.log4j.SimpleLayout" />
        </a1>
    </appender>
</log4j>
)");

    XmlConfigurator configurator;
    QVERIFY(configurator.doConfigure(file));
    QCOMPARE(LogManager::rootLogger()->level(), Level::TRACE_INT);
}

void XmlConfiguratorTest::testRealWorldConfig()
{
    // Based on idlmapp.exe.log4qt.properties:
    //   logpath=../logging
    //   log4j.rootLogger=ALL, console, daily
    //   console = ConsoleAppender with TTCCLayout (custom dateFormat, contextPrinting, threshold OFF)
    //   daily   = DailyFileAppender with ${logpath} substitution and TTCCLayout
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString logDir = dir.path() + "/logging";
    QVERIFY(QDir().mkpath(logDir));

    const QString file = dir.path() + "/test.xml";
    const QByteArray xml = QString(R"(<?xml version="1.0" encoding="UTF-8"?>
<log4j>
    <logpath>%1</logpath>
    <reset>true</reset>
    <threshold>NULL</threshold>
    <handleQtMessages>true</handleQtMessages>
    <rootLogger>ALL, console, daily</rootLogger>
    <appender>
        <console class="org.apache.log4j.ConsoleAppender">
            <target>STDOUT_TARGET</target>
            <layout class="org.apache.log4j.TTCCLayout">
                <dateFormat>dd.MM.yyyy hh:mm:ss.zzz</dateFormat>
                <contextPrinting>true</contextPrinting>
            </layout>
            <threshold>OFF</threshold>
        </console>
        <daily class="org.apache.log4j.DailyFileAppender">
            <file>${log4j.logpath}/idlmapp.log</file>
            <appendFile>true</appendFile>
            <datePattern>_yyyy_MM_dd</datePattern>
            <layout class="org.apache.log4j.TTCCLayout">
                <dateFormat>dd.MM.yyyy hh:mm:ss.zzz</dateFormat>
                <contextPrinting>true</contextPrinting>
            </layout>
        </daily>
    </appender>
</log4j>
)").arg(logDir).toUtf8();

    writeXmlFile(file, xml);
    QVERIFY(XmlConfigurator::configure(file));

    // Root logger
    Logger *root = LogManager::rootLogger();
    QCOMPARE(root->level(), Level::ALL_INT);

    // Verify both appenders exist
    auto appenders = root->appenders();
    ConsoleAppender *consoleApp = nullptr;
    DailyFileAppender *dailyApp = nullptr;
    for (const auto &a : appenders)
    {
        if (a->name() == u"console"_s)
            consoleApp = qobject_cast<ConsoleAppender *>(a.data());
        else if (a->name() == u"daily"_s)
            dailyApp = qobject_cast<DailyFileAppender *>(a.data());
    }

    // Console appender
    QVERIFY(consoleApp);
    QCOMPARE(consoleApp->target(), u"STDOUT_TARGET"_s);
    QCOMPARE(consoleApp->threshold(), Level::OFF_INT);
    auto *consoleTtcc = qobject_cast<TTCCLayout *>(consoleApp->layout().data());
    QVERIFY(consoleTtcc);
    QCOMPARE(consoleTtcc->dateFormat(), u"dd.MM.yyyy hh:mm:ss.zzz"_s);
    QCOMPARE(consoleTtcc->contextPrinting(), true);

    // Daily file appender (file() returns the dated filename, e.g. idlmapp_2026_02_24.log)
    QVERIFY(dailyApp);
    QVERIFY(dailyApp->file().contains(u"idlmapp"_s));
    QCOMPARE(dailyApp->appendFile(), true);
    QCOMPARE(dailyApp->datePattern(), u"_yyyy_MM_dd"_s);
    auto *dailyTtcc = qobject_cast<TTCCLayout *>(dailyApp->layout().data());
    QVERIFY(dailyTtcc);
    QCOMPARE(dailyTtcc->dateFormat(), u"dd.MM.yyyy hh:mm:ss.zzz"_s);
    QCOMPARE(dailyTtcc->contextPrinting(), true);
}

void XmlConfiguratorTest::testConfigureMissingFile()
{
    QVERIFY(!XmlConfigurator::configure(u"/nonexistent/path/to/file.xml"_s));
}

void XmlConfiguratorTest::testConfigureMalformedXml()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.path() + "/bad.xml";
    writeXmlFile(file, "<not><valid><xml");

    QVERIFY(!XmlConfigurator::configure(file));
}

QTEST_GUILESS_MAIN(XmlConfiguratorTest)
#include "tst_xmlconfigurator.moc"
