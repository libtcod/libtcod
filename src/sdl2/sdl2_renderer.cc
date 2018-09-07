
#include "sdl2_renderer.h"

#include <map>

#include <SDL.h>
namespace tcod {
namespace sdl2 {
using tileset::Tile;
using tileset::Tileset;
using tileset::TilesetObserver;
class SDL2TilesetAlias;
std::map<const Tileset*, SDL2TilesetAlias> sdl2_alias_map;

class SDL2TilesetAlias: public TilesetObserver {
 public:
  SDL2TilesetAlias(Tileset& tileset, struct SDL_Renderer* sdl2_renderer)
    : TilesetObserver(tileset), renderer_(sdl2_renderer) { }
 protected:
  void OnTilesetAttach(const Tileset &tileset) override {
  }
  void OnTilesetChanged(
      const Tileset &tileset,
      const std::vector<std::pair<int, Tile&>> &changes) override {
  }
  void OnTilesetDetach() override {
    sdl2_alias_map.erase(sdl2_alias_map.find(tileset_));
  }
 private:
  Tileset* tileset_;
  struct SDL_Renderer* renderer_;
  struct SDL_Texture* texture_;
};
/**
 *  Return the alias linked to this tileset,
 *  making a new one if it doesn't exist.
 */
SDL2TilesetAlias& get_sdl2_alias(Tileset& tileset,
                    struct SDL_Renderer* sdl2_renderer) {
  return sdl2_alias_map.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(&tileset),
      std::forward_as_tuple(tileset, sdl2_renderer)).first->second;
}
SDL2Renderer::SDL2Renderer(struct SDL_Renderer* sdl2_renderer) {
  renderer_ = sdl2_renderer;
}
} // namespace sdl2
} // namespace tcod
