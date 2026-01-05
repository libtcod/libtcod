cmake_minimum_required(VERSION 3.24...4.2)

include(FetchContent)
include(GNUInstallDirs)

target_compile_features(${PROJECT_NAME} PUBLIC c_std_99 cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES
    C_STANDARD 99
    CXX_STANDARD 17
    C_STANDARD_REQUIRED TRUE
    CXX_STANDARD_REQUIRED TRUE
    VERIFY_INTERFACE_HEADER_SETS TRUE
)

if(CMAKE_DISABLE_FIND_PACKAGE_SDL3)
    set(LIBTCOD_SDL3 "OFF")
endif()
if(CMAKE_DISABLE_FIND_PACKAGE_ZLIB)
    set(LIBTCOD_ZLIB "OFF")
endif()
if(CMAKE_DISABLE_FIND_PACKAGE_lodepng-c)
    set(LIBTCOD_LODEPNG "OFF")
endif()
if(CMAKE_DISABLE_FIND_PACKAGE_unofficial-utf8proc OR CMAKE_DISABLE_FIND_PACKAGE_utf8proc)
    set(LIBTCOD_UTF8PROC "OFF")
endif()

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

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(LIBTCOD_THREADS AND NOT CMAKE_DISABLE_FIND_PACKAGE_Threads)
    find_package(Threads REQUIRED)
    if(DEFINED CMAKE_THREAD_LIBS_INIT)
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

if(LIBTCOD_DOCS)
    message(STATUS "Preparing Doxygen")
    find_package(Doxygen REQUIRED)
    doxygen_add_docs(
        ${PROJECT_NAME}_docs
        ALL
        COMMENT "Generating documentation using Doxygen ${Doxygen_VERSION}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../docs
        CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../docs/Doxyfile
    )
    if(LIBTCOD_INSTALL)
        install(
            DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}/../docs/html/
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
    configure_package_config_file(${PROJECT_NAME}Config.cmake.in
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
        FILES "../LICENSE.txt"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
        RENAME copyright
    )
endif()
