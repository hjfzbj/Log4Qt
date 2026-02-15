QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

include(../../build.pri)
include(../../g++.pri)

SOURCES += \
    performancetest.cpp

HEADERS += \
    performancetest.h

# Link against log4qt library
LIBS += -L../../src/log4qt -llog4qt
INCLUDEPATH += ../../src
