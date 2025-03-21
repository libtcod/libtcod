/* BSD 3-Clause License
 *
 * Copyright © 2008-2025, Jice and the libtcod contributors.
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
#pragma once
#ifndef LIBTCOD_PORTABILITY_H
#define LIBTCOD_PORTABILITY_H
#include "config.h"
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

#if defined(_MSC_VER)
#define TCOD_VISUAL_STUDIO
#define TCOD_WINDOWS
#ifdef _WIN64
#define TCOD_WIN64
#define TCOD_64BITS
#else
#define TCOD_WIN32
#endif
#elif defined(__MINGW32__)
#define TCOD_WINDOWS
#define TCOD_MINGW32
#ifdef _WIN64
#define TCOD_WIN64
#define TCOD_64BITS
#else
#define TCOD_WIN32
#endif
#elif defined(__HAIKU__)
#define TCOD_HAIKU
#define TCOD_GCC
#if __WORDSIZE == 64
#define TCOD_64BITS
#endif
#elif defined(__linux)
#define TCOD_LINUX
#define TCOD_GCC
#if __WORDSIZE == 64
#define TCOD_LINUX64
#define TCOD_64BITS
#else
#define TCOD_LINUX32
#endif
#elif defined(__FreeBSD__)
#define TCOD_FREEBSD
#define TCOD_GCC
#if __WORDSIZE == 64
#define TCOD_FREEBSD64
#define TCOD_64BITS
#else
#define TCOD_FREEBSD32
#endif
#elif defined(__APPLE__) && defined(__MACH__)
#define TCOD_MACOSX
#define TCOD_GCC
#if __WORDSIZE == 64
#define TCOD_64BITS
#endif
#endif

/* unicode rendering functions support */
#ifndef NO_UNICODE
#include <wchar.h>
#endif

/* int types */
#include <stdint.h>

/* bool type */
#include <stdbool.h>

/**
    Allocate and return a duplicate of string `s`.  The returned memory must be freed manually.
 */
TCODLIB_CAPI char* TCOD_strdup(const char* s);
/***************************************************************************
    @brief Compare two ASCII strings ignoring case. Returns 0 if the strings are equal.
 */
TCODLIB_CAPI int TCOD_strcasecmp(const char* s1, const char* s2);
/***************************************************************************
    @brief Compare two ASCII strings ignoring case. Returns 0 if the strings are equal.
 */
TCODLIB_CAPI int TCOD_strncasecmp(const char* s1, const char* s2, size_t n);

/* Define vswprintf across platforms. */
#ifdef _WIN32
#define vswprintf _vsnwprintf /* Windows */
#endif /* _WIN32 */
#endif /* LIBTCOD_PORTABILITY_H */
