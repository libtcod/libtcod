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
#include <stdio.h>

#include <array>
#include <cstdarg>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#endif

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
    const char* str,
    const TCOD_ColorRGB* fg,
    const TCOD_ColorRGB* bg,
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
    const char* str,
    const TCOD_ColorRGB* fg,
    const TCOD_ColorRGB* bg,
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
TCOD_PUBLIC int TCOD_console_get_height_rect_wn(int width, size_t n, const char* str);
// Depreacted function.
TCOD_PUBLIC TCOD_Error TCOD_console_printn_frame(
    TCOD_Console* __restrict console,
    int x,
    int y,
    int width,
    int height,
    size_t n,
    const char* title,
    const TCOD_ColorRGB* fg,
    const TCOD_ColorRGB* bg,
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
    @param fmt The format string for a vprintf-life function.
    @param args The arguments for the formatted string.
    @return TCOD_Error Any problems such as malformed UTF-8 will return a negative error code.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_console_vprintf(
    TCOD_Console* __restrict console,
    int x,
    int y,
    const TCOD_color_t* __restrict fg,
    const TCOD_color_t* __restrict bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment,
    const char* fmt,
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
    @param fmt The format string for a vprintf-life function.
    @param args The arguments for the formatted string.
    @return TCOD_PUBLIC

    \rst
    .. versionadded:: 1.19
    \endrst
 */
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
#ifdef __cplusplus
}  // extern "C"
namespace tcod {
/*****************************************************************************
    @brief Print a string to a console.

    @param console A reference to a TCOD_Console.
    @param xy The starting `{x, y}` position, starting from the upper-left-most tile as zero.
    @param str The text to print.  This string can contain libtcod color codes.
    @param fg The foreground color.  The printed text is set to this color.
              If `{}` then the foreground will be left unchanged, inheriting the previous value of the tile.
    @param bg The background color.  The background tile under the printed text is set to this color.
              If `{}` then the background will be left unchanged.
    @param alignment The text justification.
    @param flag The background blending flag.

    @code{.cpp}
      auto console = tcod::Console{80, 50};
      tcod::print(console, {0, 0}, "Hello World", {{255, 255, 255}}, {{0, 0, 0}});
    @endcode

    \rst
    .. versionadded:: 1.19
    \endrst
 */
inline void print(
    TCOD_Console& console,
    const std::array<int, 2>& xy,
    std::string_view str,
    std::optional<TCOD_ColorRGB> fg,
    std::optional<TCOD_ColorRGB> bg,
    TCOD_alignment_t alignment = TCOD_LEFT,
    TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET) {
  const TCOD_ColorRGB* fg_ptr = fg ? &fg.value() : nullptr;
  const TCOD_ColorRGB* bg_ptr = bg ? &bg.value() : nullptr;
  check_throw_error(
      TCOD_console_printn(&console, xy.at(0), xy.at(1), str.size(), str.data(), fg_ptr, bg_ptr, flag, alignment));
}
[[deprecated]] inline void print(
    TCOD_Console& console,
    const std::array<int, 2>& xy,
    std::string_view str,
    const TCOD_ColorRGB* fg,
    const TCOD_ColorRGB* bg,
    TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET,
    TCOD_alignment_t alignment = TCOD_LEFT) {
  check_throw_error(TCOD_console_printn(&console, xy.at(0), xy.at(1), str.size(), str.data(), fg, bg, flag, alignment));
}
/*****************************************************************************
    @brief Print a string to a console contrained to a bounding box.

    @param console A reference to a TCOD_Console.
    @param rect An `{x, y, width, height}` rectangle, starting from the upper-left-most tile as zero.
                A width or height of zero will leave that axis unconstrained.
    @param str The text to print.  This string can contain libtcod color codes.
    @param fg The foreground color.  The printed text is set to this color.
              If `{}` then the foreground will be left unchanged, inheriting the previous value of the tile.
    @param bg The background color.  The background tile under the printed text is set to this color.
              If `{}` then the background will be left unchanged.
    @param flag The background blending flag.
    @param alignment The text justification.
    @return int The height of the printed output.

    @code{.cpp}
      auto console = tcod::Console{80, 50};
      static constexpr auto TEAL = TCOD_ColorRGB{0, 255, 255};
      // Print "Hello World" centered along the top row, ignoring the background color.
      tcod::print(console, {0, 0, console->w, 1}, "Hello World", TEAL, {}, TCOD_CENTER);
    @endcode

    \rst
    .. versionadded:: 1.19
    \endrst
 */
inline int print(
    TCOD_Console& console,
    const std::array<int, 4>& rect,
    std::string_view str,
    std::optional<TCOD_ColorRGB> fg,
    std::optional<TCOD_ColorRGB> bg,
    TCOD_alignment_t alignment = TCOD_LEFT,
    TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET) {
  return check_throw_error(TCOD_console_printn_rect(
      &console,
      rect.at(0),
      rect.at(1),
      rect.at(2),
      rect.at(3),
      str.size(),
      str.data(),
      fg ? &fg.value() : nullptr,
      bg ? &bg.value() : nullptr,
      flag,
      alignment));
}
[[deprecated]] inline int print_rect(
    TCOD_Console& console,
    const std::array<int, 4>& rect,
    std::string_view str,
    const TCOD_ColorRGB* fg,
    const TCOD_ColorRGB* bg,
    TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET,
    TCOD_alignment_t alignment = TCOD_LEFT) {
  return check_throw_error(TCOD_console_printn_rect(
      &console, rect.at(0), rect.at(1), rect.at(2), rect.at(3), str.size(), str.data(), fg, bg, flag, alignment));
}
[[deprecated]] inline int get_height_rect(TCOD_Console& console, const std::array<int, 4>& rect, std::string_view str) {
  return check_throw_error(
      TCOD_console_get_height_rect_n(&console, rect.at(0), rect.at(1), rect.at(2), rect.at(3), str.size(), str.data()));
}
/*****************************************************************************
    @brief Return the height of the word-wrapped text with the given width.

    @param width The maximum width of the bounding region in tiles.
    @param str The text to print.  This string can contain libtcod color codes.
    @return int The height of the text as if it were printed.

    @code{.cpp}
      auto console = tcod::Console{80, 50};
      int y = console->h; // Start Y at the bottom of this console.
      const int width = 6;
      y -= tcod::get_height_rect("Long text example", width); // Move y up by the height of this text.
      tcod::print(console, {0, y, width, 0}, "Long text example", {}, {});
    @endcode

    \rst
    .. versionadded:: 1.19
    \endrst
 */
inline int get_height_rect(int width, std::string_view str) {
  return check_throw_error(TCOD_console_get_height_rect_wn(width, str.size(), str.data()));
}
[[deprecated("It is recommended that you print your own banners for frames.")]] inline void print_frame(
    struct TCOD_Console& console,
    const std::array<int, 4>& rect,
    std::string_view title,
    const TCOD_ColorRGB* fg,
    const TCOD_ColorRGB* bg,
    TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET,
    bool clear = true) {
  check_throw_error(TCOD_console_printn_frame(
      &console, rect.at(0), rect.at(1), rect.at(2), rect.at(3), title.size(), title.data(), fg, bg, flag, clear));
}
/*****************************************************************************
    @brief Return a formatted string as a std::string object.

    This is a convience function for code using printf-like formatted strings.
    Newer more modern code might want to use the fmt library instead.

    @tparam T Parameter packed arguments.
    @param format A printf-like format string.
    @param args Any printf-like arguments.
    @return A std::string object with the resulting output.

    @code{.cpp}
      auto console = tcod::new_console({{80, 50}});
      // Use tcod::stringf to encapsulate printf-like parameters.
      tcod::print(*console, {0, 0}, tcod::stringf("%s %s", "Hello", "World"), nullptr, nullptr);
    @endcode

    \rst
    .. versionadded:: 1.19
    \endrst
 */
template <typename... T>
inline std::string stringf(const char* format, T... args) {
  const int str_length = snprintf(nullptr, 0, format, args...);
  if (str_length < 0) throw std::runtime_error("Failed to format string.");
  std::string out(str_length, '\0');
  snprintf(&out[0], str_length + 1, format, args...);
  return out;
}
}  // namespace tcod
#endif  // __cplusplus
#endif /* TCOD_CONSOLE_PRINTING_H_ */
