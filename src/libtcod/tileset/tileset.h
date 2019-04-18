/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
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
#ifndef LIBTCOD_TILESET_TILESET_H_
#define LIBTCOD_TILESET_TILESET_H_
#ifdef __cplusplus
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#endif

#include "../portability.h"
#include "tile.h"
#ifdef __cplusplus
namespace tcod {
namespace tileset {
/**
 *  This is a tile-set resource.
 */
class Tileset {
 public:
  friend class TilesetObserver;
  explicit Tileset(int tile_width, int tile_height):
      tile_width_(std::max(0, tile_width)),
      tile_height_(std::max(0, tile_height)),
      /* The tile at zero is always blank. */
      tiles_{Tile(0, tile_width_, tile_height_)}
  {}

  Tileset(Tileset&&) = default;
  Tileset& operator=(Tileset&&) = default;
  Tileset(const Tileset&) = default;
  Tileset& operator=(const Tileset&) = default;

  ~Tileset() = default;
  /**
   *  Assign `codepoint` to a new `tile` for this Tileset.
   *
   *  `codepoint` is a Unicode character.
   */
  int set_tile(int codepoint, const Image &image)
  {
    return set_tile(Tile(codepoint, image));
  }
  int set_tile(const Tile& tile)
  {
    if (VerifyTile(tile) < 0) { return -1; }
    if (tile.codepoint >= static_cast<int>(character_map_.size())) {
      character_map_.resize(tile.codepoint + 1, -1);
    }
    auto it = std::find(tiles_.begin(), tiles_.end(), tile);
    if (it == tiles_.end()) {
      character_map_[tile.codepoint] = static_cast<int>(tiles_.size());
      tiles_.push_back(tile);
    } else {
      character_map_[tile.codepoint] = static_cast<int>(
          std::distance(tiles_.begin(), it));
    }
    return 0;
  }
  /** Return the width of each tile in this Tileset */
  int get_tile_width() const { return tile_width_; }
  /** Return the height of each tile in this Tileset */
  int get_tile_height() const { return tile_height_; }
  /**
   *  Return a reference to this objects tile vector.
   */
  const std::vector<Tile>& get_tiles() const {
    return tiles_;
  }
  const std::vector<int>& get_character_map() const
  {
    return character_map_;
  }
  Image get_tile_(int codepoint) const {
    if(!has_tile_(codepoint)) {
      return Image(tile_width_, tile_height_);
    }
    return tiles_.at(character_map_.at(codepoint)).get_image();
  }
  bool has_tile_(int codepoint) const {
    return (codepoint < static_cast<int>(character_map_.size())
            && character_map_.at(codepoint) >= 0);
  }
 private:
  /**
   *  Return the tile ID for a specific code-point.
   *
   *  If the code-point is not assigned to a tile then this will return -1.
   */
  int GetTileIndexForCharacter(int codepoint) const {
    if (0 <= codepoint &&
        codepoint < static_cast<int>(character_map_.size())) {
      return character_map_[codepoint];
    }
    return -1;
  }
  /**
   *  Return 0 if this tile will fit in this tile-set.
   *
   *  Return -1 if it can't.
   */
  int VerifyTile(const Tile &tile) const {
    if (tile.width() == tile_width_ && tile.height() == tile_height_) {
      return 0;
    }
    return -1;
  }
  /** Width and height of each tile in pixels. */
  int tile_width_;
  int tile_height_;
  /** An array of tiles. */
  std::vector<Tile> tiles_;
  int tiles_last_known_capacity;
  /** Mapping of Unicode code-points to the tiles of this tile-set. */
  std::vector<int> character_map_;
  std::vector<class TilesetObserver*> observers_;
};
} // namespace tileset
} // namespace tcod
#endif // __cplusplus
#ifdef __cplusplus
/**
 *  C API alias to the tcod::Tileset class.
 */
typedef std::shared_ptr<tcod::tileset::Tileset> TCOD_Tileset;
#else
typedef struct TCOD_Tileset TCOD_Tileset;
#endif
/**
 *  Create a new tile-set with the given tile size.
 */
TCODLIB_CAPI TCOD_Tileset* TCOD_tileset_new(int tile_width, int tile_height);
/**
 *  Delete a tile-set.
 */
TCODLIB_CAPI void TCOD_tileset_delete(TCOD_Tileset* tileset);
/**
 *  Return the pixel width of tiles in this tileset.
 *
 *  The tileset functions are provisional, the API may change in the future.
 */
TCODLIB_CAPI int TCOD_tileset_get_tile_width_(const TCOD_Tileset* tileset);
/**
 *  Return the pixel height of tiles in this tileset.
 *
 *  The tileset functions are provisional, the API may change in the future.
 */
TCODLIB_CAPI int TCOD_tileset_get_tile_height_(const TCOD_Tileset* tileset);
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
TCODLIB_CAPI int TCOD_tileset_get_tile_(
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
TCODLIB_CAPI int TCOD_tileset_set_tile_(
    TCOD_Tileset* tileset,
    int codepoint,
    const struct TCOD_ColorRGBA* buffer);
#endif /* LIBTCOD_TILESET_TILESET_H_ */
