
#ifndef LIBTCOD_TILESET_OBSERVER_H_
#define LIBTCOD_TILESET_OBSERVER_H_
#include "libtcod_portability.h"

#ifdef __cplusplus
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#endif

#include "tile.h"
#ifdef __cplusplus
namespace tcod {
class Tileset;
class TilesetObserver;
typedef std::vector<std::pair<int, Tile&>> IdTileRefPairVector_;
/**
 *  An observable tile-set subject.  This is used internally.
 */
class TilesetSubject {
 public:
  friend class TilesetObserver;
  TilesetSubject() = default;

  TilesetSubject(TilesetSubject&&) = delete;
  TilesetSubject& operator=(TilesetSubject&&) = delete;
  TilesetSubject(const TilesetSubject&) = delete;
  TilesetSubject& operator=(const TilesetSubject&) = delete;
  /**
   *  On delete all attached observers are detached automatically.
   */
  ~TilesetSubject(void);
  /**
   *  Attach a TilesetObserver to this subject.
   *
   *  Called by Tileset, this function has enough information to notify the
   *  TilesetObserver about the Tileset's current state.
   */
  void AttachTilesetObserver(std::weak_ptr<TilesetObserver> observer,
                             const Tileset &tileset,
                             const std::vector<Tile>& tiles);
  /**
   *  Notify observers about the state and changes to the subject.
   *
   *  `tiles` is the current state of the subject.
   *  `changes` is for specifically changed tiles, if any.
   */
  void NotifyChanged(const Tileset &tileset, const std::vector<Tile>& tiles,
                     const IdTileRefPairVector_ &changes);
 private:
  /**
   *  An array of weak pointers to observers.
   */
  std::vector<std::weak_ptr<TilesetObserver>> observer_pointers_;
};
class TilesetObserver {
 public:
  friend class TilesetSubject;
  TilesetObserver() = default;

  TilesetObserver(TilesetObserver&&) = default;
  TilesetObserver& operator=(TilesetObserver&&) = default;
  TilesetObserver(const TilesetObserver&) = default;
  TilesetObserver& operator=(const TilesetObserver&) = default;
 protected:
  /**
   *  Called when this object starts observing a Tileset.
   *
   *  By default this will pass the Tileset's current state to the
   *  OnTilesetChanged function.
   */
  virtual void OnTilesetAttach(const Tileset &tileset,
                               const std::vector<Tile>& tiles) {
    OnTilesetChanged(tileset, tiles, IdTileRefPairVector_());
  }
  /**
   *  Called on Tileset state changes.
   *
   *  `tiles` is the current state.
   *  `changes` is a vector of index,Tile pairs.
   *
   *  The Tileset may have been resized.
   */
  virtual void OnTilesetChanged(
      const Tileset &tileset, const std::vector<Tile>& tiles,
      const std::vector<std::pair<int, Tile&>> &changes);
  /**
   *  Called when the observed Tileset is being deleted.
   */
  virtual void OnTilesetDetach();
};
} /* namespace tcod */
#endif /* __cplusplus */
#endif /* LIBTCOD_TILESET_OBSERVER_H_ */
