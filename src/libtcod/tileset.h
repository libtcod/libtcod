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
#pragma once
#ifndef LIBTCOD_TILESET_H_
#define LIBTCOD_TILESET_H_
#include <stddef.h>

#include "color.h"
#include "config.h"
#include "error.h"

struct TCOD_Tileset;
struct TCOD_TilesetObserver {
  struct TCOD_Tileset* tileset;
  struct TCOD_TilesetObserver* next;
  void* userdata;
  void (*on_observer_delete)(struct TCOD_TilesetObserver* observer);
  int (*on_tile_changed)(struct TCOD_TilesetObserver* observer, int tile_id);
};
/**
    @brief A container for libtcod tileset graphics.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
struct TCOD_Tileset {
  int tile_width;
  int tile_height;
  int tile_length;
  int tiles_capacity;
  int tiles_count;
  struct TCOD_ColorRGBA* __restrict pixels;
  int character_map_length;
  int* __restrict character_map;
  struct TCOD_TilesetObserver* observer_list;
  int virtual_columns;
  volatile int ref_count;
};
typedef struct TCOD_Tileset TCOD_Tileset;
// clang-format off
// Character maps are defined in this way so that the C and C++ API don't duplicate them.
#define TCOD_CHARMAP_CP437_ {\
    0x0000, 0x263A, 0x263B, 0x2665, 0x2666, 0x2663, 0x2660, 0x2022,\
    0x25D8, 0x25CB, 0x25D9, 0x2642, 0x2640, 0x266A, 0x266B, 0x263C,\
    0x25BA, 0x25C4, 0x2195, 0x203C, 0x00B6, 0x00A7, 0x25AC, 0x21A8,\
    0x2191, 0x2193, 0x2192, 0x2190, 0x221F, 0x2194, 0x25B2, 0x25BC,\
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,\
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,\
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,\
    0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,\
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,\
    0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,\
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,\
    0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,\
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,\
    0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,\
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,\
    0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x2302,\
    0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7,\
    0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5,\
    0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9,\
    0x00FF, 0x00D6, 0x00DC, 0x00A2, 0x00A3, 0x00A5, 0x20A7, 0x0192,\
    0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA,\
    0x00BF, 0x2310, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB,\
    0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,\
    0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,\
    0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F,\
    0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,\
    0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B,\
    0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,\
    0x03B1, 0x00DF, 0x0393, 0x03C0, 0x03A3, 0x03C3, 0x00B5, 0x03C4,\
    0x03A6, 0x0398, 0x03A9, 0x03B4, 0x221E, 0x03C6, 0x03B5, 0x2229,\
    0x2261, 0x00B1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00F7, 0x2248,\
    0x00B0, 0x2219, 0x00B7, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0,\
}
#define TCOD_CHARMAP_TCOD_ {\
    0x20,   0x21,   0x22,   0x23,   0x24,   0x25,   0x26,   0x27,\
    0x28,   0x29,   0x2A,   0x2B,   0x2C,   0x2D,   0x2E,   0x2F,\
    0x30,   0x31,   0x32,   0x33,   0x34,   0x35,   0x36,   0x37,\
    0x38,   0x39,   0x3A,   0x3B,   0x3C,   0x3D,   0x3E,   0x3F,\
    0x40,   0x5B,   0x5C,   0x5D,   0x5E,   0x5F,   0x60,   0x7B,\
    0x7C,   0x7D,   0x7E,   0x2591, 0x2592, 0x2593, 0x2502, 0x2500,\
    0x253C, 0x2524, 0x2534, 0x251C, 0x252C, 0x2514, 0x250C, 0x2510,\
    0x2518, 0x2598, 0x259D, 0x2580, 0x2596, 0x259A, 0x2590, 0x2597,\
    0x2191, 0x2193, 0x2190, 0x2192, 0x25B2, 0x25BC, 0x25C4, 0x25BA,\
    0x2195, 0x2194, 0x2610, 0x2611, 0x25CB, 0x25C9, 0x2551, 0x2550,\
    0x256C, 0x2563, 0x2569, 0x2560, 0x2566, 0x255A, 0x2554, 0x2557,\
    0x255D, 0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,\
    0x41,   0x42,   0x43,   0x44,   0x45,   0x46,   0x47,   0x48,\
    0x49,   0x4A,   0x4B,   0x4C,   0x4D,   0x4E,   0x4F,   0x50,\
    0x51,   0x52,   0x53,   0x54,   0x55,   0x56,   0x57,   0x58,\
    0x59,   0x5A,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,\
    0x61,   0x62,   0x63,   0x64,   0x65,   0x66,   0x67,   0x68,\
    0x69,   0x6A,   0x6B,   0x6C,   0x6D,   0x6E,   0x6F,   0x70,\
    0x71,   0x72,   0x73,   0x74,   0x75,   0x76,   0x77,   0x78,\
    0x79,   0x7A,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,\
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,\
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,\
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,\
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,\
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,\
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,\
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,\
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,\
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,\
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,\
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,\
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,\
}
// clang-format on
/**
    A character mapping of a Code Page 437 tileset to Unicode.
    \rst
    .. versionadded:: 1.19
    \endrst
 */
static const int TCOD_CHARMAP_CP437[256] = TCOD_CHARMAP_CP437_;
/**
    A character mapping of a deprecated TCOD tileset to Unicode.
    \rst
    .. versionadded:: 1.19
    \endrst
 */
static const int TCOD_CHARMAP_TCOD[256] = TCOD_CHARMAP_TCOD_;
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
/**
    Create a new tile-set with the given tile size.
    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCOD_NODISCARD
TCOD_PUBLIC TCOD_Tileset* TCOD_tileset_new(int tile_width, int tile_height);
/**
    Delete a tile-set.
    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCOD_PUBLIC void TCOD_tileset_delete(TCOD_Tileset* tileset);
/**
 *  Return the pixel width of tiles in this tileset.
 *
 *  The tileset functions are provisional, the API may change in the future.
 */
TCOD_NODISCARD
TCOD_PUBLIC int TCOD_tileset_get_tile_width_(const TCOD_Tileset* tileset);
/**
 *  Return the pixel height of tiles in this tileset.
 *
 *  The tileset functions are provisional, the API may change in the future.
 */
TCOD_NODISCARD
TCOD_PUBLIC int TCOD_tileset_get_tile_height_(const TCOD_Tileset* tileset);
/**
 *  Fetch a tile, outputting its data to a pixel buffer.
 *
 *  `codepoint` is the index for the tile.  Unicode is recommend.
 *
 *  `buffer` is a pointer to a contiguous row-major array of pixels.  The tile
 *  data will be outputted here.  This pointer can be NULL if you only want to
 *  know if the tileset has a specific tile.
 *
 *  Returns 0 if the tile exists.  Returns a negative value on an error or if
 *  the tileset does not have a tile for this codepoint.
 *
 *  The tileset functions are provisional, the API may change in the future.
 */
TCOD_NODISCARD
TCOD_PUBLIC TCOD_Error
TCOD_tileset_get_tile_(const TCOD_Tileset* __restrict tileset, int codepoint, struct TCOD_ColorRGBA* __restrict buffer);
/**
 *  Upload a tile from a pixel buffer into this tileset.
 *
 *  `codepoint` is the index for the tile.  Unicode is recommend.
 *
 *  `buffer` is a pointer to a contiguous row-major array of pixels.
 *  This can not be NULL.
 *
 *  The tileset functions are provisional, the API may change in the future.
 */
TCOD_NODISCARD
TCOD_PUBLIC TCOD_Error
TCOD_tileset_set_tile_(TCOD_Tileset* __restrict tileset, int codepoint, const struct TCOD_ColorRGBA* __restrict buffer);

#ifndef TCOD_NO_PNG
/**
    Load a PNG font as a tilesheet and return a TCOD_Tileset.

    `filename` is the path to a PNG file.

    `columns` and `rows` are the shape of the tileset in the image.  The tile
    size will be derived from these parameters and the size of the image.

    `charmap[n]` is an array of which codepoints to assign to which tiles.
    Tiles are assigned in row-major order.
    `TCOD_CHARMAP_CP437` or `TCOD_CHARMAP_TCOD` could be used here.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCOD_NODISCARD
TCOD_PUBLIC TCOD_Tileset* TCOD_tileset_load(
    const char* filename, int columns, int rows, int n, const int* __restrict charmap);

/**
    Load a PNG font from memory and return a TCOD_Tileset.

    `buffer[buffer_length]` is the PNG data to load.

    The remaining parameters are the same as `TCOD_tileset_load`.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCOD_NODISCARD
TCOD_PUBLIC TCOD_Tileset* TCOD_tileset_load_mem(
    size_t buffer_length, const unsigned char* buffer, int columns, int rows, int n, const int* __restrict charmap);
#endif  // TCOD_NO_PNG

/**
    Load raw RGBA data and return a TCOD_Tileset.

    `pixels[width*height]` is a row-major RGBA-ordered byte array.

    The remaining parameters are the same as `TCOD_tileset_load`.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCOD_NODISCARD
TCOD_PUBLIC TCOD_Tileset* TCOD_tileset_load_raw(
    int width,
    int height,
    const struct TCOD_ColorRGBA* __restrict pixels,
    int columns,
    int rows,
    int n,
    const int* __restrict charmap);
/***************************************************************************
    @brief Assign a codepoint to an existing tile based on its tile ID.

    @param tileset A TCOD_Tileset pointer, must not be NULL.
    @param tile_id The index of the tile.
    @param codepoint The Unicode codepoint to associate with tile_id.
    @return Returns a negative value on error.
 */
TCOD_NODISCARD
TCOD_PUBLIC int TCOD_tileset_assign_tile(struct TCOD_Tileset* tileset, int tile_id, int codepoint);
/**
 *  Return a pointer to the tile for `codepoint`.
 *
 *  Returns NULL if no tile exists for codepoint.
 */
TCOD_NODISCARD
TCOD_PUBLIC const struct TCOD_ColorRGBA* TCOD_tileset_get_tile(const TCOD_Tileset* tileset, int codepoint);
/**
 *  Return a new observer to this tileset.
 *
 *  For internal use.
 */
TCOD_NODISCARD
struct TCOD_TilesetObserver* TCOD_tileset_observer_new(struct TCOD_Tileset* tileset);
/**
 *  Delete an existing observer.
 *
 *  Will call this observers on_observer_delete callback.
 *
 *  For internal use.
 */
void TCOD_tileset_observer_delete(struct TCOD_TilesetObserver* observer);
/**
 *  Called to notify any observers that a tile has been changed.  This may
 *  cause running atlases to update or mark cache consoles as dirty.
 *
 *  For internal use.
 */
void TCOD_tileset_notify_tile_changed(TCOD_Tileset* tileset, int tile_id);
/**
 *  Reserve memory for a specific amount of tiles.
 *
 *  For internal use.
 */
TCOD_NODISCARD
TCOD_Error TCOD_tileset_reserve(TCOD_Tileset* tileset, int desired);
#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
#endif  // LIBTCOD_TILESET_H_
