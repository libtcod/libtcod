cmake_minimum_required (VERSION 3.14 FATAL_ERROR)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "")
endif()

file(STRINGS src/libtcod/version.h LIBTCOD_VERSION_LINE REGEX "TCOD_STRVERSION")
string(REGEX MATCH "TCOD_STRVERSION \"([^\"]+)\"" LIBTCOD_VERSION_LINE ${LIBTCOD_VERSION_LINE})
set(LIBTCOD_VERSION_FULL ${CMAKE_MATCH_1})
string(REGEX MATCH "([0-9]+\.[0-9]+\.[0-9]+)" LIBTCOD_VERSION ${LIBTCOD_VERSION_FULL})
message(STATUS "Libtcod version: ${LIBTCOD_VERSION}")

project(libtcod VERSION ${LIBTCOD_VERSION} LANGUAGES C CXX)

set(CMAKE_INSTALL_LIBDIR ${CMAKE_CURRENT_BINARY_DIR}/lib CACHE STRING "")
set(CMAKE_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/libtcod/cmake CACHE STRING "")
set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_CURRENT_BINARY_DIR}/include CACHE STRING "")

include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED)

add_library(LIBTCOD)
add_library(LIBTCOD::LIBTCOD ALIAS LIBTCOD)
target_compile_features(LIBTCOD PUBLIC cxx_std_14)

find_package(SDL2 REQUIRED)
find_package(ZLIB REQUIRED)

target_link_libraries(LIBTCOD PRIVATE ${SDL2_LIBRARIES})
target_link_libraries(LIBTCOD PRIVATE ${ZLIB_LIBRARIES})

target_include_directories(LIBTCOD PRIVATE ./vendor)
TARGET_INCLUDE_DIRECTORIES(LIBTCOD PRIVATE ${SDL2_INCLUDE_DIRS})

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

include(src/sources.cmake)

if(IPO_SUPPORTED)
    set_property(TARGET LIBTCOD PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    set_property(TARGET LIBTCOD PROPERTY INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE)
endif()

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
        EXPORT libtcodTargets
        RUNTIME LIBRARY ARCHIVE
        COMPONENT Library
)

#include(CMakePackageConfigHelpers)
#get_filename_component(
#        INSTALL_FULL_INCLUDEDIR
#        ${CMAKE_INSTALL_INCLUDEDIR}
#        REALPATH BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}
#)
#install(
#        EXPORT libtcodTargets
#        DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
#        NAMESPACE LIBTCOD::
#)
#configure_package_config_file(LibtcodConfig.cmake.in
#        ${CMAKE_CURRENT_BINARY_DIR}/LibtcodConfig.cmake
#        INSTALL_DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
#        PATH_VARS INSTALL_FULL_INCLUDEDIR)
#write_basic_package_version_file(
#        ${CMAKE_CURRENT_BINARY_DIR}/LibtcodConfigVersion.cmake
#        COMPATIBILITY SameMajorVersion
#)
#install(
#        FILES
#        ${CMAKE_CURRENT_BINARY_DIR}/LibtcodConfig.cmake
#        ${CMAKE_CURRENT_BINARY_DIR}/LibtcodConfigVersion.cmake
#        DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
#)
