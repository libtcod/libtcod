cmake_minimum_required (VERSION 3.9)

project (libtcod_samples C CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS KEEP_RPATHS)

# This and KEEP_RPATHS is required to handle RPATH's on MacOS.
if (APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path")
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

MESSAGE(STATUS ${PATH_TO_DATA})

add_custom_target(copy_data_dir
        COMMENT "Copy project data directory to the runtime folder."
        COMMAND cmake -E copy_directory
        ${PATH_TO_DATA}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data
        )
add_custom_target(copy_font
        COMMENT "Copy terminal.png to the runtime folder."
        COMMAND cmake -E copy_if_different
        ${PATH_TO_TERMINAL_PNG}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/terminal.png
        )

add_executable(samples_c samples_c.c)
target_link_libraries(samples_c LIBTCOD)
add_dependencies(samples_c copy_data_dir)

add_executable(samples_cpp samples_cpp.cpp)
target_link_libraries(samples_cpp LIBTCOD)
add_dependencies(samples_cpp copy_data_dir)

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

add_executable(ripples
    ripples/main.cpp
    ripples/main.hpp
    ripples/util_ripples.cpp
    ripples/util_ripples.hpp
)
target_link_libraries(ripples LIBTCOD)
add_dependencies(ripples copy_font)

add_executable(weather
    weather/main.cpp
    weather/main.hpp
    weather/util_weather.cpp
    weather/util_weather.hpp
)
target_link_libraries(weather LIBTCOD)
add_dependencies(weather copy_font)

add_executable(worldgen
    worldgen/main.cpp
    worldgen/main.hpp
    worldgen/util_worldgen.cpp
    worldgen/util_worldgen.hpp
)
target_link_libraries(worldgen LIBTCOD)
add_dependencies(worldgen copy_font)

FILE(COPY ../data/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/samples/)