cmake_minimum_required (VERSION 3.9)

include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED)
if(IPO_SUPPORTED)
    set_property(TARGET TCOD PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    set_property(TARGET TCOD PROPERTY INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE)
endif()

project (libtcod C CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(TCOD)
target_compile_features(TCOD PUBLIC cxx_std_14)
target_link_libraries(TCOD ${CONAN_LIBS})

target_include_directories(TCOD PUBLIC src/)
target_include_directories(TCOD PRIVATE src/vendor)

target_sources(TCOD PRIVATE src/libtcod.cpp src/libtcod_c.c)
target_sources(TCOD PRIVATE src/vendor/glad.c src/vendor/lodepng.cpp src/vendor/utf8proc/utf8proc.c)

if(BUILD_SHARED_LIBS)
    add_compile_definitions(LIBTCOD_EXPORTS)
else()
    add_compile_definitions(LIBTCOD_STATIC)
endif()

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

add_compile_definitions(TCOD_IGNORE_DEPRECATED)
