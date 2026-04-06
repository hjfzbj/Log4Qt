find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(jsonlayouttest tst_jsonlayout.cpp)
target_link_libraries(jsonlayouttest log4qt Qt${QT_VERSION_MAJOR}::Test)

add_test(NAME jsonlayouttest COMMAND $<TARGET_FILE:jsonlayouttest>)
