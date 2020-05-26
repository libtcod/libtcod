cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(libtcod_samples C CXX)

set(PATH_TO_DATA_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../data/)
set(PATH_TO_TERMINAL_PNG ${CMAKE_CURRENT_SOURCE_DIR}/../terminal.png)

# This and KEEP_RPATHS is required to handle RPATH's on MacOS.
if (APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path")
else ()
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif ()
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

add_custom_target(copy_data_dir
        COMMENT "Copy project data directory to the runtime folder."
        COMMAND cmake -E copy_directory
        ${CMAKE_SOURCE_DIR}/../data
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data
        )
add_custom_target(copy_font
        COMMENT "Copy terminal.png to the runtime folder."
        COMMAND cmake -E copy_if_different
        ${CMAKE_SOURCE_DIR}/../terminal.png
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/terminal.png
        )

add_executable(samples_c samples_c.c)
target_link_libraries(samples_c LIBTCOD)
add_dependencies(samples_c copy_data_dir)

add_subdirectory(frost)
add_subdirectory(hmtool)
add_subdirectory(navier)
add_subdirectory(rad)
add_subdirectory(ripples)
add_subdirectory(showcase)
add_subdirectory(weather)
add_subdirectory(worldgen)
