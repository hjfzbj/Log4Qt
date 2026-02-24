# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Log4Qt is a C++ port of Apache Log4j using the Qt Framework. It provides hierarchical, configurable logging with thread-safe operations. Requires C++20, Qt 5.12+ or Qt 6.0+.

## Build Commands

```bash
# Configure (out-of-source build required)
cmake -B build -DBUILD_SHARED_LIBS=ON

# Build
cmake --build build

# Run all tests
ctest --test-dir build --output-on-failure

# Run specific test executable
./build/bin/log4qttest

# Run a single Qt Test function (Qt Test framework convention)
./build/bin/log4qttest testFunctionName
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_SHARED_LIBS` | ON | Build shared vs static library |
| `BUILD_WITH_DB_LOGGING` | OFF | Database appender (requires Qt Sql) |
| `BUILD_WITH_TELNET_LOGGING` | ON | Telnet appender (requires Qt Network) |
| `BUILD_WITH_QML_LOGGING` | ON | QML logger (requires Qt Qml) |
| `LOG4QT_ENABLE_TESTS` | ON (standalone) | Build test suite |
| `LOG4QT_ENABLE_EXAMPLES` | ON (standalone) | Build examples |

## Architecture

### Log4j Port Pattern

The library mirrors Log4j's architecture: **Logger** -> **Appender** -> **Layout**, with a **Filter** chain on each appender. Loggers form a hierarchy using dot-separated names (e.g., `com.app.module`), where child loggers inherit parent appenders when additivity is enabled.

### Key Classes and Data Flow

1. **LogManager** — global singleton, holds the Hierarchy, handles auto-initialization from `log4qt.properties` or `log4qt.json`
2. **Hierarchy** — the logger repository; maintains a `QHash<QString, Logger*>` protected by `QReadWriteLock`
3. **Logger** — obtained via `Logger::logger("name")`; checks level then calls `callAppenders(LoggingEvent)`
4. **LoggingEvent** — value type using `QSharedDataPointer` (implicit sharing/COW) carrying level, message, timestamp, thread name, MDC/NDC context, and source location
5. **Appender** (via **AppenderSkeleton**) — applies filter chain (ACCEPT/DENY/NEUTRAL), then formats via Layout and writes to destination
6. **Layout** subclasses — format the event; **PatternLayout** supports Log4j conversion patterns (`%d`, `%t`, `%c`, `%p`, `%m`, `%n`, etc.)

### Thread Safety Model

- `Logger::mLevel` and `Logger::mAdditivity` are `std::atomic`
- `AppenderSkeleton` protects its filter chain with `QMutex`
- `Hierarchy` uses `QReadWriteLock` for logger lookup
- All public methods are documented as thread-safe
- `LoggingEvent` is safely copyable across threads via implicit sharing

### Static Logger Macros

```cpp
LOG4QT_DECLARE_QCLASS_LOGGER      // For QObject subclasses — provides logger() method
LOG4QT_DECLARE_STATIC_LOGGER(fn, ClassName)  // For non-QObject classes — provides fn() function
```

Both use lazy-initialized static pointers (thread-safe via C++11 magic statics).

### Logging Macros

`l4qFatal(...)`, `l4qError(...)`, `l4qWarn(...)`, `l4qInfo(...)`, `l4qDebug(...)`, `l4qTrace(...)` — capture source location automatically.

### Configuration

Two configuration formats are supported:

- **PropertyConfigurator** reads Java `.properties` format files.
- **JsonConfigurator** reads `.json` files, flattens them into properties (nested objects become dot-separated keys, `@class` sets the parent key's class name), and delegates to PropertyConfigurator.

Initialization searches for configuration files in this order at each location: `log4qt.properties` then `log4qt.json` (`.properties` takes priority). The `log4j.` prefix is used in property keys (matching Log4j conventions). File watching with auto-reload is supported via `log4j.watchThisFile=true`.

## Code Conventions

- **Member variables**: `m` prefix (mName, mLevel); static: `s_` prefix
- **Naming**: PascalCase classes, camelCase methods, getters without prefix, setters with `set` prefix
- **All public classes** use `Q_OBJECT`, `Q_PROPERTY`, `Q_DISABLE_COPY_MOVE`
- **Export macro**: `LOG4QT_EXPORT` on all public API classes
- **Symbol visibility**: hidden by default, explicitly exported only
- **Headers**: include guards (`#ifndef LOG4QT_CLASSNAME_H`), not `#pragma once`
- **Modern C++20**: `std::atomic`, spaceship operator (`<=>`), concepts, `std::source_location`, fold expressions
- **Level values**: `uint8_t` — NULL_INT(0), ALL_INT(32), TRACE_INT(64), DEBUG_INT(96), INFO_INT(128), WARN_INT(150), ERROR_INT(182), FATAL_INT(214), OFF_INT(255)

## Test Structure

Tests use the **Qt Test framework**. The main test suite is `tests/log4qttest/` with a single `Log4QtTest` class containing 60+ test methods. Additional focused tests exist in `tests/dailyfileappendertest/`, `tests/filewatcher/`, `tests/jsontest/`, and `tests/performancetest/`.

## Source Layout

- `include/log4qt/` — public installed headers (some classes have headers here)
- `src/log4qt/` — library source and headers for core classes
- `src/log4qt/helpers/` — utilities (Factory, Properties, PatternFormatter, DateTime, etc.)
- `src/log4qt/spi/` — service provider interface (Filter base class)
- `src/log4qt/varia/` — miscellaneous appenders and filters (ListAppender, NullAppender, level/string match filters)
