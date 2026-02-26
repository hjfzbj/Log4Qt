# Change Log

All notable changes to this project will be documented in this file.
----
## [v2.0.0] - ??
### Added
- JsonConfigurator for configuring logging from JSON files (`log4qt.json`).
  The JSON structure maps directly to flat property keys via dot-separated
  nesting. Automatic initialization searches for `log4qt.json` alongside
  `log4qt.properties` (`.properties` takes priority).
- XmlConfigurator for configuring logging from XML files (`log4qt.xml`).
  Nested elements produce dot-separated keys; XML attributes are flattened
  as child properties. Automatic initialization searches for `log4qt.xml`
  after `log4qt.json` (`.properties` > `.json` > `.xml` priority).
- Short factory aliases for all built-in components (e.g. `Console`,
  `File`, `RollingFile`, `PatternLayout`, `LevelMatch`) alongside the
  existing `org.apache.log4j.*` and `Log4Qt::*` names.
- Dedicated PropertyConfigurator unit test suite (`tests/propertytest`).

### Changed
- **Breaking:** PropertyConfigurator now uses a Log4j2-style configuration
  format. The `log4j.` prefix is removed, appenders use explicit `type` keys,
  and loggers use appender references instead of inline declarations:
  - `log4j.rootLogger=ALL, console` &rarr; `rootLogger.level=ALL` +
    `rootLogger.appenderRef.0.ref=console`
  - `log4j.appender.X=org.apache.log4j.ConsoleAppender` &rarr;
    `appender.X.type=Console`
  - `log4j.appender.X.layout=org.apache.log4j.TTCCLayout` &rarr;
    `appender.X.layout.type=TTCCLayout`
  - `log4j.logger.MyApp=ERROR, console` &rarr; `logger.MyApp.name=MyApp` +
    `logger.MyApp.level=ERROR` + `logger.MyApp.appenderRef.0.ref=console`
  - `log4j.additivity.MyApp=false` &rarr; `logger.MyApp.additivity=false`
  - Global settings: `log4j.reset` &rarr; `reset`, `log4j.Debug` &rarr;
    `status`, `log4j.threshold` &rarr; `threshold`, etc.
- Appender filters now configured via `appender.X.filter.F.type=LevelMatch`
  with properties under the same prefix.

### Improvements
- Performance optimizations
- Modernize codebase to C++20 standard

### Deprecated / Removed
- Removed qmake support
- Removed binary logger
- Removed legacy Log4j1-style property configuration format

## [v1.6.0] - ??

### Improvements
- Modernize codebase to C++20 standard
- qmake: Make database, telnet appender and qml logger optional depending on CONFIG
- cmake: Make database, telnet appender and qml logger optional depending on option
- Use std::as_const instead of deprecated qAsConst for c++17 and greater
- Use QMetaType instead of QVariant type
- if console is blocked by debugger use OutputDebugString (win)
- Support for Qt6
- Minimum required Qt version is 5.12
- Add case sensitivity option in Stringmatchfilter
- cmake: replace BUILD_STATIC_LOG4CXX_LIB with standard BUILD_SHARED_LIBS

### Fixed
- TTCCLayout crash fix (#69)
- Fixes for Qt6.7
- Replace deprecated Qt 5.15

## [v1.5.1] - 14.02.2020
### Improvements
- Automatically delete files written by DailyFileAppender after a configurable period of time.

## [v1.5.0] - 25.06.2018
### Improvements
- RollingFileAppender: Changed the behavior on application restart and if
                       appendFile is set to false to avoid data loss. The
                       existing log files are rolled instead of overwritten.
                       This behavior is different to the log4/log4cpp implementation.
- Modernize c++ and qt usage wit clang-tidy and clazy

### Fixed
- Fixed "DATE" format string, it will be formatted as MMM YYYY HH:mm:ss.zzz
- Minimum required Qt version is 5.7
- Move color console appender to windows only (is only implemented for windows),
  unified windows os preprocessor switches

## [v1.4.2] - 2017-09-06
### Improvements
- Build: Use moc file includes in order to improve build performance
- Allow java - loggernames in the form 'Foo.bar' within Hierarchy::createLogger()

### Fixed
- Work around for the QFileSystemWatcher shortcome handling changes in files which
  are modified/written into a temp file, on commit the orig file is deleted and
  the temp file renamed/moved to orig file name


## [v1.4.1] - 2017-06-17
### Improvements
- Added qbs support

### Fixed
- Fixed cmake build
- Fix Patternformatter maxLength behaviour to match the log4j documentation -
  if the data item is longer than the maximum field, then the extra characters are removed from the beginning
  of the data item and not from the end.

## [v1.4.0] - 2017-05-04

### Added
- Environment variables starting with LOG4QT_ can now be used within a log4qt.properties file ${LOG4QT_...}
- Added cmake support
- Added suport for location information (%F, %L, %M, %l)
- Added support for QLoggingCategories

### Improvements
- Support static builds
- detect minimum required compiler version (log4qt depends on magic statics c++11)
- Cleanup includes in headers
- Make database and telnet appender optional dependig on Qt module sql and network

### Fixed
- Make DatabaseAppender compile

## [v1.3.0] - 2016-09-13

### Added
- Changed directory layout
- Wrapper logger object for logging from Qml (QmlLogger)
- Appender for windows debug console (OutputDebugString)
- continuous integration windows - appveyor ci config files
- Changelog
- New global boolean property for *.properties files
    log4j.watchThisFile=[true]|[false]
    If set to true an file system watcher is installed for the *.properties file
    and the loggers are automatically reconfigured if this file changes.

### Improvements
- Replaced old-c-style casts
- Searching for *.properties files are extended:
  1. If <application binaryname.exe.log4qt.properties exists this file is used (Windows only)
  2. If <application binaryname>.log4qt.properties exists this file is used
  3. If "log4qt.properties" exists in the executables path this file is used
  4. If "log4qt.properties" exists in the current working directory this file is used (the default before)

### Fixed
- Minimum required Qt version is 5.3
- Reenabled an fixed unit test suite

## [v1.2.0] - 2016-05-03

### Added
- continuous integration linux - travis ci config files

### Improvment
- Replaced object ownership via LogObject and LogObjectPtr with QSharedPointer
- Removed pre Qt5 code
- Code improvements (use c++11 feature, e.g. nulptr, range base for loop, override, ...)
- Replaced msecSinceEpoch methods of DateTime with QDateTime methodes which exists since Qt4.7
- Replaced custom DataTime Formater with QDateTime::toString() - lost additional expression week

### Fixed
- converted inlcude file from utf-16 to ascii
- DatePatternConverter: fixed scope of toString() call

## [v1.1.0] - 2015-12-22

### Added
- Binary logger
- XMLLayout to support apache chainsaw
