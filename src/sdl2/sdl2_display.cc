
#include "sdl2_display.h"

#include <stdexcept>

#include "../console.h"
#include "../libtcod_int.h"

#include <SDL.h>

namespace tcod {
namespace sdl2 {
SDL2Display::SDL2Display(int width, int height,
                         std::shared_ptr<Tileset> tileset, int fullscreen)
{
  if (width < 0 || height < 0) {
    throw std::invalid_argument("width and height must be non-negative.");
  }
  if (!tileset) {
    throw std::invalid_argument("tileset must not be nullptr.");
  }
  int flags = SDL_WINDOW_RESIZABLE;
  if (fullscreen) {
    flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  }
  if (width == 0 && height == 0) {
    flags |= SDL_WINDOW_MAXIMIZED;
  } else {
    width *= tileset->get_tile_width();
    height *= tileset->get_tile_height();
  }
  if (SDL_CreateWindowAndRenderer(width, height, flags,
                                  &window_, &renderer_)) {
    throw std::runtime_error(SDL_GetError());
  }
  tcod_renderer_ = SDL2Renderer(renderer_, tileset);
}
SDL2Display::~SDL2Display()
{
  if (renderer_) { SDL_DestroyRenderer(renderer_); }
  if (window_) { SDL_DestroyWindow(window_); }
}
void SDL2Display::set_tileset(std::shared_ptr<Tileset> tileset)
{
  if (!renderer_) { throw std::logic_error("Unresolved class invariant."); }
  tcod_renderer_ = SDL2Renderer(renderer_, tileset);
}
void SDL2Display::set_title(const std::string title)
{
  if (!window_) { throw std::logic_error("Unresolved class invariant."); }
  SDL_SetWindowTitle(window_, title.c_str());
}
void SDL2Display::present(const TCOD_Console* console)
{
  if (!renderer_) { throw std::logic_error("Unresolved class invariant."); }
  SDL_Texture* backbuffer = tcod_renderer_.render(console);
  SDL_RenderCopy(renderer_, backbuffer, nullptr, nullptr);
  SDL_RenderPresent(renderer_);
}
} // namespace sdl2
} // namespace tcod
