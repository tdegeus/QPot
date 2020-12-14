
cmake_minimum_required(VERSION 3.0)

# Basic settings
# ==============

project(QPot)

option(BUILD_TESTS "${PROJECT_NAME} Build tests" OFF)

# Version
# =======

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/include/QPot/config.h" _QPOT_ver
     REGEX "#define QPOT_VERSION_(MAJOR|MINOR|PATCH)")

foreach(ver ${_QPOT_vers})
    if(ver MATCHES "#define QPOT_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(_QPOT_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}"  CACHE INTERNAL "")
    endif()
endforeach()

set(_QPOT_VERSION ${_QPOT_VERSION_MAJOR}.${_QPOT_VERSION_MINOR}.${_QPOT_VERSION_PATCH})

message(STATUS "Building QPot v${_QPOT_VERSION}")

# Set target
# ==========

find_package(xtensor REQUIRED)

add_library(QPot INTERFACE)

target_include_directories(QPot INTERFACE
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(QPot INTERFACE xtensor)

# Installation
# ============

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/" DESTINATION include)

install(TARGETS QPot EXPORT QPot-targets)

install(
    EXPORT QPot-targets
    FILE QPotTargets.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/QPot")

set(_QPOT ${CMAKE_SIZEOF_VOID_P})
unset(CMAKE_SIZEOF_VOID_P)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/QPotConfigVersion.cmake"
    VERSION ${_QPOT_VERSION}
    COMPATIBILITY AnyNewerVersion)

set(CMAKE_SIZEOF_VOID_P ${_QPOT})

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/QPotConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/QPotConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/QPot")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/QPot.pc.in"
               "${CMAKE_CURRENT_BINARY_DIR}/QPot.pc" @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/QPot.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig/")

# Add builds
# ==========

include("QPotConfig.cmake")

set(CTEST_TEST_TARGET_ALIAS RUN_ALL_TESTS)
include(CTest)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
