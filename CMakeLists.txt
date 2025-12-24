cmake_minimum_required(VERSION 3.24...4.2)
include(FetchContent)

option(LIBTCOD_VCPKG "Enable Vcpkg for building libtcod locally." OFF)
option(LIBTCOD_SAMPLES "Build sources from the samples directory." OFF)
option(LIBTCOD_TESTS "Build unit tests." OFF)
option(LIBTCOD_INSTALL "Enable install targets." ON)
option(LIBTCOD_CPACK "Enable CPack for libtcod." OFF)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG 7f3ae3d57459e59943a4ecfefc8f6277ec6bf540 # release-3.2.28
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS CONFIG
)
set(SDL_INSTALL ${LIBTCOD_INSTALL} CACHE BOOL "")
set(SDL_INSTALL_CPACK OFF CACHE BOOL "")
set(SDL_INSTALL_DOCS OFF CACHE BOOL "")

FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib
    GIT_TAG 5a82f71ed1dfc0bec044d9702463dbdf84ea3b71 # v1.3.1.2
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS
)
set(ZLIB_BUILD_TESTING OFF CACHE BOOL "")
set(ZLIB_INSTALL ${LIBTCOD_INSTALL} CACHE BOOL "")

FetchContent_Declare(
    lodepng-c
    GIT_REPOSITORY https://github.com/lvandeve/lodepng
    GIT_TAG d41d4aa8c63dea277e25c94ad85046b6c5335ccc
    FIND_PACKAGE_ARGS CONFIG
)

FetchContent_Declare(
    utf8proc
    GIT_REPOSITORY https://github.com/JuliaStrings/utf8proc
    GIT_TAG 90daf9f396cfec91668758eb9cc54bd5248a6b89 # v2.11.2
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS
)
set(UTF8PROC_INSTALL ${LIBTCOD_INSTALL} CACHE BOOL "")

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb
    GIT_TAG f1c79c02822848a9bed4315b12c8c8f3761e1296
    FIND_PACKAGE_ARGS
)


if(LIBTCOD_VCPKG)
    if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE
            "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
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

add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
add_subdirectory(src)

set_target_properties(${PROJECT_NAME} PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN OFF
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

if(LIBTCOD_SAMPLES)
    add_subdirectory(samples)
endif()
if(LIBTCOD_TESTS)
    add_subdirectory(tests)
    list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()

if(LIBTCOD_CPACK)
    if (WIN32)
        set(CPACK_GENERATOR "ZIP")
    else()
        set(CPACK_GENERATOR "TGZ")
    endif()
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    include(CPack)
endif()
