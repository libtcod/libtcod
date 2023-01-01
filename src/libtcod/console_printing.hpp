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
#ifndef TCOD_CONSOLE_PRINTING_HPP_
#define TCOD_CONSOLE_PRINTING_HPP_

#include <stdio.h>

#include <array>
#include <cstdarg>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

#include "console_printing.h"

namespace tcod {
#ifndef TCOD_NO_UNICODE
/*****************************************************************************
    @brief Print a string to a console.

    @param console A reference to a TCOD_Console.
    @param xy The starting `{x, y}` position, starting from the upper-left-most tile as zero.
    @param str The text to print.  This string can contain libtcod color codes.
    @param fg The foreground color.  The printed text is set to this color.
              If std::nullopt then the foreground will be left unchanged, inheriting the previous value of the tile.
    @param bg The background color.  The background tile under the printed text is set to this color.
              If std::nullopt then the background will be left unchanged.
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
/*****************************************************************************
    @brief Print a string to a console constrained to a bounding box.

    @param console A reference to a TCOD_Console.
    @param rect An `{x, y, width, height}` rectangle, starting from the upper-left-most tile as zero.
                A width or height of zero will leave that axis unconstrained.
    @param str The text to print.  This string can contain libtcod color codes.
    @param fg The foreground color.  The printed text is set to this color.
              If std::nullopt then the foreground will be left unchanged, inheriting the previous value of the tile.
    @param bg The background color.  The background tile under the printed text is set to this color.
              If std::nullopt then the background will be left unchanged.
    @param alignment The text justification.
    @param flag The background blending flag.
    @return int The height of the printed output.

    @code{.cpp}
      auto console = tcod::Console{80, 50};
      static constexpr auto TEAL = tcod::ColorRGB{0, 255, 255};
      // Print "Hello World" centered along the top row, ignoring the background color.
      tcod::print(console, {0, 0, console->w, 1}, "Hello World", TEAL, std::nullopt, TCOD_CENTER);
    @endcode

    \rst
    .. versionadded:: 1.19
    \endrst
 */
inline int print_rect(
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
      tcod::print(console, {0, y, width, 0}, "Long text example", std::nullopt, std::nullopt);
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
#endif  // TCOD_NO_UNICODE
/*****************************************************************************
    @brief Return a formatted string as a std::string object.

    This is a convience function for code using printf-like formatted strings.
    Newer more modern code might want to use [the fmt library](https://fmt.dev/latest/index.html) instead.

    @tparam T Parameter packed arguments.
    @param format A printf-like format string.
    @param args Any printf-like arguments.
    @return A std::string object with the resulting output.

    @details
    [fmt::sprintf](https://fmt.dev/latest/api.html#_CPPv4I0Dp0EN3fmt7sprintfENSt12basic_stringI4CharEERK1SDpRK1T)
    is a faster and safer alternative to this function.

    @code{.cpp}
      auto console = tcod::Console{80, 50};
      // Use tcod::stringf to encapsulate printf-like parameters.
      tcod::print(console, {0, 0}, tcod::stringf("%s %s", "Hello", "World"), nullptr, nullptr);
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
#endif  // TCOD_CONSOLE_PRINTING_HPP_
