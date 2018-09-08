
#include "sdl2_alias.h"

#include <map>

#include <SDL.h>
namespace tcod {
namespace sdl2 {
SDL2TilesetAlias::pool_type SDL2TilesetAlias::alias_pool_;
void SDL2TilesetAlias::on_tileset_attached(const Tileset &tileset) {}
void SDL2TilesetAlias::on_tileset_changed(
      const Tileset &tileset,
      const std::vector<std::pair<int, Tile&>> &changes) {}
void SDL2TilesetAlias::on_tileset_detached() {
  alias_pool_.erase(alias_pool_.find(std::make_pair(tileset_, renderer_)));
}
} // namespace sdl2
} // namespace tcod
