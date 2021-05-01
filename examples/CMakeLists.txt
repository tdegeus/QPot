cmake_minimum_required(VERSION 3.1..3.19)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(QPot-examples)
    find_package(QPot REQUIRED CONFIG)
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

find_package(prrng REQUIRED)

add_library(libraries INTERFACE IMPORTED)

target_link_libraries(libraries INTERFACE QPot prrng QPot::debug QPot::compiler_warnings)

set(exec "frozen_sequence")
set(source "frozen_sequence/main.cpp")
add_executable(${exec} ${source})
target_link_libraries(${exec} PRIVATE libraries)
add_test(NAME ${exec} COMMAND ${exec})
