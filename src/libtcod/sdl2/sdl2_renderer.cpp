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
#include "sdl2_renderer.h"

#include <map>

#include <SDL.h>
namespace tcod {
namespace sdl2 {
using tileset::Tileset;
SDL2Renderer::~SDL2Renderer()
{
  if (texture_ && !SDL_WasInit(SDL_INIT_VIDEO)) {
    SDL_DestroyTexture(texture_);
  }
}

struct SDL_Texture* SDL2Renderer::render(const TCOD_Console* console)
{
  if (!console) { throw; }
  if (console->w != cache_.width() || console->h != cache_.height()) {
    if (texture_) {
      SDL_DestroyTexture(texture_);
      texture_ = nullptr;
    }
  }
  if (!texture_) {
    cache_ = cache_type(console->w, console->h);
    const uint32_t format = 0;
    texture_ = SDL_CreateTexture(renderer_, format, SDL_TEXTUREACCESS_TARGET,
                                 tileset_->get_tile_width() * console->w,
                                 tileset_->get_tile_height() * console->h);
    if(!texture_) { throw std::runtime_error(SDL_GetError()); }
  }
  SDL_SetRenderTarget(renderer_, texture_);
  SDL_Texture* alias_texture = alias_.get_texture_alias();
  SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_NONE);
  SDL_SetTextureBlendMode(alias_texture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureAlphaMod(alias_texture, 0xff);
  for (int y = 0; y < console->h; ++y) {
    for (int x = 0; x < console->w; ++x) {
      const int i = console->w * y + x; // console index
      const int ch = console->ch_array[i];
      const TCOD_color_t fg = console->fg_array[i];
      const TCOD_color_t bg = console->bg_array[i];
      int& cache_ch = std::get<0>(cache_.at(x, y));
      ColorRGBA& cache_fg = std::get<1>(cache_.at(x, y));
      ColorRGBA& cache_bg = std::get<2>(cache_.at(x, y));
      if (cache_ch == ch &&
          cache_fg.r == fg.r && cache_fg.g == fg.g && cache_fg.b == fg.b &&
          cache_bg.r == bg.r && cache_bg.g == bg.g && cache_bg.b == bg.b) {
        continue; // This tile was already rendered on a previous frame.
      }
      cache_ch = ch;
      cache_fg.r = fg.r;
      cache_fg.b = fg.b;
      cache_fg.g = fg.g;
      cache_bg.r = bg.r;
      cache_bg.b = bg.b;
      cache_bg.g = bg.g;
      const SDL_Rect dest_rect{
          x * tileset_->get_tile_width(),
          y * tileset_->get_tile_height(),
          tileset_->get_tile_width(),
          tileset_->get_tile_height(),
      };
      SDL_SetRenderDrawColor(renderer_, bg.r, bg.g, bg.b, 0xff);
      SDL_RenderFillRect(renderer_, &dest_rect);
      if (fg.r != bg.r || fg.g != bg.g || fg.b != bg.b) {
        SDL_SetTextureColorMod(alias_texture, fg.r, fg.g, fg.b);
        const SDL_Rect alias_rect = alias_.get_char_rect(ch);
        SDL_RenderCopy(renderer_, alias_texture,
                       &alias_rect, &dest_rect);
      }
    }
  }
  SDL_SetRenderTarget(renderer_, nullptr);
  return texture_;
}
} // namespace sdl2
} // namespace tcod
