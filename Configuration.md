# Log4Qt Configuration Guide

Log4Qt supports three configuration file formats: **Properties**, **JSON**, and
**XML**. All three produce the same flat key/value model and are handled by the
same PropertyConfigurator engine, so every feature available in one format is
available in the others.

## File Discovery

During automatic initialization the LogManager searches for configuration files
in the following order at several locations (application binary path, application
directory, current working directory):

1. `log4qt.properties`
2. `log4qt.json`
3. `log4qt.xml`

The first file found wins. You can also point to a specific file via the
`LOG4QT_CONFIGURATION` environment variable or the `Log4Qt/Configuration`
QSettings entry.

---

## Configuration Model

The configuration is built from four sections:

| Section | Description |
|---------|-------------|
| **Global settings** | `reset`, `status`, `threshold`, etc. |
| **Appenders** | `appender.<alias>.*` |
| **Root logger** | `rootLogger.*` |
| **Named loggers** | `logger.<alias>.*` |

Processing order: global settings &rarr; appenders &rarr; root logger &rarr; named loggers.

---

## Global Settings

| Key | Type | Description |
|-----|------|-------------|
| `reset` | bool | Reset the logging configuration before applying new settings. |
| `status` | Level | Internal Log4Qt logging level (for debugging the logging framework itself). |
| `threshold` | Level | Repository-wide threshold; events below this level are discarded. |
| `handleQtMessages` | bool | Redirect `qDebug()` / `qWarning()` / etc. through Log4Qt. |
| `watchThisFile` | bool | Watch the configuration file and reconfigure on changes. |
| `filterRules` | string | Qt logging filter rules (semicolons are converted to newlines). |
| `messagePattern` | string | Qt message pattern for `qSetMessagePattern()`. |

**Valid levels:** `ALL`, `TRACE`, `DEBUG`, `INFO`, `WARN`, `ERROR`, `FATAL`,
`OFF`, `NULL`.

---

## Appenders

Each appender is defined under the `appender.<alias>` prefix. The alias is an
arbitrary identifier used to reference the appender from loggers.

### Required Keys

| Key | Description |
|-----|-------------|
| `appender.<alias>.type` | Appender class name (see table below). |
| `appender.<alias>.layout.type` | Layout class name (required if the appender needs a layout). |

### Optional Keys

| Key | Description |
|-----|-------------|
| `appender.<alias>.name` | Display name (defaults to the alias). |
| `appender.<alias>.threshold` | Appender-level threshold. |
| `appender.<alias>.layout.<property>` | Layout properties (e.g. `conversionPattern`, `dateFormat`). |
| `appender.<alias>.filter.<falias>.type` | Filter class name. |
| `appender.<alias>.filter.<falias>.<property>` | Filter properties. |
| `appender.<alias>.<property>` | Any Qt property on the appender object (e.g. `file`, `target`). |

### Built-in Appender Types

| Short Name | Class | Description |
|------------|-------|-------------|
| `Console` | ConsoleAppender | Writes to stdout/stderr. |
| `File` | FileAppender | Writes to a single file. |
| `RollingFile` | RollingFileAppender | File with size-based rotation. |
| `DailyRollingFile` | DailyRollingFileAppender | File with date-based rotation. |
| `DailyFile` | DailyFileAppender | Daily file with configurable retention. |
| `Async` | AsyncAppender | Asynchronous wrapper appender. |
| `MainThread` | MainThreadAppender | Dispatches to the main thread. |
| `Signal` | SignalAppender | Emits a Qt signal per log event. |
| `SystemLog` | SystemLogAppender | Writes to the system log (syslog / Event Log). |
| `Debug` | DebugAppender | Appender for debugging purposes. |
| `Null` | NullAppender | Discards all events. |
| `List` | ListAppender | Stores events in a list (for testing). |
| `ColorConsole` | ColorConsoleAppender | Coloured console output (Windows only). |
| `WDC` | WDCAppender | OutputDebugString (Windows only). |
| `Database` | DatabaseAppender | Writes to a SQL database (optional). |
| `Telnet` | TelnetAppender | Serves log events over telnet (optional). |

### Built-in Layout Types

| Short Name | Class | Description |
|------------|-------|-------------|
| `PatternLayout` | PatternLayout | Configurable pattern-based formatting. |
| `SimpleLayout` | SimpleLayout | `LEVEL - message` format. |
| `SimpleTimeLayout` | SimpleTimeLayout | Adds a timestamp to SimpleLayout. |
| `TTCCLayout` | TTCCLayout | Time, thread, category, context layout. |
| `XMLLayout` | XMLLayout | XML-formatted log events. |
| `DatabaseLayout` | DatabaseLayout | Layout for database appender (optional). |

### Built-in Filter Types

| Short Name | Class | Description |
|------------|-------|-------------|
| `DenyAll` | DenyAllFilter | Denies all events. |
| `LevelMatch` | LevelMatchFilter | Matches a specific level. Properties: `levelToMatch`, `acceptOnMatch`. |
| `LevelRange` | LevelRangeFilter | Matches a range of levels. Properties: `levelMin`, `levelMax`, `acceptOnMatch`. |
| `StringMatch` | StringMatchFilter | Matches a substring. Properties: `stringToMatch`, `acceptOnMatch`. |

---

## Root Logger

| Key | Description |
|-----|-------------|
| `rootLogger.level` | Log level for the root logger. |
| `rootLogger.appenderRef.<alias>.ref` | Name of an appender to attach. |

You can attach multiple appenders by using different aliases:

```properties
rootLogger.appenderRef.console.ref=console
rootLogger.appenderRef.daily.ref=daily
```

---

## Named Loggers

Each named logger is defined under the `logger.<alias>` prefix.

| Key | Description |
|-----|-------------|
| `logger.<alias>.name` | The actual logger name (required). |
| `logger.<alias>.level` | Log level (`INHERITED` to inherit from parent). |
| `logger.<alias>.additivity` | `true` (default) or `false`. |
| `logger.<alias>.appenderRef.<refalias>.ref` | Appender to attach. |

The alias is an arbitrary identifier; the `name` property is the real logger
name used at runtime. This separation allows logger names that contain dots or
special characters.

---

## Variable Substitution

String values support `${key}` substitution. The key is looked up in the
properties themselves first, then in environment variables prefixed with
`LOG4QT_`.

```properties
logpath=/var/log/myapp
appender.daily.file=${logpath}/app.log
```

---

## Properties Format

Standard Java-style `.properties` file. Lines starting with `#` or `!` are
comments. Keys and values are separated by `=` or `:`.

### Complete Example

```properties
# Variable for log directory
logpath=./logs

# Global settings
reset=true
status=WARN
threshold=NULL
handleQtMessages=true
watchThisFile=false

# Console appender
appender.console.type=Console
appender.console.target=STDOUT_TARGET
appender.console.threshold=ALL
appender.console.layout.type=TTCCLayout
appender.console.layout.dateFormat=dd.MM.yyyy hh:mm:ss.zzz
appender.console.layout.contextPrinting=true

# Daily rolling file appender
appender.daily.type=DailyFile
appender.daily.file=${logpath}/app.log
appender.daily.appendFile=true
appender.daily.datePattern=_yyyy_MM_dd
appender.daily.keepDays=90
appender.daily.layout.type=TTCCLayout
appender.daily.layout.dateFormat=dd.MM.yyyy hh:mm:ss.zzz
appender.daily.layout.contextPrinting=true

# Rolling file with filter
appender.rolling.type=RollingFile
appender.rolling.file=${logpath}/errors.log
appender.rolling.maxFileSize=10MB
appender.rolling.maxBackupIndex=5
appender.rolling.layout.type=PatternLayout
appender.rolling.layout.conversionPattern=%-5p %d{ISO8601} [%t] %c - %m%n
appender.rolling.filter.level.type=LevelMatch
appender.rolling.filter.level.levelToMatch=ERROR
appender.rolling.filter.level.acceptOnMatch=true

# Root logger
rootLogger.level=ALL
rootLogger.appenderRef.console.ref=console
rootLogger.appenderRef.daily.ref=daily

# Named loggers
logger.db.name=Database
logger.db.level=ERROR
logger.db.additivity=false
logger.db.appenderRef.rolling.ref=rolling

logger.network.name=Network
logger.network.level=WARN
```

---

## JSON Format

The JSON structure maps directly to the flat property keys. Nested objects
produce dot-separated keys. Boolean and numeric values are automatically
stringified.

### Complete Example

```json
{
    "logpath": "./logs",
    "reset": "true",
    "status": "WARN",
    "threshold": "NULL",
    "handleQtMessages": "true",
    "watchThisFile": "false",
    "appender": {
        "console": {
            "type": "Console",
            "target": "STDOUT_TARGET",
            "threshold": "ALL",
            "layout": {
                "type": "TTCCLayout",
                "dateFormat": "dd.MM.yyyy hh:mm:ss.zzz",
                "contextPrinting": true
            }
        },
        "daily": {
            "type": "DailyFile",
            "file": "${logpath}/app.log",
            "appendFile": "true",
            "datePattern": "_yyyy_MM_dd",
            "keepDays": 90,
            "layout": {
                "type": "TTCCLayout",
                "dateFormat": "dd.MM.yyyy hh:mm:ss.zzz",
                "contextPrinting": true
            }
        },
        "rolling": {
            "type": "RollingFile",
            "file": "${logpath}/errors.log",
            "maxFileSize": "10MB",
            "maxBackupIndex": 5,
            "layout": {
                "type": "PatternLayout",
                "conversionPattern": "%-5p %d{ISO8601} [%t] %c - %m%n"
            },
            "filter": {
                "level": {
                    "type": "LevelMatch",
                    "levelToMatch": "ERROR",
                    "acceptOnMatch": "true"
                }
            }
        }
    },
    "rootLogger": {
        "level": "ALL",
        "appenderRef": {
            "console": { "ref": "console" },
            "daily": { "ref": "daily" }
        }
    },
    "logger": {
        "db": {
            "name": "Database",
            "level": "ERROR",
            "additivity": "false",
            "appenderRef": {
                "rolling": { "ref": "rolling" }
            }
        },
        "network": {
            "name": "Network",
            "level": "WARN"
        }
    }
}
```

---

## XML Format

The XML structure uses element names as key segments. The root element (e.g.
`<configuration>`) is skipped; its children become top-level keys. XML
attributes on an element are flattened as child properties.

### Complete Example

```xml
<?xml version="1.0" encoding="UTF-8"?>
<configuration>
    <logpath>./logs</logpath>
    <reset>true</reset>
    <status>WARN</status>
    <threshold>NULL</threshold>
    <handleQtMessages>true</handleQtMessages>
    <watchThisFile>false</watchThisFile>

    <appender>
        <console>
            <type>Console</type>
            <target>STDOUT_TARGET</target>
            <threshold>ALL</threshold>
            <layout>
                <type>TTCCLayout</type>
                <dateFormat>dd.MM.yyyy hh:mm:ss.zzz</dateFormat>
                <contextPrinting>true</contextPrinting>
            </layout>
        </console>
        <daily>
            <type>DailyFile</type>
            <file>${logpath}/app.log</file>
            <appendFile>true</appendFile>
            <datePattern>_yyyy_MM_dd</datePattern>
            <keepDays>90</keepDays>
            <layout>
                <type>TTCCLayout</type>
                <dateFormat>dd.MM.yyyy hh:mm:ss.zzz</dateFormat>
                <contextPrinting>true</contextPrinting>
            </layout>
        </daily>
        <rolling>
            <type>RollingFile</type>
            <file>${logpath}/errors.log</file>
            <maxFileSize>10MB</maxFileSize>
            <maxBackupIndex>5</maxBackupIndex>
            <layout>
                <type>PatternLayout</type>
                <conversionPattern>%-5p %d{ISO8601} [%t] %c - %m%n</conversionPattern>
            </layout>
            <filter>
                <level>
                    <type>LevelMatch</type>
                    <levelToMatch>ERROR</levelToMatch>
                    <acceptOnMatch>true</acceptOnMatch>
                </level>
            </filter>
        </rolling>
    </appender>

    <rootLogger>
        <level>ALL</level>
        <appenderRef>
            <console>
                <ref>console</ref>
            </console>
            <daily>
                <ref>daily</ref>
            </daily>
        </appenderRef>
    </rootLogger>

    <logger>
        <db>
            <name>Database</name>
            <level>ERROR</level>
            <additivity>false</additivity>
            <appenderRef>
                <rolling>
                    <ref>rolling</ref>
                </rolling>
            </appenderRef>
        </db>
        <network>
            <name>Network</name>
            <level>WARN</level>
        </network>
    </logger>
</configuration>
```

---

## Legacy Log4j1 Format (Backward Compatibility)

Property files using the old Log4j1-style format (keys prefixed with `log4j.`)
are automatically detected and translated to the new format. No configuration
changes are needed — existing property files continue to work.

**Note:** Mixing legacy and new-format keys in the same file is not supported.

### Key Mapping

| Legacy Key | New Key |
|------------|---------|
| `log4j.appender.X=classname` | `appender.X.type=classname` |
| `log4j.appender.X.layout=classname` | `appender.X.layout.type=classname` |
| `log4j.appender.X.filter.F=classname` | `appender.X.filter.F.type=classname` |
| `log4j.appender.X.<prop>=value` | `appender.X.<prop>=value` |
| `log4j.rootLogger=LEVEL, A1, A2` | `rootLogger.level=LEVEL` + `rootLogger.appenderRef.N.ref=AN` |
| `log4j.rootCategory=...` | Same as `rootLogger` (deprecated alias) |
| `log4j.logger.name=LEVEL, A1` | `logger.<alias>.name=name` + `.level` + `.appenderRef` |
| `log4j.category.name=...` | Same as `logger` (deprecated alias) |
| `log4j.additivity.name=bool` | `logger.<alias>.additivity=bool` |
| `log4j.Debug` / `log4j.configDebug` | `status` |
| `log4j.reset` | `reset` |
| `log4j.threshold` | `threshold` |
| `log4j.handleQtMessages` | `handleQtMessages` |
| `log4j.watchThisFile` | `watchThisFile` |
| `log4j.qtLogging.filterRules` | `filterRules` |
| `log4j.qtLogging.messagePattern` | `messagePattern` |

### Legacy Format Example

```properties
# Variable (no log4j. prefix — passed through as-is)
logpath=./logs

# Global settings
log4j.reset=true
log4j.Debug=WARN
log4j.threshold=NULL
log4j.handleQtMessages=true

# Appender (class name as value)
log4j.appender.console=org.apache.log4j.ConsoleAppender
log4j.appender.console.target=STDOUT_TARGET
log4j.appender.console.layout=org.apache.log4j.TTCCLayout
log4j.appender.console.layout.dateFormat=dd.MM.yyyy hh:mm:ss.zzz

log4j.appender.daily=Log4Qt::DailyFileAppender
log4j.appender.daily.file=${logpath}/app.log
log4j.appender.daily.appendFile=true
log4j.appender.daily.datePattern=_yyyy_MM_dd
log4j.appender.daily.layout=org.apache.log4j.TTCCLayout

# Root logger (level + comma-separated appender list)
log4j.rootLogger=ALL, console, daily

# Named logger
log4j.logger.MyApp=ERROR, console
log4j.additivity.MyApp=false
```

---

## Programmatic Configuration via QSettings

You can also configure Log4Qt from QSettings. The keys are stored under the
`Log4Qt/Properties` group:

```cpp
QSettings s;
s.beginGroup("Log4Qt");
s.setValue("Debug", "TRACE");
s.beginGroup("Properties");
s.setValue("appender.A1.type", "File");
s.setValue("appender.A1.file", "C:/myapp.log");
s.setValue("appender.A1.layout.type", "TTCCLayout");
s.setValue("appender.A1.layout.dateFormat", "ISO8601");
s.setValue("rootLogger.level", "TRACE");
s.setValue("rootLogger.appenderRef.0.ref", "A1");
```

---

## Custom Components

Register custom appenders, layouts, or filters with the Factory before
configuration:

```cpp
Factory::registerAppender("MyAppender", []() -> Appender* {
    return new MyAppender;
});
```

Then use the registered name in any configuration file:

```properties
appender.custom.type=MyAppender
appender.custom.myProperty=value
```

Properties are set via Qt's meta-object system, so any `Q_PROPERTY` declared
on your class is automatically available as a configuration key.
