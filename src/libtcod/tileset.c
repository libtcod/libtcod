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
#include "tileset.h"

#ifndef TCOD_NO_PNG
#include <lodepng.h>
#endif  // TCOD_NO_PNG
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"

// Starting sizes of arrays:
#define DEFAULT_TILES_LENGTH 256
#define DEFAULT_CHARMAP_LENGTH 256

TCOD_Tileset* TCOD_tileset_new(int tile_width, int tile_height) {
  TCOD_Tileset* tileset = calloc(1, sizeof(*tileset));
  if (!tileset) {
    return NULL;
  }
  ++tileset->ref_count;
  tileset->tile_width = tile_width;
  tileset->tile_height = tile_height;
  tileset->tile_length = tile_width * tile_height;
  tileset->virtual_columns = 1;
  return tileset;
}
void TCOD_tileset_delete(TCOD_Tileset* tileset) {
  if (!tileset) {
    return;
  }
  if (--tileset->ref_count != 0) {
    return;
  }
  while (tileset->observer_list) {
    TCOD_tileset_observer_delete(tileset->observer_list);
  }
  free(tileset->pixels);
  free(tileset->character_map);
  free(tileset);
}
struct TCOD_TilesetObserver* TCOD_tileset_observer_new(struct TCOD_Tileset* tileset) {
  if (!tileset) {
    return NULL;
  }
  struct TCOD_TilesetObserver* observer = calloc(1, sizeof(*observer));
  observer->tileset = tileset;
  observer->next = tileset->observer_list;
  tileset->observer_list = observer;
  return observer;
}
void TCOD_tileset_observer_delete(struct TCOD_TilesetObserver* observer) {
  if (!observer) {
    return;
  }
  for (struct TCOD_TilesetObserver** it = &observer->tileset->observer_list; *it; it = &(*it)->next) {
    if (*it != observer) {
      continue;
    }
    *it = observer->next;
    if (observer->on_observer_delete) {
      observer->on_observer_delete(observer);
    }
    free(observer);
    return;
  }
  return;
}
int TCOD_tileset_get_tile_width_(const TCOD_Tileset* tileset) { return tileset ? tileset->tile_width : 0; }
int TCOD_tileset_get_tile_height_(const TCOD_Tileset* tileset) { return tileset ? tileset->tile_height : 0; }
/**
 *  Reserve memory for the character mapping array.
 */
TCOD_NODISCARD
static TCOD_Error TCOD_tileset_charmap_reserve(TCOD_Tileset* tileset, int want) {
  if (!tileset) {
    TCOD_set_errorv("Tileset argument must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (want < 0) {
    TCOD_set_errorv("Can not take a negative number.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (want <= tileset->character_map_length) {
    return TCOD_E_OK;
  }
  int new_length = tileset->character_map_length;
  if (new_length == 0) {
    new_length = DEFAULT_CHARMAP_LENGTH;
  }
  while (want > new_length) {
    new_length *= 2;
  }
  int* new_charmap = realloc(tileset->character_map, sizeof(int) * new_length);
  if (!new_charmap) {
    TCOD_set_errorv("Could not allocate enough memory for the tileset.");
    return TCOD_E_OUT_OF_MEMORY;
  }
  for (int i = tileset->character_map_length; i < new_length; ++i) {
    new_charmap[i] = 0;
  }
  tileset->character_map_length = new_length;
  tileset->character_map = new_charmap;
  return TCOD_E_OK;
}
TCOD_Error TCOD_tileset_reserve(TCOD_Tileset* tileset, int want) {
  if (!tileset) {
    TCOD_set_errorv("Tileset argument must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (tileset->tile_length == 0) {
    return TCOD_E_OK;  // Tiles have zero size.
  }
  if (want < 0) {
    TCOD_set_errorv("Can not take a negative number.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (want <= tileset->tiles_capacity) {
    return TCOD_E_OK;
  }
  int new_capacity = tileset->tiles_capacity * 2;
  if (new_capacity == 0) {
    new_capacity = DEFAULT_TILES_LENGTH;
  }
  if (new_capacity < want) {
    new_capacity = want;
  }
  struct TCOD_ColorRGBA* new_pixels =
      realloc(tileset->pixels, sizeof(tileset->pixels[0]) * new_capacity * tileset->tile_length);
  if (!new_pixels) {
    TCOD_set_errorv("Could not allocate enough memory for the tileset.");
    return TCOD_E_OUT_OF_MEMORY;
  }
  for (int i = tileset->tiles_capacity * tileset->tile_length; i < new_capacity * tileset->tile_length; ++i) {
    // Clear allocated tiles.
    new_pixels[i] = (struct TCOD_ColorRGBA){0, 0, 0, 0};
  }
  tileset->tiles_capacity = new_capacity;
  tileset->pixels = new_pixels;
  if (tileset->tiles_count == 0) {
    tileset->tiles_count = 1;  // Keep tile at zero blank.
  }
  return TCOD_E_OK;
}
/**
 *  Return the tile ID that `codepoint` is assigned to.
 *
 *  Returns 0 for unassigned codepoints.
 */
static int TCOD_tileset_get_tile_id(const TCOD_Tileset* tileset, int codepoint) {
  if (!tileset) {
    return 0;
  }
  if (codepoint < 0 || codepoint >= tileset->character_map_length) {
    return 0;
  }
  return tileset->character_map[codepoint];
}
int TCOD_tileset_assign_tile(struct TCOD_Tileset* tileset, int tile_id, int codepoint) {
  if (tile_id < 0 || tile_id >= tileset->tiles_count) {
    TCOD_set_errorv("Tile_ID is out of bounds.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (codepoint < 0) {
    TCOD_set_errorv("Codepoint argument can not be negative.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  TCOD_Error err = TCOD_tileset_charmap_reserve(tileset, codepoint + 1);
  if (err < 0) {
    return err;
  }
  tileset->character_map[codepoint] = tile_id;
  return tile_id;
}
/**
 *  Reserve space for an additional tile and return its ID.
 *
 *  Returns a negative value on error.
 */
static int TCOD_tileset_generate_tile(struct TCOD_Tileset* tileset) {
  TCOD_Error err = TCOD_tileset_reserve(tileset, tileset->tiles_count + 1);
  if (err) {
    return err;
  }
  return tileset->tiles_count++;
}
/**
 *  Ensure a tile exists for a codepoint and return its ID.
 *
 *  If `codepoint` is zero then this is the same as
 *  `TCOD_tileset_generate_tile`.
 *
 *  Returns a negative value on error.
 */
static int TCOD_tileset_generate_codepoint(struct TCOD_Tileset* tileset, int codepoint) {
  int tile_id = TCOD_tileset_get_tile_id(tileset, codepoint);
  if (tile_id != 0) {
    return tile_id;
  }
  tile_id = TCOD_tileset_generate_tile(tileset);
  if (tile_id < 0) {
    return tile_id;
  }
  return TCOD_tileset_assign_tile(tileset, tile_id, codepoint);
}
const struct TCOD_ColorRGBA* TCOD_tileset_get_tile(const TCOD_Tileset* tileset, int codepoint) {
  if (!tileset) {
    return NULL;
  }
  int tile_id = TCOD_tileset_get_tile_id(tileset, codepoint);
  if (tile_id < 0) {
    return NULL;  // No tile for the given codepoint in this tileset.
  }
  return tileset->pixels + tileset->tile_length * tile_id;
}
TCOD_Error TCOD_tileset_get_tile_(
    const TCOD_Tileset* __restrict tileset, int codepoint, struct TCOD_ColorRGBA* __restrict buffer) {
  if (!tileset) {
    TCOD_set_errorv("Tileset argument must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  const struct TCOD_ColorRGBA* tile = TCOD_tileset_get_tile(tileset, codepoint);
  if (!tile) {
    TCOD_set_errorvf("Codepoint %i is not assigned to a tile in this tileset.", codepoint);
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!buffer) {
    return TCOD_E_OK;  // buffer is NULL, just return an OK status.
  }
  memcpy(buffer, tile, sizeof(*tile) * tileset->tile_length);
  return TCOD_E_OK;  // Tile exists and was copied to buffer.
}
void TCOD_tileset_notify_tile_changed(TCOD_Tileset* tileset, int tile_id) {
  for (struct TCOD_TilesetObserver* it = tileset->observer_list; it; it = it->next) {
    if (it->on_tile_changed) {
      it->on_tile_changed(it, tile_id);
    }
  }
}
static TCOD_Error TCOD_tileset_set_tile_rgba(
    TCOD_Tileset* __restrict tileset, int codepoint, const void* __restrict pixels, int stride) {
  int tile_id = TCOD_tileset_generate_codepoint(tileset, codepoint);
  if (!pixels) {
    TCOD_set_errorv("Pixels argument must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (tile_id < 0) {
    return (TCOD_Error)tile_id;
  }
  for (int y = 0; y < tileset->tile_height; ++y) {
    const char* ptr_in = pixels;
    const struct TCOD_ColorRGBA* row_in = (const void*)(ptr_in + y * stride);
    for (int x = 0; x < tileset->tile_width; ++x) {
      tileset->pixels[tile_id * tileset->tile_length + y * tileset->tile_width + x] = row_in[x];
    }
  }
  TCOD_tileset_notify_tile_changed(tileset, tile_id);
  return TCOD_E_OK;
}
TCOD_Error TCOD_tileset_set_tile_(
    TCOD_Tileset* __restrict tileset, int codepoint, const struct TCOD_ColorRGBA* __restrict buffer) {
  if (!tileset) {
    TCOD_set_errorv("Tileset argument must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  return TCOD_tileset_set_tile_rgba(tileset, codepoint, buffer, sizeof(*buffer) * tileset->tile_width);
}
static void upload_tile_by_id_normalized(
    TCOD_Tileset* __restrict tileset,
    int tile_id,
    const void* __restrict pixels,
    int stride,
    const struct TCOD_ColorRGBA* __restrict color_key) {
  // Analyze this tiles traits.
  bool has_color = false;
  bool has_alpha = false;
  for (int y = 0; y < tileset->tile_height; ++y) {
    for (int x = 0; x < tileset->tile_width; ++x) {
      const struct TCOD_ColorRGBA pixel = ((struct TCOD_ColorRGBA*)((char*)pixels + y * stride))[x];
      if (pixel.a != 255) {
        has_alpha = true;
      }
      if (pixel.r != pixel.g || pixel.r != pixel.b) {
        has_color = true;
      }
    }
  }
  // Normalize and copy the tile.
  for (int y = 0; y < tileset->tile_height; ++y) {
    for (int x = 0; x < tileset->tile_width; ++x) {
      struct TCOD_ColorRGBA pixel = ((struct TCOD_ColorRGBA*)((char*)pixels + y * stride))[x];
      // Convert any grey-scale tiles to white-with-alpha.
      if (!has_color && !has_alpha) {
        pixel.a = pixel.r;
        pixel.r = pixel.g = pixel.b = 0xff;
      }
      // Set key-color pixels to zero.
      if (color_key && color_key->r == pixel.r && color_key->g == pixel.g && color_key->b == pixel.b &&
          color_key->a == pixel.a) {
        pixel.r = pixel.g = pixel.b = pixel.a = 0;
      }
      tileset->pixels[tile_id * tileset->tile_length + y * tileset->tile_width + x] = pixel;
    }
  }
}
TCOD_Tileset* TCOD_tileset_load_raw(
    int width,
    int height,
    const struct TCOD_ColorRGBA* __restrict pixels,
    int columns,
    int rows,
    int n,
    const int* __restrict charmap) {
  int font_tiles = columns * rows;
  TCOD_Tileset* tileset = TCOD_tileset_new(width / columns, height / rows);
  if (!tileset) {
    return NULL;
  }
  if (TCOD_tileset_reserve(tileset, font_tiles) < 0) {
    TCOD_tileset_delete(tileset);
    return NULL;
  }
  tileset->tiles_count = font_tiles;
  tileset->virtual_columns = columns;
  // Check for a color key in the first tile.
  const struct TCOD_ColorRGBA* color_key = &pixels[0];
  for (int y = 0; y < tileset->tile_height; ++y) {
    for (int x = 0; x < tileset->tile_width; ++x) {
      struct TCOD_ColorRGBA pixel = pixels[y * width + x];
      if (color_key &&
          (pixel.r != color_key->r || pixel.g != color_key->g || pixel.b != color_key->b || pixel.a != color_key->a)) {
        color_key = NULL;
      }
    }
  }
  for (int tile_id = 0; tile_id < font_tiles; ++tile_id) {
    int font_x = tile_id % columns;
    int font_y = tile_id / columns;
    upload_tile_by_id_normalized(
        tileset,
        tile_id,
        (pixels + font_y * columns * tileset->tile_length + font_x * tileset->tile_width),
        sizeof(*pixels) * width,
        color_key);
  }
  if (!charmap) {
    n = font_tiles;
  }
  for (int i = 0; i < n; ++i) {
    int codepoint = charmap ? charmap[i] : i;
    if (TCOD_tileset_assign_tile(tileset, i, codepoint) < 0) {
      TCOD_tileset_delete(tileset);
      return NULL;
    }
  }
  return tileset;
}
#ifndef TCOD_NO_PNG
TCOD_Tileset* TCOD_tileset_load(const char* filename, int columns, int rows, int n, const int* __restrict charmap) {
  struct TCOD_ColorRGBA* font;
  unsigned int font_width;
  unsigned int font_height;
  unsigned int lodepng_err;
  lodepng_err = lodepng_decode32_file((unsigned char**)&font, &font_width, &font_height, filename);
  if (lodepng_err) {
    TCOD_set_errorvf("Error loading font image: %s\n%s", filename ? filename : "", lodepng_error_text(lodepng_err));
    return NULL;  // Error decoding file.
  }
  TCOD_Tileset* tileset = TCOD_tileset_load_raw((int)font_width, (int)font_height, font, columns, rows, n, charmap);
  free(font);
  return tileset;
}
TCOD_Tileset* TCOD_tileset_load_mem(
    size_t buffer_length, const unsigned char* buffer, int columns, int rows, int n, const int* __restrict charmap) {
  struct TCOD_ColorRGBA* pixels;
  unsigned int width;
  unsigned int height;
  unsigned int lodepng_err = lodepng_decode32((unsigned char**)&pixels, &width, &height, buffer, buffer_length);
  if (lodepng_err) {
    TCOD_set_errorvf("Error decoding font image:\n%s", lodepng_error_text(lodepng_err));
    return NULL;
  }
  TCOD_Tileset* tileset = TCOD_tileset_load_raw((int)width, (int)height, pixels, columns, rows, n, charmap);
  free(pixels);
  return tileset;
}
#endif  // TCOD_NO_PNG
