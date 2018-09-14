
#ifndef LIBTCOD_SDL2_SDL2_ALIAS_H_
#define LIBTCOD_SDL2_SDL2_ALIAS_H_
#include <memory>

#include "sdl2_renderer.h"
#include "../tileset/observer.h"
#ifdef __cplusplus
struct SDL_Renderer;

namespace tcod {
namespace sdl2 {
using tileset::Tile;
using tileset::Tileset;
using tileset::TilesetObserver;
class SDL2InternalTilesetAlias_;

class SDL2TilesetAlias: public TilesetObserver {
 public:
  SDL2TilesetAlias(std::shared_ptr<Tileset>& tileset,
                   struct SDL_Renderer* sdl2_renderer);
 private:
  std::shared_ptr<SDL2InternalTilesetAlias_> alias_;
};
} // namespace sdl2
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_SDL2_SDL2_ALIAS_H_
