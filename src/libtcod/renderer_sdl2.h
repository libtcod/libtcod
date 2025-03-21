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
#ifndef LIBTCOD_RENDERER_SDL2_H_
#define LIBTCOD_RENDERER_SDL2_H_
#ifndef NO_SDL

#include <SDL3/SDL_properties.h>
#include <stdbool.h>

#include "config.h"
#include "console.h"
#include "context.h"
#include "error.h"
#include "mouse.h"
#include "tileset.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
/**
    An SDL2 tileset atlas.  This prepares a tileset for use with SDL2.
    \rst
    .. versionadded:: 1.16
    \endrst
 */
typedef struct TCOD_TilesetAtlasSDL2 {
  /** The renderer used to create this atlas. Non-owning. */
  struct SDL_Renderer* renderer;
  /** The atlas texture. */
  struct SDL_Texture* texture;
  /** The tileset used to create this atlas. Internal use only. */
  struct TCOD_Tileset* tileset;
  /** Internal use only. */
  struct TCOD_TilesetObserver* observer;
  /** Internal use only. */
  int texture_columns;
} TCOD_TilesetAtlasSDL2;
/**
    The renderer data for an SDL2 rendering context.
    Internal use only.
 */
struct TCOD_RendererSDL2 {
  struct SDL_Window* window;  // Owning power to an SDL2 window.
  struct SDL_Renderer* renderer;  // Owning pointer to an SDL2 renderer.
  struct TCOD_TilesetAtlasSDL2* __restrict atlas;  // Owning atlas pointer.
  struct TCOD_Console* __restrict cache_console;  // Tracks the data from the last console presented.
  struct SDL_Texture* __restrict cache_texture;  // Cached console rendering output.
  uint32_t sdl_subsystems;  // Which subsystems where initialzed by this context.
  // Mouse cursor transform values of the last viewport used.
  TCOD_MouseTransform cursor_transform;
};
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
/***************************************************************************
    @brief Return a libtcod rendering context using an SDL2 renderer.

    @param x Window X position, if unsure then use `SDL_WINDOWPOS_UNDEFINED`
    @param y Window Y position, if unsure then use `SDL_WINDOWPOS_UNDEFINED`
    @param width Window width in pixels
    @param height Window height in pixels
    @param title Window title text
    @param window_flags SDL window flags, see `SDL_WindowFlags`
    @param vsync Enables VSync
    @param tileset The tileset to use for this context.
    @return The new context, will need to deleted with TCOD_context_delete when you are done with it.
 */
TCOD_PUBLIC TCOD_NODISCARD struct TCOD_Context* TCOD_renderer_init_sdl2(
    int x, int y, int width, int height, const char* title, int window_flags, int vsync, struct TCOD_Tileset* tileset);
/***************************************************************************
    @brief  Return a libtcod rendering context using SDL3.

    @param window_props The SDL3 window properties. See `SDL_CreateWindowWithProperties`.
    @param renderer_props The SDL3 rendering properties. See `SDL_CreateRendererWithProperties`.
    @param tileset The tileset to use for this renderer.
    @return The new context, will need to deleted with TCOD_context_delete when you are done with it.
 */
TCOD_PUBLIC TCOD_NODISCARD TCOD_Context* TCOD_renderer_init_sdl3(
    SDL_PropertiesID window_props, SDL_PropertiesID renderer_props, struct TCOD_Tileset* tileset);
/**
    Return a new SDL2 atlas created from a tileset for an SDL2 renderer.

    You may delete the tileset if you no longer have use for it.

    Will return NULL on an error, you can check the error with
    `TCOD_get_error`.
 */
TCOD_PUBLIC TCOD_NODISCARD struct TCOD_TilesetAtlasSDL2* TCOD_sdl2_atlas_new(
    struct SDL_Renderer* renderer, struct TCOD_Tileset* tileset);
/**
    Delete an SDL2 tileset atlas.
 */
TCOD_PUBLIC void TCOD_sdl2_atlas_delete(struct TCOD_TilesetAtlasSDL2* atlas);
/**
    Setup a cache and target texture for rendering.

    `atlas` is an SDL2 atlas created with `TCOD_sdl2_atlas_new`.
    The renderer used to make this `atlas` must support
    `SDL_RENDERER_TARGETTEXTURE`.

    `console` is a non-NULL pointer to the libtcod console you want to render.

    `cache` can be NULL, or be pointer to a console pointer.
    If `*cache` is NULL then a console will be created.
    If `*cache` isn't NULL then the console pointed to might be deleted or
    recreated if it does not match the size of `console`.

    `target` must be a pointer to where you want the output texture to be placed.
    The texture at `*target` may be deleted or recreated.  When this function
    is successful then the texture at `*target` will be non-NULL and will be
    exactly fitted to the size of `console` and the tile size of `atlas`.

    If SDL2 ever provides a `SDL_RENDER_TARGETS_RESET` event then the console
    at `*cache` must be deleted and set to NULL, or else the next render will
    only partially update the texture at `*target`.

    Returns a negative value on an error, check `TCOD_get_error`.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_sdl2_render_texture_setup(
    const struct TCOD_TilesetAtlasSDL2* __restrict atlas,
    const struct TCOD_Console* __restrict console,
    struct TCOD_Console* __restrict* cache,
    struct SDL_Texture* __restrict* target);
/**
    Render a console onto a managed target texture.

    This function assumes that `cache` and `target` are valid.
    You can use `TCOD_sdl2_render_texture_setup` to automatically prepare these
    objects for use with this function.

    `atlas` is an SDL2 atlas created with `TCOD_sdl2_atlas_new`.
    The renderer used to make this `atlas` must support
    `SDL_RENDERER_TARGETTEXTURE`, unless `target` is NULL.

    `console` is a non-NULL pointer to the libtcod console you want to render.

    `cache` can be NULL, or point to a console the same size as `console`.

    `target` can be NULL, or be pointer an SDL2 texture used as the output.
    If `target` is not NULL then it should be the size of the console times the
    size of the individual tiles to fit the entire output.

    If `target` is NULL then the current render target is used instead, the
    drawn area will not be scaled to fit the render target.

    If SDL2 ever provides a `SDL_RENDER_TARGETS_RESET` event then the console
    at `cache` must be cleared, or else the next render will only partially
    update the texture of `target`.

    Returns a negative value on an error, check `TCOD_get_error`.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_sdl2_render_texture(
    const struct TCOD_TilesetAtlasSDL2* __restrict atlas,
    const struct TCOD_Console* __restrict console,
    struct TCOD_Console* __restrict cache,
    struct SDL_Texture* __restrict target);
#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
#endif  // NO_SDL
#endif  // LIBTCOD_RENDERER_SDL2_H_
