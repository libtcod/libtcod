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
#ifndef TCOD_CONSOLE_DRAWING_H_
#define TCOD_CONSOLE_DRAWING_H_
#ifdef __cplusplus
#include <array>
#include <optional>
#endif  // __cplusplus
#include "config.h"
#include "console_types.h"
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
/**
 *  Draw a rectangle onto a console.
 *
 *  \param con A console pointer.
 *  \param x The starting region, the left-most position being 0.
 *  \param y The starting region, the top-most position being 0.
 *  \param rw The width of the rectangle.
 *  \param rh The height of the rectangle.
 *  \param clear If true the drawing region will be filled with spaces.
 *  \param flag The blending flag to use.
 */
TCOD_DEPRECATED("Use TCOD_console_draw_rect_rgb instead.")
TCOD_PUBLIC void TCOD_console_rect(TCOD_Console* con, int x, int y, int rw, int rh, bool clear, TCOD_bkgnd_flag_t flag);
/**
 *  Draw a horizontal line using the default colors.
 *
 *  \param con A console pointer.
 *  \param x The starting X coordinate, the left-most position being 0.
 *  \param y The starting Y coordinate, the top-most position being 0.
 *  \param l The width of the line.
 *  \param flag The blending flag.
 *
 *  This function makes assumptions about the fonts character encoding.
 *  It will fail if the font encoding is not `cp437`.
 */
TCOD_DEPRECATED("Use TCOD_console_draw_rect_rgb instead.")
TCOD_PUBLIC void TCOD_console_hline(TCOD_Console* con, int x, int y, int l, TCOD_bkgnd_flag_t flag);
/**
 *  Draw a vertical line using the default colors.
 *
 *  \param con A console pointer.
 *  \param x The starting X coordinate, the left-most position being 0.
 *  \param y The starting Y coordinate, the top-most position being 0.
 *  \param l The height of the line.
 *  \param flag The blending flag.
 *
 *  This function makes assumptions about the fonts character encoding.
 *  It will fail if the font encoding is not `cp437`.
 */
TCOD_DEPRECATED("Use TCOD_console_draw_rect_rgb instead.")
TCOD_PUBLIC void TCOD_console_vline(TCOD_Console* con, int x, int y, int l, TCOD_bkgnd_flag_t flag);
// Next functions are provisional unless given an added version.
/**
 *  Place a single tile on a `console` at `x`,`y`.
 *
 *  If `ch` is 0 then the character code will not be updated.
 *
 *  If `fg`,`bg` is NULL then their respective colors will not be updated.
 */
TCOD_PUBLIC void TCOD_console_put_rgb(
    TCOD_Console* __restrict console,
    int x,
    int y,
    int ch,
    const TCOD_color_t* fg,
    const TCOD_color_t* bg,
    TCOD_bkgnd_flag_t flag);
/**
 *  Draw a rectangle on a `console` with a shape of `x`,`y`,`width`,`height`.
 *
 *  If `ch` is 0 then the character code will not be updated.
 *
 *  If `fg`,`bg` is NULL then their respective colors will not be updated.
 */
TCOD_PUBLIC TCOD_Error TCOD_console_draw_rect_rgb(
    TCOD_Console* __restrict console,
    int x,
    int y,
    int width,
    int height,
    int ch,
    const TCOD_color_t* fg,
    const TCOD_color_t* bg,
    TCOD_bkgnd_flag_t flag);
/**
    Draw a decorated frame onto `console` with the shape of `x`, `y`, `width`, `height`.

    `decoration[9]` is an optional array of Unicode codepoints.
    If left as NULL then a single-pipe decoration is used by default.

    If `decoration[9]` is given the codepoints are used for the edges, corners, and fill of the frame in this order:

        0 1 2
        3 4 5
        6 7 8

    If `fg` or `bg` is NULL then their respective colors will not be updated.

    If `clear` is true then the inner area of the frame is filled with the inner decoration, which is typically space.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_console_draw_frame_rgb(
    struct TCOD_Console* __restrict con,
    int x,
    int y,
    int width,
    int height,
    const int* __restrict decoration,
    const TCOD_ColorRGB* __restrict fg,
    const TCOD_ColorRGB* __restrict bg,
    TCOD_bkgnd_flag_t flag,
    bool clear);
#ifdef __cplusplus
}  // extern "C"
namespace tcod {
/***************************************************************************
    @brief Fill a region with the given graphic.

    @param console A reference to a TCOD_Console.
    @param rect An `{x, y, width, height}` rectangle, starting from the upper-left-most tile as zero.
    @param ch The character to draw.  If zero then the characters in the drawing region will not be changed.
    @param fg The foreground color.  The printed text is set to this color.
              If std::nullopt then the foreground will be left unchanged, inheriting the previous value of the tile.
    @param bg The background color.  The background tile under the printed text is set to this color.
              If std::nullopt then the background will be left unchanged.
    @param flag The background blending flag.

    @code{.cpp}
      auto console = tcod::Console{80, 50};
      // Draw a red background without replacing any foreground glyphs/colors.
      tcod::draw_rect(console, {2, 2, 24, 24}, 0, std::nullopt, tcod::ColorRGB{255, 0, 0});
      // Draw a horizontal bar.
      tcod::draw_rect(console, {8, 8, 16, 1}, '-', {{255, 255, 255}}, std::nullopt);
    @endcode
    \rst
    .. versionadded:: 1.19
    \endrst
 */
inline void draw_rect(
    TCOD_Console& console,
    const std::array<int, 4>& rect,
    int ch,
    std::optional<TCOD_ColorRGB> fg,
    std::optional<TCOD_ColorRGB> bg,
    TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET) {
  const TCOD_ColorRGB* fg_ptr = fg ? &fg.value() : nullptr;
  const TCOD_ColorRGB* bg_ptr = bg ? &bg.value() : nullptr;
  tcod::check_throw_error(
      TCOD_console_draw_rect_rgb(&console, rect.at(0), rect.at(1), rect.at(2), rect.at(3), ch, fg_ptr, bg_ptr, flag));
}
/***************************************************************************
    @brief Draw a decorative frame.

    @param console A reference to a TCOD_Console.
    @param rect An `{x, y, width, height}` rectangle, starting from the upper-left-most tile as zero.
    @param decoration The codepoints to use for the frame in row-major order.
    @param fg The foreground color.  The printed text is set to this color.
              If std::nullopt then the foreground will be left unchanged, inheriting the previous value of the tile.
    @param bg The background color.  The background tile under the printed text is set to this color.
              If std::nullopt then the background will be left unchanged.
    @param flag The background blending flag.
    @param clear If true then the center area will be cleared with the center decoration.

    `decoration` is given the codepoints to be used for the edges, corners, and fill of the frame in this order:

        0 1 2
        3 4 5
        6 7 8

    @code{.cpp}
      auto console = tcod::Console{80, 50};
      static constexpr std::array<int, 9> LEGEND = {'0', '1', '2', '3', '4', '5', '6', '7', '8'};
      tcod::draw_frame(console, {0, 0, 3, 3}, LEGEND, {{255, 255, 255}}, {{0, 0, 0}});
    @endcode
    \rst
    .. versionadded:: 1.19
    \endrst
 */
inline void draw_frame(
    TCOD_Console& console,
    const std::array<int, 4>& rect,
    const std::array<int, 9>& decoration,
    std::optional<TCOD_ColorRGB> fg,
    std::optional<TCOD_ColorRGB> bg,
    TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET,
    bool clear = true) {
  const TCOD_ColorRGB* fg_ptr = fg ? &fg.value() : nullptr;
  const TCOD_ColorRGB* bg_ptr = bg ? &bg.value() : nullptr;
  tcod::check_throw_error(TCOD_console_draw_frame_rgb(
      &console, rect.at(0), rect.at(1), rect.at(2), rect.at(3), decoration.data(), fg_ptr, bg_ptr, flag, clear));
}
}  // namespace tcod
#endif  // __cplusplus
#endif  // TCOD_CONSOLE_DRAWING_H_
