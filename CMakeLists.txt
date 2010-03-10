cmake_minimum_required (VERSION 2.6)
project (libtcod)

# All projects need "include" directory
include_directories(include)

find_package(OpenGL)

IF(MSVC)
	# msvc wants to "deprecate" standard C. Turn those warnings off.
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)

	SET(LIBTCOD_NAME libtcod)
	SET(LIBTCOD_GUI_NAME libtcod-gui)
	SET(LIB_DIR msvc)
	SET(PNG_NAME libpng)
	SET(ZLIB_NAME zdll)
ELSE(MINGW)
	SET(LIBTCOD_NAME tcod)
	SET(LIBTCOD_GUI_NAME tcod-gui)
	SET(LIB_DIR mingw)
	SET(PNG_NAME png)
	SET(ZLIB_NAME z)
ELSE(APPLE)
	SET(LIBTCOD_NAME tcod)
	SET(LIBTCOD_GUI_NAME tcod-gui)
	SET(LIB_DIR osx)
	SET(PNG_NAME png)
	SET(ZLIB_NAME z)
ELSE()
	SET(LIBTCOD_NAME tcod)
	SET(LIBTCOD_GUI_NAME tcod-gui)
	SET(LIB_DIR linux)
	SET(PNG_NAME png)
	SET(ZLIB_NAME z)
ENDIF()

# MSVC uses special SDL headers.
IF(MSVC)
	include_directories(dependencies/SDL-1.2.12/include/msvc)
ELSE()
	include_directories(dependencies/SDL-1.2.12/include)
ENDIF()

include_directories(dependencies/zlib-1.2.3/include)
include_directories(dependencies/libpng-1.2.31/include)

LINK_DIRECTORIES(../../dependencies/libpng-1.2.31/lib/${LIB_DIR}/ ../../dependencies/SDL-1.2.12/lib/${LIB_DIR}/ ../../dependencies/zlib-1.2.3/lib/${LIB_DIR}/)

SET(LIBTCOD_C_SRC src/bresenham_c.c src/bsp_c.c src/color_c.c src/console_c.c src/fov_c.c src/fov_circular_raycasting.c 
				src/fov_diamond_raycasting.c src/fov_permissive.c src/fov_permissive2.c src/fov_recursive_shadowcasting.c 
				src/fov_restrictive.c src/heightmap_c.c src/image_c.c src/lex_c.c src/list_c.c src/mersenne_c.c src/namegen_c.c 
				src/noise_c.c src/parser_c.c src/path_c.c src/sys_c.c src/sys_opengl_c.c src/sys_sdl_c.c src/sys_sdl_img_bmp.c 
				src/sys_sdl_img_png.c src/tree_c.c src/txtfield_c.c src/wrappers.c src/zip_c.c)

SET(LIBTCOD_CPP_SRC src/bresenham.cpp src/bsp.cpp src/color.cpp src/console.cpp src/fov.cpp src/heightmap.cpp src/image.cpp src/lex.cpp src/mersenne.cpp 
				src/mouse.cpp src/namegen.cpp src/noise.cpp src/parser.cpp src/path.cpp src/sys.cpp src/zip.cpp)

SET(LIBTCOD_GUI_SRC src/gui/button.cpp src/gui/container.cpp src/gui/flatlist.cpp src/gui/hbox.cpp src/gui/image.cpp src/gui/label.cpp  src/gui/radiobutton.cpp 
				src/gui/slider.cpp src/gui/statusbar.cpp src/gui/textbox.cpp src/gui/togglebutton.cpp src/gui/toolbar.cpp src/gui/vbox.cpp src/gui/widget.cpp)

SET(LIBTCOD_SAMPLE_C_SRC samples_c.c)
SET(LIBTCOD_SAMPLE_CPP_SRC samples_cpp.cpp)
SET(LIBTCOD_HP_TOOL_SRC src/hmtool/operation.cpp src/hmtool/hmtool.cpp)

add_library(${LIBTCOD_NAME} SHARED ${LIBTCOD_C_SRC} ${LIBTCOD_CPP_SRC})
target_link_libraries(${LIBTCOD_NAME} ${PNG_NAME} SDL ${ZLIB_NAME} ${OPENGL_LIBRARY}) 

include_directories(include/gui)
add_library(${LIBTCOD_GUI_NAME} SHARED ${LIBTCOD_GUI_SRC})
target_link_libraries(${LIBTCOD_GUI_NAME} ${LIBTCOD_NAME}) 

add_executable(sample_c ${LIBTCOD_SAMPLE_C_SRC})
target_link_libraries(sample_c ${LIBTCOD_NAME}) 

add_executable(sample_cpp ${LIBTCOD_SAMPLE_CPP_SRC})
target_link_libraries(sample_cpp ${LIBTCOD_NAME}) 

add_executable(hmtool ${LIBTCOD_HP_TOOL_SRC})
target_link_libraries(hmtool ${LIBTCOD_NAME} ${LIBTCOD_GUI_NAME}) 

IF(MSVC OR MINGW)
# Since we're exporting functions and clases, we use the "standard" macro combinations to do so under msvc.
# These turn them on for the right builds.
set_property(TARGET ${LIBTCOD_NAME} PROPERTY COMPILE_DEFINITIONS LIBTCOD_EXPORTS)
set_property(TARGET ${LIBTCOD_GUI_NAME} PROPERTY COMPILE_DEFINITIONS LIBTCOD_GUI_EXPORTS)
ENDIF()

IF(MSVC)
# Since libtcod pulls in DLLs that use the release CRT, force the linker to never try to cross match CRTs
set_property(TARGET ${LIBTCOD_NAME} PROPERTY LINK_FLAGS "/NODEFAULTLIB:MSVCRTD.lib ")
ENDIF()
