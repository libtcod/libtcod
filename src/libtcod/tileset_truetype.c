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
#include "tileset_truetype.h"

#include <stb_truetype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "globals.h"

// You can look here for a reference on glyph metrics:
// https://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html
TCOD_NODISCARD unsigned char* TCOD_load_binary_file_(const char* path, size_t* size);

struct BBox {
  int xMin;
  int yMin;
  int xMax;
  int yMax;
};
int bbox_width(const struct BBox* bbox) { return bbox->xMax - bbox->xMin; }
int bbox_height(const struct BBox* bbox) { return bbox->yMax - bbox->yMin; }
struct FontLoader {
  const stbtt_fontinfo* __restrict info;
  float scale;
  struct BBox font_bbox;
  struct TCOD_Tileset* tileset;
  struct TCOD_ColorRGBA* tile;
  uint8_t* __restrict tile_alpha;
  int ascent;
  int descent;
  int line_gap;
  float align_x;
  float align_y;
};
/**
 *  Return the bounding box for this glyph.
 */
TCOD_NODISCARD
static struct BBox get_glyph_bbox(const stbtt_fontinfo* font_info, int glyph, float scale) {
  struct BBox bbox;
  stbtt_GetGlyphBitmapBox(font_info, glyph, scale, scale, &bbox.xMin, &bbox.yMin, &bbox.xMax, &bbox.yMax);
  return bbox;
}
/**
 *  Return the shift needed to align this glyph with the current tile size.
 */
void get_glyph_shift(
    const struct FontLoader* __restrict loader, int glyph, float* __restrict shift_x, float* __restrict shift_y) {
  struct BBox bbox = get_glyph_bbox(loader->info, glyph, 1.0f);
  *shift_x = (loader->tileset->tile_width - bbox_width(&bbox) * loader->scale) * loader->align_x;
  *shift_y =
      (bbox.yMin + loader->ascent) * loader->scale +
      (int)((loader->tileset->tile_height - (loader->ascent - loader->descent) * loader->scale) * loader->align_y);
}
/**
 *  Render the tile for a specific glyph.
 */
void render_glyph(const struct FontLoader* __restrict loader, int glyph) {
  float shift_x;
  float shift_y;
  const struct TCOD_Tileset* tileset = loader->tileset;
  get_glyph_shift(loader, glyph, &shift_x, &shift_y);
  for (int i = 0; i < tileset->tile_length; ++i) {
    loader->tile[i].r = loader->tile[i].g = loader->tile[i].b = 255;
    loader->tile[i].a = 0;
    loader->tile_alpha[i] = 0;
  }
  stbtt_MakeGlyphBitmapSubpixel(
      loader->info,
      loader->tile_alpha,
      tileset->tile_width,
      tileset->tile_height,
      tileset->tile_width,
      loader->scale,
      loader->scale,
      shift_x,
      shift_y,
      glyph);
  for (int img_y = 0; img_y < tileset->tile_height; ++img_y) {
    for (int img_x = 0; img_x < tileset->tile_width; ++img_x) {
      int alpha_y = img_y - (int)shift_y;
      int alpha_x = img_x - (int)shift_x;
      if (alpha_y < 0 || tileset->tile_height <= alpha_y) {
        continue;
      }
      if (alpha_x < 0 || tileset->tile_width <= alpha_x) {
        continue;
      }
      loader->tile[img_y * tileset->tile_width + img_x].a = loader->tile_alpha[alpha_y * tileset->tile_width + alpha_x];
    }
  }
}
TCOD_NODISCARD
static struct TCOD_Tileset* tileset_from_ttf(const stbtt_fontinfo* font_info, int tile_width, int tile_height) {
  struct FontLoader loader = {
      .info = font_info,
      .scale = stbtt_ScaleForPixelHeight(font_info, (float)tile_height),
      .align_x = 0.5f,
      .align_y = 0.5f,
  };
  stbtt_GetFontBoundingBox(
      font_info, &loader.font_bbox.xMin, &loader.font_bbox.yMin, &loader.font_bbox.xMax, &loader.font_bbox.yMax);
  stbtt_GetFontVMetrics(font_info, &loader.ascent, &loader.descent, &loader.line_gap);
  if (tile_width <= 0) {
    tile_width = (int)((float)(bbox_width(&loader.font_bbox)) * loader.scale);
  }
  float font_width = bbox_width(&loader.font_bbox) * loader.scale;
  if (font_width > tile_width) {
    // Shrink the font to fit its tile width.
    loader.scale *= (float)tile_width / font_width;
  }
  loader.tileset = TCOD_tileset_new(tile_width, tile_height);
  loader.tile = malloc(sizeof(*loader.tile) * loader.tileset->tile_length);
  loader.tile_alpha = malloc(sizeof(*loader.tile_alpha) * loader.tileset->tile_length);
  if (!loader.tileset || !loader.tile || !loader.tile_alpha) {
    TCOD_tileset_delete(loader.tileset);
    free(loader.tile);
    free(loader.tile_alpha);
    return NULL;
  }
  for (int codepoint = 1; codepoint <= 0x1ffff; ++codepoint) {
    int glyph = stbtt_FindGlyphIndex(font_info, codepoint);
    if (!glyph) {
      continue;
    }
    render_glyph(&loader, glyph);
    if (TCOD_tileset_set_tile_(loader.tileset, codepoint, loader.tile) < 0) {
      TCOD_set_errorv("Out of memory while loading tileset.");
      TCOD_tileset_delete(loader.tileset);
      loader.tileset = NULL;
      break;
    }
  }
  free(loader.tile);
  free(loader.tile_alpha);
  return loader.tileset;
}

TCOD_Tileset* TCOD_load_truetype_font_(const char* path, int tile_width, int tile_height) {
  unsigned char* font_data = TCOD_load_binary_file_(path, NULL);
  if (!font_data) {
    return NULL;
  }
  stbtt_fontinfo font_info;
  if (!stbtt_InitFont(&font_info, font_data, 0)) {
    TCOD_set_errorvf("Failed to read font file:\n%s", path);
    free(font_data);
    return NULL;
  }
  struct TCOD_Tileset* tileset = tileset_from_ttf(&font_info, tile_width, tile_height);
  free(font_data);
  return tileset;
}
TCOD_Error TCOD_tileset_load_truetype_(const char* path, int tile_width, int tile_height) {
  TCOD_Tileset* tileset = TCOD_load_truetype_font_(path, tile_width, tile_height);
  if (!tileset) {
    return TCOD_E_ERROR;
  }
  TCOD_set_default_tileset(tileset);
  TCOD_tileset_delete(tileset);
  return TCOD_E_OK;
}
