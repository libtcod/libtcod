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
#include "context_init.h"

#include <SDL.h>

#include "console.h"
#include "console_etc.h"
#include "globals.h"
#include "libtcod_int.h"
#include "renderer_gl1.h"
#include "renderer_gl2.h"
#include "renderer_sdl2.h"
#include "stdlib.h"
#include "string.h"
#include "tileset_fallback.h"

static TCOD_Error ensure_tileset(TCOD_Tileset** tileset) {
  if (*tileset) {
    return TCOD_E_OK;
  }
  if (!TCOD_ctx.tileset) {
    TCOD_console_set_custom_font("terminal.png", TCOD_FONT_LAYOUT_ASCII_INCOL, 0, 0);
  }
  if (!TCOD_ctx.tileset) {
    TCOD_set_default_tileset(TCOD_tileset_load_fallback_font_(0, 12));
  }
  if (!TCOD_ctx.tileset) {
    TCOD_set_errorv("No font loaded and couldn't load a fallback font!");
    return TCOD_E_ERROR;
  }
  *tileset = TCOD_ctx.tileset;
  return TCOD_E_OK;
}
/**
    Return the renderer from a string object.  Returns -1 on failure.
 */
static int get_renderer_from_str(const char* string) {
  if (!string) {
    return -1;
  } else if (strcmp(string, "sdl") == 0) {
    return TCOD_RENDERER_SDL;
  } else if (strcmp(string, "opengl") == 0) {
    return TCOD_RENDERER_OPENGL;
  } else if (strcmp(string, "glsl") == 0) {
    return TCOD_RENDERER_GLSL;
  } else if (strcmp(string, "sdl2") == 0) {
    return TCOD_RENDERER_SDL2;
  } else if (strcmp(string, "opengl2") == 0) {
    return TCOD_RENDERER_OPENGL2;
  } else {
    return -1;
  }
}
/**
 *  Set `renderer` from the TCOD_RENDERER environment variable if it exists.
 */
static void get_env_renderer(int* renderer_type) {
  const char* value = getenv("TCOD_RENDERER");
  if (get_renderer_from_str(value) >= 0) {
    *renderer_type = get_renderer_from_str(value);
  }
}
/**
 *  Set `vsync` from the TCOD_VSYNC environment variable if it exists.
 */
static void get_env_vsync(bool* vsync) {
  const char* value = getenv("TCOD_VSYNC");
  if (!value) {
    return;
  }
  if (strcmp(value, "0") == 0) {
    *vsync = 0;
  } else if (strcmp(value, "1") == 0) {
    *vsync = 1;
  }
}
TCOD_Error TCOD_context_new_terminal(
    int columns,
    int rows,
    int renderer_type,
    TCOD_Tileset* tileset,
    bool vsync,
    int sdl_window_flags,
    const char* window_title,
    TCOD_Context** out) {
  get_env_renderer(&renderer_type);
  get_env_vsync(&vsync);
  TCOD_Error err = ensure_tileset(&tileset);
  if (err < 0) {
    return err;
  }
  int width = columns * tileset->tile_width;
  int height = rows * tileset->tile_height;
  return TCOD_context_new_window(
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      width,
      height,
      renderer_type,
      tileset,
      vsync,
      sdl_window_flags,
      window_title,
      out);
}
TCOD_Error TCOD_context_new_window(
    int x,
    int y,
    int pixel_width,
    int pixel_height,
    int renderer_type,
    TCOD_Tileset* tileset,
    bool vsync,
    int sdl_window_flags,
    const char* window_title,
    TCOD_Context** out) {
  if (!out) {
    TCOD_set_errorv("Output parameter is NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (pixel_width < 0 || pixel_height < 0) {
    TCOD_set_errorvf("Width and height must be non-negative. Not %i,%i", pixel_width, pixel_height);
    return TCOD_E_INVALID_ARGUMENT;
  }
  get_env_renderer(&renderer_type);
  get_env_vsync(&vsync);
  TCOD_Error err = ensure_tileset(&tileset);
  if (err < 0) {
    return err;
  }
  int renderer_flags = SDL_RENDERER_PRESENTVSYNC * vsync;
  err = TCOD_E_OK;
  switch (renderer_type) {
    case TCOD_RENDERER_SDL:
      renderer_flags |= SDL_RENDERER_SOFTWARE;
      *out = TCOD_renderer_init_sdl2(
          x, y, pixel_width, pixel_height, window_title, sdl_window_flags, renderer_flags, tileset);
      if (!*out) {
        return TCOD_E_ERROR;
      }
      return TCOD_E_OK;
    case TCOD_RENDERER_GLSL:
    case TCOD_RENDERER_OPENGL2:
      *out = TCOD_renderer_new_gl2(x, y, pixel_width, pixel_height, window_title, sdl_window_flags, vsync, tileset);
      if (*out) {
        return err;
      }
      err = TCOD_E_WARN;
      //@fallthrough@
    case TCOD_RENDERER_OPENGL:
      *out = TCOD_renderer_init_gl1(x, y, pixel_width, pixel_height, window_title, sdl_window_flags, vsync, tileset);
      if (*out) {
        return err;
      }
      err = TCOD_E_WARN;
      //@fallthrough@
    default:
    case TCOD_RENDERER_SDL2:
      *out = TCOD_renderer_init_sdl2(
          x, y, pixel_width, pixel_height, window_title, sdl_window_flags, renderer_flags, tileset);
      if (!*out) {
        return TCOD_E_ERROR;
      }
      return err;
  }
}
