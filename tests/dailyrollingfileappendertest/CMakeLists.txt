find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

set(TEST_NAME dailyrollingfileappendertest)

set(dfa_SOURCES
    dailyrollingfileappendertest.cpp
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(${TEST_NAME} ${dfa_SOURCES})
target_link_libraries(${TEST_NAME} log4qt Qt${QT_VERSION_MAJOR}::Test)

add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
