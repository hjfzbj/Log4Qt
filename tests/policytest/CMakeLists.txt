find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

set(l4qt_SOURCES
    tst_policytest.cpp
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(policytest ${l4qt_SOURCES})
target_link_libraries(policytest log4qt Qt${QT_VERSION_MAJOR}::Test)

add_test(NAME policytest COMMAND $<TARGET_FILE:policytest>)
