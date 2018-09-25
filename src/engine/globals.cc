
#include "globals.h"

namespace tcod {
namespace engine {
std::shared_ptr<Backend> active_backend = nullptr;
std::shared_ptr<Display> active_display = nullptr;
std::shared_ptr<Tileset> active_tileset = nullptr;
std::shared_ptr<Tilesheet> active_tilesheet = nullptr;

void set_backend(std::shared_ptr<Backend> backend)
{
  active_backend = backend;
}
std::shared_ptr<Backend> get_backend()
{
  return active_backend;
}
void set_display(std::shared_ptr<Display> display)
{
  active_display = display;
}
std::shared_ptr<Display> get_display()
{
  return active_display;
}
void set_tileset(std::shared_ptr<Tileset> tileset)
{
  active_tileset = tileset;
}
auto get_tileset() -> std::shared_ptr<Tileset>
{
  return active_tileset;
}
void set_tilesheet(std::shared_ptr<Tilesheet> sheet)
{
  active_tilesheet = sheet;
}
auto get_tilesheet() -> std::shared_ptr<Tilesheet>
{
  return active_tilesheet;
}
} // namespace sdl2
} // namespace tcod
