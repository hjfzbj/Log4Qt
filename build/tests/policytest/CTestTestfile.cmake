# CMake generated Testfile for 
# Source directory: C:/Workspace/log4qt/tests/policytest
# Build directory: C:/Workspace/log4qt/build/tests/policytest
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[policytest]=] "C:/Workspace/log4qt/build/bin/Debug/policytest.exe")
  set_tests_properties([=[policytest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/policytest/CMakeLists.txt;10;add_test;C:/Workspace/log4qt/tests/policytest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test([=[policytest]=] "C:/Workspace/log4qt/build/bin/Release/policytest.exe")
  set_tests_properties([=[policytest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/policytest/CMakeLists.txt;10;add_test;C:/Workspace/log4qt/tests/policytest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test([=[policytest]=] "C:/Workspace/log4qt/build/bin/MinSizeRel/policytest.exe")
  set_tests_properties([=[policytest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/policytest/CMakeLists.txt;10;add_test;C:/Workspace/log4qt/tests/policytest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test([=[policytest]=] "C:/Workspace/log4qt/build/bin/RelWithDebInfo/policytest.exe")
  set_tests_properties([=[policytest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/policytest/CMakeLists.txt;10;add_test;C:/Workspace/log4qt/tests/policytest/CMakeLists.txt;0;")
else()
  add_test([=[policytest]=] NOT_AVAILABLE)
endif()
