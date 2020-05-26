add_executable(samples_cpp samples_cpp.cpp)
target_link_libraries(samples_cpp PRIVATE LIBTCOD)

FILE(COPY ${PATH_TO_DATA_DIRECTORY} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data/)