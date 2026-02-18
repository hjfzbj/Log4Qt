#include "qstring_benchmark.h"
#include <QString>
#include <QStringView>
#include <QTest>

// Benchmark test to measure QString copy overhead and optimizations
void QStringBenchmark::initTestCase()
{
    // Setup test data
    for (int i = 0; i < 1000; ++i)
    {
        testStrings.append(QString("Test log message number %1 with some additional text").arg(i));
    }
}

void QStringBenchmark::benchmarkQStringCopy()
{
    QBENCHMARK
    {
        for (const QString &str : testStrings)
        {
            QString copy = str; // Deep copy
            QString copy2 = copy; // Another deep copy
            Q_UNUSED(copy2);
        }
    }
}

void QStringBenchmark::benchmarkQStringConstRef()
{
    QBENCHMARK
    {
        for (const QString &str : testStrings)
        {
            const QString &ref1 = str; // No copy
            const QString &ref2 = ref1; // No copy
            Q_UNUSED(ref2);
        }
    }
}

void QStringBenchmark::benchmarkQStringView()
{
    QBENCHMARK
    {
        for (const QString &str : testStrings)
        {
            QStringView view1 = str; // Lightweight view
            QStringView view2 = view1; // Lightweight copy
            Q_UNUSED(view2);
        }
    }
}

void QStringBenchmark::benchmarkQStringMove()
{
    QBENCHMARK
    {
        QList<QString> strings = testStrings;
        for (int i = 0; i < strings.size(); ++i)
        {
            QString moved = std::move(strings[i]); // Move semantics
            Q_UNUSED(moved);
        }
    }
}

void QStringBenchmark::benchmarkLoggingEventCopy()
{
    // Simulate LoggingEvent construction with QString copies
    QBENCHMARK
    {
        for (const QString &str : testStrings)
        {
            QString message = str;
            QString ndc = QString("NDC.Context");
            QString threadName = QString("ThreadName");
            QString categoryName = QString("Category");
            
            // Simulate 4 QString copies per LoggingEvent
            Q_UNUSED(message);
            Q_UNUSED(ndc);
            Q_UNUSED(threadName);
            Q_UNUSED(categoryName);
        }
    }
}

void QStringBenchmark::benchmarkLoggingEventMove()
{
    // Simulate LoggingEvent construction with move semantics
    QBENCHMARK
    {
        for (const QString &str : testStrings)
        {
            QString message = str;
            QString ndc = QString("NDC.Context");
            QString threadName = QString("ThreadName");
            QString categoryName = QString("Category");
            
            // Simulate move construction
            QString movedMessage = std::move(message);
            QString movedNdc = std::move(ndc);
            QString movedThread = std::move(threadName);
            QString movedCategory = std::move(categoryName);
            
            Q_UNUSED(movedMessage);
            Q_UNUSED(movedNdc);
            Q_UNUSED(movedThread);
            Q_UNUSED(movedCategory);
        }
    }
}

void QStringBenchmark::benchmarkStringConcatenation()
{
    QBENCHMARK
    {
        for (int i = 0; i < 100; ++i)
        {
            QString result = QString("Level: ") + QString("INFO") + 
                           QString(" - Message: ") + testStrings[i % testStrings.size()];
            Q_UNUSED(result);
        }
    }
}

void QStringBenchmark::benchmarkStringBuilder()
{
    QBENCHMARK
    {
        for (int i = 0; i < 100; ++i)
        {
            QString result = QStringLiteral("Level: ") % QStringLiteral("INFO") % 
                           QStringLiteral(" - Message: ") % testStrings[i % testStrings.size()];
            Q_UNUSED(result);
        }
    }
}

void QStringBenchmark::benchmarkQLatin1String()
{
    QBENCHMARK
    {
        for (int i = 0; i < 100; ++i)
        {
            QString result = QLatin1String("Level: ") + QLatin1String("INFO") + 
                           QLatin1String(" - Message: ") + testStrings[i % testStrings.size()];
            Q_UNUSED(result);
        }
    }
}

void QStringBenchmark::benchmarkReserveCapacity()
{
    QBENCHMARK
    {
        for (int i = 0; i < 100; ++i)
        {
            QString result;
            result.reserve(100); // Pre-allocate
            result += "Level: ";
            result += "INFO";
            result += " - Message: ";
            result += testStrings[i % testStrings.size()];
            Q_UNUSED(result);
        }
    }
}

QTEST_MAIN(QStringBenchmark)
