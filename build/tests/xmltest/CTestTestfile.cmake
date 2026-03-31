# CMake generated Testfile for 
# Source directory: C:/Workspace/log4qt/tests/xmltest
# Build directory: C:/Workspace/log4qt/build/tests/xmltest
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[xmltest]=] "C:/Workspace/log4qt/build/bin/Debug/xmltest.exe")
  set_tests_properties([=[xmltest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/xmltest/CMakeLists.txt;10;add_test;C:/Workspace/log4qt/tests/xmltest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test([=[xmltest]=] "C:/Workspace/log4qt/build/bin/Release/xmltest.exe")
  set_tests_properties([=[xmltest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/xmltest/CMakeLists.txt;10;add_test;C:/Workspace/log4qt/tests/xmltest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test([=[xmltest]=] "C:/Workspace/log4qt/build/bin/MinSizeRel/xmltest.exe")
  set_tests_properties([=[xmltest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/xmltest/CMakeLists.txt;10;add_test;C:/Workspace/log4qt/tests/xmltest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test([=[xmltest]=] "C:/Workspace/log4qt/build/bin/RelWithDebInfo/xmltest.exe")
  set_tests_properties([=[xmltest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/xmltest/CMakeLists.txt;10;add_test;C:/Workspace/log4qt/tests/xmltest/CMakeLists.txt;0;")
else()
  add_test([=[xmltest]=] NOT_AVAILABLE)
endif()
