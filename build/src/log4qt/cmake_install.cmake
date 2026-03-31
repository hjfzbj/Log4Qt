# Install script for directory: C:/Workspace/log4qt/src/log4qt

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Log4Qt")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Library" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Workspace/log4qt/build/src/log4qt/Debug/log4qt_d.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Workspace/log4qt/build/src/log4qt/Release/log4qt.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Workspace/log4qt/build/src/log4qt/MinSizeRel/log4qt.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Workspace/log4qt/build/src/log4qt/RelWithDebInfo/log4qt.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Library" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Workspace/log4qt/build/bin/Debug/log4qt_d.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Workspace/log4qt/build/bin/Release/log4qt.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Workspace/log4qt/build/bin/MinSizeRel/log4qt.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Workspace/log4qt/build/bin/RelWithDebInfo/log4qt.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/log4qt" TYPE FILE FILES
    "C:/Workspace/log4qt/src/log4qt/appender.h"
    "C:/Workspace/log4qt/src/log4qt/appenderskeleton.h"
    "C:/Workspace/log4qt/src/log4qt/asyncappender.h"
    "C:/Workspace/log4qt/src/log4qt/basicconfigurator.h"
    "C:/Workspace/log4qt/src/log4qt/consoleappender.h"
    "C:/Workspace/log4qt/src/log4qt/dailyrollingfileappender.h"
    "C:/Workspace/log4qt/src/log4qt/fileappender.h"
    "C:/Workspace/log4qt/src/log4qt/hierarchy.h"
    "C:/Workspace/log4qt/src/log4qt/jsonconfigurator.h"
    "C:/Workspace/log4qt/src/log4qt/layout.h"
    "C:/Workspace/log4qt/src/log4qt/level.h"
    "C:/Workspace/log4qt/src/log4qt/log4qtdefs.h"
    "C:/Workspace/log4qt/src/log4qt/log4qt.h"
    "C:/Workspace/log4qt/src/log4qt/log4qtshared.h"
    "C:/Workspace/log4qt/src/log4qt/log4qtsharedptr.h"
    "C:/Workspace/log4qt/src/log4qt/logger.h"
    "C:/Workspace/log4qt/src/log4qt/loggerrepository.h"
    "C:/Workspace/log4qt/src/log4qt/loggingevent.h"
    "C:/Workspace/log4qt/src/log4qt/logmanager.h"
    "C:/Workspace/log4qt/src/log4qt/logstream.h"
    "C:/Workspace/log4qt/src/log4qt/mainthreadappender.h"
    "C:/Workspace/log4qt/src/log4qt/mdc.h"
    "C:/Workspace/log4qt/src/log4qt/ndc.h"
    "C:/Workspace/log4qt/src/log4qt/patternlayout.h"
    "C:/Workspace/log4qt/src/log4qt/propertyconfigurator.h"
    "C:/Workspace/log4qt/src/log4qt/rollingfileappender.h"
    "C:/Workspace/log4qt/src/log4qt/signalappender.h"
    "C:/Workspace/log4qt/src/log4qt/simplelayout.h"
    "C:/Workspace/log4qt/src/log4qt/simpletimelayout.h"
    "C:/Workspace/log4qt/src/log4qt/systemlogappender.h"
    "C:/Workspace/log4qt/src/log4qt/ttcclayout.h"
    "C:/Workspace/log4qt/src/log4qt/writerappender.h"
    "C:/Workspace/log4qt/src/log4qt/xmlconfigurator.h"
    "C:/Workspace/log4qt/src/log4qt/xmllayout.h"
    "C:/Workspace/log4qt/src/log4qt/wdcappender.h"
    "C:/Workspace/log4qt/src/log4qt/colorconsoleappender.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/log4qt/helpers" TYPE FILE FILES
    "C:/Workspace/log4qt/src/log4qt/helpers/appenderattachable.h"
    "C:/Workspace/log4qt/src/log4qt/helpers/classlogger.h"
    "C:/Workspace/log4qt/src/log4qt/helpers/configuratorhelper.h"
    "C:/Workspace/log4qt/src/log4qt/helpers/cronexpression.h"
    "C:/Workspace/log4qt/src/log4qt/helpers/datetime.h"
    "C:/Workspace/log4qt/src/log4qt/helpers/dispatcher.h"
    "C:/Workspace/log4qt/src/log4qt/helpers/factory.h"
    "C:/Workspace/log4qt/src/log4qt/helpers/initialisationhelper.h"
    "C:/Workspace/log4qt/src/log4qt/helpers/logerror.h"
    "C:/Workspace/log4qt/src/log4qt/helpers/optionconverter.h"
    "C:/Workspace/log4qt/src/log4qt/helpers/patternformatter.h"
    "C:/Workspace/log4qt/src/log4qt/helpers/properties.h"
    "C:/Workspace/log4qt/src/log4qt/helpers/timestampprovider.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/log4qt/spi" TYPE FILE FILES
    "C:/Workspace/log4qt/src/log4qt/spi/compositetriggeringpolicy.h"
    "C:/Workspace/log4qt/src/log4qt/spi/crontriggeringpolicy.h"
    "C:/Workspace/log4qt/src/log4qt/spi/daterolloverstrategy.h"
    "C:/Workspace/log4qt/src/log4qt/spi/defaultrolloverstrategy.h"
    "C:/Workspace/log4qt/src/log4qt/spi/filter.h"
    "C:/Workspace/log4qt/src/log4qt/spi/onstartuptriggeringpolicy.h"
    "C:/Workspace/log4qt/src/log4qt/spi/rolloverstrategy.h"
    "C:/Workspace/log4qt/src/log4qt/spi/sizebasedtriggeringpolicy.h"
    "C:/Workspace/log4qt/src/log4qt/spi/timebasedtriggeringpolicy.h"
    "C:/Workspace/log4qt/src/log4qt/spi/triggeringpolicy.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/log4qt/varia" TYPE FILE FILES
    "C:/Workspace/log4qt/src/log4qt/varia/debugappender.h"
    "C:/Workspace/log4qt/src/log4qt/varia/denyallfilter.h"
    "C:/Workspace/log4qt/src/log4qt/varia/levelmatchfilter.h"
    "C:/Workspace/log4qt/src/log4qt/varia/levelrangefilter.h"
    "C:/Workspace/log4qt/src/log4qt/varia/listappender.h"
    "C:/Workspace/log4qt/src/log4qt/varia/nullappender.h"
    "C:/Workspace/log4qt/src/log4qt/varia/stringmatchfilter.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Workspace/log4qt/build/src/log4qt/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
