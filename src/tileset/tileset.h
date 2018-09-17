
#ifndef LIBTCOD_TILESET_TILESET_H_
#define LIBTCOD_TILESET_TILESET_H_
#ifdef __cplusplus
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#endif

#include "../libtcod_portability.h"
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
      tiles_{Tile(tile_width_, tile_height_)} {}
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
  int SetTile(int codepoint, const Tile &tile) {
    if (VerifyTile(tile) < 0) { return -1; }
    if (codepoint >= static_cast<int>(character_map_.size())) {
      character_map_.resize(codepoint + 1, -1);
    }
    character_map_[codepoint] = tiles_.size();
    tiles_.push_back(tile);
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
#endif /* __cplusplus */
#ifdef __cplusplus
/**
 *  C API alias to the tcod::Tileset class.
 */
typedef Tileset TCOD_Tileset;
extern "C" {
#else
typedef struct TCOD_Tileset TCOD_Tileset;
#endif
/**
 *  Create a new tile-set with the given tile size.
 */
TCOD_Tileset *TCOD_tileset_new(int tile_width, int tile_height);
/**
 *  Delete a tile-set.
 */
void TCOD_tileset_delete(TCOD_Tileset *tileset);
#ifdef __cplusplus
} // extern "C"
} // namespace tileset
} // namespace tcod
#endif
#endif /* LIBTCOD_TILESET_TILESET_H_ */
