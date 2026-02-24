Log4Qt - Logging for the Qt cross-platform application framework
================================================================

Continuous Integration
----------------------
[![CMake Ubuntu build and test](https://github.com/MEONMedical/Log4Qt/actions/workflows/cmake-ubuntu.yml/badge.svg)](https://github.com/MEONMedical/Log4Qt/actions/workflows/cmake-ubuntu.yml)
[![CMake Windows build and test](https://github.com/MEONMedical/Log4Qt/actions/workflows/cmake-windows.yml/badge.svg)](https://github.com/MEONMedical/Log4Qt/actions/workflows/cmake-windows.yml)
[![CMake macOS build and test](https://github.com/MEONMedical/Log4Qt/actions/workflows/cmake-macos.yml/badge.svg)](https://github.com/MEONMedical/Log4Qt/actions/workflows/cmake-macos.yml)


Description
-----------
Log4Qt is a C++ port of the Apache Software Foundation Log4j package using the Qt Framework.
It is intended to be used by open source and commercial Qt projects.

Original project
----------------
http://log4qt.sourceforge.net/

This Log4Qt repository is a clone of the Log4Qt repository from https://gitorious.org/log4qt
which was itself a clone of the original Log4Qt project on SourceForge http://sourceforge.net/projects/log4qt/ (http://log4qt.sourceforge.net/).

Additional features
-------------------
* JSON configuration support (`log4qt.json`) alongside the classic `.properties` format
* SimpleTimeLayout (“dd.MM.yyyy hh:mm [Thread] Level Logger Message”)
* ColorConsoleAppender (render colorized message by escape sequence and put it to console)
* SignalAppender (emit signal when log event happens)
* DatabaseAppender (append log event into sql table)
* DatabaseLayout (put log event into sql table columns)
* Telnet appender (append log event to telnet clients)
* LogStream (qDebug()-style log messages appending)
* MainThreadAppender (Proxy appender for sending log messages through event loop)
* XMLLayout to support apache chainsaw
* DailyFileAppender which generates a logfile for each day (add current date formatted to the filename)
* Binary logger
* Windows Debug Console Appender

JSON Configuration
------------------
As an alternative to `log4qt.properties`, you can use a `log4qt.json` file:

```json
{
    "log4j": {
        "rootLogger": "DEBUG, console",
        "appender": {
            "console": {
                "@class": "org.apache.log4j.ConsoleAppender",
                "target": "STDOUT_TARGET",
                "layout": {
                    "@class": "org.apache.log4j.TTCCLayout",
                    "dateFormat": "ISO8601"
                }
            }
        },
        "logger": {
            "MyApp": "ERROR, console"
        },
        "additivity": {
            "MyApp": "false"
        }
    }
}
```

Nested objects produce dot-separated property keys, and the `@class` key sets the class
name for the parent object. Place the file next to your executable or in the working
directory. When both `log4qt.properties` and `log4qt.json` exist, the `.properties` file
takes priority.

You can also configure programmatically:

```cpp
#include <log4qt/jsonconfigurator.h>

Log4Qt::JsonConfigurator::configure("path/to/config.json");
// or with file watching:
Log4Qt::JsonConfigurator::configureAndWatch("path/to/config.json");
```

Requirements
------------
* Minimum Qt version required Qt5.12 (for support of Qt versions down to 5.3 use branch 1.4 or the lates 1.4.x release
  or for support of Qt versions down to 5.7 use branch 1.5 or the lates 1.5.x release)
* C++20 required (minimum compiler version MSVC 19.29 (Visual Studio 2019 16.11), GCC 10, or Clang 10)

License
-------
Apache License 2, Version 2.0

Clone
-----
    git clone https://github.com/MEONMedical/Log4Qt.git

Build and install
-----------------
### cmake
cmake is used to build Log4Qt. An out-of-source build is required:
    <unpack/fetch to Log4Qt directory>
    mkdir Log4Qt-build
    cd Log4Qt-build
    cmake ../Log4Qt
    make/mingw32-make/msbuild Log4Qt.sln
    make/mingw32-make install


    Addition cmake options are
        * '-DBUILD_SHARED_LIBS=OFF' to build static log4qt lib (default: ON)
        * '-DBUILD_WITH_DB_LOGGING=ON|OFF to build with database logging support (default: OFF)
        * '-DBUILD_WITH_TELNET_LOGGING=ON|OFF to build with telnet appender support (default: ON)
        * '-DBUILD_WITH_QML_LOGGING=ON|OFF to build with qml logger support (default: ON)

