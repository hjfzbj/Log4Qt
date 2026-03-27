#ifndef QSTRINGBENCHMARK_H
#define QSTRINGBENCHMARK_H

#include <QObject>
#include <QString>
#include <QList>

class QStringBenchmark : public QObject
{
    Q_OBJECT

public:
    QStringBenchmark() = default;

private slots:
    void initTestCase();
    
    // Basic QString operations
    void benchmarkQStringCopy();
    void benchmarkQStringConstRef();
    void benchmarkQStringView();
    void benchmarkQStringMove();
    
    // LoggingEvent simulation
    void benchmarkLoggingEventCopy();
    void benchmarkLoggingEventMove();
    
    // String concatenation
    void benchmarkStringConcatenation();
    void benchmarkStringBuilder();
    void benchmarkQLatin1String();
    void benchmarkReserveCapacity();

private:
    QList<QString> testStrings;
};

#endif // QSTRINGBENCHMARK_H
