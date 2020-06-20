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
#ifndef LIBTCOD_CONTEXT_H_
#define LIBTCOD_CONTEXT_H_

#include "config.h"
#include "context_viewport.h"
#include "console.h"
#include "error.h"
#include "tileset.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Rect;
/**
 *  A rendering context for libtcod.
 *  \rst
 *  .. versionadded:: 1.16
 *  \endrst
 */
struct TCOD_Context {
  int type;
  void*__restrict contextdata;
  void (*destructor_)(struct TCOD_Context* self);
  TCOD_Error (*present_)(
      struct TCOD_Context* self,
      const struct TCOD_Console* console,
      const struct TCOD_ViewportOptions* viewport);
  void (*pixel_to_tile_)(struct TCOD_Context* self, double* x, double* y);
  TCOD_Error (*save_screenshot_)(struct TCOD_Context* self, const char* filename);
  struct SDL_Window* (*get_sdl_window_)(struct TCOD_Context* self);
  struct SDL_Renderer* (*get_sdl_renderer_)(struct TCOD_Context* self);
  TCOD_Error (*accumulate_)(
      struct TCOD_Context* self,
      const struct TCOD_Console* console,
      const struct TCOD_ViewportOptions* viewport);
  /**
      Change the tileset used by this context.
  */
  TCOD_Error (*set_tileset)(struct TCOD_Context* self, TCOD_Tileset* tileset);
  /**
      Output the recommended console size to `columns` and `rows`.
  */
  TCOD_Error (*cb_recommended_console_size_)(
      struct TCOD_Context* self, int* columns, int* rows);
};
typedef struct TCOD_Context TCOD_Context;
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
/**
 *  Delete a rendering context.
 *  \rst
 *  .. versionadded:: 1.16
 *  \endrst
 */
TCOD_PUBLIC void TCOD_context_delete(struct TCOD_Context* renderer);
/**
 *  Create an uninitialized rendering context.
 *
 *  Used internally.
 */
TCOD_NODISCARD struct TCOD_Context* TCOD_context_new_(void);
/**
    Present a console to the screen, using a rendering context.

    `console` is the console to present, the console can be any size.

    `viewport` is the optional viewport options to use.
    This will affect the scaling of the console with the current context.
    This can be NULL to use the default options, which are to stretch the
    console to fit the screen.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_context_present(
    struct TCOD_Context* context,
    const struct TCOD_Console* console,
    const struct TCOD_ViewportOptions* viewport);
/**
    Convert the screen coordinates to tile coordinates for this context.

    `x` and `y` are the pointers to the screen coordinates, these will be
    converted to tile coordinates after the call to this function.

    The parameters given to the last call to `TCOD_context_present` will
    determine where the tiles are for this call.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_context_screen_pixel_to_tile_d(
    struct TCOD_Context* context, double* x, double* y);
/**
    Convert the screen coordinates to integer tile coordinates for this context.

    Save as `TCOD_context_screen_pixel_to_tile` but the inputs and results are
    integers.  This is useful if you don't need sub-tile coordinates.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_context_screen_pixel_to_tile_i(
    struct TCOD_Context* context, int* x, int* y);
/**
    Save the last presented console to a PNG file.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_context_save_screenshot(
    struct TCOD_Context* context, const char* filename);
/**
    Return a pointer the SDL_Window for this context if it uses one.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC struct SDL_Window* TCOD_context_get_sdl_window(
    struct TCOD_Context* context);
/**
    Return a pointer the SDL_Renderer for this context if it uses one.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC struct SDL_Renderer* TCOD_context_get_sdl_renderer(
    struct TCOD_Context* context);
/**
    Change the active tileset for this context.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_context_change_tileset(
    struct TCOD_Context* self, TCOD_Tileset* tileset);
/**
    Return the `TCOD_renderer_t` renderer type for this context.

    Returns a negative number on error, such as `context` being NULL.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC int TCOD_context_get_renderer_type(struct TCOD_Context* context);
/**
    Set `columns` and `rows` to the recommended console size for this context.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_context_recommended_console_size(
    struct TCOD_Context* context, int* columns, int* rows);
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
#endif // LIBTCOD_CONTEXT_H_
