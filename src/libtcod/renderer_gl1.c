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
#include "renderer_gl1.h"

#include "libtcod_int.h"
#include "console.h"
#include "renderer.h"
#include "renderer_gl.h"

#include <SDL.h>
#include "../vendor/glad.h"
#include "../vendor/lodepng.h"
/**
 *  Get the texture coordinates for a codepoint.
 */
void get_tex_coord(const struct TCOD_RendererGL1* renderer, int ch, float* out)
{
  const struct TCOD_Tileset* tileset = renderer->atlas->tileset;
  float tex_tile_width = 1.0f / renderer->atlas->texture_size * tileset->tile_width;
  float tex_tile_height = 1.0f / renderer->atlas->texture_size * tileset->tile_height;
  int tile_id = 0;
  if (ch < tileset->character_map_length) {
    tile_id = tileset->character_map[ch];
  }
  int x = tile_id % renderer->atlas->texture_columns;
  int y = tile_id / renderer->atlas->texture_columns;
  out[0] = x * tex_tile_width;
  out[1] = y * tex_tile_height;
  out[2] = (x + 1) * tex_tile_width;
  out[3] = y * tex_tile_height;
  out[4] = x * tex_tile_width;
  out[5] = (y + 1) * tex_tile_height;
  out[6] = (x + 1) * tex_tile_width;
  out[7] = (y + 1) * tex_tile_height;
}
void render_background(struct TCOD_Context* context, const TCOD_Console* console)
{
  TCOD_ColorRGBA* color = malloc(sizeof(*color) * console->length);
  for (int y = 0; y < console->h; ++y) {
    for (int x = 0; x < console->w; ++x) {
      int i = y * console->w + x;
      color[i] = console->tiles[i].bg;
    }
  }

  // Setup background texture.
  uint32_t bg_tex = 0;
  glGenTextures(1, &bg_tex);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, bg_tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA,
      console->w,
      console->h,
      0,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      color);

  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glClientActiveTexture(GL_TEXTURE0);

  // Render background.
  int16_t bg_vertex[] = {0, 0, console->w, 0, 0, console->h, console->w, console->h};
  float bg_tex_coord[] = {0, 0, 1, 0, 0, 1, 1, 1};
  uint8_t bg_indices[] = {0, 1, 2, 3, 2, 1};
  glVertexPointer(2, GL_SHORT, 0, bg_vertex);
  glTexCoordPointer(2, GL_FLOAT, 0, bg_tex_coord);
  glDrawElements(GL_TRIANGLES, 6,  GL_UNSIGNED_BYTE, bg_indices);

  glDeleteTextures(1, &bg_tex);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisable(GL_TEXTURE_2D);
  free(color);
}
void render_foreground(struct TCOD_Context* context, const TCOD_Console* console)
{
  struct TCOD_RendererGL1* renderer = context->contextdata;
  // Prepare buffers.
  int16_t* vertex = malloc(sizeof(*vertex) * console->length * 2 * 4);
  TCOD_ColorRGBA* color = malloc(sizeof(*color) * console->length * 4);
  float* tex_coord = malloc(sizeof(*tex_coord) * console->length * 2 * 4);
  uint16_t* indices = malloc(sizeof(*indices) * console->length * 6);
  for (int y = 0; y < console->h; ++y) {
    for (int x = 0; x < console->w; ++x) {
      int i = y * console->w + x;
      int16_t* v = vertex + i * 2 * 4;
      v[0] = x;
      v[1] = y;
      v[2] = x + 1;
      v[3] = y;
      v[4] = x;
      v[5] = y + 1;
      v[6] = x + 1;
      v[7] = y + 1;
      TCOD_ColorRGBA* c = color + i * 4;
      c[0] = c[1] = c[2] = c[3] = console->tiles[i].fg;
      get_tex_coord(renderer, console->tiles[i].ch, tex_coord + i * 2 * 4);
      uint16_t* ind = indices + i * 6;
      ind[0] = i * 4;
      ind[1] = i * 4 + 1;
      ind[2] = i * 4 + 2;
      ind[3] = i * 4 + 2;
      ind[4] = i * 4 + 3;
      ind[5] = i * 4 + 1;
    }
  }
  // Setup OpenGL.
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, renderer->atlas->texture);
  glClientActiveTexture(GL_TEXTURE0);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  // Render characters.
  glVertexPointer(2, GL_SHORT, 0, vertex);
  glColorPointer(4, GL_UNSIGNED_BYTE, 0, color);
  glTexCoordPointer(2, GL_FLOAT, 0, tex_coord);
  glDrawElements(GL_TRIANGLES, console->length * 6,  GL_UNSIGNED_SHORT, indices);

  // Cleanup.
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  free(vertex);
  free(color);
  free(tex_coord);
  free(indices);
}
TCOD_Error gl1_accumulate(struct TCOD_Context* context, const TCOD_Console* console, const struct SDL_Rect* viewport)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  float matrix[] = {
    2.0f / console->w, 0, 0, 0,
    0, -2.0f / console->h, 0, 0,
    0, 0, 1, 0,
    -1, 1, 1, 1,
  };
  glLoadMatrixf(matrix);

  render_background(context, console);
  render_foreground(context, console);
  glFlush();
  if (glGetError()) {
    return TCOD_E_ERROR;
  }
  return TCOD_E_OK;
}

TCOD_Error gl1_present(struct TCOD_Context* context, const TCOD_Console* console)
{
  struct TCOD_RendererGL1* renderer = context->contextdata;
  int window_width;
  int window_height;
  SDL_GL_GetDrawableSize(renderer->window, &window_width, &window_height);
  glViewport(0, 0, window_width, window_height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  TCOD_Error err = gl1_accumulate(context, console, NULL);
  SDL_GL_SwapWindow(renderer->window);
  return err;
}
TCOD_Error gl1_screenshot(struct TCOD_Context* context, const char* filename)
{
  (void)context; // Unused parameter.
  int rect[4];
  glGetIntegerv(GL_VIEWPORT, rect);
  TCOD_ColorRGBA* pixels = malloc(sizeof(*pixels) * rect[2] * rect[3]);
  if (!pixels) {
    TCOD_set_errorv("Could not allocae memory for a screenshot.");
    return TCOD_E_OUT_OF_MEMORY;
  }
  glReadPixels(0, 0, rect[2], rect[3], GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  // Flip image on Y axis.
  for (int y = 0; y < rect[3] / 2; ++y) {
    for (int x = 0; x < rect[2]; ++x) {
      TCOD_ColorRGBA tmp = pixels[y * rect[2] + x];
      pixels[y * rect[2] + x] = pixels[(rect[3] - 1 - y) * rect[2] + x];
      pixels[(rect[3] - 1 - y) * rect[2] + x] = tmp;
    }
  }
  lodepng_encode32_file(filename, (const unsigned char*)pixels, (unsigned)rect[2], (unsigned)rect[3]);
  free(pixels);
  return TCOD_E_OK;
}

struct SDL_Window* gl1_get_sdl_window(struct TCOD_Context* context)
{
  struct TCOD_RendererGL1* renderer = context->contextdata;
  return renderer->window;
}
/**
 *  Convert pixel coordinates to tile coordinates.
 */
static void gl1_pixel_to_tile(struct TCOD_Context* self, double* x, double* y)
{
  struct TCOD_RendererGL1* renderer = self->contextdata;
  *x /= renderer->atlas->tileset->tile_width;
  *y /= renderer->atlas->tileset->tile_height;
}
void gl1_destructor(struct TCOD_Context* context)
{
  struct TCOD_RendererGL1* renderer = context->contextdata;
  if (!renderer) { return; }
  if (renderer->glcontext) { SDL_GL_DeleteContext(renderer->glcontext); }
  if (renderer->window) { SDL_DestroyWindow(renderer->window); }
  free(renderer);
}

TCODLIB_API TCOD_NODISCARD
struct TCOD_Context* TCOD_renderer_init_gl1(
    int pixel_width,
    int pixel_height,
    const char* title,
    int window_flags,
    bool vsync,
    struct TCOD_Tileset* tileset)
{
  if (!tileset) { return NULL; }
  TCOD_sys_startup();
  struct TCOD_Context* context = TCOD_renderer_init_custom();
  if (!context) { return NULL; }
  context->destructor_ = gl1_destructor;
  struct TCOD_RendererGL1* renderer = calloc(sizeof(*renderer), 1);
  if (!renderer) { TCOD_renderer_delete(context); return NULL; }
  context->contextdata = renderer;
  context->get_sdl_window_ = gl1_get_sdl_window;
  context->accumulate_ = gl1_accumulate;
  context->present_ = gl1_present;
  context->pixel_to_tile_ = gl1_pixel_to_tile;
  context->save_screenshot_ = gl1_screenshot;
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  renderer->window = SDL_CreateWindow(
      title,
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      pixel_width,
      pixel_height,
      window_flags | SDL_WINDOW_OPENGL);
  if (!renderer->window) { TCOD_renderer_delete(context); return NULL; }
  renderer->glcontext = SDL_GL_CreateContext(renderer->window);
  if (!renderer->glcontext) { TCOD_renderer_delete(context); return NULL; }
  if(!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    TCOD_set_errorv("Failed to invoke the GLAD loader.");
    return NULL;
  }
  SDL_GL_SetSwapInterval(vsync);
  renderer->atlas = TCOD_gl_atlas_new(tileset);
  if (!renderer->atlas) { TCOD_renderer_delete(context); return NULL; }
  return context;
}
