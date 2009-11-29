SUBDIRS = . gui
include_HEADERS = ../../include/*.h ../../include/*.hpp
lib_LTLIBRARIES = libtcod.la libtcodxx.la

libtcod_la_SOURCES = ../../src/bresenham_c.c ../../src/bsp_c.c ../../src/color_c.c \
		../../src/console_c.c ../../src/fov_c.c ../../src/fov_circular_raycasting.c \
		../../src/fov_diamond_raycasting.c ../../src/fov_digital.c \
		../../src/fov_recursive_shadowcasting.c ../../src/fov_permissive2.c \
		../../src/fov_restrictive.c \
		../../src/heightmap_c.c ../../src/image_c.c ../../src/lex_c.c ../../src/list_c.c ../../src/mersenne_c.c \
		../../src/noise_c.c ../../src/parser_c.c ../../src/path_c.c ../../src/sys_c.c ../../src/sys_sdl_c.c \
		../../src/sys_sdl_img_bmp.c ../../src/sys_sdl_img_png.c ../../src/tree_c.c ../../src/wrappers.c \
		../../src/zip_c.c
libtcod_la_LDFLAGS = `sdl-config --libs` -lpthread

libtcodxx_la_SOURCES = ../../src/bresenham.cpp ../../src/bsp.cpp ../../src/color.cpp \
		 ../../src/console.cpp ../../src/fov.cpp ../../src/heightmap.cpp ../../src/image.cpp \
		 ../../src/lex.cpp ../../src/mersenne.cpp ../../src/mouse.cpp ../../src/noise.cpp ../../src/parser.cpp \
		 ../../src/path.cpp ../../src/sys.cpp ../../src/zip.cpp

