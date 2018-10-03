ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

AM_CFLAGS = \
	-I$(top_srcdir)/../../src \
	-DNO_OPENGL \
	-DNDEBUG -O2 \
	-Wall \
	$(SDL_CFLAGS) $(X11_CFLAGS) $(ZLIB_CFLAGS)

AM_CPPFLAGS = $(AM_CFLAGS)

AM_LDFLAGS = $(SDL_LIBS) $(X11_LIBS) $(ZLIB_LIBS)


lib_LTLIBRARIES = libtcod.la


otherincludedir = $(includedir)/libtcod/gui
otherinclude_HEADERS = \
	../../src/libtcod/gui/button.hpp \
	../../src/libtcod/gui/container.hpp \
	../../src/libtcod/gui/flatlist.hpp \
	../../src/libtcod/gui/gui.hpp \
	../../src/libtcod/gui/hbox.hpp \
	../../src/libtcod/gui/image.hpp \
	../../src/libtcod/gui/label.hpp \
	../../src/libtcod/gui/radiobutton.hpp \
	../../src/libtcod/gui/slider.hpp \
	../../src/libtcod/gui/statusbar.hpp \
	../../src/libtcod/gui/textbox.hpp \
	../../src/libtcod/gui/togglebutton.hpp \
	../../src/libtcod/gui/toolbar.hpp \
	../../src/libtcod/gui/vbox.hpp \
	../../src/libtcod/gui/widget.hpp \
	../../src/libtcod/gui/gui_portability.hpp


libtcod_la_SOURCES = \
	../../src/vendor/lodepng.cpp \
	../../src/vendor/stb.c \
	../../src/vendor/utf8proc/utf8proc.c \
	../../src/libtcod/bresenham_c.c \
	../../src/libtcod/bsp_c.c \
	../../src/libtcod/color_c.c \
	../../src/libtcod/console_c.cpp \
	../../src/libtcod/console_printing.c \
	../../src/libtcod/console_rexpaint.c \
	../../src/libtcod/fov_c.c \
	../../src/libtcod/fov_circular_raycasting.c \
	../../src/libtcod/fov_diamond_raycasting.c \
	../../src/libtcod/fov_permissive2.c \
	../../src/libtcod/fov_recursive_shadowcasting.c \
	../../src/libtcod/fov_restrictive.c \
	../../src/libtcod/heightmap_c.c \
	../../src/libtcod/image_c.c \
	../../src/libtcod/lex_c.c \
	../../src/libtcod/list_c.c \
	../../src/libtcod/mersenne_c.c \
	../../src/libtcod/namegen_c.c \
	../../src/libtcod/noise_c.c \
	../../src/libtcod/parser_c.c \
	../../src/libtcod/path_c.c \
	../../src/libtcod/sys_c.cpp \
	../../src/libtcod/sys_sdl2_c.cpp \
	../../src/libtcod/sys_sdl_c.cpp \
	../../src/libtcod/sys_sdl_img_bmp.cpp \
	../../src/libtcod/sys_sdl_img_png.cpp \
	../../src/libtcod/tree_c.c \
	../../src/libtcod/txtfield_c.c \
	../../src/libtcod/wrappers.c \
	../../src/libtcod/zip_c.c \
	../../src/libtcod/bresenham.cpp \
	../../src/libtcod/bsp.cpp \
	../../src/libtcod/color.cpp \
	../../src/libtcod/console.cpp \
	../../src/libtcod/fov.cpp \
	../../src/libtcod/heightmap.cpp \
	../../src/libtcod/image.cpp \
	../../src/libtcod/lex.cpp \
	../../src/libtcod/mersenne.cpp \
	../../src/libtcod/mouse.cpp \
	../../src/libtcod/namegen.cpp \
	../../src/libtcod/noise.cpp \
	../../src/libtcod/parser.cpp \
	../../src/libtcod/path.cpp \
	../../src/libtcod/sys.cpp \
	../../src/libtcod/txtfield.cpp \
	../../src/libtcod/zip.cpp \
	../../src/libtcod/color/canvas.cpp \
	../../src/libtcod/engine/backend.cpp \
	../../src/libtcod/engine/display.cpp \
	../../src/libtcod/engine/globals.cpp \
	../../src/libtcod/gui/button.cpp \
	../../src/libtcod/gui/container.cpp \
	../../src/libtcod/gui/flatlist.cpp \
	../../src/libtcod/gui/hbox.cpp \
	../../src/libtcod/gui/image.cpp \
	../../src/libtcod/gui/label.cpp \
	../../src/libtcod/gui/radiobutton.cpp \
	../../src/libtcod/gui/slider.cpp \
	../../src/libtcod/gui/statusbar.cpp \
	../../src/libtcod/gui/textbox.cpp \
	../../src/libtcod/gui/togglebutton.cpp \
	../../src/libtcod/gui/toolbar.cpp \
	../../src/libtcod/gui/vbox.cpp \
	../../src/libtcod/gui/widget.cpp \
	../../src/libtcod/sdl2/legacy_backend.cpp \
	../../src/libtcod/sdl2/sdl2_alias.cpp \
	../../src/libtcod/sdl2/sdl2_display.cpp \
	../../src/libtcod/sdl2/sdl2_renderer.cpp \
	../../src/libtcod/tileset/observer.cpp \
	../../src/libtcod/tileset/tile.cpp \
	../../src/libtcod/tileset/tileset.cpp \
	../../src/libtcod/tileset/tilesheet.cpp

libtcod_la_CFLAGS = $(AM_CFLAGS) $(PTHREAD_CFLAGS)
libtcod_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(SOVERSION)
libtcod_la_LIBADD = $(PTHREAD_LIBS)


pkginclude_HEADERS = \
	../../src/libtcod/bresenham.h \
	../../src/libtcod/mersenne_types.h \
	../../src/libtcod/bsp.h \
	../../src/libtcod/mouse.h \
	../../src/libtcod/color.h \
	../../src/libtcod/mouse_types.h \
	../../src/libtcod/console.h \
	../../src/libtcod/namegen.h \
	../../src/libtcod/console_types.h \
	../../src/libtcod/console_rexpaint.h \
	../../src/libtcod/noise_defaults.h \
	../../src/libtcod/fov.h \
	../../src/libtcod/noise.h \
	../../src/libtcod/fov_types.h \
	../../src/libtcod/parser.h \
	../../src/libtcod/heightmap.h \
	../../src/libtcod/path.h \
	../../src/libtcod/image.h \
	../../src/libtcod/sys.h \
	../../src/libtcod/lex.h \
	../../src/libtcod/tree.h \
	../../src/libtcod/libtcod.h \
	../../src/libtcod/txtfield.h \
	../../src/libtcod/libtcod_int.h \
	../../src/libtcod/portability.h \
	../../src/libtcod/utility.h \
	../../src/libtcod/version.h \
	../../src/libtcod/wrappers.h \
	../../src/libtcod/list.h \
	../../src/libtcod/zip.h \
	../../src/libtcod/mersenne.h \
	../../src/libtcod/bresenham.hpp \
	../../src/libtcod/mersenne.hpp \
	../../src/libtcod/bsp.hpp \
	../../src/libtcod/mouse.hpp \
	../../src/libtcod/color.hpp \
	../../src/libtcod/namegen.hpp \
	../../src/libtcod/console.hpp \
	../../src/libtcod/noise.hpp \
	../../src/libtcod/fov.hpp \
	../../src/libtcod/parser.hpp \
	../../src/libtcod/heightmap.hpp \
	../../src/libtcod/path.hpp \
	../../src/libtcod/sys.hpp \
	../../src/libtcod/image.hpp \
	../../src/libtcod/tree.hpp \
	../../src/libtcod/lex.hpp \
	../../src/libtcod/txtfield.hpp \
	../../src/libtcod/libtcod.hpp \
	../../src/libtcod/zip.hpp \
	../../src/libtcod/list.hpp

engineincludedir = $(includedir)/libtcod/engine
engineinclude_HEADERS = \
	../../src/libtcod/engine/backend.h \
	../../src/libtcod/engine/display.h \
	../../src/libtcod/engine/globals.h

colorincludedir = $(includedir)/libtcod/color
colorinclude_HEADERS = \
	../../src/libtcod/color/color.h \
	../../src/libtcod/color/canvas.h

sdl2includedir = $(includedir)/libtcod/sdl2
sdl2include_HEADERS = \
	../../src/libtcod/sdl2/legacy_backend.h \
	../../src/libtcod/sdl2/sdl2_alias.h \
	../../src/libtcod/sdl2/sdl2_display.h \
	../../src/libtcod/sdl2/sdl2_renderer.h

tilesetincludedir = $(includedir)/libtcod/tileset
tilesetinclude_HEADERS = \
	../../src/libtcod/tileset/observer.h \
	../../src/libtcod/tileset/tileset.h \
	../../src/libtcod/tileset/tilesheet.h \
	../../src/libtcod/tileset/tile.h

utilityincludedir = $(includedir)/libtcod/utility
utilityinclude_HEADERS = \
	../../src/libtcod/utility/vector2.h


if ENABLE_SAMPLES
noinst_PROGRAMS = samples_c samples_cpp

samples_c_SOURCES = ../../samples/samples_c.c
samples_c_LDADD = libtcod.la

samples_cpp_SOURCES = ../../samples/samples_cpp.cpp
samples_cpp_LDADD = libtcod.la
endif

check_scripts = unittest.sh
check_PROGRAMS = unittest
unittest_SOURCES = ../../tests/unittest.cpp
unittest_LDADD = libtcod.la
unittest_CXXFLAGS = $(CXXFLAGS) -std=c++11

TESTS = unittest.sh

dist_noinst_DATA = \
	../../build.bat \
	../../CHANGELOG.md \
	../../LIBTCOD-CREDITS.txt \
	../../LIBTCOD-LICENSE.txt \
	../../README.md \
	../../libtcod.cfg

dist_noinst_DATA += \
	../../terminal.png \
	../../data/img/circle.png \
	../../data/img/skull.png \
	../../data/namegen/mingos_town.cfg \
	../../data/namegen/README.txt \
	../../data/namegen/mingos_demon.cfg \
	../../data/namegen/mingos_dwarf.cfg \
	../../data/namegen/jice_norse.cfg \
	../../data/namegen/jice_town.cfg \
	../../data/namegen/jice_region.cfg \
	../../data/namegen/jice_celtic.cfg \
	../../data/namegen/mingos_norse.cfg \
	../../data/namegen/jice_fantasy.cfg \
	../../data/namegen/mingos_standard.cfg \
	../../data/namegen/jice_mesopotamian.cfg \
	../../data/fonts/dejavu8x8_gs_tc.png \
	../../data/fonts/consolas10x10_gs_tc.png \
	../../data/fonts/dundalk12x12_gs_tc.png \
	../../data/fonts/terminal8x8_gs_as.png \
	../../data/fonts/README.txt \
	../../data/fonts/terminal8x12_gs_tc.png \
	../../data/fonts/courier12x12_aa_tc.png \
	../../data/fonts/arial10x10.png \
	../../data/fonts/prestige12x12_gs_tc.png \
	../../data/fonts/terminal8x8_aa_as.png \
	../../data/fonts/terminal8x14_gs_ro.png \
	../../data/fonts/terminal8x12_gs_ro.png \
	../../data/fonts/terminal16x16_gs_ro.png \
	../../data/fonts/prestige10x10_gs_tc.png \
	../../data/fonts/caeldera8x8_gs_tc.png \
	../../data/fonts/terminal7x7_gs_tc.png \
	../../data/fonts/terminal10x18_gs_ro.png \
	../../data/fonts/terminal10x16_gs_tc.png \
	../../data/fonts/dejavu_wide12x12_gs_tc.png \
	../../data/fonts/consolas_unicode_8x8.png \
	../../data/fonts/arial8x8.png \
	../../data/fonts/terminal8x8_gs_tc.png \
	../../data/fonts/dejavu12x12_gs_tc.png \
	../../data/fonts/consolas_unicode_12x12.png \
	../../data/fonts/lucida12x12_gs_tc.png \
	../../data/fonts/arial12x12.png \
	../../data/fonts/consolas8x8_gs_tc.png \
	../../data/fonts/consolas_unicode_16x16.png \
	../../data/fonts/lucida8x8_gs_tc.png \
	../../data/fonts/terminal10x16_gs_ro.png \
	../../data/fonts/dejavu_wide16x16_gs_tc.png \
	../../data/fonts/consolas12x12_gs_tc.png \
	../../data/fonts/terminal8x8_aa_ro.png \
	../../data/fonts/terminal10x10_gs_tc.png \
	../../data/fonts/prestige8x8_gs_tc.png \
	../../data/fonts/terminal8x8_aa_tc.png \
	../../data/fonts/terminal12x12_gs_ro.png \
	../../data/fonts/celtic_garamond_10x10_gs_tc.png \
	../../data/fonts/courier8x8_aa_tc.png \
	../../data/fonts/lucida10x10_gs_tc.png \
	../../data/fonts/consolas_unicode_10x10.png \
	../../data/fonts/courier10x10_aa_tc.png \
	../../data/fonts/terminal8x8_gs_ro.png \
	../../data/fonts/dejavu16x16_gs_tc.png \
	../../data/fonts/dejavu10x10_gs_tc.png
