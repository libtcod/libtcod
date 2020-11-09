cmake_minimum_required (VERSION 3.9)
project(PackageTest CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake OPTIONAL RESULT_VARIABLE CONAN_FILE)
if(CONAN_FILE)
    conan_basic_setup(KEEP_RPATHS)
    set(LINK_TARGETS ${CONAN_LIBS})
else()
    set(LINK_TARGETS LIBTCOD::LIBTCOD)
endif()


if (APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path")
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

add_executable(unittest catch.cpp unittest.cpp)
target_link_libraries(unittest ${LINK_TARGETS})
target_compile_features(unittest PUBLIC cxx_std_14)
target_compile_definitions(unittest PRIVATE _CRT_SECURE_NO_WARNINGS)

# CTest is a testing tool that can be used to test your project.
# enable_testing()
# add_test(NAME example
#          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
#          COMMAND example)
