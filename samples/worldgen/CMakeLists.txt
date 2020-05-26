add_executable(worldgen
        main.cpp
        main.hpp
        util_worldgen.cpp
        util_worldgen.hpp )

target_link_libraries(worldgen PRIVATE LIBTCOD)
FILE(COPY ${PATH_TO_TERMINAL_PNG} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)