
#include <tileset/tileset.h>

namespace tcod {
static std::vector<std::shared_ptr<Tileset>> c_tilesets;
extern "C" {
TCOD_Tileset *TCOD_tileset_new(int tile_width, int tile_height) {
  std::shared_ptr<Tileset> ptr = Tileset::New(tile_width, tile_height);
  c_tilesets.push_back(ptr);
  return ptr.get();
}
void TCOD_tileset_delete(TCOD_Tileset *tileset) {
}
} // extern "C"
} // namespace tcod
