
#ifndef LIBTCOD_SDL2_SDL2_RENDERER_H_
#define LIBTCOD_SDL2_SDL2_RENDERER_H_
#ifdef __cplusplus
#include <tuple>
#endif /* __cplusplus */

#include "sdl2_alias.h"
#include "../color/color.h"
#include "../console_types.h"
#include "../tileset/observer.h"
#include "../tileset/tileset.h"
#include "../utility/vector2.h"
#ifdef __cplusplus
struct SDL_Renderer;
struct SDL_Texture;
namespace tcod {
namespace sdl2 {
using tcod::tileset::Tileset;
class SDL2Renderer: public TilesetObserver {
 public:
  SDL2Renderer()
  {}
  SDL2Renderer(struct SDL_Renderer* renderer, SDL2TilesetAlias alias)
  : TilesetObserver(alias.get_tileset()), alias_{alias}, renderer_{renderer}
  {}

  SDL2Renderer(struct SDL_Renderer* renderer,
               std::shared_ptr<Tileset> tileset)
  : SDL2Renderer(renderer, SDL2TilesetAlias(renderer, tileset))
  {}

  ~SDL2Renderer();

  struct SDL_Texture* render(const TCOD_Console* console);
 private:
  using cache_type = Vector2<std::tuple<int, ColorRGBA, ColorRGBA>>;
  SDL2TilesetAlias alias_;
  cache_type cache_;
  struct SDL_Renderer* renderer_;
  struct SDL_Texture* texture_ = nullptr;
};
} // namespace sdl2
} // namespace tcod
#endif /* __cplusplus */
#endif /* LIBTCOD_SDL2_SDL2_RENDERER_H_ */
