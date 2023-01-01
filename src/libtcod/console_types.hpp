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
#ifndef TCOD_CONSOLE_TYPES_HPP_
#define TCOD_CONSOLE_TYPES_HPP_

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "console.h"

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
    swap(*this, rhs);
    return *this;
  }
  /***************************************************************************
      @brief Standard destructor.
   */
  ~Console() noexcept = default;
  /***************************************************************************
      @brief Swap two console objects.
   */
  friend void swap(Console& lhs, Console& rhs) noexcept {
    using std::swap;
    swap(lhs.console_, rhs.console_);
  }
  /***************************************************************************
      @brief Allow implicit conversions to a TCOD_Console reference.
   */
  [[nodiscard]] operator TCOD_Console&() { return *console_; }
  /***************************************************************************
      @brief Allow implicit conversions to a const TCOD_Console reference.
   */
  [[nodiscard]] operator const TCOD_Console&() const { return *console_; }
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
        auto console = tcod::Console{80, 50};
        auto same_size = tcod::Console{console.get_shape()}  // New console with the same shape of the previous one.
      @endcode
   */
  [[nodiscard]] auto get_shape() const noexcept -> std::array<int, 2> { return {console_->w, console_->h}; }
  /***************************************************************************
      @brief Clear a console by setting all tiles to the provided TCOD_ConsoleTile object.

      @param tile A TCOD_ConsoleTile reference which will be used to clear the console.

      @details
      @code{.cpp}
        // New consoles start already cleared with the space character, a white foreground, and a black background.
        auto console = tcod::Console{80, 50};
        console.clear()  // Clear with the above mentioned defaults.
        console.clear({0x20, {255, 255, 255, 255}, {0, 0, 0, 255}});  // Same as the above.
        console.clear({0x20, tcod::ColorRGB{255, 255, 255}, tcod::ColorRGB{0, 0, 0}})  // Also same as the above.
      @endcode
   */
  void clear(const TCOD_ConsoleTile& tile = {0x20, {255, 255, 255, 255}, {0, 0, 0, 255}}) noexcept {
    for (auto& it : *this) it = tile;
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
#endif  // TCOD_CONSOLE_TYPES_HPP_
