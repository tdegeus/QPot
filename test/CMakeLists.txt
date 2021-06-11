cmake_minimum_required(VERSION 3.19)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(QPot-test)
    find_package(QPot REQUIRED CONFIG)
endif()

option(HDF5 "Compile HDF5-based tests" ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

set(test_name "unit-tests")

add_executable(${test_name} main.cpp Chunked.cpp Redraw.cpp Static.cpp)

find_package(Catch2 REQUIRED)
find_package(xtensor REQUIRED)

target_link_libraries(${test_name} PRIVATE Catch2::Catch2 QPot QPot::debug QPot::compiler_warnings)

if(USE_HDF5)
    set(HIGHFIVE_USE_BOOST 0)
    set(HIGHFIVE_USE_XTENSOR 1)
    find_package(HighFive REQUIRED)
    target_link_libraries(${test_name} PRIVATE HighFive)
    target_compile_definitions(${test_name} PRIVATE USE_HDF5 H5_BUILT_AS_DYNAMIC_LIB)
endif()

add_test(NAME ${test_name} COMMAND ${test_name})

# allows running with ctest
set_tests_properties(${test_name} PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# allows running from the same directory as the binary
configure_file("Redraw_reconstruct-data.h5" "Redraw_reconstruct-data.h5" COPYONLY)
