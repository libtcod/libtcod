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
#ifndef TCOD_CONSOLE_ETC_H_
#define TCOD_CONSOLE_ETC_H_

#include <stdbool.h>
#ifndef NO_UNICODE
#include <wchar.h>
#endif

#include "color.h"
#include "config.h"
#include "console.h"
#include "console_types.h"
#include "context.h"
#include "error.h"
#include "image.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TCOD_BKGND_ALPHA(alpha) ((TCOD_bkgnd_flag_t)(TCOD_BKGND_ALPH | (((uint8_t)(alpha * 255)) << 8)))
#define TCOD_BKGND_ADDALPHA(alpha) ((TCOD_bkgnd_flag_t)(TCOD_BKGND_ADDA | (((uint8_t)(alpha * 255)) << 8)))

TCOD_DEPRECATED(
    "This function is not compatible with contexts.  Consider using tcod::load_tilesheet or TCOD_tileset_load instead."
    "  https://libtcod.readthedocs.io/en/latest/upgrading.html")
TCODLIB_API TCOD_Error
TCOD_console_set_custom_font(const char* fontFile, int flags, int nb_char_horiz, int nb_char_vertic);
TCOD_DEPRECATED("This function is not compatible with contexts.")
TCODLIB_API void TCOD_console_map_ascii_code_to_font(int asciiCode, int fontCharX, int fontCharY);
TCOD_DEPRECATED("This function is not compatible with contexts.")
TCODLIB_API void TCOD_console_map_ascii_codes_to_font(int asciiCode, int nbCodes, int fontCharX, int fontCharY);
TCOD_DEPRECATED("This function is not compatible with contexts.")
TCODLIB_API void TCOD_console_map_string_to_font(const char* s, int fontCharX, int fontCharY);
#ifndef NO_UNICODE
TCOD_DEPRECATED("This function is not compatible with contexts.")
TCODLIB_API void TCOD_console_map_string_to_font_utf(const wchar_t* s, int fontCharX, int fontCharY);
#endif

TCOD_DEPRECATED("This function does nothing.")
TCODLIB_API void TCOD_console_set_dirty(int x, int y, int w, int h);
/**
    Render and present a console with optional viewport options.

    `console` is the console to render.

    `viewport` is optional.

    Returns a negative values on error.  See `TCOD_get_error`.
    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_console_flush_ex(TCOD_Console* console, struct TCOD_ViewportOptions* viewport);
/**
 *  Render and present the root console to the active display.
 */
TCOD_PUBLIC TCOD_Error TCOD_console_flush(void);
/**
    Return True if the libtcod keycode is held.

    \rst
    .. deprecated:: 1.16
        You should instead use SDL_GetKeyboardState to check if keys are held.
    \endrst
 */
TCOD_DEPRECATED("Use SDL to check the keyboard state.")
TCODLIB_API bool TCOD_console_is_key_pressed(TCOD_keycode_t key);

/* ASCII paint file support */
TCODLIB_API TCOD_console_t TCOD_console_from_file(const char* filename);
TCODLIB_API bool TCOD_console_load_asc(TCOD_console_t con, const char* filename);
TCODLIB_API bool TCOD_console_load_apf(TCOD_console_t con, const char* filename);
TCODLIB_API bool TCOD_console_save_asc(TCOD_console_t con, const char* filename);
TCODLIB_API bool TCOD_console_save_apf(TCOD_console_t con, const char* filename);

#ifndef NO_SDL
/**
    Return immediately with a recently pressed key.

    \param flags A TCOD_event_t bit-field, for example: `TCOD_EVENT_KEY_PRESS`
    \return A TCOD_key_t struct with a recently pressed key.
            If no event exists then the `vk` attribute will be `TCODK_NONE`
 */
TCOD_DEPRECATED("This API is deprecated, use SDL_PollEvent instead.")
TCODLIB_API TCOD_key_t TCOD_console_check_for_keypress(int flags);
/**
    Wait for a key press event, then return it.

    \param flush If 1 then the event queue will be cleared before waiting for
                 the next event.  This should always be 0.
    \return A TCOD_key_t struct with the most recent key data.

    Do not solve input lag issues by arbitrarily dropping events!
 */
TCOD_DEPRECATED("This API is deprecated, use SDL_WaitEvent instead.")
TCODLIB_API TCOD_key_t TCOD_console_wait_for_keypress(bool flush);

TCOD_DEPRECATED("This function does not support contexts.  Consider using `TCOD_console_credits_render_ex`.")
TCODLIB_API void TCOD_console_credits(void);
TCOD_DEPRECATED("This function does not support contexts.")
TCODLIB_API void TCOD_console_credits_reset(void);
TCOD_DEPRECATED("This function does not support contexts.  Consider using `TCOD_console_credits_render_ex`.")
TCODLIB_API bool TCOD_console_credits_render(int x, int y, bool alpha);
/*****************************************************************************
    @brief Render a libtcod credit animation to a console.

    @param console The console to render to.
    @param x
    @param y
    @param alpha
    @param delta_time Delta time in seconds.
    @return Returns true once the credits animation has ended.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCODLIB_API bool TCOD_console_credits_render_ex(TCOD_Console* console, int x, int y, bool alpha, float delta_time);

TCOD_DEPRECATED("This function is a stub and will do nothing.")
TCODLIB_API void TCOD_console_set_keyboard_repeat(int initial_delay, int interval);
TCOD_DEPRECATED("This function is a stub and will do nothing.")
TCODLIB_API void TCOD_console_disable_keyboard_repeat(void);
#endif  // NO_SDL
#ifdef __cplusplus
}  // extern "C"
#endif
#endif  // TCOD_CONSOLE_ETC_H_
