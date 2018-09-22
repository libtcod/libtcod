ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

AM_CFLAGS = \
	-I$(top_srcdir)/../../src \
	-DNO_OPENGL \
	-DNDEBUG -O2 \
	-Wall \
	$(SDL_CFLAGS) $(X11_CFLAGS) $(ZLIB_CFLAGS)

AM_CPPFLAGS = $(AM_CFLAGS)

AM_LDFLAGS = $(SDL_LIBS) $(X11_LIBS) $(ZLIB_LIBS)


lib_LTLIBRARIES = libtcod.la libtcodgui.la


libtcodgui_la_SOURCES = \
	../../src/gui/button.cpp \
	../../src/gui/container.cpp \
	../../src/gui/flatlist.cpp \
	../../src/gui/hbox.cpp \
	../../src/gui/image.cpp \
	../../src/gui/label.cpp \
	../../src/gui/radiobutton.cpp \
	../../src/gui/slider.cpp \
	../../src/gui/statusbar.cpp \
	../../src/gui/textbox.cpp \
	../../src/gui/togglebutton.cpp \
	../../src/gui/toolbar.cpp \
	../../src/gui/vbox.cpp \
	../../src/gui/widget.cpp

libtcodgui_la_CXXFLAGS = $(AM_CXXFLAGS)
libtcodgui_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(SOVERSION)
libtcodgui_la_LIBADD = libtcod.la


otherincludedir = $(includedir)/libtcod/gui
otherinclude_HEADERS = \
	../../src/gui/button.hpp \
	../../src/gui/container.hpp \
	../../src/gui/flatlist.hpp \
	../../src/gui/gui.hpp \
	../../src/gui/hbox.hpp \
	../../src/gui/image.hpp \
	../../src/gui/label.hpp \
	../../src/gui/radiobutton.hpp \
	../../src/gui/slider.hpp \
	../../src/gui/statusbar.hpp \
	../../src/gui/textbox.hpp \
	../../src/gui/togglebutton.hpp \
	../../src/gui/toolbar.hpp \
	../../src/gui/vbox.hpp \
	../../src/gui/widget.hpp \
	../../src/gui/gui_portability.hpp


libtcod_la_SOURCES = \
	../../src/bresenham_c.c \
	../../src/bsp_c.c \
	../../src/color_c.c \
	../../src/console_c.cpp \
	../../src/console_printing.c \
	../../src/console_rexpaint.c \
	../../src/fov_c.c \
	../../src/fov_circular_raycasting.c \
	../../src/fov_diamond_raycasting.c \
	../../src/fov_permissive2.c \
	../../src/fov_recursive_shadowcasting.c \
	../../src/fov_restrictive.c \
	../../src/heightmap_c.c \
	../../src/image_c.c \
	../../src/lex_c.c \
	../../src/list_c.c \
	../../src/mersenne_c.c \
	../../src/namegen_c.c \
	../../src/noise_c.c \
	../../src/parser_c.c \
	../../src/path_c.c \
	../../src/sys_c.cpp \
	../../src/sys_sdl2_c.cpp \
	../../src/sys_sdl_c.cpp \
	../../src/sys_sdl_img_bmp.cpp \
	../../src/sys_sdl_img_png.cpp \
	../../src/tree_c.c \
	../../src/txtfield_c.c \
	../../src/wrappers.c \
	../../src/zip_c.c \
	../../src/vendor/lodepng.cpp \
	../../src/vendor/stb.c \
	../../src/vendor/utf8proc/utf8proc.c \
	../../src/bresenham.cpp \
	../../src/bsp.cpp \
	../../src/color.cpp \
	../../src/console.cpp \
	../../src/fov.cpp \
	../../src/heightmap.cpp \
	../../src/image.cpp \
	../../src/lex.cpp \
	../../src/mersenne.cpp \
	../../src/mouse.cpp \
	../../src/namegen.cpp \
	../../src/noise.cpp \
	../../src/parser.cpp \
	../../src/path.cpp \
	../../src/sys.cpp \
	../../src/txtfield.cpp \
	../../src/zip.cpp \
	../../src/color/canvas.cc \
	../../src/engine/backend.cc \
	../../src/engine/display.cc \
	../../src/engine/globals.cc \
	../../src/sdl2/legacy_backend.cc \
	../../src/sdl2/sdl2_alias.cc \
	../../src/sdl2/sdl2_display.cc \
	../../src/sdl2/sdl2_renderer.cc \
	../../src/tileset/observer.cc \
	../../src/tileset/tile.cc \
	../../src/tileset/tileset.cc \
	../../src/tileset/tilesheet.cc

libtcod_la_CFLAGS = $(AM_CFLAGS) $(PTHREAD_CFLAGS)
libtcod_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(SOVERSION)
libtcod_la_LIBADD = $(PTHREAD_LIBS)


pkginclude_HEADERS = \
	../../src/bresenham.h \
	../../src/mersenne_types.h \
	../../src/bsp.h \
	../../src/mouse.h \
	../../src/color.h \
	../../src/mouse_types.h \
	../../src/console.h \
	../../src/namegen.h \
	../../src/console_types.h \
	../../src/console_rexpaint.h \
	../../src/noise_defaults.h \
	../../src/fov.h \
	../../src/noise.h \
	../../src/fov_types.h \
	../../src/parser.h \
	../../src/heightmap.h \
	../../src/path.h \
	../../src/image.h \
	../../src/sys.h \
	../../src/lex.h \
	../../src/tree.h \
	../../src/libtcod.h \
	../../src/txtfield.h \
	../../src/libtcod_int.h \
	../../src/libtcod_portability.h \
	../../src/libtcod_utility.h \
	../../src/libtcod_version.h \
	../../src/wrappers.h \
	../../src/list.h \
	../../src/zip.h \
	../../src/mersenne.h \
	../../src/bresenham.hpp \
	../../src/mersenne.hpp \
	../../src/bsp.hpp \
	../../src/mouse.hpp \
	../../src/color.hpp \
	../../src/namegen.hpp \
	../../src/console.hpp \
	../../src/noise.hpp \
	../../src/fov.hpp \
	../../src/parser.hpp \
	../../src/heightmap.hpp \
	../../src/path.hpp \
	../../src/howto.hpp \
	../../src/sys.hpp \
	../../src/image.hpp \
	../../src/tree.hpp \
	../../src/lex.hpp \
	../../src/txtfield.hpp \
	../../src/libtcod.hpp \
	../../src/zip.hpp \
	../../src/list.hpp

engineincludedir = $(includedir)/libtcod/engine
engineinclude_HEADERS = \
	../../src/engine/backend.h \
	../../src/engine/display.h \
	../../src/engine/globals.h

colorincludedir = $(includedir)/libtcod/color
colorinclude_HEADERS = \
	../../src/color/color.h \
	../../src/color/canvas.h

sdl2includedir = $(includedir)/libtcod/sdl2
sdl2include_HEADERS = \
	../../src/sdl2/legacy_backend.h \
	../../src/sdl2/sdl2_alias.h \
	../../src/sdl2/sdl2_display.h \
	../../src/sdl2/sdl2_renderer.h

tilesetincludedir = $(includedir)/libtcod/tileset
tilesetinclude_HEADERS = \
	../../src/tileset/observer.h \
	../../src/tileset/tileset.h \
	../../src/tileset/tilesheet.h \
	../../src/tileset/tile.h

utilityincludedir = $(includedir)/libtcod/utility
utilityinclude_HEADERS = \
	../../src/utility/vector2.h


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
