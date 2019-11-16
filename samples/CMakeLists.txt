cmake_minimum_required (VERSION 3.9)

add_executable(samples_c samples_c.c)
target_link_libraries(samples_c TCOD::TCOD)

add_executable(samples_cpp samples_cpp.cpp)
target_link_libraries(samples_cpp TCOD::TCOD)
