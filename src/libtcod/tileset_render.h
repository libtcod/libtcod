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
#ifndef LIBTCOD_TILESET_RENDER_H_
#define LIBTCOD_TILESET_RENDER_H_

#include "console.h"
#include "tileset.h"

struct SDL_Surface;
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
#ifndef NO_SDL
/**
    Render a console to a SDL_Surface with a software renderer.

    `tileset` is the tiles to render with, must not be NULL.

    `console` is the console to render, must not be NULL.

    `cache` is an optional pointer to a consoled used as a cache.  The console
    at `*cache` will be created or modified.  The `cache` will be used to skip
    drawing already drawn tiles on any subsequent calls.

    `surface_out` is a pointer to where to put the surface will be managed.
    The surface at `*surface_out` will be created or modified and will change
    to match the size of `console` and `tileset`.  The pixel format will be
    SDL_PIXELFORMAT_RGBA32.

    Returns a negative value on error, see `TCOD_get_error`.
    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_tileset_render_to_surface(
    const TCOD_Tileset* __restrict tileset,
    const TCOD_Console* __restrict console,
    TCOD_Console* __restrict* cache,
    struct SDL_Surface* __restrict* surface_out);
#endif  // NO_SDL
#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
#endif  // LIBTCOD_TILESET_RENDER_H_
