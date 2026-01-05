cmake_minimum_required(VERSION 3.24...4.2)
include(FetchContent)

option(LIBTCOD_VCPKG "Enable Vcpkg for building libtcod locally." OFF)
option(LIBTCOD_SAMPLES "Build sources from the samples directory." OFF)
option(LIBTCOD_TESTS "Build unit tests." OFF)
option(LIBTCOD_INSTALL "Enable install targets." ON)
option(LIBTCOD_CPACK "Enable CPack for libtcod." OFF)

option(LIBTCOD_THREADS "If true then deprecated thread functions are enabled by libtcod." ON)
option(LIBTCOD_SDL3 "Enable or disable this library dependency." ON)
option(LIBTCOD_ZLIB "Enable or disable this library dependency." ON)
set(LIBTCOD_LODEPNG "ON" CACHE STRING "How this library will be linked.")
set(LIBTCOD_UTF8PROC "ON" CACHE STRING "How this library will be linked.")
set(LIBTCOD_STB "ON" CACHE STRING "How this library will be linked.")

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG f5e5f6588921eed3d7d048ce43d9eb1ff0da0ffc # release-3.2.30
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS CONFIG
)
set(SDL_INSTALL ${LIBTCOD_INSTALL} CACHE BOOL "")
set(SDL_INSTALL_CPACK OFF CACHE BOOL "")
set(SDL_INSTALL_DOCS OFF CACHE BOOL "")

FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib
    GIT_TAG 570720b0c24f9686c33f35a1b3165c1f568b96be # v1.3.1.2
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS
)
set(ZLIB_BUILD_TESTING OFF CACHE BOOL "")
set(ZLIB_INSTALL ${LIBTCOD_INSTALL} CACHE BOOL "")

FetchContent_Declare(
    lodepng-c
    GIT_REPOSITORY https://github.com/lvandeve/lodepng
    GIT_TAG d41d4aa8c63dea277e25c94ad85046b6c5335ccc
    PATCH_COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/cmake/lodepng-c.patch
    FIND_PACKAGE_ARGS CONFIG
)

FetchContent_Declare(
    utf8proc
    GIT_REPOSITORY https://github.com/JuliaStrings/utf8proc
    GIT_TAG e5e799221b45bbb90f5fdc5c69b6b8dfbf017e78 # v2.11.3
    GIT_SHALLOW TRUE
    PATCH_COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/cmake/utf8proc.patch
    FIND_PACKAGE_ARGS
)
set(UTF8PROC_INSTALL ${LIBTCOD_INSTALL} CACHE BOOL "")

FetchContent_Declare(
    Stb
    GIT_REPOSITORY https://github.com/nothings/stb
    GIT_TAG f1c79c02822848a9bed4315b12c8c8f3761e1296
    PATCH_COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/cmake/stb.patch
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
