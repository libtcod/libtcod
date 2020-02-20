/* BSD 3-Clause License
 *
 * Copyright © 2008-2020, Jice and the libtcod contributors.
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

#include "stdbool.h"
#include "stdlib.h"
#include "string.h"

#include <SDL.h>
#include "console.h"
#include "console_etc.h"
#include "libtcod_int.h"
#include "tileset_fallback.h"
#include "renderer_sdl2.h"
#include "renderer_gl1.h"
#include "renderer_gl2.h"
static struct TCOD_Tileset* ensure_tileset(void)
{
  if (!TCOD_ctx.tileset) {
    TCOD_console_set_custom_font(
        "terminal.png", TCOD_FONT_LAYOUT_ASCII_INCOL, 0, 0);
  }
  if (!TCOD_ctx.tileset) {
    TCOD_ctx.tileset = TCOD_tileset_load_fallback_font_(0, 12);
  }
  if (!TCOD_ctx.tileset) {
    TCOD_set_errorv("No font loaded and couldn't load a fallback font!");
    return NULL;
  }
  return TCOD_ctx.tileset;
}
/**
 *  Set `renderer` from the TCOD_RENDERER environment variable if it exists.
 */
static void get_env_renderer(TCOD_renderer_t* renderer)
{
  const char* value = getenv("TCOD_RENDERER");
  if (!value) { return; }
  if (strcmp(value, "sdl")) {
    *renderer = TCOD_RENDERER_SDL;
  } else if (strcmp(value, "opengl")) {
    *renderer = TCOD_RENDERER_OPENGL;
  } else if (strcmp(value, "glsl")) {
    *renderer = TCOD_RENDERER_GLSL;
  } else if (strcmp(value, "sdl2")) {
    *renderer = TCOD_RENDERER_SDL2;
  } else if (strcmp(value, "opengl2")) {
    *renderer = TCOD_RENDERER_OPENGL2;
  }
}
/**
 *  Set `vsync` from the TCOD_VSYNC environment variable if it exists.
 */
static void get_env_vsync(bool* vsync)
{
  const char* value = getenv("TCOD_RENDERER");
  if (!value) { return; }
  if (strcmp(value, "0")) {
    *vsync = 0;
  } else if (strcmp(value, "1")) {
    *vsync = 1;
  }
}
TCOD_Error TCOD_console_init_root_(
    int w,
    int h,
    const char* title,
    bool fullscreen,
    TCOD_renderer_t renderer,
    bool vsync)
{
  if (w < 0 || h < 0) {
    TCOD_set_errorvf("Width and height must be non-negative. Not %i,%i",
                     w, h);
    return TCOD_E_INVALID_ARGUMENT;
  }
  TCOD_Error err = TCOD_sys_load_player_config();
  if (err < 0) { return err; }
  get_env_renderer(&renderer);
  get_env_vsync(&vsync);
  TCOD_console_delete(NULL);
  TCOD_ctx.root = TCOD_console_new(w, h);
  if (!TCOD_ctx.root) { return TCOD_E_ERROR; }
  TCOD_ctx.renderer = renderer;
  strncpy(TCOD_ctx.window_title, title ? title : "",
          sizeof(TCOD_ctx.window_title) - 1);
  TCOD_ctx.fullscreen = fullscreen;
  struct TCOD_Tileset* tileset = ensure_tileset();
  if (!tileset) { return TCOD_E_ERROR; }
  int renderer_flags = SDL_RENDERER_PRESENTVSYNC * vsync;
  int window_flags = (SDL_WINDOW_RESIZABLE |
                      (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
  switch (renderer) {
    case TCOD_RENDERER_SDL:
      renderer_flags |= SDL_RENDERER_SOFTWARE;
      TCOD_ctx.engine = TCOD_renderer_init_sdl2(
          w * tileset->tile_width, h * tileset->tile_height,
          title, window_flags, renderer_flags, tileset);
      if (!TCOD_ctx.engine) {
        return TCOD_E_ERROR;
      }
      break;
    case TCOD_RENDERER_GLSL:
    case TCOD_RENDERER_OPENGL2:
      TCOD_ctx.renderer = TCOD_RENDERER_OPENGL2;
      TCOD_ctx.engine = TCOD_renderer_new_gl2(
          w * tileset->tile_width, h * tileset->tile_height,
          title, window_flags, vsync, tileset);
      if (TCOD_ctx.engine) { break; }
      err = TCOD_E_WARN;
      //@fallthrough@
    case TCOD_RENDERER_OPENGL:
      TCOD_ctx.renderer = TCOD_RENDERER_OPENGL;
      TCOD_ctx.engine = TCOD_renderer_init_gl1(
          w * tileset->tile_width, h * tileset->tile_height,
          title, window_flags, vsync, tileset);
      if (TCOD_ctx.engine) { break; }
      err = TCOD_E_WARN;
      //@fallthrough@
    case TCOD_RENDERER_SDL2:
      TCOD_ctx.renderer = TCOD_RENDERER_SDL2;
      TCOD_ctx.engine = TCOD_renderer_init_sdl2(
          w * tileset->tile_width, h * tileset->tile_height,
          title, window_flags, renderer_flags, tileset);
      if (!TCOD_ctx.engine) {
        return TCOD_E_ERROR;
      }
      break;
    default:
      return TCOD_console_init(TCOD_ctx.root, title, fullscreen);
  }
  return err;
}
TCOD_Error TCOD_console_init_root(int w, int h, const char* title,
                                  bool fullscreen, TCOD_renderer_t renderer)
{
  return TCOD_console_init_root_(w, h, title, fullscreen, renderer, false);
}
void TCOD_quit(void)
{
  TCOD_console_delete(NULL);
}
void TCOD_console_set_window_title(const char *title)
{
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  SDL_SetWindowTitle(window, title);
  strncpy(TCOD_ctx.window_title, title, sizeof(TCOD_ctx.window_title) - 1);
}
void TCOD_console_set_fullscreen(bool fullscreen)
{
  TCOD_ctx.fullscreen = fullscreen;
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  if (window) {
    SDL_SetWindowFullscreen(
        window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
  }
}
bool TCOD_console_is_fullscreen(void)
{
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  if (!window) { return TCOD_ctx.fullscreen; }
  return (SDL_GetWindowFlags(window)
          & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) != 0;
}
bool TCOD_console_has_mouse_focus(void)
{
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  if (!window) { return TCOD_ctx.app_has_mouse_focus; }
  return (SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS) != 0;
}
bool TCOD_console_is_active(void)
{
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  if (!window) { return TCOD_ctx.app_is_active; }
  return (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
}
bool TCOD_console_is_window_closed(void) {
	return TCOD_ctx.is_window_closed;
}
struct SDL_Window* TCOD_sys_get_sdl_window(void)
{
  if (TCOD_ctx.engine && TCOD_ctx.engine->get_sdl_window_) {
    return TCOD_ctx.engine->get_sdl_window_(TCOD_ctx.engine);
  }
  return NULL;
}
struct SDL_Renderer* TCOD_sys_get_sdl_renderer(void)
{
  if (TCOD_ctx.engine && TCOD_ctx.engine->get_sdl_renderer_) {
    return TCOD_ctx.engine->get_sdl_renderer_(TCOD_ctx.engine);
  }
  return NULL;
}
int TCOD_sys_accumulate_console(const TCOD_Console* console)
{
  return TCOD_sys_accumulate_console_(console, NULL);
}
int TCOD_sys_accumulate_console_(const TCOD_Console* console, const struct SDL_Rect* viewport)
{
  (void)viewport; // Ignored parameter.
  console = TCOD_console_validate_(console);
  if (!console) { return -1; }
  if (TCOD_ctx.engine && TCOD_ctx.engine->accumulate_) {
    return TCOD_ctx.engine->accumulate_(TCOD_ctx.engine, console, NULL);
  }
  return -1;
}
