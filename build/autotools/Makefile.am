ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

AM_CFLAGS = \
	-I$(top_srcdir)/../../src \
	-I$(top_srcdir)/../../src/vendor \
	-fvisibility=hidden \
	-DLIBTCOD_EXPORTS \
	-DNDEBUG \
	-O3 \
	-Wall \
	-Wextra \
	-Wpedantic \
	$(SDL_CFLAGS) $(X11_CFLAGS) $(ZLIB_CFLAGS)

AM_CPPFLAGS = $(AM_CFLAGS)

AM_LDFLAGS = $(SDL_LIBS) $(X11_LIBS) $(ZLIB_LIBS)


lib_LTLIBRARIES = libtcod.la

include $(srcdir)/collected_files.am

libtcod_la_CFLAGS = $(AM_CFLAGS) $(PTHREAD_CFLAGS)
libtcod_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(SOVERSION)
libtcod_la_LIBADD = $(PTHREAD_LIBS)

if ENABLE_SAMPLES
noinst_PROGRAMS = samples_c samples_cpp

samples_c_SOURCES = ../../samples/samples_c.c
samples_c_LDADD = libtcod.la

samples_cpp_SOURCES = ../../samples/samples_cpp.cpp
samples_cpp_LDADD = libtcod.la
endif

check_scripts = unittest.sh
check_PROGRAMS = unittest
unittest_SOURCES = ../../tests/catch.cpp ../../tests/unittest.cpp
unittest_LDADD = libtcod.la
unittest_CXXFLAGS = $(CXXFLAGS)

TESTS = unittest.sh

dist_noinst_DATA = \
	../../build.bat \
	../../CHANGELOG.md \
	../../LIBTCOD-CREDITS.txt \
	../../LICENSE.txt \
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

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libtcod.pc
