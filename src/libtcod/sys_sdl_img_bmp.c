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
#ifndef NO_SDL
#include <SDL3/SDL.h>

#include "error.h"
#include "libtcod_int.h"
#include "sys.h"
bool TCOD_sys_check_bmp(const char* filename) {
  static uint8_t magic_number[] = {0x42, 0x4d};
  return TCOD_sys_check_magic_number(filename, sizeof(magic_number), magic_number);
}
/**
    Return a new SDL_Surface from a BMP file.
 */
SDL_Surface* TCOD_sys_read_bmp(const char* filename) {
  SDL_Surface* out_surface = SDL_LoadBMP(filename);
  if (!out_surface) {
    TCOD_set_errorvf("SDL: %s", SDL_GetError());
    return NULL;
  }
  return out_surface;
}

TCOD_Error TCOD_sys_write_bmp(SDL_Surface* surf, const char* filename) {
  if (!SDL_SaveBMP(surf, filename)) {
    return TCOD_set_errorvf("SDL: %s", SDL_GetError());
  }
  return TCOD_E_OK;
}
#endif  // NO_SDL
