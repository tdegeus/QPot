cmake_minimum_required(VERSION 3.19..3.21)

set(MYPROJECT "${PROJECT_NAME}-test")

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(QPot-test)
    find_package(QPot REQUIRED CONFIG)
    option(USE_ASSERT "${PROJECT_NAME}: Build with assertions" ON)
    option(USE_DEBUG "${PROJECT_NAME}: Build in debug mode" OFF)
    option(USE_SIMD "${PROJECT_NAME}: Build with hardware optimization" OFF)
    option(USE_HDF5 "${PROJECT_NAME}: Build HDF5-based tests" ON)
    set(MYPROJECT "${PROJECT_NAME}")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(mytest "unit-tests")

add_executable(${mytest} main.cpp Chunked.cpp Redraw.cpp Static.cpp)

find_package(Catch2 REQUIRED)
find_package(xtensor REQUIRED)

target_link_libraries(${mytest} PRIVATE Catch2::Catch2 QPot QPot::debug QPot::compiler_warnings)

if(USE_HDF5)
    set(HIGHFIVE_USE_BOOST 0)
    set(HIGHFIVE_USE_XTENSOR 1)
    find_package(HighFive REQUIRED)
    target_link_libraries(${mytest} PRIVATE HighFive)
    target_compile_definitions(${mytest} PRIVATE USE_HDF5)
endif()

if(USE_ASSERT)
    target_link_libraries(${mytest} PRIVATE QPot::assert)
    message(STATUS "Compiling ${MYPROJECT} with assertions")
endif()

if(USE_DEBUG)
    target_link_libraries(${mytest} PRIVATE QPot::debug)
    message(STATUS "Compiling ${MYPROJECT} in debug mode")
endif()

if(USE_SIMD)
    find_package(xsimd REQUIRED)
    target_link_libraries(${mytest} PRIVATE xtensor::use_xsimd xtensor::optimize)
    message(STATUS "Compiling ${MYPROJECT} with hardware optimization")
endif()

add_test(NAME ${mytest} COMMAND ${mytest})

# allows running with ctest
set_tests_properties(${mytest} PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# allows running from the same directory as the binary
configure_file("Redraw_reconstruct-data.h5" "Redraw_reconstruct-data.h5" COPYONLY)
