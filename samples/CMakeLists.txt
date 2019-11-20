cmake_minimum_required (VERSION 3.9)

project (libtcod_samples C CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(samples_c samples_c.c)
target_link_libraries(samples_c CONAN_PKG::libtcod)

add_executable(samples_cpp samples_cpp.cpp)
target_link_libraries(samples_cpp CONAN_PKG::libtcod)
