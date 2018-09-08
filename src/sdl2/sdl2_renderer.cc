
#include "sdl2_renderer.h"

#include <map>

#include <SDL.h>
namespace tcod {
namespace sdl2 {
using tileset::Tileset;
SDL2Renderer::SDL2Renderer(struct SDL_Renderer* sdl2_renderer) {
  renderer_ = sdl2_renderer;
}
} // namespace sdl2
} // namespace tcod
