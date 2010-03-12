cmake_minimum_required (VERSION 2.6)
project (libtcod)

# All projects need "include" directory
include_directories(include)

find_package(OpenGL REQUIRED)

IF(MSVC)
	# msvc wants to "deprecate" standard C. Turn those warnings off.
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)

	SET(LIBTCOD_NAME libtcod-VS)
	SET(LIBTCOD_GUI_NAME libtcod-gui-VS)
	SET(LIB_DIR msvc)
	SET(PNG_NAME libpng)
	SET(ZLIB_NAME zdll)
ELSEIF(MINGW)
	SET(LIBTCOD_NAME tcod-mingw)
	SET(LIBTCOD_GUI_NAME tcod-gui-mingw)
	SET(LIB_DIR mingw)
	SET(PNG_NAME png)
	SET(ZLIB_NAME z)
ELSEIF(APPLE)
	SET(LIBTCOD_NAME tcod)
	SET(LIBTCOD_GUI_NAME tcod-gui)
	SET(LIB_DIR osx)
	SET(PNG_NAME png)
	SET(ZLIB_NAME z)
ELSEIF(UNIX)

	SET(LIBTCOD_NAME tcod)
	SET(LIBTCOD_CPP_NAME tcodxx)
	SET(LIBTCOD_GUI_NAME tcod-gui)
	SET(LIB_DIR linux)
	SET(PNG_NAME png)
	SET(ZLIB_NAME z)
ENDIF()

#Linux uses system libraries
if(MSVC OR MINGW)
	# MSVC uses special SDL headers.
	IF(MSVC)
		include_directories(dependencies/SDL-1.2.12/include/msvc)
	ELSE()
		include_directories(dependencies/SDL-1.2.12/include)
	ENDIF()

	include_directories(dependencies/zlib-1.2.3/include)
	include_directories(dependencies/libpng-1.2.31/include)

	LINK_DIRECTORIES(../../dependencies/libpng-1.2.31/lib/${LIB_DIR}/ ../../dependencies/SDL-1.2.12/lib/${LIB_DIR}/ ../../dependencies/zlib-1.2.3/lib/${LIB_DIR}/)
ELSE()
	find_package(SDL REQUIRED)
	find_package(PNG REQUIRED)
	find_package(ZLIB REQUIRED)
	find_package(Threads REQUIRED)
	find_library(M_LIB m)
	IF(CMAKE_BUILD_TYPE MATCHES "Debug")
		find_library(EFENCE_LIBRARY efence PATHS /usr/lib /usr/local/lib)
	ENDIF()
ENDIF()


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

# Linux wants seperate C and C++ libraries.
IF(UNIX)
	add_library(${LIBTCOD_NAME} SHARED ${LIBTCOD_C_SRC})
	target_link_libraries(${LIBTCOD_NAME} ${PNG_NAME} SDL ${ZLIB_NAME} ${OPENGL_LIBRARY} ${M_LIB} ${EFENCE_LIBRARY})

	add_library(${LIBTCOD_CPP_NAME} SHARED ${LIBTCOD_CPP_SRC})
	target_link_libraries(${LIBTCOD_CPP_NAME} ${LIBTCOD_NAME} ${PNG_NAME} SDL ${ZLIB_NAME} ${OPENGL_LIBRARY} ${M_LIB} ${EFENCE_LIBRARY})
ELSE()
	add_library(${LIBTCOD_NAME} SHARED ${LIBTCOD_C_SRC} ${LIBTCOD_CPP_SRC})
	target_link_libraries(${LIBTCOD_NAME} ${PNG_NAME} SDL ${ZLIB_NAME} ${OPENGL_LIBRARY}) 
ENDIF()

include_directories(include/gui)
add_library(${LIBTCOD_GUI_NAME} SHARED ${LIBTCOD_GUI_SRC})
target_link_libraries(${LIBTCOD_GUI_NAME} ${LIBTCOD_NAME} ${LIBTCOD_CPP_NAME}) 	#${LIBTCOD_CPP_NAME} is empty on non-linux platforms

add_executable(samples_c ${LIBTCOD_SAMPLE_C_SRC})
target_link_libraries(samples_c ${LIBTCOD_NAME}) 

add_executable(samples_cpp ${LIBTCOD_SAMPLE_CPP_SRC})
target_link_libraries(samples_cpp ${LIBTCOD_NAME} ${LIBTCOD_CPP_NAME})  #${LIBTCOD_CPP_NAME} is empty on non-linux platforms

add_executable(hmtool ${LIBTCOD_HP_TOOL_SRC})
target_link_libraries(hmtool ${LIBTCOD_NAME} ${LIBTCOD_GUI_NAME}) 

IF(MSVC OR MINGW)
	# Since we're exporting functions and clases, we use the "standard" macro combinations to do so under msvc.
	# These turn them on for the right builds.
	set_property(TARGET ${LIBTCOD_NAME} PROPERTY COMPILE_DEFINITIONS LIBTCOD_EXPORTS)
	set_property(TARGET ${LIBTCOD_GUI_NAME} PROPERTY COMPILE_DEFINITIONS LIBTCOD_GUI_EXPORTS)
ENDIF()



IF(UNIX)
	SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "") #Workaround for -rdynamic

	SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "") #same

	SET(CMAKE_SKIP_RPATH ON) #Don't place hard coded rpaths'
	SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,-rpath,. ${CMAKE_SHARED_LINKER_FLAGS}")

	SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,. ${CMAKE_EXE_LINKER_FLAGS}")
ENDIF()

IF(MSVC)
	# Since libtcod pulls in DLLs that use the release CRT, force the linker to never try to cross match CRTs
	set_property(TARGET ${LIBTCOD_NAME} PROPERTY LINK_FLAGS "/NODEFAULTLIB:MSVCRTD.lib ")
ENDIF()


set_property(TARGET ${LIBTCOD_NAME} PROPERTY DEBUG_POSTFIX "-debug")
set_property(TARGET ${LIBTCOD_CPP_NAME} PROPERTY DEBUG_POSTFIX "-debug")
set_property(TARGET ${LIBTCOD_GUI_NAME} PROPERTY DEBUG_POSTFIX "-debug")

set_property(TARGET samples_c PROPERTY DEBUG_POSTFIX "_debug")
set_property(TARGET samples_cpp PROPERTY DEBUG_POSTFIX "_debug")
set_property(TARGET hmtool PROPERTY DEBUG_POSTFIX "_debug")


IF(UNIX)
	IF(CMAKE_BUILD_TYPE MATCHES "Release")
		ADD_CUSTOM_COMMAND(
		    TARGET samples_c
		    POST_BUILD
		    COMMAND upx samples_c
		    VERBATIM
		)

		ADD_CUSTOM_COMMAND(
		    samples_cpp
		    POST_BUILD
		    COMMAND upx samples_cpp
		    VERBATIM
		)

		ADD_CUSTOM_COMMAND(
   		    TARGET hmtool
		    POST_BUILD
		    COMMAND upx hmtool
		    VERBATIM
		)
	ENDIF()
ENDIF()

#${LIBTCOD_CPP_NAME} is empty on non-linux platforms
INSTALL(TARGETS samples_c samples_cpp hmtool ${LIBTCOD_NAME} ${LIBTCOD_GUI_NAME} ${LIBTCOD_CPP_NAME}
  RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}
  LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}
)
