/* BSD 3-Clause License
 *
 * Copyright © 2008-2025, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
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
TCOD_event_t process_event(const union SDL_Event& in, TCOD_key_t& out) noexcept;
/**
 *  Parse an SDL_Event into a mouse event and return the relevant TCOD_event_t.
 *
 *  Returns TCOD_EVENT_NONE if the event wasn't mouse related.
 *  \rst
 *  .. versionadded:: 1.11
 *  \endrst
 */
TCODLIB_API
TCOD_event_t process_event(const union SDL_Event& in, TCOD_mouse_t& out) noexcept;
}  // namespace sdl2
}  // namespace tcod
#endif  // __cplusplus
/**
 *  Parse an SDL_Event into a key event and return the relevant TCOD_event_t.
 *
 *  Returns TCOD_EVENT_NONE if the event wasn't keyboard related.
 *  \rst
 *  .. versionadded:: 1.11
 *  \endrst
 */
TCODLIB_CAPI
TCOD_event_t TCOD_sys_process_key_event(const union SDL_Event* in, TCOD_key_t* out);
/**
 *  Parse an SDL_Event into a mouse event and return the relevant TCOD_event_t.
 *
 *  Returns TCOD_EVENT_NONE if the event wasn't mouse related.
 *  \rst
 *  .. versionadded:: 1.11
 *  \endrst
 */
TCODLIB_CAPI
TCOD_event_t TCOD_sys_process_mouse_event(const union SDL_Event* in, TCOD_mouse_t* out);
#endif  // LIBTCOD_SDL2_EVENT_H_
