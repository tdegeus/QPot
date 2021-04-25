cmake_minimum_required(VERSION 3.1..3.19)

project(main)

set(CMAKE_BUILD_TYPE Release CACHE STRING "Release mode")

set(HIGHFIVE_USE_BOOST 0)
set(HIGHFIVE_USE_XTENSOR 1)

find_package(QPot REQUIRED)
find_package(prrng REQUIRED)
find_package(HighFive REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE QPot prrng QPot::assert QPot::debug HighFive)
