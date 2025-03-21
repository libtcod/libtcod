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
#include "console_rexpaint.h"
#ifndef TCOD_NO_ZLIB

#include <limits.h>
#include <stdlib.h>
#include <zlib.h>

#include "color.h"
#include "console.h"
#include "console_types.h"
#include "libtcod_int.h"

#ifndef NO_SDL
#include <SDL3/SDL_endian.h>
#else
// Ignore endianness for now if SDL is missing.
#define SDL_Swap32LE(x) (x)
#endif  // NO_SDL

#pragma pack(push, 1)
/** REXPaint header struct, always at the beginning of the gzip stream. */
typedef struct RexPaintHeader {
  int32_t version;
  int32_t layer_count;
} RexPaintHeader;
/** The struct at the start of each REXPaint layer. */
typedef struct RexPaintLayerChunk {
  int32_t width;
  int32_t height;
} RexPaintLayerChunk;
/** The layout of REXPaint's tile data. */
typedef struct RexPaintTile {
  int32_t ch;
  TCOD_color_t fg;
  TCOD_color_t bg;
} RexPaintTile;
#pragma pack(pop)
/* Convert a list of consoles into a single console, deleting the list.
  Follows REXPaint's rules for transparency. */
static TCOD_console_t combine_console_list(TCOD_list_t console_list) {
  TCOD_Console* main_console;
  if (!console_list) {
    return NULL;
  }
  /* Reverse the list so that elements will be dequeued. */
  TCOD_list_reverse(console_list);
  main_console = TCOD_list_pop(console_list);
  while (!TCOD_list_is_empty(console_list)) {
    TCOD_console_t console = TCOD_list_pop(console_list);
    /* Set key color to {255, 0, 255} before blit. */
    TCOD_console_set_key_color(console, TCOD_fuchsia);
    /* This blit may fail if the consoles do not match shapes. */
    TCOD_console_blit(console, 0, 0, 0, 0, main_console, 0, 0, 1.0f, 1.0f);
    TCOD_console_delete(console);
  }
  TCOD_list_delete(console_list);
  return main_console;
}
void xp_tile_to_console_tile(int i, const RexPaintTile* xp_tile, TCOD_Console* console) {
  const int x = i / console->h;
  const int y = i % console->h;
  console->tiles[x + y * console->w] = (TCOD_ConsoleTile){
      SDL_Swap32LE(xp_tile->ch),
      {xp_tile->fg.r, xp_tile->fg.g, xp_tile->fg.b, 0xff},
      {xp_tile->bg.r, xp_tile->bg.g, xp_tile->bg.b, 0xff},
  };
}
/**
    Read a console from a REXPaint gzFile file.
 */
static int console_from_xp_file(gzFile gz_file, TCOD_Console** out) {
  RexPaintLayerChunk xp_layer;
  int z_errno = gzread(gz_file, &xp_layer, sizeof(xp_layer));
  if (z_errno < 0) {
    return TCOD_set_errorvf("Error decoding REXPaint file: %s", gzerror(gz_file, &z_errno));
  }
  xp_layer.width = SDL_Swap32LE(xp_layer.width);
  xp_layer.height = SDL_Swap32LE(xp_layer.height);
  *out = TCOD_console_new(xp_layer.width, xp_layer.height);
  if (!*out) return -1;
  const int layer_total = xp_layer.width * xp_layer.height;
  for (int i = 0; i < layer_total; ++i) {
    RexPaintTile xp_tile;
    z_errno = gzread(gz_file, &xp_tile, sizeof(xp_tile));
    if (z_errno < 0) {
      return TCOD_set_errorvf("Error decoding REXPaint file: %s", gzerror(gz_file, &z_errno));
    }
    xp_tile_to_console_tile(i, &xp_tile, *out);
  }
  return TCOD_E_OK;
}

int TCOD_load_xp(const char* path, int n, TCOD_Console** out) {
  int z_errno = Z_ERRNO;
  gzFile gz_file = gzopen(path, "rb");
  if (!gz_file) {
    return TCOD_set_errorvf("Could not open file: '%s'", path);
  }
  RexPaintHeader xp_header;
  z_errno = gzread(gz_file, &xp_header, sizeof(xp_header));
  if (z_errno < 0) {
    TCOD_set_errorvf("Error parsing '%s'\n%s", path, gzerror(gz_file, &z_errno));
    gzclose(gz_file);
    return TCOD_E_ERROR;
  }
  xp_header.version = SDL_Swap32LE(xp_header.version);
  xp_header.layer_count = SDL_Swap32LE(xp_header.layer_count);
  if (n <= 0 || !out) {
    gzclose(gz_file);
    return xp_header.layer_count;
  }
  for (int i = 0; i < n; ++i) {
    if (console_from_xp_file(gz_file, &out[i]) < 0) {
      for (; i >= 0; --i) {
        TCOD_console_delete(out[i]);
        out[i] = NULL;
      }
      gzclose(gz_file);
      return TCOD_E_ERROR;
    }
  }
  gzclose(gz_file);
  return xp_header.layer_count;
}
/**
    Append a console to a REXPaint gzFile.
 */
static TCOD_Error console_to_xp_file(const TCOD_Console* console, gzFile gz_file) {
  if (console->w < 0 || console->h < 0) {
    return TCOD_set_errorv("Console data is corrupt.");
  }
  RexPaintLayerChunk xp_layer = {.width = SDL_Swap32LE(console->w), .height = SDL_Swap32LE(console->h)};
  int z_err = gzwrite(gz_file, &xp_layer, sizeof(xp_layer));
  if (z_err < 0) {
    return TCOD_set_errorvf("Error encoding file: %s", gzerror(gz_file, &z_err));
  };
  for (int x = 0; x < console->w; ++x) {
    for (int y = 0; y < console->h; ++y) {
      const TCOD_ConsoleTile tile = console->tiles[x + y * console->w];
      const RexPaintTile xp_tile = {
          SDL_Swap32LE(tile.ch),
          {tile.fg.r, tile.fg.g, tile.fg.b},
          {tile.bg.r, tile.bg.g, tile.bg.b},
      };
      z_err = gzwrite(gz_file, &xp_tile, sizeof(xp_tile));
      if (z_err < 0) {
        return TCOD_set_errorvf("Error encoding: %s", gzerror(gz_file, &z_err));
      };
    }
  }
  return TCOD_E_OK;
}
TCOD_Error TCOD_save_xp(int n, const TCOD_Console* const* consoles, const char* path, int compress_level) {
  if (n < 0) {
    TCOD_set_errorv("n parameter can not be negative");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (n >= 0 && !consoles) {
    TCOD_set_errorv("consoles parameter can not be NULL");
    return TCOD_E_INVALID_ARGUMENT;
  }
  for (int i = 0; i < n; ++i) {
    if (consoles[i] == NULL) {
      TCOD_set_errorvf("consoles[%i] can not be NULL.", i);
      return TCOD_E_INVALID_ARGUMENT;
    }
    if (consoles[i]->w < 0 || consoles[i]->h < 0) {
      return TCOD_set_errorv("Console data is corrupt.");
    }
  }
  if (!path) {
    TCOD_set_errorv("path parameter can not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  RexPaintHeader xp_header;
  gzFile gz_file = gzopen(path, "wb");
  if (!gz_file) {
    return TCOD_set_errorvf("Could not open the file for writing: %s", path);
  }
  int z_err = gzsetparams(gz_file, compress_level, Z_DEFAULT_STRATEGY);
  if (z_err < 0) {
    TCOD_set_errorvf("Error encoding: %s", gzerror(gz_file, &z_err));
    gzclose(gz_file);
    return TCOD_E_ERROR;
  }
  xp_header.version = SDL_Swap32LE(-1);
  xp_header.layer_count = SDL_Swap32LE(n);
  z_err = gzwrite(gz_file, &xp_header, sizeof(xp_header));
  if (z_err < 0) {
    TCOD_set_errorvf("Error encoding: %s", gzerror(gz_file, &z_err));
    gzclose(gz_file);
    return TCOD_E_ERROR;
  }
  for (int i = 0; i < n; ++i) {
    if (console_to_xp_file(consoles[i], gz_file)) {
      z_err = gzclose(gz_file);
      return TCOD_E_ERROR;
    }
  }
  z_err = gzclose(gz_file);
  return TCOD_E_OK;
}

TCOD_list_t TCOD_console_list_from_xp(const char* filename) {
  int layer_count = TCOD_load_xp(filename, 0, NULL);
  if (layer_count < 0) return NULL;
  TCOD_list_t console_list = TCOD_list_allocate(layer_count);
  for (int i = 0; i < layer_count; ++i) TCOD_list_push(console_list, NULL);
  if (TCOD_load_xp(filename, layer_count, (TCOD_Console**)TCOD_list_begin(console_list)) < 0) {
    TCOD_list_delete(console_list);
    console_list = NULL;
  }
  return console_list;
}
TCOD_console_t TCOD_console_from_xp(const char* filename) {
  return combine_console_list(TCOD_console_list_from_xp(filename));
}
bool TCOD_console_load_xp(TCOD_console_t con, const char* filename) {
  TCOD_console_t xp_console = TCOD_console_from_xp(filename);
  if (!xp_console) {
    return false;
  }
  if (TCOD_console_get_width(con) != TCOD_console_get_width(xp_console) ||
      TCOD_console_get_height(con) != TCOD_console_get_height(xp_console)) {
    TCOD_console_delete(xp_console);
    return false;
  }
  TCOD_console_blit(xp_console, 0, 0, 0, 0, con, 0, 0, 1.0f, 1.0f);
  TCOD_console_delete(xp_console);
  return true;
}
bool TCOD_console_save_xp(const TCOD_Console* con, const char* filename, int compress_level) {
  con = TCOD_console_validate_(con);
  return TCOD_save_xp(1, &con, filename, compress_level) == TCOD_E_OK;
}
bool TCOD_console_list_save_xp(TCOD_list_t console_list, const char* filename, int compress_level) {
  return (
      TCOD_save_xp(
          TCOD_list_size(console_list),
          (const TCOD_Console* const*)TCOD_list_begin(console_list),
          filename,
          compress_level) == TCOD_E_OK);
}
/**
    Load a console from a REXPaint chunk from a zlib `stream` to `out`.

    Returns a negative value on error.
 */
static int console_from_xp_stream(z_stream* stream, TCOD_Console** out) {
  RexPaintLayerChunk xp_layer;
  stream->next_out = (unsigned char*)&xp_layer;
  stream->avail_out = sizeof(xp_layer);
  int z_err = inflate(stream, Z_SYNC_FLUSH);
  if (z_err < 0) {
    return TCOD_set_errorvf("Decoding error: %s", stream->msg);
  }
  xp_layer.width = SDL_Swap32LE(xp_layer.width);
  xp_layer.height = SDL_Swap32LE(xp_layer.height);
  *out = TCOD_console_new(xp_layer.width, xp_layer.height);
  if (!*out) return -1;
  const int layer_total = xp_layer.width * xp_layer.height;
  for (int i = 0; i < layer_total; ++i) {
    RexPaintTile xp_tile;
    stream->next_out = (Bytef*)&xp_tile;
    stream->avail_out = sizeof(xp_tile);
    if (inflate(stream, Z_SYNC_FLUSH) < 0) {
      return TCOD_set_errorvf("Decoding error: %s", stream->msg);
    }
    xp_tile_to_console_tile(i, &xp_tile, *out);
  }
  return TCOD_E_OK;
}

int TCOD_load_xp_from_memory(int n_data, const unsigned char* data, int n_out, TCOD_Console** out) {
  z_stream stream = {.next_in = (Bytef*)data, .avail_in = n_data};
  inflateInit(&stream);
  RexPaintHeader header;
  stream.next_out = (Bytef*)&header;
  stream.avail_out = sizeof(header);
  if (inflate(&stream, Z_SYNC_FLUSH) < 0) {
    TCOD_set_errorvf("Decoding error: %s", stream.msg);
    inflateEnd(&stream);
    return TCOD_E_ERROR;
  }
  header.version = SDL_Swap32LE(header.version);
  header.layer_count = SDL_Swap32LE(header.layer_count);
  if (n_out <= 0 || !out) {
    inflateEnd(&stream);
    return header.layer_count;
  }
  for (int i = 0; i < n_out && i < header.layer_count; ++i) {
    if (console_from_xp_stream(&stream, &out[i]) < 0) {
      inflateEnd(&stream);
      for (; i >= 0; --i) {
        TCOD_console_delete(out[i]);
        out[i] = NULL;
      }
      return TCOD_E_ERROR;
    }
  }
  inflateEnd(&stream);
  return header.layer_count;
}

static int console_to_xp_stream(const TCOD_Console* console, z_stream* stream) {
  const RexPaintLayerChunk xp_layer = {.width = SDL_Swap32LE(console->w), .height = SDL_Swap32LE(console->h)};
  stream->next_in = (Bytef*)&xp_layer;
  stream->avail_in = sizeof(xp_layer);
  if (deflate(stream, Z_NO_FLUSH) < 0) {
    return TCOD_set_errorvf("Error encoding: %s", stream->msg);
  }
  for (int x = 0; x < console->w; ++x) {
    for (int y = 0; y < console->h; ++y) {
      const TCOD_ConsoleTile in_tile = console->tiles[x + y * console->w];
      const RexPaintTile out_tile = {
          SDL_Swap32LE(in_tile.ch),
          {in_tile.fg.r, in_tile.fg.g, in_tile.fg.b},
          {in_tile.bg.r, in_tile.bg.g, in_tile.bg.b},
      };
      stream->next_in = (Bytef*)&out_tile;
      stream->avail_in = sizeof(out_tile);
      if (deflate(stream, Z_NO_FLUSH) < 0) {
        return TCOD_set_errorvf("Error encoding: %s", stream->msg);
      }
    }
  }
  return TCOD_E_OK;
}

int TCOD_save_xp_to_memory(
    int n_consoles, const TCOD_Console* const* consoles, int n_out, unsigned char* out, int compression_level) {
  z_stream stream = {.next_out = out, .avail_out = n_out};
  deflateInit(&stream, compression_level);
  size_t total_size = sizeof(RexPaintHeader) + sizeof(RexPaintLayerChunk) * n_consoles;
  for (int i = 0; i < n_consoles; ++i) {
    total_size += consoles[i]->w * consoles[i]->h * sizeof(RexPaintTile);
    if (consoles[i]->w < 0 || consoles[i]->h < 0) {
      deflateEnd(&stream);
      return TCOD_set_errorv("Input console is corrupt.");
    }
  }
  if (n_out <= 0 || !out) {
    int upper_size = deflateBound(&stream, (uLong)total_size);
    deflateEnd(&stream);
    return upper_size;
  }
  const RexPaintHeader header = {.version = SDL_Swap32LE(-1), .layer_count = SDL_Swap32LE(n_consoles)};
  stream.next_in = (Bytef*)&header;
  stream.avail_in = sizeof(header);
  if (deflate(&stream, Z_NO_FLUSH)) {
    TCOD_set_errorvf("Error encoding: %s", stream.msg);
    deflateEnd(&stream);
    return TCOD_E_ERROR;
  }
  for (int i = 0; i < n_consoles; ++i) {
    if (console_to_xp_stream(consoles[i], &stream) < 0) {
      deflateEnd(&stream);
      return TCOD_E_ERROR;
    }
  }
  if (deflate(&stream, Z_FINISH) == Z_STREAM_END) {
    deflateEnd(&stream);
    return stream.total_out;  // Success.
  }
  deflateEnd(&stream);
  return TCOD_set_errorv("Output buffer was too small.");
}
#endif  // TCOD_NO_ZLIB
