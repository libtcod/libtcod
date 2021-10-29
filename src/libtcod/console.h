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
#include <algorithm>
#include <array>
#include <memory>
#include <stdexcept>
#include <utility>
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
/***************************************************************************
    @brief The raw data for a single TCOD_Console tile.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
typedef struct TCOD_ConsoleTile {
#ifdef __cplusplus
  bool operator==(const TCOD_ConsoleTile& rhs) const noexcept { return ch == rhs.ch && fg == rhs.fg && bg == rhs.bg; }
  bool operator!=(const TCOD_ConsoleTile& rhs) const noexcept { return !(*this == rhs); }
#endif  // __cplusplus
  /***************************************************************************
      @brief The Unicode codepoint for this tile.
   */
  int ch;
  /***************************************************************************
      @brief The tile glyph color, rendered on top of the background.
   */
  TCOD_ColorRGBA fg;
  /***************************************************************************
      @brief The tile background color, rendered behind the glyph.
   */
  TCOD_ColorRGBA bg;
} TCOD_ConsoleTile;
/***************************************************************************
    @brief A libtcod console containing a grid of tiles with `{ch, fg, bg}` information.

    @details In C++ this struct has several convience methods to make working with consoles easier.
    Note that all tile references are to TCOD_ConsoleTile structs and will include an alpha channel.

    @code{.cpp}
      tcod::ConsolePtr console = tcod::new_console({{80, 50}});
      console->at({1, 1}).ch = '@';  // Bounds-checked references to a tile.
      (*console)[{1, 1}].bg = {0, 0, 255, 255};  // Access a tile without bounds checking, colors are RGBA.
      if (console->in_bounds({100, 100})) {}  // Test if an index is in bounds.
      for (auto& tile : *console) tile.fg = {255, 255, 0, 255};  // Iterate over all tiles on a console.
      for (auto& tile : *console) tile = {0x20, {255, 255, 255, 255}, {0, 0, 0, 255}};  // Clear all tiles.
    @endcode

    \rst
    .. versionadded:: 1.19
    \endrst
 */
struct TCOD_Console {
#ifdef __cplusplus
  /***************************************************************************
      @brief Return a pointer to the beginning of this consoles tile data.
   */
  [[nodiscard]] auto begin() noexcept -> TCOD_ConsoleTile* { return tiles; }
  /***************************************************************************
      @brief Return a const pointer to the beginning of this consoles tile data.
   */
  [[nodiscard]] auto begin() const noexcept -> const TCOD_ConsoleTile* { return tiles; }
  /***************************************************************************
      @brief Return a pointer to the end of this consoles tile data.
   */
  [[nodiscard]] auto end() noexcept -> TCOD_ConsoleTile* { return tiles + elements; }
  /***************************************************************************
      @brief Return a const pointer to the end of this consoles tile data.
   */
  [[nodiscard]] auto end() const noexcept -> const TCOD_ConsoleTile* { return tiles + elements; }
  /***************************************************************************
      @brief Clear a console by setting all tiles to the provided TCOD_ConsoleTile object.

      @param tile A TCOD_ConsoleTile refernce which will be used to clear the console.

      @code{.cpp}
        // New consoles start already cleared with the space character, a white foreground, and a black background.
        auto console = tcod::new_console(80, 50)
        console->clear()  // Clear with the above mentioned defaults.
        console->clear({0x20, {255, 255, 255, 255}, {0, 0, 0, 255}});  // Same as the above.
        console->clear(0x20, {255, 255, 255}, {0, 0, 0})  // Also same as the above.
      @endcode
   */
  void clear(const TCOD_ConsoleTile& tile = {0x20, {255, 255, 255, 255}, {0, 0, 0, 255}}) noexcept {
    for (auto& it : *this) it = tile;
  }
  /***************************************************************************
      @brief Clear the console with the given character and RGB colors.

      @param ch A Unicode codepoint, if unsure then use the space codepoint ``int{0x20}``
      @param fg The foreground color to clear with.
          While it doesn't seem useful at first to set the foreground color at the same time as clearing with space,
          this will still determine what the default color will be if a character is set without also overwriting the
          colors for that tile.
      @param bg The background color to clear with.
   */
  void clear(int ch, const TCOD_ColorRGB& fg, const TCOD_ColorRGB& bg) noexcept {
    clear({ch, TCOD_ColorRGBA{fg.r, fg.g, fg.b, 255}, TCOD_ColorRGBA{bg.r, bg.g, bg.b, 255}});
  }
  /***************************************************************************
      @brief Return a reference to the tile at `xy`.
   */
  [[nodiscard]] auto operator[](const std::array<int, 2>& xy) noexcept -> TCOD_ConsoleTile& {
    return tiles[get_index(xy)];
  }
  /***************************************************************************
      @brief Return a constant reference to the tile at `xy`.
   */
  [[nodiscard]] auto operator[](const std::array<int, 2>& xy) const noexcept -> const TCOD_ConsoleTile& {
    return tiles[get_index(xy)];
  }
  /***************************************************************************
      @brief Return a reference to the tile at `xy`.

      @throws std::out_of_range if the index is out-of-bounds
   */
  [[nodiscard]] auto at(const std::array<int, 2>& xy) -> TCOD_ConsoleTile& { return tiles[bounds_check(xy)]; }
  /***************************************************************************
      @brief Return a constant reference to the tile at `xy`.

      @throws std::out_of_range if the index is out-of-bounds
   */
  [[nodiscard]] auto at(const std::array<int, 2>& xy) const -> const TCOD_ConsoleTile& {
    return tiles[bounds_check(xy)];
  }
  /***************************************************************************
      @brief Return a reference to the tile at `x`,`y`.

      @throws std::out_of_range if the index is out-of-bounds
   */
  [[nodiscard]] auto at(int x, int y) -> TCOD_ConsoleTile& { return at({x, y}); }
  /***************************************************************************
      @brief Return a constant reference to the tile at `x`,`y`.

      @throws std::out_of_range if the index is out-of-bounds
   */
  [[nodiscard]] auto at(int x, int y) const -> const TCOD_ConsoleTile& { return at({x, y}); }
  /***************************************************************************
      @brief Checks if `xy` is in bounds then return an in-bounds index.

      @throws std::out_of_range if `xy` is out-of-bounds
   */
  int bounds_check(const std::array<int, 2>& xy) const {
    if (!in_bounds(xy)) {
      throw std::out_of_range(
          std::string("Out of bounds lookup {") + std::to_string(xy[0]) + ", " + std::to_string(xy[1]) +
          "} on console of shape {" + std::to_string(w) + ", " + std::to_string(h) + "}.");
    }
    return get_index(xy);
  }
  /***************************************************************************
      @brief Convert `xy` into a 1-dimensional index.

      @details This index is normally used to index the tiles attribute.
   */
  [[nodiscard]] int get_index(const std::array<int, 2>& xy) const noexcept { return w * xy[1] + xy[0]; }
  /***************************************************************************
      @brief Return true if `xy` are within the bounds of this console.
   */
  [[nodiscard]] bool in_bounds(const std::array<int, 2>& xy) const noexcept {
    return 0 <= xy[0] && xy[0] < w && 0 <= xy[1] && xy[1] < h;
  }
#endif  // __cplusplus
  /** Console width and height in tiles. */
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
      @brief The total length of the tiles array.  Same as `w * h`.

      \rst
      .. versionadded:: 1.16
      \endrst
   */
  int elements;
  /**
      @brief A userdata attribute which can be repurposed.

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
 *  \param src Pointer to the source console.
 *  \param xSrc The left region of the source console to blit from.
 *  \param ySrc The top region of the source console to blit from.
 *  \param wSrc The width of the region to blit from.
 *              If 0 then it will fill to the maximum width.
 *  \param hSrc The height of the region to blit from.
 *              If 0 then it will fill to the maximum height.
 *  \param dst Pointer to the destination console.
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
 *  \param console A console pointer.
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
    Fade the color of the display.

    \param val Where at 255 colors are normal and at 0 colors are completely
               faded.
    \param fade_color Color to fade towards.
    \rst
    .. deprecated:: 1.19
        This function will not work with libtcod contexts.
    \endrst
 */
TCOD_DEPRECATED("This function does not support contexts.")
TCOD_PUBLIC
void TCOD_console_set_fade(uint8_t val, TCOD_color_t fade_color);
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
/***************************************************************************
    @brief A unique pointer to a TCOD_Console.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
typedef std::unique_ptr<struct TCOD_Console, ConsoleDeleter> ConsolePtr;
/***************************************************************************
    @brief Return a new console with the given size.

    @param width The width of the new console in tiles.
    @param height The height of the new console in tiles.
    @return ConsolePtr, a unique_ptr to a TCOD_Console.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
[[nodiscard]] inline auto new_console(int width, int height) -> ConsolePtr {
  ConsolePtr console{TCOD_console_new(width, height)};
  if (!console) {
    throw std::runtime_error(TCOD_get_error());
  }
  return console;
}
/***************************************************************************
    @brief Return a new console with the given size.

    @param size The {width, height} of the new console.
    @return ConsolePtr, a unique_ptr to a TCOD_Console.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
[[nodiscard]] inline auto new_console(std::array<int, 2> size) -> ConsolePtr {
  return new_console(size.at(0), size.at(1));
}
/***************************************************************************
    @brief Return a copy of another console.

    Only the tile data is copied, all other data will be the same as if a new console was initialized.

    @param console The console to make a copy from.
    @return ConsolePtr, a unique_ptr to a TCOD_Console.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
[[nodiscard]] inline auto new_console(const TCOD_Console& console) -> ConsolePtr {
  auto clone = new_console(console.w, console.h);
  std::copy(console.begin(), console.end(), clone->begin());
  return clone;
}
/***************************************************************************
    @brief A managed libtcod console containing a grid of tiles with `{ch, fg, bg}` information.

    @details Note that all tile references are to TCOD_ConsoleTile structs and will include an alpha channel.

    @code{.cpp}
      auto console = tcod::Console{80, 50};
      console.at({1, 1}).ch = '@';  // Bounds-checked references to a tile.
      console[{1, 1}].bg = {0, 0, 255, 255};  // Access a tile without bounds checking, colors are RGBA.
      if (console.in_bounds({100, 100})) {}  // Test if an index is in bounds.
      for (auto& tile : console) tile.fg = {255, 255, 0, 255};  // Iterate over all tiles on a console.
      for (auto& tile : console) tile = {0x20, {255, 255, 255, 255}, {0, 0, 0, 255}};  // Same as clearing all tiles.
      for (int y = 0; y < console.get_height(); ++y) {
        for (int x = 0; x < console.get_width(); ++x) {
          auto& tile = console.at({x, y});  // Iterate over the coordinates of a console.
        }
      }
    @endcode

    \rst
    .. versionadded:: 1.19
    \endrst
 */
class Console {
 public:
  /***************************************************************************
      @brief Default initializer.
   */
  Console() : Console{0, 0} {}
  /***************************************************************************
      @brief Create a new Console with the given size.

      @param width The number of columns in the new console.
      @param height The number of rows in the new console.
   */
  explicit Console(int width, int height) : console_(TCOD_console_new(width, height)) {
    if (!console_) throw std::runtime_error(TCOD_get_error());
  }
  /***************************************************************************
      @brief Create a new Console with the given size.

      @param size The new console size of `{width, height}`.
   */
  explicit Console(const std::array<int, 2>& size) : Console{size[0], size[1]} {}
  /***************************************************************************
      @brief Clone the shape and tile data of a Console.
   */
  explicit Console(const Console& other) : Console{other.console_->w, other.console_->h} {
    std::copy(other.console_->begin(), other.console_->end(), console_->begin());
  }
  /***************************************************************************
      @brief Pass ownership of a ConsolePtr to a new Console.

      @param ptr A `tcod::ConsolePtr`, must not be nullptr.
   */
  explicit Console(ConsolePtr ptr) : console_{std::move(ptr)} {
    if (!console_) throw std::invalid_argument("Pointer must not be nullptr.");
  }
  /***************************************************************************
      @brief Takes ownership of a raw TCOD_Console pointer.

      @param ptr A pointer which will now be managed by this Console object.  Must not be nullptr.
   */
  explicit Console(TCOD_Console* ptr) : console_{ptr} {
    if (!console_) throw std::invalid_argument("TCOD_Console pointer must not be nullptr.");
  }
  /***************************************************************************
      @brief Copy the shape and tile data of another console.
   */
  Console& operator=(const Console& rhs) {
    if (console_->w != rhs.console_->w || console_->h != rhs.console_->h) {
      *this = Console{{rhs.console_->w, rhs.console_->h}};
    }
    std::copy(rhs.console_->begin(), rhs.console_->end(), console_->begin());
    return *this;
  }
  /***************************************************************************
      @brief Standard move constructor.
   */
  Console(Console&&) noexcept = default;
  /***************************************************************************
      @brief Standard move assignment.
   */
  Console& operator=(Console&& rhs) noexcept {
    std::swap(console_, rhs.console_);
    return *this;
  }
  /***************************************************************************
      @brief Standard destructor.
   */
  ~Console() noexcept = default;

  /***************************************************************************
      @brief Allow implicit conversions to a TCOD_Console reference.
   */
  [[nodiscard]] operator TCOD_Console&() { return *console_; }
  /***************************************************************************
      @brief Allow implicit conversions to a const TCOD_Console reference.
   */
  [[nodiscard]] operator const TCOD_Console&() const { return *console_; }
  /***************************************************************************
      @brief Allow explicit conversions to a TCOD_Console pointer.
   */
  [[nodiscard]] explicit operator TCOD_Console*() noexcept { return console_.get(); }
  /***************************************************************************
      @brief Allow explicit conversions to a const TCOD_Console pointer.
   */
  [[nodiscard]] explicit operator const TCOD_Console*() const noexcept { return console_.get(); }
  /***************************************************************************
      @brief Return a pointer to the internal TCOD_Console struct.
   */
  [[nodiscard]] auto get() noexcept -> TCOD_Console* { return console_.get(); }
  /***************************************************************************
      @brief Return a const pointer to the internal TCOD_Console struct.
   */
  [[nodiscard]] auto get() const noexcept -> const TCOD_Console* { return console_.get(); }
  /***************************************************************************
      @brief Release ownership of this Console's `TCOD_Console*` and return the pointer.

      Using this Console afterwards is undefined.
   */
  auto release() noexcept -> TCOD_Console* { return console_.release(); }
  /***************************************************************************
      @brief Return a pointer to the beginning of this consoles tile data.
   */
  [[nodiscard]] auto begin() noexcept -> TCOD_ConsoleTile* { return console_->tiles; }
  /***************************************************************************
      @brief Return a const pointer to the beginning of this consoles tile data.
   */
  [[nodiscard]] auto begin() const noexcept -> const TCOD_ConsoleTile* { return console_->tiles; }
  /***************************************************************************
      @brief Return a pointer to the end of this consoles tile data.
   */
  [[nodiscard]] auto end() noexcept -> TCOD_ConsoleTile* { return console_->tiles + console_->elements; }
  /***************************************************************************
      @brief Return a const pointer to the end of this consoles tile data.
   */
  [[nodiscard]] auto end() const noexcept -> const TCOD_ConsoleTile* { return console_->tiles + console_->elements; }
  /***************************************************************************
      @brief Return the width of this console.
   */
  [[nodiscard]] auto get_width() const noexcept -> int { return console_->w; }
  /***************************************************************************
      @brief Return the height of this console.
   */
  [[nodiscard]] auto get_height() const noexcept -> int { return console_->h; }
  /***************************************************************************
      @brief Return the `{width, height}` shape of this console as a `std::array<int, 2>`.

      @details
      @code{.cpp}
        auto console = tcod::Console{{80, 50}};
        auto same_size = tcod::Console{console.get_shape()}  // New console with the same shape of the previous one.
      @endcode
   */
  [[nodiscard]] auto get_shape() const noexcept -> std::array<int, 2> { return {console_->w, console_->h}; }
  /***************************************************************************
      @brief Clear a console by setting all tiles to the provided TCOD_ConsoleTile object.

      @param tile A TCOD_ConsoleTile refernce which will be used to clear the console.

      @details
      @code{.cpp}
        // New consoles start already cleared with the space character, a white foreground, and a black background.
        auto console = tcod::Console{{80, 50}};
        console.clear()  // Clear with the above mentioned defaults.
        console.clear({0x20, {255, 255, 255, 255}, {0, 0, 0, 255}});  // Same as the above.
        console.clear(0x20, {255, 255, 255}, {0, 0, 0})  // Also same as the above.
      @endcode
   */
  void clear(const TCOD_ConsoleTile& tile = {0x20, {255, 255, 255, 255}, {0, 0, 0, 255}}) noexcept {
    for (auto& it : *this) it = tile;
  }
  /***************************************************************************
      @brief Clear the console with the given character and RGB colors.

      @param ch A Unicode codepoint, if unsure then use the space codepoint ``int{0x20}``
      @param fg The foreground color to clear with.
          While it doesn't seem useful at first to set the foreground color at the same time as clearing with space,
          this will still determine what the default color will be if a character is set without also overwriting the
          colors for that tile.
      @param bg The background color to clear with.
   */
  void clear(int ch, const TCOD_ColorRGB& fg, const TCOD_ColorRGB& bg) noexcept {
    clear({ch, TCOD_ColorRGBA{fg.r, fg.g, fg.b, 255}, TCOD_ColorRGBA{bg.r, bg.g, bg.b, 255}});
  }
  /***************************************************************************
      @brief Return a reference to the tile at `xy`.
   */
  [[nodiscard]] auto operator[](const std::array<int, 2>& xy) noexcept -> TCOD_ConsoleTile& {
    return console_->tiles[get_index(xy)];
  }
  /***************************************************************************
      @brief Return a constant reference to the tile at `xy`.
   */
  [[nodiscard]] auto operator[](const std::array<int, 2>& xy) const noexcept -> const TCOD_ConsoleTile& {
    return console_->tiles[get_index(xy)];
  }
  /***************************************************************************
      @brief Return a reference to the tile at `xy`.

      @throws std::out_of_range if the index is out-of-bounds
   */
  [[nodiscard]] auto at(const std::array<int, 2>& xy) -> TCOD_ConsoleTile& { return console_->tiles[bounds_check(xy)]; }
  /***************************************************************************
      @brief Return a constant reference to the tile at `xy`.

      @throws std::out_of_range if the index is out-of-bounds
   */
  [[nodiscard]] auto at(const std::array<int, 2>& xy) const -> const TCOD_ConsoleTile& {
    return console_->tiles[bounds_check(xy)];
  }
  /***************************************************************************
      @brief Return a reference to the tile at `x`,`y`.

      @throws std::out_of_range if the index is out-of-bounds
   */
  [[nodiscard]] auto at(int x, int y) -> TCOD_ConsoleTile& { return at({x, y}); }
  /***************************************************************************
      @brief Return a constant reference to the tile at `x`,`y`.

      @throws std::out_of_range if the index is out-of-bounds
   */
  [[nodiscard]] auto at(int x, int y) const -> const TCOD_ConsoleTile& { return at({x, y}); }
  /***************************************************************************
      @brief Return true if `xy` are within the bounds of this console.
   */
  [[nodiscard]] bool in_bounds(const std::array<int, 2>& xy) const noexcept { return console_->in_bounds(xy); }

 private:
  /***************************************************************************
      @brief Checks if `xy` is in bounds then return an in-bounds index.

      @throws std::out_of_range if `xy` is out-of-bounds
   */
  auto bounds_check(const std::array<int, 2>& xy) const -> int {
    if (!in_bounds(xy)) {
      throw std::out_of_range(
          std::string("Out of bounds lookup {") + std::to_string(xy[0]) + ", " + std::to_string(xy[1]) +
          "} on console of shape {" + std::to_string(console_->w) + ", " + std::to_string(console_->h) + "}.");
    }
    return get_index(xy);
  }
  /***************************************************************************
      @brief Convert `xy` into a 1-dimensional index.

      @details This index is normally used to index the tiles attribute.
   */
  [[nodiscard]] auto get_index(const std::array<int, 2>& xy) const noexcept -> int {
    return console_->w * xy[1] + xy[0];
  }

  ConsolePtr console_ = nullptr;  // The owned TCOD_Console pointer.
};
}  // namespace tcod
#endif  // __cplusplus
#endif  // TCOD_CONSOLE_H_
