
#include "sdl2_alias.h"

#include <map>
#include <memory>
#include <tuple>

#include <SDL.h>
namespace tcod {
namespace sdl2 {
std::map<std::tuple<const Tileset*, const struct SDL_Renderer*>,
         std::shared_ptr<SDL2InternalTilesetAlias_>> sdl2_alias_pool = {};

class SDL2InternalTilesetAlias_: public TilesetObserver {
 public:
  using key_type = std::tuple<const Tileset*, const struct SDL_Renderer*>;
  SDL2InternalTilesetAlias_(Tileset& tileset,
                           struct SDL_Renderer* sdl2_renderer)
  : TilesetObserver(tileset), renderer_(sdl2_renderer)
  {
    observe(tileset);
  }
 protected:
  virtual void on_tileset_attached(const Tileset &tileset) override
  {
    tileset_ = &tileset;
  }

  virtual void on_tileset_changed(
      const Tileset &tileset,
      const std::vector<std::pair<int, Tile&>> &changes) override
  {}

  virtual void on_tileset_detached() override
  {}
 private:
  const Tileset* tileset_;
  struct SDL_Renderer* renderer_;
  struct SDL_Texture* texture_;

};

SDL2TilesetAlias::SDL2TilesetAlias(Tileset& tileset,
                                   struct SDL_Renderer* sdl2_renderer)
{
  SDL2InternalTilesetAlias_::key_type key =
      std::make_tuple(&tileset, sdl2_renderer);
  auto alias_it = sdl2_alias_pool.find(key);
  if (alias_it == sdl2_alias_pool.end()) {
    alias_it = sdl2_alias_pool.emplace(
        key,
        std::make_shared<SDL2InternalTilesetAlias_>(tileset,
                                                    sdl2_renderer)).first;
  }
  alias_ = alias_it->second;
}

} // namespace sdl2
} // namespace tcod
