
#include "legacy_backend.h"

#include "../console.h"

namespace tcod {
TCOD_event_t LegacyBackend::legacy_wait_for_event(
    int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse, bool flush) {
  return TCOD_sys_wait_for_event(eventMask, key, mouse, flush);
}
TCOD_event_t LegacyBackend::legacy_check_for_event(
      int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse) {
  return TCOD_sys_check_for_event(eventMask, key, mouse);
}
void LegacyBackend::legacy_flush() {
  TCOD_console_flush();
}
} /* namespace tcod */
