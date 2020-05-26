add_executable(frost frost.cpp)
target_link_libraries(frost LIBTCOD)

FILE(COPY ${PATH_TO_TERMINAL_PNG} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)