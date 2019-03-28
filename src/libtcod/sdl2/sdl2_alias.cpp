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

template <typename T, typename Key, typename Init=Key>
class SharedObjectPool {
 public:
  auto get_instance(const Key& key, const Init& init) -> std::shared_ptr<T>
  {
    std::unique_lock<std::mutex> lock(mutex_);
    std::shared_ptr<T> pointer = pool_[key].lock();
    if (!pointer) {
      pool_[key] = pointer = std::make_shared<T>(init);
    }
    return pointer;
  }
  auto get_instance(const Key& key) -> std::shared_ptr<T>
  {
    return get_instance(key, key);
  }
 private:
  std::mutex mutex_;
  std::map<Key, std::weak_ptr<T>> pool_;
};

class SDL2TilesetAlias::impl : public TilesetObserver {
 public:
  impl() = delete;
  impl(const std::shared_ptr<Tileset>& tileset, struct SDL_Renderer* renderer)
  : TilesetObserver(tileset), renderer_(renderer)
  {
    SDL_AddEventWatch(on_sdl_event, this);
    if (!renderer_) {
      throw std::invalid_argument("renderer cannot be nullptr.");
    }
    clear_alias();
  }
  impl(const std::tuple<std::shared_ptr<Tileset>, struct SDL_Renderer*>& init)
  : impl(std::get<0>(init), std::get<1>(init))
  {}
  impl(const impl&) = delete;
  impl& operator=(const impl&) = delete;
  impl(impl&&) = delete;
  impl& operator=(impl&&) = delete;
  ~impl() noexcept {
    SDL_DelEventWatch(on_sdl_event, this);
  }
  /**
   *  Prepare and return a SDL_Texture after it contains all glyphs needed by
   *  `console`.
   */
  auto prepare_alias(const TCOD_Console& console) -> SDL_Texture*
  {
    for (int i = 0; i < console.w * console.h; ++i) {
      int codepoint = console.tiles[i].ch;
      if (codepoint >= static_cast<int>(local_map_.size())
          || local_map_.at(codepoint) < 0) {
        if (ensure_tile(codepoint) == -1) {
          // The texture alias was replaced by a bigger one.
          i = -1; // Need to start over from the beginning.
          continue;
        }
      }
    }
    return texture_.get();
  }
  /**
   *  Perform a fast rectangle lookup of a codepoint.
   *
   *  Called after preparations finished by the `prepare_alias` method.
   */
  auto get_char_rect(int codepoint) const -> SDL_Rect
  {
    return get_alias_rect(local_map_.at(codepoint));
  }
 protected:
  /**
   *  Keep the texture alias in a valid state during tileset changes.
   */
  virtual void on_tileset_changed(
      const std::vector<std::pair<int, Tile&>> &changes) override
  {
    for (const auto& changed : changes) {
      // If this glyph is already in this alias then update it.
      // Otherwise just ignore it.
      int codepoint = changed.second.codepoint;
      if (codepoint < static_cast<int>(local_map_.size())
          && local_map_.at(codepoint) >= 0) {
        SDL_Rect rect(get_char_rect(codepoint));
        Image tile(changed.second.get_image());
        SDL_UpdateTexture(
            texture_.get(),
            &rect,
            tile.data(),
            static_cast<int>(sizeof(tile.data()[0]) * tile.width()));
      }
    }
  }
 private:
  /**
   *  Return the rectangle for the tile `index` on the texture.
   *
   *  Does not perform any extra checks.
   */
  auto get_alias_rect(int index) const noexcept -> SDL_Rect
  {
    return SDL_Rect{
        (index % columns()) * tile_width(),
        (index / columns()) * tile_height(),
        tile_width(),
        tile_height(),
    };
  }
  /**
   *  Make sure a codepoint exists on this alias.
   *
   *  Returns 0 on success, or returns -1 if the texture was reset and
   *  everything needs to be regenerated.
   */
  auto ensure_tile(int codepoint) -> int
  {
    if (codepoint >= static_cast<int>(local_map_.size())) {
      local_map_.resize(codepoint + 1, -1);
    }
    if (next_alias_index_ == capacity()) {
      // Replace the current texture with a larger one.
      texture_size_ *= 2;
      clear_alias();
      return -1; // Tell prepare_alias to start over.
    }
    // Allocate an area of the texture alias for this codepoint.
    SDL_Rect alias_rect(get_alias_rect(next_alias_index_));
    local_map_.at(codepoint) = next_alias_index_++;
    // Find and draw the tile glyph into the alias.
    const auto& charmap = tileset_->get_character_map();
    if (codepoint >= static_cast<int>(charmap.size())
        || charmap.at(codepoint) < 0) {
      return 0; // Glyph not in this tileset, leave blank.
    }
    Image tile(tileset_->get_tiles().at(charmap.at(codepoint)).get_image());
    SDL_UpdateTexture(texture_.get(), &alias_rect, tile.data(),
                      static_cast<int>(sizeof(tile.data()[0]) * tile.width()));
    return 0; // Tile is ready.
  }
  /**
   *  Replace the texture alias with a blank texture.
   *
   *  This function depends on the value of `texture_size_` before calling.
   */
  void clear_alias()
  {
    for (auto& it : local_map_) { it = -1; }
    next_alias_index_ = 0;
    texture_ = std::unique_ptr<SDL_Texture, sdl_deleter>(
        SDL_CreateTexture(
            renderer_,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STATIC,
            texture_size_,
            texture_size_),
        sdl_deleter());
    if (!texture_) {
      throw std::runtime_error(
          std::string("SDL_CreateTexture failed: ") + SDL_GetError());
    }
  }
  /**
   *  React to events which my invalidate the texture.
   */
  static int on_sdl_event(void* userdata, SDL_Event* event)
  {
    auto this_ = static_cast<impl*>(userdata);
    switch (event->type) {
      case SDL_RENDER_DEVICE_RESET: // Lost all textures.
        this_->clear_alias();
        break;
    }
    return 0;
  }
  /**
   *  Return the tile width of the assigned tileset.
   */
  int tile_width() const noexcept { return tileset_->get_tile_width(); }
  /**
   *  Return the tile height of the assigned tileset.
   */
  int tile_height() const noexcept { return tileset_->get_tile_height(); }
  /**
   *  Return the maximum columns supported by the current texture alias.
   */
  int columns() const noexcept { return texture_size_ / tile_width(); }
  /**
   *  Return the maximum rows supported by the current texture alias.
   */
  int rows() const noexcept { return texture_size_ / tile_height(); }
  /**
   *  Return the total number of tiles supported by the current texture alias.
   */
  int capacity() const noexcept { return columns() * rows(); }
  /**
   *  The assigned SDL2 renderer.
   */
  struct SDL_Renderer* renderer_;
  /**
   *  The texture alias.
   */
  std::unique_ptr<SDL_Texture, sdl_deleter> texture_ = nullptr;
  /**
   *  The current texture width and height. The starting value is defined here.
   */
  int texture_size_ = 256;
  /**
   *  The next index on the alias. Check `capacity` before using.
   */
  int next_alias_index_ = 0;
  /**
   *  Mapping of `Unicode -> texture_index`.
   */
  std::vector<int> local_map_;
};

class SDL2AliasPool
: public SharedObjectPool<
    SDL2TilesetAlias::impl,
    std::tuple<Tileset*, struct SDL_Renderer*>,
    std::tuple<std::shared_ptr<Tileset>, struct SDL_Renderer*>> {
};
static SDL2AliasPool sdl2_alias_pool;

SDL2TilesetAlias::SDL2TilesetAlias(struct SDL_Renderer* renderer,
                                   const std::shared_ptr<Tileset>& tileset)
: impl_(sdl2_alias_pool.get_instance({tileset.get(), renderer},
                                     {tileset, renderer}))
{}
const std::shared_ptr<Tileset> SDL2TilesetAlias::get_tileset() const
{
  return impl_->get_tileset();
}
auto SDL2TilesetAlias::prepare_alias(const TCOD_Console& console)
-> SDL_Texture*
{
  return impl_->prepare_alias(console);
}
auto SDL2TilesetAlias::get_char_rect(int codepoint) const -> SDL_Rect
{
  return impl_->get_char_rect(codepoint);
}
} // namespace sdl2
} // namespace tcod
