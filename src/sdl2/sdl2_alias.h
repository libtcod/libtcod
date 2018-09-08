
#ifndef LIBTCOD_SDL2_SDL2_ALIAS_H_
#define LIBTCOD_SDL2_SDL2_ALIAS_H_
#include <map>
#include <tuple>

#include "sdl2_renderer.h"
#include "../tileset/observer.h"
#ifdef __cplusplus
namespace tcod {
namespace sdl2 {
using tileset::Tile;
using tileset::Tileset;
using tileset::TilesetObserver;
class SDL2TilesetAlias: public TilesetObserver {
 public:
  SDL2TilesetAlias(Tileset& tileset, struct SDL_Renderer* sdl2_renderer)
    : TilesetObserver(tileset), renderer_(sdl2_renderer) {
      observe(tileset);
    }
  static SDL2TilesetAlias& get(Tileset& tileset,
                               struct SDL_Renderer* sdl2_renderer) {
    return alias_pool_.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(&tileset, sdl2_renderer),
      std::forward_as_tuple(tileset, sdl2_renderer)).first->second;
  }
 protected:
  virtual void on_tileset_attached(const Tileset &tileset) override;
  virtual void on_tileset_changed(
      const Tileset &tileset,
      const std::vector<std::pair<int, Tile&>> &changes) override;
  virtual void on_tileset_detached() override;
 private:
  using pool_type = std::map<std::tuple<const Tileset*,
                                        const struct SDL_Renderer*>,
                             SDL2TilesetAlias>;
  Tileset* tileset_;
  struct SDL_Renderer* renderer_;
  struct SDL_Texture* texture_;
  static pool_type alias_pool_;
};
} // namespace sdl2
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_SDL2_SDL2_ALIAS_H_
