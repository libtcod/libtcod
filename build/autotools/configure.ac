AC_INIT([libtcod], [1.6])
AM_INIT_AUTOMAKE([subdir-objects])

AC_CONFIG_MACRO_DIR([m4])

LT_INIT

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AM_PROG_CC_C_O

dnl -----------------------------------
dnl From wiki.libsdl.org/FAQLinux
dnl Check for SDL2
SDL_VERSION=2.0.0
AM_PATH_SDL2($SDL_VERSION, :, AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!]))
CFLAGS="$CFLAGS $SDL_CFLAGS"
CPPFLAGS="$CPPFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"
dnl -----------------------------------

AC_CHECK_HEADERS([stdlib.h math.h stdio.h stdarg.h string.h stdlib.h ctype.h wchar.h wctype.h time.h sys/stat.h X11/Xlib.h X11/Xatom.h unistd.h sys/types.h dirent.h errno.h pthread.h semaphore.h dlfcn.h zlib.h])

AC_CONFIG_FILES([Makefile gui/Makefile])
AC_OUTPUT
