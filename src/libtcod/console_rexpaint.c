/* BSD 3-Clause License
 *
 * Copyright © 2008-2026, Jice and the libtcod contributors.
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

#include <assert.h>
#include <errno.h>
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

// REXPaint Specification:
// https://steveasleep.com/rexpaint_manual.html#appendix-b:-.xp-format-specification-(and-import-libraries)

#pragma pack(push, 1)
/// @brief REXPaint header struct, always at the beginning of the gzip stream.
typedef struct RexPaintHeader {
  int32_t version;  // Typically ``-1`` is expected here.
  int32_t layer_count;  // Official specification is 1 to 9 layer but more can be handled
} RexPaintHeader;
/// @brief The struct header at the start of each REXPaint layer.
typedef struct RexPaintLayerChunk {
  int32_t width;
  int32_t height;
} RexPaintLayerChunk;
/// @brief The layout of REXPaint's tile data.
typedef struct RexPaintTile {
  int32_t ch;  // REXPaint uses EASCII but libtcod will typically use UCS-4. Any decoding happens later.
  TCOD_ColorRGB fg;
  TCOD_ColorRGB bg;
} RexPaintTile;
#pragma pack(pop)

/// @brief REXPaint uses this color as its transparency key color.
static const TCOD_ColorRGB XP_KEY_COLOR = {255, 0, 255};

/// @brief Enum for the tagged union ZipIO.
enum ZipType {
  ZIP_TYPE_UNDEFINED,
  ZIP_TYPE_FILE,
  ZIP_TYPE_STREAM,
};
/// @brief How a generic stream is configured.
enum ZipMode {
  ZIP_MODE_UNDEFINED,
  ZIP_MODE_READ,  // This stream is primarily for reading/decompression
  ZIP_MODE_WRITE,  // This stream is primarily for writing/compression
};
/// @brief In-memory zipped compression/decompression stream.
struct ZipStreamIO {
  enum ZipType type;  // Always ZIP_TYPE_STREAM
  enum ZipMode mode;  // Stream compression/decompression configuration
  z_stream z_stream;
};
/// @brief Zipped file stream.
struct ZipFileIO {
  enum ZipType type;  // Always ZIP_TYPE_FILE
  enum ZipMode mode;  // File read/write mode
  gzFile gz_file;
};
/// @brief Generic zip stream.
typedef union ZipIO {
  enum ZipType type;  // Union tag
  struct ZipStreamIO stream;
  struct ZipFileIO file;
} ZipIO;

/// @brief Read from a generic zip stream.
/// @param zip Must be readable.
/// @param out_n Bytes length of the output buffer.
/// @param out_buffer Output buffer to read data to.
/// @return Negative value on error.
TCOD_NODISCARD
static TCOD_Error zip_read(ZipIO* zip, unsigned int out_n, void* out_buffer) {
  if (zip->stream.mode != ZIP_MODE_READ) {
    assert(false);
    return TCOD_E_INVALID_ARGUMENT;
  }
  switch (zip->type) {
    case ZIP_TYPE_FILE:
      const int z_errno = gzread(zip->file.gz_file, out_buffer, out_n);
      if (z_errno < 0) return TCOD_set_errorvf("Error decoding REXPaint file: %s", gzerror(zip->file.gz_file, NULL));
      return TCOD_E_OK;
    case ZIP_TYPE_STREAM:
      zip->stream.z_stream.next_out = (Bytef*)out_buffer;
      zip->stream.z_stream.avail_out = out_n;
      if (inflate(&zip->stream.z_stream, Z_SYNC_FLUSH) < 0) {
        return TCOD_set_errorvf("Decoding error: %s", zip->stream.z_stream.msg);
      }
      if (zip->stream.z_stream.avail_out != 0) return TCOD_set_errorv("Unexpected end of stream.");
      return TCOD_E_OK;
    default:
    case ZIP_TYPE_UNDEFINED:
      assert(false);
      return TCOD_E_INVALID_ARGUMENT;
  }
}
/// @brief Write to a generic zip stream.
/// @param zip Must be writeable.
/// @param in_n Bytes length of the input buffer.
/// @param in_buffer Input buffer to save.
/// @return Negative value on error.
TCOD_NODISCARD
static TCOD_Error zip_write(ZipIO* zip, unsigned int in_n, const void* in_buffer) {
  if (zip->stream.mode != ZIP_MODE_WRITE) {
    assert(false);
    return TCOD_E_INVALID_ARGUMENT;
  }
  switch (zip->type) {
    case ZIP_TYPE_FILE:
      const int z_errno = gzwrite(zip->file.gz_file, in_buffer, in_n);
      if (z_errno < 0) return TCOD_set_errorvf("Error encoding file: %s", gzerror(zip->file.gz_file, NULL));
      return TCOD_E_OK;
    case ZIP_TYPE_STREAM:
      zip->stream.z_stream.next_in = (Bytef*)in_buffer;
      zip->stream.z_stream.avail_in = in_n;
      if (deflate(&zip->stream.z_stream, Z_NO_FLUSH) < 0) {
        return TCOD_set_errorvf("Error encoding: %s", zip->stream.z_stream.msg);
      }
      return TCOD_E_OK;
    default:
    case ZIP_TYPE_UNDEFINED:
      assert(false);
      return TCOD_E_INVALID_ARGUMENT;
  }
}
/// @brief Flush and close a generic zip stream.
/// @param zip Generic zip union.
/// @return Negative value on error, such as a file failing to be written.
static TCOD_Error zip_close(ZipIO* zip) {
  switch (zip->type) {
    case ZIP_TYPE_FILE:
      if (zip->file.gz_file) {
        const int z_errno = gzclose(zip->file.gz_file);
        zip->file.gz_file = NULL;
        if (z_errno == Z_ERRNO) return TCOD_set_errorvf("File error: %s", strerror(errno));
      }
      break;
    case ZIP_TYPE_STREAM:
      const int z_errno =
          (zip->stream.mode == ZIP_MODE_WRITE ? deflateEnd(&zip->stream.z_stream) : inflateEnd(&zip->stream.z_stream));
      if (z_errno != Z_OK) return TCOD_set_errorvf("Stream error: %s", zip->stream.z_stream.msg);
      break;
    default:
    case ZIP_TYPE_UNDEFINED:
      assert(false);
      break;
  }
  zip->type = ZIP_TYPE_UNDEFINED;
  return TCOD_E_OK;
}

/// @brief Convert a list of consoles into a single console, deleting the list when done.
///     Follows REXPaint's rules for transparency.
/// @param console_list Will be taken and deleted by this call.
/// @return A new console or NULL on error.
static TCOD_Console* combine_console_list(TCOD_list_t console_list) {
  TCOD_Console* main_console;
  if (!console_list) {
    return NULL;
  }
  /* Reverse the list so that elements will be dequeued. */
  TCOD_list_reverse(console_list);
  main_console = TCOD_list_pop(console_list);
  while (!TCOD_list_is_empty(console_list)) {
    TCOD_Console* console = TCOD_list_pop(console_list);
    TCOD_console_set_key_color(console, XP_KEY_COLOR);  // Set key color before blit.
    // This blit may fail if the consoles do not match shapes.
    TCOD_console_blit(console, 0, 0, 0, 0, main_console, 0, 0, 1.0f, 1.0f);
    TCOD_console_delete(console);
  }
  TCOD_list_delete(console_list);
  return main_console;
}
/// @brief Convert a REXPaint tile to a Console tile and output it to a console.
/// @param i Index of the REXPaint tile.
/// @param xp_tile Raw uncompressed data of the REXPaint tile.
/// @param console Output console with the same width/height as the input.
static void xp_tile_to_console_tile(int i, const RexPaintTile* xp_tile, TCOD_Console* console) {
  // Input is column-major, output is row-major
  const int x = i / console->h;
  const int y = i % console->h;
  console->tiles[x + y * console->w] = (TCOD_ConsoleTile){
      SDL_Swap32LE(xp_tile->ch),
      {xp_tile->fg.r, xp_tile->fg.g, xp_tile->fg.b, 0xff},
      {xp_tile->bg.r, xp_tile->bg.g, xp_tile->bg.b, 0xff},
  };
}

/// @brief Read a console from a generic stream.
/// @param zip Input zip for reading.
/// @param out New console will be outputted here.
/// @return Negative value on error.
static TCOD_Error console_from_zip(ZipIO* zip, TCOD_Console** out) {
  RexPaintLayerChunk xp_layer;
  TCOD_Error error = zip_read(zip, sizeof(xp_layer), &xp_layer);
  if (error < 0) return error;
  // Untrusted width/height input values
  xp_layer.width = SDL_Swap32LE(xp_layer.width);
  xp_layer.height = SDL_Swap32LE(xp_layer.height);
  *out = TCOD_console_new(xp_layer.width, xp_layer.height);
  if (!*out) return TCOD_E_ERROR;
  const int layer_total = xp_layer.width * xp_layer.height;
  for (int i = 0; i < layer_total; ++i) {
    RexPaintTile xp_tile;
    error = zip_read(zip, sizeof(xp_tile), &xp_tile);
    if (error < 0) return error;
    xp_tile_to_console_tile(i, &xp_tile, *out);
  }
  return TCOD_E_OK;
}

int TCOD_load_xp(const char* path, int n, TCOD_Console** out) {
  ZipIO zip = {
      .file = {
          .type = ZIP_TYPE_FILE,
          .mode = ZIP_MODE_READ,
          .gz_file = gzopen(path, "rb"),
      }};
  if (!zip.file.gz_file) {
    return TCOD_set_errorvf("Could not open file: '%s'", path);
  }
  RexPaintHeader xp_header;
  TCOD_Error error = zip_read(&zip, sizeof(xp_header), &xp_header);
  if (error < 0) {
    zip_close(&zip);
    return error;
  }
  xp_header.version = SDL_Swap32LE(xp_header.version);
  xp_header.layer_count = SDL_Swap32LE(xp_header.layer_count);
  if (n <= 0 || !out) {
    zip_close(&zip);
    return xp_header.layer_count;
  }
  for (int i = 0; i < n; ++i) {
    if (console_from_zip(&zip, &out[i]) < 0) {
      for (; i >= 0; --i) {
        TCOD_console_delete(out[i]);
        out[i] = NULL;
      }
      zip_close(&zip);
      return TCOD_E_ERROR;
    }
  }
  zip_close(&zip);
  return xp_header.layer_count;
}

/// @brief Append a console to a generic zip.
/// @param console Console to save, should not be NULL.
/// @param zip Output for writing.
/// @return Negative values on failure.
static TCOD_Error console_to_zip(const TCOD_Console* console, ZipIO* zip) {
  if (!console) return TCOD_set_errorv("Console was NULL.");
  if (console->w < 0 || console->h < 0) return TCOD_set_errorv("Console data is corrupt.");
  RexPaintLayerChunk xp_layer = {.width = SDL_Swap32LE(console->w), .height = SDL_Swap32LE(console->h)};
  TCOD_Error error = zip_write(zip, sizeof(xp_layer), &xp_layer);
  if (error < 0) return error;
  for (int x = 0; x < console->w; ++x) {
    for (int y = 0; y < console->h; ++y) {
      const TCOD_ConsoleTile tile = console->tiles[x + y * console->w];
      const RexPaintTile xp_tile = {
          SDL_Swap32LE(tile.ch),
          {tile.fg.r, tile.fg.g, tile.fg.b},
          {tile.bg.r, tile.bg.g, tile.bg.b},
      };
      error = zip_write(zip, sizeof(xp_tile), &xp_tile);
      if (error < 0) return error;
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
  ZipIO zip = {
      .file = {
          .type = ZIP_TYPE_FILE,
          .mode = ZIP_MODE_WRITE,
          .gz_file = gzopen(path, "wb"),
      }};
  if (!zip.file.gz_file) {
    return TCOD_set_errorvf("Could not open the file for writing: %s", path);
  }
  const int z_errno = gzsetparams(zip.file.gz_file, compress_level, Z_DEFAULT_STRATEGY);
  if (z_errno < 0) {
    TCOD_set_errorvf("Error encoding: %s", gzerror(zip.file.gz_file, NULL));
    zip_close(&zip);
    return TCOD_E_ERROR;
  }
  xp_header.version = SDL_Swap32LE(-1);
  xp_header.layer_count = SDL_Swap32LE(n);
  TCOD_Error error = zip_write(&zip, sizeof(xp_header), &xp_header);
  if (error < 0) {
    zip_close(&zip);
    return error;
  }
  for (int i = 0; i < n; ++i) {
    error = console_to_zip(consoles[i], &zip);
    if (error < 0) {
      zip_close(&zip);
      return TCOD_E_ERROR;
    }
  }
  return zip_close(&zip);
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
TCOD_Console* TCOD_console_from_xp(const char* filename) {
  return combine_console_list(TCOD_console_list_from_xp(filename));
}
bool TCOD_console_load_xp(TCOD_Console* con, const char* filename) {
  TCOD_Console* xp_console = TCOD_console_from_xp(filename);
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

int TCOD_load_xp_from_memory(int n_data, const unsigned char* data, int n_out, TCOD_Console** out) {
  ZipIO zip = {
      .stream = {
          .type = ZIP_TYPE_STREAM,
          .mode = ZIP_MODE_READ,
          .z_stream = {.next_in = (Bytef*)data, .avail_in = n_data},
      }};
  const int z_errno = inflateInit(&zip.stream.z_stream);
  if (z_errno != Z_OK) return TCOD_set_errorf("Stream error: %s", zip.stream.z_stream.msg);
  RexPaintHeader header;
  TCOD_Error error = zip_read(&zip, sizeof(header), &header);
  if (error < 0) {
    zip_close(&zip);
    return error;
  }
  header.version = SDL_Swap32LE(header.version);
  header.layer_count = SDL_Swap32LE(header.layer_count);
  if (n_out <= 0 || !out) {
    zip_close(&zip);
    return header.layer_count;
  }
  for (int i = 0; i < n_out && i < header.layer_count; ++i) {
    if (console_from_zip(&zip, &out[i]) < 0) {
      zip_close(&zip);
      for (; i >= 0; --i) {
        TCOD_console_delete(out[i]);
        out[i] = NULL;
      }
      return TCOD_E_ERROR;
    }
  }
  zip_close(&zip);
  return header.layer_count;
}

int TCOD_save_xp_to_memory(
    int n_consoles, const TCOD_Console* const* consoles, int n_out, unsigned char* out, int compression_level) {
  ZipIO zip = {
      .stream = {
          .type = ZIP_TYPE_STREAM,
          .mode = ZIP_MODE_WRITE,
          .z_stream = {.next_out = out, .avail_out = n_out},
      }};
  const int z_errno = deflateInit(&zip.stream.z_stream, compression_level);
  if (z_errno != Z_OK) return TCOD_set_errorf("Stream error: %s", zip.stream.z_stream.msg);
  size_t total_size = sizeof(RexPaintHeader) + sizeof(RexPaintLayerChunk) * n_consoles;
  for (int i = 0; i < n_consoles; ++i) {
    total_size += consoles[i]->w * consoles[i]->h * sizeof(RexPaintTile);
    if (consoles[i]->w < 0 || consoles[i]->h < 0) {
      zip_close(&zip);
      return TCOD_set_errorv("Input console is corrupt.");
    }
  }
  if (n_out <= 0 || !out) {
    int upper_size = deflateBound(&zip.stream.z_stream, (uLong)total_size);
    zip_close(&zip);
    return upper_size;
  }
  const RexPaintHeader header = {.version = SDL_Swap32LE(-1), .layer_count = SDL_Swap32LE(n_consoles)};
  TCOD_Error error = zip_write(&zip, sizeof(header), &header);
  if (error < 0) {
    zip_close(&zip);
    return error;
  }
  for (int i = 0; i < n_consoles; ++i) {
    if (console_to_zip(consoles[i], &zip) < 0) {
      zip_close(&zip);
      return TCOD_E_ERROR;
    }
  }
  if (deflate(&zip.stream.z_stream, Z_FINISH) == Z_STREAM_END) {
    zip_close(&zip);
    return zip.stream.z_stream.total_out;  // Success.
  }
  zip_close(&zip);
  return TCOD_set_errorv("Output buffer was too small.");
}
#endif  // TCOD_NO_ZLIB
