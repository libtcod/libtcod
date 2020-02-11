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
#ifndef LIBTCOD_RENDERER_SDL2_H_
#define LIBTCOD_RENDERER_SDL2_H_

#include "stdbool.h"

#include "config.h"
#include "console.h"
#include "context.h"
#include "tileset.h"
#include "error.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

struct TCOD_TilesetAtlasSDL2 {
  struct SDL_Renderer* renderer;
  struct SDL_Texture* texture;
  struct TCOD_Tileset* tileset;
  struct TCOD_TilesetObserver* observer;
  int texture_columns;
};

struct TCOD_RendererSDL2 {
  struct SDL_Window* window;
  struct SDL_Renderer* renderer;
  struct TCOD_TilesetAtlasSDL2* atlas;
  struct TCOD_Console* cache_console;
  struct SDL_Texture* cache_texture;
  uint32_t sdl_subsystems;
};
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
TCODLIB_API TCOD_NODISCARD
struct TCOD_Context* TCOD_renderer_init_sdl2(
    int width,
    int height,
    const char* title,
    int window_flags,
    int renderer_flags,
    struct TCOD_Tileset* tileset);

TCODLIB_API TCOD_NODISCARD
struct TCOD_TilesetAtlasSDL2* TCOD_sdl2_atlas_new(
    struct SDL_Renderer* renderer,
    struct TCOD_Tileset* tileset);

TCODLIB_API
void TCOD_sdl2_atlas_delete(struct TCOD_TilesetAtlasSDL2* atlas);

TCODLIB_API TCOD_NODISCARD
TCOD_Error TCOD_sdl2_render_console(
    const struct TCOD_TilesetAtlasSDL2* atlas,
    const struct TCOD_Console* console,
    struct TCOD_Console** cache);
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
#endif // LIBTCOD_RENDERER_SDL2_H_
