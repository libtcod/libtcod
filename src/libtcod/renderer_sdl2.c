/* BSD 3-Clause License
 *
 * Copyright © 2008-2021, Jice and the libtcod contributors.
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
#include <SDL.h>
#include <lodepng.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "libtcod_int.h"

static inline float minf(float a, float b) { return a < b ? a : b; }
static inline float maxf(float a, float b) { return a > b ? a : b; }
static inline float clampf(float v, float low, float high) { return maxf(low, minf(v, high)); }
// ----------------------------------------------------------------------------
// SDL2 Atlas
/**
 *  Return a rectangle shaped for a tile at `x`,`y`.
 */
static SDL_Rect get_aligned_tile(const struct TCOD_Tileset* tileset, int x, int y) {
  SDL_Rect tile_rect = {x * tileset->tile_width, y * tileset->tile_height, tileset->tile_width, tileset->tile_height};
  return tile_rect;
}
/**
 *  Return the rectangle for the tile at `tile_id`.
 */
static SDL_Rect get_sdl2_atlas_tile(const struct TCOD_TilesetAtlasSDL2* atlas, int tile_id) {
  return get_aligned_tile(atlas->tileset, tile_id % atlas->texture_columns, tile_id / atlas->texture_columns);
}
/**
 *  Upload a single tile to the atlas texture.
 */
static int update_sdl2_tile(struct TCOD_TilesetAtlasSDL2* atlas, int tile_id) {
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
    SDL_QueryTexture(atlas->texture, NULL, NULL, &current_size, NULL);
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
  struct TCOD_TilesetAtlasSDL2* atlas = calloc(sizeof(*atlas), 1);
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
/**
    Render a console onto the current render target.

    `atlas` is an SDL2 atlas created with `TCOD_sdl2_atlas_new`.

    `console` is the libtcod console you want to render.  Must not be NULL.

    `cache` can be NULL, or a pointer to a console pointer.
    `cache` should be NULL unless you are using a non-default render target.

    Returns a negative value on an error, check `TCOD_get_error`.
 */
static TCOD_Error TCOD_sdl2_render(
    const struct TCOD_TilesetAtlasSDL2* __restrict atlas,
    const struct TCOD_Console* __restrict console,
    struct TCOD_Console* __restrict cache) {
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
  SDL_SetRenderDrawBlendMode(atlas->renderer, SDL_BLENDMODE_NONE);
  SDL_SetTextureBlendMode(atlas->texture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureAlphaMod(atlas->texture, 0xff);
  for (int y = 0; y < console->h; ++y) {
    for (int x = 0; x < console->w; ++x) {
      const SDL_Rect dest = get_aligned_tile(atlas->tileset, x, y);
      struct TCOD_ConsoleTile tile = console->tiles[console->w * y + x];
      if (tile.ch == 0x20) {
        tile.ch = 0;  // Tile is the space character.
      }
      if (tile.ch < 0 || tile.ch >= atlas->tileset->character_map_length) {
        tile.ch = 0;  // Tile is out-of-bounds.
      }
      if (tile.fg.a == 0) {
        tile.ch = 0;  // No foreground alpha.
      }
      if (tile.ch == 0) {
        // Clear foreground color if the foreground glyph is skipped.
        tile.fg.r = tile.fg.g = tile.fg.b = tile.fg.a = 0;
      }
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
      int tile_id = atlas->tileset->character_map[tile.ch];
      const SDL_Rect src = get_sdl2_atlas_tile(atlas, tile_id);
      SDL_RenderCopy(atlas->renderer, atlas->texture, &src, &dest);
    }
  }
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
  SDL_RendererInfo renderer_info;
  if (SDL_GetRendererInfo(atlas->renderer, &renderer_info)) {
    return TCOD_set_errorvf("SDL error getting renderer info: %s", SDL_GetError());
  }
  if (!(renderer_info.flags & SDL_RENDERER_TARGETTEXTURE)) {
    return TCOD_set_errorv("SDL_RENDERER_TARGETTEXTURE is required.");
  }
  if (*target) {
    // Checks if *target texture is still valid for the current parameters, deletes *target if not.
    int tex_width;
    int tex_height;
    SDL_QueryTexture(*target, NULL, NULL, &tex_width, &tex_height);
    if (tex_width != atlas->tileset->tile_width * console->w ||
        tex_height != atlas->tileset->tile_height * console->h) {
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
static int sdl2_handle_event(void* userdata, SDL_Event* event) {
  struct TCOD_RendererSDL2* context = userdata;
  switch (event->type) {
    case SDL_RENDER_TARGETS_RESET:
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
  SDL_DelEventWatch(sdl2_handle_event, context);
  if (context->cache_console) {
    TCOD_console_delete(context->cache_console);
  }
  if (context->cache_texture) {
    SDL_DestroyTexture(context->cache_texture);
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
TCOD_NODISCARD static SDL_Rect get_destination_rect(
    const struct TCOD_TilesetAtlasSDL2* atlas,
    const struct TCOD_Console* console,
    const struct TCOD_ViewportOptions* viewport) {
  if (!viewport) {
    viewport = &TCOD_VIEWPORT_DEFAULT_;
  }
  const int tile_width = atlas->tileset->tile_width;
  const int tile_height = atlas->tileset->tile_height;
  int output_w;
  int output_h;
  SDL_GetRendererOutputSize(atlas->renderer, &output_w, &output_h);
  SDL_Rect out = {0, 0, output_w, output_h};
  float scale_w = (float)output_w / (float)(console->w * tile_width);
  float scale_h = (float)output_h / (float)(console->h * tile_height);
  if (viewport->integer_scaling) {
    scale_w = scale_w <= 1.0f ? scale_w : floorf(scale_w);
    scale_h = scale_h <= 1.0f ? scale_h : floorf(scale_h);
  }
  if (viewport->keep_aspect) {
    scale_w = scale_h = minf(scale_w, scale_h);
  }
  out.w = (int)((float)(console->w * tile_width) * scale_w);
  out.h = (int)((float)(console->h * tile_height) * scale_h);
  out.x = (int)((float)(output_w - out.w) * clampf(viewport->align_x, 0, 1));
  out.y = (int)((float)(output_h - out.h) * clampf(viewport->align_y, 0, 1));
  return out;
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
  SDL_Rect dest = get_destination_rect(context->atlas, console, viewport);
  // Set mouse coordinate scaling.
  context->last_offset_x = dest.x;
  context->last_offset_y = dest.y;
  context->last_scale_x = (double)console->w / (double)dest.w;
  context->last_scale_y = (double)console->h / (double)dest.h;
  if (!TCOD_ctx.sdl_cbk) {
    // Normal rendering.
    SDL_RenderCopy(context->renderer, context->cache_texture, NULL, &dest);
  } else {
    // Deprecated callback rendering.
    int tex_width;
    int tex_height;
    SDL_QueryTexture(context->cache_texture, NULL, NULL, &tex_width, &tex_height);
    SDL_Surface* canvas = SDL_CreateRGBSurfaceWithFormat(0, tex_width, tex_height, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_SetRenderTarget(context->renderer, context->cache_texture);
    SDL_RenderReadPixels(context->renderer, NULL, SDL_PIXELFORMAT_RGBA32, canvas->pixels, tex_width * 4);
    SDL_SetRenderTarget(context->renderer, NULL);
    TCOD_ctx.sdl_cbk(canvas);
    SDL_Texture* canvas_tex = SDL_CreateTextureFromSurface(context->renderer, canvas);
    SDL_RenderCopy(context->renderer, canvas_tex, NULL, &dest);
    SDL_DestroyTexture(canvas_tex);
    SDL_FreeSurface(canvas);
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
  *x = (*x - context->last_offset_x) * context->last_scale_x;
  *y = (*y - context->last_offset_y) * context->last_scale_y;
}
/**
 *  Save a PNG screen-shot to `file`.
 */
static TCOD_Error sdl2_save_screenshot(struct TCOD_Context* __restrict self, const char* __restrict filename) {
  struct TCOD_RendererSDL2* context = self->contextdata_;
  if (!context->cache_texture) {
    TCOD_set_errorv("Nothing to save before the first frame.");
    lodepng_encode32_file(filename, NULL, 0, 0);
    return TCOD_E_WARN;
  }
  SDL_SetRenderTarget(context->renderer, context->cache_texture);
  int width;
  int height;
  SDL_QueryTexture(context->cache_texture, NULL, NULL, &width, &height);
  void* pixels = malloc(sizeof(uint8_t) * 4 * width * height);
  if (!pixels) {
    SDL_SetRenderTarget(context->renderer, NULL);
    TCOD_set_errorv("Out of memory.");
    return TCOD_E_OUT_OF_MEMORY;
  }
  SDL_RenderReadPixels(context->renderer, NULL, SDL_PIXELFORMAT_RGBA32, pixels, (int)(sizeof(uint8_t) * 4 * width));
  lodepng_encode32_file(filename, pixels, (unsigned)width, (unsigned)height);
  free(pixels);
  SDL_SetRenderTarget(context->renderer, NULL);
  return TCOD_E_OK;
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
  if (SDL_GetRendererOutputSize(context->renderer, &w, &h) < 0) {
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
/**
    Lists SDL2's video driver status to `log_out[log_length]`.

    The output will always be NULL terminated.
 */
static void TCOD_sdl2_debug_video_drivers(int log_length, char* log_out) {
  if (log_length < 1) {
    return;
  }
  log_out[0] = '\0';
  if (SDL_GetCurrentVideoDriver() != NULL) {
    return;
  }
  int driver_count = SDL_GetNumVideoDrivers();
  for (int i = 0; i < driver_count; ++i) {
    if (log_length <= 1) {
      return;
    }
    const char* driver_name = SDL_GetVideoDriver(i);
    bool is_working;
    if (SDL_VideoInit(driver_name) == 0) {
      SDL_VideoQuit();
      is_working = 1;
    } else {
      is_working = 0;
    }
    int print_length = snprintf(
        log_out,
        log_length,
        "%sVideo driver '%s' %s.",
        i == 0 ? "" : "\n",
        driver_name,
        is_working ? "is available" : "is not working");
    if (print_length < 0) {
      return;
    }
    log_out += print_length;
    log_length -= print_length;
  }
}
struct TCOD_Context* TCOD_renderer_init_sdl2(
    int x,
    int y,
    int pixel_width,
    int pixel_height,
    const char* title,
    int window_flags,
    int renderer_flags,
    struct TCOD_Tileset* tileset) {
  if (!tileset) {
    TCOD_set_errorv("Tileset must not be NULL.");
    return NULL;
  }
  struct TCOD_Context* context = TCOD_context_new_();
  if (!context) {
    return NULL;
  }
  context->c_destructor_ = sdl2_destructor;
  struct TCOD_RendererSDL2* sdl2_data = calloc(sizeof(*sdl2_data), 1);
  context->contextdata_ = sdl2_data;
  if (!sdl2_data) {
    TCOD_set_errorv("Out of memory.");
    TCOD_context_delete(context);
    return NULL;
  }
  if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
    char video_driver_log[512];
    TCOD_sdl2_debug_video_drivers((int)sizeof(video_driver_log), video_driver_log);
    TCOD_set_errorvf("Could not initialize SDL:\n%s\n%s", SDL_GetError(), video_driver_log);
    TCOD_context_delete(context);
    return NULL;
  }
  sdl2_data->sdl_subsystems = SDL_INIT_VIDEO;
  context->type = TCOD_RENDERER_SDL2;
  if (renderer_flags & SDL_RENDERER_SOFTWARE) {
    context->type = TCOD_RENDERER_SDL;
  }
  context->c_present_ = sdl2_present;
  context->c_accumulate_ = sdl2_accumulate;
  context->c_get_sdl_window_ = sdl2_get_window;
  context->c_get_sdl_renderer_ = sdl2_get_renderer;
  context->c_pixel_to_tile_ = sdl2_pixel_to_tile;
  context->c_save_screenshot_ = sdl2_save_screenshot;
  context->c_set_tileset_ = sdl2_set_tileset;
  context->c_recommended_console_size_ = sdl2_recommended_console_size;

  SDL_AddEventWatch(sdl2_handle_event, sdl2_data);
  sdl2_data->window = SDL_CreateWindow(title, x, y, pixel_width, pixel_height, window_flags);
  if (!sdl2_data->window) {
    TCOD_set_errorvf("Could not create SDL window:\n%s", SDL_GetError());
    TCOD_context_delete(context);
    return NULL;
  }
  renderer_flags |= SDL_RENDERER_TARGETTEXTURE;
  sdl2_data->renderer = SDL_CreateRenderer(sdl2_data->window, -1, renderer_flags);
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
#endif  // NO_SDL
