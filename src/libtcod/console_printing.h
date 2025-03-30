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
#ifndef TCOD_CONSOLE_PRINTING_H_
#define TCOD_CONSOLE_PRINTING_H_

#include <stdarg.h>
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
/// @defgroup PrintEASCII
/// @{
/***************************************************************************
    @brief Print a string on a console, using default colors and alignment.

    @param con A console pointer.
    @param x The starting X coordinate, the left-most position being 0.
    @param y The starting Y coordinate, the top-most position being 0.
    @param fmt A format string as if passed to printf.
    @param ... Variadic arguments as if passed to printf.
 */
TCOD_DEPRECATED("Use TCOD_console_printf instead.")
TCODLIB_API void TCOD_console_print(TCOD_Console* con, int x, int y, const char* fmt, ...);
/***************************************************************************
    @brief Print an EASCII string on a console, using default colors.

    @param con A console pointer.
    @param x The starting X coordinate, the left-most position being 0.
    @param y The starting Y coordinate, the top-most position being 0.
    @param flag The blending flag.
    @param alignment The font alignment to use.
    @param fmt A format string as if passed to printf.
    @param ... Variadic arguments as if passed to printf.
 */
TCOD_DEPRECATED("Use TCOD_console_printf_ex instead.")
TCODLIB_API void TCOD_console_print_ex(
    TCOD_Console* con, int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char* fmt, ...);
/***************************************************************************
    @brief Print an EASCII string on a console constrained to a rectangle, using default colors and alignment.

    @param con A console pointer.
    @param x The starting X coordinate, the left-most position being 0.
    @param y The starting Y coordinate, the top-most position being 0.
    @param w The width of the region.
             If 0 then the maximum width will be used.
    @param h The height of the region.
             If 0 then the maximum height will be used.
    @param fmt A format string as if passed to printf.
    @param ... Variadic arguments as if passed to printf.
    @return The number of lines actually printed.
 */
TCOD_DEPRECATED("Use TCOD_console_printf_rect instead.")
TCODLIB_API int TCOD_console_print_rect(TCOD_Console* con, int x, int y, int w, int h, const char* fmt, ...);
/***************************************************************************
    @brief Print an EASCII string on a console constrained to a rectangle, using default colors.

    @param con A console pointer.
    @param x The starting X coordinate, the left-most position being 0.
    @param y The starting Y coordinate, the top-most position being 0.
    @param w The width of the region.
             If 0 then the maximum width will be used.
    @param h The height of the region.
             If 0 then the maximum height will be used.
    @param flag The blending flag.
    @param alignment The font alignment to use.
    @param fmt A format string as if passed to printf.
    @param ... Variadic arguments as if passed to printf.
    @return The number of lines actually printed.
 */
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
/***************************************************************************
    @brief Print a titled, framed region on a console, using default colors and alignment.

    @param con A console pointer.
    @param x The starting X coordinate, the left-most position being 0.
    @param y The starting Y coordinate, the top-most position being 0.
    @param w The width of the frame.
    @param h The height of the frame.
    @param empty If true the characters inside of the frame will be cleared
                 with spaces.
    @param flag The blending flag.
    @param fmt A format string as if passed to printf.
    @param ... Variadic arguments as if passed to printf.

    This function makes assumptions about the fonts character encoding and may draw garbage with some tilesets.

    \rst
    .. deprecated:: 1.19
      This function is not using Unicode frame characters and has been deprecated.
    \endrst
 */
TCOD_DEPRECATED("Use TCOD_console_printf_frame instead.")
TCODLIB_API void TCOD_console_print_frame(
    TCOD_console_t con, int x, int y, int w, int h, bool empty, TCOD_bkgnd_flag_t flag, const char* fmt, ...);
/***************************************************************************
    @brief Return the number of lines that would be printed by an EASCII string.

    @param con A console pointer.
    @param x The starting X coordinate, the left-most position being 0.
    @param y The starting Y coordinate, the top-most position being 0.
    @param w The width of the region.
             If 0 then the maximum width will be used.
    @param h The height of the region.
             If 0 then the maximum height will be used.
    @param fmt A format string as if passed to printf.
    @param ... Variadic arguments as if passed to printf.
    @return The number of lines that would have been printed.
 */
TCOD_DEPRECATED("Use TCOD_console_get_height_rect_fmt instead.")
TCODLIB_API int TCOD_console_get_height_rect(TCOD_Console* con, int x, int y, int w, int h, const char* fmt, ...);
/// @}
#ifndef NO_UNICODE
/// @defgroup PrintWide
/// @{
/***************************************************************************
    \rst
    .. deprecated:: 1.8
      Use :any:`TCOD_console_printf` instead.
    \endrst
 */
TCOD_DEPRECATED("Use TCOD_console_printf instead.")
TCODLIB_API void TCOD_console_print_utf(TCOD_Console* con, int x, int y, const wchar_t* fmt, ...);
/***************************************************************************
    \rst
    .. deprecated:: 1.8
      Use :any:`TCOD_console_printf_ex` instead.
    \endrst
 */
TCOD_DEPRECATED("Use TCOD_console_printf_ex instead.")
TCODLIB_API void TCOD_console_print_ex_utf(
    TCOD_Console* con, int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const wchar_t* fmt, ...);
/***************************************************************************
    \rst
    .. deprecated:: 1.8
      Use :any:`TCOD_console_printf_rect` instead.
    \endrst
 */
TCOD_DEPRECATED("Use TCOD_console_printf_rect instead.")
TCODLIB_API int TCOD_console_print_rect_utf(TCOD_Console* con, int x, int y, int w, int h, const wchar_t* fmt, ...);
/***************************************************************************
    \rst
    .. deprecated:: 1.8
      Use :any:`TCOD_console_printf_rect_ex` instead.
    \endrst
 */
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
/***************************************************************************
    \rst
    .. deprecated:: 1.8
    \endrst
 */
TCOD_DEPRECATED("Use TCOD_console_get_height_rect_fmt instead.")
TCODLIB_API int TCOD_console_get_height_rect_utf(
    TCOD_Console* con, int x, int y, int w, int h, const wchar_t* fmt, ...);
/// @}
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
#ifndef TCOD_NO_UNICODE
/**
    Format and print a UTF-8 string to a console.
    \rst
    .. versionadded:: 1.8

    .. versionchanged:: 1.16
        Now returns a negative error code on failure.
    \endrst
 */
TCOD_DEPRECATED("Use TCOD_printf_rgb instead.")
TCODLIB_API TCODLIB_FORMAT(4, 5) TCOD_Error
    TCOD_console_printf(TCOD_Console* __restrict con, int x, int y, const char* __restrict fmt, ...);
/**
    Format and print a UTF-8 string to a console.
    \rst
    .. versionadded:: 1.8

    .. versionchanged:: 1.16
        Now returns a negative error code on failure.
    \endrst
 */
TCOD_DEPRECATED("Use TCOD_printf_rgb instead.")
TCODLIB_API TCODLIB_FORMAT(6, 7) TCOD_Error TCOD_console_printf_ex(
    TCOD_Console* __restrict con,
    int x,
    int y,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment,
    const char* __restrict fmt,
    ...);
/**
    Format and print a UTF-8 string to a console.
    \rst
    .. versionadded:: 1.8

    .. versionchanged:: 1.16
        Now returns a negative error code on failure.
    \endrst
 */
TCOD_DEPRECATED("Use TCOD_printf_rgb instead.")
TCODLIB_API TCODLIB_FORMAT(6, 7) int TCOD_console_printf_rect(
    TCOD_Console* __restrict con, int x, int y, int w, int h, const char* __restrict fmt, ...);
/**
    Format and print a UTF-8 string to a console.
    \rst
    .. versionadded:: 1.8

    .. versionchanged:: 1.16
        Now returns a negative error code on failure.
    \endrst
 */
TCOD_DEPRECATED("Use TCOD_printf_rgb instead.")
TCODLIB_API TCODLIB_FORMAT(8, 9) int TCOD_console_printf_rect_ex(
    TCOD_Console* __restrict con,
    int x,
    int y,
    int w,
    int h,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment,
    const char* __restrict fmt,
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
TCOD_DEPRECATED("Print function TCOD_printf_rgb and TCOD_console_draw_frame_rgb should be called separately.")
TCODLIB_API TCODLIB_FORMAT(8, 9) TCOD_Error TCOD_console_printf_frame(
    TCOD_Console* __restrict con,
    int x,
    int y,
    int w,
    int h,
    int empty,
    TCOD_bkgnd_flag_t flag,
    const char* __restrict fmt,
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
    TCOD_Console* __restrict con, int x, int y, int w, int h, const char* __restrict fmt, ...);
/**
    @brief Print a string of a specified length to a console.

    @param console A pointer to a TCOD_Console.
    @param x The starting X position, starting from the left-most tile as zero.
    @param y The starting Y position, starting from the upper-most tile as zero.
    @param n The length of the string buffer `str[n]` in bytes.
    @param str The text to print.  This string can contain libtcod color codes.
    @param fg The foreground color.  The printed text is set to this color.
              If NULL then the foreground will be left unchanged, inheriting the previous value of the tile.
    @param bg The background color.  The background tile under the printed text is set to this color.
              If NULL then the background will be left unchanged.
    @param flag The background blending flag.  If unsure then use `TCOD_BKGND_SET`.
    @param alignment The text justification.  This is one of `TCOD_alignment_t` and is normally `TCOD_LEFT`.
    @return TCOD_Error Any problems such as malformed UTF-8 will return a negative error code.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_console_printn(
    TCOD_Console* __restrict console,
    int x,
    int y,
    size_t n,
    const char* __restrict str,
    const TCOD_ColorRGB* __restrict fg,
    const TCOD_ColorRGB* __restrict bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment);
/**
    @brief Print a string of a specified length in a bounding box to a console.

    @param console A pointer to a TCOD_Console.
    @param x The starting X position, starting from the left-most tile as zero.
    @param y The starting Y position, starting from the upper-most tile as zero.
    @param width The maximum width of the bounding region in tiles.
    @param height The maximum height of the bounding region in tiles.
    @param n The length of the string buffer `str[n]` in bytes.
    @param str The text to print.  This string can contain libtcod color codes.
    @param fg The foreground color.  The printed text is set to this color.
              If NULL then the foreground will be left unchanged, inheriting the previous value of the tile.
    @param bg The background color.  The background tile under the printed text is set to this color.
              If NULL then the background will be left unchanged.
    @param flag The background blending flag.  If unsure then use `TCOD_BKGND_SET`.
    @param alignment The text justification.  This is one of `TCOD_alignment_t` and is normally `TCOD_LEFT`.
    @return int The height of the printed text, or a negative error code on failure.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCOD_PUBLIC int TCOD_console_printn_rect(
    TCOD_Console* __restrict console,
    int x,
    int y,
    int width,
    int height,
    size_t n,
    const char* __restrict str,
    const TCOD_ColorRGB* __restrict fg,
    const TCOD_ColorRGB* __restrict bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment);
/**
    @brief Return the height of the word-wrapped text with the given parameters.

    @param console A pointer to a TCOD_Console.
    @param x The starting X position, starting from the left-most tile as zero.
    @param y The starting Y position, starting from the upper-most tile as zero.
    @param width The maximum width of the bounding region in tiles.
    @param height The maximum height of the bounding region in tiles.
    @param n The length of the string buffer `str[n]` in bytes.
    @param str The text to print.  This string can contain libtcod color codes.
    @return int The height of the word-wrapped text as if it were printed, or a negative error code on failure.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCOD_PUBLIC int TCOD_console_get_height_rect_n(
    TCOD_Console* __restrict console, int x, int y, int width, int height, size_t n, const char* __restrict str);
/**
    @brief Return the height of the word-wrapped text with the given width.

    @param width The maximum width of the bounding region in tiles.
    @param n The length of the string buffer `str[n]` in bytes.
    @param str The text to print.  This string can contain libtcod color codes.
    @return int The height of the word-wrapped text as if it were printed, or a negative error code on failure.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCOD_PUBLIC int TCOD_console_get_height_rect_wn(int width, size_t n, const char* __restrict str);
TCOD_DEPRECATED("Print function TCOD_printn_rgb and TCOD_console_draw_frame_rgb should be called separately.")
TCOD_PUBLIC TCOD_Error TCOD_console_printn_frame(
    TCOD_Console* __restrict console,
    int x,
    int y,
    int width,
    int height,
    size_t n,
    const char* __restrict title,
    const TCOD_ColorRGB* __restrict fg,
    const TCOD_ColorRGB* __restrict bg,
    TCOD_bkgnd_flag_t flag,
    bool clear);
/*****************************************************************************
    @brief Print a formatted string using a va_list.

    @param console A pointer to a TCOD_Console.
    @param x The starting X position, starting from the left-most tile as zero.
    @param y The starting Y position, starting from the upper-most tile as zero.
    @param fg The foreground color.  The printed text is set to this color.
              If NULL then the foreground will be left unchanged, inheriting the previous value of the tile.
    @param bg The background color.  The background tile under the printed text is set to this color.
              If NULL then the background will be left unchanged.
    @param flag The background blending flag.  If unsure then use `TCOD_BKGND_SET`.
    @param alignment The text justification.  This is one of `TCOD_alignment_t` and is normally `TCOD_LEFT`.
    @param fmt The format string for a vprintf-like function.
    @param args The arguments for the formatted string.
    @return TCOD_Error Any problems such as malformed UTF-8 will return a negative error code.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCOD_DEPRECATED("Use TCOD_vprintf_rgb instead.")
TCOD_PUBLIC TCOD_Error TCOD_console_vprintf(
    TCOD_Console* __restrict console,
    int x,
    int y,
    const TCOD_color_t* __restrict fg,
    const TCOD_color_t* __restrict bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment,
    const char* __restrict fmt,
    va_list args);
/*****************************************************************************
    @brief Print a formatted string using a va_list within a bounding box.

    @param console A pointer to a TCOD_Console.
    @param x The starting X position, starting from the left-most tile as zero.
    @param y The starting Y position, starting from the upper-most tile as zero.
    @param width The maximum width of the bounding region in tiles.
    @param height The maximum height of the bounding region in tiles.
    @param fg The foreground color.  The printed text is set to this color.
              If NULL then the foreground will be left unchanged, inheriting the previous value of the tile.
    @param bg The background color.  The background tile under the printed text is set to this color.
              If NULL then the background will be left unchanged.
    @param flag The background blending flag.  If unsure then use `TCOD_BKGND_SET`.
    @param alignment The text justification.  This is one of `TCOD_alignment_t` and is normally `TCOD_LEFT`.
    @param fmt The format string for a vprintf-like function.
    @param args The arguments for the formatted string.
    @return TCOD_PUBLIC

    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCOD_DEPRECATED("Use TCOD_vprintf_rgb instead.")
TCOD_PUBLIC int TCOD_console_vprintf_rect(
    TCOD_Console* __restrict console,
    int x,
    int y,
    int width,
    int height,
    const TCOD_color_t* __restrict fg,
    const TCOD_color_t* __restrict bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment,
    const char* fmt,
    va_list args);
/*****************************************************************************
    @brief Information about a string to be printed
 */
typedef struct TCOD_PrintParamsRGB {
  int x;  // The starting X coordinate, the left-most position being 0.
  int y;  // The starting Y coordinate, the top-most position being 0.
  int width;  // Width of the bounding rectangle. Will be unbound if set to 0
  int height;  // Height of the bounding rectangle. Will be unbound if set to 0
  const TCOD_ColorRGB* __restrict fg;  // An optional foreground color of the string
  const TCOD_ColorRGB* __restrict bg;  // An optional background color of the string
  TCOD_bkgnd_flag_t flag;  // The background blending flag. The default of `TCOD_BKGND_NONE` implies `TCOD_BKGND_SET`.
  TCOD_alignment_t alignment;  // The text justification.  Defaults to `TCOD_LEFT`.
} TCOD_PrintParamsRGB;
/*****************************************************************************
    @brief Prints a formatted string to the console.

    @param console A pointer to a TCOD_Console.
    @param params Information about how the string should be printed
    @param fmt The format string for a vprintf-like function.
    @param args The arguments for the formatted string.
    @return An error code if less than 0
    \rst
    .. versionadded:: 1.23
    \endrst
 */
TCOD_PUBLIC TCODLIB_FORMAT(3, 4) int TCOD_printf_rgb(
    TCOD_Console* __restrict console, TCOD_PrintParamsRGB params, const char* __restrict fmt, ...);
/*****************************************************************************
    @brief Prints n-bytes of a string string to the console.

    @param console A pointer to a TCOD_Console.
    @param params Information about how the string should be printed
    @param str The string to be read from.
    @param n Length of string in bytes
    @return An error code if less than 0
    \rst
    .. versionadded:: 1.23
    \endrst
 */
TCOD_PUBLIC int TCOD_printn_rgb(
    TCOD_Console* __restrict console, TCOD_PrintParamsRGB params, int n, const char* __restrict str);
/*****************************************************************************
    @brief Prints a formatted string using va_list

    @param console A pointer to a TCOD_Console.
    @param params Information about how the string should be printed
    @param fmt The format string for a vprintf-like function
    @param args The arguments for the format string
    @return An error code if less than 0
    \rst
    .. versionadded:: 1.23
    \endrst
 */
TCOD_PUBLIC int TCOD_vprintf_rgb(
    TCOD_Console* __restrict console, TCOD_PrintParamsRGB params, const char* __restrict fmt, va_list args);
#endif  // TCOD_NO_UNICODE
#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
#endif /* TCOD_CONSOLE_PRINTING_H_ */
