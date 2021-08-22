cmake_minimum_required(VERSION 3.1..3.19)

set(MYPROJECT "${PROJECT_NAME}-examples")

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(QPot-examples)
    find_package(QPot REQUIRED CONFIG)
    option(USE_ASSERT "${PROJECT_NAME}: Build with QPot::assert" ON)
    option(USE_DEBUG "${PROJECT_NAME}: Build with QPot::debug" OFF)
    option(USE_SIMD "${PROJECT_NAME}: Use xsimd and -march=native if available" OFF)
    set(MYPROJECT "${PROJECT_NAME}")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(prrng REQUIRED)

add_library(mytarget INTERFACE IMPORTED)

target_link_libraries(mytarget INTERFACE QPot prrng QPot::debug QPot::compiler_warnings)

if(USE_ASSERT)
    target_link_libraries(mytarget INTERFACE QPot::assert)
    message(STATUS "Compiling ${MYPROJECT} with assertions")
endif()

if(USE_DEBUG)
    target_link_libraries(mytarget INTERFACE QPot::debug)
    message(STATUS "Compiling ${MYPROJECT} in debug mode")
endif()

if(USE_SIMD)
    find_package(xsimd REQUIRED)
    target_link_libraries(mytarget INTERFACE xtensor::use_xsimd xtensor::optimize)
    message(STATUS "Compiling ${MYPROJECT} with hardware optimization")
endif()

set(myexec "frozen_sequence")
set(mysource "frozen_sequence/main.cpp")
add_executable(${myexec} ${mysource})
target_link_libraries(${myexec} PRIVATE mytarget)
add_test(NAME ${myexec} COMMAND ${myexec})
