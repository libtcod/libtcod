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
#ifndef LIBTCOD_TILESET_TILESHEET_H_
#define LIBTCOD_TILESET_TILESHEET_H_
#ifdef __cplusplus
#include <string>
#include <utility>
#endif // __cplusplus

#include "../color/canvas.h"
#include "tile.h"
#ifdef __cplusplus
namespace tcod {
namespace tileset {
using image::Image;
/**
 *  The layout of tiles on a tile-sheet.
 */
struct TilesheetLayout {
  int tile_width;
  int tile_height;
  int columns;
  int rows;
};
class Tilesheet {
 public:
  Tilesheet() = default;
  explicit Tilesheet(const Image& canvas, const TilesheetLayout& layout)
  : canvas_(canvas), layout_(layout) {
    fill_layout();
  }
  explicit Tilesheet(const Image& canvas, const std::pair<int, int>& layout)
  : Tilesheet(canvas, {0, 0, layout.first, layout.second})
  {}
  Tilesheet(Tilesheet&&) = default;
  Tilesheet& operator=(Tilesheet&&) = default;
  Tilesheet(const Tilesheet&) = default;
  Tilesheet& operator=(const Tilesheet&) = default;
  /**
   *  Return the Tile at `x` and `y`.
   */
  Image get_tile(int x, int y) const {
    if (!(0 <= x && x < layout_.columns && 0 <= y && y < layout_.rows)) {
      throw std::out_of_range("Tile not in Tilesheet layout.");
    }
    return new_tile(x * layout_.tile_width, y * layout_.tile_height,
                    layout_.tile_width, layout_.tile_height);
  }
  /**
   *  Return the Tile at `n`.
   */
  Image get_tile(int n) const {
    return get_tile(n % layout_.columns, n / layout_.columns);
  }
  int get_tile_width() const {
    return layout_.tile_width;
  }
  int get_tile_height() const {
    return layout_.tile_height;
  }
  int get_columns() const {
    return layout_.columns;
  }
  int get_rows() const {
    return layout_.rows;
  }
  int count() const {
    return get_columns() * get_rows();
  }
 private:
  /**
   *  Automatically fill values which are 0 in layout_.
   */
  void fill_layout() {
    // Find undefined columns/rows from tile size and canvas_ size.
    if (layout_.columns == 0 && layout_.tile_width > 0) {
      layout_.columns = canvas_.width() / layout_.tile_width;
    }
    if (layout_.rows == 0 && layout_.tile_height > 0) {
      layout_.rows = canvas_.height() / layout_.tile_height;
    }
    // Find undefined tile width/height from columns/rows and canvas_ size.
    if (layout_.tile_width == 0 && layout_.columns > 0) {
      layout_.tile_width = canvas_.width() / layout_.columns;
    }
    if (layout_.tile_height == 0 && layout_.rows > 0) {
      layout_.tile_height = canvas_.height() / layout_.rows;
    }
    if (!layout_.columns || !layout_.rows) {
      throw std::logic_error("Tilesheet layout is non-valid.");
    }
  }
  /**
   *  Return a new Tile from the given region on the Tilesheet.
   */
  Image new_tile(int x, int y, int width, int height) const {
    Image tile{width, height};
    for (int pixel_y = 0; pixel_y < height; ++pixel_y) {
      for (int pixel_x = 0; pixel_x < width; ++pixel_x) {
        tile.at(pixel_x, pixel_y) = canvas_.at(x + pixel_x,
                                                      y + pixel_y);
      }
    }
    bool is_colored = false;
    for (const ColorRGBA& pixel : tile) {
      if (pixel.r != pixel.g || pixel.r != pixel.b) {
        is_colored = true;
        break;
      }
    }
    bool has_alpha = false;
    for (ColorRGBA& pixel : tile) {
      if (pixel.a != 0xff) {
        has_alpha = true;
        break;
      }
    }
    if (!is_colored && !has_alpha) {
      for (ColorRGBA& pixel : tile) {
        pixel.a = pixel.r;
        pixel.r = pixel.g = pixel.b = 0xff;
      }
    }
    return tile;
  }
  Image canvas_;
  TilesheetLayout layout_;
};
/**
 *  Load a Tilesheet from an image file.
 */
Tilesheet LoadTilesheet(const std::string &filename);
} // namespace tileset
} // namespace tcod
#endif // __cplusplus
#endif /* LIBTCOD_TILESET_TILESHEET_H_ */
