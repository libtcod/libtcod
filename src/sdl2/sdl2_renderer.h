
#ifndef LIBTCOD_SDL2_SDL2_RENDERER_H_
#define LIBTCOD_SDL2_SDL2_RENDERER_H_
#include "../tileset/observer.h"
#ifdef __cplusplus
struct SDL_Renderer;
struct SDL_Texture;
namespace tcod {
namespace sdl2 {
class SDL2Renderer {
 public:
  SDL2Renderer(struct SDL_Renderer* sdl2_renderer);
 private:
  struct SDL_Renderer* renderer_;
  struct SDL_Texture* texture_;
};
} // namespace sdl2
} // namespace tcod
#endif /* __cplusplus */
#endif /* LIBTCOD_SDL2_SDL2_RENDERER_H_ */
