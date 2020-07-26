cmake_minimum_required (VERSION 3.9)

set(CMAKE_INSTALL_LIBDIR ${CMAKE_CURRENT_BINARY_DIR}/lib CACHE STRING "")
set(CMAKE_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/libtcod/cmake CACHE STRING "")
set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_CURRENT_BINARY_DIR}/include CACHE STRING "")

include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake OPTIONAL RESULT_VARIABLE CONAN_FILE)
if(CONAN_FILE)
    conan_basic_setup(TARGETS)
endif()

add_library(LIBTCOD)
add_library(LIBTCOD::LIBTCOD ALIAS LIBTCOD)
target_compile_features(LIBTCOD PUBLIC cxx_std_14)

if(CONAN_FILE)
    message(STATUS "Fetching dependencies from Conan.")
    target_link_libraries(LIBTCOD PUBLIC CONAN_PKG::sdl2)
    target_link_libraries(LIBTCOD PRIVATE CONAN_PKG::zlib)
else()
    message(STATUS "Fetching dependencies from Vcpkg or other sources.")
    find_package(SDL2 CONFIG REQUIRED)
    find_package(ZLIB REQUIRED)
    target_link_libraries(LIBTCOD PUBLIC SDL2::SDL2)
    target_link_libraries(LIBTCOD PRIVATE ZLIB::ZLIB)
endif()

target_include_directories(LIBTCOD PRIVATE ./vendor)

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

if(NOT CONAN_FILE)
    install(
        TARGETS LIBTCOD
        EXPORT libtcodTargets
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
        EXPORT libtcodTargets
        DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
        NAMESPACE LIBTCOD::
    )
    configure_package_config_file(LibtcodConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/LibtcodConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
        PATH_VARS INSTALL_FULL_INCLUDEDIR)
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/LibtcodConfigVersion.cmake
        COMPATIBILITY SameMajorVersion
    )
    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/LibtcodConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/LibtcodConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
    )
endif()
