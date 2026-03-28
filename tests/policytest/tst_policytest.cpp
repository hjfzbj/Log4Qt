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
#include <QtTest>

#include "log4qt/helpers/cronexpression.h"
#include "log4qt/helpers/factory.h"
#include "log4qt/helpers/properties.h"
#include "log4qt/loggingevent.h"
#include "log4qt/logmanager.h"
#include "log4qt/propertyconfigurator.h"
#include "log4qt/rollingfileappender.h"
#include "log4qt/simplelayout.h"
#include "log4qt/spi/compositetriggeringpolicy.h"
#include "log4qt/spi/crontriggeringpolicy.h"
#include "log4qt/spi/defaultrolloverstrategy.h"
#include "log4qt/spi/onstartuptriggeringpolicy.h"
#include "log4qt/spi/sizebasedtriggeringpolicy.h"
#include "log4qt/spi/timebasedtriggeringpolicy.h"

using namespace Log4Qt;

class PolicyTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void cleanup();

    // CronExpression
    void CronExpression_parse_data();
    void CronExpression_parse();
    void CronExpression_nextFireTime_data();
    void CronExpression_nextFireTime();
    void CronExpression_dayOfWeek();
    void CronExpression_noMatchWithin4Years();

    // SizeBasedTriggeringPolicy
    void SizeBasedTriggeringPolicy_defaults();
    void SizeBasedTriggeringPolicy_trigger_data();
    void SizeBasedTriggeringPolicy_trigger();
    void SizeBasedTriggeringPolicy_maxFileSize_string();

    // TimeBasedTriggeringPolicy
    void TimeBasedTriggeringPolicy_defaults();
    void TimeBasedTriggeringPolicy_frequency_data();
    void TimeBasedTriggeringPolicy_frequency();
    void TimeBasedTriggeringPolicy_invalidPattern();

    // CronTriggeringPolicy
    void CronTriggeringPolicy_defaults();
    void CronTriggeringPolicy_invalidSchedule();
    void CronTriggeringPolicy_activateAndTrigger();

    // OnStartupTriggeringPolicy
    void OnStartupTriggeringPolicy_isTriggeringEvent();
    void OnStartupTriggeringPolicy_isStartupTrigger_data();
    void OnStartupTriggeringPolicy_isStartupTrigger();

    // CompositeTriggeringPolicy
    void CompositeTriggeringPolicy_empty();
    void CompositeTriggeringPolicy_orCombination();
    void CompositeTriggeringPolicy_startupTrigger();

    // DefaultRolloverStrategy
    void DefaultRolloverStrategy_defaults();
    void DefaultRolloverStrategy_rollover();
    void DefaultRolloverStrategy_noExistingFile();
    void DefaultRolloverStrategy_singleBackup();

    // Factory
    void Factory_createTriggeringPolicy_data();
    void Factory_createTriggeringPolicy();
    void Factory_createRolloverStrategy_data();
    void Factory_createRolloverStrategy();

    // PropertyConfigurator integration
    void PropertyConfigurator_cronPolicy();
};

void PolicyTest::cleanup()
{
    LogManager::resetConfiguration();
}

// ---------------------------------------------------------------------------
// CronExpression
// ---------------------------------------------------------------------------

void PolicyTest::CronExpression_parse_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<bool>("valid");

    QTest::newRow("valid-midnight-daily")   << "0 0 0 * * ?"      << true;
    QTest::newRow("valid-every-minute")     << "0 * * * * ?"       << true;
    QTest::newRow("valid-every-second")     << "* * * * * ?"       << true;
    QTest::newRow("valid-range")            << "0 0 9-17 * * ?"    << true;
    QTest::newRow("valid-list")             << "0 0,15,30,45 * * * ?" << true;
    QTest::newRow("valid-step")             << "0 0 0/4 * * ?"     << true;
    QTest::newRow("valid-month-name")       << "0 0 0 1 JAN ?"     << true;
    QTest::newRow("valid-dow-name")         << "0 0 8 ? * MON-FRI" << true;

    QTest::newRow("invalid-empty")          << ""                  << false;
    QTest::newRow("invalid-5-fields")       << "0 0 0 * *"         << false;
    QTest::newRow("invalid-7-fields")       << "0 0 0 * * ? 2026"  << false;
    QTest::newRow("invalid-seconds-60")     << "60 0 0 * * ?"      << false;
    QTest::newRow("invalid-hours-24")       << "0 0 24 * * ?"      << false;
    QTest::newRow("invalid-dom-32")         << "0 0 0 32 * ?"      << false;
    QTest::newRow("invalid-month-13")       << "0 0 0 * 13 ?"      << false;
    QTest::newRow("invalid-dow-8")          << "0 0 0 * * 8"       << false;
    QTest::newRow("invalid-step-zero")      << "0 0 0/0 * * ?"     << false;
    QTest::newRow("invalid-inverted-range") << "0 0 5-1 * * ?"     << false;
}

void PolicyTest::CronExpression_parse()
{
    QFETCH(QString, expression);
    QFETCH(bool, valid);

    Log4Qt::CronExpression cron(expression);
    QCOMPARE(cron.isValid(), valid);
    if (!valid)
        QVERIFY(!cron.errorString().isEmpty());
}

void PolicyTest::CronExpression_nextFireTime_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QDateTime>("from");
    QTest::addColumn<QDateTime>("expected");

    // midnight daily: from 10:00 -> next day 00:00
    QTest::newRow("midnight-daily")
        << "0 0 0 * * ?"
        << QDateTime(QDate(2026, 3, 28), QTime(10, 0, 0))
        << QDateTime(QDate(2026, 3, 29), QTime(0, 0, 0));

    // half-past every hour: from 10:15 -> 10:30
    QTest::newRow("half-past-hour")
        << "0 30 * * * ?"
        << QDateTime(QDate(2026, 3, 28), QTime(10, 15, 0))
        << QDateTime(QDate(2026, 3, 28), QTime(10, 30, 0));

    // first of month: from Mar 15 -> Apr 1
    QTest::newRow("first-of-month")
        << "0 0 0 1 * ?"
        << QDateTime(QDate(2026, 3, 15), QTime(0, 0, 0))
        << QDateTime(QDate(2026, 4, 1), QTime(0, 0, 0));

    // every 15 seconds: from 10:00:07 -> 10:00:15
    QTest::newRow("every-15-seconds")
        << "*/15 * * * * ?"
        << QDateTime(QDate(2026, 3, 28), QTime(10, 0, 7))
        << QDateTime(QDate(2026, 3, 28), QTime(10, 0, 15));

    // noon and midnight: from 10:00 -> 12:00
    QTest::newRow("noon-and-midnight")
        << "0 0 0,12 * * ?"
        << QDateTime(QDate(2026, 3, 28), QTime(10, 0, 0))
        << QDateTime(QDate(2026, 3, 28), QTime(12, 0, 0));

    // January only: from March -> next January
    QTest::newRow("january-only")
        << "0 0 0 * JAN ?"
        << QDateTime(QDate(2026, 3, 1), QTime(0, 0, 0))
        << QDateTime(QDate(2027, 1, 1), QTime(0, 0, 0));
}

void PolicyTest::CronExpression_nextFireTime()
{
    QFETCH(QString, expression);
    QFETCH(QDateTime, from);
    QFETCH(QDateTime, expected);

    Log4Qt::CronExpression cron(expression);
    QVERIFY(cron.isValid());

    QDateTime result = cron.nextFireTime(from);
    QVERIFY(result.isValid());
    QCOMPARE(result, expected);
}

void PolicyTest::CronExpression_dayOfWeek()
{
    // 2026-03-28 is a Saturday
    // MON schedule should fire on 2026-03-30 (Monday)
    Log4Qt::CronExpression cron("0 0 8 ? * MON");
    QVERIFY(cron.isValid());

    QDateTime from(QDate(2026, 3, 28), QTime(9, 0, 0));
    QDateTime result = cron.nextFireTime(from);
    QCOMPARE(result, QDateTime(QDate(2026, 3, 30), QTime(8, 0, 0)));
    QCOMPARE(result.date().dayOfWeek(), 1); // Qt: 1=Monday

    // SUN schedule: from Saturday -> Sunday
    Log4Qt::CronExpression cronSun("0 0 0 ? * SUN");
    QVERIFY(cronSun.isValid());
    QDateTime resultSun = cronSun.nextFireTime(from);
    QCOMPARE(resultSun, QDateTime(QDate(2026, 3, 29), QTime(0, 0, 0)));
    QCOMPARE(resultSun.date().dayOfWeek(), 7); // Qt: 7=Sunday

    // SAT schedule: from Saturday 9:00 -> next Saturday (since 8:00 already passed)
    Log4Qt::CronExpression cronSat("0 0 8 ? * SAT");
    QVERIFY(cronSat.isValid());
    QDateTime resultSat = cronSat.nextFireTime(from);
    QCOMPARE(resultSat, QDateTime(QDate(2026, 4, 4), QTime(8, 0, 0)));
}

void PolicyTest::CronExpression_noMatchWithin4Years()
{
    // Day 31 of February will never match
    Log4Qt::CronExpression cron("0 0 0 31 2 ?");
    QVERIFY(cron.isValid()); // parses fine, but no date ever matches

    QDateTime from(QDate(2026, 1, 1), QTime(0, 0, 0));
    QDateTime result = cron.nextFireTime(from);
    QVERIFY(!result.isValid());
}

// ---------------------------------------------------------------------------
// SizeBasedTriggeringPolicy
// ---------------------------------------------------------------------------

void PolicyTest::SizeBasedTriggeringPolicy_defaults()
{
    Log4Qt::SizeBasedTriggeringPolicy policy;
    QCOMPARE(policy.maximumFileSize(), static_cast<qint64>(10 * 1024 * 1024));
}

void PolicyTest::SizeBasedTriggeringPolicy_trigger_data()
{
    QTest::addColumn<qint64>("fileSize");
    QTest::addColumn<qint64>("maxSize");
    QTest::addColumn<bool>("expected");

    QTest::newRow("below-limit")    << qint64(100) << qint64(200)  << false;
    QTest::newRow("at-limit")       << qint64(200) << qint64(200)  << false; // uses >
    QTest::newRow("one-over")       << qint64(201) << qint64(200)  << true;
    QTest::newRow("zero-size")      << qint64(0)   << qint64(200)  << false;
    QTest::newRow("large-file")     << qint64(1024 * 1024 * 100) << qint64(1024 * 1024) << true;
}

void PolicyTest::SizeBasedTriggeringPolicy_trigger()
{
    QFETCH(qint64, fileSize);
    QFETCH(qint64, maxSize);
    QFETCH(bool, expected);

    Log4Qt::SizeBasedTriggeringPolicy policy;
    policy.setMaximumFileSize(maxSize);

    LoggingEvent event;
    QCOMPARE(policy.isTriggeringEvent(QString(), fileSize, event), expected);
}

void PolicyTest::SizeBasedTriggeringPolicy_maxFileSize_string()
{
    Log4Qt::SizeBasedTriggeringPolicy policy;

    policy.setMaxFileSize("5MB");
    QCOMPARE(policy.maximumFileSize(), static_cast<qint64>(5 * 1024 * 1024));

    policy.setMaxFileSize("1KB");
    QCOMPARE(policy.maximumFileSize(), static_cast<qint64>(1024));

    // Invalid string should not change value
    qint64 before = policy.maximumFileSize();
    policy.setMaxFileSize("notanumber");
    QCOMPARE(policy.maximumFileSize(), before);
}

// ---------------------------------------------------------------------------
// TimeBasedTriggeringPolicy
// ---------------------------------------------------------------------------

void PolicyTest::TimeBasedTriggeringPolicy_defaults()
{
    Log4Qt::TimeBasedTriggeringPolicy policy;
    QCOMPARE(policy.datePattern(), QString("'.'yyyy-MM-dd"));
    QCOMPARE(policy.frequency(), Log4Qt::TimeBasedTriggeringPolicy::Daily);
}

void PolicyTest::TimeBasedTriggeringPolicy_frequency_data()
{
    QTest::addColumn<QString>("pattern");
    QTest::addColumn<int>("expectedFrequency");

    QTest::newRow("minutely-mm")  << "'.'mm"         << static_cast<int>(TimeBasedTriggeringPolicy::Minutely);
    QTest::newRow("hourly-HH")   << "'.'HH"         << static_cast<int>(TimeBasedTriggeringPolicy::Hourly);
    QTest::newRow("halfdaily-a")  << "'.'a"          << static_cast<int>(TimeBasedTriggeringPolicy::HalfDaily);
    QTest::newRow("daily-dd")     << "'.'yyyy-MM-dd" << static_cast<int>(TimeBasedTriggeringPolicy::Daily);
    QTest::newRow("monthly-MM")   << "'.'yyyy-MM"    << static_cast<int>(TimeBasedTriggeringPolicy::Monthly);
}

void PolicyTest::TimeBasedTriggeringPolicy_frequency()
{
    QFETCH(QString, pattern);
    QFETCH(int, expectedFrequency);

    Log4Qt::TimeBasedTriggeringPolicy policy;
    policy.setDatePattern(pattern);
    policy.activateOptions();

    QCOMPARE(static_cast<int>(policy.frequency()), expectedFrequency);
}

void PolicyTest::TimeBasedTriggeringPolicy_invalidPattern()
{
    Log4Qt::TimeBasedTriggeringPolicy policy;
    policy.setDatePattern("'.'constant-no-change");
    policy.activateOptions();

    // Pattern that doesn't change at any frequency -> always returns false
    LoggingEvent event;
    QCOMPARE(policy.isTriggeringEvent(QString(), 0, event), false);
}

// ---------------------------------------------------------------------------
// CronTriggeringPolicy
// ---------------------------------------------------------------------------

void PolicyTest::CronTriggeringPolicy_defaults()
{
    Log4Qt::CronTriggeringPolicy policy;
    QCOMPARE(policy.schedule(), QString("0 0 0 * * ?"));
}

void PolicyTest::CronTriggeringPolicy_invalidSchedule()
{
    Log4Qt::CronTriggeringPolicy policy;
    policy.setSchedule("invalid cron");
    policy.activateOptions();

    LoggingEvent event;
    QCOMPARE(policy.isTriggeringEvent(QString(), 0, event), false);
}

void PolicyTest::CronTriggeringPolicy_activateAndTrigger()
{
    Log4Qt::CronTriggeringPolicy policy;
    // Fire every second
    policy.setSchedule("* * * * * ?");
    policy.activateOptions();

    // Wait just over 1 second for the next fire time to pass
    QTest::qWait(1100);

    LoggingEvent event;
    QCOMPARE(policy.isTriggeringEvent(QString(), 0, event), true);

    // After triggering, next fire time is recomputed; immediate re-check should be false
    // (unless another second boundary was crossed during the call)
}

// ---------------------------------------------------------------------------
// OnStartupTriggeringPolicy
// ---------------------------------------------------------------------------

void PolicyTest::OnStartupTriggeringPolicy_isTriggeringEvent()
{
    Log4Qt::OnStartupTriggeringPolicy policy;
    LoggingEvent event;

    // Always returns false regardless of parameters
    QCOMPARE(policy.isTriggeringEvent("anyfile.log", 999, event), false);
    QCOMPARE(policy.isTriggeringEvent(QString(), 0, event), false);
}

void PolicyTest::OnStartupTriggeringPolicy_isStartupTrigger_data()
{
    QTest::addColumn<bool>("createFile");
    QTest::addColumn<bool>("writeContent");
    QTest::addColumn<bool>("expected");

    QTest::newRow("file-with-content")  << true  << true  << true;
    QTest::newRow("empty-file")         << true  << false << false;
    QTest::newRow("no-file")            << false << false << false;
}

void PolicyTest::OnStartupTriggeringPolicy_isStartupTrigger()
{
    QFETCH(bool, createFile);
    QFETCH(bool, writeContent);
    QFETCH(bool, expected);

    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    QString filePath = tempDir.path() + "/test.log";

    if (createFile)
    {
        QFile file(filePath);
        QVERIFY(file.open(QIODevice::WriteOnly));
        if (writeContent)
            file.write("some log content\n");
        file.close();
    }

    Log4Qt::OnStartupTriggeringPolicy policy;
    QCOMPARE(policy.isStartupTrigger(filePath, 0), expected);
}

// ---------------------------------------------------------------------------
// CompositeTriggeringPolicy
// ---------------------------------------------------------------------------

void PolicyTest::CompositeTriggeringPolicy_empty()
{
    Log4Qt::CompositeTriggeringPolicy policy;
    LoggingEvent event;

    QCOMPARE(policy.isTriggeringEvent(QString(), 100, event), false);
    QCOMPARE(policy.isStartupTrigger(QString(), 0), false);
    QCOMPARE(policy.policies().count(), 0);
}

void PolicyTest::CompositeTriggeringPolicy_orCombination()
{
    Log4Qt::CompositeTriggeringPolicy composite;

    // Policy 1: triggers above 100 bytes
    auto policy1 = TriggeringPolicySharedPtr(new Log4Qt::SizeBasedTriggeringPolicy);
    qobject_cast<Log4Qt::SizeBasedTriggeringPolicy *>(policy1.data())->setMaximumFileSize(100);

    // Policy 2: triggers above 500 bytes
    auto policy2 = TriggeringPolicySharedPtr(new Log4Qt::SizeBasedTriggeringPolicy);
    qobject_cast<Log4Qt::SizeBasedTriggeringPolicy *>(policy2.data())->setMaximumFileSize(500);

    composite.addPolicy(policy1);
    composite.addPolicy(policy2);

    QCOMPARE(composite.policies().count(), 2);

    LoggingEvent event;

    // 50 bytes: neither triggers
    QCOMPARE(composite.isTriggeringEvent(QString(), 50, event), false);

    // 200 bytes: policy1 triggers (> 100), policy2 doesn't
    QCOMPARE(composite.isTriggeringEvent(QString(), 200, event), true);

    // 600 bytes: both trigger
    QCOMPARE(composite.isTriggeringEvent(QString(), 600, event), true);
}

void PolicyTest::CompositeTriggeringPolicy_startupTrigger()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    QString filePath = tempDir.path() + "/test.log";
    {
        QFile file(filePath);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("content");
        file.close();
    }

    Log4Qt::CompositeTriggeringPolicy composite;

    // OnStartup policy should trigger for existing file with content
    auto startupPolicy = TriggeringPolicySharedPtr(new Log4Qt::OnStartupTriggeringPolicy);
    composite.addPolicy(startupPolicy);

    QCOMPARE(composite.isStartupTrigger(filePath, 0), true);

    // Without OnStartup, size-based alone won't trigger startup
    Log4Qt::CompositeTriggeringPolicy composite2;
    auto sizePolicy = TriggeringPolicySharedPtr(new Log4Qt::SizeBasedTriggeringPolicy);
    composite2.addPolicy(sizePolicy);
    QCOMPARE(composite2.isStartupTrigger(filePath, 0), false);
}

// ---------------------------------------------------------------------------
// DefaultRolloverStrategy
// ---------------------------------------------------------------------------

void PolicyTest::DefaultRolloverStrategy_defaults()
{
    Log4Qt::DefaultRolloverStrategy strategy;
    QCOMPARE(strategy.minIndex(), 1);
    QCOMPARE(strategy.maxIndex(), 7);
}

void PolicyTest::DefaultRolloverStrategy_rollover()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    QString basePath = tempDir.path() + "/app.log";

    Log4Qt::DefaultRolloverStrategy strategy;
    strategy.setMinIndex(1);
    strategy.setMaxIndex(3);

    // Create the base file
    auto writeFile = [](const QString &path, const QString &content)
    {
        QFile f(path);
        f.open(QIODevice::WriteOnly);
        f.write(content.toUtf8());
        f.close();
    };

    auto readFile = [](const QString &path) -> QString
    {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly))
            return QString();
        return QString::fromUtf8(f.readAll());
    };

    // First rollover: app.log -> app.log.1
    writeFile(basePath, "round1");
    QString result = strategy.rollover(basePath);
    QCOMPARE(result, basePath);
    QVERIFY(QFile::exists(basePath + ".1"));
    QCOMPARE(readFile(basePath + ".1"), "round1");
    QVERIFY(!QFile::exists(basePath));

    // Second rollover: app.log.1 -> app.log.2, app.log -> app.log.1
    writeFile(basePath, "round2");
    strategy.rollover(basePath);
    QCOMPARE(readFile(basePath + ".1"), "round2");
    QCOMPARE(readFile(basePath + ".2"), "round1");

    // Third rollover: app.log.2 -> app.log.3, app.log.1 -> app.log.2, app.log -> app.log.1
    writeFile(basePath, "round3");
    strategy.rollover(basePath);
    QCOMPARE(readFile(basePath + ".1"), "round3");
    QCOMPARE(readFile(basePath + ".2"), "round2");
    QCOMPARE(readFile(basePath + ".3"), "round1");

    // Fourth rollover: app.log.3 deleted, others shifted
    writeFile(basePath, "round4");
    strategy.rollover(basePath);
    QCOMPARE(readFile(basePath + ".1"), "round4");
    QCOMPARE(readFile(basePath + ".2"), "round3");
    QCOMPARE(readFile(basePath + ".3"), "round2");
    // "round1" was in .3 and got deleted
    QVERIFY(!QFile::exists(basePath + ".4"));
}

void PolicyTest::DefaultRolloverStrategy_noExistingFile()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    QString basePath = tempDir.path() + "/nonexistent.log";

    Log4Qt::DefaultRolloverStrategy strategy;
    strategy.setMaxIndex(3);

    // Should not crash, returns the filename
    QString result = strategy.rollover(basePath);
    QCOMPARE(result, basePath);
}

void PolicyTest::DefaultRolloverStrategy_singleBackup()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    QString basePath = tempDir.path() + "/single.log";

    Log4Qt::DefaultRolloverStrategy strategy;
    strategy.setMinIndex(1);
    strategy.setMaxIndex(1);

    auto writeFile = [](const QString &path, const QString &content)
    {
        QFile f(path);
        f.open(QIODevice::WriteOnly);
        f.write(content.toUtf8());
        f.close();
    };

    auto readFile = [](const QString &path) -> QString
    {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly))
            return QString();
        return QString::fromUtf8(f.readAll());
    };

    // First rollover
    writeFile(basePath, "first");
    strategy.rollover(basePath);
    QCOMPARE(readFile(basePath + ".1"), "first");

    // Second rollover: .1 is deleted (maxIndex), then base -> .1
    writeFile(basePath, "second");
    strategy.rollover(basePath);
    QCOMPARE(readFile(basePath + ".1"), "second");
    QVERIFY(!QFile::exists(basePath + ".2"));
}

// ---------------------------------------------------------------------------
// Factory
// ---------------------------------------------------------------------------

void PolicyTest::Factory_createTriggeringPolicy_data()
{
    QTest::addColumn<QString>("className");

    QTest::newRow("SizeBasedTriggeringPolicy")      << "SizeBasedTriggeringPolicy";
    QTest::newRow("SizeBased")                      << "SizeBased";
    QTest::newRow("Log4Qt::SizeBasedTriggeringPolicy") << "Log4Qt::SizeBasedTriggeringPolicy";

    QTest::newRow("TimeBasedTriggeringPolicy")      << "TimeBasedTriggeringPolicy";
    QTest::newRow("TimeBased")                      << "TimeBased";
    QTest::newRow("Log4Qt::TimeBasedTriggeringPolicy") << "Log4Qt::TimeBasedTriggeringPolicy";

    QTest::newRow("CronTriggeringPolicy")           << "CronTriggeringPolicy";
    QTest::newRow("Cron")                           << "Cron";
    QTest::newRow("Log4Qt::CronTriggeringPolicy")   << "Log4Qt::CronTriggeringPolicy";

    QTest::newRow("OnStartupTriggeringPolicy")      << "OnStartupTriggeringPolicy";
    QTest::newRow("OnStartup")                      << "OnStartup";
    QTest::newRow("Log4Qt::OnStartupTriggeringPolicy") << "Log4Qt::OnStartupTriggeringPolicy";
}

void PolicyTest::Factory_createTriggeringPolicy()
{
    QFETCH(QString, className);

    TriggeringPolicy *policy = Factory::createTriggeringPolicy(className);
    QVERIFY(policy != nullptr);
    delete policy;
}

void PolicyTest::Factory_createRolloverStrategy_data()
{
    QTest::addColumn<QString>("className");

    QTest::newRow("DefaultRolloverStrategy")        << "DefaultRolloverStrategy";
    QTest::newRow("Default")                        << "Default";
    QTest::newRow("Log4Qt::DefaultRolloverStrategy") << "Log4Qt::DefaultRolloverStrategy";
}

void PolicyTest::Factory_createRolloverStrategy()
{
    QFETCH(QString, className);

    RolloverStrategy *strategy = Factory::createRolloverStrategy(className);
    QVERIFY(strategy != nullptr);
    delete strategy;
}

// ---------------------------------------------------------------------------
// PropertyConfigurator integration
// ---------------------------------------------------------------------------

void PolicyTest::PropertyConfigurator_cronPolicy()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    QString file = tempDir.path() + "/cron.log";

    Properties props;
    props.setProperty("appender.R.type", "RollingFile");
    props.setProperty("appender.R.file", file);
    props.setProperty("appender.R.layout.type", "SimpleLayout");
    props.setProperty("appender.R.policy.CRON.type", "CronTriggeringPolicy");
    props.setProperty("appender.R.policy.CRON.schedule", "0 0 0 * * ?");
    props.setProperty("appender.R.strategy.type", "DefaultRolloverStrategy");
    props.setProperty("appender.R.strategy.maxIndex", "5");
    props.setProperty("rootLogger.level", "DEBUG");
    props.setProperty("rootLogger.appenderRef.0.ref", "R");

    QVERIFY(PropertyConfigurator::configure(props));

    Logger *root = LogManager::rootLogger();
    QCOMPARE(root->appenders().count(), 1);

    auto *appender = qobject_cast<RollingFileAppender *>(root->appenders().first().data());
    QVERIFY(appender != nullptr);
    QVERIFY(appender->triggeringPolicy() != nullptr);
    QVERIFY(appender->rolloverStrategy() != nullptr);

    // Check that the cron policy was correctly configured
    auto *cronPolicy = qobject_cast<CronTriggeringPolicy *>(appender->triggeringPolicy().data());
    QVERIFY(cronPolicy != nullptr);
    QCOMPARE(cronPolicy->schedule(), QString("0 0 0 * * ?"));

    auto *strategy = qobject_cast<DefaultRolloverStrategy *>(appender->rolloverStrategy().data());
    QVERIFY(strategy != nullptr);
    QCOMPARE(strategy->maxIndex(), 5);
}

QTEST_MAIN(PolicyTest)

#include "tst_policytest.moc"
