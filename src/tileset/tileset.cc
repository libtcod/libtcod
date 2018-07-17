
#include <tileset/tileset.h>

namespace tcod {
extern "C" {
TCOD_Tileset *TCOD_tileset_new(int tile_width, int tile_height) {
  return new Tileset(tile_width, tile_height);
}
void TCOD_tileset_delete(TCOD_Tileset *tileset) {
  if (tileset) { delete tileset; }
}
} // extern "C"
} // namespace tcod
