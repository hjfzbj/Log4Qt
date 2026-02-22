find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Performance test suite
add_executable(performancetest
    performancetest.cpp
    performancetest.h
)
target_link_libraries(performancetest log4qt Qt${QT_VERSION_MAJOR}::Test)
add_test(NAME performancetest COMMAND $<TARGET_FILE:performancetest>)

# QString benchmark
add_executable(qstring_benchmark
    qstring_benchmark.cpp
    qstring_benchmark.h
)
target_link_libraries(qstring_benchmark log4qt Qt${QT_VERSION_MAJOR}::Test)
add_test(NAME qstring_benchmark COMMAND $<TARGET_FILE:qstring_benchmark>)
