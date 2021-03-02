cmake_minimum_required (VERSION 3.13)

set(CMAKE_INSTALL_LIBDIR ${CMAKE_CURRENT_BINARY_DIR}/lib CACHE STRING "")
set(CMAKE_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/libtcod/cmake CACHE STRING "")
set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_CURRENT_BINARY_DIR}/include CACHE STRING "")

include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED)

if(IPO_SUPPORTED)
    set_property(TARGET LIBTCOD PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    set_property(TARGET LIBTCOD PROPERTY INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE)
endif()

target_compile_features(LIBTCOD PUBLIC c_std_99 cxx_std_11)

set(LIBTCOD_SDL2 "find_package" CACHE STRING "How this library will be linked.")
set(LIBTCOD_ZLIB "find_package" CACHE STRING "How this library will be linked.")

if(CONAN_FILE)
    message(STATUS "Fetching dependencies from Conan.")
    set(LIBTCOD_SDL2 "conan")
    set(LIBTCOD_ZLIB "conan")
endif()

set_property(CACHE LIBTCOD_SDL2 PROPERTY STRINGS "find_package" "conan")
if(LIBTCOD_SDL2 STREQUAL "find_package")
    find_package(SDL2 CONFIG REQUIRED)
    target_link_libraries(LIBTCOD PUBLIC SDL2::SDL2)
elseif(LIBTCOD_SDL2 STREQUAL "conan")
    target_link_libraries(LIBTCOD PUBLIC CONAN_PKG::sdl2)
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_SDL2: '${LIBTCOD_SDL2}'")
endif()

set_property(CACHE LIBTCOD_ZLIB PROPERTY STRINGS "find_package" "conan")
if(LIBTCOD_ZLIB STREQUAL "find_package")
    find_package(ZLIB REQUIRED)
    target_link_libraries(LIBTCOD PRIVATE ZLIB::ZLIB)
elseif(LIBTCOD_ZLIB STREQUAL "conan")
    target_link_libraries(LIBTCOD PRIVATE CONAN_PKG::zlib)
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_ZLIB: '${LIBTCOD_ZLIB}'")
endif()

set(LIBTCOD_GLAD "vendored" CACHE STRING "How this library will be linked.")
set_property(CACHE LIBTCOD_GLAD PROPERTY STRINGS "vendored" "find_package")
if(LIBTCOD_GLAD STREQUAL "find_package")
    find_package(glad CONFIG REQUIRED)
    target_link_libraries(LIBTCOD PRIVATE glad::glad)
elseif(LIBTCOD_GLAD STREQUAL "vendored")
    message("Will be vendored: GLAD")
    target_sources(LIBTCOD PRIVATE "vendor/glad.c")
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_GLAD: '${LIBTCOD_GLAD}'")
endif()


set(LIBTCOD_GLAD "vendored" CACHE STRING "How this library will be linked.")
set_property(CACHE LIBTCOD_GLAD PROPERTY STRINGS "vendored" "find_package")
if(LIBTCOD_GLAD STREQUAL "find_package")
    find_package(glad CONFIG REQUIRED)
    target_link_libraries(LIBTCOD PRIVATE glad::glad)
elseif(LIBTCOD_GLAD STREQUAL "vendored")
    message("Will be vendored: GLAD")
    target_sources(LIBTCOD PRIVATE "vendor/glad.c")
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_GLAD: '${LIBTCOD_GLAD}'")
endif()

set(LIBTCOD_LODEPNG "vendored" CACHE STRING "How this library will be linked.")
set_property(CACHE LIBTCOD_LODEPNG PROPERTY STRINGS "vendored" "find_package")
if(LIBTCOD_LODEPNG STREQUAL "find_package")
    find_package(lodepng-c CONFIG REQUIRED)
    target_link_libraries(LIBTCOD PRIVATE lodepng-c)
elseif(LIBTCOD_LODEPNG STREQUAL "vendored")
    message("Will be vendored: LodePNG")
    target_sources(LIBTCOD PRIVATE "vendor/lodepng.c")
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_LODEPNG: '${LIBTCOD_LODEPNG}'")
endif()

set(LIBTCOD_UTF8PROC "vendored" CACHE STRING "How this library will be linked.")
set_property(CACHE LIBTCOD_UTF8PROC PROPERTY STRINGS "vendored")
if(LIBTCOD_UTF8PROC STREQUAL "vendored")
    message("Will be vendored: utf8proc")
    target_include_directories(LIBTCOD PRIVATE "vendor/utf8proc")
    target_sources(LIBTCOD PRIVATE "vendor/utf8proc/utf8proc.c")
elseif (LIBTCOD_UTF8PROC STREQUAL "vcpkg")
    include(FindPackageHandleStandardArgs)
    include(SelectLibraryConfigurations)

    find_path(UTF8PROC_INCLUDE_DIRS utf8proc.h)
    find_library(UTF8PROC_LIBRARY_DEBUG NAMES utf8proc utf8proc_static PATHS ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib NO_DEFAULT_PATH Release RelWithDebInfo MinSizeRel)
    find_library(UTF8PROC_LIBRARY_RELEASE NAMES utf8proc utf8proc_static PATHS ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib NO_DEFAULT_PATH Debug)
    select_library_configurations(UTF8PROC)

    target_include_directories(LIBTCOD PRIVATE ${UTF8PROC_INCLUDE_DIRS})
    target_link_libraries(LIBTCOD PRIVATE ${UTF8PROC_LIBRARY})
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_UTF8PROC: '${LIBTCOD_UTF8PROC}'")
endif()

set(LIBTCOD_STB "vendored" CACHE STRING "How this library will be linked.")
set_property(CACHE LIBTCOD_STB PROPERTY STRINGS "vendored" "vcpkg")
if(LIBTCOD_STB STREQUAL "vendored")
    message("Will be vendored: stb")
elseif(LIBTCOD_STB STREQUAL "vcpkg")
    find_path(STB_INCLUDE_DIRS "stb.h")
    target_include_directories(LIBTCOD PRIVATE ${STB_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_STB: '${LIBTCOD_STB}'")
endif()

target_include_directories(LIBTCOD INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_include_directories(LIBTCOD PRIVATE AFTER ./vendor)

find_package(Threads REQUIRED)
target_link_libraries(LIBTCOD PUBLIC ${CMAKE_THREAD_LIBS_INIT})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(LIBTCOD PRIVATE LIBTCOD_EXPORTS)
else()
    target_compile_definitions(LIBTCOD PUBLIC LIBTCOD_STATIC)
endif()

if(MSVC)
    target_compile_definitions(LIBTCOD PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(LIBTCOD PRIVATE TCOD_IGNORE_DEPRECATED)

include(sources.cmake)

if(MSVC)
    set_property(TARGET LIBTCOD PROPERTY OUTPUT_NAME libtcod)
else()
    set_property(TARGET LIBTCOD PROPERTY OUTPUT_NAME tcod)
endif()

if(MSVC)
  target_compile_options(LIBTCOD PRIVATE /W4)
else()
  target_compile_options(LIBTCOD PRIVATE -Wall -Wextra)
endif()

install(
    TARGETS LIBTCOD
    EXPORT LIBTCODTargets
    RUNTIME LIBRARY ARCHIVE
    COMPONENT Library
)
include(CMakePackageConfigHelpers)
get_filename_component(
    INSTALL_FULL_INCLUDEDIR
    ${CMAKE_INSTALL_INCLUDEDIR}
    REALPATH BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}
)
install(
    EXPORT LIBTCODTargets
    DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
    NAMESPACE LIBTCOD::
)
configure_package_config_file(LIBTCODConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/LIBTCODConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
    PATH_VARS INSTALL_FULL_INCLUDEDIR)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/LIBTCODConfigVersion.cmake
    COMPATIBILITY SameMajorVersion
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/LIBTCODConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/LIBTCODConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
)
