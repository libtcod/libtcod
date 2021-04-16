ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

AM_CFLAGS = \
	-I$(top_srcdir)/../../src \
	-I$(top_srcdir)/../../src/vendor \
	-I$(top_srcdir)/../../src/vendor/utf8proc \
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

libtcod_root_includedir = $(includedir)
libtcod_root_include_HEADERS = ../../src/libtcod.h ../../src/libtcod.hpp

include $(srcdir)/sources.am

libtcod_la_CFLAGS = $(AM_CFLAGS) $(PTHREAD_CFLAGS)
libtcod_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(SOVERSION) -lpthread
libtcod_la_LIBADD = $(PTHREAD_LIBS) -lpthread

if ENABLE_SAMPLES
noinst_PROGRAMS = samples_c samples_cpp

samples_c_SOURCES = ../../samples/samples_c.c
samples_c_LDADD = libtcod.la

samples_cpp_SOURCES = ../../samples/samples_cpp.cpp
samples_cpp_LDADD = libtcod.la
endif

dist_noinst_DATA = \
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
	../../data/fonts/README.txt \
	../../data/fonts/dejavu10x10_gs_tc.png

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libtcod.pc
