/* BSD 3-Clause License
 *
 * Copyright © 2008-2021, Jice and the libtcod contributors.
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
#ifndef TCOD_CONSOLE_PRINTING_H_
#define TCOD_CONSOLE_PRINTING_H_

#ifdef __cplusplus
#include <array>
#include <string>
#endif

#include <stdbool.h>
#ifndef NO_UNICODE
#include <wchar.h>
#endif

#include "config.h"
#include "console_types.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif
TCOD_DEPRECATED("Use TCOD_console_printf instead.")
TCODLIB_API void TCOD_console_print(TCOD_Console* con, int x, int y, const char* fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_ex instead.")
TCODLIB_API void TCOD_console_print_ex(
    TCOD_Console* con, int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char* fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_rect instead.")
TCODLIB_API int TCOD_console_print_rect(TCOD_Console* con, int x, int y, int w, int h, const char* fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_rect_ex instead.")
TCODLIB_API int TCOD_console_print_rect_ex(
    TCOD_Console* con,
    int x,
    int y,
    int w,
    int h,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment,
    const char* fmt,
    ...);
TCOD_DEPRECATED("Use TCOD_console_printf_frame instead.")
TCODLIB_API void TCOD_console_print_frame(
    TCOD_console_t con, int x, int y, int w, int h, bool empty, TCOD_bkgnd_flag_t flag, const char* fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_get_height_rect_fmt instead.")
TCODLIB_API int TCOD_console_get_height_rect(TCOD_Console* con, int x, int y, int w, int h, const char* fmt, ...);

#ifndef NO_UNICODE
TCOD_DEPRECATED("Use TCOD_console_printf instead.")
TCODLIB_API void TCOD_console_print_utf(TCOD_Console* con, int x, int y, const wchar_t* fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_ex instead.")
TCODLIB_API void TCOD_console_print_ex_utf(
    TCOD_Console* con, int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const wchar_t* fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_rect instead.")
TCODLIB_API int TCOD_console_print_rect_utf(TCOD_Console* con, int x, int y, int w, int h, const wchar_t* fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_rect_ex instead.")
TCODLIB_API int TCOD_console_print_rect_ex_utf(
    TCOD_Console* con,
    int x,
    int y,
    int w,
    int h,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment,
    const wchar_t* fmt,
    ...);
TCOD_DEPRECATED("Use TCOD_console_get_height_rect_fmt instead.")
TCODLIB_API int TCOD_console_get_height_rect_utf(
    TCOD_Console* con, int x, int y, int w, int h, const wchar_t* fmt, ...);
#endif

typedef enum {
  TCOD_COLCTRL_1 = 1,
  TCOD_COLCTRL_2,
  TCOD_COLCTRL_3,
  TCOD_COLCTRL_4,
  TCOD_COLCTRL_5,
  TCOD_COLCTRL_NUMBER = 5,
  TCOD_COLCTRL_FORE_RGB,
  TCOD_COLCTRL_BACK_RGB,
  TCOD_COLCTRL_STOP
} TCOD_colctrl_t;

TCODLIB_API void TCOD_console_set_color_control(TCOD_colctrl_t con, TCOD_color_t fore, TCOD_color_t back);

/* UTF-8 functions */
/**
    Format and print a UTF-8 string to a console.
    \rst
    .. versionadded:: 1.8

    .. versionchanged:: 1.16
        Now returns a negative error code on failure.
    \endrst
 */
TCODLIB_API TCODLIB_FORMAT(4, 5) TCOD_Error
    TCOD_console_printf(TCOD_Console* __restrict con, int x, int y, const char* fmt, ...);
/**
    Format and print a UTF-8 string to a console.
    \rst
    .. versionadded:: 1.8

    .. versionchanged:: 1.16
        Now returns a negative error code on failure.
    \endrst
 */
TCODLIB_API TCODLIB_FORMAT(6, 7) TCOD_Error TCOD_console_printf_ex(
    TCOD_Console* __restrict con,
    int x,
    int y,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment,
    const char* fmt,
    ...);
/**
    Format and print a UTF-8 string to a console.
    \rst
    .. versionadded:: 1.8

    .. versionchanged:: 1.16
        Now returns a negative error code on failure.
    \endrst
 */
TCODLIB_API TCODLIB_FORMAT(6, 7) int TCOD_console_printf_rect(
    TCOD_Console* __restrict con, int x, int y, int w, int h, const char* fmt, ...);
/**
    Format and print a UTF-8 string to a console.
    \rst
    .. versionadded:: 1.8

    .. versionchanged:: 1.16
        Now returns a negative error code on failure.
    \endrst
 */
TCODLIB_API TCODLIB_FORMAT(8, 9) int TCOD_console_printf_rect_ex(
    TCOD_Console* __restrict con,
    int x,
    int y,
    int w,
    int h,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment,
    const char* fmt,
    ...);
/**
    Print a framed and optionally titled region to a console, using default
    colors and alignment.

    This function uses Unicode box-drawing characters and a UTF-8 formatted
    string.
    \rst
    .. versionadded:: 1.8

    .. versionchanged:: 1.16
        Now returns a negative error code on failure.
    \endrst
 */
TCODLIB_API TCODLIB_FORMAT(8, 9) TCOD_Error TCOD_console_printf_frame(
    struct TCOD_Console* __restrict con,
    int x,
    int y,
    int w,
    int h,
    int empty,
    TCOD_bkgnd_flag_t flag,
    const char* fmt,
    ...);
/**
    Return the number of lines that would be printed by this formatted string.
    \rst
    .. versionadded:: 1.8

    .. versionchanged:: 1.16
        Now returns a negative error code on failure.
    \endrst
 */
TCODLIB_API TCODLIB_FORMAT(6, 7) int TCOD_console_get_height_rect_fmt(
    struct TCOD_Console* con, int x, int y, int w, int h, const char* fmt, ...);

TCOD_PUBLIC TCOD_Error TCOD_console_printn(
    TCOD_Console* __restrict con,
    int x,
    int y,
    size_t n,
    const char* str,
    const TCOD_color_t* fg,
    const TCOD_color_t* bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment);
TCOD_PUBLIC int TCOD_console_printn_rect(
    TCOD_Console* __restrict con,
    int x,
    int y,
    int width,
    int height,
    size_t n,
    const char* str,
    const TCOD_color_t* fg,
    const TCOD_color_t* bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment);
TCOD_PUBLIC int TCOD_console_get_height_rect_n(
    TCOD_Console* __restrict console, int x, int y, int width, int height, size_t n, const char* __restrict str);
TCOD_PUBLIC int TCOD_console_get_height_rect_wn(int width, size_t n, const char* str);
TCOD_PUBLIC TCOD_Error TCOD_console_printn_frame(
    struct TCOD_Console* __restrict con,
    int x,
    int y,
    int width,
    int height,
    size_t n,
    const char* title,
    const TCOD_color_t* fg,
    const TCOD_color_t* bg,
    TCOD_bkgnd_flag_t flag,
    bool empty);
#ifdef __cplusplus
}  // extern "C"
namespace tcod {
inline void print(
    TCOD_Console& con,
    int x,
    int y,
    const std::string& str,
    const TCOD_color_t* fg,
    const TCOD_color_t* bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment) {
  check_throw_error(TCOD_console_printn(&con, x, y, str.size(), str.data(), fg, bg, flag, alignment));
}
inline int print_rect(
    TCOD_Console& con,
    int x,
    int y,
    int width,
    int height,
    const std::string& str,
    const TCOD_color_t* fg,
    const TCOD_color_t* bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment) {
  return check_throw_error(
      TCOD_console_printn_rect(&con, x, y, width, height, str.size(), str.data(), fg, bg, flag, alignment));
}
inline int get_height_rect(TCOD_Console& console, int x, int y, int width, int height, const std::string& str) {
  return check_throw_error(TCOD_console_get_height_rect_n(&console, x, y, width, height, str.size(), str.data()));
}
inline int get_height_rect(int width, const std::string& str) {
  return check_throw_error(TCOD_console_get_height_rect_wn(width, str.size(), str.data()));
}
inline void print_frame(
    struct TCOD_Console& con,
    int x,
    int y,
    int width,
    int height,
    const std::string& title,
    const TCOD_color_t* fg,
    const TCOD_color_t* bg,
    TCOD_bkgnd_flag_t flag,
    bool empty) {
  check_throw_error(
      TCOD_console_printn_frame(&con, x, y, width, height, title.size(), title.data(), fg, bg, flag, empty));
}
}  // namespace tcod
#endif  // __cplusplus
#endif /* TCOD_CONSOLE_PRINTING_H_ */
