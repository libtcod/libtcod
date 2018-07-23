
#ifndef LIBTCOD_TILESET_TILE_H_
#define LIBTCOD_TILESET_TILE_H_
#include "libtcod_portability.h"

#include <stdint.h>
#ifdef __cplusplus
#include <algorithm>
#include <vector>
#endif
#ifdef __cplusplus
namespace tcod {
struct ColorRGBA {
  uint8_t r, g, b, a;
};
class Tile {
 public:
  Tile(int width, int height) {
    width_ = std::max(0, width);
    height_ = std::max(0, height);
    pixels_.resize(width_ * height_ * 4, ColorRGBA{0xff, 0xff, 0xff, 0x00});
  }
  Tile(Tile&&) = default;
  Tile& operator=(Tile&&) = default;
  Tile(const Tile&) = default;
  Tile& operator=(const Tile&) = default;
  /** Return the width of this tile. */
  int GetWidth(void) const noexcept { return width_; }
  /** Return the height of this tile. */
  int GetHeight(void) const noexcept { return height_; }
  /**
   *  Return the ColorRGBA data for the pixel at `x`,`y`.
   *
   *  Out-of-bounds coordinates will return `ColorRGBA{0, 0, 0, 0}`.
   */
  ColorRGBA GetPixel(int x, int y) const noexcept {
    if (IsInBounds(x, y)) {
      return pixels_[y * width_ + x];
    } else {
      return ColorRGBA{0, 0, 0, 0};
    }
  }
  /**
   *  Set the pixel at `x`,`y` to `rgba`.
   *
   *  Out-of-bounds coordinates are silently ignored.
   */
  void SetPixel(int x, int y, const ColorRGBA &rgba) noexcept {
    if (IsInBounds(x, y)) {
      pixels_[y * width_ + x] = rgba;
    }
  }
  /**
   *  Set an alpha-only pixel at `x`,`y` to `alpha`.
   */
  void SetPixel(int x, int y, uint8_t alpha) noexcept {
    if (IsInBounds(x, y)) {
      pixels_[y * width_ + x] = {0xff, 0xff, 0xff, alpha};
    }
  }
 private:
  /**
   *  Return true if x and y are in the bounds of this tile.
   */
  bool IsInBounds(int x, int y) const noexcept {
    return 0 <= x && x < width_ && 0 <= y && y < height_;
  }
  /** The width of this tile. */
  int width_;
  /** The height of this tile. */
  int height_;
  /** A 2D array of ColorRGBA pixels. */
  std::vector<ColorRGBA> pixels_;
};
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_TILESET_TILE_H_
