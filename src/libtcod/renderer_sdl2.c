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
#include "renderer_sdl2.h"
#ifndef NO_SDL
#include <SDL3/SDL.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "libtcod_int.h"
#include "logging.h"

#define BUFFER_TILES_MAX 10922  // Max number of tiles to buffer. (65536 / 6) to fit indices in a uint16_t type.
/// Vertex element with position and color data.  Position uses pixel coordinates.
typedef struct VertexElement {
  float x;
  float y;
  SDL_FColor rgba;
} VertexElement;
/// Vertex normalize UV coords.
typedef struct VertexUV {
  float u;
  float v;
} VertexUV;
/// A fixed-size dynamic buffer for vertex data.
/// Vertices are ordered: upper-left, lower-left, upper-right, lower-right.
typedef struct VertexBuffer {
  int16_t index;  // Next tile to assign to.  Groups indicies in sets of 6 and vertices in sets of 4.
  int16_t indices_initialized;  // Sets of indicies initialized.
  uint16_t indices[BUFFER_TILES_MAX * 6];  // Vertex indices.  Vertex quads are assigned as: 0 1 2, 2 1 3.
  VertexElement vertex[BUFFER_TILES_MAX * 4];
  VertexUV vertex_uv[BUFFER_TILES_MAX * 4];
} VertexBuffer;

static inline float minf(float a, float b) { return a < b ? a : b; }
static inline float maxf(float a, float b) { return a > b ? a : b; }
static inline float clampf(float v, float low, float high) { return maxf(low, minf(v, high)); }
// ----------------------------------------------------------------------------
// SDL2 Atlas
/**
 *  Return a rectangle shaped for a tile at `x`,`y`.
 */
static SDL_Rect get_aligned_tile(const struct TCOD_Tileset* __restrict tileset, int x, int y) {
  SDL_Rect tile_rect = {x * tileset->tile_width, y * tileset->tile_height, tileset->tile_width, tileset->tile_height};
  return tile_rect;
}
/// Return the rectangle for the tile at `tile_id`.
static SDL_Rect get_sdl2_atlas_tile(const struct TCOD_TilesetAtlasSDL2* __restrict atlas, int tile_id) {
  return get_aligned_tile(atlas->tileset, tile_id % atlas->texture_columns, tile_id / atlas->texture_columns);
}
/**
 *  Upload a single tile to the atlas texture.
 */
static int update_sdl2_tile(struct TCOD_TilesetAtlasSDL2* __restrict atlas, int tile_id) {
  const SDL_Rect dest = get_sdl2_atlas_tile(atlas, tile_id);
  return SDL_UpdateTexture(
      atlas->texture,
      &dest,
      atlas->tileset->pixels + (tile_id * atlas->tileset->tile_length),
      atlas->tileset->tile_width * sizeof(*atlas->tileset->pixels));
}
/**
 *  Setup a atlas texture and upload the tileset graphics.
 */
static int prepare_sdl2_atlas(struct TCOD_TilesetAtlasSDL2* atlas) {
  if (!atlas) {
    return -1;
  }
  int current_size = 0;
  if (atlas->texture) {
    float current_size_f = 0;
    SDL_GetTextureSize(atlas->texture, &current_size_f, NULL);
    current_size = (int)current_size_f;
  }
  int new_size = current_size ? current_size : 256;
  int columns = 1;  // Must be more than zero.
  int rows = 1;
  while (1) {
    if (atlas->tileset->tile_width == 0 || atlas->tileset->tile_height == 0) {
      break;  // Avoid division by zero.
    }
    columns = new_size / atlas->tileset->tile_width;
    rows = new_size / atlas->tileset->tile_height;
    if (rows * columns >= atlas->tileset->tiles_capacity) {
      break;
    }
    new_size *= 2;
  }
  if (new_size != current_size) {
    if (atlas->texture) {
      SDL_DestroyTexture(atlas->texture);
    }
    TCOD_log_debug_f("Creating tileset atlas of pixel size %dx%d.", new_size, new_size);
    atlas->texture =
        SDL_CreateTexture(atlas->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, new_size, new_size);
    if (!atlas->texture) {
      return -1;
    }
    atlas->texture_columns = columns;
    for (int i = 0; i < atlas->tileset->tiles_count; ++i) {
      if (update_sdl2_tile(atlas, i) < 0) {
        return -1;  // Issue with SDL_UpdateTexture.
      }
    }
    return 1;  // Atlas texture has been resized and refreshed.
  }
  return 0;  // No action.
}
/**
 *  Respond to changes in a tileset.
 */
static int sdl2_atlas_on_tile_changed(struct TCOD_TilesetObserver* observer, int tile_id) {
  struct TCOD_TilesetAtlasSDL2* atlas = observer->userdata;
  if (prepare_sdl2_atlas(atlas) == 1) {
    return 0;  // Tile updated as a side-effect of prepare_sdl2_atlas.
  }
  return update_sdl2_tile(atlas, tile_id);
}
struct TCOD_TilesetAtlasSDL2* TCOD_sdl2_atlas_new(struct SDL_Renderer* renderer, struct TCOD_Tileset* tileset) {
  if (!renderer || !tileset) {
    return NULL;
  }
  struct TCOD_TilesetAtlasSDL2* atlas = calloc(1, sizeof(*atlas));
  if (!atlas) {
    return NULL;
  }
  atlas->observer = TCOD_tileset_observer_new(tileset);
  if (!atlas->observer) {
    TCOD_sdl2_atlas_delete(atlas);
    return NULL;
  }
  atlas->renderer = renderer;
  atlas->tileset = tileset;
  atlas->tileset->ref_count += 1;
  atlas->observer->userdata = atlas;
  atlas->observer->on_tile_changed = sdl2_atlas_on_tile_changed;
  prepare_sdl2_atlas(atlas);
  return atlas;
}
void TCOD_sdl2_atlas_delete(struct TCOD_TilesetAtlasSDL2* atlas) {
  if (!atlas) {
    return;
  }
  if (atlas->observer) {
    TCOD_tileset_observer_delete(atlas->observer);
  }
  if (atlas->tileset) {
    TCOD_tileset_delete(atlas->tileset);
  }
  if (atlas->texture) {
    SDL_DestroyTexture(atlas->texture);
  }
  free(atlas);
}
/**
 *  Update a cache console by resetting tiles which point to the updated tile.
 */
static int cache_console_update(struct TCOD_TilesetObserver* observer, int tile_id) {
  struct TCOD_Console* console = observer->userdata;
  for (int c = 0; c < observer->tileset->character_map_length; ++c) {
    // Find codepoints that point to the tile_id.
    if (observer->tileset->character_map[c] != tile_id) {
      continue;
    }
    for (int i = 0; i < console->elements; ++i) {
      // Compare matched codepoints to the cache console characters.
      if (console->tiles[i].ch != c) {
        continue;
      }
      console->tiles[i].ch = -1;
    }
  }
  return 0;
}
/**
 *  Delete a consoles observer if it exists.
 */
static void cache_console_on_delete(struct TCOD_Console* console) {
  if (!console->userdata) {
    return;
  }
  TCOD_tileset_observer_delete(console->userdata);
}
/**
 *  Clear the observer of a console pointed to by this observer.
 */
static void cache_console_observer_delete(struct TCOD_TilesetObserver* observer) {
  ((struct TCOD_Console*)observer->userdata)->userdata = NULL;
}
/**
 *  Setup the console at `cache` to match the current console size.
 */
TCOD_NODISCARD
static TCOD_Error setup_cache_console(
    const struct TCOD_TilesetAtlasSDL2* __restrict atlas,
    const struct TCOD_Console* __restrict console,
    struct TCOD_Console* __restrict* cache) {
  if (!atlas) {
    TCOD_set_errorv("Atlas can not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!console) {
    TCOD_set_errorv("Console can not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!cache) {
    return TCOD_E_OK;
  }
  if (*cache) {
    if ((*cache)->w != console->w || (*cache)->h != console->h) {
      TCOD_console_delete(*cache);
      *cache = NULL;
    }
  }
  if (!*cache) {
    *cache = TCOD_console_new(console->w, console->h);
    struct TCOD_TilesetObserver* observer = TCOD_tileset_observer_new(atlas->tileset);
    if (!*cache || !observer) {
      TCOD_console_delete(*cache);
      *cache = NULL;
      TCOD_tileset_observer_delete(observer);
      TCOD_set_errorv("Failed to create an internal cache console.");
      return TCOD_E_OUT_OF_MEMORY;  // Failed to allocate cache.
    }
    observer->userdata = *cache;
    (*cache)->userdata = observer;
    observer->on_tile_changed = cache_console_update;
    (*cache)->on_delete = cache_console_on_delete;
    observer->on_observer_delete = cache_console_observer_delete;
    for (int i = 0; i < (*cache)->elements; ++i) {
      (*cache)->tiles[i].ch = -1;
    }
  }
  return TCOD_E_OK;
}
/// Normalize a console tile so that it collides with a cached value more easily.
/// Removes invisible or nonexistant foreground glyphs.
TCOD_ConsoleTile normalize_tile_for_drawing(TCOD_ConsoleTile tile, const TCOD_Tileset* __restrict tileset) {
  if (tile.ch == 0x20) tile.ch = 0;  // Tile is the space character.
  if (tile.ch < 0 || tile.ch >= tileset->character_map_length) {
    tile.ch = 0;  // Tile character is out-of-bounds.
  } else if (tileset->character_map[tile.ch] == 0) {
    tile.ch = 0;  // Ignore characters not defined in the tileset.
  }
  if (tile.fg.a == 0) tile.ch = 0;  // No foreground alpha.
  if (tile.bg.r == tile.fg.r && tile.bg.g == tile.fg.g && tile.bg.b == tile.fg.b && tile.bg.a == 255 &&
      tile.fg.a == 255) {
    tile.ch = 0;  // Foreground and background color match, so the foreground glyph would be invisible.
  }
  if (tile.ch == 0) {
    tile.fg.r = tile.fg.g = tile.fg.b = tile.fg.a = 0;  // Clear foreground color if the foreground glyph is skipped.
  }
  return tile;
}
#if SDL_VERSION_ATLEAST(2, 0, 18)
/// Initialize indices up to the current buffer index.
static void vertex_buffer_sync_indices(VertexBuffer* __restrict buffer) {
  for (; buffer->indices_initialized < buffer->index; ++buffer->indices_initialized) {
    buffer->indices[buffer->indices_initialized * 6 + 0] = buffer->indices_initialized * 4;
    buffer->indices[buffer->indices_initialized * 6 + 1] = buffer->indices_initialized * 4 + 1;
    buffer->indices[buffer->indices_initialized * 6 + 2] = buffer->indices_initialized * 4 + 2;
    buffer->indices[buffer->indices_initialized * 6 + 3] = buffer->indices_initialized * 4 + 2;
    buffer->indices[buffer->indices_initialized * 6 + 4] = buffer->indices_initialized * 4 + 1;
    buffer->indices[buffer->indices_initialized * 6 + 5] = buffer->indices_initialized * 4 + 3;
  }
}
/// Draw all background elements and clear the buffer.
static void vertex_buffer_flush_bg(VertexBuffer* __restrict buffer, const TCOD_TilesetAtlasSDL2* __restrict atlas) {
  vertex_buffer_sync_indices(buffer);
  SDL_SetRenderDrawBlendMode(atlas->renderer, SDL_BLENDMODE_NONE);
  SDL_RenderGeometryRaw(
      atlas->renderer,
      NULL,  // No texture, this renders solid colors.
      &buffer->vertex->x,
      sizeof(*buffer->vertex),
      &buffer->vertex->rgba,
      sizeof(*buffer->vertex),
      NULL,
      0,
      buffer->index * 4,
      buffer->indices,
      buffer->index * 6,
      2);
  buffer->index = 0;
}
/// Draw all foreground elements and clear the buffer.
static void vertex_buffer_flush_fg(VertexBuffer* __restrict buffer, const TCOD_TilesetAtlasSDL2* __restrict atlas) {
  vertex_buffer_sync_indices(buffer);
  SDL_SetTextureBlendMode(atlas->texture, SDL_BLENDMODE_BLEND);
  SDL_RenderGeometryRaw(
      atlas->renderer,
      atlas->texture,
      &buffer->vertex->x,
      sizeof(*buffer->vertex),
      &buffer->vertex->rgba,
      sizeof(*buffer->vertex),
      (float*)buffer->vertex_uv,
      sizeof(*buffer->vertex_uv),
      buffer->index * 4,
      buffer->indices,
      buffer->index * 6,
      2);
  buffer->index = 0;
}
/// Set the vertices of a tile position.
static void vertex_buffer_set_tile_pos(
    VertexBuffer* __restrict buffer, int index, int x, int y, const TCOD_Tileset* __restrict tileset) {
  buffer->vertex[index * 4 + 0].x = (float)(x * tileset->tile_width);
  buffer->vertex[index * 4 + 0].y = (float)(y * tileset->tile_height);
  buffer->vertex[index * 4 + 1].x = (float)(x * tileset->tile_width);
  buffer->vertex[index * 4 + 1].y = (float)((y + 1) * tileset->tile_height);
  buffer->vertex[index * 4 + 2].x = (float)((x + 1) * tileset->tile_width);
  buffer->vertex[index * 4 + 2].y = (float)(y * tileset->tile_height);
  buffer->vertex[index * 4 + 3].x = (float)((x + 1) * tileset->tile_width);
  buffer->vertex[index * 4 + 3].y = (float)((y + 1) * tileset->tile_height);
}
/// Set the colors of a tile.
static void vertex_buffer_set_color(VertexBuffer* __restrict buffer, int index, TCOD_ColorRGBA rgba) {
  SDL_FColor new_color = {
      (float)rgba.r * (1.0f / 255.0f),
      (float)rgba.g * (1.0f / 255.0f),
      (float)rgba.b * (1.0f / 255.0f),
      (float)rgba.a * (1.0f / 255.0f)};
  buffer->vertex[index * 4 + 0].rgba = new_color;
  buffer->vertex[index * 4 + 1].rgba = new_color;
  buffer->vertex[index * 4 + 2].rgba = new_color;
  buffer->vertex[index * 4 + 3].rgba = new_color;
}
/// Push a background element onto the buffer, flushing it if needed.
static void vertex_buffer_push_bg(
    VertexBuffer* __restrict buffer,
    int x,
    int y,
    TCOD_ConsoleTile tile,
    const TCOD_TilesetAtlasSDL2* __restrict atlas) {
  if (buffer->index == BUFFER_TILES_MAX) vertex_buffer_flush_bg(buffer, atlas);
  vertex_buffer_set_tile_pos(buffer, buffer->index, x, y, atlas->tileset);
  vertex_buffer_set_color(buffer, buffer->index, tile.bg);
  ++buffer->index;
}
/// Push a foreground element onto the buffer, flushing it if needed.
static void vertex_buffer_push_fg(
    VertexBuffer* __restrict buffer,
    int x,
    int y,
    TCOD_ConsoleTile tile,
    const TCOD_TilesetAtlasSDL2* __restrict atlas,
    float u_multiply,
    float v_multiply) {
  if (buffer->index == BUFFER_TILES_MAX) vertex_buffer_flush_fg(buffer, atlas);
  vertex_buffer_set_tile_pos(buffer, buffer->index, x, y, atlas->tileset);
  vertex_buffer_set_color(buffer, buffer->index, tile.fg);
  // Used a lazy method of UV assignment.  This could be improved to use fewer math operations.
  const int tile_id = atlas->tileset->character_map[tile.ch];
  const SDL_Rect src = get_sdl2_atlas_tile(atlas, tile_id);
  buffer->vertex_uv[buffer->index * 4 + 0].u = (float)(src.x) * u_multiply;
  buffer->vertex_uv[buffer->index * 4 + 0].v = (float)(src.y) * v_multiply;
  buffer->vertex_uv[buffer->index * 4 + 1].u = (float)(src.x) * u_multiply;
  buffer->vertex_uv[buffer->index * 4 + 1].v = (float)(src.y + src.h) * v_multiply;
  buffer->vertex_uv[buffer->index * 4 + 2].u = (float)(src.x + src.w) * u_multiply;
  buffer->vertex_uv[buffer->index * 4 + 2].v = (float)(src.y) * v_multiply;
  buffer->vertex_uv[buffer->index * 4 + 3].u = (float)(src.x + src.w) * u_multiply;
  buffer->vertex_uv[buffer->index * 4 + 3].v = (float)(src.y + src.h) * v_multiply;
  ++buffer->index;
}
#endif  // SDL_VERSION_ATLEAST(2, 0, 18)
/**
    Render a console onto the current render target.

    `atlas` is an SDL2 atlas created with `TCOD_sdl2_atlas_new`.

    `console` is the libtcod console you want to render.  Must not be NULL.

    `cache` can be NULL, or a pointer to a console pointer.
    `cache` should be NULL unless you are using a non-default render target.

    Returns a negative value on an error, check `TCOD_get_error`.
 */
static TCOD_Error TCOD_sdl2_render(
    const TCOD_TilesetAtlasSDL2* __restrict atlas,
    const TCOD_Console* __restrict console,
    TCOD_Console* __restrict cache) {
  if (!atlas) {
    TCOD_set_errorv("Atlas must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!console) {
    TCOD_set_errorv("Console must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (cache && (cache->w != console->w || cache->h != console->h)) {
    TCOD_set_errorv("Cache console must match the size of the input console.");
    return TCOD_E_INVALID_ARGUMENT;
  }
#if SDL_VERSION_ATLEAST(2, 0, 18)
  // Allocate a buffer on the stack and initialize only a few variables.
  // Reused for the background and foreground passes.
  VertexBuffer* buffer = malloc(sizeof(*buffer));
  if (!buffer) return TCOD_E_OUT_OF_MEMORY;
  buffer->index = 0;
  buffer->indices_initialized = 0;
  for (int y = 0; y < console->h; ++y) {
    for (int x = 0; x < console->w; ++x) {
      const TCOD_ConsoleTile tile = normalize_tile_for_drawing(console->tiles[console->w * y + x], atlas->tileset);
      if (cache) {
        TCOD_ConsoleTile* cached = &cache->tiles[cache->w * y + x];
        // True if there are changes to the BG color.
        const bool bg_changed = tile.bg.r != cached->bg.r || tile.bg.g != cached->bg.g || tile.bg.b != cached->bg.b ||
                                tile.bg.a != cached->bg.a;
        // True if there are changes to the FG glyph.
        const bool fg_changed =
            cached->ch && (tile.ch != cached->ch || tile.fg.r != cached->fg.r || tile.fg.g != cached->fg.g ||
                           tile.fg.b != cached->fg.b || tile.fg.a != cached->fg.a);
        if (!(bg_changed || fg_changed)) {
          continue;  // If no changes exist then this tile can be skipped entirely.
        }
        // Cache the BG and unset the FG data, this will tell the FG pass if it needs to draw the glyph.
        *cached = (TCOD_ConsoleTile){0, {0, 0, 0, 0}, tile.bg};
      }
      // Data is pushed onto the buffer, this is flushed automatically if it would otherwise overflow.
      vertex_buffer_push_bg(buffer, x, y, tile, atlas);
    }
  }
  // Flush any remaining data.  The buffer can now be reused for foreground data.
  vertex_buffer_flush_bg(buffer, atlas);

  // The foreground rendering pass.  Draw FG glyphs on top of the background tiles.
  float tex_width;
  float tex_height;
  SDL_GetTextureSize(atlas->texture, &tex_width, &tex_height);
  const float u_multiply = 1.0f / (float)(tex_width);  // Used to transform texture pixel coordinates to UV coords.
  const float v_multiply = 1.0f / (float)(tex_height);
  for (int y = 0; y < console->h; ++y) {
    for (int x = 0; x < console->w; ++x) {
      const TCOD_ConsoleTile tile = normalize_tile_for_drawing(console->tiles[console->w * y + x], atlas->tileset);
      if (tile.ch == 0) continue;  // No FG glyph to draw.
      if (cache) {
        TCOD_ConsoleTile* cached = &cache->tiles[cache->w * y + x];
        // cached->ch will be set to zero by the background pass on changes to the foreground color or glyph.
        // Because of this the FG color does not need to be checked here.
        if (tile.ch == cached->ch) {
          continue;  // The glyph has not changed since the last render.
        }
        // Cache the foreground glyph.
        cached->ch = tile.ch;
        cached->fg = tile.fg;
      }
      vertex_buffer_push_fg(buffer, x, y, tile, atlas, u_multiply, v_multiply);
    }
  }
  vertex_buffer_flush_fg(buffer, atlas);
  free(buffer);
#else  // SDL VERSION < 2.0.18
  SDL_SetRenderDrawBlendMode(atlas->renderer, SDL_BLENDMODE_NONE);
  SDL_SetTextureBlendMode(atlas->texture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureAlphaMod(atlas->texture, 0xff);
  for (int y = 0; y < console->h; ++y) {
    for (int x = 0; x < console->w; ++x) {
      const SDL_Rect dest = get_aligned_tile(atlas->tileset, x, y);
      const TCOD_ConsoleTile tile = normalize_tile_for_drawing(console->tiles[console->w * y + x], atlas->tileset);
      if (cache) {
        const struct TCOD_ConsoleTile cached = cache->tiles[cache->w * y + x];
        if (tile.ch == cached.ch && tile.fg.r == cached.fg.r && tile.fg.g == cached.fg.g && tile.fg.b == cached.fg.b &&
            tile.fg.a == cached.fg.a && tile.bg.r == cached.bg.r && tile.bg.g == cached.bg.g &&
            tile.bg.b == cached.bg.b && tile.bg.a == cached.bg.a) {
          continue;
        }
        cache->tiles[cache->w * y + x] = tile;
      }
      // Fill the background of the tile with a solid color.
      SDL_SetRenderDrawColor(atlas->renderer, tile.bg.r, tile.bg.g, tile.bg.b, tile.bg.a);
      SDL_RenderFillRect(atlas->renderer, &dest);
      if (tile.ch == 0) {
        continue;  // Skip foreground glyph.
      }
      // Blend the foreground glyph on top of the background.
      SDL_SetTextureColorMod(atlas->texture, tile.fg.r, tile.fg.g, tile.fg.b);
      SDL_SetTextureAlphaMod(atlas->texture, tile.fg.a);
      const int tile_id = atlas->tileset->character_map[tile.ch];
      const SDL_Rect src = get_sdl2_atlas_tile(atlas, tile_id);
      SDL_RenderCopy(atlas->renderer, atlas->texture, &src, &dest);
    }
  }
#endif  // SDL_VERSION_ATLEAST
  return TCOD_E_OK;
}
TCOD_Error TCOD_sdl2_render_texture_setup(
    const struct TCOD_TilesetAtlasSDL2* __restrict atlas,
    const struct TCOD_Console* __restrict console,
    struct TCOD_Console* __restrict* cache,
    struct SDL_Texture* __restrict* target) {
  if (!atlas) {
    TCOD_set_errorv("Atlas must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!console) {
    TCOD_set_errorv("Console must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!target) {
    TCOD_set_errorv("target must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (*target) {
    // Checks if *target texture is still valid for the current parameters, deletes *target if not.
    float tex_width;
    float tex_height;
    SDL_GetTextureSize(*target, &tex_width, &tex_height);
    if ((int)tex_width != atlas->tileset->tile_width * console->w ||
        (int)tex_height != atlas->tileset->tile_height * console->h) {
      TCOD_log_debug("The console renderer buffer is the wrong size and will be replaced.");
      SDL_DestroyTexture(*target);
      *target = NULL;
      if (cache && *cache) {
        TCOD_console_delete(*cache);
        *cache = NULL;
      }
    }
  }
  if (!*target) {
    // If *target is missing or deleted then create a new texture.
    TCOD_log_debug_f(
        "Creating console renderer buffer of pixel size %dx%d.",
        atlas->tileset->tile_width * console->w,
        atlas->tileset->tile_height * console->h);
    *target = SDL_CreateTexture(
        atlas->renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        atlas->tileset->tile_width * console->w,
        atlas->tileset->tile_height * console->h);
    if (!*target) {
      return TCOD_set_errorv("Failed to create a new target texture.");
    }
  }
  TCOD_Error err = TCOD_E_OK;
  if (cache) {
    err = setup_cache_console(atlas, console, cache);
  }
  return err;
}
TCOD_Error TCOD_sdl2_render_texture(
    const struct TCOD_TilesetAtlasSDL2* __restrict atlas,
    const struct TCOD_Console* __restrict console,
    struct TCOD_Console* __restrict cache,
    struct SDL_Texture* __restrict target) {
  if (!target) {  // Render without a managed target.
    return TCOD_sdl2_render(atlas, console, cache);
  }
  SDL_Texture* old_target = SDL_GetRenderTarget(atlas->renderer);
  SDL_SetRenderTarget(atlas->renderer, target);
  TCOD_Error err = TCOD_sdl2_render(atlas, console, cache);
  SDL_SetRenderTarget(atlas->renderer, old_target);
  return err;
}
// ----------------------------------------------------------------------------
// SDL2 Rendering
/**
    Handle events from SDL2.

    Target textures need to be reset on an SDL_RENDER_TARGETS_RESET event.

    This is sometimes called while the renderer is holding a reference to the
    cache console.
 */
static bool sdl2_handle_event(void* userdata, SDL_Event* event) {
  struct TCOD_RendererSDL2* context = userdata;
  switch (event->type) {
    case SDL_EVENT_RENDER_TARGETS_RESET:
      TCOD_log_debug("SDL2 renderer targets have been reset.");
      if (context->cache_console) {
        for (int i = 0; i < context->cache_console->elements; ++i) {
          context->cache_console->tiles[i] = (struct TCOD_ConsoleTile){-1, {0}, {0}};
        }
      }
      break;
  }
  return 0;
}
/**
 *  Deconstruct an SDL2 rendering context.
 */
static void sdl2_destructor(struct TCOD_Context* __restrict self) {
  struct TCOD_RendererSDL2* context = self->contextdata_;
  if (!context) {
    return;
  }
  SDL_RemoveEventWatch(sdl2_handle_event, context);
  if (context->cache_console) {
    TCOD_console_delete(context->cache_console);
  }
  if (context->cache_texture) {
    SDL_DestroyTexture(context->cache_texture);
  }
  if (context->atlas) {
    TCOD_sdl2_atlas_delete(context->atlas);
  }
  if (context->renderer) {
    SDL_DestroyRenderer(context->renderer);
  }
  if (context->window) {
    SDL_DestroyWindow(context->window);
  }
  SDL_QuitSubSystem(context->sdl_subsystems);
  free(context);
}
/** Return the destination rectangle for these inputs. */
TCOD_NODISCARD static SDL_FRect get_destination_rect(
    const struct TCOD_TilesetAtlasSDL2* atlas,
    int source_width,
    int source_height,
    const struct TCOD_ViewportOptions* viewport) {
  if (!viewport) viewport = &TCOD_VIEWPORT_DEFAULT_;
  float output_w;
  float output_h;
  SDL_Texture* render_target = SDL_GetRenderTarget(atlas->renderer);
  if (render_target) {
    SDL_GetTextureSize(render_target, &output_w, &output_h);
  } else {
    int out_w_int;
    int out_h_int;
    SDL_GetCurrentRenderOutputSize(atlas->renderer, &out_w_int, &out_h_int);
    output_w = (float)out_w_int;
    output_h = (float)out_h_int;
  }
  SDL_FRect out = {0, 0, output_w, output_h};
  float scale_w = (float)output_w / (float)(source_width);
  float scale_h = (float)output_h / (float)(source_height);
  if (viewport->integer_scaling) {
    scale_w = scale_w <= 1.0f ? scale_w : floorf(scale_w);
    scale_h = scale_h <= 1.0f ? scale_h : floorf(scale_h);
  }
  if (viewport->keep_aspect) {
    scale_w = scale_h = minf(scale_w, scale_h);
  }
  out.w = (float)source_width * scale_w;
  out.h = (float)source_height * scale_h;
  out.x = (output_w - out.w) * clampf(viewport->align_x, 0.0f, 1.0f);
  out.y = (output_h - out.h) * clampf(viewport->align_y, 0.0f, 1.0f);
  return out;
}
/***************************************************************************
    @brief Return the destination rectangle for these inputs
 */
TCOD_NODISCARD static SDL_FRect get_destination_rect_for_console(
    const struct TCOD_TilesetAtlasSDL2* atlas,
    const struct TCOD_Console* console,
    const struct TCOD_ViewportOptions* viewport) {
  const int tile_width = atlas->tileset->tile_width;
  const int tile_height = atlas->tileset->tile_height;
  return get_destination_rect(atlas, console->w * tile_width, console->h * tile_height, viewport);
}
/***************************************************************************
    @brief Return info needed to convert the mouse between pixel and console coordinates.
 */
TCOD_NODISCARD static TCOD_MouseTransform sdl2_cursor_transform_for_console_viewport(
    const struct TCOD_TilesetAtlasSDL2* atlas,
    const struct TCOD_Console* console,
    const struct TCOD_ViewportOptions* viewport) {
  SDL_FRect dest = get_destination_rect_for_console(atlas, console, viewport);
  return (TCOD_MouseTransform){
      dest.x,
      dest.y,
      (double)console->w / (double)dest.w,
      (double)console->h / (double)dest.h,
  };
} /***************************************************************************
     @brief Manually set the mouse transform.
  */
TCOD_NODISCARD static TCOD_Error sdl2_cursor_set_transform(
    struct TCOD_Context* __restrict self, const TCOD_MouseTransform* __restrict transform) {
  struct TCOD_RendererSDL2* context = self->contextdata_;
  context->cursor_transform = *transform;
  return TCOD_E_OK;
}
/**
 *  Render to the SDL2 renderer without presenting the screen.
 */
static TCOD_Error sdl2_accumulate(
    struct TCOD_Context* __restrict self,
    const struct TCOD_Console* __restrict console,
    const struct TCOD_ViewportOptions* __restrict viewport) {
  struct TCOD_RendererSDL2* context = self->contextdata_;
  if (!context || !console) {
    return -1;
  }
  TCOD_Error err;
  err = TCOD_sdl2_render_texture_setup(context->atlas, console, &context->cache_console, &context->cache_texture);
  if (err < 0) {
    return err;
  }
  err = TCOD_sdl2_render_texture(context->atlas, console, context->cache_console, context->cache_texture);
  if (err < 0) {
    return err;
  }
  SDL_FRect dest = get_destination_rect_for_console(context->atlas, console, viewport);
  // Set mouse coordinate scaling.
  context->cursor_transform = sdl2_cursor_transform_for_console_viewport(context->atlas, console, viewport);
  if (!TCOD_ctx.sdl_cbk) {
    // Normal rendering.
    SDL_RenderTexture(context->renderer, context->cache_texture, NULL, &dest);
  } else {
    // Deprecated callback rendering.
    SDL_Texture* old_target = SDL_GetRenderTarget(context->renderer);
    SDL_SetRenderTarget(context->renderer, context->cache_texture);
    SDL_Surface* canvas = SDL_RenderReadPixels(context->renderer, NULL);
    SDL_SetRenderTarget(context->renderer, old_target);
    TCOD_ctx.sdl_cbk(canvas);
    SDL_Texture* canvas_tex = SDL_CreateTextureFromSurface(context->renderer, canvas);
    SDL_RenderTexture(context->renderer, canvas_tex, NULL, &dest);
    SDL_DestroyTexture(canvas_tex);
    SDL_DestroySurface(canvas);
  }
  return TCOD_E_OK;
}
/**
 *  Clear, render, and present a libtcod console to the screen.
 */
static TCOD_Error sdl2_present(
    struct TCOD_Context* __restrict self,
    const struct TCOD_Console* __restrict console,
    const struct TCOD_ViewportOptions* __restrict viewport) {
  if (!viewport) {
    viewport = &TCOD_VIEWPORT_DEFAULT_;
  }
  struct TCOD_RendererSDL2* context = self->contextdata_;
  SDL_SetRenderTarget(context->renderer, NULL);
  SDL_SetRenderDrawColor(
      context->renderer,
      viewport->clear_color.r,
      viewport->clear_color.g,
      viewport->clear_color.b,
      viewport->clear_color.a);
  SDL_RenderClear(context->renderer);
  TCOD_Error err = sdl2_accumulate(self, console, viewport);
  if (err) {
    return err;
  }
  SDL_RenderPresent(context->renderer);
  return TCOD_E_OK;
}
/**
 *  Convert pixel coordinates to tile coordinates.
 */
static void sdl2_pixel_to_tile(struct TCOD_Context* __restrict self, double* __restrict x, double* __restrict y) {
  struct TCOD_RendererSDL2* context = self->contextdata_;
  *x = (*x - context->cursor_transform.offset_x) * context->cursor_transform.scale_x;
  *y = (*y - context->cursor_transform.offset_y) * context->cursor_transform.scale_y;
}
/**
 *  Save a PNG screen-shot to `file`.
 */
static TCOD_Error sdl2_screen_capture(
    struct TCOD_Context* __restrict self,
    TCOD_ColorRGBA* __restrict out_pixels,
    int* __restrict width,
    int* __restrict height) {
  struct TCOD_RendererSDL2* context = self->contextdata_;
  if (!context->cache_texture) {
    TCOD_set_errorv("Nothing to save before the first frame.");
    *width = 0;
    *height = 0;
    return TCOD_E_WARN;
  }
  SDL_SetRenderTarget(context->renderer, context->cache_texture);
  float current_width = 0;
  float current_height = 0;
  SDL_GetTextureSize(context->cache_texture, &current_width, &current_height);
  TCOD_Error err = TCOD_E_OK;
  if (!out_pixels) {
    *width = (int)current_width;
    *height = (int)current_height;
  } else {
    SDL_Surface* capture = SDL_RenderReadPixels(context->renderer, NULL);
    if (capture->w == (*width) && capture->h == (*height)) {
      SDL_ConvertPixels(
          capture->w,
          capture->h,
          capture->format,
          capture->pixels,
          capture->pitch,
          SDL_PIXELFORMAT_RGBA32,
          out_pixels,
          (int)(sizeof(*out_pixels) * (*width)));
    } else {
      err = TCOD_set_errorv("width or height do not match the size of the screen.");
    }
    SDL_DestroySurface(capture);
  }
  SDL_SetRenderTarget(context->renderer, NULL);
  return err;
}
/**
 *  Return a pointer to the SDL2 window.
 */
static struct SDL_Window* sdl2_get_window(struct TCOD_Context* __restrict self) {
  return ((struct TCOD_RendererSDL2*)self->contextdata_)->window;
}
/**
 *  Return a pointer to the SDL2 renderer.
 */
static struct SDL_Renderer* sdl2_get_renderer(struct TCOD_Context* __restrict self) {
  return ((struct TCOD_RendererSDL2*)self->contextdata_)->renderer;
}
/**
    Change the atlas to the given tileset.
 */
static TCOD_Error sdl2_set_tileset(struct TCOD_Context* __restrict self, TCOD_Tileset* __restrict tileset) {
  struct TCOD_RendererSDL2* context = self->contextdata_;
  struct TCOD_TilesetAtlasSDL2* atlas = TCOD_sdl2_atlas_new(context->renderer, tileset);
  if (!atlas) {
    return TCOD_E_ERROR;
  }
  if (context->atlas) {
    TCOD_sdl2_atlas_delete(context->atlas);
  }
  context->atlas = atlas;
  if (context->cache_console) {
    TCOD_console_delete(context->cache_console);
    context->cache_console = NULL;
  }
  return TCOD_E_OK;
}
static TCOD_Error sdl2_recommended_console_size(
    struct TCOD_Context* __restrict self, float magnification, int* __restrict columns, int* __restrict rows) {
  struct TCOD_RendererSDL2* context = self->contextdata_;
  int w;
  int h;
  if (!SDL_GetCurrentRenderOutputSize(context->renderer, &w, &h)) {
    TCOD_set_errorvf("SDL Error: %s", SDL_GetError());
    return TCOD_E_ERROR;
  }
  if (columns && context->atlas->tileset->tile_width * magnification != 0) {
    *columns = (int)(w / (context->atlas->tileset->tile_width * magnification));
  }
  if (rows && context->atlas->tileset->tile_height * magnification != 0) {
    *rows = (int)(h / (context->atlas->tileset->tile_height * magnification));
  }
  return TCOD_E_OK;
}
struct TCOD_Context* TCOD_renderer_init_sdl3(
    SDL_PropertiesID window_props, SDL_PropertiesID renderer_props, struct TCOD_Tileset* tileset) {
  TCOD_log_debug("Initializing an SDL3 renderer.");
  if (!tileset) {
    TCOD_set_errorv("Tileset must not be NULL.");
    return NULL;
  }
  struct TCOD_Context* context = TCOD_context_new_();
  if (!context) {
    return NULL;
  }
  context->c_destructor_ = sdl2_destructor;
  struct TCOD_RendererSDL2* sdl2_data = calloc(1, sizeof(*sdl2_data));
  context->contextdata_ = sdl2_data;
  if (!sdl2_data) {
    TCOD_set_errorv("Out of memory.");
    TCOD_context_delete(context);
    return NULL;
  }
  if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
    TCOD_set_errorvf("Could not initialize SDL:\n%s", SDL_GetError());
    TCOD_context_delete(context);
    return NULL;
  }
  sdl2_data->sdl_subsystems = SDL_INIT_VIDEO;
  context->type = TCOD_RENDERER_SDL2;
  context->c_present_ = sdl2_present;
  context->c_accumulate_ = sdl2_accumulate;
  context->c_get_sdl_window_ = sdl2_get_window;
  context->c_get_sdl_renderer_ = sdl2_get_renderer;
  context->c_pixel_to_tile_ = sdl2_pixel_to_tile;
  context->c_screen_capture_ = sdl2_screen_capture;
  context->c_set_tileset_ = sdl2_set_tileset;
  context->c_recommended_console_size_ = sdl2_recommended_console_size;
  context->c_set_mouse_transform_ = sdl2_cursor_set_transform;

  SDL_AddEventWatch(sdl2_handle_event, sdl2_data);
  sdl2_data->window = SDL_CreateWindowWithProperties(window_props);
  if (!sdl2_data->window) {
    TCOD_set_errorvf("Could not create SDL window:\n%s", SDL_GetError());
    TCOD_context_delete(context);
    return NULL;
  }
  SDL_SetPointerProperty(renderer_props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, sdl2_data->window);
  sdl2_data->renderer = SDL_CreateRendererWithProperties(renderer_props);
  SDL_SetPointerProperty(renderer_props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, NULL);
  if (!sdl2_data->renderer) {
    TCOD_set_errorvf("Could not create SDL renderer:\n%s", SDL_GetError());
    TCOD_context_delete(context);
    return NULL;
  }
  if (context->c_set_tileset_(context, tileset) < 0) {
    TCOD_context_delete(context);
    return NULL;
  }
  return context;
}
struct TCOD_Context* TCOD_renderer_init_sdl2(
    int x,
    int y,
    int pixel_width,
    int pixel_height,
    const char* title,
    int window_flags,
    int vsync,
    struct TCOD_Tileset* tileset) {
  SDL_PropertiesID window_props = SDL_CreateProperties();
  SDL_SetStringProperty(window_props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title);
  SDL_SetNumberProperty(window_props, SDL_PROP_WINDOW_CREATE_X_NUMBER, x);
  SDL_SetNumberProperty(window_props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, y);
  SDL_SetNumberProperty(window_props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, pixel_width);
  SDL_SetNumberProperty(window_props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, pixel_height);
  SDL_SetNumberProperty(window_props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, window_flags);

  SDL_PropertiesID renderer_props = SDL_CreateProperties();
  SDL_SetNumberProperty(renderer_props, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER, (vsync ? 1 : 0));
  TCOD_Context* context = TCOD_renderer_init_sdl3(window_props, renderer_props, tileset);
  SDL_DestroyProperties(renderer_props);
  SDL_DestroyProperties(window_props);
  return context;
}
#endif  // NO_SDL
