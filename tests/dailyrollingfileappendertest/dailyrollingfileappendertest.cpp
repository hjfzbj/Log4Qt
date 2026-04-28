#include "log4qt/dailyrollingfileappender.h"
#include "log4qt/helpers/datetime.h"

#include "log4qt/loggingevent.h"
#include "log4qt/simplelayout.h"

#include <QDate>
#include <QTemporaryDir>
#include <QtTest/QTest>

using Log4Qt::DailyRollingFileAppender;

class DailyRollingFileAppenderTest : public QObject
{
    Q_OBJECT
public:
    explicit DailyRollingFileAppenderTest(QObject * parent = nullptr) :
                                QObject(parent),
                                mLogDirectory(nullptr),
                                mAppender(nullptr) {}
private Q_SLOTS:
    void init();
    void cleanup();

    void testFileCreation_data();
    void testFileCreation();
    void testAppend();
    void testRollOver();
    void testObsoleteLogFileDeletion();

private:
    QTemporaryDir *mLogDirectory;
    QDate mMockDate;
    DailyRollingFileAppender *mAppender;
};

void DailyRollingFileAppenderTest::init()
{
    mLogDirectory = new QTemporaryDir;
    mMockDate = QDate(2019, 1, 15);

    Log4Qt::DateTime::setProvider([this]() { return QDateTime(mMockDate, QTime(0, 0)); });

    mAppender = new DailyRollingFileAppender;
    mAppender->setLayout(Log4Qt::LayoutSharedPtr(new Log4Qt::SimpleLayout));
}

void DailyRollingFileAppenderTest::cleanup()
{
    delete mAppender;
    delete mLogDirectory;  // destructor will remove temporary directory
    Log4Qt::DateTime::setProvider({});  // reset to real clock
}

void DailyRollingFileAppenderTest::testFileCreation_data()
{
    QTest::addColumn<QString>("appName");
    QTest::addColumn<QString>("datePattern");
    QTest::addColumn<QString>("fileName");

    QTest::newRow("default") << "app" << "_yyyy_MM_dd" << "app_2019_07_09.log";
    QTest::newRow("Austria") << "app" << "_dd.MM.yyyy" << "app_09.07.2019.log";
    QTest::newRow("service") << "srv" << "_yyyy_MM_dd" << "srv_2019_07_09.log";
}

void DailyRollingFileAppenderTest::testFileCreation()
{
    mMockDate = QDate(2019, 7, 9);

    QFETCH(QString, appName);
    QFETCH(QString, datePattern);
    QFETCH(QString, fileName);

    mAppender->setDatePattern(datePattern);
    mAppender->setFile(mLogDirectory->path() + QLatin1Char('/') + appName +  QStringLiteral(".log"));

    mAppender->activateOptions();

    const QFileInfo fileInfo(mAppender->file());

    QVERIFY(fileInfo.exists());

    QCOMPARE(fileInfo.fileName(), fileName);
}

void DailyRollingFileAppenderTest::testAppend()
{
    mAppender->setFile(mLogDirectory->path() + QLatin1Char('/') + QStringLiteral("app.log"));
    mAppender->activateOptions();

    const auto fileName = mAppender->file();

    QVERIFY(QFileInfo::exists(fileName));

    const QFile logFile(fileName);

    // nothing written yet
    QCOMPARE(logFile.size(), 0);

    mAppender->doAppend(Log4Qt::LoggingEvent());

    QVERIFY(logFile.size() > 0);
}

void DailyRollingFileAppenderTest::testRollOver()
{
    mAppender->setFile(mLogDirectory->path() + QLatin1Char('/') + QStringLiteral("app.log"));
    mAppender->activateOptions();

    mAppender->doAppend(Log4Qt::LoggingEvent());

    const auto fileNameDay1 = mAppender->file();
    QVERIFY(QFileInfo::exists(fileNameDay1));

    // one day has passed ...
    mMockDate = mMockDate.addDays(1);

    // ... and when we try to append ...
    mAppender->doAppend(Log4Qt::LoggingEvent());

    // ... we get a new log file
    const auto fileNameDay2 = mAppender->file();

    QVERIFY(QFileInfo::exists(fileNameDay2));
    QVERIFY(fileNameDay1 != fileNameDay2);
}

namespace
{

void createFile(const QString& fileName)
{
    QFile file(fileName);
    QVERIFY(file.open(QFile::WriteOnly));
    file.close();
    QVERIFY2(file.exists(), qPrintable(fileName));
}

}

void DailyRollingFileAppenderTest::testObsoleteLogFileDeletion()
{
    const QString deleteOnActivateFileName = mLogDirectory->path() + QLatin1Char('/') + QStringLiteral("app_2019_01_05.log");

    createFile(deleteOnActivateFileName);

    const QString deleteAfterOneDayFileName = mLogDirectory->path() + QLatin1Char('/') + QStringLiteral("app_2019_01_06.log");

    createFile(deleteAfterOneDayFileName);

    const QString alwaysKeptFileName = mLogDirectory->path() + QLatin1Char('/') + QStringLiteral("app_2019_01_07.log");

    createFile(alwaysKeptFileName);

    mMockDate = QDate(2019, 1, 10);

    mAppender->setFile(mLogDirectory->path() + QLatin1Char('/') + QStringLiteral("app.log"));

    mAppender->setDatePattern(QStringLiteral("_yyyy_MM_dd"));
    mAppender->setKeepDays(4);

    // after configuration ...
    mAppender->activateOptions();

    // ... we delete obsolete files
    QVERIFY(!QFileInfo::exists(deleteOnActivateFileName));
    QVERIFY(QFileInfo::exists(deleteAfterOneDayFileName));
    QVERIFY(QFileInfo::exists(alwaysKeptFileName));

    // appending later today ...
    mAppender->doAppend(Log4Qt::LoggingEvent());

    // ... does not delete anything
    QVERIFY(QFileInfo::exists(deleteAfterOneDayFileName));
    QVERIFY(QFileInfo::exists(alwaysKeptFileName));

    // one day has passed ...
    mMockDate = mMockDate.addDays(1);

    // ... and we append additional messages ...
    mAppender->doAppend(Log4Qt::LoggingEvent());

    // ... one file becomes obsolete and is deleted automatically
    // Since deletion takes place in a separate thread, we would need to sleep here. To avoid that,
    // we rely on the appender to wait for completion in its destructor
    delete mAppender;
    mAppender = nullptr;
    QVERIFY(!QFileInfo::exists(deleteAfterOneDayFileName));
    QVERIFY(QFileInfo::exists(alwaysKeptFileName));
}

QTEST_GUILESS_MAIN(DailyRollingFileAppenderTest)

#include "dailyrollingfileappendertest.moc"
