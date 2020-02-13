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
#ifndef LIBTCOD_TILESET_H_
#define LIBTCOD_TILESET_H_
#ifdef __cplusplus
#include <memory>
#endif // __cplusplus
#include "config.h"
#include "error.h"
#include "color.h"
struct TCOD_Tileset;
struct TCOD_TilesetObserver {
  struct TCOD_Tileset* tileset;
  struct TCOD_TilesetObserver* next;
  void* userdata;
  void (*on_observer_delete)(struct TCOD_TilesetObserver* observer);
  int (*on_tile_changed)(struct TCOD_TilesetObserver* observer, int tile_id);
};
struct TCOD_Tileset {
  int tile_width;
  int tile_height;
  int tile_length;
  int tiles_capacity;
  int tiles_count;
  struct TCOD_ColorRGBA* pixels;
  int character_map_length;
  int* character_map;
  struct TCOD_TilesetObserver* observer_list;
  int virtual_columns;
  volatile int ref_count;
};
typedef struct TCOD_Tileset TCOD_Tileset;
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
/**
 *  Create a new tile-set with the given tile size.
 */
TCOD_NODISCARD
TCOD_PUBLIC TCOD_Tileset* TCOD_tileset_new(int tile_width, int tile_height);
/**
 *  Delete a tile-set.
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
TCOD_PUBLIC TCOD_Error TCOD_tileset_get_tile_(
    const TCOD_Tileset* tileset,
    int codepoint,
    struct TCOD_ColorRGBA* buffer);
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
TCOD_PUBLIC TCOD_Error TCOD_tileset_set_tile_(
    TCOD_Tileset* tileset,
    int codepoint,
    const struct TCOD_ColorRGBA* buffer);
/**
 *  Load a font from a tilesheet.
 */
TCOD_NODISCARD
TCOD_PUBLIC TCOD_Tileset* TCOD_tileset_load(
  const char* filename, int columns, int rows, int n, int* charmap);
/**
 *  Assign a codepoint to an existing tile based on its tile ID.
 *
 *  Returns the tile ID on success.
 *
 *  Returns a negative value on error.
 */
TCOD_NODISCARD
TCOD_PUBLIC int TCOD_tileset_assign_tile(
    struct TCOD_Tileset* tileset, int tile_id, int codepoint);
/**
 *  Return a pointer to the tile for `codepoint`.
 *
 *  Returns NULL if no tile exists for codepoint.
 */
TCOD_NODISCARD
TCOD_PUBLIC const struct TCOD_ColorRGBA* TCOD_tileset_get_tile(
    const TCOD_Tileset* tileset, int codepoint);
/**
 *  Return a new observer to this tileset.
 *
 *  For internal use.
 */
TCOD_NODISCARD
struct TCOD_TilesetObserver* TCOD_tileset_observer_new(
    struct TCOD_Tileset* tileset);
/**
 *  Delete an existing observer.
 *
 *  Will call this observers on_observer_delete callback.
 *
 *  For internal use.
 */
void TCOD_tileset_observer_delete(
    struct TCOD_TilesetObserver* observer);
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
} // extern "C"
namespace tcod {
struct TilesetDeleter {
  void operator()(TCOD_Tileset* tileset) const {
    TCOD_tileset_delete(tileset);
  }
};
typedef std::unique_ptr<TCOD_Tileset, TilesetDeleter> TilesetPtr;
} // namespace tcod
#endif // __cplusplus
#endif /* LIBTCOD_TILESET_TILESET_H_ */
