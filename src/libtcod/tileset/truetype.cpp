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
#include <array>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <utility>

#include "truetype.h"
#include "../engine/error.h"
#include "../engine/globals.h"
#include "../../vendor/stb_truetype.h"
// You can look here for a reference on glyph metrics:
// https://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html
namespace tcod {
namespace tileset {
/**
 *  Return an entire file as a single data buffer.
 */
static auto load_data_file(const std::string& path)
-> std::basic_string<unsigned char>
{
  std::ifstream file(path, std::ios::in | std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error(std::string("File not found: ") + path);
  }
  return std::basic_string<unsigned char>(
      std::istreambuf_iterator<char>(file),
      std::istreambuf_iterator<char>()
  );
}
/**
 *  Return the stbtt_fontinfo for the given `font_data`.
 */
static auto init_font_info(const std::basic_string<unsigned char>& font_data)
-> stbtt_fontinfo
{
  stbtt_fontinfo font_info;
  stbtt_InitFont(&font_info, font_data.data(), 0);
  return font_info;
}
/**
 *  Converts TrueType fonts into tiles.
 */
class TTFontLoader {
 public:
  TTFontLoader(const std::string& path, int tile_width, int tile_height)
  : font_data_(load_data_file(path)),
    font_info_(init_font_info(font_data_)),
    width_(tile_width),
    height_(tile_height),
    scale_(stbtt_ScaleForPixelHeight(&font_info_, height_))
  {
    if (width_ <= 0) {
      tile_width = width_ = guess_font_width();
    }
    BBox font_bbox = get_font_bbox();
    float font_width = font_bbox.width() * scale_;
    if (font_width > tile_width) {
      // Shrink the font to fit its tile width.
      scale_ *= static_cast<float>(tile_width) / font_width;
    }
  }
  auto generate_tileset() const -> std::unique_ptr<Tileset>
  {
    auto tileset = std::make_unique<Tileset>(width_, height_);
    for (int codepoint = 1; codepoint <= 0x1ffff; ++codepoint) {
      int glyph = stbtt_FindGlyphIndex(&font_info_, codepoint);
      if (!glyph) { continue; }
      tileset->set_tile(codepoint, render_glyph(glyph));
    }
    return tileset;
  }
 private:
  struct BBox {
    int width() const noexcept
    {
      return xMax - xMin;
    }
    int height() const noexcept
    {
      return yMax - yMin;
    }
    int xMin;
    int yMin;
    int xMax;
    int yMax;
  };
  template<typename T>
  struct Point {
    T x;
    T y;
  };
  /**
   *  Return the Image for a specific glyph.
   */
  auto render_glyph(int glyph) const -> Image
  {
    Point<float> shift(get_glyph_shift(glyph));
    Image image(width_, height_, {0xff, 0xff, 0xff, 0x00});
    Vector2<unsigned char> alpha(width_, height_);
    stbtt_MakeGlyphBitmapSubpixel(
        &font_info_,
        alpha.data(),
        width_,
        height_,
        static_cast<int>(sizeof(alpha.data()[0]) * alpha.width()),
        scale_,
        scale_,
        shift.x,
        shift.y,
        glyph);
    for (int img_y = 0; img_y < image.height(); ++img_y) {
      for (int img_x = 0; img_x < image.width(); ++img_x) {
        int alpha_y = img_y - static_cast<int>(shift.y);
        int alpha_x = img_x - static_cast<int>(shift.x);
        if (alpha_y < 0 || alpha.height() <= alpha_y) { continue; }
        if (alpha_x < 0 || alpha.width() <= alpha_x) { continue; }
        image.atf(img_x, img_y).a = alpha.atf(alpha_x, alpha_y);
      }
    }
    return image;
  }
  /**
   *  Return the bounding box for this glyph.
   */
  auto get_glyph_bbox(int glyph, float scale=1.0f) const -> BBox
  {
    BBox bbox;
    stbtt_GetGlyphBitmapBox(&font_info_, glyph, scale, scale,
                            &bbox.xMin, &bbox.yMin, &bbox.xMax, &bbox.yMax);
    return bbox;
  }
  /**
   *  Return the bounding box for all possible glyphs.
   */
  auto get_font_bbox() const -> BBox
  {
    BBox bbox;
    stbtt_GetFontBoundingBox(
        &font_info_, &bbox.xMin, &bbox.yMin, &bbox.xMax, &bbox.yMax);
    return bbox;
  }
  /**
   *  Try to return a decent guess for this fonts width.
   */
  int guess_font_width() const
  {
    BBox font_bbox = get_font_bbox();
    return static_cast<int>(static_cast<float>(font_bbox.width()) * scale_);
  }
  /**
   *  Return the shift needed to align this glyph with the current tile size.
   */
  auto get_glyph_shift(int glyph) const -> Point<float>
  {
    int ascent;
    int descent;
    int line_gap;
    stbtt_GetFontVMetrics(&font_info_, &ascent, &descent, &line_gap);
    BBox font_bbox = get_font_bbox();
    BBox bbox = get_glyph_bbox(glyph);
    return {
        (width_ - bbox.width() * scale_) * alignment_.at(0),
        (bbox.yMin + ascent) * scale_
        + static_cast<int>((height_ - (ascent - descent) * scale_)
                           * alignment_.at(1)),
    };
  }
  /**
   *  A data buffer referenced by `font_info_`.
   */
  std::basic_string<unsigned char> font_data_;
  /**
   *  stb_truetype font info struct.
   */
  stbtt_fontinfo font_info_;
  /**
   *  Tile width.
   */
  int width_;
  /**
   *  Tile height.
   */
  int height_;
  /**
   *  Font scale needed to fit the current height.
   */
  float scale_;
  /**
   *  Font glyph alignment inside of a tile.
   */
  std::array<float, 2> alignment_{0.5f, 0.5f};
};
auto load_truetype(
    const std::string& path,
    const std::array<int, 2>& tile_size)
-> std::unique_ptr<Tileset>
{
  return TTFontLoader(path, tile_size[0], tile_size[1]).generate_tileset();
}
} // namespace tileset
} // namespace tcod
TCODLIB_CAPI TCOD_Tileset* TCOD_load_truetype_font_(
    const char* path,
    int tile_width,
    int tile_height)
{
  using tcod::tileset::load_truetype;
  using tcod::tileset::Tileset;
  if (!path) { return nullptr; }
  try {
    auto tileset = load_truetype(path, {tile_width, tile_height});
    return new TCOD_Tileset(std::move(tileset));
  } catch (const std::exception& e) {
    tcod::set_error(e);
    return nullptr;
  }
}
int TCOD_tileset_load_truetype_(
    const char* path,
    int tile_width,
    int tile_height)
{
  TCOD_Tileset* tileset = TCOD_load_truetype_font_(path,
                                                   tile_width, tile_height);
  if (!tileset) { return -1; }
  try {
    tcod::engine::set_tileset(*tileset);
  } catch (const std::exception& e) {
    return tcod::set_error(e);
  }
  return 0;
}
