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

#include "log4qt/logger.h"

#include "loggerobject.h"
#include "loggerobjectprio.h"
#include "loggerstatic.h"
#include "log4qt/abstractlayout.h"
#include "log4qt/helpers/factory.h"
#include "log4qt/logmanager.h"
#include "log4qt/propertyconfigurator.h"
#include "log4qt/loggerrepository.h"
#include "log4qt/spi/headerfooterprovider.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QScopedPointer>
#include <QStringBuilder>
#include <QString>
#include <QFile>

// Application-defined provider that writes the device serial number into
// every log file header. Registered with the Factory before the properties
// file is loaded so PropertyConfigurator can instantiate and configure it
// by class name via the serialNumber Q_PROPERTY.
class SerialNumberHeaderProvider : public Log4Qt::HeaderFooterProvider
{
    Q_OBJECT
    Q_PROPERTY(QString serialNumber READ serialNumber WRITE setSerialNumber)
public:
    explicit SerialNumberHeaderProvider(QObject *parent = nullptr)
        : Log4Qt::HeaderFooterProvider(parent) {}

    QString serialNumber() const { return mSerialNumber; }
    void setSerialNumber(const QString &sn) { mSerialNumber = sn; }

    QString header() const override
    { return QStringLiteral("Device S/N: ") + mSerialNumber; }

private:
    QString mSerialNumber;
};

// Provider for JSON log files: emits header and footer as JSON objects so
// the serial number and session timestamps are machine-readable.
// Registered with the Factory; the serial number is injected by the
// application after the config has created the provider instance.
class JsonSerialNumberHeaderProvider : public Log4Qt::HeaderFooterProvider
{
    Q_OBJECT
    Q_PROPERTY(QString serialNumber READ serialNumber WRITE setSerialNumber)
public:
    explicit JsonSerialNumberHeaderProvider(QObject *parent = nullptr)
        : Log4Qt::HeaderFooterProvider(parent) {}

    QString serialNumber() const { return mSerialNumber; }
    void setSerialNumber(const QString &sn) { mSerialNumber = sn; }

    QString header() const override
    {
        return QStringLiteral(R"({"event":"start","serialNumber":"%1","time":"%2"})")
            .arg(mSerialNumber, QDateTime::currentDateTime().toString(Qt::ISODate));
    }

    QString footer() const override
    {
        return QStringLiteral(R"({"event":"end","serialNumber":"%1","time":"%2"})")
            .arg(mSerialNumber, QDateTime::currentDateTime().toString(Qt::ISODate));
    }

private:
    QString mSerialNumber;
};

static void initializeRootLogger();
static void shutdownRootLogger();
static void logStartup();
static void logShutdown();
static void setupRootLogger(const QString &introMessage);
static void shutDownRootLogger(const QString &extroMessage);


// Shows how to configure logging with a property file
int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);

    // Force LogManager to initialize (running doStartup) and immediately reset
    // it before any application-specific configuration is done.  Without this,
    // doStartup would auto-load the properties file before the custom
    // HeaderFooterProvider factories are registered, resulting in a spurious
    // footer/header pair written at startup.
    Log4Qt::LogManager::resetConfiguration();

    const auto *object = new LoggerObject(&application);
    const auto *object1 = new LoggerObjectPrio(&application);
    Q_UNUSED(object1)

    QObject::connect(object, &LoggerObject::exit, &application, &QCoreApplication::exit);

    initializeRootLogger();
    logStartup();

    int ret;
    {
        QScopedPointer<LoggerStatic> object2(new LoggerStatic());

        ret = QCoreApplication::exec();
    }

    logShutdown();
    shutdownRootLogger();

    return ret;
}

void initializeRootLogger()
{
    setupRootLogger(QStringLiteral("Root logger is setup."));
}

void shutdownRootLogger()
{
    shutDownRootLogger(QStringLiteral("Root logger was shutdown."));
}

void logStartup()
{
    auto logger = Log4Qt::Logger::rootLogger();

    logger->info(QStringLiteral("################################################################"));
    logger->info(QStringLiteral("#                          START                               #"));
    logger->info(QStringLiteral("################################################################"));
}

void logShutdown()
{
    auto logger = Log4Qt::Logger::rootLogger();

    logger->info(QStringLiteral("################################################################"));
    logger->info(QStringLiteral("#                          STOP                                #"));
    logger->info(QStringLiteral("################################################################"));
}

void setupRootLogger(const QString &introMessage)
{
    // Register the application-defined provider before loading the config so
    // PropertyConfigurator can instantiate it by class name and set its
    // serialNumber Q_PROPERTY from the properties file.
    // Read the serial number from hardware before registering the factory
    // functions. The lambdas capture it so every provider instance created
    // by the configurator already has the value set when activateOptions()
    // opens the file and writes the header.
    const QString serialNumber = QStringLiteral("SN-20260001");   // read from hardware in a real application

    Log4Qt::Factory::registerHeaderFooterProvider(
        "SerialNumberHeaderProvider",
        [serialNumber]() -> Log4Qt::HeaderFooterProvider * {
            auto *p = new SerialNumberHeaderProvider;
            p->setSerialNumber(serialNumber);
            return p;
        });
    Log4Qt::Factory::registerHeaderFooterProvider(
        "JsonSerialNumberHeaderProvider",
        [serialNumber]() -> Log4Qt::HeaderFooterProvider * {
            auto *p = new JsonSerialNumberHeaderProvider;
            p->setSerialNumber(serialNumber);
            return p;
        });

    QString configFile = QCoreApplication::applicationFilePath() + QStringLiteral(".log4qt.properties");
    if (QFile::exists(configFile))
        Log4Qt::PropertyConfigurator::configureAndWatch(configFile);

    if (!introMessage.isEmpty())
        Log4Qt::Logger::rootLogger()->info(introMessage);
}

void shutDownRootLogger(const QString &extroMessage)
{
    auto logger = Log4Qt::Logger::rootLogger();

    if (!extroMessage.isEmpty())
        logger->info(extroMessage);
    logger->removeAllAppenders();
    logger->loggerRepository()->shutdown();
    Log4Qt::AbstractLayout::setGlobalHeaderFooterProvider({});
    Log4Qt::Factory::unregisterHeaderFooterProvider("SerialNumberHeaderProvider");
    Log4Qt::Factory::unregisterHeaderFooterProvider("JsonSerialNumberHeaderProvider");
}

#include "main.moc"
