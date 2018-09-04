
#include "sdl2_renderer.h"

#include <SDL.h>
namespace tcod {
namespace sdl2 {
class SDL2TilesetAlias: public tileset::TilesetObserver {
 public:
  SDL2TilesetAlias(struct SDL_Renderer* sdl2_renderer) {
    renderer_ = sdl2_renderer;
  }
 protected:
  void OnTilesetAttach(const tileset::Tileset &tileset) override {
  }
  void OnTilesetChanged(
      const tileset::Tileset &tileset,
      const std::vector<std::pair<int, tileset::Tile&>> &changes) override {
  }
  void OnTilesetDetach() override {
  }
 private:
  struct SDL_Renderer* renderer_;
  struct SDL_Texture* texture_;
};
SDL2Renderer::SDL2Renderer(struct SDL_Renderer* sdl2_renderer) {
  renderer_ = sdl2_renderer;
}
} // namespace sdl2
} // namespace tcod
