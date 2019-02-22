
#include <fstream>
#include <iterator>

#include "truetype.h"
#include "../engine/globals.h"
#include "../../vendor/stb_truetype.h"

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
    return {};
  }
  return std::basic_string<unsigned char>(
      std::istreambuf_iterator<char>(file),
      std::istreambuf_iterator<char>()
  );
}
class TTFontLoader {
 public:
  TTFontLoader(const std::string& path, int tile_width, int tile_height)
  : font_data_(load_data_file(path)), width_(tile_width), height_(tile_height)
  {
    stbtt_InitFont(&font_info_, font_data_.data(), 0);
    scale_ = stbtt_ScaleForPixelHeight(&font_info_, height_);
    int raw_ascent;
    int raw_descent;
    int raw_line_gap;
    stbtt_GetFontVMetrics(&font_info_, &raw_ascent, &raw_descent, &raw_line_gap);
    ascent_ = raw_ascent * scale_;
    descent_ = raw_descent * scale_;
  }
  auto generate_tileset() const -> std::unique_ptr<Tileset>
  {
    auto tileset = std::make_unique<Tileset>(width_, height_);
    for (int codepoint = 1; codepoint <= 0x1ffff; ++codepoint) {
      int glyph = stbtt_FindGlyphIndex(&font_info_, codepoint);
      if (!glyph) { continue; }
      Image image(width_, height_, {0xff, 0xff, 0xff, 0x00});
      Vector2<unsigned char> alpha(width_, height_);
      BBox bbox = get_glyph_bbox(glyph);
      float shift_x = (width_ - bbox.right) / 2.0f;
      float shift_y = bbox.top + ascent_;
      stbtt_MakeGlyphBitmapSubpixel(
          &font_info_,
          alpha.data(),
          width_,
          height_,
          static_cast<int>(sizeof(alpha.data()[0]) * alpha.width()),
          scale_,
          scale_,
          0.0f, // shift_x
          0.0f, // shift_y
          glyph);
      for (int img_y = 0; img_y < image.height(); ++img_y) {
        int alpha_y = img_y - static_cast<int>(shift_y);
        if (alpha_y < 0 || alpha.height() <= alpha_y) { continue; }
        for (int img_x = 0; img_x < image.width(); ++img_x) {
          int alpha_x = img_x - static_cast<int>(shift_x);
          if (alpha_x < 0 || alpha.width() <= alpha_x) { continue; }
          image.at(img_x, img_y).a = alpha.at(alpha_x, alpha_y);
        }
      }
      tileset->set_tile(codepoint, image);
    }
    return tileset;
  }
 private:
  struct BBox {
    int left;
    int top;
    int right;
    int bottom;
  };
  auto get_glyph_bbox(int glyph) const -> BBox
  {
    BBox bbox;
    stbtt_GetGlyphBitmapBox(&font_info_, glyph, scale_, scale_,
                            &bbox.left, &bbox.top, &bbox.right, &bbox.bottom);
    return bbox;
  }
  /**
   *  Data buffer for the font file.
   */
  std::basic_string<unsigned char> font_data_;
  /**
   *  Tile width.
   */
  int width_;
  /**
   *  Tile height.
   */
  int height_;
  /**
   *  stb_truetype font info struct.
   */
  stbtt_fontinfo font_info_;
  /**
   *  Font scale needed to fit the current height.
   */
  float scale_;
  float ascent_;
  float descent_;
};
auto load_truetype(const std::string& path, int tile_width, int tile_height)
-> std::unique_ptr<Tileset>
{
  return TTFontLoader(path, tile_width, tile_height).generate_tileset();
}
} // namespace tileset
} // namespace tcod
int TCOD_tileset_load_truetype_(
    const char* path,
    int tile_width,
    int tile_height)
{
  using tcod::engine::set_tileset;
  using tcod::tileset::load_truetype;
  if (!path) { return -1; }
  set_tileset(load_truetype(path, tile_width, tile_height));
  return 0;
}
