cmake_minimum_required (VERSION 3.13...3.21)

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

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake OPTIONAL RESULT_VARIABLE CONAN_FILE)
if(CONAN_FILE)
    conan_basic_setup(TARGETS)
endif()

if(NOT CMAKE_INSTALL_BINDIR)
    set(CMAKE_INSTALL_BINDIR "bin" CACHE STRING "")
endif()
if(NOT CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib" CACHE STRING "")
endif()
if(NOT CMAKE_INSTALL_DATAROOTDIR)
    set(CMAKE_INSTALL_DATAROOTDIR "share" CACHE STRING "")
endif()
if(NOT CMAKE_INSTALL_INCLUDEDIR)
    set(CMAKE_INSTALL_INCLUDEDIR "include" CACHE STRING "")
endif()

if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
endif()
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
endif()

set(LIBTCOD_SAMPLES OFF CACHE BOOL "Build sources from the samples directory.")
set(LIBTCOD_TESTS OFF CACHE BOOL "Build unit tests.")

add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
add_subdirectory(src)

if(LIBTCOD_SAMPLES)
    add_subdirectory(samples)
endif()
if(LIBTCOD_TESTS)
    add_subdirectory(tests)
endif()
