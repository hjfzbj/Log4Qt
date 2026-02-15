find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

set(performancetest_SOURCES
    performancetest.cpp
    performancetest.h
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(performancetest ${performancetest_SOURCES})
target_link_libraries(performancetest log4qt Qt${QT_VERSION_MAJOR}::Test)

add_test(NAME performancetest COMMAND $<TARGET_FILE:performancetest>)
