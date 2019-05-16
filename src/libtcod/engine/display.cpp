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
/**
 *  Initialize the display using one of the new renderers.
 */
template <class T>
static void init_display(int w, int h, const std::string& title,
                         int fullscreen)
{
  auto tileset = tcod::engine::get_tileset();
  if (!tileset) {
    try { // Try to load a fall-back Tileset, but ignore failure.
      tileset = tcod::tileset::new_fallback_tileset();
      tcod::engine::set_tileset(tileset);
    } catch (const std::runtime_error& e) {
      throw std::runtime_error(
          "Couldn't load a fallback font for the SDL2/OPENGL2 renderer.");
    }
  }
  std::array<int, 2> display_size{tileset->get_tile_width() * w,
                                  tileset->get_tile_height() * h};
  int display_flags = (SDL_WINDOW_RESIZABLE |
                       (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
  tcod::engine::set_display(
      std::make_shared<T>(tileset, display_size, display_flags, title));
}
namespace console {
void init_root(int w, int h, const std::string& title, bool fullscreen,
               TCOD_renderer_t renderer)
{
  if (w <= 0 || h <= 0) {
    throw std::invalid_argument("Width and height must be greater than zero.");
  }
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
    case TCOD_RENDERER_SDL2:
      init_display<tcod::sdl2::SDL2Display>(w, h, title, fullscreen);
      break;
    case TCOD_RENDERER_OPENGL2:
      init_display<tcod::sdl2::OpenGL2Display>(w, h, title, fullscreen);
      break;
    default:
      if(!TCOD_console_init(TCOD_ctx.root, title, fullscreen)) {
        throw std::runtime_error(TCOD_get_error());
      }
      break;
  }
}
} // namespace console
} // namespace tcod
int TCOD_console_init_root(int w, int h, const char* title, bool fullscreen,
                           TCOD_renderer_t renderer)
{
  try {
    tcod::console::init_root(w, h, title ? title : "", fullscreen, renderer);
  } catch (const std::exception& e) {
    return tcod::set_error(e);
  }
  return 0;
}
void TCOD_quit(void)
{
  TCOD_console_delete(NULL);
}
void TCOD_console_set_window_title(const char *title)
{
  auto display = tcod::engine::get_display();
  if (display) {
    display->set_title(title);
  } else {
    TCOD_sys_set_window_title(title);
  }
}
void TCOD_console_set_fullscreen(bool fullscreen)
{
  auto display = tcod::engine::get_display();
  if (display) {
    display->set_fullscreen(fullscreen);
  } else {
    TCOD_IFNOT(TCOD_ctx.root != NULL) { return; }
    TCOD_sys_set_fullscreen(fullscreen);
    TCOD_ctx.fullscreen = fullscreen;
  }
}
bool TCOD_console_is_fullscreen(void)
{
  auto display = tcod::engine::get_display();
  if (display) {
    return display->get_fullscreen() == 1;
  } else {
    return TCOD_ctx.fullscreen;
  }
}
bool TCOD_console_has_mouse_focus(void)
{
  auto display = tcod::engine::get_display();
  if (display) {
    auto window = display->get_sdl_window();
    if (window) {
      return (SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS) != 0;
    }
  }
  return TCOD_ctx.app_has_mouse_focus;
}
bool TCOD_console_is_active(void)
{
  auto display = tcod::engine::get_display();
  if (display) {
    auto window = display->get_sdl_window();
    if (window) {
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
  auto display = tcod::engine::get_display();
  if (display) {
    return display->get_sdl_window();
  }
  return TCOD_sys_get_sdl_window_();
}
struct SDL_Renderer* TCOD_sys_get_sdl_renderer(void)
{
  auto display = tcod::engine::get_display();
  if (display) {
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
