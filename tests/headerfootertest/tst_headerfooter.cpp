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

#include <QBuffer>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>
#include <QtTest>

#include "log4qt/fileappender.h"
#include "log4qt/helpers/datetime.h"
#include "log4qt/loggingevent.h"
#include "log4qt/logmanager.h"
#include "log4qt/patternlayout.h"
#include "log4qt/randomaccessfileappender.h"
#include "log4qt/rollingfileappender.h"
#include "log4qt/spi/defaultrolloverstrategy.h"
#include "log4qt/spi/headerfooterprovider.h"
#include "log4qt/spi/onstartuptriggeringpolicy.h"
#include "log4qt/spi/triggeringpolicy.h"
#include "log4qt/writerappender.h"

using namespace Log4Qt;

// Exposes rollOver() so tests can trigger a rollover directly without needing
// a triggering policy or a minimum file size.
class RollableFileAppender : public RollingFileAppender
{
public:
    using RollingFileAppender::RollingFileAppender;
    void triggerRollover() { rollOver(); }
};

class HeaderFooterTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanup();

    // --- PatternLayout: headerPattern / footerPattern ---
    void patternLayout_headerPattern_literal();
    void patternLayout_footerPattern_literal();
    void patternLayout_headerPattern_overridesStaticHeader();
    void patternLayout_headerPattern_clearFallsBackToStaticHeader();
    void patternLayout_headerPattern_withDate();
    void patternLayout_footerPattern_withDate();

    // --- WriterAppender: static and pattern header/footer via in-memory buffer ---
    void writerAppender_header_writtenOnSetWriter();
    void writerAppender_footer_writtenOnClose();
    void writerAppender_emptyHeader_notWritten();
    void writerAppender_patternHeader_writtenAsFormattedString();

    // --- FileAppender: append-mode guard ---
    void fileAppender_header_writtenForNewFile_appendMode();
    void fileAppender_header_skippedForExistingNonEmptyFile_appendMode();
    void fileAppender_header_writtenAfterTruncate();
    void fileAppender_footer_writtenOnClose();

    // --- RandomAccessFileAppender: endOfLine consistency + append-mode guard ---
    void randomAccessFileAppender_header_hasTrailingNewline();
    void randomAccessFileAppender_footer_hasTrailingNewline();
    void randomAccessFileAppender_header_skippedForExistingNonEmptyFile_appendMode();

    // --- RollingFileAppender: footer/header across rollover ---
    void rollingFileAppender_rollover_writesFooterToOldAndHeaderToNew();
    void rollingFileAppender_startupRollover_withSkipFooter_suppressesFooter();
    void rollingFileAppender_startupRollover_withoutSkipFooter_writesFooter();

    // --- PatternHeaderFooterProvider: %P{key} user properties ---
    void patternProvider_staticProperty_resolvedInHeader();
    void patternProvider_staticProperty_resolvedInFooter();
    void patternProvider_staticProperty_setBeforePattern();
    void patternProvider_staticProperty_updatedAfterPatternSet();
    void patternProvider_dynamicProperty_resolvedInHeader();
    void patternProvider_property_missingKeyExpandsToEmpty();
    void patternProvider_property_formattingWidthApplied();
    void patternProvider_property_combinedWithDate();

    // --- HeaderFooterProvider ---
    void provider_globalProvider_usedAsFallback();
    void provider_globalProvider_notUsedWhenStaticIsSet();
    void provider_globalProvider_footerFallback();
    void provider_perLayoutProvider_overridesStaticHeader();
    void provider_perLayoutProvider_overridesHeaderPattern();
    void provider_perLayoutProvider_overridesGlobal();
    void provider_patternProvider_formatsDateInHeader();
    void provider_globalProvider_writtenToFile();
    void provider_propertyProvider_writtenToFile();

private:
    QTemporaryDir mTmpDir;

    QString tempFile(const QString &name) const
    {
        return mTmpDir.path() + QLatin1Char('/') + name;
    }

    static QByteArray readFileBytes(const QString &path)
    {
        QFile f(path);
        // Open in text mode so that platform line endings (CRLF on Windows) are
        // normalised to LF on read, matching the \n literals in the assertions.
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
            return {};
        return f.readAll();
    }

    static void writeFileContent(const QString &path, const QByteArray &content)
    {
        QFile f(path);
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write(content);
    }
};

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

void HeaderFooterTest::initTestCase()
{
    // Pre-initialize the LogManager so its doStartup() filesystem scan runs here
    // (once, before any test), not inside the first PatternFormatter construction
    // which would cause the first test function to time out.
    LogManager::resetConfiguration();

    // Disable the 1ms thread-local timestamp cache for the duration of the test
    // suite. Without this, setProvider() mocks would not take effect immediately:
    // the cached real timestamp would be returned until the 1ms window expired.
    DateTime::setCacheWindow(0);
}

void HeaderFooterTest::cleanup()
{
    DateTime::setProvider({});                            // restore real clock
    AbstractLayout::setGlobalHeaderFooterProvider({});    // clear global provider
    LogManager::resetConfiguration();                     // drop any registered appenders/loggers
}

// ---------------------------------------------------------------------------
// PatternLayout: headerPattern / footerPattern
// ---------------------------------------------------------------------------

void HeaderFooterTest::patternLayout_headerPattern_literal()
{
    PatternLayout layout;
    layout.setHeaderPattern(QStringLiteral("APP START"));
    QCOMPARE(layout.header(), QStringLiteral("APP START"));
}

void HeaderFooterTest::patternLayout_footerPattern_literal()
{
    PatternLayout layout;
    layout.setFooterPattern(QStringLiteral("APP END"));
    QCOMPARE(layout.footer(), QStringLiteral("APP END"));
}

void HeaderFooterTest::patternLayout_headerPattern_overridesStaticHeader()
{
    PatternLayout layout;
    layout.setHeader(QStringLiteral("STATIC"));
    layout.setHeaderPattern(QStringLiteral("PATTERN"));

    // When headerPattern is set it takes precedence over the plain header.
    QCOMPARE(layout.header(), QStringLiteral("PATTERN"));
}

void HeaderFooterTest::patternLayout_headerPattern_clearFallsBackToStaticHeader()
{
    PatternLayout layout;
    layout.setHeader(QStringLiteral("STATIC"));
    layout.setHeaderPattern(QStringLiteral("PATTERN"));
    layout.setHeaderPattern(QString());  // clear the pattern

    // Plain header is restored as the result.
    QCOMPARE(layout.header(), QStringLiteral("STATIC"));
}

void HeaderFooterTest::patternLayout_headerPattern_withDate()
{
    DateTime::setProvider([]() {
        return QDateTime(QDate(2026, 1, 15), QTime(10, 30, 0));
    });

    PatternLayout layout;
    layout.setHeaderPattern(QStringLiteral("Started %d{yyyy-MM-dd}"));

    // %d is evaluated at the moment header() is called — i.e. at file-open time.
    QCOMPARE(layout.header(), QStringLiteral("Started 2026-01-15"));
}

void HeaderFooterTest::patternLayout_footerPattern_withDate()
{
    DateTime::setProvider([]() {
        return QDateTime(QDate(2026, 1, 15), QTime(10, 30, 0));
    });

    PatternLayout layout;
    layout.setFooterPattern(QStringLiteral("Stopped %d{HH:mm}"));

    QCOMPARE(layout.footer(), QStringLiteral("Stopped 10:30"));
}

// ---------------------------------------------------------------------------
// WriterAppender: in-memory buffer tests
// ---------------------------------------------------------------------------

void HeaderFooterTest::writerAppender_header_writtenOnSetWriter()
{
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    auto layout = LayoutSharedPtr(new PatternLayout(QStringLiteral("%m%n")));
    layout->setHeader(QStringLiteral("HEADER"));

    QTextStream stream(&buffer);
    WriterAppender appender(layout);
    appender.setWriter(&stream);
    stream.flush();

    QVERIFY(QString::fromUtf8(buffer.data()).startsWith(QStringLiteral("HEADER\n")));

    appender.setWriter(nullptr);  // prevent destructor from using dangling stream pointer
}

void HeaderFooterTest::writerAppender_footer_writtenOnClose()
{
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    auto layout = LayoutSharedPtr(new PatternLayout(QStringLiteral("%m%n")));
    layout->setFooter(QStringLiteral("FOOTER"));

    QTextStream stream(&buffer);
    WriterAppender appender(layout);
    appender.setWriter(&stream);
    appender.setWriter(nullptr);  // triggers closeWriter() → writeFooter()
    stream.flush();               // flush footer bytes into the QBuffer

    QVERIFY(QString::fromUtf8(buffer.data()).contains(QStringLiteral("FOOTER\n")));
}

void HeaderFooterTest::writerAppender_emptyHeader_notWritten()
{
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    auto layout = LayoutSharedPtr(new PatternLayout(QStringLiteral("%m%n")));
    // No header or footer set.

    QTextStream stream(&buffer);
    WriterAppender appender(layout);
    appender.setWriter(&stream);
    stream.flush();

    QCOMPARE(buffer.size(), qint64(0));

    appender.setWriter(nullptr);
}

void HeaderFooterTest::writerAppender_patternHeader_writtenAsFormattedString()
{
    // Verifies the full pipeline: PatternLayout::header() formats the pattern,
    // and WriterAppender writes the result to the underlying stream.
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    auto *patternLayout = new PatternLayout(QStringLiteral("%m%n"));
    patternLayout->setHeaderPattern(QStringLiteral("-- log start --"));
    auto layout = LayoutSharedPtr(patternLayout);

    QTextStream stream(&buffer);
    WriterAppender appender(layout);
    appender.setWriter(&stream);
    stream.flush();

    QVERIFY(QString::fromUtf8(buffer.data()).startsWith(QStringLiteral("-- log start --\n")));

    appender.setWriter(nullptr);
}

// ---------------------------------------------------------------------------
// FileAppender: append-mode guard
// ---------------------------------------------------------------------------

void HeaderFooterTest::fileAppender_header_writtenForNewFile_appendMode()
{
    // In append mode on a brand-new (empty) file the header MUST be written.
    const QString path = tempFile(QStringLiteral("fa_new.log"));
    auto layout = LayoutSharedPtr(new PatternLayout(QStringLiteral("%m%n")));
    layout->setHeader(QStringLiteral("HEADER"));

    FileAppender appender(layout, path, /*append=*/true);
    appender.activateOptions();
    appender.close();

    QVERIFY(readFileBytes(path).startsWith("HEADER\n"));
}

void HeaderFooterTest::fileAppender_header_skippedForExistingNonEmptyFile_appendMode()
{
    // In append mode on a non-empty existing file the header must NOT be written
    // again — it was already written during the previous run.
    const QString path = tempFile(QStringLiteral("fa_existing.log"));
    writeFileContent(path, "existing content\n");

    auto layout = LayoutSharedPtr(new PatternLayout(QStringLiteral("%m%n")));
    layout->setHeader(QStringLiteral("HEADER"));

    FileAppender appender(layout, path, /*append=*/true);
    appender.activateOptions();
    appender.close();

    const QByteArray content = readFileBytes(path);
    QVERIFY(!content.contains("HEADER"));
    QVERIFY(content.startsWith("existing content\n"));
}

void HeaderFooterTest::fileAppender_header_writtenAfterTruncate()
{
    // In truncate (non-append) mode the file is always freshly created, so the
    // header must always be written regardless of pre-existing content.
    const QString path = tempFile(QStringLiteral("fa_truncate.log"));
    writeFileContent(path, "old content\n");

    auto layout = LayoutSharedPtr(new PatternLayout(QStringLiteral("%m%n")));
    layout->setHeader(QStringLiteral("HEADER"));

    FileAppender appender(layout, path, /*append=*/false);
    appender.activateOptions();
    appender.close();

    QVERIFY(readFileBytes(path).startsWith("HEADER\n"));
}

void HeaderFooterTest::fileAppender_footer_writtenOnClose()
{
    const QString path = tempFile(QStringLiteral("fa_footer.log"));
    auto layout = LayoutSharedPtr(new PatternLayout(QStringLiteral("%m%n")));
    layout->setFooter(QStringLiteral("FOOTER"));

    FileAppender appender(layout, path);
    appender.activateOptions();
    appender.doAppend(LoggingEvent());
    appender.close();

    QVERIFY(readFileBytes(path).endsWith("FOOTER\n"));
}

// ---------------------------------------------------------------------------
// RandomAccessFileAppender: endOfLine consistency + append-mode guard
// ---------------------------------------------------------------------------

void HeaderFooterTest::randomAccessFileAppender_header_hasTrailingNewline()
{
    // WriterAppender appends endOfLine() after the header string.
    // RandomAccessFileAppender must do the same so headers are on their own line.
    const QString path = tempFile(QStringLiteral("rafa_header.log"));
    auto layout = LayoutSharedPtr(new PatternLayout(QStringLiteral("%m%n")));
    layout->setHeader(QStringLiteral("HEADER"));

    RandomAccessFileAppender appender(layout, path);
    appender.activateOptions();
    appender.doAppend(LoggingEvent());
    appender.close();

    QVERIFY(readFileBytes(path).startsWith("HEADER\n"));
}

void HeaderFooterTest::randomAccessFileAppender_footer_hasTrailingNewline()
{
    const QString path = tempFile(QStringLiteral("rafa_footer.log"));
    auto layout = LayoutSharedPtr(new PatternLayout(QStringLiteral("%m%n")));
    layout->setFooter(QStringLiteral("FOOTER"));

    RandomAccessFileAppender appender(layout, path);
    appender.activateOptions();
    appender.doAppend(LoggingEvent());
    appender.close();

    QVERIFY(readFileBytes(path).endsWith("FOOTER\n"));
}

void HeaderFooterTest::randomAccessFileAppender_header_skippedForExistingNonEmptyFile_appendMode()
{
    const QString path = tempFile(QStringLiteral("rafa_existing.log"));
    writeFileContent(path, "existing\n");

    auto layout = LayoutSharedPtr(new PatternLayout(QStringLiteral("%m%n")));
    layout->setHeader(QStringLiteral("HEADER"));

    RandomAccessFileAppender appender(layout, path, /*append=*/true);
    appender.activateOptions();
    appender.close();

    const QByteArray content = readFileBytes(path);
    QVERIFY(!content.contains("HEADER"));
    QVERIFY(content.startsWith("existing\n"));
}

// ---------------------------------------------------------------------------
// RollingFileAppender: footer / header across a rollover
// ---------------------------------------------------------------------------

void HeaderFooterTest::rollingFileAppender_rollover_writesFooterToOldAndHeaderToNew()
{
    const QString path = tempFile(QStringLiteral("rolling.log"));
    auto layout = LayoutSharedPtr(new PatternLayout(QStringLiteral("%m%n")));
    layout->setHeader(QStringLiteral("HEADER"));
    layout->setFooter(QStringLiteral("FOOTER"));

    RollableFileAppender appender(layout, path);
    appender.activateOptions();
    appender.doAppend(LoggingEvent());  // write content so the file is non-empty
    appender.triggerRollover();
    appender.close();

    // DefaultRolloverStrategy renames "rolling.log" → "rolling.log.1"
    const QByteArray oldContent = readFileBytes(path + QStringLiteral(".1"));
    QVERIFY(oldContent.contains("FOOTER\n"));

    // A fresh file is opened at the original path and gets a new header.
    const QByteArray newContent = readFileBytes(path);
    QVERIFY(newContent.startsWith("HEADER\n"));
}

void HeaderFooterTest::rollingFileAppender_startupRollover_withSkipFooter_suppressesFooter()
{
    // Scenario: previous run left a non-empty log file.
    // With skipFooterOnStartup=true the footer must NOT appear in the rolled-over file.
    const QString path = tempFile(QStringLiteral("startup_skip.log"));
    writeFileContent(path, "previous run content\n");

    auto layout = LayoutSharedPtr(new PatternLayout(QStringLiteral("%m%n")));
    layout->setHeader(QStringLiteral("HEADER"));
    layout->setFooter(QStringLiteral("FOOTER"));

    RollingFileAppender appender(layout, path, /*append=*/true);
    appender.setSkipFooterOnStartup(true);
    appender.setTriggeringPolicy(
        TriggeringPolicySharedPtr(new OnStartupTriggeringPolicy));
    appender.activateOptions();
    appender.close();

    // The old file must retain only the content from the previous run.
    const QByteArray oldContent = readFileBytes(path + QStringLiteral(".1"));
    QCOMPARE(oldContent, QByteArray("previous run content\n"));

    // The new file should start with the header.
    const QByteArray newContent = readFileBytes(path);
    QVERIFY(newContent.startsWith("HEADER\n"));
}

void HeaderFooterTest::rollingFileAppender_startupRollover_withoutSkipFooter_writesFooter()
{
    // Control test: with the default skipFooterOnStartup=false, the footer IS
    // written to the rolled-over file.
    const QString path = tempFile(QStringLiteral("startup_noskip.log"));
    writeFileContent(path, "previous run content\n");

    auto layout = LayoutSharedPtr(new PatternLayout(QStringLiteral("%m%n")));
    layout->setFooter(QStringLiteral("FOOTER"));

    RollingFileAppender appender(layout, path, /*append=*/true);
    // skipFooterOnStartup is false by default.
    appender.setTriggeringPolicy(
        TriggeringPolicySharedPtr(new OnStartupTriggeringPolicy));
    appender.activateOptions();
    appender.close();

    const QByteArray oldContent = readFileBytes(path + QStringLiteral(".1"));
    QVERIFY(oldContent.contains("FOOTER\n"));
}

// ---------------------------------------------------------------------------
// HeaderFooterProvider
// ---------------------------------------------------------------------------

void HeaderFooterTest::provider_globalProvider_usedAsFallback()
{
    // A global provider is consulted when the layout has no static header set.
    auto *p = new PatternHeaderFooterProvider;
    p->setHeaderPattern(QStringLiteral("GLOBAL HEADER"));
    AbstractLayout::setGlobalHeaderFooterProvider(HeaderFooterProviderSharedPtr(p));

    PatternLayout layout;
    // No static header or headerPattern on the layout.
    QCOMPARE(layout.header(), QStringLiteral("GLOBAL HEADER"));
}

void HeaderFooterTest::provider_globalProvider_notUsedWhenStaticIsSet()
{
    // The static header on the layout takes precedence over the global provider.
    auto *p = new PatternHeaderFooterProvider;
    p->setHeaderPattern(QStringLiteral("GLOBAL HEADER"));
    AbstractLayout::setGlobalHeaderFooterProvider(HeaderFooterProviderSharedPtr(p));

    PatternLayout layout;
    layout.setHeader(QStringLiteral("STATIC HEADER"));

    QCOMPARE(layout.header(), QStringLiteral("STATIC HEADER"));
}

void HeaderFooterTest::provider_globalProvider_footerFallback()
{
    // Global provider footer is used when no static footer is configured.
    auto *p = new PatternHeaderFooterProvider;
    p->setFooterPattern(QStringLiteral("GLOBAL FOOTER"));
    AbstractLayout::setGlobalHeaderFooterProvider(HeaderFooterProviderSharedPtr(p));

    PatternLayout layout;
    QCOMPARE(layout.footer(), QStringLiteral("GLOBAL FOOTER"));
}

void HeaderFooterTest::provider_perLayoutProvider_overridesStaticHeader()
{
    // A per-layout provider overrides the static header string.
    PatternLayout layout;
    layout.setHeader(QStringLiteral("STATIC HEADER"));

    auto *p = new PatternHeaderFooterProvider;
    p->setHeaderPattern(QStringLiteral("PROVIDER HEADER"));
    layout.setHeaderFooterProvider(HeaderFooterProviderSharedPtr(p));

    QCOMPARE(layout.header(), QStringLiteral("PROVIDER HEADER"));
}

void HeaderFooterTest::provider_perLayoutProvider_overridesHeaderPattern()
{
    // A per-layout provider takes priority over the PatternLayout headerPattern.
    PatternLayout layout;
    layout.setHeaderPattern(QStringLiteral("PATTERN HEADER"));

    auto *p = new PatternHeaderFooterProvider;
    p->setHeaderPattern(QStringLiteral("PROVIDER HEADER"));
    layout.setHeaderFooterProvider(HeaderFooterProviderSharedPtr(p));

    QCOMPARE(layout.header(), QStringLiteral("PROVIDER HEADER"));
}

void HeaderFooterTest::provider_perLayoutProvider_overridesGlobal()
{
    // A per-layout provider wins over the global provider.
    auto *global = new PatternHeaderFooterProvider;
    global->setHeaderPattern(QStringLiteral("GLOBAL HEADER"));
    AbstractLayout::setGlobalHeaderFooterProvider(HeaderFooterProviderSharedPtr(global));

    PatternLayout layout;
    auto *perLayout = new PatternHeaderFooterProvider;
    perLayout->setHeaderPattern(QStringLiteral("PER-LAYOUT HEADER"));
    layout.setHeaderFooterProvider(HeaderFooterProviderSharedPtr(perLayout));

    QCOMPARE(layout.header(), QStringLiteral("PER-LAYOUT HEADER"));
}

void HeaderFooterTest::provider_patternProvider_formatsDateInHeader()
{
    DateTime::setProvider([]() {
        return QDateTime(QDate(2026, 3, 10), QTime(8, 0, 0));
    });

    auto *p = new PatternHeaderFooterProvider;
    p->setHeaderPattern(QStringLiteral("Log %d{yyyy-MM-dd}"));
    AbstractLayout::setGlobalHeaderFooterProvider(HeaderFooterProviderSharedPtr(p));

    PatternLayout layout;
    QCOMPARE(layout.header(), QStringLiteral("Log 2026-03-10"));
}

void HeaderFooterTest::provider_globalProvider_writtenToFile()
{
    // End-to-end: the global provider header must appear in a file written by
    // a FileAppender whose layout has no static header.
    auto *p = new PatternHeaderFooterProvider;
    p->setHeaderPattern(QStringLiteral("FILE HEADER"));
    AbstractLayout::setGlobalHeaderFooterProvider(HeaderFooterProviderSharedPtr(p));

    const QString path = tempFile(QStringLiteral("provider_file.log"));
    auto layout = LayoutSharedPtr(new PatternLayout(QStringLiteral("%m%n")));
    // No static header on the layout — global provider must supply it.

    FileAppender appender(layout, path);
    appender.activateOptions();
    appender.close();

    QVERIFY(readFileBytes(path).startsWith("FILE HEADER\n"));
}

void HeaderFooterTest::provider_propertyProvider_writtenToFile()
{
    // End-to-end: a %P{key} property value must appear in the header written
    // to a real file. This exercises the full chain:
    //   TestPropertyProvider (Q_PROPERTY serialNumber)
    //   → PatternFormatter resolves %P{serialNumber} via QObject::property()
    //   → FileAppender writes the formatted header on open
    auto *p = new TestPropertyProvider;
    p->setHeaderPattern(QStringLiteral("S/N: %P{serialNumber}"));
    p->setSerialNumber(QStringLiteral("SN-FILE-001"));
    AbstractLayout::setGlobalHeaderFooterProvider(HeaderFooterProviderSharedPtr(p));

    const QString path = tempFile(QStringLiteral("property_provider_file.log"));
    auto layout = LayoutSharedPtr(new PatternLayout(QStringLiteral("%m%n")));

    FileAppender appender(layout, path);
    appender.activateOptions();
    appender.close();

    QVERIFY(readFileBytes(path).startsWith("S/N: SN-FILE-001\n"));
}

// ---------------------------------------------------------------------------
// PatternHeaderFooterProvider: %P{key} user properties
// ---------------------------------------------------------------------------

// Test helper: provider subclass with a Q_PROPERTY that %P{key} can resolve.
class TestPropertyProvider : public PatternHeaderFooterProvider
{
    Q_OBJECT
    Q_PROPERTY(QString serialNumber READ serialNumber WRITE setSerialNumber)
    Q_PROPERTY(QString label       READ label         WRITE setLabel)
public:
    using PatternHeaderFooterProvider::PatternHeaderFooterProvider;
    QString serialNumber() const { return mSerialNumber; }
    void setSerialNumber(const QString &v) { mSerialNumber = v; }
    QString label() const { return mLabel; }
    void setLabel(const QString &v) { mLabel = v; }
private:
    QString mSerialNumber;
    QString mLabel;
};

void HeaderFooterTest::patternProvider_staticProperty_resolvedInHeader()
{
    // %P{key} expands to the value of the matching Q_PROPERTY on the provider.
    TestPropertyProvider p;
    p.setHeaderPattern(QStringLiteral("S/N: %P{serialNumber}"));
    p.setSerialNumber(QStringLiteral("SN-001"));

    QCOMPARE(p.header(), QStringLiteral("S/N: SN-001"));
}

void HeaderFooterTest::patternProvider_staticProperty_resolvedInFooter()
{
    // %P{key} works in footer patterns too.
    TestPropertyProvider p;
    p.setFooterPattern(QStringLiteral("End S/N: %P{serialNumber}"));
    p.setSerialNumber(QStringLiteral("SN-002"));

    QCOMPARE(p.footer(), QStringLiteral("End S/N: SN-002"));
}

void HeaderFooterTest::patternProvider_staticProperty_setBeforePattern()
{
    // Property set BEFORE setHeaderPattern() is visible because setPropertySource(this)
    // is called when the formatter is created — the property-source pointer is this,
    // so the live value is always read at format time.
    TestPropertyProvider p;
    p.setSerialNumber(QStringLiteral("EARLY"));
    p.setHeaderPattern(QStringLiteral("S/N: %P{serialNumber}"));

    QCOMPARE(p.header(), QStringLiteral("S/N: EARLY"));
}

void HeaderFooterTest::patternProvider_staticProperty_updatedAfterPatternSet()
{
    // Changing the Q_PROPERTY after the pattern is set is reflected immediately —
    // the converter holds a pointer to mPropertySource (this) and calls
    // QObject::property() at each format() call.
    TestPropertyProvider p;
    p.setHeaderPattern(QStringLiteral("S/N: %P{serialNumber}"));
    p.setSerialNumber(QStringLiteral("FIRST"));
    QCOMPARE(p.header(), QStringLiteral("S/N: FIRST"));

    p.setSerialNumber(QStringLiteral("SECOND"));
    QCOMPARE(p.header(), QStringLiteral("S/N: SECOND"));
}

void HeaderFooterTest::patternProvider_dynamicProperty_resolvedInHeader()
{
    // Users can inject values without subclassing by using QObject::setProperty()
    // to create dynamic properties — %P{key} resolves both static Q_PROPERTYs
    // and dynamic properties through the same QObject::property() lookup.
    PatternHeaderFooterProvider p;
    p.setHeaderPattern(QStringLiteral("S/N: %P{serialNumber}"));
    p.setProperty("serialNumber", QStringLiteral("DYN-001"));  // QObject::setProperty

    QCOMPARE(p.header(), QStringLiteral("S/N: DYN-001"));
}

void HeaderFooterTest::patternProvider_property_missingKeyExpandsToEmpty()
{
    // An unknown key produces an empty string — no crash, no error message.
    PatternHeaderFooterProvider p;
    p.setHeaderPattern(QStringLiteral("X=%P{noSuchKey}!"));

    QCOMPARE(p.header(), QStringLiteral("X=!"));
}

void HeaderFooterTest::patternProvider_property_formattingWidthApplied()
{
    // Width/padding modifiers work on %P{key} just like on other specifiers.
    TestPropertyProvider p;
    p.setHeaderPattern(QStringLiteral("[%-10P{serialNumber}]"));
    p.setSerialNumber(QStringLiteral("ABC"));

    QCOMPARE(p.header(), QStringLiteral("[ABC       ]"));
}

void HeaderFooterTest::patternProvider_property_combinedWithDate()
{
    DateTime::setProvider([]() {
        return QDateTime(QDate(2026, 6, 1), QTime(0, 0, 0));
    });

    TestPropertyProvider p;
    p.setHeaderPattern(QStringLiteral("%P{label} %d{yyyy-MM-dd}"));
    p.setLabel(QStringLiteral("Started"));

    QCOMPARE(p.header(), QStringLiteral("Started 2026-06-01"));
}

QTEST_GUILESS_MAIN(HeaderFooterTest)
#include "tst_headerfooter.moc"
