
#include "observer.h"
namespace tcod {
namespace tileset {
TilesetSubject::~TilesetSubject() {
  for (TilesetObserver* observer : observers_) {
    observer->subject_ = nullptr;
  }
  observers_.clear();
}
void TilesetSubject::notify_changed(const IdTileRefPairVector_ &changes)
{
  for (TilesetObserver* observer : observers_) {
    observer->on_tileset_changed(changes);
  }
}
} // namespace tileset
} // namespace tcod
