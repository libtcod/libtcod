
#ifndef LIBTCOD_TILESET_TILESHEET_H_
#define LIBTCOD_TILESET_TILESHEET_H_
#ifdef __cplusplus
#include <string>
#endif // __cplusplus

#include "../color/canvas.h"
#ifdef __cplusplus
namespace tcod {
struct TilesheetSettings {
  int tile_width;
  int tile_height;
  int columns;
  int rows;
};
class Tilesheet {
 public:
  Tilesheet() = default;
  explicit Tilesheet(const Canvas& canvas)
  : canvas_(canvas) {}

  Tilesheet(Tilesheet&&) = default;
  Tilesheet& operator=(Tilesheet&&) = default;
  Tilesheet(const Tilesheet&) = default;
  Tilesheet& operator=(const Tilesheet&) = default;
 private:
  Canvas canvas_;
};
/**
 *  Load a Tilesheet from an image file.
 */
Tilesheet LoadTilesheet(const std::string &filename);
} // namespace tcod
#endif // __cplusplus
#endif /* LIBTCOD_TILESET_TILESHEET_H_ */
