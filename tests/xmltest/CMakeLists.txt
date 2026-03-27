find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

set(l4qt_SOURCES
    tst_xmlconfigurator.cpp
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(xmltest ${l4qt_SOURCES})
target_link_libraries(xmltest log4qt Qt${QT_VERSION_MAJOR}::Test)

add_test(NAME xmltest COMMAND $<TARGET_FILE:xmltest>)
