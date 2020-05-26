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

add_subdirectory(frost)
add_subdirectory(hmtool)
add_subdirectory(navier)
add_subdirectory(rad)
add_subdirectory(ripples)
add_subdirectory(showcase)
add_subdirectory(weather)
add_subdirectory(worldgen)
