/******************************************************************************
 *
 * package:     Log4Qt
 * file:        performancetest.cpp
 * created:     February 2026
 * author:      Performance Test
 *
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

#include "performancetest.h"
#include "log4qt/logger.h"
#include "log4qt/logmanager.h"
#include "log4qt/fileappender.h"
#include "log4qt/patternlayout.h"
#include "log4qt/simplelayout.h"
#include "log4qt/varia/nullappender.h"
#include "log4qt/varia/levelmatchfilter.h"
#include "log4qt/helpers/datetime.h"
#include "log4qt/helpers/timestampprovider.h"
#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QThread>
#include <QThreadPool>
#include <QRunnable>
#include <QElapsedTimer>
#include <QDebug>

void PerformanceTest::initTestCase()
{
    // Create temporary directory for test files
    QTemporaryDir tempDir;
    tempDir.setAutoRemove(false);
    mTestDir = tempDir.path();
    qDebug() << "Test directory:" << mTestDir;
}

void PerformanceTest::cleanupTestCase()
{
    // Clean up test directory
    QDir dir(mTestDir);
    if (dir.exists())
    {
        dir.removeRecursively();
    }
}

void PerformanceTest::init()
{
    // Reset logging before each test
    Log4Qt::LogManager::resetConfiguration();
}

void PerformanceTest::cleanup()
{
    // Reset logging after each test
    Log4Qt::LogManager::resetConfiguration();
}

void PerformanceTest::testSimpleLoggingPerformance_data()
{
    QTest::addColumn<int>("iterations");
    QTest::addColumn<QString>("level");
    
    QTest::newRow("1000 DEBUG messages") << 1000 << "DEBUG";
    QTest::newRow("10000 DEBUG messages") << 10000 << "DEBUG";
    QTest::newRow("100000 DEBUG messages") << 100000 << "DEBUG";
    QTest::newRow("1000 INFO messages") << 1000 << "INFO";
    QTest::newRow("10000 INFO messages") << 10000 << "INFO";
}

void PerformanceTest::testSimpleLoggingPerformance()
{
    QFETCH(int, iterations);
    QFETCH(QString, level);
    
    // Setup logger with null appender (no I/O overhead)
    auto logger = Log4Qt::Logger::rootLogger();
    auto nullAppender = new Log4Qt::NullAppender();
    nullAppender->setName("NullAppender");
    nullAppender->activateOptions();
    logger->addAppender(nullAppender);
    
    if (level == "DEBUG")
        logger->setLevel(Log4Qt::Level::DEBUG_INT);
    else if (level == "INFO")
        logger->setLevel(Log4Qt::Level::INFO_INT);
    
    // Benchmark logging
    QBENCHMARK
    {
        for (int i = 0; i < iterations; ++i)
        {
            if (level == "DEBUG")
                logger->debug("Test message %1", i);
            else
                logger->info("Test message %1", i);
        }
    }
    
    logger->removeAllAppenders();
}

void PerformanceTest::testFileAppenderPerformance_data()
{
    QTest::addColumn<int>("iterations");
    QTest::addColumn<bool>("immediateFlush");
    QTest::addColumn<QString>("layoutType");
    
    QTest::newRow("1000 msgs, immediate flush, simple layout") << 1000 << true << "simple";
    QTest::newRow("1000 msgs, buffered, simple layout") << 1000 << false << "simple";
    QTest::newRow("10000 msgs, immediate flush, simple layout") << 10000 << true << "simple";
    QTest::newRow("10000 msgs, buffered, simple layout") << 10000 << false << "simple";
    QTest::newRow("1000 msgs, immediate flush, pattern layout") << 1000 << true << "pattern";
    QTest::newRow("1000 msgs, buffered, pattern layout") << 1000 << false << "pattern";
}

void PerformanceTest::testFileAppenderPerformance()
{
    QFETCH(int, iterations);
    QFETCH(bool, immediateFlush);
    QFETCH(QString, layoutType);
    
    QString testFile = mTestDir + "/performance_test.log";
    
    // Setup logger with file appender
    auto logger = Log4Qt::Logger::rootLogger();
    logger->setLevel(Log4Qt::Level::DEBUG_INT);
    
    auto fileAppender = new Log4Qt::FileAppender();
    fileAppender->setName("FileAppender");
    fileAppender->setFile(testFile);
    fileAppender->setImmediateFlush(immediateFlush);
    
    if (layoutType == "simple")
    {
        auto layout = new Log4Qt::SimpleLayout();
        fileAppender->setLayout(layout);
    }
    else if (layoutType == "pattern")
    {
        auto layout = new Log4Qt::PatternLayout();
        layout->setConversionPattern("%d{ISO8601} [%t] %-5p %c - %m%n");
        fileAppender->setLayout(layout);
    }
    
    fileAppender->activateOptions();
    logger->addAppender(fileAppender);
    
    // Benchmark logging
    QBENCHMARK
    {
        for (int i = 0; i < iterations; ++i)
        {
            logger->info("Performance test message number %1", i);
        }
    }
    
    logger->removeAllAppenders();
    
    // Clean up test file
    QFile::remove(testFile);
}

// Worker class for multi-threaded logging test
class LoggingWorker : public QRunnable
{
public:
    LoggingWorker(Log4Qt::Logger *logger, int iterations, int threadId)
        : mLogger(logger), mIterations(iterations), mThreadId(threadId)
    {
    }
    
    void run() override
    {
        for (int i = 0; i < mIterations; ++i)
        {
            mLogger->info("Thread %1 - Message %2", mThreadId, i);
        }
    }
    
private:
    Log4Qt::Logger *mLogger;
    int mIterations;
    int mThreadId;
};

void PerformanceTest::testMultiThreadedLoggingPerformance_data()
{
    QTest::addColumn<int>("threadCount");
    QTest::addColumn<int>("messagesPerThread");
    
    QTest::newRow("2 threads, 1000 msgs each") << 2 << 1000;
    QTest::newRow("4 threads, 1000 msgs each") << 4 << 1000;
    QTest::newRow("8 threads, 1000 msgs each") << 8 << 1000;
    QTest::newRow("4 threads, 5000 msgs each") << 4 << 5000;
}

void PerformanceTest::testMultiThreadedLoggingPerformance()
{
    QFETCH(int, threadCount);
    QFETCH(int, messagesPerThread);
    
    QString testFile = mTestDir + "/multithread_test.log";
    
    // Setup logger with file appender
    auto logger = Log4Qt::Logger::rootLogger();
    logger->setLevel(Log4Qt::Level::INFO_INT);
    
    auto fileAppender = new Log4Qt::FileAppender();
    fileAppender->setName("FileAppender");
    fileAppender->setFile(testFile);
    fileAppender->setImmediateFlush(false); // Use buffering for better performance
    
    auto layout = new Log4Qt::SimpleLayout();
    fileAppender->setLayout(layout);
    fileAppender->activateOptions();
    logger->addAppender(fileAppender);
    
    // Benchmark multi-threaded logging
    QBENCHMARK
    {
        QThreadPool pool;
        pool.setMaxThreadCount(threadCount);
        
        for (int i = 0; i < threadCount; ++i)
        {
            auto worker = new LoggingWorker(logger, messagesPerThread, i);
            worker->setAutoDelete(true);
            pool.start(worker);
        }
        
        pool.waitForDone();
    }
    
    logger->removeAllAppenders();
    
    // Clean up test file
    QFile::remove(testFile);
}

void PerformanceTest::testFormattingPerformance_data()
{
    QTest::addColumn<QString>("pattern");
    QTest::addColumn<int>("iterations");
    
    QTest::newRow("Simple pattern, 10000 msgs") << "%m%n" << 10000;
    QTest::newRow("Basic pattern, 10000 msgs") << "%d %p %c - %m%n" << 10000;
    QTest::newRow("Complex pattern, 10000 msgs") << "%d{ISO8601} [%t] %-5p %c{2} - %m%n" << 10000;
    QTest::newRow("Very complex pattern, 10000 msgs") << "%d{yyyy-MM-dd HH:mm:ss.zzz} [%t] %-5p %c - %F:%L - %m%n" << 10000;
}

void PerformanceTest::testFormattingPerformance()
{
    QFETCH(QString, pattern);
    QFETCH(int, iterations);
    
    // Setup logger with null appender but with pattern layout
    auto logger = Log4Qt::Logger::rootLogger();
    logger->setLevel(Log4Qt::Level::DEBUG_INT);
    
    auto nullAppender = new Log4Qt::NullAppender();
    nullAppender->setName("NullAppender");
    
    auto layout = new Log4Qt::PatternLayout();
    layout->setConversionPattern(pattern);
    nullAppender->setLayout(layout);
    nullAppender->activateOptions();
    
    logger->addAppender(nullAppender);
    
    // Benchmark formatting
    QBENCHMARK
    {
        for (int i = 0; i < iterations; ++i)
        {
            logger->debug("Test message with parameter: %1", i);
        }
    }
    
    logger->removeAllAppenders();
}

void PerformanceTest::testFilteringPerformance_data()
{
    QTest::addColumn<int>("filterCount");
    QTest::addColumn<int>("iterations");
    
    QTest::newRow("No filters, 10000 msgs") << 0 << 10000;
    QTest::newRow("1 filter, 10000 msgs") << 1 << 10000;
    QTest::newRow("3 filters, 10000 msgs") << 3 << 10000;
    QTest::newRow("5 filters, 10000 msgs") << 5 << 10000;
}

void PerformanceTest::testFilteringPerformance()
{
    QFETCH(int, filterCount);
    QFETCH(int, iterations);
    
    // Setup logger with null appender and filters
    auto logger = Log4Qt::Logger::rootLogger();
    logger->setLevel(Log4Qt::Level::DEBUG_INT);
    
    auto nullAppender = new Log4Qt::NullAppender();
    nullAppender->setName("NullAppender");
    
    // Add filters
    for (int i = 0; i < filterCount; ++i)
    {
        auto filter = new Log4Qt::LevelMatchFilter();
        filter->setLevelToMatch(Log4Qt::Level::INFO_INT);
        filter->setAcceptOnMatch(true);
        nullAppender->addFilter(filter);
    }
    
    nullAppender->activateOptions();
    logger->addAppender(nullAppender);
    
    // Benchmark with filtering
    QBENCHMARK
    {
        for (int i = 0; i < iterations; ++i)
        {
            logger->info("Test message %1", i);
        }
    }
    
    logger->removeAllAppenders();
}

void PerformanceTest::testTimestampCacheWindowPerformance_data()
{
    QTest::addColumn<int>("cacheWindowMs");
    QTest::addColumn<int>("messageCount");
    
    // Test different cache windows with various message counts
    QTest::newRow("0ms (no cache), 1000 msgs") << 0 << 1000;
    QTest::newRow("1ms (default), 1000 msgs") << 1 << 1000;
    QTest::newRow("10ms, 1000 msgs") << 10 << 1000;
    QTest::newRow("100ms, 1000 msgs") << 100 << 1000;
    
    QTest::newRow("0ms (no cache), 10000 msgs") << 0 << 10000;
    QTest::newRow("1ms (default), 10000 msgs") << 1 << 10000;
    QTest::newRow("10ms, 10000 msgs") << 10 << 10000;
    QTest::newRow("100ms, 10000 msgs") << 100 << 10000;
    
    QTest::newRow("0ms (no cache), 100000 msgs") << 0 << 100000;
    QTest::newRow("1ms (default), 100000 msgs") << 1 << 100000;
    QTest::newRow("10ms, 100000 msgs") << 10 << 100000;
    QTest::newRow("100ms, 100000 msgs") << 100 << 100000;
}

void PerformanceTest::testTimestampCacheWindowPerformance()
{
    QFETCH(int, cacheWindowMs);
    QFETCH(int, messageCount);
    
    // Configure timestamp cache window
    Log4Qt::TimestampProvider::setCacheWindow(cacheWindowMs);
    
    // Create a null appender (discards all output - pure LoggingEvent creation benchmark)
    auto logger = Log4Qt::Logger::rootLogger();
    logger->setLevel(Log4Qt::Level::DEBUG_INT);
    logger->removeAllAppenders();
    
    auto nullAppender = new Log4Qt::NullAppender();
    nullAppender->setName("NullAppender");
    nullAppender->activateOptions();
    logger->addAppender(nullAppender);
    
    // Benchmark pure logging event creation with different cache windows
    QBENCHMARK
    {
        for (int i = 0; i < messageCount; ++i)
        {
            logger->debug("Timestamp benchmark message %1", i);
        }
    }
    
    logger->removeAllAppenders();
    
    // Reset to default
    Log4Qt::TimestampProvider::setCacheWindow(1);
}

QTEST_MAIN(PerformanceTest)
#include "performancetest.moc"
