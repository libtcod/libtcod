/* libtcod
 * Copyright © 2008-2018 Jice and the libtcod contributors.
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
#ifndef TCOD_CONSOLE_PRINTING_H_
#define TCOD_CONSOLE_PRINTING_H_

#include "../portability.h"
#include "../console_types.h"

#ifdef __cplusplus
extern "C" {
#endif
TCOD_DEPRECATED("Use TCOD_console_printf instead.")
TCODLIB_API void TCOD_console_print(TCOD_Console* con,int x, int y, const char *fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_ex instead.")
TCODLIB_API void TCOD_console_print_ex(TCOD_Console* con,int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char *fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_rect instead.")
TCODLIB_API int TCOD_console_print_rect(TCOD_Console* con,int x, int y, int w, int h, const char *fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_rect_ex instead.")
TCODLIB_API int TCOD_console_print_rect_ex(TCOD_Console* con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char *fmt, ...);
TCODLIB_API void TCOD_console_print_frame(TCOD_console_t con,int x,int y,int w,int h, bool empty, TCOD_bkgnd_flag_t flag, const char *fmt, ...);
TCODLIB_API int TCOD_console_get_height_rect(TCOD_Console* con,int x, int y, int w, int h, const char *fmt, ...);

#ifndef NO_UNICODE
TCOD_DEPRECATED("Use TCOD_console_printf instead.")
TCODLIB_API void TCOD_console_print_utf(TCOD_Console* con,int x, int y, const wchar_t *fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_ex instead.")
TCODLIB_API void TCOD_console_print_ex_utf(TCOD_Console* con,int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const wchar_t *fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_rect instead.")
TCODLIB_API int TCOD_console_print_rect_utf(TCOD_Console* con,int x, int y, int w, int h, const wchar_t *fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_rect_ex instead.")
TCODLIB_API int TCOD_console_print_rect_ex_utf(TCOD_Console* con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const wchar_t *fmt, ...);
TCODLIB_API int TCOD_console_get_height_rect_utf(TCOD_Console* con,int x, int y, int w, int h, const wchar_t *fmt, ...);
#endif

/* UTF-8 functions */
TCODLIB_API TCODLIB_FORMAT(4, 5) void TCOD_console_printf(
    TCOD_Console* con, int x, int y, const char *fmt, ...);
TCODLIB_API TCODLIB_FORMAT(6, 7) void TCOD_console_printf_ex(
    TCOD_Console* con, int x, int y, TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment, const char *fmt, ...);
TCODLIB_API TCODLIB_FORMAT(6, 7) int TCOD_console_printf_rect(
    TCOD_Console* con, int x, int y, int w, int h, const char *fmt, ...);
TCODLIB_API TCODLIB_FORMAT(8, 9) int TCOD_console_printf_rect_ex(
    TCOD_Console* con,int x, int y, int w, int h,
    TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char *fmt, ...);
TCODLIB_API TCODLIB_FORMAT(8, 9)
void TCOD_console_printf_frame(struct TCOD_Console *con,
                               int x, int y, int w, int h, int empty,
                               TCOD_bkgnd_flag_t flag, const char *fmt, ...);
TCODLIB_API TCODLIB_FORMAT(6, 7)
int TCOD_console_get_height_rect_fmt(struct TCOD_Console *con,
                                     int x, int y, int w, int h,
                                     const char *fmt, ...);

/* Private internal functions. */
int TCOD_console_print_internal_utf8_(
    TCOD_Console* con, int x, int y, int max_width, int max_height,
    TCOD_bkgnd_flag_t flag, TCOD_alignment_t align,
    const unsigned char *string, int can_split, int count_only);
#ifdef __cplusplus
}
#endif

#endif /* TCOD_CONSOLE_PRINTING_H_ */
