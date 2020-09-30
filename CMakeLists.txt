cmake_minimum_required (VERSION 3.9)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT})
        message(STATUS "Using Vcpkg: $ENV{VCPKG_ROOT}")
        set(CMAKE_TOOLCHAIN_FILE
            "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING ""
        )
    else()
        message(STATUS "Vcpkg not being used.")
    endif()
endif()

file(STRINGS src/libtcod/version.h LIBTCOD_VERSION_LINE REGEX "TCOD_STRVERSION")
string(REGEX MATCH "TCOD_STRVERSION \"([^\"]+)\"" LIBTCOD_VERSION_LINE ${LIBTCOD_VERSION_LINE})
set(LIBTCOD_VERSION_FULL ${CMAKE_MATCH_1})
string(REGEX MATCH "([0-9]+\.[0-9]+\.[0-9]+)" LIBTCOD_VERSION ${LIBTCOD_VERSION_FULL})
message(STATUS "Libtcod version: ${LIBTCOD_VERSION}")

project(
    libtcod
    VERSION ${LIBTCOD_VERSION}
    LANGUAGES C CXX
)

set(BUILD_SAMPLES OFF CACHE BOOL "Build sources from the samples directory.")
set(BUILD_TESTS OFF CACHE BOOL "Build unit tests.")

add_subdirectory(src)
if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
