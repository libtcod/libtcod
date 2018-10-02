
#ifndef LIBTCOD_TILESET_TILE_H_
#define LIBTCOD_TILESET_TILE_H_
#include <stdint.h>
#ifdef __cplusplus
#include <algorithm>
#include <vector>
#endif

#include "../portability.h"
#include "../color/canvas.h"
#ifdef __cplusplus
namespace tcod {
namespace tileset {
using image::Image;

class Tile {
 public:
  Tile();
  explicit Tile(int codepoint, const Image &image)
  : codepoint(codepoint), image_(image)
  {}
  explicit Tile(int codepoint, int width, int height)
  : Tile(codepoint, Image(width, height))
  {}

  int width(void) const { return image_.width(); }
  int height(void) const { return image_.height(); }
  Image get_image() const {
    return image_;
  }
  const int codepoint;
 private:
  Image image_;
};
} // namespace tileset
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_TILESET_TILE_H_
