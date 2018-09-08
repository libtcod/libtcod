
#include "observer.h"
namespace tcod {
namespace tileset {
TilesetSubject::~TilesetSubject() {
  for (TilesetObserver* observer : observers_) {
    observer->on_tileset_detached();
    observer->subject_ = nullptr;
  }
  observers_.clear();
}
void TilesetSubject::notify_changed(const Tileset &tileset,
                                    const IdTileRefPairVector_ &changes) {
  for (TilesetObserver* observer : observers_) {
    observer->on_tileset_changed(tileset, changes);
  }
}
} // namespace tileset
} // namespace tcod
