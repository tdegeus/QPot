cmake_minimum_required(VERSION 3.19)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(QPot-test)
    find_package(QPot REQUIRED CONFIG)
    option(USE_ASSERT "${PROJECT_NAME}: Build with QPot::assert" ON)
    option(USE_DEBUG "${PROJECT_NAME}: Build with QPot::debug" OFF)
    option(USE_SIMD "${PROJECT_NAME}: Use xsimd and -march=native if available" OFF)
    option(USE_HDF5 "${PROJECT_NAME}: Build HDF5-based tests" ON)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(testname "unit-tests")

add_executable(${testname} main.cpp Chunked.cpp Redraw.cpp Static.cpp)

find_package(Catch2 REQUIRED)
find_package(xtensor REQUIRED)

target_link_libraries(${testname} PRIVATE Catch2::Catch2 QPot QPot::debug QPot::compiler_warnings)

if(USE_HDF5)
    set(HIGHFIVE_USE_BOOST 0)
    set(HIGHFIVE_USE_XTENSOR 1)
    find_package(HighFive REQUIRED)
    target_link_libraries(${testname} PRIVATE HighFive)
    target_compile_definitions(${testname} PRIVATE USE_HDF5)
endif()

if(USE_ASSERT)
    target_link_libraries(${testname} PRIVATE QPot::assert)
    message(STATUS "Compiling ${PROJECT_NAME}-test with QPot::assert")
endif()

if(USE_DEBUG)
    target_link_libraries(${testname} PRIVATE QPot::debug)
    message(STATUS "Compiling ${PROJECT_NAME}-test with QPot::debug")
endif()

if(USE_SIMD)
    find_package(xsimd REQUIRED)
    target_link_libraries(${testname} PRIVATE xtensor::use_xsimd xtensor::optimize)
    message(STATUS "Compiling ${PROJECT_NAME}-test with hardware optimization")
endif()

add_test(NAME ${testname} COMMAND ${testname})

# allows running with ctest
set_tests_properties(${testname} PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# allows running from the same directory as the binary
configure_file("Redraw_reconstruct-data.h5" "Redraw_reconstruct-data.h5" COPYONLY)
