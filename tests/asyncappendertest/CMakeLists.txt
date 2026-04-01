find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

set(l4qt_SOURCES
    tst_asyncappender.cpp
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(asyncappendertest ${l4qt_SOURCES})
target_link_libraries(asyncappendertest log4qt Qt${QT_VERSION_MAJOR}::Test)

add_test(NAME asyncappendertest COMMAND $<TARGET_FILE:asyncappendertest>)
