#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([libtcod], [1.4.3], [jice@eptalys.net])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([1.10])
AC_CONFIG_SRCDIR([../src/bresenham_c.c])
AC_CONFIG_HEADERS([../include/config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AC_CHECK_PROG([AM_LDFLAGS], [sdl-config], `sdl-config --libs`,
              AC_MSG_FAILURE([SDL not found]))
AC_CHECK_PROG([AM_CFLAGS], [sdl-config], `sdl-config --cflags`,
              AC_MSG_FAILURE([SDL not found]))

# Checks for libraries.
AC_CHECK_LIB([png], [png_read_png], [],
             AC_MSG_FAILURE([libpng not found]))

# Checks for header files.
AC_PATH_X
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([inttypes.h malloc.h memory.h stddef.h stdint.h stdlib.h \
                  string.h strings.h unistd.h windows.h])
AC_CHECK_HEADERS([png.h], [], AC_MSG_FAILURE([png.h not found], 1))

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_INTPTR_T
AC_TYPE_UINTPTR_T
AC_CHECK_TYPES([ptrdiff_t])
AC_CHECK_SIZEOF([int *])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([strdup _strdup strcasecmp _stricmp strncasecmp _strnicmp strcasestr])
AC_CHECK_FUNCS([atexit floor memset mkdir rmdir select sqrt strchr strrchr strtol])

AC_CONFIG_FILES([../Makefile
                 ../src/Makefile
                 ../src/gui/Makefile])
AC_OUTPUT

