
#ifndef LIBTCOD_SDL2_SDL2_ALIAS_H_
#define LIBTCOD_SDL2_SDL2_ALIAS_H_
#include <memory>

#include "../tileset/observer.h"
#ifdef __cplusplus
struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Rect;

namespace tcod {
namespace sdl2 {
using tileset::Tile;
using tileset::Tileset;
using tileset::TilesetObserver;
class SDL2InternalTilesetAlias_;

class SDL2TilesetAlias {
 public:
  SDL2TilesetAlias(struct SDL_Renderer* renderer,
                   std::shared_ptr<Tileset> tileset);

  std::shared_ptr<Tileset>& get_tileset();

  SDL_Texture*& get_texture_alias();
  SDL_Rect get_char_rect(int codepoint);
 private:
  std::shared_ptr<SDL2InternalTilesetAlias_> alias_;
};
} // namespace sdl2
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_SDL2_SDL2_ALIAS_H_
