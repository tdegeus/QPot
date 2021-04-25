# QPot cmake module
#
# This module sets the target:
#
#     QPot
#
# In addition, it sets the following variables:
#
#     QPot_FOUND - true if the library is found
#     QPot_VERSION - the library's version
#     QPot_INCLUDE_DIRS - directory containing the library's headers
#
# The following support targets are defined to simplify things:
#
#     QPot::compiler_warnings - enable compiler warnings
#     QPot::assert - enable library assertions
#     QPot::debug - enable all assertions (slow)

include(CMakeFindDependencyMacro)

# Define target "QPot"

if(NOT TARGET QPot)
    include("${CMAKE_CURRENT_LIST_DIR}/QPotTargets.cmake")
endif()

# Define "QPot_INCLUDE_DIRS"

get_target_property(
    QPot_INCLUDE_DIRS
    QPot
    INTERFACE_INCLUDE_DIRECTORIES)

# Find dependencies

find_dependency(xtensor)

# Define support target "QPot::compiler_warnings"

if(NOT TARGET QPot::compiler_warnings)
    add_library(QPot::compiler_warnings INTERFACE IMPORTED)
    if(MSVC)
        set_property(
            TARGET QPot::compiler_warnings
            PROPERTY INTERFACE_COMPILE_OPTIONS
            /W4)
    else()
        set_property(
            TARGET QPot::compiler_warnings
            PROPERTY INTERFACE_COMPILE_OPTIONS
            -Wall -Wextra -pedantic -Wno-unknown-pragmas)
    endif()
endif()

# Define support target "QPot::assert"

if(NOT TARGET QPot::assert)
    add_library(QPot::assert INTERFACE IMPORTED)
    set_property(
        TARGET QPot::assert
        PROPERTY INTERFACE_COMPILE_DEFINITIONS
        QPOT_ENABLE_ASSERT)
endif()

# Define support target "QPot::debug"

if(NOT TARGET QPot::debug)
    add_library(QPot::debug INTERFACE IMPORTED)
    set_property(
        TARGET QPot::debug
        PROPERTY INTERFACE_COMPILE_DEFINITIONS
        XTENSOR_ENABLE_ASSERT QPOT_ENABLE_ASSERT QPOT_ENABLE_DEBUG)
endif()
