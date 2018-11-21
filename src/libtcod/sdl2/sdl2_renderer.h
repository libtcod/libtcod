/* libtcod
 * Copyright © 2008-2018 Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The name of copyright holder nor the names of its contributors may not
 *       be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef LIBTCOD_SDL2_SDL2_RENDERER_H_
#define LIBTCOD_SDL2_SDL2_RENDERER_H_
#ifdef __cplusplus
#include <tuple>
#endif /* __cplusplus */

#include "sdl2_alias.h"
#include "../color/color.h"
#include "../console_types.h"
#include "../tileset/observer.h"
#include "../tileset/tileset.h"
#include "../utility/vector2.h"
#ifdef __cplusplus
struct SDL_Renderer;
struct SDL_Texture;
namespace tcod {
namespace sdl2 {
using tcod::tileset::Tileset;
class SDL2Renderer: public TilesetObserver {
 public:
  SDL2Renderer()
  {}
  SDL2Renderer(struct SDL_Renderer* renderer, SDL2TilesetAlias alias)
  : TilesetObserver(alias.get_tileset()), alias_{alias}, renderer_{renderer}
  {}

  SDL2Renderer(struct SDL_Renderer* renderer,
               std::shared_ptr<Tileset> tileset)
  : SDL2Renderer(renderer, SDL2TilesetAlias(renderer, tileset))
  {}

  void on_tileset_changed(
      const std::vector<std::pair<int, Tile&>> &changes) override
  {}

  ~SDL2Renderer();

  struct SDL_Texture* render(const TCOD_Console* console);
 private:
  using cache_type = Vector2<std::tuple<int, ColorRGBA, ColorRGBA>>;
  SDL2TilesetAlias alias_;
  cache_type cache_;
  struct SDL_Renderer* renderer_;
  struct SDL_Texture* texture_ = nullptr;
};
} // namespace sdl2
} // namespace tcod
#endif /* __cplusplus */
#endif /* LIBTCOD_SDL2_SDL2_RENDERER_H_ */
