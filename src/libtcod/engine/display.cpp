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
#include "display.h"

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>

#include <SDL.h>
#include "error.h"
#include "globals.h"
#include "../console.h"
#include "../console.hpp"
#include "../libtcod_int.h"
#include "../sdl2/sdl2_display.h"
#include "../sdl2/gl2_display.h"
#include "../tileset/fallback.h"
namespace tcod {
static auto ensure_tileset() -> std::shared_ptr<tcod::tileset::Tileset>
{
  using tcod::engine::get_tileset;
  using tcod::engine::set_tileset;
  using tcod::tileset::new_fallback_tileset;
  if (!get_tileset()) {
    try {
      set_tileset(new_fallback_tileset());
    }
    catch (const std::runtime_error& e) {
      throw std::runtime_error(
        std::string()
        + "Couldn't load a fallback font for the SDL2/OPENGL2 renderer: "
        + e.what());
    }
  }
  return get_tileset();
}
/**
 *  Return an environment value as a std::string.
 *
 *  The returned string will be empty if the environment value does not exist.
 */
static std::string get_env(const char* key)
{
  char* value = std::getenv(key);
  return value ? value : "";
}
/**
 *  Set `renderer` from the TCOD_RENDERER environment variable if it exists.
 */
static void get_env_renderer(TCOD_renderer_t& renderer)
{
  const std::string value(get_env("TCOD_RENDERER"));
  if (value == "sdl") {
    renderer = TCOD_RENDERER_SDL;
  } else if (value == "opengl") {
    renderer = TCOD_RENDERER_OPENGL;
  } else if (value == "glsl") {
    renderer = TCOD_RENDERER_GLSL;
  } else if (value == "sdl2") {
    renderer = TCOD_RENDERER_SDL2;
  } else if (value == "opengl2") {
    renderer = TCOD_RENDERER_OPENGL2;
  }
}
/**
 *  Set `vsync` from the TCOD_VSYNC environment variable if it exists.
 */
static void get_env_vsync(bool& vsync)
{
  const std::string value(get_env("TCOD_RENDERER"));
  if (value == "0") {
    vsync = 0;
  } else if (value == "1") {
    vsync = 1;
  }
}
/**
 *  Initialize the display using one of the new renderers.
 */
template <class T, class ...Args>
static void init_display(int w, int h, const std::string& title,
                         int fullscreen, Args... args)
{
  auto tileset = ensure_tileset();
  std::array<int, 2> display_size{tileset->get_tile_width() * w,
                                  tileset->get_tile_height() * h};
  int display_flags = (SDL_WINDOW_RESIZABLE |
                       (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
  tcod::engine::set_display(
      std::make_shared<T>(tileset, display_size, display_flags, title, args...));
}
namespace console {
void init_root(int w, int h, const std::string& title, bool fullscreen,
               TCOD_renderer_t renderer, bool vsync)
{
  if (w <= 0 || h <= 0) {
    throw std::invalid_argument("Width and height must be greater than zero.");
  }
  get_env_renderer(renderer);
  get_env_vsync(vsync);
  TCOD_console_delete(NULL);
  TCODConsole::root->data = TCOD_ctx.root = TCOD_console_new(w, h);
#ifndef TCOD_BARE
  TCOD_ctx.renderer=renderer;
#endif
  strncpy(TCOD_ctx.window_title, title.c_str(),
          sizeof(TCOD_ctx.window_title) - 1);
  TCOD_ctx.window_title[sizeof(TCOD_ctx.window_title) - 1] = '\0';
  TCOD_ctx.fullscreen = fullscreen;
  switch (renderer) {
    case TCOD_RENDERER_SDL2: {
      using tcod::sdl2::SDL2Display;
      int renderer_flags = SDL_RENDERER_PRESENTVSYNC * vsync;
      init_display<SDL2Display>(w, h, title, fullscreen, renderer_flags);
      break;
    }
    case TCOD_RENDERER_OPENGL2:
      init_display<tcod::sdl2::OpenGL2Display>(w, h, title, fullscreen, vsync);
      break;
    default:
      if(!TCOD_console_init(TCOD_ctx.root, title, fullscreen)) {
        throw std::runtime_error(TCOD_get_error());
      }
      break;
  }
}
void init_root(int w, int h, const std::string& title, bool fullscreen,
               TCOD_renderer_t renderer)
{
  init_root(w, h, title, fullscreen, renderer, false);
}
} // namespace console
} // namespace tcod
int TCOD_console_init_root_(
    int w,
    int h,
    const char* title,
    bool fullscreen,
    TCOD_renderer_t renderer,
    bool vsync)
{
  using tcod::console::init_root;
  try {
    init_root(w, h, title ? title : "", fullscreen, renderer, vsync);
  } catch (const std::exception& e) {
    return tcod::set_error(e);
  }
  return 0;
}
int TCOD_console_init_root(int w, int h, const char* title, bool fullscreen,
                           TCOD_renderer_t renderer)
{
  return TCOD_console_init_root_(w, h, title, fullscreen, renderer, false);
}
void TCOD_quit(void)
{
  TCOD_console_delete(NULL);
}
void TCOD_console_set_window_title(const char *title)
{
  if (auto display = tcod::engine::get_display()) {
    display->set_title(title);
  } else { // Deprecated renderer.
    TCOD_sys_set_window_title(title);
  }
}
void TCOD_console_set_fullscreen(bool fullscreen)
{
  if (auto display = tcod::engine::get_display()) {
    display->set_fullscreen(fullscreen);
  } else { // Deprecated renderer.
    TCOD_IFNOT(TCOD_ctx.root != NULL) { return; }
    TCOD_sys_set_fullscreen(fullscreen);
    TCOD_ctx.fullscreen = fullscreen;
  }
}
bool TCOD_console_is_fullscreen(void)
{
  if (auto display = tcod::engine::get_display()) {
    return display->get_fullscreen() == 1;
  } else { // Deprecated renderer.
    return TCOD_ctx.fullscreen;
  }
}
bool TCOD_console_has_mouse_focus(void)
{
  if (auto display = tcod::engine::get_display()) {
    if (auto window = display->get_sdl_window()) {
      return (SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS) != 0;
    }
  }
  return TCOD_ctx.app_has_mouse_focus;
}
bool TCOD_console_is_active(void)
{
  if (auto display = tcod::engine::get_display()) {
    if (auto window = display->get_sdl_window()) {
      return (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
    }
  }
  return TCOD_ctx.app_is_active;
}
bool TCOD_console_is_window_closed(void) {
	return TCOD_ctx.is_window_closed;
}
struct SDL_Window* TCOD_sys_get_sdl_window(void)
{
  if (auto display = tcod::engine::get_display()) {
    return display->get_sdl_window();
  }
  return TCOD_sys_get_sdl_window_();
}
struct SDL_Renderer* TCOD_sys_get_sdl_renderer(void)
{
  if (auto display = tcod::engine::get_display()) {
    return display->get_sdl_renderer();
  }
  return TCOD_sys_get_sdl_renderer_();
}
int TCOD_sys_accumulate_console(const TCOD_Console* console)
{
  return TCOD_sys_accumulate_console_(console, nullptr);
}
int TCOD_sys_accumulate_console_(const TCOD_Console* console, const struct SDL_Rect* viewport)
{
  console = tcod::console::validate_(console);
  auto display = tcod::engine::get_display();
  if (!console || !display) { return -1; }
  display->accumulate(console, viewport);
  return 0;
}
int TCOD_sys_init_sdl2_renderer_(
    int width,
    int height,
    const char* title,
    int window_flags,
    int renderer_flags)
{
  using tcod::sdl2::SDL2Display;
  try {
    auto tileset = tcod::ensure_tileset();
    std::array<int, 2> window_size{ width, height };
    auto display = std::make_shared<SDL2Display>(
      tileset, window_size, window_flags, title, renderer_flags);
    tcod::engine::set_display(display);
    TCOD_ctx.renderer = TCOD_RENDERER_SDL2;
  } catch (const std::exception& e) {
    return tcod::set_error(e);
  }
  return 0;
}
