
#include "legacy_backend.h"

#include "../console.h"
#include "../libtcod_int.h"

namespace tcod {
namespace sdl2 {
void LegacyBackend::activate() {
  TCOD_console_init(TCOD_ctx.root, TCOD_ctx.window_title, TCOD_ctx.fullscreen);
}
void LegacyBackend::deactivate() {
  TCOD_console_delete(nullptr);
}
TCOD_event_t LegacyBackend::legacy_wait_for_event(
    int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse, bool flush) {
  return TCOD_sys_wait_for_event(eventMask, key, mouse, flush);
}
TCOD_event_t LegacyBackend::legacy_check_for_event(
      int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse) {
  return TCOD_sys_check_for_event(eventMask, key, mouse);
}
void LegacyBackend::legacy_flush() {
  TCOD_sys_flush(true);
}
} // namespace sdl2
} // namespace tcod
