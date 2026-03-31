# CMake generated Testfile for 
# Source directory: C:/Workspace/log4qt/tests/log4qttest
# Build directory: C:/Workspace/log4qt/build/tests/log4qttest
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[log4qttest]=] "C:/Workspace/log4qt/build/bin/Debug/log4qttest.exe")
  set_tests_properties([=[log4qttest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/log4qttest/CMakeLists.txt;11;add_test;C:/Workspace/log4qt/tests/log4qttest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test([=[log4qttest]=] "C:/Workspace/log4qt/build/bin/Release/log4qttest.exe")
  set_tests_properties([=[log4qttest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/log4qttest/CMakeLists.txt;11;add_test;C:/Workspace/log4qt/tests/log4qttest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test([=[log4qttest]=] "C:/Workspace/log4qt/build/bin/MinSizeRel/log4qttest.exe")
  set_tests_properties([=[log4qttest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/log4qttest/CMakeLists.txt;11;add_test;C:/Workspace/log4qt/tests/log4qttest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test([=[log4qttest]=] "C:/Workspace/log4qt/build/bin/RelWithDebInfo/log4qttest.exe")
  set_tests_properties([=[log4qttest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/log4qttest/CMakeLists.txt;11;add_test;C:/Workspace/log4qt/tests/log4qttest/CMakeLists.txt;0;")
else()
  add_test([=[log4qttest]=] NOT_AVAILABLE)
endif()
