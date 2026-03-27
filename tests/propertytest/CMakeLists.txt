find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

set(l4qt_SOURCES
    tst_propertyconfigurator.cpp
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(propertytest ${l4qt_SOURCES})
target_link_libraries(propertytest log4qt Qt${QT_VERSION_MAJOR}::Test)

add_test(NAME propertytest COMMAND $<TARGET_FILE:propertytest>)
