
#include "tileset.h"

#include <memory>
#include <string>

#include "observer.h"
#include "tilesheet.h"
#include "../libtcod_int.h"

namespace tcod {
namespace tileset {
std::unique_ptr<Tileset> load_legacy_font()
{
  TilesheetLayout layout;
  layout.tile_width = TCOD_ctx.font_width;
  layout.tile_height = TCOD_ctx.font_height;
  // TCOD_ctx.font_file might be invalid.
  image::Image image = image::load(TCOD_ctx.font_file);

  Tilesheet tilesheet(image, layout);

  std::unique_ptr<Tileset> tileset =
      std::make_unique<Tileset>(layout.tile_width, layout.tile_height);

  for (int i = 0; i < tilesheet.count(); ++i) {
    tileset->set_tile(0, tilesheet.get_tile(i));
  }
  return tileset;
}

extern "C" {
Tileset* TCOD_tileset_new(int tile_width, int tile_height) {
  return new Tileset(tile_width, tile_height);
}
void TCOD_tileset_delete(Tileset* tileset) {
  if (tileset) { delete tileset; }
}
} // extern "C"
} // namespace tileset
} // namespace tcod
