add_executable(weather
        main.cpp
        main.hpp
        util_weather.cpp
        util_weather.hpp)

target_link_libraries(weather PRIVATE LIBTCOD)
file(COPY ${PATH_TO_TERMINAL_PNG} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
