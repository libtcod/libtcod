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
#ifndef TCOD_CONSOLE_REXPAINT_H_
#define TCOD_CONSOLE_REXPAINT_H_
#ifndef TCOD_NO_ZLIB

#ifdef __cplusplus
#include <string>
#include <vector>
#endif  // __cplusplus

#include "config.h"
#include "console_types.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
/**
    \brief Return a new console loaded from a REXPaint ``.xp`` file.

    \param [in] filename A path to the REXPaint file.
    \return A new TCOD_console_t object.  New consoles will need
    to be deleted with a call to :any:`TCOD_console_delete`.
    Returns NULL on an error.

 */
TCODLIB_API TCOD_console_t TCOD_console_from_xp(const char* filename);
/**
    \brief Update a console from a REXPaint ``.xp`` file.

    \param [out] con A console instance to update from the REXPaint file.
    \param [in] filename A path to the REXPaint file.

    In C++, you can pass the filepath directly to the :any:`TCODConsole`
    constructor to load a REXPaint file.
 */
TCODLIB_API bool TCOD_console_load_xp(TCOD_Console* con, const char* filename);
/**
    \brief Save a console as a REXPaint ``.xp`` file.

    \param [in] con The console instance to save.
    \param [in] filename The filepath to save to.
    \param [in] compress_level A zlib compression level, from 0 to 9.
    1=fast, 6=balanced, 9=slowest, 0=uncompressed.
    \return ``true`` when the file is saved successfully, or ``false`` when an
    issue is detected.

    The REXPaint format can support a 1:1 copy of a libtcod console.
 */
TCODLIB_API bool TCOD_console_save_xp(const TCOD_Console* con, const char* filename, int compress_level);
/**
    \brief Return a list of consoles from a REXPaint file.

    \param [in] filename A path to the REXPaint file.
    \return Returns a TCOD_list_t of TCOD_console_t objects.  Or NULL on an
    error.  You will need to delete this list and each console individually.

    This function can load a REXPaint file with variable layer shapes,
    which would cause issues for a function like TCOD_console_list_from_xp.

    \rst
    .. deprecated:: 1.20
        TCOD_list_t is deprecated, use TCOD_load_xp instead.
    \endrst
 */
TCOD_DEPRECATED("TCOD_list_t is deprecated, use TCOD_load_xp instead.")
TCODLIB_API TCOD_list_t TCOD_console_list_from_xp(const char* filename);
/**
    \brief Save a list of consoles to a REXPaint file.

    \param [in] console_list A TCOD_list_t of TCOD_console_t objects.
    \param [in] filename Path to save to.
    \param [in] compress_level zlib compression level.
    \return true on success, false on a failure such as not being able to write
    to the path provided.

    This function can save any number of layers with multiple
    different sizes.

    The REXPaint tool only supports files with up to 9 layers where
    all layers are the same size.

    \rst
    .. deprecated:: 1.20
        TCOD_list_t is deprecated, use TCOD_save_xp instead.
    \endrst
 */
TCOD_DEPRECATED("TCOD_list_t is deprecated, use TCOD_save_xp instead.")
TCODLIB_API bool TCOD_console_list_save_xp(TCOD_list_t console_list, const char* filename, int compress_level);
/**
    @brief Load an array of consoles from a REXPaint file in memory.

    You can call this function with `n_out=0` and `out=NULL` to get the number of consoles in the file.

    @param n_data The length of the input `data` buffer.
    @param data The buffer where the REXPaint file is held.
    @param n_out The length of the output console `out` array.  Can be zero.
    @param out The array to fill with loaded consoles.
    @return Returns the number of consoles held by the file.  Returns a negative error code on error.

    \rst
    .. versionadded:: 1.18
    \endrst
 */
TCODLIB_API int TCOD_load_xp_from_memory(int n_data, const unsigned char* data, int n_out, TCOD_Console** out);
/**
    @brief Save an array of consoles to a REXPaint file in memory.

    Partially initialized consoles are released on failures.

    @param n_consoles The length of the input `consoles` array.
    @param consoles An array of tcod consoles, can not be NULL.
    @param n_out The size of the `out` buffer, if this is zero then upper bound to be returned.
    @param out A pointer to an output buffer, can be NULL.
    @param compression_level A compression level for the zlib library.
    @return If `out=NULL` then returns the upper bound of the buffer size needed.
            Otherwise this returns the number of bytes actually filled.
            On an error a negative error code is returned.

    \rst
    .. versionadded:: 1.18
    \endrst
 */
TCODLIB_API int TCOD_save_xp_to_memory(
    int n_consoles, const TCOD_Console* const* consoles, int n_out, unsigned char* out, int compression_level);
/**
    @brief Load an array of consoles from a REXPaint file.

    @param path The path to the REXPaint file, can not be NULL.
    @param n The size of the `out` array.  Can be zero.
    @param out The array to fill with loaded consoles.
    @return Returns the number of consoles held by the file.  Returns a negative error code on error.

    \rst
    .. versionadded:: 1.18
    \endrst
 */
TCODLIB_API int TCOD_load_xp(const char* path, int n, TCOD_Console** out);
/**
    @brief Save an array of consoles to a REXPaint file.

    Partially initialized consoles are released on failures.

    @param n The number of consoles in the `consoles` array.
    @param consoles An array of consoles.
    @param path The path write the REXPaint file, can not be NULL.
    @param compress_level A compression level for the zlib library.
    @return Returns an error code on failure.

    \rst
    .. versionadded:: 1.18
    \endrst
 */
TCODLIB_API TCOD_Error TCOD_save_xp(int n, const TCOD_Console* const* consoles, const char* path, int compress_level);
#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
#endif  // TCOD_NO_ZLIB
#endif  // TCOD_CONSOLE_REXPAINT_H_
