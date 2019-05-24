/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
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
#include "tilesheet.h"

#include "../../vendor/lodepng.h"
#include "../color/canvas.h"
namespace tcod {
namespace tileset {
class Tilesheet::impl {
 public:
  impl() = default;
  explicit impl(const Image& canvas, const TilesheetLayout& layout)
  : canvas_(canvas), layout_(layout) {
    fill_layout();
    color_key_ = guess_color_key();
  }
  impl(impl&&) = default;
  impl& operator=(impl&&) = default;
  impl(const impl&) = default;
  impl& operator=(const impl&) = default;
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
  int get_tile_width() const noexcept {
    return layout_.tile_width;
  }
  int get_tile_height() const noexcept {
    return layout_.tile_height;
  }
  int get_columns() const noexcept {
    return layout_.columns;
  }
  int get_rows() const noexcept {
    return layout_.rows;
  }
  int count() const  noexcept {
    return get_columns() * get_rows();
  }
 private:
  /**
   *  Automatically fill values which are 0 in layout_.
   */
  void fill_layout() {
    // Find undefined columns/rows from tile size and canvas_ size.
    if (layout_.columns == 0 && layout_.tile_width > 0) {
      layout_.columns = static_cast<int>(canvas_.width() / layout_.tile_width);
    }
    if (layout_.rows == 0 && layout_.tile_height > 0) {
      layout_.rows = static_cast<int>(canvas_.height() / layout_.tile_height);
    }
    // Find undefined tile width/height from columns/rows and canvas_ size.
    if (layout_.tile_width == 0 && layout_.columns > 0) {
      layout_.tile_width = static_cast<int>(canvas_.width() / layout_.columns);
    }
    if (layout_.tile_height == 0 && layout_.rows > 0) {
      layout_.tile_height = static_cast<int>(canvas_.height() / layout_.rows);
    }
    if (!layout_.columns || !layout_.rows) {
      throw std::logic_error("Tilesheet layout is non-valid.");
    }
  }
  /**
   *  Attempt to figure out a tile-sheets color key automatically.
   */
  ColorRGBA guess_color_key()
  {
    Image tile(get_tile(0)); // Check the NULL tile.
    const ColorRGBA color_key(tile.atf(0, 0));
    if (color_key.a != 255) {
      return {0, 0, 0, 0};
    }
    // Tile must be a solid color.
    for (const ColorRGBA& pixel : tile) {
      if (pixel != color_key) {
        return {0, 0, 0, 0};
      }
    }
    return color_key;
  }
  /**
   *  Return a new Tile from the given region on the Tilesheet.
   */
  Image new_tile(int x, int y, int width, int height) const {
    Image tile{width, height};
    for (int pixel_y = 0; pixel_y < height; ++pixel_y) {
      for (int pixel_x = 0; pixel_x < width; ++pixel_x) {
        tile.atf(pixel_x, pixel_y) = canvas_.atf(x + pixel_x, y + pixel_y);
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
    // Convert a grey-scale tiles to white-with-alpha.
    if (!is_colored && !has_alpha) {
      for (ColorRGBA& pixel : tile) {
        pixel.a = pixel.r;
        pixel.r = pixel.g = pixel.b = 0xff;
      }
    }
    // Set key-color pixels to zero.
    for (ColorRGBA& pixel : tile) {
      if (pixel == color_key_) {
        pixel.r = pixel.g = pixel.b = pixel.a = 0;
      }
    }
    return tile;
  }
  Image canvas_;
  TilesheetLayout layout_;
  ColorRGBA color_key_;
};

Tilesheet::Tilesheet() = default;
Tilesheet::Tilesheet(const Image& canvas, const TilesheetLayout& layout)
: impl_(std::make_unique<impl>(canvas, layout))
{}
Tilesheet::Tilesheet(const Image& canvas, const std::pair<int, int>& layout)
: Tilesheet(canvas, {0, 0, layout.first, layout.second})
{}

Tilesheet::Tilesheet(Tilesheet&&) noexcept = default;
Tilesheet& Tilesheet::operator=(Tilesheet&&) noexcept = default;
Tilesheet::Tilesheet(const Tilesheet& rhs)
: impl_(std::make_unique<impl>(*rhs.impl_))
{}
Tilesheet& Tilesheet::operator=(const Tilesheet& rhs)
{
  impl_ = std::make_unique<impl>(*rhs.impl_);
  return *this;
}
Tilesheet::~Tilesheet() noexcept = default;

Image Tilesheet::get_tile(int x, int y) const
{
  return impl_->get_tile(x, y);
}
Image Tilesheet::get_tile(int n) const
{
  return impl_->get_tile(n);
}
int Tilesheet::get_tile_width() const noexcept
{
  return impl_->get_tile_width();
}
int Tilesheet::get_tile_height() const noexcept
{
  return impl_->get_tile_height();
}
int Tilesheet::get_columns() const noexcept
{
  return impl_->get_columns();
}
int Tilesheet::get_rows() const noexcept
{
  return impl_->get_rows();
}
int Tilesheet::count() const noexcept
{
  return impl_->count();
}
Tilesheet LoadTilesheet(const std::string& filename,
                        const TilesheetLayout& layout) {
  unsigned img_width, img_height;
  std::vector<unsigned char> img_data;
  if (lodepng::decode(img_data, img_width, img_height, filename)) {
    return Tilesheet();
  }
  Image canvas(img_width, img_height);
  std::vector<unsigned char>::iterator img_iter = img_data.begin();
  for (int y = 0; y < canvas.height(); ++y) {
    for (int x = 0; y < canvas.width(); ++x) {
      canvas.atf(x, y) = ColorRGBA{
          img_iter[0], img_iter[1], img_iter[2], img_iter[3]
      };
      img_iter += 4;
    }
  }
  return Tilesheet(canvas, layout);
}
} // namespace tileset
} // namespace tcod
