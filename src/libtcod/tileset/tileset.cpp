/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
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
#include "tileset.h"

#include <algorithm>
#include <memory>
#include <string>

#include "observer.h"
#include "tilesheet.h"
#include "../libtcod_int.h"

namespace tcod {
namespace tileset {
extern "C" {
TCOD_Tileset* TCOD_tileset_new(int tile_width, int tile_height)
{
  auto tileset = std::make_shared<Tileset>(tile_width, tile_height);
  return new TCOD_Tileset(tileset);
}
void TCOD_tileset_delete(TCOD_Tileset* tileset)
{
  if (tileset) { delete tileset; }
}
int TCOD_tileset_get_tile_width_(const TCOD_Tileset* tileset)
{
  if (!tileset || !(*tileset)) { return 0; }
  return (*tileset)->get_tile_width();
}
int TCOD_tileset_get_tile_height_(const TCOD_Tileset* tileset)
{
  if (!tileset || !(*tileset)) { return 0; }
  return (*tileset)->get_tile_height();
}
int TCOD_tileset_get_tile_(
    const TCOD_Tileset* tileset,
    int codepoint,
    struct TCOD_ColorRGBA* buffer)
{
  if (!tileset || !(*tileset)) { return -1; }
  if (!(*tileset)->has_tile_(codepoint)) {
    return -1; // No tile for the given codepoint in this tileset.
  }
  if (!buffer) {
    return 0; // buffer is NULL, just return an OK status.
  }
  auto tile = (*tileset)->get_tile_(codepoint);
  std::copy(tile.begin(), tile.end(), buffer);
  return 0; // Tile exists and was copied to buffer.
}
int TCOD_tileset_set_tile_(
    TCOD_Tileset* tileset,
    int codepoint,
    const struct TCOD_ColorRGBA* buffer)
{
  if (!tileset || !(*tileset)) { return -1; }
  Image tile((*tileset)->get_tile_width(), (*tileset)->get_tile_height());
  std::copy(buffer, buffer + (tile.width() * tile.height()), tile.begin());
  (*tileset)->set_tile(codepoint, tile);
  return 0; // Tile uploaded successfully.
}
} // extern "C"
} // namespace tileset
} // namespace tcod
