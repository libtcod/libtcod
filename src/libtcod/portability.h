/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
#  if __WORDSIZE == 64
#    define TCOD_64BITS
#  endif
#endif

/* unicode rendering functions support */
#ifndef NO_UNICODE
#include <wchar.h>
#endif

/* DLL export */
#if defined _WIN32 || defined __CYGWIN__ || defined __MINGW32__
#ifndef TCODLIB_API
#ifdef LIBTCOD_EXPORTS
#ifdef __GNUC__
#define TCODLIB_API __attribute__((dllexport))
#else
#define TCODLIB_API __declspec(dllexport)
#endif // __GNUC__
#else
#ifdef __GNUC__
#define TCODLIB_API __attribute__((dllimport))
#else
#define TCODLIB_API __declspec(dllimport)
#endif // __GNUC__
#endif // LIBTCOD_EXPORTS
#endif // TCODLIB_API
#else
#if __GNUC__ >= 4
#define TCODLIB_API __attribute__((visibility("default")))
#else
#define TCODLIB_API
#endif // __GNUC__ >= 4
#endif

#ifndef TCODLIB_CAPI
#ifdef __cplusplus
#define TCODLIB_CAPI extern "C" TCODLIB_API
#else
#define TCODLIB_CAPI TCODLIB_API
#endif // __cplusplus
#endif // TCODLIB_CAPI

/* For now this encapsulates mouse, keyboard, and consoles themselves. */
#undef TCOD_CONSOLE_SUPPORT
#undef TCOD_IMAGE_SUPPORT
#undef TCOD_OSUTIL_SUPPORT

#ifndef TCOD_BARE
#define TCOD_CONSOLE_SUPPORT
#define TCOD_IMAGE_SUPPORT
#define TCOD_OSUTIL_SUPPORT
#endif

/* int types */
#include <stdint.h>

/* bool type */
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ansi C lacks support for those functions */
TCODLIB_API char *TCOD_strdup(const char *s);
TCODLIB_API int TCOD_strcasecmp(const char *s1, const char *s2);
TCODLIB_API int TCOD_strncasecmp(const char *s1, const char *s2, size_t n);

/* Define vswprintf across platforms. */
#ifdef _WIN32
#define vswprintf _vsnwprintf /* Windows */
#endif /* _WIN32 */

/* cross platform deprecation */
#if defined(__cplusplus) && __cplusplus >= 201402L && !defined(__clang__)
#define TCOD_DEPRECATED(msg) [[deprecated(msg)]]
#define TCOD_DEPRECATED_NOMESSAGE [[deprecated]]
#elif defined(_MSC_VER)
#define TCOD_DEPRECATED(msg) __declspec(deprecated(msg))
#define TCOD_DEPRECATED_NOMESSAGE __declspec(deprecated)
#elif defined(__GNUC__)
#define TCOD_DEPRECATED(msg) __attribute__ ((deprecated))
#define TCOD_DEPRECATED_NOMESSAGE __attribute__ ((deprecated))
#else
#define TCOD_DEPRECATED(msg)
#define TCOD_DEPRECATED_NOMESSAGE
#endif

/* Tells GCC the these functions are like printf. */
#ifdef __GNUC__
#define TCODLIB_FORMAT(str_index, first_arg) \
    __attribute__((format(printf, str_index, first_arg)))
#else
#define TCODLIB_FORMAT(str_index, first_arg)
#endif

#if defined(__cplusplus) && __cplusplus >= 201703L
#define TCOD_NODISCARD [[nodiscard]]
#elif defined(_MSC_VER)
#define TCOD_NODISCARD _Check_return_
#elif defined(__GNUC__)
#define TCOD_NODISCARD __attribute__ ((warn_unused_result))
#else
#define TCOD_NODISCARD
#endif

#ifdef __cplusplus
}
#endif

#endif /* LIBTCOD_PORTABILITY_H */
