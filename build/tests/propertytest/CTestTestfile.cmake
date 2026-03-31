# CMake generated Testfile for 
# Source directory: C:/Workspace/log4qt/tests/propertytest
# Build directory: C:/Workspace/log4qt/build/tests/propertytest
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[propertytest]=] "C:/Workspace/log4qt/build/bin/Debug/propertytest.exe")
  set_tests_properties([=[propertytest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/propertytest/CMakeLists.txt;10;add_test;C:/Workspace/log4qt/tests/propertytest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test([=[propertytest]=] "C:/Workspace/log4qt/build/bin/Release/propertytest.exe")
  set_tests_properties([=[propertytest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/propertytest/CMakeLists.txt;10;add_test;C:/Workspace/log4qt/tests/propertytest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test([=[propertytest]=] "C:/Workspace/log4qt/build/bin/MinSizeRel/propertytest.exe")
  set_tests_properties([=[propertytest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/propertytest/CMakeLists.txt;10;add_test;C:/Workspace/log4qt/tests/propertytest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test([=[propertytest]=] "C:/Workspace/log4qt/build/bin/RelWithDebInfo/propertytest.exe")
  set_tests_properties([=[propertytest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/propertytest/CMakeLists.txt;10;add_test;C:/Workspace/log4qt/tests/propertytest/CMakeLists.txt;0;")
else()
  add_test([=[propertytest]=] NOT_AVAILABLE)
endif()
