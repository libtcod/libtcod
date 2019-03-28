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
#include "gl_alias.h"

#include <map>
#include <memory>
#include <mutex>

#include "gl2_ext_.h"
#include <SDL.h>
namespace tcod {
namespace sdl2 {
using image::Image;
template <class T, typename Key, typename Init>
class AliasPool {
 public:
  AliasPool()
  : mutex_(), alias_pool_()
  {};
  std::shared_ptr<T> get(const Key& key, const Init& init)
  {
    std::unique_lock<std::mutex> lock(mutex_);
    alias_pool_.size();
    std::shared_ptr<T> alias = alias_pool_[key].lock();
    if (!alias) {
      alias_pool_[key] = alias = std::make_shared<T>(init);
    }
    return alias;
  }
 private:
  std::mutex mutex_;
  std::map<Key, std::weak_ptr<T>> alias_pool_;
};
class OpenGLTilesetAlias::impl : public TilesetObserver {
  using pool_type = AliasPool<
      impl, const Tileset*, const std::shared_ptr<Tileset>&>;
 public:
  impl(const std::shared_ptr<Tileset>& tileset)
  : TilesetObserver(tileset)
  {
    glGenTextures(1, &gltexture_); gl_check();
    clear_alias();
  }
  auto prepare_alias(const TCOD_Console& console) -> uint32_t
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
    return gltexture_;
  }
  auto get_tile_position(int codepoint) const -> std::array<int, 2>
  {
    int index = local_map_.at(codepoint);
    return {index % columns(), index / columns()};
  }
  auto get_alias_shape() const -> std::array<float, 2>
  {
    return {
        static_cast<float>(texture_size_) / tile_width(),
        static_cast<float>(texture_size_) / tile_height(),
    };
  }
  auto get_alias_size() const -> std::array<float, 2>
  {
    return {
        static_cast<float>(columns() * tile_width()),
        static_cast<float>(rows() * tile_height()),
    };
  }
  auto get_alias_matrix() const -> std::array<float, 3 * 3>
  {
    float x = 1.0f / columns();
    x *= static_cast<float>(columns() * tile_width()) / texture_size_;
    float y = 1.0f / rows();
    y *= static_cast<float>(rows() * tile_height()) / texture_size_;
    return {
        x, 0, 0,
        0, y, 0,
        0, 0, 0,
    };
  }
  static pool_type pool_;
 protected:
  virtual void on_tileset_changed(
      const std::vector<std::pair<int, Tile&>> &changes) override
  {
    for (const auto& changed : changes) {
      // If this glyph is already in this alias then free it.
      int codepoint = changed.second.codepoint;
      if (codepoint < static_cast<int>(local_map_.size())
          && local_map_.at(codepoint) >= 0) {
        unallocated_.emplace_back(local_map_.at(codepoint));
        local_map_.at(codepoint) = -1;
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
    SDL_Rect alias_rect;
    if (unallocated_.size()) {
      alias_rect = get_alias_rect(unallocated_.back());
      unallocated_.pop_back();
    } else {
      alias_rect = get_alias_rect(next_alias_index_);
      local_map_.at(codepoint) = next_alias_index_++;
    }
    // Find and draw the tile glyph into the alias.
    const auto& charmap = tileset_->get_character_map();
    Image tile;
    if (codepoint >= static_cast<int>(charmap.size())
        || charmap.at(codepoint) < 0) {
      // Glyph not in this tileset, leave blank.
      tile = Image(alias_rect.w, alias_rect.h, {0, 0, 0, 0});
    } else{
      tile = tileset_->get_tiles().at(charmap.at(codepoint)).get_image();
    }
    glBindTexture(GL_TEXTURE_2D, gltexture_); gl_check();
    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        alias_rect.x,
        alias_rect.y,
        alias_rect.w,
        alias_rect.h,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        tile.data());
    return 0; // Tile is ready.
  }
  /**
   *  Replace the texture alias with an undefined texture.
   *
   *  This function depends on the value of `texture_size_` before calling.
   */
  void clear_alias()
  {
    for (auto& it : local_map_) { it = -1; }
    unallocated_.clear();
    next_alias_index_ = 0;
    glBindTexture(GL_TEXTURE_2D, gltexture_); gl_check();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl_check();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_size_, texture_size_, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr); gl_check();
    glBindTexture(GL_TEXTURE_2D, 0); gl_check();
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
  int texture_size_ = 256;
  GLuint gltexture_ = 0;
  /**
   *  The next index on the alias. Check `capacity` before using.
   */
  int next_alias_index_ = 0;
  /**
   *  Mapping of `Unicode -> texture_index`.
   */
  std::vector<int> local_map_;
  /**
   *  Alias indexes that have been temporarily freed by changed tiles.
   */
  std::vector<int> unallocated_;
};
typename OpenGLTilesetAlias::impl::pool_type OpenGLTilesetAlias::impl::pool_;

OpenGLTilesetAlias::OpenGLTilesetAlias(std::shared_ptr<Tileset> tileset)
: impl_(OpenGLTilesetAlias::impl::pool_.get(tileset.get(), tileset))
{}
const std::shared_ptr<Tileset> OpenGLTilesetAlias::get_tileset() const
{
  return impl_->get_tileset();
}
uint32_t OpenGLTilesetAlias::get_alias_texture(const TCOD_Console& console)
{
  return impl_->prepare_alias(console);
}
auto OpenGLTilesetAlias::get_alias_shape() const -> std::array<float, 2>
{
  return impl_->get_alias_shape();
}
auto OpenGLTilesetAlias::get_alias_size() const -> std::array<float, 2>
{
  return impl_->get_alias_size();
}
auto OpenGLTilesetAlias::get_tile_position(int codepoint) const
-> std::array<int, 2>
{
  return impl_->get_tile_position(codepoint);
}
auto OpenGLTilesetAlias::get_alias_matrix() const -> std::array<float, 3 * 3>
{
  return impl_->get_alias_matrix();
}
} // namespace sdl2
} // namespace tcod
