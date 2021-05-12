cmake_minimum_required (VERSION 3.9)

project (libtcod_samples C CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake OPTIONAL RESULT_VARIABLE CONAN_FILE)
if(CONAN_FILE)
    conan_basic_setup(TARGETS)
    set(LINK_TCOD CONAN_PKG::libtcod)
    set(SDL2MAIN)
else()
    find_package(SDL2 CONFIG REQUIRED)
    set(LINK_TCOD libtcod::libtcod)
    set(SDL2MAIN SDL2::SDL2main)
endif()

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

# Enforce UTF-8 encoding on MSVC.
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_executable(samples_c samples_c.c)
target_link_libraries(samples_c ${LINK_TCOD} ${SDL2MAIN})
if (CONAN_FILE)
    add_dependencies(samples_c copy_data_dir)
endif()

add_executable(samples_cpp samples_cpp.cpp)
target_link_libraries(samples_cpp ${LINK_TCOD} ${SDL2MAIN})
if (CONAN_FILE)
    add_dependencies(samples_cpp copy_data_dir)
endif()

add_executable(frost frost/frost.cpp)
target_link_libraries(frost ${LINK_TCOD} SDL2::SDL2main SDL2::SDL2)
if (CONAN_FILE)
    add_dependencies(frost copy_font)
endif()

add_executable(hmtool
    hmtool/hmtool.cpp hmtool/operation.cpp hmtool/operation.hpp
)
target_link_libraries(hmtool ${LINK_TCOD})
if (CONAN_FILE)
    add_dependencies(hmtool copy_font)
endif()

add_executable(navier navier/main.cpp navier/main.hpp)
target_link_libraries(navier ${LINK_TCOD})
if (CONAN_FILE)
    add_dependencies(navier copy_font)
endif()

add_executable(rad
    rad/main.cpp
    rad/bsp_helper.cpp
    rad/bsp_helper.hpp
    rad/rad_shader.cpp
    rad/rad_shader.hpp
    rad/rad_shader_photon.cpp
    rad/rad_shader_standard.cpp
)
target_link_libraries(rad ${LINK_TCOD})
if (CONAN_FILE)
    add_dependencies(rad copy_font)
endif()

add_executable(ripples
    ripples/main.cpp
    ripples/main.hpp
    ripples/util_ripples.cpp
    ripples/util_ripples.hpp
)
target_link_libraries(ripples ${LINK_TCOD})
if (CONAN_FILE)
    add_dependencies(ripples copy_font)
endif()

add_executable(weather
    weather/main.cpp
    weather/main.hpp
    weather/util_weather.cpp
    weather/util_weather.hpp
)
target_link_libraries(weather ${LINK_TCOD})
if (CONAN_FILE)
    add_dependencies(weather copy_font)
endif()

add_executable(worldgen
    worldgen/main.cpp
    worldgen/main.hpp
    worldgen/util_worldgen.cpp
    worldgen/util_worldgen.hpp
)
target_link_libraries(worldgen ${LINK_TCOD})
if (CONAN_FILE)
    add_dependencies(worldgen copy_font)
endif()
