/* libtcod
 * Copyright © 2008-2019 Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The name of copyright holder nor the names of its contributors may not
 *       be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _TCOD_CONSOLE_H
#define _TCOD_CONSOLE_H

#include "portability.h"

#ifdef TCOD_CONSOLE_SUPPORT

#include "console/drawing.h"
#include "console/printing.h"
#include "console/rexpaint.h"
#include "color.h"
#include "console_types.h"
#include "image.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TCOD_BKGND_ALPHA(alpha) ((TCOD_bkgnd_flag_t)(TCOD_BKGND_ALPH|(((uint8_t)(alpha*255))<<8)))
#define TCOD_BKGND_ADDALPHA(alpha) ((TCOD_bkgnd_flag_t)(TCOD_BKGND_ADDA|(((uint8_t)(alpha*255))<<8)))

TCODLIB_API void TCOD_console_set_custom_font(const char *fontFile, int flags,int nb_char_horiz, int nb_char_vertic);
TCODLIB_API void TCOD_console_map_ascii_code_to_font(int asciiCode, int fontCharX, int fontCharY);
TCODLIB_API void TCOD_console_map_ascii_codes_to_font(int asciiCode, int nbCodes, int fontCharX, int fontCharY);
TCODLIB_API void TCOD_console_map_string_to_font(const char *s, int fontCharX, int fontCharY);
#ifndef NO_UNICODE
TCODLIB_API void TCOD_console_map_string_to_font_utf(const wchar_t *s, int fontCharX, int fontCharY);
#endif

TCODLIB_API void TCOD_console_set_dirty(int x, int y, int w, int h);
TCODLIB_API void TCOD_console_set_default_background(TCOD_console_t con,TCOD_color_t col);
TCODLIB_API void TCOD_console_set_default_foreground(TCOD_console_t con,TCOD_color_t col);
TCODLIB_API void TCOD_console_clear(TCOD_console_t con);
TCODLIB_API void TCOD_console_set_char_background(TCOD_console_t con,int x, int y, TCOD_color_t col, TCOD_bkgnd_flag_t flag);
TCODLIB_API void TCOD_console_set_char_foreground(TCOD_console_t con,int x, int y, TCOD_color_t col);
TCODLIB_API void TCOD_console_set_char(TCOD_console_t con,int x, int y, int c);
TCODLIB_API void TCOD_console_put_char(TCOD_console_t con,int x, int y, int c, TCOD_bkgnd_flag_t flag);
TCODLIB_API void TCOD_console_put_char_ex(TCOD_console_t con,int x, int y, int c, TCOD_color_t fore, TCOD_color_t back);

TCODLIB_API void TCOD_console_set_background_flag(TCOD_console_t con,TCOD_bkgnd_flag_t flag);
TCODLIB_API TCOD_bkgnd_flag_t TCOD_console_get_background_flag(TCOD_console_t con);
TCODLIB_API void TCOD_console_set_alignment(TCOD_console_t con,TCOD_alignment_t alignment);
TCODLIB_API TCOD_alignment_t TCOD_console_get_alignment(TCOD_console_t con);

TCODLIB_API TCOD_color_t TCOD_console_get_default_background(TCOD_console_t con);
TCODLIB_API TCOD_color_t TCOD_console_get_default_foreground(TCOD_console_t con);
TCODLIB_API TCOD_color_t TCOD_console_get_char_background(
    const TCOD_Console* con,int x, int y);
TCODLIB_API TCOD_color_t TCOD_console_get_char_foreground(
    const TCOD_Console* con,int x, int y);
TCODLIB_API int TCOD_console_get_char(const TCOD_Console* con,int x, int y);

TCODLIB_API void TCOD_console_set_fade(uint8_t val, TCOD_color_t fade);
TCODLIB_API uint8_t TCOD_console_get_fade(void);
TCODLIB_API TCOD_color_t TCOD_console_get_fading_color(void);

TCODLIB_API void TCOD_console_flush(void);

TCODLIB_API void TCOD_console_set_color_control(TCOD_colctrl_t con, TCOD_color_t fore, TCOD_color_t back);

TCODLIB_API TCOD_key_t TCOD_console_check_for_keypress(int flags);
TCODLIB_API TCOD_key_t TCOD_console_wait_for_keypress(bool flush);
TCODLIB_API bool TCOD_console_is_key_pressed(TCOD_keycode_t key);

/* ASCII paint file support */
TCODLIB_API TCOD_console_t TCOD_console_from_file(const char *filename);
TCODLIB_API bool TCOD_console_load_asc(TCOD_console_t con, const char *filename);
TCODLIB_API bool TCOD_console_load_apf(TCOD_console_t con, const char *filename);
TCODLIB_API bool TCOD_console_save_asc(TCOD_console_t con, const char *filename);
TCODLIB_API bool TCOD_console_save_apf(TCOD_console_t con, const char *filename);

TCODLIB_API TCOD_console_t TCOD_console_new(int w, int h);
TCODLIB_API int TCOD_console_get_width(const TCOD_Console* con);
TCODLIB_API int TCOD_console_get_height(const TCOD_Console* con);
TCODLIB_API void TCOD_console_set_key_color(TCOD_console_t con,TCOD_color_t col);
TCODLIB_API void TCOD_console_blit(TCOD_console_t src,int xSrc, int ySrc, int wSrc, int hSrc, TCOD_console_t dst, int xDst, int yDst, float foreground_alpha, float background_alpha);
TCODLIB_API void TCOD_console_blit_key_color(
    TCOD_console_t src,int xSrc, int ySrc, int wSrc, int hSrc,
    TCOD_console_t dst, int xDst, int yDst,
    float foreground_alpha, float background_alpha,
    const TCOD_color_t *key_color);
TCODLIB_API void TCOD_console_delete(TCOD_console_t console);

TCODLIB_API void TCOD_console_credits(void);
TCODLIB_API void TCOD_console_credits_reset(void);
TCODLIB_API bool TCOD_console_credits_render(int x, int y, bool alpha);

TCOD_DEPRECATED("This function is a stub and will do nothing.")
TCODLIB_API void TCOD_console_set_keyboard_repeat(int initial_delay,
                                                  int interval);
TCOD_DEPRECATED("This function is a stub and will do nothing.")
TCODLIB_API void TCOD_console_disable_keyboard_repeat();
#ifdef __cplusplus
}
#endif

#endif /* TCOD_CONSOLE_SUPPORT */

#endif /* _TCOD_CONSOLE_H */
