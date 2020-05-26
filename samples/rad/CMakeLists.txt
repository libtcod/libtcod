add_executable(rad
        main.cpp
        bsp_helper.cpp
        bsp_helper.hpp
        rad_shader.cpp
        rad_shader.hpp
        rad_shader_photon.cpp
        rad_shader_standard.cpp)

target_link_libraries(rad PRIVATE LIBTCOD)
file(COPY ${PATH_TO_TERMINAL_PNG} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
