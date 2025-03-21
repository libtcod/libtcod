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
#include "tileset_fallback.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "tileset_truetype.h"

TCOD_Tileset* TCOD_tileset_load_fallback_font_(int tile_width, int tile_height) {
#if defined(__EMSCRIPTEN__)
  return NULL;
#elif defined(_WIN32)  // Windows.
  const char* sys_root = getenv("SystemRoot");
  const char* filename = "\\Fonts\\LUCON.TTF";
  char path[4096] = "";
  strncpy(path, sys_root, sizeof(path) - 1);
  strncat(path, filename, sizeof(path) - 1 - strlen(path));
  return TCOD_load_truetype_font_(path, tile_width, tile_height);
#elif defined(__APPLE__)  // MacOS.
  return TCOD_load_truetype_font_("/System/Library/Fonts/SFNSMono.ttf", tile_width, tile_height);
#elif defined(__unix__)  // Linux
  FILE* pipe = popen("fc-match --format=%{file} monospace", "r");
  char path[4096] = "";
  if (!pipe) {
    TCOD_set_errorv("Failed to run fc-match cmd.");
    return NULL;
  }
  fgets(path, sizeof(path) - 1, pipe);
  if (pclose(pipe) != 0) {
    TCOD_set_errorv("Could not get a font from fc-match.");
    return NULL;
  }
  return TCOD_load_truetype_font_(path, tile_width, tile_height);
#else
  return NULL;
  // throw std::runtime_error("Fallback font not supported for this OS.");
#endif
}
