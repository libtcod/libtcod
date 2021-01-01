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
#ifndef TCOD_CONSOLE_H_
#define TCOD_CONSOLE_H_

#include <stdbool.h>
#ifdef __cplusplus
#include <array>
#include <memory>
#include <stdexcept>
#endif  // __cplusplus

#include "color.h"
#include "config.h"
#include "error.h"
#include "tileset.h"
/**
 *  \enum TCOD_bkgnd_flag_t
 *
 *  Background color blend modes.
 */
typedef enum {
  TCOD_BKGND_NONE,
  TCOD_BKGND_SET,
  TCOD_BKGND_MULTIPLY,
  TCOD_BKGND_LIGHTEN,
  TCOD_BKGND_DARKEN,
  TCOD_BKGND_SCREEN,
  TCOD_BKGND_COLOR_DODGE,
  TCOD_BKGND_COLOR_BURN,
  TCOD_BKGND_ADD,
  TCOD_BKGND_ADDA,
  TCOD_BKGND_BURN,
  TCOD_BKGND_OVERLAY,
  TCOD_BKGND_ALPH,
  TCOD_BKGND_DEFAULT
} TCOD_bkgnd_flag_t;
/**
 *  \enum TCOD_alignment_t
 *
 *  Print justification options.
 */
typedef enum { TCOD_LEFT, TCOD_RIGHT, TCOD_CENTER } TCOD_alignment_t;
/**
    A console tile.
 */
typedef struct TCOD_ConsoleTile {
#ifdef __cplusplus
  bool operator==(const TCOD_ConsoleTile& rhs) const noexcept { return ch == rhs.ch && fg == rhs.fg && bg == rhs.bg; }
  bool operator!=(const TCOD_ConsoleTile& rhs) const noexcept { return !(*this == rhs); }
#endif  // __cplusplus
  /**
      The Unicode codepoint for this tile.
   */
  int ch;
  /**
      The tile glyph color, rendered on top of the background.
   */
  TCOD_ColorRGBA fg;
  /**
      The tile background color, rendered behind the glyph.
   */
  TCOD_ColorRGBA bg;
} TCOD_ConsoleTile;
/**
    The libtcod console struct.

    All attributes should be considered private.

    All C++ methods should be considered provisional, and are subject to
    change.
 */
struct TCOD_Console {
#ifdef __cplusplus
  auto begin() noexcept -> TCOD_ConsoleTile* { return tiles; }
  auto begin() const noexcept -> const TCOD_ConsoleTile* { return tiles; }
  auto end() noexcept -> TCOD_ConsoleTile* { return tiles + size(); }
  auto end() const noexcept -> const TCOD_ConsoleTile* { return tiles + size(); }
  /**
      Return a reference to the tile at `xy`.
   */
  auto operator[](const std::array<int, 2>& xy) noexcept -> TCOD_ConsoleTile& { return tiles[w * xy[1] + xy[0]]; }
  /**
      Return a constant reference to the tile at `xy`.
   */
  auto operator[](const std::array<int, 2>& xy) const noexcept -> const TCOD_ConsoleTile& {
    return tiles[w * xy[1] + xy[0]];
  }
  /**
      Return a reference to the tile at `x`,`y`.

      Throws if the index is out-of-bounds.
   */
  auto at(int x, int y) -> TCOD_ConsoleTile& {
    range_check_(x, y);
    return (*this)[{x, y}];
  }
  /**
      Return a constant reference to the tile at `x`,`y`.

      Throws if the index is out-of-bounds.
   */
  auto at(int x, int y) const -> const TCOD_ConsoleTile& {
    range_check_(x, y);
    return (*this)[{x, y}];
  }
  /**
      Return the total number of tiles in this console.
   */
  int size() const { return elements; }
  /**
      Internal function.  Throws `std::out_of_range` if `x` or `y` is out-of-bounds.
   */
  void range_check_(int x, int y) const {
    if (!in_bounds(x, y)) {
      throw std::out_of_range(
          std::string("Out of bounds lookup {x=") + std::to_string(x) + ", y=" + std::to_string(y) +
          "} on console of shape {" + std::to_string(w) + ", " + std::to_string(h) + "}.");
    }
  }
  /**
      Return true if `x`,`y` are within the bounds of this console.
   */
  bool in_bounds(int x, int y) const noexcept { return 0 <= x && x < w && 0 <= y && y < h; }
#endif  // __cplusplus
  /** Console width and height (in characters, not pixels.) */
  int w, h;
  /** A contiguous array of console tiles. */
  TCOD_ConsoleTile* __restrict tiles;
  /** Default background operator for print & print_rect functions. */
  TCOD_bkgnd_flag_t bkgnd_flag;
  /** Default alignment for print & print_rect functions. */
  TCOD_alignment_t alignment;
  /** Foreground (text) and background colors. */
  TCOD_color_t fore, back;
  /** True if a key color is being used. */
  bool has_key_color;
  /** The current key color for this console. */
  TCOD_color_t key_color;
  /**
      The total length of the tiles array.  Same as `w * h`.

      \rst
      .. versionadded:: 1.16
      \endrst
   */
  int elements;
  /**
      \rst
      .. versionadded:: 1.16
      \endrst
   */
  void* userdata;
  /** Internal use. */
  void (*on_delete)(struct TCOD_Console* self);
};
typedef struct TCOD_Console TCOD_Console;
typedef struct TCOD_Console* TCOD_console_t;
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
/**
 *  Return a new console with a specific number of columns and rows.
 *
 *  \param w Number of columns.
 *  \param h Number of columns.
 *  \return A pointer to the new console, or NULL on error.
 */
TCOD_PUBLIC TCOD_NODISCARD TCOD_Console* TCOD_console_new(int w, int h);
/**
 *  Return the width of a console.
 */
TCOD_PUBLIC TCOD_NODISCARD int TCOD_console_get_width(const TCOD_Console* con);
/**
 *  Return the height of a console.
 */
TCOD_PUBLIC TCOD_NODISCARD int TCOD_console_get_height(const TCOD_Console* con);
TCOD_PUBLIC void TCOD_console_set_key_color(TCOD_Console* con, TCOD_color_t col);
/**
 *  Blit from one console to another.
 *
 *  \param srcCon Pointer to the source console.
 *  \param xSrc The left region of the source console to blit from.
 *  \param ySrc The top region of the source console to blit from.
 *  \param wSrc The width of the region to blit from.
 *              If 0 then it will fill to the maximum width.
 *  \param hSrc The height of the region to blit from.
 *              If 0 then it will fill to the maximum height.
 *  \param dstCon Pointer to the destination console.
 *  \param xDst The left corner to blit onto the destination console.
 *  \param yDst The top corner to blit onto the destination console.
 *  \param foreground_alpha Foreground blending alpha.
 *  \param background_alpha Background blending alpha.
 *
 *  If the source console has a key color, this function will use it.
 *  \rst
 *  .. versionchanged:: 1.16
 *      Blits can now handle per-cell alpha transparency.
 *  \endrst
 */
TCOD_PUBLIC void TCOD_console_blit(
    const TCOD_Console* __restrict src,
    int xSrc,
    int ySrc,
    int wSrc,
    int hSrc,
    TCOD_Console* __restrict dst,
    int xDst,
    int yDst,
    float foreground_alpha,
    float background_alpha);
TCOD_PUBLIC void TCOD_console_blit_key_color(
    const TCOD_Console* __restrict src,
    int xSrc,
    int ySrc,
    int wSrc,
    int hSrc,
    TCOD_Console* __restrict dst,
    int xDst,
    int yDst,
    float foreground_alpha,
    float background_alpha,
    const TCOD_color_t* key_color);
/**
 *  Delete a console.
 *
 *  \param con A console pointer.
 *
 *  If the console being deleted is the root console, then the display will be
 *  uninitialized.
 */
TCOD_PUBLIC void TCOD_console_delete(TCOD_Console* console);

TCOD_PUBLIC void TCOD_console_set_default_background(TCOD_Console* con, TCOD_color_t col);
TCOD_PUBLIC void TCOD_console_set_default_foreground(TCOD_Console* con, TCOD_color_t col);
/**
 *  Clear a console to its default colors and the space character code.
 */
TCOD_PUBLIC void TCOD_console_clear(TCOD_Console* con);
/**
 *  Blend a background color onto a console tile.
 *
 *  \param con A console pointer.
 *  \param x The X coordinate, the left-most position being 0.
 *  \param y The Y coordinate, the top-most position being 0.
 *  \param col The background color to blend.
 *  \param flag The blend mode to use.
 */
TCOD_PUBLIC void TCOD_console_set_char_background(
    TCOD_Console* con, int x, int y, TCOD_color_t col, TCOD_bkgnd_flag_t flag);
/**
 *  Change the foreground color of a console tile.
 *
 *  \param con A console pointer.
 *  \param x The X coordinate, the left-most position being 0.
 *  \param y The Y coordinate, the top-most position being 0.
 *  \param col The foreground color to set.
 */
TCOD_PUBLIC void TCOD_console_set_char_foreground(TCOD_Console* con, int x, int y, TCOD_color_t col);
/**
 *  Change a character on a console tile, without changing its colors.
 *
 *  \param con A console pointer.
 *  \param x The X coordinate, the left-most position being 0.
 *  \param y The Y coordinate, the top-most position being 0.
 *  \param c The character code to set.
 */
TCOD_PUBLIC void TCOD_console_set_char(TCOD_Console* con, int x, int y, int c);
/**
 *  Draw a character on a console using the default colors.
 *
 *  \param con A console pointer.
 *  \param x The X coordinate, the left-most position being 0.
 *  \param y The Y coordinate, the top-most position being 0.
 *  \param c The character code to place.
 *  \param flag A TCOD_bkgnd_flag_t flag.
 */
TCOD_PUBLIC void TCOD_console_put_char(TCOD_Console* con, int x, int y, int c, TCOD_bkgnd_flag_t flag);
/**
 *  Draw a character on the console with the given colors.
 *
 *  \param con A console pointer.
 *  \param x The X coordinate, the left-most position being 0.
 *  \param y The Y coordinate, the top-most position being 0.
 *  \param c The character code to place.
 *  \param fore The foreground color.
 *  \param back The background color.  This color will not be blended.
 */
TCOD_PUBLIC void TCOD_console_put_char_ex(TCOD_Console* con, int x, int y, int c, TCOD_color_t fore, TCOD_color_t back);
/**
 *  Set a consoles default background flag.
 *
 *  \param con A console pointer.
 *  \param flag One of `TCOD_bkgnd_flag_t`.
 */
TCOD_PUBLIC void TCOD_console_set_background_flag(TCOD_Console* con, TCOD_bkgnd_flag_t flag);
/**
 *  Return a consoles default background flag.
 */
TCOD_PUBLIC TCOD_NODISCARD TCOD_bkgnd_flag_t TCOD_console_get_background_flag(TCOD_Console* con);
/**
 *  Set a consoles default alignment.
 *
 *  \param con A console pointer.
 *  \param alignment One of TCOD_alignment_t
 */
TCOD_PUBLIC void TCOD_console_set_alignment(TCOD_Console* con, TCOD_alignment_t alignment);
/**
 *  Return a consoles default alignment.
 */
TCOD_PUBLIC TCOD_NODISCARD TCOD_alignment_t TCOD_console_get_alignment(TCOD_Console* con);

TCOD_PUBLIC TCOD_NODISCARD TCOD_color_t TCOD_console_get_default_background(TCOD_Console* con);
TCOD_PUBLIC TCOD_NODISCARD TCOD_color_t TCOD_console_get_default_foreground(TCOD_Console* con);
/**
 *  Return the background color of a console at x,y
 *
 *  \param con A console pointer.
 *  \param x The X coordinate, the left-most position being 0.
 *  \param y The Y coordinate, the top-most position being 0.
 *  \return A TCOD_color_t struct with a copy of the background color.
 */
TCOD_PUBLIC TCOD_NODISCARD TCOD_color_t TCOD_console_get_char_background(const TCOD_Console* con, int x, int y);
/**
 *  Return the foreground color of a console at x,y
 *
 *  \param con A console pointer.
 *  \param x The X coordinate, the left-most position being 0.
 *  \param y The Y coordinate, the top-most position being 0.
 *  \return A TCOD_color_t struct with a copy of the foreground color.
 */
TCOD_PUBLIC TCOD_NODISCARD TCOD_color_t TCOD_console_get_char_foreground(const TCOD_Console* con, int x, int y);
/**
 *  Return a character code of a console at x,y
 *
 *  \param con A console pointer.
 *  \param x The X coordinate, the left-most position being 0.
 *  \param y The Y coordinate, the top-most position being 0.
 *  \return The character code.
 */
TCOD_PUBLIC TCOD_NODISCARD int TCOD_console_get_char(const TCOD_Console* con, int x, int y);
/**
 *  Fade the color of the display.
 *
 *  \param val Where at 255 colors are normal and at 0 colors are completely
 *             faded.
 *  \param fadecol Color to fade towards.
 */
TCOD_PUBLIC
void TCOD_console_set_fade(uint8_t val, TCOD_color_t fade);
/**
 *  Return the fade value.
 *
 *  \return At 255 colors are normal and at 0 colors are completely faded.
 */
TCOD_PUBLIC TCOD_NODISCARD uint8_t TCOD_console_get_fade(void);
/**
 *  Return the fade color.
 *
 *  \return The current fading color.
 */
TCOD_PUBLIC TCOD_NODISCARD TCOD_color_t TCOD_console_get_fading_color(void);
void TCOD_console_resize_(TCOD_Console* console, int width, int height);
#ifdef __cplusplus
}  // extern "C"
namespace tcod {
struct ConsoleDeleter {
  void operator()(TCOD_Console* console) const { TCOD_console_delete(console); }
};
typedef std::unique_ptr<struct TCOD_Console, ConsoleDeleter> ConsolePtr;
TCOD_NODISCARD
inline auto new_console(int width, int height) -> ConsolePtr {
  ConsolePtr console{TCOD_console_new(width, height)};
  if (!console) {
    throw std::runtime_error(TCOD_get_error());
  }
  return console;
}
}  // namespace tcod
#endif  // __cplusplus
#endif  // TCOD_CONSOLE_H_
