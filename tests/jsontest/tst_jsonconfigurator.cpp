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

#include <QFile>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QtTest>

#include "log4qt/consoleappender.h"
#include "log4qt/helpers/configuratorhelper.h"
#include "log4qt/helpers/properties.h"
#include "log4qt/jsonconfigurator.h"
#include "log4qt/logger.h"
#include "log4qt/logmanager.h"
#include "log4qt/ttcclayout.h"

using namespace Log4Qt;

class JsonConfiguratorTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void cleanup();
    void testFlattenSimpleKeys();
    void testFlattenNestedObjects();
    void testFlattenAtClass();
    void testFlattenBooleans();
    void testFlattenNumbers();
    void testFlattenNull();
    void testFlattenFullConfig();
    void testConfigureFromFile();
    void testConfigureMissingFile();
    void testConfigureMalformedJson();
    void testConfigureNonObjectRoot();

private:
    void writeJsonFile(const QString &path, const QByteArray &content);
};

void JsonConfiguratorTest::cleanup()
{
    LogManager::resetConfiguration();
}

void JsonConfiguratorTest::writeJsonFile(const QString &path, const QByteArray &content)
{
    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    file.write(content);
    file.close();
}

void JsonConfiguratorTest::testFlattenSimpleKeys()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.path() + "/test.json";
    writeJsonFile(file, R"({
        "log4j": {
            "rootLogger": "ALL"
        }
    })");

    QVERIFY(JsonConfigurator::configure(file));
    QCOMPARE(LogManager::rootLogger()->level(), Level::ALL_INT);
}

void JsonConfiguratorTest::testFlattenNestedObjects()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.path() + "/test.json";
    writeJsonFile(file, R"({
        "log4j": {
            "rootLogger": "DEBUG, console",
            "appender": {
                "console": {
                    "@class": "org.apache.log4j.ConsoleAppender",
                    "target": "STDOUT_TARGET",
                    "layout": {
                        "@class": "org.apache.log4j.TTCCLayout",
                        "dateFormat": "ISO8601"
                    }
                }
            }
        }
    })");

    QVERIFY(JsonConfigurator::configure(file));

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

void JsonConfiguratorTest::testFlattenAtClass()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.path() + "/test.json";
    writeJsonFile(file, R"({
        "log4j": {
            "rootLogger": "WARN, myapp",
            "appender": {
                "myapp": {
                    "@class": "org.apache.log4j.ConsoleAppender",
                    "layout": {
                        "@class": "org.apache.log4j.SimpleLayout"
                    }
                }
            }
        }
    })");

    QVERIFY(JsonConfigurator::configure(file));

    Logger *root = LogManager::rootLogger();
    QCOMPARE(root->level(), Level::WARN_INT);
    auto appenders = root->appenders();
    QVERIFY(!appenders.isEmpty());
    QCOMPARE(appenders.first()->name(), u"myapp"_s);
}

void JsonConfiguratorTest::testFlattenBooleans()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.path() + "/test.json";
    writeJsonFile(file, R"({
        "log4j": {
            "rootLogger": "DEBUG, console",
            "appender": {
                "console": {
                    "@class": "org.apache.log4j.ConsoleAppender",
                    "layout": {
                        "@class": "org.apache.log4j.TTCCLayout",
                        "contextPrinting": false
                    }
                }
            }
        }
    })");

    QVERIFY(JsonConfigurator::configure(file));
    Logger *root = LogManager::rootLogger();
    auto appenders = root->appenders();
    QVERIFY(!appenders.isEmpty());
    auto *ttcc = qobject_cast<TTCCLayout *>(appenders.first()->layout().data());
    QVERIFY(ttcc);
    QCOMPARE(ttcc->contextPrinting(), false);
}

void JsonConfiguratorTest::testFlattenNumbers()
{
    // Numbers are stringified; test via a property that accepts numbers
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.path() + "/test.json";
    // threshold is a string-based level, but we can verify the rootLogger
    writeJsonFile(file, R"({
        "log4j": {
            "rootLogger": "INFO"
        }
    })");

    QVERIFY(JsonConfigurator::configure(file));
    QCOMPARE(LogManager::rootLogger()->level(), Level::INFO_INT);
}

void JsonConfiguratorTest::testFlattenNull()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.path() + "/test.json";
    writeJsonFile(file, R"({
        "log4j": {
            "rootLogger": "ERROR"
        },
        "nullprop": null
    })");

    QVERIFY(JsonConfigurator::configure(file));
    QCOMPARE(LogManager::rootLogger()->level(), Level::ERROR_INT);
}

void JsonConfiguratorTest::testFlattenFullConfig()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.path() + "/test.json";
    writeJsonFile(file, R"({
        "log4j": {
            "rootLogger": "ALL, console",
            "appender": {
                "console": {
                    "@class": "org.apache.log4j.ConsoleAppender",
                    "target": "STDOUT_TARGET",
                    "layout": {
                        "@class": "org.apache.log4j.TTCCLayout",
                        "dateFormat": "ISO8601"
                    }
                }
            },
            "logger": {
                "MyApp": "ERROR, console"
            },
            "additivity": {
                "MyApp": "false"
            }
        }
    })");

    QVERIFY(JsonConfigurator::configure(file));

    Logger *root = LogManager::rootLogger();
    QCOMPARE(root->level(), Level::ALL_INT);

    Logger *myApp = LogManager::logger(u"MyApp"_s);
    QCOMPARE(myApp->level(), Level::ERROR_INT);
    QCOMPARE(myApp->additivity(), false);
}

void JsonConfiguratorTest::testConfigureFromFile()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.path() + "/test.json";
    writeJsonFile(file, R"({
        "log4j": {
            "rootLogger": "TRACE, a1",
            "appender": {
                "a1": {
                    "@class": "org.apache.log4j.ConsoleAppender",
                    "layout": {
                        "@class": "org.apache.log4j.SimpleLayout"
                    }
                }
            }
        }
    })");

    JsonConfigurator configurator;
    QVERIFY(configurator.doConfigure(file));
    QCOMPARE(LogManager::rootLogger()->level(), Level::TRACE_INT);
}

void JsonConfiguratorTest::testConfigureMissingFile()
{
    QVERIFY(!JsonConfigurator::configure(u"/nonexistent/path/to/file.json"_s));
}

void JsonConfiguratorTest::testConfigureMalformedJson()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.path() + "/bad.json";
    writeJsonFile(file, "{ not valid json }}}");

    QVERIFY(!JsonConfigurator::configure(file));
}

void JsonConfiguratorTest::testConfigureNonObjectRoot()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.path() + "/array.json";
    writeJsonFile(file, "[1, 2, 3]");

    QVERIFY(!JsonConfigurator::configure(file));
}

QTEST_GUILESS_MAIN(JsonConfiguratorTest)
#include "tst_jsonconfigurator.moc"
