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
#include "sdl2_renderer.h"

#include <map>

#include "../utility/matrix.h"

#include <SDL.h>
namespace tcod {
namespace sdl2 {
using tileset::Tileset;
class SDL2Renderer::impl: public TilesetObserver {
 public:
  impl(struct SDL_Renderer* renderer, SDL2TilesetAlias alias)
  : TilesetObserver(alias.get_tileset()), alias_{alias}, renderer_{renderer}
  {
    SDL_AddEventWatch(on_sdl_event, this);
  }
  ~impl()
  {
    if (texture_ && !SDL_WasInit(SDL_INIT_VIDEO)) {
      SDL_DestroyTexture(texture_);
    }
    SDL_DelEventWatch(on_sdl_event, this);
  }
  /**
   *  Selectively reset the cache so that only changed tiles are redrawn.
   */
  void on_tileset_changed(
      const std::vector<std::pair<int, Tile&>> &changes) override
  {
    for (const auto& changed : changes) {
      int codepoint = changed.second.codepoint;
      for (auto& it : cache_) {
        if (it.ch == codepoint) {
          it.ch = -1;
        }
      }
    }
  }
  auto render(const TCOD_Console& console) -> struct SDL_Texture*
  {
    if (cache_.get_shape()
        != cache_type::shape_type{{console.h, console.w}}) {
      if (texture_) {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
      }
    }
    if (!texture_) {
      cache_ = cache_type({console.h, console.w});
      const uint32_t format = 0;
      texture_ = SDL_CreateTexture(
          renderer_, format, SDL_TEXTUREACCESS_TARGET,
          tileset_->get_tile_width() * console.w,
          tileset_->get_tile_height() * console.h);
      SDL_SetTextureBlendMode(texture_, SDL_BLENDMODE_BLEND);
      if(!texture_) { throw std::runtime_error(SDL_GetError()); }
    }
    SDL_SetRenderTarget(renderer_, texture_);
    SDL_Texture* alias_texture = alias_.prepare_alias(console);
    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_NONE);
    SDL_SetTextureBlendMode(alias_texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(alias_texture, 0xff);
    for (int y = 0; y < console.h; ++y) {
      for (int x = 0; x < console.w; ++x) {
        auto tile = console.at(y, x);
        if (tile.ch == 0x20 || tile.fg.a == 0) {
          // Tile is the space character or has no alpha.
          tile.ch = 0; // Skip drawing this glyph.
        }
        if (tcod::ColorRGB(tile.fg) == tcod::ColorRGB(tile.bg)
            && tile.bg.a == 0xff) {
          // Foreground color is the same as the background.
          tile.ch = 0; // Skip drawing this glyph.
        }
        if (tile.ch == 0) {
          // There is no foreground glyph, so ignore the color.
          tile.fg = {0, 0, 0, 0};
        }
        auto& cache = cache_.at({y, x});
        if (tile == cache) {
          continue; // This tile was already rendered from the last frame.
        }
        cache = tile;
        const SDL_Rect dest_rect{
            x * tileset_->get_tile_width(),
            y * tileset_->get_tile_height(),
            tileset_->get_tile_width(),
            tileset_->get_tile_height(),
        };
        // Fill the background of the tile with a solid color.
        SDL_SetRenderDrawColor(
            renderer_, tile.bg.r, tile.bg.g, tile.bg.b, tile.bg.a);
        SDL_RenderFillRect(renderer_, &dest_rect);
        if (tile.ch == 0) {
          continue; // No foreground glyph.
        }
        // Blend the foreground glyph on top of the background.
        SDL_SetTextureColorMod(alias_texture, tile.fg.r, tile.fg.g, tile.fg.b);
        SDL_SetTextureAlphaMod(alias_texture, tile.fg.a);
        const SDL_Rect alias_rect = alias_.get_char_rect(tile.ch);
        SDL_RenderCopy(renderer_, alias_texture, &alias_rect, &dest_rect);
      }
    }
    SDL_SetRenderTarget(renderer_, nullptr);
    return texture_;
  }
  auto read_pixels() const -> Image
  {
    SDL_SetRenderTarget(renderer_, texture_);
    int width, height;
    SDL_QueryTexture(texture_, nullptr, nullptr, &width, &height);
    Image pixels(width, height);
    SDL_RenderReadPixels(
        renderer_,
        nullptr,
        SDL_PIXELFORMAT_RGBA32,
        static_cast<void*>(pixels.data()),
        width * 4);
    SDL_SetRenderTarget(renderer_, nullptr);
    return pixels;
  }
 private:
  using cache_type = Matrix<struct TCOD_ConsoleTile, 2>;
  static int on_sdl_event(void* userdata, SDL_Event* event)
  {
    auto this_ = static_cast<impl*>(userdata);
    switch (event->type) {
      case SDL_RENDER_TARGETS_RESET: // Lost target textures.
        // Reset cache so that cells are updated.
        for (auto& cell : this_->cache_) {
          cell = {-1, {0, 0, 0, 0}, {0, 0, 0, 0}};
        }
        break;
    }
    return 0;
  }
  SDL2TilesetAlias alias_;
  cache_type cache_;
  struct SDL_Renderer* renderer_;
  struct SDL_Texture* texture_ = nullptr;
};

SDL2Renderer::SDL2Renderer()
{}
SDL2Renderer::SDL2Renderer(struct SDL_Renderer* renderer, SDL2TilesetAlias alias)
: impl_(std::make_unique<impl>(renderer, alias))
{}

SDL2Renderer::SDL2Renderer(struct SDL_Renderer* renderer,
             std::shared_ptr<Tileset> tileset)
: impl_(std::make_unique<impl>(renderer, SDL2TilesetAlias(renderer, tileset)))
{}

SDL2Renderer::SDL2Renderer(SDL2Renderer&&) noexcept = default;
SDL2Renderer& SDL2Renderer::operator=(SDL2Renderer&&) noexcept = default;
SDL2Renderer::~SDL2Renderer() = default;

struct SDL_Texture* SDL2Renderer::render(const TCOD_Console* console)
{
  if (!console) { throw; }
  return impl_->render(*console);
}
auto SDL2Renderer::read_pixels() const -> Image
{
  return impl_->read_pixels();
}
} // namespace sdl2
} // namespace tcod
