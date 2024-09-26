cmake_minimum_required (VERSION 3.13...3.21)

target_compile_features(${PROJECT_NAME} PUBLIC c_std_99 cxx_std_17)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD_REQUIRED TRUE)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED TRUE)

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
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
