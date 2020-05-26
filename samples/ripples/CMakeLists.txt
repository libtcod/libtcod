add_executable(ripples
        main.cpp
        main.hpp
        util_ripples.cpp
        util_ripples.hpp )

target_link_libraries(ripples LIBTCOD)
FILE(COPY ${PATH_TO_TERMINAL_PNG} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)