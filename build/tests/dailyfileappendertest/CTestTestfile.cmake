# CMake generated Testfile for 
# Source directory: C:/Workspace/log4qt/tests/dailyfileappendertest
# Build directory: C:/Workspace/log4qt/build/tests/dailyfileappendertest
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[dailyfileappendertest]=] "C:/Workspace/log4qt/build/bin/Debug/dailyfileappendertest.exe")
  set_tests_properties([=[dailyfileappendertest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/dailyfileappendertest/CMakeLists.txt;12;add_test;C:/Workspace/log4qt/tests/dailyfileappendertest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test([=[dailyfileappendertest]=] "C:/Workspace/log4qt/build/bin/Release/dailyfileappendertest.exe")
  set_tests_properties([=[dailyfileappendertest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/dailyfileappendertest/CMakeLists.txt;12;add_test;C:/Workspace/log4qt/tests/dailyfileappendertest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test([=[dailyfileappendertest]=] "C:/Workspace/log4qt/build/bin/MinSizeRel/dailyfileappendertest.exe")
  set_tests_properties([=[dailyfileappendertest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/dailyfileappendertest/CMakeLists.txt;12;add_test;C:/Workspace/log4qt/tests/dailyfileappendertest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test([=[dailyfileappendertest]=] "C:/Workspace/log4qt/build/bin/RelWithDebInfo/dailyfileappendertest.exe")
  set_tests_properties([=[dailyfileappendertest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/dailyfileappendertest/CMakeLists.txt;12;add_test;C:/Workspace/log4qt/tests/dailyfileappendertest/CMakeLists.txt;0;")
else()
  add_test([=[dailyfileappendertest]=] NOT_AVAILABLE)
endif()
