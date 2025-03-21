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
#ifndef TCOD_MOUSE_H_
#define TCOD_MOUSE_H_

#include "mouse_types.h"
#include "portability.h"

#ifdef __cplusplus
extern "C" {
#endif
TCOD_DEPRECATED("Use SDL_ShowCursor to handle the mouse cursor.")
TCODLIB_API void TCOD_mouse_show_cursor(bool visible);

TCOD_DEPRECATED("Use SDL_GetMouseState to check the mouse state.")
TCODLIB_API TCOD_mouse_t TCOD_mouse_get_status(void);

TCOD_DEPRECATED("Use SDL_ShowCursor to check the mouse cursor.")
TCODLIB_API bool TCOD_mouse_is_cursor_visible(void);

TCOD_DEPRECATED("Use SDL_WarpMouseInWindow to move mouse cursor instead.")
TCODLIB_API void TCOD_mouse_move(int x, int y);

TCOD_DEPRECATED_NOMESSAGE
TCODLIB_API void TCOD_mouse_includes_touch(bool enable);
#ifdef __cplusplus
}
#endif
#endif  // TCOD_MOUSE_H_
