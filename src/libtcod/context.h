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
#include "console.h"
#include "error.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Rect;
/**
 *  A rendering context for libtcod.
 */
struct TCOD_Context {
  int type;
  void* contextdata;
  void (*destructor_)(struct TCOD_Context* self);
  TCOD_Error (*present_)(struct TCOD_Context* self, const struct TCOD_Console* console);
  void (*pixel_to_tile_)(struct TCOD_Context* self, double* x, double* y);
  TCOD_Error (*save_screenshot_)(struct TCOD_Context* self, const char* filename);
  struct SDL_Window* (*get_sdl_window_)(struct TCOD_Context* self);
  struct SDL_Renderer* (*get_sdl_renderer_)(struct TCOD_Context* self);
  TCOD_Error (*accumulate_)(
      struct TCOD_Context* self,
      const struct TCOD_Console* console,
      const struct SDL_Rect* viewport);
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
TCOD_NODISCARD struct TCOD_Context* TCOD_context_new_();
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
#endif // LIBTCOD_CONTEXT_H_
