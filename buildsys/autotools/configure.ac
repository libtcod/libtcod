AC_INIT([libtcod],[m4_esyscmd_s(./get_version.py)])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([1.14 subdir-objects foreign])
AM_SILENT_RULES([yes])
AM_MAINTAINER_MODE([enable])

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

LT_INIT

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AM_PROG_CC_C_O

AC_ARG_ENABLE([samples], [AS_HELP_STRING([--enable-samples], [enable building sample applications])])
AM_CONDITIONAL([ENABLE_SAMPLES], [test "x${enable_samples}" != "xno"])

dnl -----------------------------------
dnl From wiki.libsdl.org/FAQLinux
dnl Check for SDL2
SDL_VERSION=2.0.5
AM_PATH_SDL2($SDL_VERSION, :, AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!]))
dnl -----------------------------------

AC_CHECK_HEADERS([stdlib.h math.h stdio.h stdarg.h string.h stdlib.h ctype.h wchar.h wctype.h time.h sys/stat.h X11/Xlib.h X11/Xatom.h unistd.h sys/types.h dirent.h errno.h pthread.h semaphore.h dlfcn.h])

AC_SEARCH_LIBS([atan2], [m], [:], [AC_MSG_ERROR([can't find the maths library])])

PKG_CHECK_MODULES([ZLIB], [zlib >= 1.2.0])

AS_CASE([$host_os],
    [mingw*], [:],
    [darwin*], [:],
    [linux-android*], [:],
    [haiku*], [:],
    [PKG_CHECK_MODULES([X11], [x11 >= 1.6.0])]
)

AS_CASE([$host_os],
    [mingw*], [:],
    [
        AC_SEARCH_LIBS([dlopen], [dl], [:], [AC_MSG_ERROR([can't find dlopen])])
        AX_PTHREAD([:], [AC_MSG_ERROR([can't find pthreads])])
    ]
)
AC_SUBST([SOVERSION], [m4_esyscmd_s(./get_version.py --so)])

AX_CXX_COMPILE_STDCXX([17], [noext], [mandatory])

# In older versions of gcc implementation of std::filesystem is kept
# in separate stdc++fs library. You should link it explicitly
AC_LANG_SAVE
AC_LANG([C++])
AC_MSG_CHECKING([if std::filesystem requires linking stdc++fs])
AC_LINK_IFELSE(
    [AC_LANG_SOURCE([
        #include <filesystem>
        int main() {
            std::filesystem::create_directory("/dev/null");
        }
    ])],
    [ac_cv_fs_stdlib=no],
    [ac_cv_fs_stdlib=yes]
)
if test "x$ac_cv_fs_stdlib" = xyes; then
    AC_MSG_RESULT(yes)
    SYSLIBS="$SYSLIBS -lstdc++fs"
else
    AC_MSG_RESULT(no)
fi
AC_LANG_RESTORE

AC_SUBST(SYSLIBS)

AC_CONFIG_FILES([Makefile libtcod.pc])
AC_OUTPUT
