/* BSD 3-Clause License
 *
 * Copyright © 2008-2022, Jice and the libtcod contributors.
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
 *  Return a rectangle shaped for a tile at column `x` and row `y` of a 2D
 *   tile atlas.
 */
static SDL_Rect get_aligned_tile(const struct TCOD_Tileset* tileset, int x, int y) {
#ifndef TILE_PADDING
  const int TILE_PADDING = 0;
#endif
  // account for padding by adding initial value, plus per-tile value to upper
  //  left coordinates
  // size needs to stay true because it determines how big of a source area is
  //  copied
  SDL_Rect tile_rect = {
    TILE_PADDING + x * (tileset->tile_width  + TILE_PADDING),
    TILE_PADDING + y * (tileset->tile_height + TILE_PADDING),
    tileset->tile_width, tileset->tile_height};
  return tile_rect;
}
/**
 *  Return the rectangle containing coordinates at which tile index `tile_id`
 *   should be placed in the atlas texture.
 */
static SDL_Rect get_gl_atlas_tile(const struct TCOD_TilesetAtlasOpenGL* atlas, int tile_id) {
  // calculate atlas row and column, and use that to request a destination
  //  rectangle in pixel coordinates
  return get_aligned_tile(
    atlas->tileset,
    tile_id % atlas->texture_columns,
    tile_id / atlas->texture_columns);
}
#ifdef TILE_PADDING
/**
 *  Upload padding strips to the atlas texture.
 *
 *  Propagates all four edges of tile into padding space.
 */
static void upload_gl_tile_padding(struct TCOD_TilesetAtlasOpenGL* atlas, struct TCOD_ColorRGBA* tile_top_left, SDL_Rect* dest)
{
  // pointer to lower left tile pixel in tileset pixel data buffer
  struct TCOD_ColorRGBA* tile_bottom_left =
    tile_top_left + atlas->tileset->tile_length - atlas->tileset->tile_width;
  // pointer to upper right tile pixel in tileset pixel data buffer
  struct TCOD_ColorRGBA* tile_top_right =
    tile_top_left + atlas->tileset->tile_width - 1;
  // pointer to lower right tile pixel in tileset pixel data buffer
  struct TCOD_ColorRGBA* tile_bottom_right =
    tile_top_left + atlas->tileset->tile_length - 1;

  for (int i = 0; i < TILE_PADDING / 2; ++i)
  {
    // top:
    //  to  : dest->x, dest->y - 1 - i
    //  size: dest->w, 1
    //  from: tile_top_left
    glBindTexture(GL_TEXTURE_2D, atlas->texture);
    glTexSubImage2D(
      GL_TEXTURE_2D, 0,
      dest->x, dest->y - 1 - i,
      dest->w, 1,
      GL_RGBA, GL_UNSIGNED_BYTE,
      tile_top_left);
    // glBindTexture(GL_TEXTURE_2D, 0);
    // bottom:
    //  to  : dest->x, dest->y + dest->h + i
    //  size: dest->w, 1
    //  from: tile_bottom_left
    // glBindTexture(GL_TEXTURE_2D, atlas->texture);
    glTexSubImage2D(
      GL_TEXTURE_2D, 0,
      dest->x, dest->y + dest->h + i,
      dest->w, 1,
      GL_RGBA, GL_UNSIGNED_BYTE,
      tile_bottom_left);
    glBindTexture(GL_TEXTURE_2D, 0);
    // left:
    //  to  : dest->x - 1 - i, dest->y
    //  size: 1,               dest->h
    //  from: tile_top_left
    glBindTexture(GL_TEXTURE_2D, atlas->texture);
    // to copy a vertical strip, OpenGL needs to know tileset bytes per row
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, atlas->tileset->tile_width);
    glTexSubImage2D(
      GL_TEXTURE_2D, 0,
      dest->x - 1 - i, dest->y,
      1, dest->h,
      GL_RGBA, GL_UNSIGNED_BYTE,
      tile_top_left);
    // glBindTexture(GL_TEXTURE_2D, 0);
    // right:
    //  to  : dest->x + dest->w + i, dest->y
    //  size: 1,                     dest->h
    //  from: tile_top_right
    // glBindTexture(GL_TEXTURE_2D, atlas->texture);
    // to copy a vertical strip, OpenGL needs to know tileset bytes per row
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // glPixelStorei(GL_UNPACK_ROW_LENGTH, atlas->tileset->tile_width);
    glTexSubImage2D(
      GL_TEXTURE_2D, 0,
      dest->x + dest->w + i, dest->y,
      1, dest->h,
      GL_RGBA, GL_UNSIGNED_BYTE,
      tile_top_right);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  }

  // test: fill in the padding corners by copying in the nearest tile corner
  // this seems to help with the noise demo at least
  for (int py = 0; py < TILE_PADDING / 2; ++py)
  {
    for (int px = 0; px < TILE_PADDING / 2; ++px)
    {
      // upper left:
      //  to  : dest->x - 1 - px, dest->y - 1 - py
      //  size: 1,           1
      //  from: tile_top_left
      glBindTexture(GL_TEXTURE_2D, atlas->texture);
      glTexSubImage2D(
        GL_TEXTURE_2D, 0,
        dest->x - 1 - px, dest->y - 1 - py,
        1, 1,
        GL_RGBA, GL_UNSIGNED_BYTE,
        tile_top_left);
      // glBindTexture(GL_TEXTURE_2D, 0);
      // upper right:
      //  to  : dest->x + dest->w + px, dest->y - 1 - py
      //  size: 1,                 1
      //  from: tile_top_right
      // glBindTexture(GL_TEXTURE_2D, atlas->texture);
      glTexSubImage2D(
        GL_TEXTURE_2D, 0,
        dest->x + dest->w + px, dest->y - 1 - py,
        1, 1,
        GL_RGBA, GL_UNSIGNED_BYTE,
        tile_top_right);
      // glBindTexture(GL_TEXTURE_2D, 0);
      // lower left:
      //  to  : dest->x - 1 - px, dest->y + dest->h + py
      //  size: 1,           1
      //  from: tile_bottom_left
      // glBindTexture(GL_TEXTURE_2D, atlas->texture);
      glTexSubImage2D(
        GL_TEXTURE_2D, 0,
        dest->x - 1 - px, dest->y + dest->h + py,
        1, 1,
        GL_RGBA, GL_UNSIGNED_BYTE,
        tile_bottom_left);
      // glBindTexture(GL_TEXTURE_2D, 0);
      // lower right:
      //  to  : dest->x + dest->w + px, dest->y + dest->h + py
      //  size: 1,                 1
      //  from: tile_bottom_right
      // glBindTexture(GL_TEXTURE_2D, atlas->texture);
      glTexSubImage2D(
        GL_TEXTURE_2D, 0,
        dest->x + dest->w + px, dest->y + dest->h + py,
        1, 1,
        GL_RGBA, GL_UNSIGNED_BYTE,
        tile_bottom_right);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }
}
#endif
/**
 *  Upload a single tile to the atlas texture.
 */
static int upload_gl_tile(struct TCOD_TilesetAtlasOpenGL* atlas, int tile_id) {
  SDL_Rect dest = get_gl_atlas_tile(atlas, tile_id);
  struct TCOD_ColorRGBA* tile_origin = atlas->tileset->pixels + (tile_id * atlas->tileset->tile_length);
#ifdef TILE_PADDING
  // also repeat edge pixels into the padding space
  upload_gl_tile_padding(atlas, tile_origin, &dest);
#endif
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
      tile_origin);
  glBindTexture(GL_TEXTURE_2D, 0);
  return 0;
}
/**
 *  Setup a atlas texture and upload the tileset graphics.
 *
 *  This will be a square texture whose dimensions are a power of 2, of
 *   sufficient size to hold all of the tiles.
 */
TCOD_NODISCARD
static int prepare_gl_atlas(struct TCOD_TilesetAtlasOpenGL* atlas) {
#ifndef TILE_PADDING
  const int TILE_PADDING = 0;
#endif
  GLenum gl_error = glGetError();
  if (gl_error) return TCOD_set_errorvf("Unexpected OpenGL error before texture allocation: %u", gl_error);
  // prospective texture size; start at a minimum of 64x64?
  int new_size = atlas->texture_size ? atlas->texture_size : 64;
  // get maximum texture size supported by GPU
  int max_size;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
  // starting values for tiles per row and per column in the atlas texture
  // end values may differ because tiles aren't necessarily square in dimension
  int columns = 1;  // Must be more than zero.
  int rows = 1;
  // calculate a texture size large enough to hold the tileset
  while (1) {
    if (atlas->tileset->tile_width == 0 || atlas->tileset->tile_height == 0) {
      break;  // Avoid division by zero.
    }
    // calculate how many rows and colums of tiles can fit in a square texture
    //  whose dimensions are both new_size
    // padding around tiles is accounted for by adding to the individual tile
    //  dimension divisors, while padding around the remaining horizontal and
    //  vertical edge is accounted for by subtracting from the prospective
    //  texture size
    columns = (new_size - TILE_PADDING) / (atlas->tileset->tile_width + TILE_PADDING);
    rows = (new_size - TILE_PADDING) / (atlas->tileset->tile_height + TILE_PADDING);
    if (rows * columns >= atlas->tileset->tiles_capacity) {
      break;
    }
    // double the dimensions until texture is large enough to hold the tiles
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
    // populate atlas with tile data
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
