add_executable(hmtool
        hmtool.cpp operation.cpp operation.hpp )

target_link_libraries(hmtool PRIVATE LIBTCOD)
FILE(COPY ${PATH_TO_TERMINAL_PNG} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)