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
#include "sys.h"
#ifndef NO_SDL
#ifndef TCOD_NO_PNG
#if !defined(__HAIKU__) && !defined(__ANDROID__)
#include <stdio.h>
#include <stdlib.h>
#endif

#include <SDL3/SDL.h>
#include <lodepng.h>

#include "error.h"
#include "libtcod_int.h"

bool TCOD_sys_check_png(const char* filename) {
  static uint8_t magic_number[] = {137, 80, 78, 71, 13, 10, 26, 10};
  return TCOD_sys_check_magic_number(filename, sizeof(magic_number), magic_number);
}

SDL_Surface* TCOD_sys_read_png(const char* filename) {
  unsigned char* png = NULL;
  size_t png_size;
  if (!TCOD_sys_read_file(filename, &png, &png_size)) {
    TCOD_set_errorvf("File not found: %s", filename);
    return NULL;
  }
  LodePNGState state;
  lodepng_state_init(&state);
  unsigned width;
  unsigned height;
  lodepng_inspect(&width, &height, &state, png, png_size);
  if (state.error) {
    TCOD_set_errorvf("Error decoding PNG: %s", lodepng_error_text(state.error));
    free(png);
    return NULL;
  }

  unsigned bpp = lodepng_get_bpp(&state.info_png.color);
  if (bpp == 24) {
    /* don't convert to 32 bits because libtcod's 24bits renderer is faster */
    state.info_raw.colortype = LCT_RGB;
  } else if (bpp != 24 && bpp != 32) {
    /* paletted png. convert to 24 bits */
    state.info_raw.colortype = LCT_RGB;
    state.info_raw.bitdepth = 8;
    bpp = 24;
  }
  unsigned char* image = NULL;
  unsigned error = lodepng_decode(&image, &width, &height, &state, png, png_size);
  free(png);
  lodepng_state_cleanup(&state);
  if (error) {
    TCOD_set_errorvf("Error decoding PNG: %s", lodepng_error_text(error));
    return NULL;
  }
  SDL_Surface* bitmap;
  if (bpp == 32) {
    bitmap = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
  } else {
    bitmap = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGB24);
  }
  if (bitmap) {
    if (!SDL_ConvertPixels(
            width, height, bitmap->format, image, width * bpp / 8, bitmap->format, bitmap->pixels, bitmap->pitch)) {
      SDL_DestroySurface(bitmap);
      bitmap = NULL;
      TCOD_set_errorvf("SDL Error: %s", SDL_GetError());
    }
  } else {
    TCOD_set_errorvf("Could not allocate SDLSurface: %s", SDL_GetError());
  }
  free(image);
  return bitmap;
}

TCOD_Error TCOD_sys_write_png(SDL_Surface* surf, const char* filename) {
  SDL_Surface* rgba32 = SDL_ConvertSurface(surf, SDL_PIXELFORMAT_RGBA32);
  if (!rgba32) {
    return TCOD_set_errorvf("SDL: %s", SDL_GetError());
  }
  unsigned int error = lodepng_encode32_file(filename, rgba32->pixels, rgba32->w, rgba32->h);
  SDL_DestroySurface(rgba32);
  if (error) {
    return TCOD_set_errorvf("LodePNG error %u: %s\n", error, lodepng_error_text(error));
  }
  return TCOD_E_OK;
}
#endif  // TCOD_NO_PNG
#endif  // NO_SDL
