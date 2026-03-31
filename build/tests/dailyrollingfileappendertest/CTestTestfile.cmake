# CMake generated Testfile for 
# Source directory: C:/Workspace/log4qt/tests/dailyrollingfileappendertest
# Build directory: C:/Workspace/log4qt/build/tests/dailyrollingfileappendertest
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[dailyrollingfileappendertest]=] "C:/Workspace/log4qt/build/bin/Debug/dailyrollingfileappendertest.exe")
  set_tests_properties([=[dailyrollingfileappendertest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/dailyrollingfileappendertest/CMakeLists.txt;12;add_test;C:/Workspace/log4qt/tests/dailyrollingfileappendertest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test([=[dailyrollingfileappendertest]=] "C:/Workspace/log4qt/build/bin/Release/dailyrollingfileappendertest.exe")
  set_tests_properties([=[dailyrollingfileappendertest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/dailyrollingfileappendertest/CMakeLists.txt;12;add_test;C:/Workspace/log4qt/tests/dailyrollingfileappendertest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test([=[dailyrollingfileappendertest]=] "C:/Workspace/log4qt/build/bin/MinSizeRel/dailyrollingfileappendertest.exe")
  set_tests_properties([=[dailyrollingfileappendertest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/dailyrollingfileappendertest/CMakeLists.txt;12;add_test;C:/Workspace/log4qt/tests/dailyrollingfileappendertest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test([=[dailyrollingfileappendertest]=] "C:/Workspace/log4qt/build/bin/RelWithDebInfo/dailyrollingfileappendertest.exe")
  set_tests_properties([=[dailyrollingfileappendertest]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Workspace/log4qt/tests/dailyrollingfileappendertest/CMakeLists.txt;12;add_test;C:/Workspace/log4qt/tests/dailyrollingfileappendertest/CMakeLists.txt;0;")
else()
  add_test([=[dailyrollingfileappendertest]=] NOT_AVAILABLE)
endif()
