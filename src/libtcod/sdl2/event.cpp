#include "event.h"

#include <cstring>

#include "../libtcod_int.h"
#include "../engine/globals.h"

#include <SDL.h>
namespace tcod {
namespace sdl2 {
TCOD_event_t process_event(const union SDL_Event& ev,
                           TCOD_key_t& key) noexcept
{
  return TCOD_sys_handle_key_event(&ev, &key);
}
TCOD_event_t process_event(const union SDL_Event& ev,
                           TCOD_mouse_t& mouse) noexcept
{
  return TCOD_sys_handle_mouse_event(&ev, &mouse);
}
} // namespace sdl2
} // namespace tcod
TCOD_event_t TCOD_sys_process_key_event(
    const union SDL_Event* in, TCOD_key_t* out)
{
  if (!in || !out) { return TCOD_EVENT_NONE; }
  return tcod::sdl2::process_event(*in, *out);
}
TCOD_event_t TCOD_sys_process_mouse_event(
    const union SDL_Event* in, TCOD_mouse_t* out)
{
  if (!in || !out) { return TCOD_EVENT_NONE; }
  return tcod::sdl2::process_event(*in, *out);
}
