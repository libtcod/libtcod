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
#ifndef LIBTCOD_RENDERER_GL_INTERNAL_H_
#define LIBTCOD_RENDERER_GL_INTERNAL_H_
#include <SDL.h>
#include <lodepng.h>
#include <math.h>
#include <stdbool.h>

#include "../vendor/glad.h"
#include "config.h"
#include "libtcod_int.h"
#include "renderer_gl.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
/**
 *  Save a screen capture.
 */
static TCOD_Error gl_screenshot(struct TCOD_Context* __restrict context, const char* __restrict filename) {
  (void)context;  // Unused parameter.
  int rect[4];
  glGetIntegerv(GL_VIEWPORT, rect);
  TCOD_ColorRGBA* pixels = malloc(sizeof(*pixels) * rect[2] * rect[3]);
  if (!pixels) {
    TCOD_set_errorv("Could not allocate memory for a screenshot.");
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
/**
 *  Return the SDL2 window.
 */
static struct SDL_Window* gl_get_sdl_window(struct TCOD_Context* __restrict context) {
  struct TCOD_RendererGLCommon* renderer = context->contextdata_;
  return renderer->window;
}
/**
 *  Convert pixel coordinates to tile coordinates.
 */
static void gl_pixel_to_tile(struct TCOD_Context* __restrict self, double* __restrict x, double* __restrict y) {
  struct TCOD_RendererGLCommon* renderer = self->contextdata_;
  *x = (*x - renderer->last_offset_x) * renderer->last_scale_x;
  *y = (*y - renderer->last_offset_y) * renderer->last_scale_y;
}
/**
    Change the atlas to the given tileset.
 */
static TCOD_Error gl_set_tileset(struct TCOD_Context* __restrict self, TCOD_Tileset* __restrict tileset) {
  struct TCOD_RendererGLCommon* renderer = self->contextdata_;
  struct TCOD_TilesetAtlasOpenGL* atlas = TCOD_gl_atlas_new(tileset);
  if (!atlas) {
    return TCOD_E_ERROR;
  }
  if (renderer->atlas) {
    TCOD_gl_atlas_delete(renderer->atlas);
  }
  renderer->atlas = atlas;
  return TCOD_E_OK;
}
static TCOD_Error gl_recommended_console_size(
    struct TCOD_Context* __restrict self, float magnification, int* __restrict columns, int* __restrict rows) {
  struct TCOD_RendererGLCommon* context = self->contextdata_;
  int w;
  int h;
  SDL_GL_GetDrawableSize(context->window, &w, &h);
  if (columns && context->atlas->tileset->tile_width * magnification != 0) {
    *columns = (int)(w / (context->atlas->tileset->tile_width * magnification));
  }
  if (rows && context->atlas->tileset->tile_height * magnification != 0) {
    *rows = (int)(h / (context->atlas->tileset->tile_height * magnification));
  }
  return TCOD_E_OK;
}
static void TCOD_renderer_gl_common_uninit(struct TCOD_RendererGLCommon* common) {
  if (common->atlas) {
    TCOD_gl_atlas_delete(common->atlas);
    common->atlas = NULL;
  }
  if (common->glcontext) {
    SDL_GL_DeleteContext(common->glcontext);
    common->glcontext = NULL;
  }
  if (common->window) {
    SDL_DestroyWindow(common->window);
    common->window = NULL;
  }
  SDL_QuitSubSystem(common->sdl_subsystems);
  common->sdl_subsystems = 0;
}
TCOD_NODISCARD
static TCOD_Error TCOD_renderer_gl_common_init(
    int x,
    int y,
    int pixel_width,
    int pixel_height,
    const char* title,
    int window_flags,
    int vsync,
    struct TCOD_Tileset* tileset,
    int gl_major,
    int gl_minor,
    int gl_profile,
    struct TCOD_Context* out) {
  out->c_get_sdl_window_ = gl_get_sdl_window;
  out->c_pixel_to_tile_ = gl_pixel_to_tile;
  out->c_save_screenshot_ = gl_screenshot;
  out->c_set_tileset_ = gl_set_tileset;
  out->c_recommended_console_size_ = gl_recommended_console_size;
  struct TCOD_RendererGLCommon* renderer = out->contextdata_;
  if (!tileset) {
    return TCOD_E_ERROR;
  }
  if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
    TCOD_set_errorvf("Could not initialize SDL:\n%s", SDL_GetError());
    return TCOD_E_ERROR;
  }
  renderer->sdl_subsystems = SDL_INIT_VIDEO;
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, gl_profile);
  renderer->window = SDL_CreateWindow(title, x, y, pixel_width, pixel_height, window_flags | SDL_WINDOW_OPENGL);
  if (!renderer->window) {
    TCOD_set_errorvf("Could not create SDL window:\n%s", SDL_GetError());
    TCOD_renderer_gl_common_uninit(renderer);
    return TCOD_E_ERROR;
  }
  renderer->glcontext = SDL_GL_CreateContext(renderer->window);
  if (!renderer->glcontext) {
    TCOD_set_errorvf("Could not create GL context:\n%s", SDL_GetError());
    TCOD_renderer_gl_common_uninit(renderer);
    return TCOD_E_ERROR;
  }
  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    TCOD_set_errorv("Failed to invoke the GLAD loader.");
    TCOD_renderer_gl_common_uninit(renderer);
    return TCOD_E_ERROR;
  }
  SDL_GL_SetSwapInterval(vsync);
  if (out->c_set_tileset_(out, tileset) < 0) {
    TCOD_renderer_gl_common_uninit(renderer);
    return TCOD_E_ERROR;
  }
  return TCOD_E_OK;
}
static inline float minf(float a, float b) { return a < b ? a : b; }
static inline float maxf(float a, float b) { return a > b ? a : b; }
static inline float clampf(float v, float low, float high) { return maxf(low, minf(v, high)); }
static void gl_get_viewport_scale(
    const struct TCOD_TilesetAtlasOpenGL* atlas,
    const struct TCOD_Console* console,
    const struct TCOD_ViewportOptions* viewport,
    float matrix_4x4_out[16],
    struct TCOD_RendererGLCommon* common_out) {
  if (!viewport) {
    viewport = &TCOD_VIEWPORT_DEFAULT_;
  }
  SDL_Rect gl_viewport;
  glGetIntegerv(GL_VIEWPORT, (int*)&gl_viewport);
  const int tile_width = atlas->tileset->tile_width;
  const int tile_height = atlas->tileset->tile_height;
  float scale_w = (float)gl_viewport.w / (float)(console->w * tile_width);
  float scale_h = (float)gl_viewport.h / (float)(console->h * tile_height);
  if (viewport->integer_scaling) {
    scale_w = scale_w <= 1.0f ? scale_w : floorf(scale_w);
    scale_h = scale_h <= 1.0f ? scale_h : floorf(scale_h);
  }
  if (viewport->keep_aspect) {
    scale_w = scale_h = minf(scale_w, scale_h);
  }
  scale_w *= (float)(console->w * tile_width) / (float)gl_viewport.w;
  scale_h *= (float)(console->h * tile_height) / (float)gl_viewport.h;
  float translate_x = ((1.0f - scale_w) * clampf(viewport->align_x, 0, 1));
  float translate_y = ((1.0f - scale_h) * clampf(viewport->align_y, 0, 1));
  translate_x = roundf(translate_x * gl_viewport.w) / gl_viewport.w;
  translate_y = roundf(translate_y * gl_viewport.h) / gl_viewport.h;
  float matrix[4 * 4] = {
      2.0f * scale_w,
      0,
      0,
      0,
      0,
      2.0f * scale_h,
      0,
      0,
      0,
      0,
      1,
      0,
      -1.0f + 2.0f * translate_x,
      -1.0f + 2.0f * translate_y,
      1,
      1,
  };
  memcpy(matrix_4x4_out, matrix, sizeof(matrix));
  // Track viewport scaling for mouse coordinates.
  common_out->last_offset_x = (double)translate_x * (double)gl_viewport.w;
  common_out->last_offset_y = (double)translate_x * (double)gl_viewport.h;
  common_out->last_scale_x = (double)scale_w * (double)console->w / (double)gl_viewport.w;
  common_out->last_scale_y = (double)scale_h * (double)console->h / (double)gl_viewport.h;
}
#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
#endif  // LIBTCOD_RENDERER_GL_INTERNAL_H_
