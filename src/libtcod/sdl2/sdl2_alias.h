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
#ifndef LIBTCOD_SDL2_SDL2_ALIAS_H_
#define LIBTCOD_SDL2_SDL2_ALIAS_H_
#include <memory>

#include "../console.h"
#include "../tileset/observer.h"
#ifdef __cplusplus
struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Rect;

namespace tcod {
namespace sdl2 {
using tileset::Tile;
using tileset::Tileset;
using tileset::TilesetObserver;
class SDL2InternalTilesetAlias_;
/**
 *  A texture alias manager for SDL2 renderers.
 */
class SDL2TilesetAlias {
 public:
  SDL2TilesetAlias() = default;
  SDL2TilesetAlias(struct SDL_Renderer* renderer,
                   const std::shared_ptr<Tileset>& tileset);

  const std::shared_ptr<Tileset> get_tileset() const;
  /**
   *  Return a texture alias containing all the glyphs required by `console`.
   *
   *  The returned pointer is owned by this object, and will expire on the next
   *  call to `prepare_alias`;
   */
  auto prepare_alias(const TCOD_Console& console) -> struct SDL_Texture*;
  /**
   *  Return the rectangle where a codepoint exists on the texture alias.
   */
  auto get_char_rect(int codepoint) const -> SDL_Rect;
 private:
  friend class SDL2AliasPool;
  class impl;
  std::shared_ptr<impl> impl_;
};
} // namespace sdl2
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_SDL2_SDL2_ALIAS_H_
