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

#include <QTest>
#include <QJsonDocument>
#include <QJsonObject>

#include "log4qt/jsonlayout.h"
#include "log4qt/loggingevent.h"
#include "log4qt/logmanager.h"

using namespace Log4Qt;

// Fixed timestamp for deterministic tests: 2024-01-15 10:30:00.123 UTC
static constexpr qint64 TEST_TIMESTAMP = 1705314600123LL;

class JsonLayoutTest : public QObject
{
    Q_OBJECT

private:
    // Helper: build a minimal event with explicit NDC, MDC, thread name, timestamp
    static LoggingEvent makeEvent(Level level,
                                  const QString &message,
                                  const QString &ndc = {},
                                  const QHash<QString, QString> &mdc = {},
                                  const QString &threadName = QStringLiteral("testThread"))
    {
        return LoggingEvent(LogManager::rootLogger(), level, message,
                            ndc, mdc, threadName, TEST_TIMESTAMP);
    }

    // Helper: format an event and parse the resulting JSON object
    static QJsonObject formatToJson(JsonLayout &layout, const LoggingEvent &event)
    {
        const QString output = layout.format(event);
        // Strip trailing newline before parsing
        const QString stripped = output.trimmed();
        QJsonParseError err;
        const QJsonDocument doc = QJsonDocument::fromJson(stripped.toUtf8(), &err);
        Q_ASSERT_X(err.error == QJsonParseError::NoError,
                   "formatToJson", qPrintable(err.errorString()));
        return doc.object();
    }

private Q_SLOTS:
    void cleanup();

    void defaultProperties();
    void contentType();
    void requiresLocation();

    void format_isValidJson();
    void format_endsWithNewline();
    void format_defaultFields();
    void format_timestamp();
    void format_level();
    void format_logger();
    void format_thread();
    void format_message();
    void format_disableFields();

    void format_includeNdc();
    void format_ndcOmittedWhenEmpty();
    void format_includeMdc();
    void format_mdcOmittedWhenEmpty();

    void format_includeLocation();
    void format_locationOmittedByDefault();

    void format_prettyPrint();
};

void JsonLayoutTest::cleanup()
{
    LogManager::resetConfiguration();
}

// ---------------------------------------------------------------------------
// Properties / metadata
// ---------------------------------------------------------------------------

void JsonLayoutTest::defaultProperties()
{
    JsonLayout layout;
    QCOMPARE(layout.includeTimestamp(), true);
    QCOMPARE(layout.includeLevel(),     true);
    QCOMPARE(layout.includeLogger(),    true);
    QCOMPARE(layout.includeThread(),    true);
    QCOMPARE(layout.includeMessage(),   true);
    QCOMPARE(layout.includeNdc(),       false);
    QCOMPARE(layout.includeMdc(),       false);
    QCOMPARE(layout.includeLocation(),  false);
    QCOMPARE(layout.prettyPrint(),      false);
}

void JsonLayoutTest::contentType()
{
    JsonLayout layout;
    QCOMPARE(layout.contentType(), QStringLiteral("application/json; charset=UTF-8"));
}

void JsonLayoutTest::requiresLocation()
{
    JsonLayout layout;
    QCOMPARE(layout.requiresLocation(), false);

    layout.setIncludeLocation(true);
    QCOMPARE(layout.requiresLocation(), true);

    layout.setIncludeLocation(false);
    QCOMPARE(layout.requiresLocation(), false);
}

// ---------------------------------------------------------------------------
// Output correctness
// ---------------------------------------------------------------------------

void JsonLayoutTest::format_isValidJson()
{
    JsonLayout layout;
    const QString output = layout.format(makeEvent(Level::INFO_INT, QStringLiteral("hello")));
    const QString stripped = output.trimmed();

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(stripped.toUtf8(), &err);
    QCOMPARE(err.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());
}

void JsonLayoutTest::format_endsWithNewline()
{
    JsonLayout layout;
    const QString output = layout.format(makeEvent(Level::DEBUG_INT, QStringLiteral("x")));
    QVERIFY(output.endsWith(QLatin1Char('\n')));
}

void JsonLayoutTest::format_defaultFields()
{
    JsonLayout layout;
    const LoggingEvent event = makeEvent(Level::WARN_INT, QStringLiteral("test message"));
    const QJsonObject obj = formatToJson(layout, event);

    QVERIFY(obj.contains(QStringLiteral("timestamp")));
    QVERIFY(obj.contains(QStringLiteral("level")));
    QVERIFY(obj.contains(QStringLiteral("logger")));
    QVERIFY(obj.contains(QStringLiteral("thread")));
    QVERIFY(obj.contains(QStringLiteral("message")));
    QVERIFY(!obj.contains(QStringLiteral("ndc")));
    QVERIFY(!obj.contains(QStringLiteral("mdc")));
    QVERIFY(!obj.contains(QStringLiteral("file")));
    QVERIFY(!obj.contains(QStringLiteral("line")));
    QVERIFY(!obj.contains(QStringLiteral("function")));
}

void JsonLayoutTest::format_timestamp()
{
    JsonLayout layout;
    const QJsonObject obj = formatToJson(layout, makeEvent(Level::INFO_INT, {}));
    QCOMPARE(obj.value(QStringLiteral("timestamp")).toVariant().toLongLong(), TEST_TIMESTAMP);
}

void JsonLayoutTest::format_level()
{
    JsonLayout layout;
    QJsonObject obj = formatToJson(layout, makeEvent(Level::ERROR_INT, {}));
    QCOMPARE(obj.value(QStringLiteral("level")).toString(), QStringLiteral("ERROR"));

    obj = formatToJson(layout, makeEvent(Level::DEBUG_INT, {}));
    QCOMPARE(obj.value(QStringLiteral("level")).toString(), QStringLiteral("DEBUG"));
}

void JsonLayoutTest::format_logger()
{
    JsonLayout layout;
    const QJsonObject obj = formatToJson(layout, makeEvent(Level::INFO_INT, {}));
    // root logger name is empty string or "root"
    QVERIFY(obj.contains(QStringLiteral("logger")));
}

void JsonLayoutTest::format_thread()
{
    JsonLayout layout;
    const QJsonObject obj = formatToJson(layout, makeEvent(Level::INFO_INT, {},
                                                           {}, {}, QStringLiteral("myThread")));
    QCOMPARE(obj.value(QStringLiteral("thread")).toString(), QStringLiteral("myThread"));
}

void JsonLayoutTest::format_message()
{
    JsonLayout layout;
    const QJsonObject obj = formatToJson(layout, makeEvent(Level::INFO_INT,
                                                           QStringLiteral("hello world")));
    QCOMPARE(obj.value(QStringLiteral("message")).toString(), QStringLiteral("hello world"));
}

void JsonLayoutTest::format_disableFields()
{
    JsonLayout layout;
    layout.setIncludeTimestamp(false);
    layout.setIncludeLevel(false);
    layout.setIncludeLogger(false);
    layout.setIncludeThread(false);
    layout.setIncludeMessage(false);

    const QJsonObject obj = formatToJson(layout, makeEvent(Level::INFO_INT, QStringLiteral("x")));
    QVERIFY(!obj.contains(QStringLiteral("timestamp")));
    QVERIFY(!obj.contains(QStringLiteral("level")));
    QVERIFY(!obj.contains(QStringLiteral("logger")));
    QVERIFY(!obj.contains(QStringLiteral("thread")));
    QVERIFY(!obj.contains(QStringLiteral("message")));
    QVERIFY(obj.isEmpty());
}

// ---------------------------------------------------------------------------
// NDC
// ---------------------------------------------------------------------------

void JsonLayoutTest::format_includeNdc()
{
    JsonLayout layout;
    layout.setIncludeNdc(true);

    const QJsonObject obj = formatToJson(layout,
        makeEvent(Level::INFO_INT, {}, QStringLiteral("requestId=42")));
    QCOMPARE(obj.value(QStringLiteral("ndc")).toString(), QStringLiteral("requestId=42"));
}

void JsonLayoutTest::format_ndcOmittedWhenEmpty()
{
    JsonLayout layout;
    layout.setIncludeNdc(true);

    // Empty NDC string → field should be absent
    const QJsonObject obj = formatToJson(layout, makeEvent(Level::INFO_INT, {}, {}));
    QVERIFY(!obj.contains(QStringLiteral("ndc")));
}

// ---------------------------------------------------------------------------
// MDC
// ---------------------------------------------------------------------------

void JsonLayoutTest::format_includeMdc()
{
    JsonLayout layout;
    layout.setIncludeMdc(true);

    const QHash<QString, QString> mdc{{QStringLiteral("userId"), QStringLiteral("99")},
                                      {QStringLiteral("env"),    QStringLiteral("test")}};
    const QJsonObject obj = formatToJson(layout, makeEvent(Level::INFO_INT, {}, {}, mdc));

    QVERIFY(obj.contains(QStringLiteral("mdc")));
    const QJsonObject mdcObj = obj.value(QStringLiteral("mdc")).toObject();
    QCOMPARE(mdcObj.value(QStringLiteral("userId")).toString(), QStringLiteral("99"));
    QCOMPARE(mdcObj.value(QStringLiteral("env")).toString(), QStringLiteral("test"));
}

void JsonLayoutTest::format_mdcOmittedWhenEmpty()
{
    JsonLayout layout;
    layout.setIncludeMdc(true);

    // Empty MDC → field should be absent
    const QJsonObject obj = formatToJson(layout, makeEvent(Level::INFO_INT, {}, {}, {}));
    QVERIFY(!obj.contains(QStringLiteral("mdc")));
}

// ---------------------------------------------------------------------------
// Location
// ---------------------------------------------------------------------------

void JsonLayoutTest::format_includeLocation()
{
    JsonLayout layout;
    layout.setIncludeLocation(true);

    const MessageContext ctx("myfile.cpp", 42, "MyClass::myMethod");
    const LoggingEvent event(LogManager::rootLogger(), Level::INFO_INT,
                             QStringLiteral("msg"), {}, {},
                             TEST_TIMESTAMP, ctx, {});

    const QJsonObject obj = formatToJson(layout, event);
    QCOMPARE(obj.value(QStringLiteral("file")).toString(),     QStringLiteral("myfile.cpp"));
    QCOMPARE(obj.value(QStringLiteral("line")).toInt(),        42);
    QCOMPARE(obj.value(QStringLiteral("function")).toString(), QStringLiteral("MyClass::myMethod"));
}

void JsonLayoutTest::format_locationOmittedByDefault()
{
    JsonLayout layout;
    // includeLocation defaults to false
    const MessageContext ctx("myfile.cpp", 10, "foo");
    const LoggingEvent event(LogManager::rootLogger(), Level::INFO_INT,
                             QStringLiteral("msg"), {}, {},
                             TEST_TIMESTAMP, ctx, {});

    const QJsonObject obj = formatToJson(layout, event);
    QVERIFY(!obj.contains(QStringLiteral("file")));
    QVERIFY(!obj.contains(QStringLiteral("line")));
    QVERIFY(!obj.contains(QStringLiteral("function")));
}

// ---------------------------------------------------------------------------
// Pretty print
// ---------------------------------------------------------------------------

void JsonLayoutTest::format_prettyPrint()
{
    JsonLayout layout;
    layout.setPrettyPrint(true);

    const QString output = layout.format(makeEvent(Level::INFO_INT, QStringLiteral("hi")));
    // Indented JSON contains newlines inside the object
    QVERIFY(output.count(QLatin1Char('\n')) > 1);
    // Still valid JSON
    const QJsonDocument doc = QJsonDocument::fromJson(output.trimmed().toUtf8());
    QVERIFY(!doc.isNull());
}

QTEST_MAIN(JsonLayoutTest)
#include "tst_jsonlayout.moc"
