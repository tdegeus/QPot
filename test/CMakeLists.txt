cmake_minimum_required(VERSION 3.19)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(QPot-test)
    find_package(QPot REQUIRED CONFIG)
endif()

option(USE_XSIMD "Use xsimd and 'march=native' optimisations" OFF)
option(USE_HDF5 "Use xsimd and 'march=native' optimisations" ON)

set(CMAKE_BUILD_TYPE Release)

set(test_name "unit-tests")

add_executable(${test_name} main.cpp Redraw.cpp Static.cpp)

find_package(Catch2 REQUIRED)
find_package(xtensor REQUIRED)

target_link_libraries(${test_name} PRIVATE Catch2::Catch2 QPot QPot::compiler_warnings QPot::assert)

if(USE_HDF5)
    set(HIGHFIVE_USE_BOOST 0)
    set(HIGHFIVE_USE_XTENSOR 1)
    find_package(HighFive REQUIRED)
    target_link_libraries(${test_name} PRIVATE HighFive)
    target_compile_definitions(${test_name} PRIVATE USE_HDF5)
endif()

if(USE_XSIMD)
    find_package(xsimd REQUIRED)
    target_link_libraries(${test_name} PRIVATE xsimd xtensor::optimize xtensor::use_xsimd)
endif()

add_test(NAME ${test_name} COMMAND ${test_name})

# allows running with ctest
set_tests_properties(${test_name} PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# allows running from the same directory as the binary
configure_file("Redraw_reconstruct-data.h5" "Redraw_reconstruct-data.h5" COPYONLY)
