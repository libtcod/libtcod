cmake_minimum_required (VERSION 3.0)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    file(TO_CMAKE_PATH "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CMAKE_TOOLCHAIN_FILE)
endif()

project (libtcod C CXX)

find_package(SDL2 REQUIRED)
find_package(ZLIB REQUIRED)

add_library(LIBTCOD)
target_compile_features(LIBTCOD PUBLIC cxx_std_14)

target_link_libraries(LIBTCOD PUBLIC SDL2::SDL2)
target_link_libraries(LIBTCOD PRIVATE ZLIB::ZLIB)

target_include_directories(LIBTCOD PUBLIC ${SDL2_INCLUDE_DIRS})
target_include_directories(LIBTCOD PUBLIC src/)
target_include_directories(LIBTCOD PRIVATE src/vendor)

target_sources(LIBTCOD PRIVATE src/libtcod.cpp src/libtcod_c.c)
target_sources(LIBTCOD PRIVATE src/vendor/glad.c src/vendor/lodepng.cpp src/vendor/utf8proc/utf8proc.c)
