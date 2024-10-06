ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

AM_CFLAGS = \
	-I$(top_srcdir)/../../src \
	-fvisibility=hidden \
	-DTCODFOV_EXPORTS \
	-DNDEBUG \
	-O3 \
	-Wall \
	-Wextra \
	-Wpedantic \
	$(SDL_CFLAGS) $(X11_CFLAGS) $(ZLIB_CFLAGS)

AM_CPPFLAGS = $(AM_CFLAGS)

AM_LDFLAGS = $(SDL_LIBS) $(X11_LIBS) $(ZLIB_LIBS) $(SYSLIBS)


lib_LTLIBRARIES = libtcod-fov.la

libtcod_fov_root_includedir = $(includedir)
libtcod_fov_root_include_HEADERS = ../../src/libtcod-fov.h

include $(srcdir)/sources.am

libtcod_fov_la_CFLAGS = $(AM_CFLAGS) $(PTHREAD_CFLAGS)
libtcod_fov_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(SOVERSION) -lpthread
libtcod_fov_la_LIBADD = $(PTHREAD_LIBS) -lpthread

dist_noinst_DATA = \
	../../CHANGELOG.md \
	../../LIBTCOD-CREDITS.txt \
	../../LICENSE.txt \
	../../README.md

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libtcod-fov.pc
