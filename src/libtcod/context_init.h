/* BSD 3-Clause License
 *
 * Copyright © 2008-2020, Jice and the libtcod contributors.
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
#ifndef LIBTCOD_CONTEXT_INIT_H_
#define LIBTCOD_CONTEXT_INIT_H_
#include "config.h"

#include "stdbool.h"

#include "context.h"
#include "error.h"
#include "tileset.h"
/**
    Create a new context with the given console size.


    Same as `TCOD_context_new_window`, but the following parameters have
    different effects:

    `columns` and `rows` is the desired size of the terminal window.
    The actual size of the window will be derived from this and the given
    `tileset`.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_NODISCARD TCOD_Error TCOD_context_new_terminal(
  int columns,
  int rows,
  int renderer_type,
  TCOD_Tileset* tileset,
  bool vsync,
  int sdl_window_flags,
  const char* window_title,
  TCOD_Context** out);
/**
    Create a new context with a window of the given size.

    Same as `TCOD_context_new_terminal`, but the following parameters have
    different effects:

    `pixel_width` and `pixel_height` are the desired size of the window in
    pixels.

    `renderer_type` is a `TCOD_renderer_t` type.

    `tileset` is an optional pointer to a tileset object.

    If `vsync` is True, then vertical sync will be enabled whenever possible.
    A value of true is recommended.

    `sdl_window_flags` is a bitfield of SDL_WindowFlags flags.
    For a window, a value of ``SDL_WINDOW_RESIZABLE`` is recommended.
    For fullscreen, a value of
    ``SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP`` is recommended.
    You should avoid the ``SDL_WINDOW_FULLSCREEN`` flag whenever possible.

    `window_title` is the title of the opened window.

    `out` is the output for the `TCOD_Context`, must not be NULL.

    Returns a negative error code on failure, `out` will unlikely be set in this
    case.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_NODISCARD TCOD_Error TCOD_context_new_window(
  int pixel_width,
  int pixel_height,
  int renderer_type,
  TCOD_Tileset* tileset,
  bool vsync,
  int sdl_window_flags,
  const char* window_title,
  TCOD_Context** out);
#endif // LIBTCOD_CONTEXT_INIT_H_
