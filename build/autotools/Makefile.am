ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = \
	-I$(top_srcdir)/../../include \
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

libtcodgui_la_CXXFLAGS = $(CXXFLAGS) -I$(top_srcdir)/../../include/gui
libtcodgui_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(SOVERSION)
libtcodgui_la_LIBADD = libtcod.la

externalincludedir = $(includedir)/libtcod/external
externalinclude_HEADERS = \
	../../include/external/pstdint.h


otherincludedir = $(includedir)/libtcod/gui
otherinclude_HEADERS = \
	../../include/gui/button.hpp \
	../../include/gui/container.hpp \
	../../include/gui/flatlist.hpp \
	../../include/gui/gui.hpp \
	../../include/gui/hbox.hpp \
	../../include/gui/image.hpp \
	../../include/gui/label.hpp \
	../../include/gui/radiobutton.hpp \
	../../include/gui/slider.hpp \
	../../include/gui/statusbar.hpp \
	../../include/gui/textbox.hpp \
	../../include/gui/togglebutton.hpp \
	../../include/gui/toolbar.hpp \
	../../include/gui/vbox.hpp \
	../../include/gui/widget.hpp \
	../../include/gui/gui_portability.hpp


libtcod_la_SOURCES = \
	../../src/bresenham_c.c \
	../../src/bsp_c.c \
	../../src/color_c.c \
	../../src/console_c.c \
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
	../../src/sys_c.c \
	../../src/sys_sdl2_c.c \
	../../src/sys_sdl_c.c \
	../../src/sys_sdl_img_bmp.c \
	../../src/sys_sdl_img_png.c \
	../../src/tree_c.c \
	../../src/txtfield_c.c \
	../../src/wrappers.c \
	../../src/zip_c.c \
	../../src/vendor/lodepng.c \
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
	../../src/zip.cpp

libtcod_la_CFLAGS = $(AM_CFLAGS) $(PTHREAD_CFLAGS) -ansi
libtcod_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(SOVERSION)
libtcod_la_LIBADD = $(PTHREAD_LIBS)


pkginclude_HEADERS = \
	../../include/bresenham.h \
	../../include/mersenne_types.h \
	../../include/bsp.h \
	../../include/mouse.h \
	../../include/color.h \
	../../include/mouse_types.h \
	../../include/console.h \
	../../include/namegen.h \
	../../include/console_types.h \
	../../include/console_rexpaint.h \
	../../include/noise_defaults.h \
	../../include/fov.h \
	../../include/noise.h \
	../../include/fov_types.h \
	../../include/parser.h \
	../../include/heightmap.h \
	../../include/path.h \
	../../include/image.h \
	../../include/sys.h \
	../../include/lex.h \
	../../include/tree.h \
	../../include/libtcod.h \
	../../include/txtfield.h \
	../../include/libtcod_int.h \
	../../include/libtcod_portability.h \
	../../include/libtcod_utility.h \
	../../include/libtcod_version.h \
	../../include/wrappers.h \
	../../include/list.h \
	../../include/zip.h \
	../../include/mersenne.h \
	../../include/bresenham.hpp \
	../../include/mersenne.hpp \
	../../include/bsp.hpp \
	../../include/mouse.hpp \
	../../include/color.hpp \
	../../include/namegen.hpp \
	../../include/console.hpp \
	../../include/noise.hpp \
	../../include/fov.hpp \
	../../include/parser.hpp \
	../../include/heightmap.hpp \
	../../include/path.hpp \
	../../include/howto.hpp \
	../../include/sys.hpp \
	../../include/image.hpp \
	../../include/tree.hpp \
	../../include/lex.hpp \
	../../include/txtfield.hpp \
	../../include/libtcod.hpp \
	../../include/zip.hpp \
	../../include/list.hpp


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
