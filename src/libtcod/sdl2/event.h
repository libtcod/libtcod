#ifndef LIBTCOD_SDL2_EVENT_H_
#define LIBTCOD_SDL2_EVENT_H_

#include "../console_types.h"
#include "../mouse.h"
#include "../sys.h"

union SDL_Event;
#ifdef __cplusplus
namespace tcod {
namespace sdl2 {
/**
 *  Parse an SDL_Event into a key event and return the relevant TCOD_event_t.
 *
 *  Returns TCOD_EVENT_NONE if the event wasn't keyboard related.
 *  \rst
 *  .. versionadded:: 1.11
 *  \endrst
 */
TCODLIB_API
TCOD_event_t process_event(const union SDL_Event& in,
                           TCOD_key_t& out) noexcept;
/**
 *  Parse an SDL_Event into a mouse event and return the relevant TCOD_event_t.
 *
 *  Returns TCOD_EVENT_NONE if the event wasn't mouse related.
 *  \rst
 *  .. versionadded:: 1.11
 *  \endrst
 */
TCODLIB_API
TCOD_event_t process_event(const union SDL_Event& in,
                           TCOD_mouse_t& out) noexcept;
} // namespace sdl2
} // namespace tcod
#endif // __cplusplus
/**
 *  Parse an SDL_Event into a key event and return the relevant TCOD_event_t.
 *
 *  Returns TCOD_EVENT_NONE if the event wasn't keyboard related.
 *  \rst
 *  .. versionadded:: 1.11
 *  \endrst
 */
TCODLIB_CAPI
TCOD_event_t TCOD_sys_process_key_event(const union SDL_Event* in,
                                        TCOD_key_t* out);
/**
 *  Parse an SDL_Event into a mouse event and return the relevant TCOD_event_t.
 *
 *  Returns TCOD_EVENT_NONE if the event wasn't mouse related.
 *  \rst
 *  .. versionadded:: 1.11
 *  \endrst
 */
TCODLIB_CAPI
TCOD_event_t TCOD_sys_process_mouse_event(const union SDL_Event* in,
                                          TCOD_mouse_t* out);
#endif // LIBTCOD_SDL2_EVENT_H_
