
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
// "tileset.h" included near end of header.
#ifdef __cplusplus
namespace tcod {
namespace tileset {
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
  virtual ~TilesetSubject();
 protected:
  /**
   *  Notify observers about the state and changes to the subject.
   *
   *  `tiles` is the current state of the subject.
   *  `changes` is for specifically changed tiles, if any.
   */
  void notify_changed(const Tileset &tileset,
                      const IdTileRefPairVector_ &changes);
  /**
   *  Return this object as a Tileset object.
   */
  virtual Tileset& as_tileset() = 0;
 private:
  /**
   *  An array of weak pointers to observers.
   */
  std::vector<TilesetObserver*> observers_;
};
class TilesetObserver {
 public:
  friend class TilesetSubject;
  TilesetObserver();
  TilesetObserver(TilesetSubject& subject) {
    subject.observers_.emplace_back(this);
  }

  TilesetObserver(TilesetObserver&&) = delete;
  TilesetObserver& operator=(TilesetObserver&&) = delete;
  TilesetObserver(const TilesetObserver&) = delete;
  TilesetObserver& operator=(const TilesetObserver&) = delete;
  virtual ~TilesetObserver() {
    unobserve();
  }
  void observe(TilesetSubject& subject) {
    unobserve();
    subject_ = &subject;
    on_tileset_attached(subject.as_tileset());
  }
 protected:
  /**
   *  Called when this object starts observing a Tileset.
   *
   *  By default this will pass the Tileset's current state to the
   *  OnTilesetChanged function.
   */
  virtual void on_tileset_attached(const Tileset &tileset) {
    on_tileset_changed(tileset, IdTileRefPairVector_());
  }
  /**
   *  Called on Tileset state changes.
   *
   *  `changes` is a vector of index,Tile pairs.
   *
   *  The Tileset may have been resized.
   */
  virtual void on_tileset_changed(
      const Tileset &tileset,
      const std::vector<std::pair<int, Tile&>> &changes) { };
  /**
   *  Called when the observed Tileset is being deleted.
   */
  virtual void on_tileset_detached() { };
  TilesetSubject* subject_;
 private:
  void unobserve() {
    if (!subject_) { return; }
    on_tileset_detached();
    auto& observers = subject_->observers_;
    observers.erase(std::find(observers.begin(), observers.end(), this));
    subject_ = nullptr;
  }
};
} // namespace tileset
} // namespace tcod
#include "tileset.h" // These classes are incomplete without tileset.h
#endif /* __cplusplus */
#endif /* LIBTCOD_TILESET_OBSERVER_H_ */
