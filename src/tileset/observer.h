
#ifndef LIBTCOD_TILESET_OBSERVER_H_
#define LIBTCOD_TILESET_OBSERVER_H_
#include "../libtcod_portability.h"

#ifdef __cplusplus
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#endif

#include "tile.h"
#include "tileset.h"
#ifdef __cplusplus
namespace tcod {
namespace tileset {
class Tileset;
class TilesetObserver;
typedef std::vector<std::pair<int, Tile&>> IdTileRefPairVector_;
class TilesetObserver {
 public:
  TilesetObserver() = default;

  TilesetObserver(std::shared_ptr<Tileset> subject)
  {
    if (!subject) {
      throw std::invalid_argument("tileset cannot be nullptr.");
    }
    observe(subject);
  }

  TilesetObserver(TilesetObserver& subject)
  : TilesetObserver(subject.tileset_)
  {}

  TilesetObserver(TilesetObserver&&) = delete;
  TilesetObserver& operator=(TilesetObserver&&) = delete;
  TilesetObserver(const TilesetObserver&) = delete;
  TilesetObserver& operator=(const TilesetObserver&) = delete;
  virtual ~TilesetObserver() {
    unobserve();
  }
  /**
   *  Called on Tileset state changes.
   *
   *  `changes` is a vector of index,Tile pairs.
   *
   *  The Tileset may have been resized.
   */
  virtual void on_tileset_changed(
      const std::vector<std::pair<int, Tile&>> &changes)
  {}
  std::shared_ptr<Tileset>& get_tileset()
  {
    return tileset_;
  }
 protected:
  std::shared_ptr<Tileset> tileset_;
 private:
  void observe(std::shared_ptr<Tileset> subject)
  {
    unobserve();
    tileset_ = subject;
    subject->observers_.emplace_back(this);
  }
  void unobserve() {
    if (!tileset_) { return; }
    auto& observers = tileset_->observers_;
    observers.erase(std::find(observers.begin(), observers.end(), this));
    tileset_ = nullptr;
  }
};
} // namespace tileset
} // namespace tcod
#endif /* __cplusplus */
#endif /* LIBTCOD_TILESET_OBSERVER_H_ */
