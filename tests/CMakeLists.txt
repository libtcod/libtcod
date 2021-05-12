cmake_minimum_required (VERSION 3.9)
project(PackageTest CXX)

find_package(Catch2 CONFIG REQUIRED)

if (APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path")
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

file(GLOB SRC_FILES test_*.cpp)

add_executable(unittest unittest.cpp ${SRC_FILES})
target_link_libraries(unittest libtcod::libtcod Catch2::Catch2)
target_compile_features(unittest PUBLIC cxx_std_14)
target_compile_definitions(unittest PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_definitions(unittest PRIVATE CATCH_CONFIG_ENABLE_BENCHMARKING)
if (MSVC)
    target_compile_options(unittest PRIVATE /utf-8)
endif()

# CTest is a testing tool that can be used to test your project.
# enable_testing()
# add_test(NAME example
#          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
#          COMMAND example)
