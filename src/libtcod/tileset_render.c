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
#include "tileset_render.h"

#ifndef NO_SDL
#include <SDL3/SDL.h>
#endif  // NO_SDL
/**
    Render a single tile.
 */
static void render_tile(
    const TCOD_Tileset* __restrict tileset,
    const struct TCOD_ConsoleTile* __restrict tile,
    struct TCOD_ColorRGBA* __restrict out_rgba,
    int stride) {
  const TCOD_ColorRGBA* graphic = TCOD_tileset_get_tile(tileset, tile->ch);
  for (int y = 0; y < tileset->tile_height; ++y) {
    TCOD_ColorRGBA* out = (TCOD_ColorRGBA*)((char*)out_rgba + stride * y);
    for (int x = 0; x < tileset->tile_width; ++x) {
      if (graphic) {
        // Multiply the foreground and tileset colors, then blend with bg.
        struct TCOD_ColorRGBA rgba = tile->bg;
        int graphic_i = y * tileset->tile_width + x;
        struct TCOD_ColorRGBA fg = {
            tile->fg.r * graphic[graphic_i].r / 255,
            tile->fg.g * graphic[graphic_i].g / 255,
            tile->fg.b * graphic[graphic_i].b / 255,
            tile->fg.a * graphic[graphic_i].a / 255,
        };
        TCOD_color_alpha_blend(&rgba, &fg);
        out[x] = rgba;
      } else {
        out[x] = tile->bg;
      }
    }
  }
}
#ifndef NO_SDL
TCOD_Error TCOD_tileset_render_to_surface(
    const TCOD_Tileset* __restrict tileset,
    const TCOD_Console* __restrict console,
    TCOD_Console* __restrict* cache,
    struct SDL_Surface* __restrict* surface_out) {
  if (!tileset) {
    TCOD_set_errorv("Tileset argument must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!console) {
    TCOD_set_errorv("Tileset argument must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!surface_out) {
    TCOD_set_errorv("Surface out argument must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  const int total_width = tileset->tile_width * console->w;
  const int total_height = tileset->tile_height * console->h;
  if (*surface_out) {
    if ((*surface_out)->w != total_width || (*surface_out)->h != total_height ||
        (*surface_out)->format != SDL_PIXELFORMAT_RGBA32) {
      SDL_DestroySurface(*surface_out);
      *surface_out = NULL;
    }
  }
  if (!*surface_out) {
    *surface_out = SDL_CreateSurface(total_width, total_height, SDL_PIXELFORMAT_RGBA32);
  }
  if (cache) {
    if (*cache) {
      if ((*cache)->w != console->w || (*cache)->h != console->h) {
        TCOD_console_delete(*cache);
        *cache = NULL;
      }
    }
    if (!*cache) {
      *cache = TCOD_console_new(console->w, console->h);
    }
  }
  for (int console_y = 0; console_y < console->h; ++console_y) {
    for (int console_x = 0; console_x < console->w; ++console_x) {
      // Get the console index and tileset graphic.
      int console_i = console_y * console->w + console_x;
      const struct TCOD_ConsoleTile* tile = &console->tiles[console_i];
      if (cache && *cache) {
        const struct TCOD_ConsoleTile* cache_tile = &(*cache)->tiles[console_i];
        if (cache_tile->ch == tile->ch && cache_tile->fg.r == tile->fg.r && cache_tile->fg.g == tile->fg.g &&
            cache_tile->fg.b == tile->fg.b && cache_tile->fg.a == tile->fg.a && cache_tile->bg.r == tile->bg.r &&
            cache_tile->bg.g == tile->bg.g && cache_tile->bg.b == tile->bg.b && cache_tile->bg.a == tile->bg.a) {
          continue;
        }
      }
      // clang-format off
      TCOD_ColorRGBA* out = (TCOD_ColorRGBA*)(
          (char*)(*surface_out)->pixels
          + console_y * tileset->tile_height * (*surface_out)->pitch
          + console_x * tileset->tile_width * sizeof(*out)
      );
      // clang-format on
      render_tile(tileset, tile, out, (*surface_out)->pitch);
    }
  }
  return TCOD_E_OK;
}
#endif  // NO_SDL
