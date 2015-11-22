AM_CFLAGS = -I$(top_srcdir)/../../include -DTCOD_SDL2 -DNO_OPENGL
AM_CPPFLAGS = -I$(top_srcdir)/../../include -DTCOD_SDL2 -DNO_OPENGL
AM_LDFLAGS = -lz

lib_LTLIBRARIES = libtcod.la
libtcod_la_SOURCES = ../../src/bresenham_c.c ../../src/bsp_c.c ../../src/color_c.c ../../src/console_c.c ../../src/fov_c.c ../../src/fov_circular_raycasting.c ../../src/fov_diamond_raycasting.c ../../src/fov_permissive2.c ../../src/fov_recursive_shadowcasting.c ../../src/fov_restrictive.c ../../src/heightmap_c.c ../../src/image_c.c ../../src/lex_c.c ../../src/list_c.c ../../src/mersenne_c.c ../../src/namegen_c.c ../../src/noise_c.c ../../src/parser_c.c ../../src/path_c.c ../../src/sys_c.c ../../src/sys_sdl2_c.c ../../src/sys_sdl_c.c ../../src/sys_sdl_img_bmp.c ../../src/sys_sdl_img_png.c ../../src/tree_c.c ../../src/txtfield_c.c ../../src/wrappers.c ../../src/zip_c.c ../../src/png/lodepng.c

otherincludedir = $(includedir)/tcod
otherinclude_HEADERS = ../../include/bresenham.h ../../include/mersenne_types.h ../../include/bsp.h ../../include/mouse.h ../../include/color.h ../../include/mouse_types.h ../../include/console.h ../../include/namegen.h ../../include/console_types.h ../../include/noise_defaults.h ../../include/fov.h ../../include/noise.h ../../include/fov_types.h ../../include/parser.h ../../include/heightmap.h ../../include/path.h ../../include/image.h ../../include/sys.h ../../include/lex.h ../../include/tree.h ../../include/libtcod.h ../../include/txtfield.h ../../include/libtcod_int.h ../../include/wrappers.h ../../include/list.h ../../include/zip.h ../../include/mersenne.h


# lib_LTLIBRARIES = libtcodxx.la
# libtcodxx_la_SOURCES = ../../bresenham.cpp ../../bsp.cpp ../../color.cpp ../../console.cpp ../../fov.cpp ../../heightmap.cpp ../../image.cpp ../../lex.cpp ../../mersenne.cpp ../../mouse.cpp ../../namegen.cpp ../../noise.cpp ../../parser.cpp ../../path.cpp ../../sys.cpp ../../txtfield.cpp ../../zip.cpp
# libtcodxx_la_LIBADD = libtcod.la
# include_HEADERS = ../../include/bresenham.hpp ../../include/mersenne.hpp ../../include/bsp.hpp ../../include/mouse.hpp ../../include/color.hpp ../../include/namegen.hpp ../../include/console.hpp ../../include/noise.hpp ../../include/fov.hpp ../../include/parser.hpp ../../include/heightmap.hpp ../../include/path.hpp ../../include/howto.hpp ../../include/sys.hpp ../../include/image.hpp ../../include/tree.hpp ../../include/lex.hpp ../../include/txtfield.hpp ../../include/libtcod.hpp ../../include/zip.hpp ../../include/list.hpp
