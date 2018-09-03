
#ifndef LIBTCOD_SDL2_LEGACY_BACKEND_H_
#define LIBTCOD_SDL2_LEGACY_BACKEND_H_

#include "../backend/backend.h"

#ifdef __cplusplus
namespace tcod {
namespace sdl2 {
class LegacyBackend: public backend::Backend {
  void activate() override;
  void deactivate() override;
  TCOD_event_t legacy_wait_for_event(
      int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse, bool flush) override;
  TCOD_event_t legacy_check_for_event(
      int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse) override;
  void legacy_flush() override;
};
} // namespace sdl2
} // namespace tcod
#endif /* __cplusplus */
#endif /* LIBTCOD_SDL2_LEGACY_BACKEND_H_ */
