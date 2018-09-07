
#include "observer.h"
namespace tcod {
namespace tileset {
TilesetSubject::~TilesetSubject(void) {
  for (TilesetObserver* observer : observers_) {
    observer->OnTilesetDetach();
    observer->subject_ = nullptr;
  }
  observers_.clear();
}
void TilesetSubject::NotifyChanged(const Tileset &tileset,
                                   const IdTileRefPairVector_ &changes) {
  for (TilesetObserver* observer : observers_) {
    observer->OnTilesetChanged(tileset, changes);
  }
}
} // namespace tileset
} // namespace tcod
