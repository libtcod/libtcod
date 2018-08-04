
#ifndef LIBTCOD_TILESET_TILESHEET_H_
#define LIBTCOD_TILESET_TILESHEET_H_
#include <string>

#include "../color/canvas.h"
#ifdef __cplusplus
namespace tcod {
class Tilesheet: public Canvas<ColorRGBA> {
 public:
  Tilesheet() = default;
  explicit Tilesheet(int width, int height): Canvas(width, height) {}

  Tilesheet(Tilesheet&&) = default;
  Tilesheet& operator=(Tilesheet&&) = default;
  Tilesheet(const Tilesheet&) = default;
  Tilesheet& operator=(const Tilesheet&) = default;
};
/**
 *  Load a Tilesheet from an image file.
 */
Tilesheet LoadTilesheet(const std::string &filename);
} // namespace tcod
#endif // __cplusplus
#endif /* LIBTCOD_TILESET_TILESHEET_H_ */
