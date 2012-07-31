IF(MSVC)
	# msvc wants to "deprecate" standard C. Turn those warnings off.
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)

	SET(LIBTCOD_NAME libtcod-VS)
	SET(LIBTCOD_GUI_NAME libtcod-gui-VS)
	SET(SDL_NAME SDL)
	SET(ZLIB_NAME zdll)
ELSEIF(MINGW)
	SET(LIBTCOD_NAME tcod-mingw)
	SET(LIBTCOD_GUI_NAME tcod-gui-mingw)
	SET(SDL_NAME SDL)
	SET(ZLIB_NAME z)
ELSEIF(APPLE)
	SET(LIBTCOD_NAME tcod)
	SET(LIBTCOD_GUI_NAME tcod-gui)
	find_package(SDL REQUIRED)
	SET(SDL_NAME ${SDL_LIBRARY})
	SET(ZLIB_NAME z)
ELSEIF(UNIX)
	SET(LIBTCOD_NAME tcod)
	SET(LIBTCOD_CPP_NAME tcodxx)
	SET(LIBTCOD_GUI_NAME tcod-gui)
	SET(SDL_NAME SDL)
	SET(ZLIB_NAME z)
ENDIF()

SET(LIBTCOD_C_SRC bresenham_c.c bsp_c.c color_c.c console_c.c fov_c.c fov_circular_raycasting.c 
				fov_diamond_raycasting.c fov_permissive.c fov_permissive2.c fov_recursive_shadowcasting.c 
				fov_restrictive.c heightmap_c.c image_c.c lex_c.c list_c.c mersenne_c.c namegen_c.c 
				noise_c.c parser_c.c path_c.c sys_c.c sys_opengl_c.c sys_sdl_c.c sys_sdl_img_bmp.c 
				sys_sdl_img_png.c tree_c.c txtfield_c.c wrappers.c zip_c.c png/lodepng.c)

SET(LIBTCOD_CPP_SRC bresenham.cpp bsp.cpp color.cpp console.cpp fov.cpp heightmap.cpp image.cpp lex.cpp mersenne.cpp 
				mouse.cpp namegen.cpp noise.cpp parser.cpp path.cpp sys.cpp txtfield.cpp zip.cpp)

IF(APPLE)
SET(LIBTCOD_C_SRC ${LIBTCOD_C_SRC} ../osx/macsupport.m)
ENDIF()

SET(LIBTCOD_GUI_SRC gui/button.cpp gui/container.cpp gui/flatlist.cpp gui/hbox.cpp gui/image.cpp gui/label.cpp  gui/radiobutton.cpp 
				gui/slider.cpp gui/statusbar.cpp gui/textbox.cpp gui/togglebutton.cpp gui/toolbar.cpp gui/vbox.cpp gui/widget.cpp)

# Linux wants seperate C and C++ libraries.
IF(UNIX AND NOT APPLE)
	include_directories( ${EXTERNAL_INCLUDES} )
	add_library(${LIBTCOD_NAME} SHARED ${LIBTCOD_C_SRC})
	target_link_libraries(${LIBTCOD_NAME} ${EXTERNAL_LIBS} ${EFENCE_LIBRARY})

	add_library(${LIBTCOD_CPP_NAME} SHARED ${LIBTCOD_CPP_SRC})
	target_link_libraries(${LIBTCOD_CPP_NAME} ${LIBTCOD_NAME} ${EXTERNAL_LIBS} ${EFENCE_LIBRARY})
ELSE()
	add_library(${LIBTCOD_NAME} SHARED ${LIBTCOD_C_SRC} ${LIBTCOD_CPP_SRC})
	target_link_libraries(${LIBTCOD_NAME} ${SDL_NAME} ${ZLIB_NAME} ${OPENGL_LIBRARY}) 
ENDIF()

include_directories(${GUI_INCLUDE_DIR})
add_library(${LIBTCOD_GUI_NAME} SHARED ${LIBTCOD_GUI_SRC})
target_link_libraries(${LIBTCOD_GUI_NAME} ${LIBTCOD_NAME} ${LIBTCOD_CPP_NAME}) 	#${LIBTCOD_CPP_NAME} is empty on non-linux platforms

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

set_property(TARGET ${LIBTCOD_NAME} PROPERTY DEBUG_POSTFIX "-debug")
set_property(TARGET ${LIBTCOD_CPP_NAME} PROPERTY DEBUG_POSTFIX "-debug")
set_property(TARGET ${LIBTCOD_GUI_NAME} PROPERTY DEBUG_POSTFIX "-debug")

IF(UNIX)
	SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "") #Workaround for -rdynamic
	SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "") #same

	SET(CMAKE_SKIP_RPATH ON) #Don't place hard coded rpaths'
	SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,-rpath,. ${CMAKE_SHARED_LINKER_FLAGS}")
ENDIF()

#${LIBTCOD_CPP_NAME} is empty on non-linux platforms
INSTALL(TARGETS ${LIBTCOD_NAME} ${LIBTCOD_GUI_NAME} ${LIBTCOD_CPP_NAME}
  RUNTIME DESTINATION ${INSTALL_DIR}
  LIBRARY DESTINATION ${INSTALL_DIR}
)
