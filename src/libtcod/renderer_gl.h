/* BSD 3-Clause License
 *
 * Copyright © 2008-2021, Jice and the libtcod contributors.
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
#ifndef LIBTCOD_RENDERER_GL_H_
#define LIBTCOD_RENDERER_GL_H_
#include <stdint.h>

#include "config.h"
#include "tileset.h"

struct TCOD_TilesetAtlasOpenGL {
  struct TCOD_Tileset* tileset;  // An owning pointer to tileset, tracks a tilesets character map and other info.
  struct TCOD_TilesetObserver* observer;  // Tracks changes to a tileset.
  uint32_t texture;  // Texture atlas.
  int texture_size;  // Texture atlas width and height in pixels.
  int texture_columns;  // Number of columns in texture atlas.
  int texture_rows;  // Number of rows in texture atlas.
};

struct TCOD_RendererGLCommon {
  struct SDL_Window* window;
  void* glcontext;
  struct TCOD_TilesetAtlasOpenGL* atlas;
  uint32_t sdl_subsystems;  // Which subsystems were initialzed by this context.
  // Mouse cursor transform values of the last viewport used.
  double last_offset_x;
  double last_offset_y;
  double last_scale_x;
  double last_scale_y;
};

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
struct TCOD_TilesetAtlasOpenGL* TCOD_gl_atlas_new(struct TCOD_Tileset* tileset);
void TCOD_gl_atlas_delete(struct TCOD_TilesetAtlasOpenGL* atlas);
#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
#endif  // LIBTCOD_RENDERER_GL_H_
