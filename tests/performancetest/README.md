# Log4Qt Performance Test

## Overview
This test suite benchmarks the performance of Log4Qt logging framework under various scenarios.

## Test Categories

### 1. Simple Logging Performance (`testSimpleLoggingPerformance`)
Tests basic logging performance with NullAppender (no I/O overhead) to measure pure logging overhead.

**Test Cases:**
- 1,000 DEBUG messages
- 10,000 DEBUG messages
- 100,000 DEBUG messages
- 1,000 INFO messages
- 10,000 INFO messages

**Results:**
- 1,000 DEBUG messages: ~0.39 msecs per iteration
- 10,000 DEBUG messages: ~4.2 msecs per iteration
- 100,000 DEBUG messages: ~40 msecs per iteration
- 1,000 INFO messages: ~0.35 msecs per iteration
- 10,000 INFO messages: ~3.5 msecs per iteration

### 2. File Appender Performance (`testFileAppenderPerformance`)
Tests logging performance with file I/O operations under different configurations.

**Test Cases:**
- Immediate flush vs buffered writing
- Simple layout vs pattern layout
- Various message volumes (1,000 - 10,000 messages)

**Results:**
- 1,000 msgs, immediate flush, simple layout: ~2.6 msecs per iteration
- 1,000 msgs, buffered, simple layout: ~0.73 msecs per iteration (3.5x faster)
- 10,000 msgs, immediate flush, simple layout: ~27 msecs per iteration
- 10,000 msgs, buffered, simple layout: ~7.0 msecs per iteration (3.8x faster)
- 1,000 msgs, immediate flush, pattern layout: ~5.6 msecs per iteration
- 1,000 msgs, buffered, pattern layout: ~3.5 msecs per iteration

**Key Findings:**
- Buffered writing is approximately 3-4x faster than immediate flush
- Pattern layout adds overhead (~2x slower) compared to simple layout

### 3. Multi-threaded Logging Performance (`testMultiThreadedLoggingPerformance`)
Tests concurrent logging from multiple threads.

**Test Cases:**
- 2 threads, 1,000 messages each
- 4 threads, 1,000 messages each
- 8 threads, 1,000 messages each
- 4 threads, 5,000 messages each

**Results:**
- 2 threads, 1,000 msgs each: ~2.6 msecs per iteration
- 4 threads, 1,000 msgs each: ~6.8 msecs per iteration
- 8 threads, 1,000 msgs each: ~18 msecs per iteration
- 4 threads, 5,000 msgs each: ~30 msecs per iteration

**Key Findings:**
- Performance scales relatively linearly with thread count
- Thread synchronization overhead is minimal

### 4. Formatting Performance (`testFormattingPerformance`)
Tests the overhead of different pattern layouts.

**Test Cases:**
- Simple pattern: `%m%n`
- Basic pattern: `%d %p %c - %m%n`
- Complex pattern: `%d{ISO8601} [%t] %-5p %c{2} - %m%n`
- Very complex pattern: `%d{yyyy-MM-dd HH:mm:ss.zzz} [%t] %-5p %c - %F:%L - %m%n`

**Results:**
All patterns perform similarly (~3.5-4.0 msecs for 10,000 messages), showing that Log4Qt's pattern formatting is well-optimized.

### 5. Filtering Performance (`testFilteringPerformance`)
Tests the overhead of adding filters to appenders.

**Test Cases:**
- No filters
- 1 filter
- 3 filters
- 5 filters

**Results:**
Filtering overhead is negligible (~3.4-3.6 msecs for 10,000 messages regardless of filter count).

## Running the Tests

### Build
```bash
cd <log4qt-root>
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH="C:\Qt\6.11.0\msvc2022_64"
cmake --build . --config Release --target performancetest
```

### Execute
```bash
cd build/bin/Release
.\performancetest.exe -v2
```

### Command Line Options
- `-v2` - Verbose output with results
- `-functions` - List all test functions
- `-datatags` - List all data tags
- `-help` - Show all available options

## Performance Recommendations

Based on the test results:

1. **Use buffered writing** when possible (disable immediate flush) for 3-4x performance improvement
2. **Pattern complexity** has minimal impact on performance - choose patterns based on readability
3. **Multi-threaded logging** scales well, but be aware of the synchronization overhead with many threads
4. **Filters are cheap** - don't hesitate to use them when needed
5. **Simple layouts** are 2x faster than pattern layouts if maximum performance is critical

## System Information
- Qt Version: 6.11.0
- Compiler: MSVC 2022
- Build Type: Release
- Platform: Windows 11
