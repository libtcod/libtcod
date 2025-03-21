/* BSD 3-Clause License
 *
 * Copyright © 2008-2025, Jice and the libtcod contributors.
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
#include "console_init.h"

#ifndef NO_SDL
#include <SDL3/SDL.h>
#endif  // NO_SDL
#include <stdbool.h>
#include <string.h>

#include "console.h"
#include "console_etc.h"
#include "context_init.h"
#include "libtcod_int.h"

#ifndef NO_SDL
TCOD_Error TCOD_console_init_root_(
    int w, int h, const char* title, bool fullscreen, TCOD_renderer_t renderer, bool vsync) {
  if (w < 0 || h < 0) {
    TCOD_set_errorvf("Width and height must be non-negative. Not %i,%i", w, h);
    return TCOD_E_INVALID_ARGUMENT;
  }
  TCOD_Error err = TCOD_sys_load_player_config();
  if (err < 0) {
    return err;
  }
  TCOD_console_delete(NULL);
  TCOD_ctx.root = TCOD_console_new(w, h);
  if (!TCOD_ctx.root) {
    return TCOD_E_ERROR;
  }
  strncpy(TCOD_ctx.window_title, title ? title : "", sizeof(TCOD_ctx.window_title) - 1);
  TCOD_ctx.fullscreen = fullscreen;
  struct TCOD_ContextParams params = {
      .tcod_version = 0,
      .window_x = SDL_WINDOWPOS_UNDEFINED,
      .window_y = SDL_WINDOWPOS_UNDEFINED,
      .columns = w,
      .rows = h,
      .vsync = vsync,
      .sdl_window_flags = SDL_WINDOW_RESIZABLE | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0),
      .window_title = title,
      .renderer_type = renderer,
      .window_xy_defined = true,
  };
  return TCOD_context_new(&params, &TCOD_ctx.engine);
}
TCOD_Error TCOD_console_init_root(int w, int h, const char* title, bool fullscreen, TCOD_renderer_t renderer) {
  const bool vsync = renderer != TCOD_RENDERER_SDL;  // Enable VSync for all except the software renderer.
  return TCOD_console_init_root_(w, h, title, fullscreen, renderer, vsync);
}
void TCOD_console_set_window_title(const char* title) {
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  SDL_SetWindowTitle(window, title);
  strncpy(TCOD_ctx.window_title, title, sizeof(TCOD_ctx.window_title) - 1);
}
void TCOD_console_set_fullscreen(bool fullscreen) {
  TCOD_ctx.fullscreen = fullscreen;
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  if (window) {
    SDL_SetWindowFullscreen(window, fullscreen);
  }
}
bool TCOD_console_is_fullscreen(void) {
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  if (!window) {
    return TCOD_ctx.fullscreen;
  }
  return (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) != 0;
}
bool TCOD_console_has_mouse_focus(void) {
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  if (!window) {
    return TCOD_ctx.app_has_mouse_focus;
  }
  return (SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS) != 0;
}
bool TCOD_console_is_active(void) {
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  if (!window) {
    return TCOD_ctx.app_is_active;
  }
  return (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
}
bool TCOD_console_is_window_closed(void) { return TCOD_ctx.is_window_closed; }
struct SDL_Window* TCOD_sys_get_sdl_window(void) {
  if (TCOD_ctx.engine && TCOD_ctx.engine->c_get_sdl_window_) {
    return TCOD_ctx.engine->c_get_sdl_window_(TCOD_ctx.engine);
  }
  return NULL;
}
struct SDL_Renderer* TCOD_sys_get_sdl_renderer(void) {
  if (TCOD_ctx.engine && TCOD_ctx.engine->c_get_sdl_renderer_) {
    return TCOD_ctx.engine->c_get_sdl_renderer_(TCOD_ctx.engine);
  }
  return NULL;
}
#endif  // NO_SDL
int TCOD_sys_accumulate_console(const TCOD_Console* console) { return TCOD_sys_accumulate_console_(console, NULL); }
int TCOD_sys_accumulate_console_(const TCOD_Console* console, const struct SDL_Rect* viewport) {
  (void)viewport;  // Ignored parameter.
  console = TCOD_console_validate_(console);
  if (!console) {
    return -1;
  }
  if (TCOD_ctx.engine && TCOD_ctx.engine->c_accumulate_) {
    return TCOD_ctx.engine->c_accumulate_(TCOD_ctx.engine, console, NULL);
  }
  return -1;
}
TCOD_Context* TCOD_sys_get_internal_context(void) { return TCOD_ctx.engine; }
TCOD_Console* TCOD_sys_get_internal_console(void) { return TCOD_ctx.root; }
void TCOD_quit(void) { TCOD_console_delete(NULL); }
