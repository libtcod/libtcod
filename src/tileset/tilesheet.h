
#ifndef LIBTCOD_TILESET_TILESHEET_H_
#define LIBTCOD_TILESET_TILESHEET_H_
#ifdef __cplusplus
#include <string>
#endif // __cplusplus

#include "../color/canvas.h"
#ifdef __cplusplus
namespace tcod {
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
  explicit Tilesheet(const Canvas& canvas, const TilesheetLayout& layout)
  : canvas_(canvas), layout_(layout) {
    fill_layout();
  }
  Tilesheet(Tilesheet&&) = default;
  Tilesheet& operator=(Tilesheet&&) = default;
  Tilesheet(const Tilesheet&) = default;
  Tilesheet& operator=(const Tilesheet&) = default;
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
  }
  Canvas canvas_;
  TilesheetLayout layout_;
};
/**
 *  Load a Tilesheet from an image file.
 */
Tilesheet LoadTilesheet(const std::string &filename);
} // namespace tcod
#endif // __cplusplus
#endif /* LIBTCOD_TILESET_TILESHEET_H_ */
