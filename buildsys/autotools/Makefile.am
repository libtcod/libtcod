ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

AM_CFLAGS = \
	-I$(top_srcdir)/../../src \
	-fvisibility=hidden \
	-DLIBTCOD_EXPORTS \
	-DNDEBUG \
	-O3 \
	-Wall \
	-Wextra \
	-Wpedantic \
	$(SDL_CFLAGS) $(X11_CFLAGS) $(ZLIB_CFLAGS)

AM_CPPFLAGS = $(AM_CFLAGS)

AM_LDFLAGS = $(SDL_LIBS) $(X11_LIBS) $(ZLIB_LIBS) $(SYSLIBS)


lib_LTLIBRARIES = libtcod.la

libtcod_root_includedir = $(includedir)
libtcod_root_include_HEADERS = ../../src/libtcod.h ../../src/libtcod.hpp

include $(srcdir)/sources.am

libtcod_la_CFLAGS = $(AM_CFLAGS) $(PTHREAD_CFLAGS)
libtcod_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(SOVERSION) -lpthread
libtcod_la_LIBADD = $(PTHREAD_LIBS) -lpthread

dist_noinst_DATA = \
	../../CHANGELOG.md \
	../../LIBTCOD-CREDITS.txt \
	../../LICENSE.txt \
	../../README.md

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libtcod.pc
