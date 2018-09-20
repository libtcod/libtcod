
#include "globals.h"

namespace tcod {
namespace engine {
std::shared_ptr<Backend> active_backend = nullptr;
std::shared_ptr<Display> active_display = nullptr;

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
} // namespace sdl2
} // namespace tcod
