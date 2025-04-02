cmake_minimum_required (VERSION 3.23...4.0)

target_compile_features(${PROJECT_NAME} PUBLIC c_std_99 cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES
    C_STANDARD 99
    CXX_STANDARD 17
    C_STANDARD_REQUIRED TRUE
    CXX_STANDARD_REQUIRED TRUE
    VERIFY_INTERFACE_HEADER_SETS TRUE
)

set(LIBTCOD_THREADS ON CACHE BOOL "If true then older thread functions are enabled by libtcod.")
set(LIBTCOD_SDL3 "find_package" CACHE STRING "How this library will be linked.")
set(LIBTCOD_ZLIB "find_package" CACHE STRING "How this library will be linked.")
set(LIBTCOD_LODEPNG "find_package" CACHE STRING "How this library will be linked.")
set(LIBTCOD_UTF8PROC "vcpkg" CACHE STRING "How this library will be linked.")
set(LIBTCOD_STB "find_package" CACHE STRING "How this library will be linked.")

if(CONAN_FILE)
    message(STATUS "Fetching dependencies from Conan.")
    set(LIBTCOD_SDL3 "conan")
    set(LIBTCOD_ZLIB "conan")
endif()

if (CMAKE_DISABLE_FIND_PACKAGE_SDL3)
    set(LIBTCOD_SDL3 "disable")
endif()
if (CMAKE_DISABLE_FIND_PACKAGE_ZLIB)
    set(LIBTCOD_ZLIB "disable")
endif()
if (CMAKE_DISABLE_FIND_PACKAGE_lodepng-c)
    set(LIBTCOD_LODEPNG "disable")
endif()
if (CMAKE_DISABLE_FIND_PACKAGE_unofficial-utf8proc OR CMAKE_DISABLE_FIND_PACKAGE_utf8proc)
    set(LIBTCOD_UTF8PROC "disable")
endif()

set_property(CACHE LIBTCOD_SDL3 PROPERTY STRINGS "find_package" "conan" "disable")
if(LIBTCOD_SDL3 STREQUAL "find_package")
    list(APPEND VCPKG_MANIFEST_FEATURES "sdl")
    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3)
elseif(LIBTCOD_SDL3 STREQUAL "conan")
    target_link_libraries(${PROJECT_NAME} PUBLIC CONAN_PKG::sdl2)
elseif(LIBTCOD_SDL3 STREQUAL "disable")
    target_compile_definitions(${PROJECT_NAME} PUBLIC NO_SDL)
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_SDL3: '${LIBTCOD_SDL3}'")
endif()

set_property(CACHE LIBTCOD_ZLIB PROPERTY STRINGS "find_package" "conan" "disable")
if(LIBTCOD_ZLIB STREQUAL "find_package")
    find_package(ZLIB REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
elseif(LIBTCOD_ZLIB STREQUAL "conan")
    target_link_libraries(${PROJECT_NAME} PRIVATE CONAN_PKG::zlib)
elseif(LIBTCOD_ZLIB STREQUAL "disable")
    target_compile_definitions(${PROJECT_NAME} PUBLIC TCOD_NO_ZLIB)
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_ZLIB: '${LIBTCOD_ZLIB}'")
endif()

set_property(CACHE LIBTCOD_LODEPNG PROPERTY STRINGS "vendored" "find_package" "disable")
if(LIBTCOD_LODEPNG STREQUAL "find_package")
    find_package(lodepng-c CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE lodepng-c)
elseif(LIBTCOD_LODEPNG STREQUAL "vendored")
    message("Will be vendored: LodePNG")
    target_sources(${PROJECT_NAME} PRIVATE "vendor/lodepng.c")
    target_include_directories(${PROJECT_NAME} PRIVATE "vendor/")
elseif(LIBTCOD_LODEPNG STREQUAL "disable")
    target_compile_definitions(${PROJECT_NAME} PUBLIC TCOD_NO_PNG)
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_LODEPNG: '${LIBTCOD_LODEPNG}'")
endif()

set_property(CACHE LIBTCOD_UTF8PROC PROPERTY STRINGS "find_package" "vendored" "vcpkg" "disable")
if(LIBTCOD_UTF8PROC STREQUAL "vendored")
    message("Will be vendored: utf8proc")
    target_include_directories(${PROJECT_NAME} PRIVATE "vendor/utf8proc")
    target_sources(${PROJECT_NAME} PRIVATE "vendor/utf8proc/utf8proc.c")
elseif (LIBTCOD_UTF8PROC STREQUAL "find_package")
    find_package(utf8proc REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE utf8proc)
elseif (LIBTCOD_UTF8PROC STREQUAL "vcpkg")
    find_package(unofficial-utf8proc CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE utf8proc)
elseif(LIBTCOD_UTF8PROC STREQUAL "disable")
    target_compile_definitions(${PROJECT_NAME} PUBLIC TCOD_NO_UNICODE)
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_UTF8PROC: '${LIBTCOD_UTF8PROC}'")
endif()

set_property(CACHE LIBTCOD_STB PROPERTY STRINGS "find_package" "vendored" "vcpkg")
if(LIBTCOD_STB STREQUAL "vendored")
    message("Will be vendored: stb")
    target_include_directories(${PROJECT_NAME} PRIVATE "vendor/")
elseif(LIBTCOD_STB STREQUAL "find_package" OR LIBTCOD_STB STREQUAL "vcpkg")
    find_package(Stb REQUIRED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})
else()
    message(FATAL_ERROR "Unknown option for LIBTCOD_STB: '${LIBTCOD_STB}'")
endif()

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

if(LIBTCOD_THREADS AND NOT CMAKE_DISABLE_FIND_PACKAGE_Threads)
    find_package(Threads REQUIRED)
    if (DEFINED CMAKE_THREAD_LIBS_INIT)
        target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_THREAD_LIBS_INIT})
    endif()
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC TCOD_NO_THREADS)
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

if(LIBTCOD_INSTALL)
    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        FILE_SET ${PROJECT_NAME}_header_set
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
endif()
