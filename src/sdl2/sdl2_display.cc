
#include "sdl2_display.h"

#include <cstdint>
#include <stdexcept>

#include "../console.h"
#include "../libtcod_int.h"

#include <SDL.h>

namespace tcod {
namespace sdl2 {
WindowedDisplay::WindowedDisplay(std::pair<int, int> window_size,
                                 int window_flags, const std::string& title)
{
  int width = window_size.first;
  int height = window_size.second;
  if (width < 0 || height < 0) {
    throw std::invalid_argument("width and height must be non-negative.");
  }
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER)) {
    throw std::runtime_error(SDL_GetError());
  }
  window_ = std::shared_ptr<SDL_Window>(
      SDL_CreateWindow(
          title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
          width, height, window_flags),
      [](SDL_Window* window){ SDL_DestroyWindow(window); });
  if (!window_) { throw std::runtime_error(SDL_GetError()); }
}
void WindowedDisplay::set_title(const std::string title)
{
  if (!window_) { throw std::logic_error("Unresolved class invariant."); }
  SDL_SetWindowTitle(window_.get(), title.c_str());
}
std::string WindowedDisplay::get_title()
{
  if (!window_) { throw std::logic_error("Unresolved class invariant."); }
  return std::string(SDL_GetWindowTitle(window_.get()));
}

SDL2Display::SDL2Display(std::shared_ptr<Tileset> tileset,
                         std::pair<int, int> window_size, int window_flags,
                         const std::string& title)
: WindowedDisplay(window_size, window_flags, title)
{
  // Configure SDL2 renderer.
  renderer_ = std::shared_ptr<SDL_Renderer>(
      SDL_CreateRenderer(get_window(), -1, SDL_RENDERER_TARGETTEXTURE),
      [](SDL_Renderer* renderer){ SDL_DestroyRenderer(renderer); });
  if (!renderer_) { throw std::runtime_error(SDL_GetError()); }
  // Configure libtcod renderer.
  set_tileset(tileset);
}
void SDL2Display::set_tileset(std::shared_ptr<Tileset> tileset)
{
  if (!renderer_) { throw std::logic_error("Unresolved class invariant."); }
  if (!tileset) {
    throw std::invalid_argument("tileset must not be nullptr.");
  }
  tcod_renderer_ = SDL2Renderer(renderer_.get(), tileset);
}
void SDL2Display::present(const TCOD_Console* console)
{
  if (!renderer_) { throw std::logic_error("Unresolved class invariant."); }
  SDL_Texture* backbuffer = tcod_renderer_.render(console);
  SDL_RenderClear(renderer_.get());
  SDL_RenderCopy(renderer_.get(), backbuffer, nullptr, nullptr);
  SDL_RenderPresent(renderer_.get());
}
} // namespace sdl2
} // namespace tcod
