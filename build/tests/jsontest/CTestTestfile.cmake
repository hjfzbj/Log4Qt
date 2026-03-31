# CMake generated Testfile for 
# Source directory: C:/Workspace/log4qt/tests/jsontest
# Build directory: C:/Workspace/log4qt/build/tests/jsontest
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[jsontest]=] "C:/Workspace/log4qt/build/bin/Debug/jsontest.exe")
  set_tests_properties([=[jsontest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/jsontest/CMakeLists.txt;10;add_test;C:/Workspace/log4qt/tests/jsontest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test([=[jsontest]=] "C:/Workspace/log4qt/build/bin/Release/jsontest.exe")
  set_tests_properties([=[jsontest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/jsontest/CMakeLists.txt;10;add_test;C:/Workspace/log4qt/tests/jsontest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test([=[jsontest]=] "C:/Workspace/log4qt/build/bin/MinSizeRel/jsontest.exe")
  set_tests_properties([=[jsontest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/jsontest/CMakeLists.txt;10;add_test;C:/Workspace/log4qt/tests/jsontest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test([=[jsontest]=] "C:/Workspace/log4qt/build/bin/RelWithDebInfo/jsontest.exe")
  set_tests_properties([=[jsontest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/jsontest/CMakeLists.txt;10;add_test;C:/Workspace/log4qt/tests/jsontest/CMakeLists.txt;0;")
else()
  add_test([=[jsontest]=] NOT_AVAILABLE)
endif()
