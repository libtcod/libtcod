add_executable(navier main.cpp)
target_link_libraries(navier PRIVATE LIBTCOD)
FILE(COPY ${PATH_TO_TERMINAL_PNG} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)