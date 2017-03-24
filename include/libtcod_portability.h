/*
* libtcod 1.6.3
* Copyright (c) 2008,2009,2010,2012,2013,2016,2017 Jice & Mingos & rmtew
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice or Mingos may not be used to endorse or promote
*       products derived from this software without specific prior written
*       permission.
*
* THIS SOFTWARE IS PROVIDED BY JICE, MINGOS AND RMTEW ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE, MINGOS OR RMTEW BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef LIBTCOD_PORTABILITY_H
#define LIBTCOD_PORTABILITY_H
/* uncomment to disable unicode support */
/*#define NO_UNICODE */

/* uncomment to disable opengl support */
/*#define NO_OPENGL */

/* os identification
   TCOD_WINDOWS : OS is windows
   TCOD_LINUX : OS is Linux
   TCOD_MACOSX : OS is Mac OS X
   TCOD_HAIKU : OS is Haiku */

/* compiler identification
   TCOD_VISUAL_STUDIO : compiler is Microsoft Visual Studio
   TCOD_MINGW32 : compiler is Mingw32
   TCOD_GCC : compiler is gcc/g++ */

/* word size
   TCOD_64BITS : 64 bits OS
   TCOD_WIN64 : 64 bits Windows
   TCOD_WIN32 : 32 bits Windows
   TCOD_LINUX64 : 64 bits Linux
   TCOD_LINUX32 : 32 bits Linux
   TCOD_FREEBSD64 : 64 bits FreeBSD
   TCOD_FREEBSD32 : 32 bits FreeBSD */

#if defined( _MSC_VER )
#  define TCOD_VISUAL_STUDIO
#  define TCOD_WINDOWS
#  ifdef _WIN64
#    define TCOD_WIN64
#    define TCOD_64BITS
#  else
#    define TCOD_WIN32
#  endif
#elif defined( __MINGW32__ )
#  define TCOD_WINDOWS
#  define TCOD_MINGW32
#  ifdef _WIN64
#    define TCOD_WIN64
#    define TCOD_64BITS
#  else
#    define TCOD_WIN32
#  endif
#elif defined( __HAIKU__ )
#  define TCOD_HAIKU
#  define TCOD_GCC
#  if __WORDSIZE == 64
#    define TCOD_64BITS
#  endif
#elif defined( __linux )
#  define TCOD_LINUX
#  define TCOD_GCC
#  if __WORDSIZE == 64
#    define TCOD_LINUX64
#    define TCOD_64BITS
#  else
#    define TCOD_LINUX32
#  endif
#elif defined( __FreeBSD__ )
#  define TCOD_FREEBSD
#  define TCOD_GCC
#  if __WORDSIZE == 64
#    define TCOD_FREEBSD64
#    define TCOD_64BITS
#  else
#    define TCOD_FREEBSD32
#  endif
#elif defined (__APPLE__) && defined (__MACH__)
#  define TCOD_MACOSX
#  define TCOD_GCC
#endif

/* unicode rendering functions support */
#ifndef NO_UNICODE
#include <wchar.h>
#endif

/* DLL export */
#ifndef TCODLIB_API
#ifdef TCOD_WINDOWS
#ifdef LIBTCOD_EXPORTS
#define TCODLIB_API __declspec(dllexport)
#else
#define TCODLIB_API __declspec(dllimport)
#endif
#else
#define TCODLIB_API
#endif
#endif

/* For now this encapsulates mouse, keyboard, and consoles themselves. */
#undef TCOD_CONSOLE_SUPPORT
#undef TCOD_IMAGE_SUPPORT
#undef TCOD_OSUTIL_SUPPORT

#ifdef TCOD_SDL2
#define TCOD_CONSOLE_SUPPORT
#define TCOD_IMAGE_SUPPORT
#define TCOD_OSUTIL_SUPPORT
#else
#define TCOD_BARE
#endif

/* int types */
#include "external/pstdint.h"

/* bool type */
#ifndef __bool_true_false_are_defined
#ifndef __cplusplus
#if defined(_MSC_VER) && _MSC_VER < 1800
typedef uint8_t bool;
#define true 1
#define false 0
#define __bool_true_false_are_defined 1
#else
#include <stdbool.h>
#endif
#endif /* __cplusplus */
#endif /* __bool_true_false_are_defined */

#ifdef __cplusplus
extern "C" {
#endif

/* ansi C lacks support for those functions */
TCODLIB_API char *TCOD_strdup(const char *s);
TCODLIB_API int TCOD_strcasecmp(const char *s1, const char *s2);
TCODLIB_API int TCOD_strncasecmp(const char *s1, const char *s2, size_t n);

#if defined(TCOD_WINDOWS)
char *strcasestr (const char *haystack, const char *needle);
#endif
#if defined(TCOD_LINUX) || defined(TCOD_HAIKU) || defined(TCOD_FREEBSD) || defined(TCOD_MACOSX)
#define vsnwprintf vswprintf
#endif
#ifdef TCOD_WINDOWS
#define vsnwprintf _vsnwprintf
#endif

#ifdef __cplusplus
}
#endif

#endif /* LIBTCOD_PORTABILITY_H */
