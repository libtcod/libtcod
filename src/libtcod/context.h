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

#include "stdbool.h"

#include "config.h"
#include "color.h"
#include "console.h"
#include "error.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Rect;
/**
    Viewport options for the rendering context.
    \rst
    .. versionadded:: 1.16
    \endrst
 */
struct TCOD_ViewportOptions {
  /**
      If true then the aspect ratio will be kept square when the console is
      scaled.  The view will be letter-boxed.

      If false the console will be stretched to fill the screen.

      Set this is true if your tileset is deigned for square pixels.
   */
  bool keep_aspect;
  /**
      If true then console scaling will be fixed to integer increments.

      Has no effect if the console must be scaled down.
   */
  bool integer_scaling;
  /**
      If true then when a console is "close enough" to the screen-size a small
      border will be added instead of stretching the console.

      This is generally recommended to be set to true, especially if you're
      resizing the console to fit the screen.
   */
  bool snap_to_integer;
  /**
      The color to clear the screen with before rendering the console.
   */
  TCOD_ColorRGBA clear_color;
  /**
      Alignment of the console when it is letter-boxed: 0.0f renders the
      console in the upper-left corner, and 1.0f in the lower-right.

      Values of 0.5f will center the console.
      Values outside of the range 0.0f to 1.0f are clamped.
   */
  float align_x;
  float align_y;
};
#if !defined(__cplusplus)
/**
    Default viewport options if none are provided.
 */
static const struct TCOD_ViewportOptions TCOD_VIEWPORT_DEFAULT_ = {
    .keep_aspect = false,
    .integer_scaling = false,
    .snap_to_integer = true,
    .clear_color = {0, 0, 0, 255},
    .align_x = 0.5f,
    .align_y = 0.5f,
};
#endif // !defined(__cplusplus)
/**
 *  A rendering context for libtcod.
 *  \rst
 *  .. versionadded:: 1.16
 *  \endrst
 */
struct TCOD_Context {
  int type;
  void* contextdata;
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
};
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
 *  Present a console to the screen, using a rendering context.
 */
TCOD_PUBLIC TCOD_Error TCOD_context_present(
    struct TCOD_Context* context,
    const struct TCOD_Console* console,
    const struct TCOD_ViewportOptions* viewport);
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
#endif // LIBTCOD_CONTEXT_H_
