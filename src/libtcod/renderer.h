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
#ifndef LIBTCOD_RENDERER_H_
#define LIBTCOD_RENDERER_H_

#include "portability.h"
#include "console.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Rect;

struct TCOD_Renderer {
  int type;
  void* userdata;
  void (*destructor)(struct TCOD_Renderer* self);
  int (*present)(struct TCOD_Renderer* self,
                 const struct TCOD_Console* console);
  void (*pixel_to_tile)(struct TCOD_Renderer* self, double* x, double* y);
  int (*save_screenshot)(struct TCOD_Renderer* self, const char* filename);
  struct SDL_Window* (*get_sdl_window)(struct TCOD_Renderer* self);
  struct SDL_Renderer* (*get_sdl_renderer)(struct TCOD_Renderer* self);
  int (*accumulate)(
      struct TCOD_Renderer* self,
      const struct TCOD_Console* console,
      const struct SDL_Rect* viewport);
};

TCODLIB_CAPI struct TCOD_Renderer* TCOD_renderer_init_custom();
TCODLIB_CAPI void TCOD_renderer_delete(struct TCOD_Renderer* renderer);

#endif // LIBTCOD_RENDERER_H_
