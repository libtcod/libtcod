cmake_minimum_required (VERSION 3.14 FATAL_ERROR)

project (libtcod_samples C CXX)

SET(PATH_TO_DATA_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../data/)
SET(PATH_TO_TERMINAL_PNG ${CMAKE_CURRENT_SOURCE_DIR}/../terminal.png)

# This and KEEP_RPATHS is required to handle RPATH's on MacOS.
if (APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path")
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()
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

ADD_SUBDIRECTORY(showcase)

add_executable(frost frost/frost.cpp)
target_link_libraries(frost LIBTCOD)
add_dependencies(frost copy_font)

add_executable(hmtool
    hmtool/hmtool.cpp hmtool/operation.cpp hmtool/operation.hpp
)
target_link_libraries(hmtool LIBTCOD)
add_dependencies(hmtool copy_font)

add_executable(navier navier/main.cpp navier/main.hpp)
target_link_libraries(navier LIBTCOD)
add_dependencies(navier copy_font)

add_executable(rad
    rad/main.cpp
    rad/bsp_helper.cpp
    rad/bsp_helper.hpp
    rad/rad_shader.cpp
    rad/rad_shader.hpp
    rad/rad_shader_photon.cpp
    rad/rad_shader_standard.cpp
)
target_link_libraries(rad LIBTCOD)
add_dependencies(rad copy_font)

ADD_SUBDIRECTORY(ripples)
ADD_SUBDIRECTORY(weather)
ADD_SUBDIRECTORY(worldgen)
