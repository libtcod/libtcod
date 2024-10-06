/* BSD 3-Clause License
 *
 * Copyright © 2008-2023, Jice and the libtcod contributors.
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
#ifndef LIBTCODFOV_CONFIG_H_
#define LIBTCODFOV_CONFIG_H_

/* DLL export */
#ifndef TCODFOV_PUBLIC
#ifdef TCODFOV_STATIC
/// Publicly visible symbols and functions.
#define TCODFOV_PUBLIC
#elif defined _WIN32 || defined __CYGWIN__ || defined __MINGW32__
#ifdef TCODFOV_EXPORTS
#ifdef __GNUC__
#define TCODFOV_PUBLIC __attribute__((dllexport))
#else
#define TCODFOV_PUBLIC __declspec(dllexport)
#endif  // __GNUC__
#else
#ifdef __GNUC__
#define TCODFOV_PUBLIC __attribute__((dllimport))
#else
#define TCODFOV_PUBLIC __declspec(dllimport)
#endif  // __GNUC__
#endif  // TCODFOV_EXPORTS
#elif __GNUC__ >= 4
#define TCODFOV_PUBLIC __attribute__((visibility("default")))
#else
#define TCODFOV_PUBLIC
#endif
#endif  // TCODFOV_PUBLIC

#ifndef TCODFOV_PUBLIC_INLINE_EXPORT
#ifdef TCODFOV_EXPORTS
/// Only export, but don't import a symbol.  Used to export functions moved inline.
#define TCODFOV_PUBLIC_INLINE_EXPORT TCODFOV_PUBLIC
#else
#define TCODFOV_PUBLIC_INLINE_EXPORT
#endif
#endif  // TCODFOV_PUBLIC_INLINE_EXPORT

// Private hidden symbols.
#if __GNUC__ >= 4
#define TCODFOV_PRIVATE __attribute__((visibility("hidden")))
#else
#define TCODFOV_PRIVATE
#endif  // __GNUC__ >= 4

// Cross platform deprecation.
#ifdef TCODFOV_IGNORE_DEPRECATED
#define TCODFOV_DEPRECATED(msg)
#define TCODFOV_DEPRECATED_NOMESSAGE
#define TCODFOV_DEPRECATED_ENUM
#elif defined(__cplusplus) && __cplusplus >= 201402L && !defined(__clang__)
#define TCODFOV_DEPRECATED(msg) [[deprecated(msg)]]
#define TCODFOV_DEPRECATED_NOMESSAGE [[deprecated]]
#define TCODFOV_DEPRECATED_ENUM [[deprecated]]
#elif defined(_MSC_VER)
#define TCODFOV_DEPRECATED(msg) __declspec(deprecated(msg))
#define TCODFOV_DEPRECATED_NOMESSAGE __declspec(deprecated)
#define TCODFOV_DEPRECATED_ENUM
#elif defined(__GNUC__)
#define TCODFOV_DEPRECATED(msg) __attribute__((deprecated(msg)))
#define TCODFOV_DEPRECATED_NOMESSAGE __attribute__((deprecated))
#define TCODFOV_DEPRECATED_ENUM __attribute__((deprecated))
#else
#define TCODFOV_DEPRECATED(msg)
#define TCODFOV_DEPRECATED_NOMESSAGE
#define TCODFOV_DEPRECATED_ENUM
#endif

#ifdef __GNUC__
// Tells GCC the these functions are printf-like.
#define TCODFOV_FORMAT(str_index, first_arg) __attribute__((format(printf, str_index, first_arg)))
#else
#define TCODFOV_FORMAT(str_index, first_arg)
#endif

#if defined(__cplusplus) && __cplusplus >= 201703L && !defined(__clang__)
#define TCODFOV_NODISCARD [[nodiscard]]
#elif defined(_MSC_VER)
#define TCODFOV_NODISCARD
#elif defined(__GNUC__)
#define TCODFOV_NODISCARD __attribute__((warn_unused_result))
#else
#define TCODFOV_NODISCARD
#endif

#endif  // LIBTCODFOV_CONFIG_H_
