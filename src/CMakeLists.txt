cmake_minimum_required (VERSION 3.13...3.21)

target_compile_features(${PROJECT_NAME} PUBLIC c_std_99 cxx_std_17)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD_REQUIRED TRUE)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED TRUE)

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
    target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2)
elseif(LIBTCOD_SDL2 STREQUAL "conan")
    target_link_libraries(${PROJECT_NAME} PUBLIC CONAN_PKG::sdl2)
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_SDL2: '${LIBTCOD_SDL2}'")
endif()

set_property(CACHE LIBTCOD_ZLIB PROPERTY STRINGS "find_package" "conan")
if(LIBTCOD_ZLIB STREQUAL "find_package")
    find_package(ZLIB REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
elseif(LIBTCOD_ZLIB STREQUAL "conan")
    target_link_libraries(${PROJECT_NAME} PRIVATE CONAN_PKG::zlib)
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_ZLIB: '${LIBTCOD_ZLIB}'")
endif()

set(LIBTCOD_GLAD "find_package" CACHE STRING "How this library will be linked.")
set_property(CACHE LIBTCOD_GLAD PROPERTY STRINGS "vendored" "find_package")
if(LIBTCOD_GLAD STREQUAL "find_package")
    find_package(glad CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)
elseif(LIBTCOD_GLAD STREQUAL "vendored")
    message("Will be vendored: GLAD")
    target_sources(${PROJECT_NAME} PRIVATE "vendor/glad.c")
    target_include_directories(${PROJECT_NAME} PRIVATE "vendor/")
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_GLAD: '${LIBTCOD_GLAD}'")
endif()

set(LIBTCOD_LODEPNG "find_package" CACHE STRING "How this library will be linked.")
set_property(CACHE LIBTCOD_LODEPNG PROPERTY STRINGS "vendored" "find_package")
if(LIBTCOD_LODEPNG STREQUAL "find_package")
    find_package(lodepng-c CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE lodepng-c)
elseif(LIBTCOD_LODEPNG STREQUAL "vendored")
    message("Will be vendored: LodePNG")
    target_sources(${PROJECT_NAME} PRIVATE "vendor/lodepng.c")
    target_include_directories(${PROJECT_NAME} PRIVATE "vendor/")
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_LODEPNG: '${LIBTCOD_LODEPNG}'")
endif()

set(LIBTCOD_UTF8PROC "vcpkg" CACHE STRING "How this library will be linked.")
set_property(CACHE LIBTCOD_UTF8PROC PROPERTY STRINGS "vendored" "vcpkg")
if(LIBTCOD_UTF8PROC STREQUAL "vendored")
    message("Will be vendored: utf8proc")
    target_include_directories(${PROJECT_NAME} PRIVATE "vendor/utf8proc")
    target_sources(${PROJECT_NAME} PRIVATE "vendor/utf8proc/utf8proc.c")
elseif (LIBTCOD_UTF8PROC STREQUAL "vcpkg")
    include(FindPackageHandleStandardArgs)
    include(SelectLibraryConfigurations)

    find_path(UTF8PROC_INCLUDE_DIRS utf8proc.h)
    find_library(UTF8PROC_LIBRARY_DEBUG NAMES utf8proc utf8proc_static PATHS ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib NO_DEFAULT_PATH Release RelWithDebInfo MinSizeRel)
    find_library(UTF8PROC_LIBRARY_RELEASE NAMES utf8proc utf8proc_static PATHS ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib NO_DEFAULT_PATH Debug)
    select_library_configurations(UTF8PROC)

    if(NOT BUILD_SHARED_LIBS)
        target_compile_definitions(${PROJECT_NAME} PRIVATE UTF8PROC_STATIC)
    endif()
    target_include_directories(${PROJECT_NAME} PRIVATE ${UTF8PROC_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${UTF8PROC_LIBRARY})
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_UTF8PROC: '${LIBTCOD_UTF8PROC}'")
endif()

set(LIBTCOD_STB "vcpkg" CACHE STRING "How this library will be linked.")
set_property(CACHE LIBTCOD_STB PROPERTY STRINGS "vendored" "vcpkg")
if(LIBTCOD_STB STREQUAL "vendored")
    message("Will be vendored: stb")
    target_include_directories(${PROJECT_NAME} PRIVATE "vendor/")
elseif(LIBTCOD_STB STREQUAL "vcpkg")
    find_path(STB_INCLUDE_DIRS "stb.h")
    target_include_directories(${PROJECT_NAME} PRIVATE ${STB_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_STB: '${LIBTCOD_STB}'")
endif()

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

find_package(Threads REQUIRED)
if (DEFINED CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_THREAD_LIBS_INIT})
endif()

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LIBTCOD_EXPORTS)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC LIBTCOD_STATIC)
endif()

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE TCOD_IGNORE_DEPRECATED)

include(sources.cmake)

# Remove the "lib" prefix to prevent a library name like "liblibtcod".
set_property(TARGET ${PROJECT_NAME} PROPERTY PREFIX "")

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
  target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
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
    EXPORT ${PROJECT_NAME}Targets
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
    NAMESPACE ${PROJECT_NAME}::
)
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
    PATH_VARS INSTALL_FULL_INCLUDEDIR)
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
    FILES "../LICENSE.txt"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
    RENAME copyright
)
