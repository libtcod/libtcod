
#include "sdl2_display.h"

#include <cstdint>
#include <stdexcept>

#include "../console.h"
#include "../libtcod_int.h"

#include <SDL.h>

namespace tcod {
namespace sdl2 {
WindowedDisplay::WindowedDisplay(std::pair<int, int> window_size,
                                 int window_flags)
{
  int width = window_size.first;
  int height = window_size.second;
  if (width < 0 || height < 0) {
    throw std::invalid_argument("width and height must be non-negative.");
  }
  if (SDL_Init(SDL_INIT_VIDEO)) { throw std::runtime_error(SDL_GetError()); }
  window_ = SDL_CreateWindow(
      nullptr, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      width, height, window_flags);
  if (!window_) { throw std::runtime_error(SDL_GetError()); }
}
WindowedDisplay::~WindowedDisplay()
{
  if (window_) { SDL_DestroyWindow(window_); }
}
void WindowedDisplay::set_title(const std::string title)
{
  if (!window_) { throw std::logic_error("Unresolved class invariant."); }
  SDL_SetWindowTitle(window_, title.c_str());
}
std::string WindowedDisplay::get_title()
{
  if (!window_) { throw std::logic_error("Unresolved class invariant."); }
  return std::string(SDL_GetWindowTitle(window_));
}

SDL2Display::SDL2Display(std::shared_ptr<Tileset> tileset,
                         std::pair<int, int> window_size, int window_flags)
: WindowedDisplay(window_size, window_flags)
{
  // Configure SDL2 renderer.
  renderer_ = SDL_CreateRenderer(get_window(), -1, SDL_RENDERER_TARGETTEXTURE);
  if (!renderer_) { throw std::runtime_error(SDL_GetError()); }
  // Configure libtcod renderer.
  set_tileset(tileset);
}
SDL2Display::~SDL2Display()
{
  if (renderer_) { SDL_DestroyRenderer(renderer_); }
}
void SDL2Display::set_tileset(std::shared_ptr<Tileset> tileset)
{
  if (!renderer_) { throw std::logic_error("Unresolved class invariant."); }
  if (!tileset) {
    throw std::invalid_argument("tileset must not be nullptr.");
  }
  tcod_renderer_ = SDL2Renderer(renderer_, tileset);
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
