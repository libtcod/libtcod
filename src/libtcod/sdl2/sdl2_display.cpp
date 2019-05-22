/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "sdl2_display.h"

#include <cstdint>
#include <stdexcept>

#include "../console.h"
#include "../libtcod_int.h"

#include <SDL.h>

namespace tcod {
namespace sdl2 {
/**
 *  Initialize a return a new SDL2 window.
 *
 *  If the SDL_WINDOW_OPENGL flag is set then it will create an OpenGL 2.0
 *  context.
 */
static auto init_sdl2_window(
    const std::array<int, 2>& window_size,
    int window_flags,
    const std::string& title)
-> std::shared_ptr<struct SDL_Window>
{
  int width = window_size.at(0);
  int height = window_size.at(1);
  if (width < 0 || height < 0) {
    throw std::invalid_argument("width and height must be non-negative.");
  }
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER)) {
    throw std::runtime_error(SDL_GetError());
  }
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  auto window = std::shared_ptr<SDL_Window>(
      SDL_CreateWindow(
          title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
          width, height, window_flags),
      [](SDL_Window* window_ptr){ SDL_DestroyWindow(window_ptr); });
  if (!window) { throw std::runtime_error(SDL_GetError()); }
  return window;
}
WindowedDisplay::WindowedDisplay(std::shared_ptr<struct SDL_Window> window)
: window_(window)
{}
WindowedDisplay::WindowedDisplay(const std::array<int, 2>& window_size,
                                 int window_flags, const std::string& title)
: WindowedDisplay(init_sdl2_window(window_size, window_flags, title))
{}
void WindowedDisplay::set_title(const std::string& title)
{
  if (!window_) { throw std::logic_error("Unresolved class invariant."); }
  SDL_SetWindowTitle(window_.get(), title.c_str());
}
std::string WindowedDisplay::get_title()
{
  if (!window_) { throw std::logic_error("Unresolved class invariant."); }
  return std::string(SDL_GetWindowTitle(window_.get()));
}
void WindowedDisplay::set_fullscreen(bool fullscreen)
{
  if (fullscreen) {
    SDL_SetWindowFullscreen(window_.get(), SDL_WINDOW_FULLSCREEN_DESKTOP);
  } else {
    SDL_SetWindowFullscreen(window_.get(), 0);
  }
}
int WindowedDisplay::get_fullscreen()
{
  return ((SDL_GetWindowFlags(window_.get())
           & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) != 0);
}
void WindowedDisplay::update_pixel_to_tile_scale(const TCOD_Console* console)
{
  int width, height;
  SDL_GetWindowSize(window_.get(), &width, &height);
  pixel_to_tile_scale = {
    static_cast<double>(TCOD_console_get_width(console))
    / static_cast<double>(width),
    static_cast<double>(TCOD_console_get_height(console))
    / static_cast<double>(height),
  };
}

SDL2Display::SDL2Display(
    std::shared_ptr<Tileset> tileset,
    const std::array<int, 2>& window_size,
    int window_flags,
    const std::string& title,
    int renderer_flags)
: WindowedDisplay(window_size, window_flags, title)
{
  // Configure SDL2 renderer.
  renderer_flags |= SDL_RENDERER_TARGETTEXTURE;
  renderer_ = std::shared_ptr<SDL_Renderer>(
      SDL_CreateRenderer(get_sdl_window(), -1, renderer_flags),
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
void SDL2Display::accumulate(const TCOD_Console* console)
{
  accumulate(console, nullptr);
}
void SDL2Display::accumulate(const TCOD_Console* console, const struct SDL_Rect* viewport)
{
  if (!renderer_) { throw std::logic_error("Unresolved class invariant."); }
  SDL_Texture* backbuffer = tcod_renderer_.render(console);
  SDL_RenderCopy(renderer_.get(), backbuffer, nullptr, viewport);
  update_pixel_to_tile_scale(console);
}
void SDL2Display::present(const TCOD_Console* console)
{
  if (!renderer_) { throw std::logic_error("Unresolved class invariant."); }
  accumulate(console);
  SDL_RenderPresent(renderer_.get());
}
auto SDL2Display::read_pixels() const -> Image
{
  return tcod_renderer_.read_pixels();
}
} // namespace sdl2
} // namespace tcod
