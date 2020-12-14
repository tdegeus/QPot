cmake_minimum_required(VERSION 3.0)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(QPot-test)
    find_package(QPot REQUIRED CONFIG)
endif()

option(XSIMD "Use xsimd and 'march=native' optimisations" OFF)

set(CMAKE_BUILD_TYPE Release)

set(test_name "unit-tests")

find_package(Catch2 REQUIRED)
find_package(xtensor REQUIRED)

add_executable(${test_name} main.cpp Redraw.cpp Static.cpp)

target_link_libraries(${test_name} PRIVATE Catch2::Catch2 QPot)
target_link_libraries(${test_name} PRIVATE QPot::compiler_warnings)
target_link_libraries(${test_name} PRIVATE QPot::assert)

if(XSIMD)
    target_link_libraries(${test_name} PRIVATE xtensor::optimize xtensor::use_xsimd)
endif()

add_test(NAME ${test_name} COMMAND ${test_name})
