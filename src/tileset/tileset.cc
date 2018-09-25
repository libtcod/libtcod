
#include "tileset.h"

#include <memory>
#include <string>

#include "observer.h"
#include "tilesheet.h"
#include "../libtcod_int.h"

namespace tcod {
namespace tileset {
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
