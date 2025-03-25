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
#include "context.h"

#ifndef NO_SDL
#include <SDL3/SDL_events.h>
#endif  // NO_SDL
#ifndef TCOD_NO_PNG
#include <lodepng.h>
#endif  // TCOD_NO_PNG
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct TCOD_Context* TCOD_context_new_(void) {
  struct TCOD_Context* renderer = calloc(1, sizeof(*renderer));
  return renderer;
}

void TCOD_context_delete(struct TCOD_Context* renderer) {
  if (!renderer) {
    return;
  }
  if (renderer->c_destructor_) {
    renderer->c_destructor_(renderer);
  }
  free(renderer);
}
TCOD_Error TCOD_context_present(
    struct TCOD_Context* context, const struct TCOD_Console* console, const struct TCOD_ViewportOptions* viewport) {
  if (!context) {
    TCOD_set_errorv("Context must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!console) {
    TCOD_set_errorv("Console must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!context->c_present_) {
    return TCOD_set_errorv("Context is missing a present method.");
  }
  return context->c_present_(context, console, viewport);
}
TCOD_Error TCOD_context_screen_pixel_to_tile_d(struct TCOD_Context* context, double* x, double* y) {
  if (!context) {
    TCOD_set_errorv("Context must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!context->c_pixel_to_tile_) {
    return TCOD_E_OK;
  }
  context->c_pixel_to_tile_(context, x, y);
  return TCOD_E_OK;
}
TCOD_Error TCOD_context_screen_pixel_to_tile_i(struct TCOD_Context* context, int* x, int* y) {
  double xd = x ? (double)(*x) : 0;
  double yd = y ? (double)(*y) : 0;
  TCOD_Error err = TCOD_context_screen_pixel_to_tile_d(context, &xd, &yd);
  if (x) {
    *x = (int)floor(xd);
  }
  if (y) {
    *y = (int)floor(yd);
  }
  return err;
}
#ifndef NO_SDL
TCOD_Error TCOD_context_convert_event_coordinates(struct TCOD_Context* context, union SDL_Event* event) {
  if (!event) return TCOD_E_OK;
  switch (event->type) {
    case SDL_EVENT_MOUSE_MOTION: {
      int tile_x = (int)event->motion.x;
      int tile_y = (int)event->motion.y;
      int previous_tile_x = event->motion.x - event->motion.xrel;
      int previous_tile_y = event->motion.y - event->motion.yrel;
      TCOD_Error err = TCOD_context_screen_pixel_to_tile_i(context, &tile_x, &tile_y);
      if (err < 0) return err;
      err = TCOD_context_screen_pixel_to_tile_i(context, &previous_tile_x, &previous_tile_y);
      if (err < 0) return err;
      event->motion.x = (float)tile_x;
      event->motion.y = (float)tile_y;
      event->motion.xrel = tile_x - previous_tile_x;
      event->motion.yrel = tile_y - previous_tile_y;
    } break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP: {
      int x = (int)event->button.x;
      int y = (int)event->button.y;
      TCOD_Error err = TCOD_context_screen_pixel_to_tile_i(context, &x, &y);
      event->button.x = (float)x;
      event->button.y = (float)y;
      return err;
    } break;
  }
  return TCOD_E_OK;
}
#endif  // NO_SDL
TCOD_PUBLIC TCOD_Error TCOD_context_save_screenshot(struct TCOD_Context* context, const char* filename) {
#ifndef TCOD_NO_PNG
  if (!context) {
    TCOD_set_errorv("Context must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  char unique_path[128];
  while (!filename) {
    static int unique_id = 0;
    snprintf(unique_path, sizeof(unique_path), "./screenshot%03d.png", unique_id++);
    FILE* access_file = fopen(unique_path, "rb");
    if (access_file) {  // File exists, increment and try again.
      fclose(access_file);
      continue;
    }
    filename = unique_path;
  }
  if (!context->c_save_screenshot_) {
    int width = 0;
    int height = 0;
    TCOD_ColorRGBA* pixels = TCOD_context_screen_capture_alloc(context, &width, &height);
    if (!pixels) return TCOD_E_ERROR;
    lodepng_encode32_file(filename, (const unsigned char*)pixels, (unsigned)width, (unsigned)height);
    free(pixels);
    return TCOD_E_OK;
  }
  return context->c_save_screenshot_(context, filename);
#else
  return TCOD_set_errorv("Can not save screenshots without PNG support.");
#endif  // TCOD_NO_PNG
}
TCOD_PUBLIC struct SDL_Window* TCOD_context_get_sdl_window(struct TCOD_Context* context) {
  if (!context) {
    TCOD_set_errorv("Context must not be NULL.");
    return NULL;
  }
  if (!context->c_get_sdl_window_) {
    return NULL;
  }
  return context->c_get_sdl_window_(context);
}
TCOD_PUBLIC struct SDL_Renderer* TCOD_context_get_sdl_renderer(struct TCOD_Context* context) {
  if (!context) {
    TCOD_set_errorv("Context must not be NULL.");
    return NULL;
  }
  if (!context->c_get_sdl_renderer_) {
    return NULL;
  }
  return context->c_get_sdl_renderer_(context);
}
TCOD_Error TCOD_context_change_tileset(struct TCOD_Context* context, TCOD_Tileset* tileset) {
  if (!context) {
    TCOD_set_errorv("Context must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!context->c_set_tileset_) {
    return TCOD_set_errorv("Context does not support changing tilesets.");
  }
  return context->c_set_tileset_(context, tileset);
}
int TCOD_context_get_renderer_type(struct TCOD_Context* context) {
  if (!context) {
    TCOD_set_errorv("Context must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  return context->type;
}
TCOD_Error TCOD_context_recommended_console_size(
    struct TCOD_Context* __restrict context, float magnification, int* __restrict columns, int* __restrict rows) {
  if (!context) {
    TCOD_set_errorv("Context must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!context->c_recommended_console_size_) {
    TCOD_set_errorv("Context is missing configuration..");
    return TCOD_E_ERROR;
  }
  if (magnification <= 0) {
    magnification = 1.0f;
  }
  return context->c_recommended_console_size_(context, magnification, columns, rows);
}
TCOD_Error TCOD_context_screen_capture(
    struct TCOD_Context* __restrict context,
    TCOD_ColorRGBA* __restrict out_pixels,
    int* __restrict width,
    int* __restrict height) {
  if (!context) {
    TCOD_set_errorv("Context must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!context->c_screen_capture_) return TCOD_set_errorv("Context does not support screen capture.");
  if (!width || !height) {
    TCOD_set_errorv("width and height can not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  return context->c_screen_capture_(context, out_pixels, width, height);
}

TCOD_ColorRGBA* TCOD_context_screen_capture_alloc(
    struct TCOD_Context* __restrict context, int* __restrict width, int* __restrict height) {
  while (true) {
    if (TCOD_context_screen_capture(context, NULL, width, height) != TCOD_E_OK) return NULL;
    TCOD_ColorRGBA* pixels = malloc((*width) * (*height) * sizeof(*pixels));
    if (!pixels) {
      TCOD_set_errorv("Failed to allocate image for screen capture.");
      return NULL;
    }
    const TCOD_Error err = TCOD_context_screen_capture(context, pixels, width, height);
    if (err >= TCOD_E_OK) return pixels;
    free(pixels);
    if (err == TCOD_E_INVALID_ARGUMENT) continue;
    return NULL;
  }
}
TCOD_Error TCOD_context_set_mouse_transform(
    struct TCOD_Context* __restrict context, const TCOD_MouseTransform* __restrict transform) {
  if (!context) {
    TCOD_set_errorv("Context must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!context->c_set_mouse_transform_) {
    TCOD_set_errorv("Context is missing configuration..");
    return TCOD_E_ERROR;
  }
  if (!context) {
    TCOD_set_errorv("transform must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  return context->c_set_mouse_transform_(context, transform);
}
