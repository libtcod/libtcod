
#ifndef LIBTCOD_TILESET_TILESET_H_
#define LIBTCOD_TILESET_TILESET_H_
#include "libtcod_portability.h"

#ifdef __cplusplus
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#endif

#include "observer.h"
#include "tile.h"
#ifdef __cplusplus
namespace tcod {
/**
 *  This is a tile-set resource.
 */
class Tileset: public std::enable_shared_from_this<Tileset> {
 public:
  explicit Tileset(int tile_width, int tile_height) {
    tile_width_ = std::max(0, tile_width);
    tile_height_ = std::max(0, tile_height);
    /* The tile at zero is always blank. */
    tiles_.push_back(Tile(tile_width_, tile_height_));
  }
  static std::shared_ptr<Tileset> New(int tile_width, int tile_height) {
    return std::make_shared<Tileset>(tile_width, tile_height);
  }
  Tileset(Tileset&&) = default;
  Tileset& operator=(Tileset&&) = default;
  Tileset(const Tileset&) = delete;
  Tileset& operator=(const Tileset&) = delete;
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
  int GetTileWidth(void) { return tile_width_; }
  /** Return the height of each tile in this Tileset */
  int GetTileHeight(void) { return tile_height_; }
  /**
   *  Attach a TilesetObserver instance to this Tileset.
   *
   *  Detachment is automatic.  The Tileset or TilesetObserver may be deleted
   *  at any time without issues.
   */
  void AttachTilesetObserver(std::weak_ptr<TilesetObserver> observer) {
    tileset_subject_.AttachTilesetObserver(observer, *this, tiles_);
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
    if (tile.GetWidth() == tile_width_ && tile.GetHeight() == tile_height_) {
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
  TilesetSubject tileset_subject_;
};
#endif /* __cplusplus */
#ifdef __cplusplus
/**
 *  C API alias to the tcod::Tileset class.
 */
typedef tcod::Tileset TCOD_Tileset;
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
} // namespace tcod
#endif
#endif /* LIBTCOD_TILESET_TILESET_H_ */
