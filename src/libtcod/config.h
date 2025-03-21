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
#ifndef LIBTCOD_CONFIG_H_
#define LIBTCOD_CONFIG_H_

/* DLL export */
#ifndef TCODLIB_API
#ifdef LIBTCOD_STATIC
#define TCODLIB_API
#elif defined _WIN32 || defined __CYGWIN__ || defined __MINGW32__
#ifdef LIBTCOD_EXPORTS
#ifdef __GNUC__
#define TCODLIB_API __attribute__((dllexport))
#else
#define TCODLIB_API __declspec(dllexport)
#endif  // __GNUC__
#else
#ifdef __GNUC__
#define TCODLIB_API __attribute__((dllimport))
#else
#define TCODLIB_API __declspec(dllimport)
#endif  // __GNUC__
#endif  // LIBTCOD_EXPORTS
#elif __GNUC__ >= 4
#define TCODLIB_API __attribute__((visibility("default")))
#else
#define TCODLIB_API
#endif
#endif  // TCODLIB_API

#ifndef TCODLIB_API_INLINE_EXPORT
#ifdef LIBTCOD_EXPORTS
/// Only export, but don't import a symbol.  Used to export functions moved inline.
#define TCODLIB_API_INLINE_EXPORT TCODLIB_API
#else
#define TCODLIB_API_INLINE_EXPORT
#endif
#endif  // TCODLIB_API_INLINE_EXPORT

#ifndef TCODLIB_CAPI
#ifdef __cplusplus
#define TCODLIB_CAPI extern "C" TCODLIB_API
#else
#define TCODLIB_CAPI TCODLIB_API
#endif  // __cplusplus
#endif  // TCODLIB_CAPI

// Publicly visible symbols and functions.
#define TCOD_PUBLIC TCODLIB_API

// Private hidden symbols.
#if __GNUC__ >= 4
#define TCOD_PRIVATE __attribute__((visibility("hidden")))
#else
#define TCOD_PRIVATE
#endif  // __GNUC__ >= 4

// Cross platform deprecation.
#ifdef TCOD_IGNORE_DEPRECATED
#define TCOD_DEPRECATED(msg)
#define TCOD_DEPRECATED_NOMESSAGE
#define TCOD_DEPRECATED_ENUM
#elif defined(__cplusplus) && __cplusplus >= 201402L && !defined(__clang__)
#define TCOD_DEPRECATED(msg) [[deprecated(msg)]]
#define TCOD_DEPRECATED_NOMESSAGE [[deprecated]]
#define TCOD_DEPRECATED_ENUM [[deprecated]]
#elif defined(_MSC_VER)
#define TCOD_DEPRECATED(msg) __declspec(deprecated(msg))
#define TCOD_DEPRECATED_NOMESSAGE __declspec(deprecated)
#define TCOD_DEPRECATED_ENUM
#elif defined(__GNUC__)
#define TCOD_DEPRECATED(msg) __attribute__((deprecated(msg)))
#define TCOD_DEPRECATED_NOMESSAGE __attribute__((deprecated))
#define TCOD_DEPRECATED_ENUM __attribute__((deprecated))
#else
#define TCOD_DEPRECATED(msg)
#define TCOD_DEPRECATED_NOMESSAGE
#define TCOD_DEPRECATED_ENUM
#endif

#ifdef __GNUC__
// Tells GCC the these functions are printf-like.
#define TCODLIB_PRINTF(str_index, first_arg) __attribute__((format(printf, str_index, first_arg)))
#else
#define TCODLIB_PRINTF(str_index, first_arg)
#endif

#define TCODLIB_FORMAT TCODLIB_PRINTF

#if defined(__cplusplus) && __cplusplus >= 201703L && !defined(__clang__)
#define TCOD_NODISCARD [[nodiscard]]
#elif defined(_MSC_VER)
#define TCOD_NODISCARD
#elif defined(__GNUC__)
#define TCOD_NODISCARD __attribute__((warn_unused_result))
#else
#define TCOD_NODISCARD
#endif

#ifdef __GNUC__
/// Used to suppress internal header warnings.
#define TCODLIB_BEGIN_IGNORE_DEPRECATIONS \
  _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wdeprecated\"")
#define TCODLIB_END_IGNORE_DEPRECATIONS _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
#define TCODLIB_BEGIN_IGNORE_DEPRECATIONS _Pragma("warning(push)") _Pragma("warning(disable : 4996)")
#define TCODLIB_END_IGNORE_DEPRECATIONS _Pragma("warning(pop)")
#else
#define TCODLIB_BEGIN_IGNORE_DEPRECATIONS
#define TCODLIB_END_IGNORE_DEPRECATIONS
#endif

#ifndef TCOD_FALLBACK_FONT_SIZE
// The default height of the fallback font size in pixels.
#define TCOD_FALLBACK_FONT_SIZE 16
#endif  // TCOD_FALLBACK_FONT_SIZE

#endif  // LIBTCOD_CONFIG_H_
