# Performance Optimization Implementation Roadmap

## Overview

This document provides a detailed roadmap for implementing the performance improvements identified in `PERFORMANCE_IMPROVEMENTS.md`.

---

## Phase 1: Quick Wins (Week 1-2)

### 1.1 Lazy String Formatting ⏱️ 3-5 days

**Files to Modify:**
- `src/log4qt/logger.h` (template functions)
- `src/log4qt/logger.cpp` (implementation)

**Implementation Steps:**

1. **Add feature flag** (Day 1)
```cpp
// In log4qt.h
namespace Log4Qt {
    class PerformanceConfig {
    public:
        static bool isLazyFormattingEnabled() { return mLazyFormatting; }
        static void setLazyFormattingEnabled(bool enabled) { mLazyFormatting = enabled; }
    private:
        static bool mLazyFormatting;
    };
}
```

2. **Create deferred formatter helper** (Day 2)
```cpp
// New file: src/log4qt/helpers/deferredformatter.h
template<typename... Args>
class DeferredFormatter {
public:
    DeferredFormatter(const QString &format, Args&&... args)
        : mFormat(format), mArgs(std::forward_as_tuple(args...)) {}
    
    QString format() const {
        return formatImpl(std::index_sequence_for<Args...>{});
    }
    
private:
    template<std::size_t... Is>
    QString formatImpl(std::index_sequence<Is...>) const {
        return mFormat.arg(std::get<Is>(mArgs)...);
    }
    
    QString mFormat;
    std::tuple<Args...> mArgs;
};
```

3. **Update logger template functions** (Day 3-4)
```cpp
// In logger.h
template<typename T, typename ...Ts>
void debug(const QString &message, T &&t, Ts &&...ts) const
{
    if (isEnabledFor(Level::DEBUG_INT)) {
        forcedLog(Level::DEBUG_INT, 
                  QString(message).arg(std::forward<T>(t), std::forward<Ts>(ts)...));
    }
}
```

4. **Add unit tests** (Day 5)
```cpp
// In tests/log4qttest/log4qttest.cpp
void Log4QtTest::testLazyFormatting()
{
    Logger *logger = Logger::rootLogger();
    logger->setLevel(Level::INFO_INT);  // Disable DEBUG
    
    bool expensive_called = false;
    auto expensive_op = [&]() -> QString {
        expensive_called = true;
        return "expensive";
    };
    
    logger->debug("Test: %1", expensive_op());
    
    QVERIFY(!expensive_called);  // Should NOT be called when DEBUG disabled
}
```

**Testing:**
```bash
cd build
cmake --build . --config Release --target log4qttest
bin/Release/log4qttest.exe
```

**Expected Results:**
- 40-60% improvement when logging disabled
- No performance regression when enabled
- All existing tests pass

---

### 1.2 Fast Path for Simple Patterns ⏱️ 2-3 days

**Files to Modify:**
- `src/log4qt/helpers/patternformatter.h`
- `src/log4qt/helpers/patternformatter.cpp`

**Implementation Steps:**

1. **Detect simple patterns** (Day 1)
```cpp
// In patternformatter.cpp
void PatternFormatter::parse()
{
    // Existing parsing...
    
    // Detect simple patterns
    if (mPattern == "%m%n" || mPattern == "%m\n") {
        mFastPath = FastPath::MessageOnly;
    }
    else if (mPattern == "%p: %m%n") {
        mFastPath = FastPath::LevelAndMessage;
    }
    else if (mPattern == "%d %p %m%n") {
        mFastPath = FastPath::TimeLevelMessage;
    }
    else {
        mFastPath = FastPath::None;
    }
}
```

2. **Implement fast paths** (Day 2)
```cpp
QString PatternFormatter::format(const LoggingEvent &event) const
{
    switch (mFastPath) {
    case FastPath::MessageOnly:
        return event.message() + '\n';
    
    case FastPath::LevelAndMessage:
        return event.level().toString() + ": " + event.message() + '\n';
    
    case FastPath::TimeLevelMessage:
        return QTime::currentTime().toString("HH:mm:ss") + ' ' +
               event.level().toString() + ' ' + 
               event.message() + '\n';
    
    default:
        return formatComplex(event);
    }
}
```

3. **Add benchmarks** (Day 3)
```cpp
void PerformanceTest::testSimplePatternPerformance()
{
    auto layout = QSharedPointer<PatternLayout>::create("%m%n");
    
    QBENCHMARK {
        for (int i = 0; i < 10000; ++i) {
            LoggingEvent event(nullptr, Level::INFO_INT, "Test message");
            layout->format(event);
        }
    }
}
```

**Expected Results:**
- 40-50% improvement for patterns "%m%n"
- 30-40% improvement for patterns "%p: %m%n"
- No regression for complex patterns

---

### 1.3 Batch Write Operations ⏱️ 2-3 days

**Files to Modify:**
- `src/log4qt/writerappender.h`
- `src/log4qt/writerappender.cpp`

**Implementation Steps:**

1. **Add configuration** (Day 1)
```cpp
// In writerappender.h
class WriterAppender : public AppenderSkeleton
{
    Q_PROPERTY(int flushBatchSize READ flushBatchSize WRITE setFlushBatchSize)
    Q_PROPERTY(bool flushOnError READ flushOnError WRITE setFlushOnError)
    
public:
    int flushBatchSize() const { return mFlushBatchSize; }
    void setFlushBatchSize(int size) { mFlushBatchSize = size; }
    
    bool flushOnError() const { return mFlushOnError; }
    void setFlushOnError(bool flush) { mFlushOnError = flush; }
    
private:
    int mFlushBatchSize = 10;
    bool mFlushOnError = true;
    std::atomic<int> mFlushCounter{0};
};
```

2. **Implement batching** (Day 2)
```cpp
// In writerappender.cpp
void WriterAppender::append(const LoggingEvent &event)
{
    QString message(layout()->format(event));
    *mWriter << message;
    
    if (immediateFlush())
    {
        bool shouldFlush = false;
        
        // Always flush errors/fatal if configured
        if (mFlushOnError && event.level() >= Level::ERROR_INT) {
            shouldFlush = true;
        }
        // Batch flush
        else if (++mFlushCounter >= mFlushBatchSize) {
            shouldFlush = true;
            mFlushCounter = 0;
        }
        
        if (shouldFlush) {
            mWriter->flush();
            if (handleIoErrors())
                return;
        }
    }
}
```

3. **Add periodic timer flush** (Day 3)
```cpp
// Optional: Add timer-based flushing
void WriterAppender::activateOptions()
{
    // Existing code...
    
    if (mFlushInterval > 0) {
        if (!mFlushTimer) {
            mFlushTimer = new QTimer(this);
            connect(mFlushTimer, &QTimer::timeout, this, &WriterAppender::periodicFlush);
        }
        mFlushTimer->start(mFlushInterval);
    }
}

void WriterAppender::periodicFlush()
{
    QMutexLocker locker(&mObjectGuard);
    if (mWriter) {
        mWriter->flush();
        mFlushCounter = 0;
    }
}
```

**Expected Results:**
- 30-50% improvement in file I/O throughput
- Configurable trade-off between performance and data safety

---

### 1.4 Cache Formatted Thread Names ⏱️ 1-2 days

**Files to Modify:**
- `src/log4qt/loggingevent.cpp`

**Implementation Steps:**

1. **Implement thread-local cache** (Day 1)
```cpp
void LoggingEvent::setThreadNameToCurrent()
{
    // Thread-local cache
    struct ThreadNameCache {
        Qt::HANDLE threadId = nullptr;
        QString threadName;
    };
    
    static thread_local ThreadNameCache cache;
    
    Qt::HANDLE currentId = QThread::currentThreadId();
    
    if (cache.threadId != currentId) {
        QThread *current = QThread::currentThread();
        if (current) {
            cache.threadName = current->objectName();
            if (cache.threadName.isEmpty()) {
                cache.threadName = QStringLiteral("0x%1").arg(
                    reinterpret_cast<quintptr>(current), 
                    QT_POINTER_SIZE * 2, 16, QChar('0'));
            }
        }
        cache.threadId = currentId;
    }
    
    mThreadName = cache.threadName;
}
```

2. **Add multi-threaded test** (Day 2)
```cpp
void PerformanceTest::testThreadNameCaching()
{
    QBENCHMARK {
        QtConcurrent::blockingMap(QVector<int>(1000), [](int) {
            LoggingEvent event;
            return 0;
        });
    }
}
```

**Expected Results:**
- 10-15% improvement in multi-threaded scenarios
- Zero regression in single-threaded use

---

## Phase 2: Medium Effort (Week 3-6)

### 2.1 Lock-Free Appender List ⏱️ 5-7 days

**Complexity:** High  
**Risk:** Medium  
**Impact:** Very High (40-60% in multi-threaded scenarios)

**Files to Modify:**
- `src/log4qt/logger.h`
- `src/log4qt/logger.cpp`
- `src/log4qt/helpers/appenderattachable.h`
- `src/log4qt/helpers/appenderattachable.cpp`

**Implementation Steps:**

1. **Add atomic shared pointer** (Day 1-2)
```cpp
// In appenderattachable.h
class AppenderAttachable
{
protected:
    using AppenderList = QList<AppenderSharedPtr>;
    std::atomic<std::shared_ptr<AppenderList>> mAppendersAtomic;
    QMutex mAppenderModifyMutex;  // Only for modifications
    
    // Remove old: mutable QReadWriteLock mAppenderGuard;
};
```

2. **Update read operations** (Day 3-4)
```cpp
// In logger.cpp
void Logger::callAppenders(const LoggingEvent &event) const
{
    // Lock-free read
    auto appenders = mAppendersAtomic.load(std::memory_order_acquire);
    if (appenders) {
        for (const auto& appender : *appenders)
            appender->doAppend(event);
    }
    
    if (additivity() && (parentLogger() != nullptr))
        parentLogger()->callAppenders(event);
}
```

3. **Update write operations** (Day 4-5)
```cpp
void AppenderAttachable::addAppender(const AppenderSharedPtr &appender)
{
    QMutexLocker locker(&mAppenderModifyMutex);
    
    // Copy-on-write
    auto current = mAppendersAtomic.load(std::memory_order_relaxed);
    auto newList = std::make_shared<AppenderList>(current ? *current : AppenderList());
    
    if (!newList->contains(appender))
        newList->append(appender);
    
    mAppendersAtomic.store(newList, std::memory_order_release);
}
```

4. **Extensive testing** (Day 6-7)
```cpp
void Log4QtTest::testLockFreeAppendersConcurrency()
{
    Logger *logger = Logger::rootLogger();
    
    // Concurrent reads and writes
    QThreadPool pool;
    
    // Writer thread - adds/removes appenders
    QtConcurrent::run(&pool, [logger]() {
        for (int i = 0; i < 100; ++i) {
            auto appender = QSharedPointer<ListAppender>::create();
            logger->addAppender(appender);
            QThread::msleep(10);
            logger->removeAppender(appender);
        }
    });
    
    // Reader threads - log messages
    for (int t = 0; t < 4; ++t) {
        QtConcurrent::run(&pool, [logger]() {
            for (int i = 0; i < 10000; ++i)
                logger->info("Test message");
        });
    }
    
    pool.waitForDone();
    
    // Verify no crashes, no data corruption
}
```

**Testing Strategy:**
- Thread sanitizer (TSan)
- Valgrind/Helgrind
- Stress testing with multiple threads
- Memory leak detection

**Expected Results:**
- 40-60% improvement in multi-threaded scenarios
- No race conditions
- No memory leaks

---

### 2.2 Object Pooling for LoggingEvent ⏱️ 4-5 days

**Complexity:** Medium  
**Risk:** Low  
**Impact:** High (30-40% reduction in allocations)

**Files to Modify:**
- `src/log4qt/loggingevent.h`
- `src/log4qt/loggingevent.cpp`
- `src/log4qt/logger.cpp`

**Implementation Steps:**

1. **Create pool manager** (Day 1-2)
```cpp
// New file: src/log4qt/helpers/loggingeventpool.h
class LoggingEventPool
{
public:
    static LoggingEvent* acquire()
    {
        thread_local std::vector<std::unique_ptr<LoggingEvent>> pool;
        thread_local const size_t MAX_POOL_SIZE = 32;
        
        if (!pool.empty()) {
            auto event = pool.back().release();
            pool.pop_back();
            return event;
        }
        
        return new LoggingEvent();
    }
    
    static void release(LoggingEvent* event)
    {
        thread_local std::vector<std::unique_ptr<LoggingEvent>> pool;
        thread_local const size_t MAX_POOL_SIZE = 32;
        
        if (pool.size() < MAX_POOL_SIZE) {
            event->reset();  // Clear for reuse
            pool.emplace_back(event);
        } else {
            delete event;
        }
    }
};
```

2. **Add reset method** (Day 2)
```cpp
// In loggingevent.h
class LoggingEvent
{
public:
    void reset()
    {
        mLevel = Level::NULL_INT;
        mLogger = nullptr;
        mMessage.clear();
        mNdc.clear();
        mProperties.clear();
        mThreadName.clear();
        mTimeStamp = 0;
        mContext = MessageContext();
        mCategoryName.clear();
    }
};
```

3. **Update logger to use pool** (Day 3-4)
```cpp
// In logger.cpp - Optional opt-in
void Logger::forcedLog(Level level, const QString &message) const
{
    if (PerformanceConfig::isEventPoolingEnabled()) {
        auto* event = LoggingEventPool::acquire();
        event->reinitialize(this, level, message);
        callAppenders(*event);
        LoggingEventPool::release(event);
    } else {
        LoggingEvent event(this, level, message);
        callAppenders(event);
    }
}
```

4. **Benchmark** (Day 5)
```cpp
void PerformanceTest::testEventPooling()
{
    PerformanceConfig::setEventPoolingEnabled(true);
    
    QBENCHMARK {
        for (int i = 0; i < 100000; ++i) {
            LoggingEvent event(nullptr, Level::INFO_INT, "Test");
        }
    }
}
```

**Expected Results:**
- 30-40% reduction in allocations
- Improved cache locality
- Lower GC pressure (if applicable)

---

## Phase 3: Advanced Optimizations (Week 7-12)

### 3.1 Reduce LoggingEvent Size ⏱️ 7-10 days

**Complexity:** High  
**Risk:** Medium (ABI breaking)  
**Impact:** Medium (20-30% memory reduction)

**Strategy:** Make rarely-used fields optional

### 3.2 Optimize NDC/MDC Access ⏱️ 3-4 days

**Complexity:** Medium  
**Risk:** Low  
**Impact:** Medium (20-30% when not used)

**Strategy:** Lazy initialization based on pattern

### 3.3 Direct QFile::write() ⏱️ 2-3 days

**Complexity:** Low  
**Risk:** Low  
**Impact:** Medium (10-15% I/O improvement)

**Strategy:** Optional fast file appender

---

## Testing & Validation

### Unit Tests
- All existing tests must pass
- Add new tests for each optimization
- Thread safety tests
- Memory leak tests

### Performance Tests
```bash
# Before changes
cd build/bin/Release
./performancetest.exe > baseline.txt

# After changes
./performancetest.exe > optimized.txt

# Compare
python ../../../scripts/compare_performance.py baseline.txt optimized.txt
```

### Integration Tests
- Run full test suite
- Test real applications
- Stress testing

---

## Risk Mitigation

### Feature Flags
All optimizations should be opt-in initially:
```cpp
PerformanceConfig::setLazyFormattingEnabled(true);
PerformanceConfig::setEventPoolingEnabled(true);
PerformanceConfig::setLockFreeAppendersEnabled(true);
```

### Gradual Rollout
1. Internal testing (Week 1-2)
2. Beta release (Week 3-4)
3. Production release (Week 5+)

### Rollback Plan
- Keep old code paths available
- Version control tags for each phase
- Quick disable via config

---

## Documentation Updates

1. Update README.md with performance notes
2. Add performance tuning guide
3. Update API documentation
4. Create migration guide for breaking changes

---

## Success Metrics

### Performance Targets
- Simple logging: 50-70% improvement
- Multi-threaded: 40-60% improvement
- File I/O: 30-50% improvement

### Quality Targets
- Zero crashes in stress testing
- Zero memory leaks
- All unit tests pass
- No performance regressions in any scenario

---

## Timeline Summary

| Phase | Duration | Cumulative | Expected Gain |
|-------|----------|------------|---------------|
| Phase 1 | 2 weeks | 2 weeks | 50-70% |
| Phase 2 | 4 weeks | 6 weeks | 70-90% |
| Phase 3 | 6 weeks | 12 weeks | 90-120% |

---

**Document Version:** 1.0  
**Last Updated:** 2026-02-15  
**Status:** Planning Phase
