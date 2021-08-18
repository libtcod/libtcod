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
#ifndef NO_SDL
#include "renderer_gl.h"

#include <SDL.h>

#include "../vendor/glad.h"
#include "error.h"
/**
 *  Return a rectangle shaped for a tile at `x`,`y`.
 */
static SDL_Rect get_aligned_tile(const struct TCOD_Tileset* tileset, int x, int y) {
  SDL_Rect tile_rect = {x * tileset->tile_width, y * tileset->tile_height, tileset->tile_width, tileset->tile_height};
  return tile_rect;
}
/**
 *  Return the rectangle for the tile at `tile_id`.
 */
static SDL_Rect get_gl_atlas_tile(const struct TCOD_TilesetAtlasOpenGL* atlas, int tile_id) {
  return get_aligned_tile(atlas->tileset, tile_id % atlas->texture_columns, tile_id / atlas->texture_columns);
}
/**
 *  Upload a single tile to the atlas texture.
 */
static int upload_gl_tile(struct TCOD_TilesetAtlasOpenGL* atlas, int tile_id) {
  SDL_Rect dest = get_gl_atlas_tile(atlas, tile_id);
  glBindTexture(GL_TEXTURE_2D, atlas->texture);
  glTexSubImage2D(
      GL_TEXTURE_2D,
      0,
      dest.x,
      dest.y,
      dest.w,
      dest.h,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      atlas->tileset->pixels + (tile_id * atlas->tileset->tile_length));
  glBindTexture(GL_TEXTURE_2D, 0);
  return 0;
}
/**
 *  Setup a atlas texture and upload the tileset graphics.
 */
TCOD_NODISCARD
static int prepare_gl_atlas(struct TCOD_TilesetAtlasOpenGL* atlas) {
  GLenum gl_error = glGetError();
  if (gl_error) return TCOD_set_errorvf("Unexpected OpenGL error before texture allocation: %u", gl_error);
  int new_size = atlas->texture_size ? atlas->texture_size : 64;
  int max_size;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
  int columns = 1;  // Must be more than zero.
  int rows = 1;
  while (1) {
    if (atlas->tileset->tile_width == 0 || atlas->tileset->tile_height == 0) {
      break;  // Avoid division by zero.
    }
    columns = new_size / atlas->tileset->tile_width;
    rows = new_size / atlas->tileset->tile_height;
    if (rows * columns >= atlas->tileset->tiles_capacity) {
      break;
    }
    new_size *= 2;
  }
  if (new_size > max_size) {
    TCOD_set_errorv("Tried to allocate a texture size above the maximum limit!");
    return TCOD_E_ERROR;
  }
  if (new_size != atlas->texture_size) {
    atlas->texture_size = new_size;
    atlas->texture_columns = columns;
    atlas->texture_rows = rows;
    glBindTexture(GL_TEXTURE_2D, atlas->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, atlas->texture_size, atlas->texture_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    gl_error = glGetError();
    switch (gl_error) {
      case GL_NO_ERROR:
        break;
      case GL_OUT_OF_MEMORY:
        TCOD_set_errorv("Out of memory while allocating texture atlas.");
        return TCOD_E_OUT_OF_MEMORY;
      default:
        return TCOD_set_errorvf("OpenGL error while allocating texture atlas: %u", gl_error);
    }
    for (int i = 0; i < atlas->tileset->tiles_count; ++i) {
      if (upload_gl_tile(atlas, i) < 0) {
        return -1;
      }
    }
    return 1;  // Atlas texture has been resized and refreshed.
  }
  return 0;  // No action.
}
/**
 *  Respond to changes in a tileset.
 */
static int gl_atlas_on_tile_changed(struct TCOD_TilesetObserver* observer, int tile_id) {
  struct TCOD_TilesetAtlasOpenGL* atlas = observer->userdata;
  if (prepare_gl_atlas(atlas) == 1) {
    return 0;  // Tile updated as a side-effect of prepare_gl_atlas.
  }
  return upload_gl_tile(atlas, tile_id);
}
struct TCOD_TilesetAtlasOpenGL* TCOD_gl_atlas_new(struct TCOD_Tileset* tileset) {
  if (!tileset) {
    return NULL;
  }
  struct TCOD_TilesetAtlasOpenGL* atlas = calloc(sizeof(*atlas), 1);
  if (!atlas) {
    return NULL;
  }
  atlas->observer = TCOD_tileset_observer_new(tileset);
  if (!atlas->observer) {
    TCOD_gl_atlas_delete(atlas);
    return NULL;
  }
  atlas->tileset = tileset;
  atlas->tileset->ref_count += 1;
  atlas->observer->userdata = atlas;
  atlas->observer->on_tile_changed = gl_atlas_on_tile_changed;
  glGenTextures(1, &atlas->texture);
  if (prepare_gl_atlas(atlas) < 0) {
    TCOD_gl_atlas_delete(atlas);
    return NULL;
  }
  return atlas;
}
void TCOD_gl_atlas_delete(struct TCOD_TilesetAtlasOpenGL* atlas) {
  if (!atlas) {
    return;
  }
  if (atlas->tileset) {
    TCOD_tileset_delete(atlas->tileset);
  }
  if (atlas->texture) {
    glDeleteTextures(1, &atlas->texture);
  }
  free(atlas);
}
#endif  // NO_SDL
