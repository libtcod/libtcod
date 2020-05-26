add_executable(samples_cpp samples_cpp.cpp)
target_link_libraries(samples_cpp PRIVATE LIBTCOD)

add_executable(samples_c samples_c.c)
target_link_libraries(samples_c PRIVATE LIBTCOD)

file(COPY ${PATH_TO_DATA_DIRECTORY} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data/)
