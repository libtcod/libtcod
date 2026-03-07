cmake_minimum_required(VERSION 3.24...4.2)
include(FetchContent)
include(GNUInstallDirs)

option(LIBTCOD_VCPKG "Enable Vcpkg toolchain by default." OFF)
option(LIBTCOD_SAMPLES "Build sources from the samples directory." OFF)
option(LIBTCOD_TESTS "Build unit tests." OFF)
option(LIBTCOD_INSTALL "Enable install targets." ON)
option(LIBTCOD_INSTALL_PKGCONFIG "Install pkg-config files." ON)
option(LIBTCOD_CPACK "Enable CPack for libtcod." OFF)
option(LIBTCOD_DOCS "Build documentation using Doxygen for installation." OFF)

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
    set(CMAKE_TOOLCHAIN_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
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

target_compile_features(${PROJECT_NAME} PUBLIC c_std_99 cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    PREFIX "" # Remove the "lib" prefix to prevent a library name like "liblibtcod"
    VISIBILITY_INLINES_HIDDEN OFF
    VERIFY_INTERFACE_HEADER_SETS TRUE
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

macro(fix_deprecated_link_setting LINK_OPTION)  # Replaces find_package/disable with ON/OFF
    if(${LINK_OPTION} STREQUAL "find_package")
        message(DEPRECATION "${LINK_OPTION}=find_package is deprecated, this should be ${LINK_OPTION}=ON")
        set(${LINK_OPTION} "ON")
    elseif(${LINK_OPTION} STREQUAL "disable")
        message(DEPRECATION "${LINK_OPTION}=disable is deprecated, this should be ${LINK_OPTION}=OFF")
        set(${LINK_OPTION} "OFF")
    endif()
endmacro()

fix_deprecated_link_setting(LIBTCOD_SDL3)
if(LIBTCOD_SDL3)
    list(APPEND VCPKG_MANIFEST_FEATURES "sdl")
    FetchContent_MakeAvailable(SDL3)
    target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC NO_SDL)
endif()

fix_deprecated_link_setting(LIBTCOD_ZLIB)
if(LIBTCOD_ZLIB)
    FetchContent_MakeAvailable(ZLIB)
    target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC TCOD_NO_ZLIB)
endif()

set_property(CACHE LIBTCOD_LODEPNG PROPERTY STRINGS "vendored" "ON" "OFF")
fix_deprecated_link_setting(LIBTCOD_LODEPNG)
if(LIBTCOD_LODEPNG STREQUAL "vendored")
    message("Will be vendored: LodePNG")
    target_sources(${PROJECT_NAME} PRIVATE "vendor/lodepng.c")
    target_include_directories(${PROJECT_NAME} PRIVATE "vendor/")
elseif(LIBTCOD_LODEPNG)
    FetchContent_MakeAvailable(lodepng-c)
    target_link_libraries(${PROJECT_NAME} PRIVATE lodepng-c)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC TCOD_NO_PNG)
endif()

set_property(CACHE LIBTCOD_UTF8PROC PROPERTY STRINGS "vendored" "ON" "OFF")
if(LIBTCOD_UTF8PROC STREQUAL "vcpkg")
    message(DEPRECATION "LIBTCOD_UTF8PROC=vcpkg is deprecated, this should be LIBTCOD_UTF8PROC=ON")
    set(LIBTCOD_UTF8PROC "ON")
endif()
fix_deprecated_link_setting(LIBTCOD_UTF8PROC)
if(LIBTCOD_UTF8PROC STREQUAL "vendored")
    message("Will be vendored: utf8proc")
    target_include_directories(${PROJECT_NAME} PRIVATE "vendor/utf8proc")
    target_sources(${PROJECT_NAME} PRIVATE "vendor/utf8proc/utf8proc.c")
elseif(LIBTCOD_UTF8PROC)
    FetchContent_MakeAvailable(utf8proc)
    target_link_libraries(${PROJECT_NAME} PRIVATE utf8proc::utf8proc)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC TCOD_NO_UNICODE)
endif()

set_property(CACHE LIBTCOD_STB PROPERTY STRINGS "vendored" "ON")
if(LIBTCOD_STB STREQUAL "vcpkg")
    message(DEPRECATION "LIBTCOD_STB=vcpkg is deprecated, this should be LIBTCOD_STB=ON")
    set(LIBTCOD_STB "ON")
endif()
fix_deprecated_link_setting(LIBTCOD_STB)
if(LIBTCOD_STB STREQUAL "vendored")
    message("Will be vendored: stb")
    target_include_directories(${PROJECT_NAME} PRIVATE "vendor/")
else()
    FetchContent_MakeAvailable(Stb)
    target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})
endif()

if(LIBTCOD_THREADS AND NOT CMAKE_DISABLE_FIND_PACKAGE_Threads)
    find_package(Threads REQUIRED)
    if(DEFINED CMAKE_THREAD_LIBS_INIT)
        target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_THREAD_LIBS_INIT})
    endif()
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC TCOD_NO_THREADS)
endif()

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LIBTCOD_EXPORTS)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC LIBTCOD_STATIC)
endif()

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE TCOD_IGNORE_DEPRECATED)

add_subdirectory(src)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
  target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

if(LIBTCOD_DOCS)
    message(STATUS "Preparing Doxygen")
    find_package(Doxygen REQUIRED)
    doxygen_add_docs(
        ${PROJECT_NAME}_docs
        ALL
        COMMENT "Generating documentation using Doxygen ${Doxygen_VERSION}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
        CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
    )
    if(LIBTCOD_INSTALL)
        install(
            DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}/docs/html/
            TYPE DOC
        )
    endif()
endif()

if(LIBTCOD_INSTALL)
    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        FILE_SET ${PROJECT_NAME}_header_set
        RUNTIME LIBRARY ARCHIVE
        COMPONENT Library
    )
    include(CMakePackageConfigHelpers)
    install(
        EXPORT ${PROJECT_NAME}Targets
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
        NAMESPACE ${PROJECT_NAME}::
    )
    configure_package_config_file(
        src/${PROJECT_NAME}Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
        PATH_VARS CMAKE_INSTALL_INCLUDEDIR)
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        COMPATIBILITY SameMajorVersion
    )
    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
    )
    install(
        FILES "LICENSE.txt"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
        RENAME copyright
    )

    if (LIBTCOD_INSTALL_PKGCONFIG)
        set(prefix "${CMAKE_INSTALL_PREFIX}")
        set(exec_prefix "\${prefix}")
        set(libdir "\${exec_prefix}/${CMAKE_INSTALL_LIBDIR}")
        set(includedir "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}")
        set(PACKAGE_VERSION "${LIBTCOD_VERSION_FULL}")
        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/buildsys/autotools/libtcod.pc.in
            ${CMAKE_CURRENT_BINARY_DIR}/libtcod.pc
            @ONLY
            NEWLINE_STYLE UNIX
        )
        install(
            FILES ${CMAKE_CURRENT_BINARY_DIR}/libtcod.pc
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        )
    endif()
endif()

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
