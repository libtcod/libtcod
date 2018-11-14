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
#include "sdl2_alias.h"

#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <tuple>

#include <SDL.h>
namespace tcod {
namespace sdl2 {
using image::Image;

struct sdl_deleter
{
  void operator()(SDL_Window *p) const { SDL_DestroyWindow(p); }
  void operator()(SDL_Renderer *p) const { SDL_DestroyRenderer(p); }
  void operator()(SDL_Texture *p) const { SDL_DestroyTexture(p); }
};

std::map<std::tuple<const Tileset*, const struct SDL_Renderer*>,
         std::weak_ptr<SDL2InternalTilesetAlias_>> sdl2_alias_pool = {};

class SDL2InternalTilesetAlias_: public TilesetObserver {
 public:
  using key_type = std::tuple<const Tileset*, const struct SDL_Renderer*>;
  SDL2InternalTilesetAlias_(struct SDL_Renderer* renderer,
                            std::shared_ptr<Tileset> tileset)
  : TilesetObserver(tileset), renderer_(renderer), texture_(nullptr)
  {
    SDL_AddEventWatch(on_sdl_event, this);
    if (!renderer_) {
      throw std::invalid_argument("renderer cannot be nullptr.");
    }
    sync_alias();
  }
  ~SDL2InternalTilesetAlias_() {
    SDL_DelEventWatch(on_sdl_event, this);
  }
  SDL_Texture* get_texture_alias()
  {
    return texture_.get();
  }
  struct SDL_Renderer* renderer_;
  struct std::unique_ptr<SDL_Texture, sdl_deleter> texture_;
 protected:
  virtual void on_tileset_changed(
      const std::vector<std::pair<int, Tile&>> &changes) override
  {
    sync_alias();
  }
 private:
  void clear_alias()
  {
    texture_ = nullptr;
  }
  void sync_alias()
  {
    clear_alias();
    const std::vector<Tile>& tiles = tileset_->get_tiles();
    int tile_width = tileset_->get_tile_width();
    int tile_height = tileset_->get_tile_height();
    int width = tile_width * tiles.size();
    int height = tile_height;
    texture_ = std::unique_ptr<SDL_Texture, sdl_deleter>(
        SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA32,
                          SDL_TEXTUREACCESS_STATIC, width, height),
        sdl_deleter());
    Image alias(width, height);
    for (size_t i = 0; i < tiles.size(); ++i) {
      Image tile = tiles.at(i).get_image();
      for (int y = 0; y < tile_height; ++y) {
        for (int x = 0; x < tile_width; ++x) {
          alias.at(x + tile_width * i, y) = tile.at(x, y);
        }
      }
    }
    SDL_UpdateTexture(texture_.get(), nullptr, alias.data(),
                      sizeof(alias.at(0,0)) * alias.width());
  }
  static int on_sdl_event(void* userdata, SDL_Event* event)
  {
    auto this_ = static_cast<SDL2InternalTilesetAlias_*>(userdata);
    switch (event->type) {
      case SDL_RENDER_DEVICE_RESET: // Lost all textures.
        this_->sync_alias();
        break;
    }
    return 0;
  }
};

SDL2TilesetAlias::SDL2TilesetAlias(struct SDL_Renderer* renderer,
                                   std::shared_ptr<Tileset> tileset)
{
  static std::mutex mutex;
  std::unique_lock<std::mutex> lock(mutex);
  SDL2InternalTilesetAlias_::key_type key =
      std::make_tuple(tileset.get(), renderer);
  alias_ = sdl2_alias_pool[key].lock();
  if (!alias_) {
    sdl2_alias_pool[key] = alias_ =
        std::make_shared<SDL2InternalTilesetAlias_>(renderer, tileset);
  }
}

std::shared_ptr<Tileset>& SDL2TilesetAlias::get_tileset()
{
  return alias_->get_tileset();
}

SDL_Texture* SDL2TilesetAlias::get_texture_alias()
{
  return alias_->texture_.get();
}
SDL_Rect SDL2TilesetAlias::get_char_rect(int codepoint) {
  auto& charmap = alias_->get_tileset()->get_character_map();
  int tile_index = 0;
  if (static_cast<unsigned int>(codepoint) < charmap.size()) {
    tile_index = charmap.at(codepoint);
  }
  return SDL_Rect{
      alias_->get_tileset()->get_tile_width() * tile_index,
      0,
      alias_->get_tileset()->get_tile_width(),
      alias_->get_tileset()->get_tile_height(),
  };
}

} // namespace sdl2
} // namespace tcod
