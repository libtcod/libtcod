/* libtcod
 * Copyright © 2008-2019 Jice and the libtcod contributors.
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
class OpenGLInternalTilesetAlias_ : public TilesetObserver {
 public:
  OpenGLInternalTilesetAlias_(const std::shared_ptr<Tileset>& tileset)
  : TilesetObserver(tileset)
  {
    SDL_AddEventWatch(on_sdl_event, this);
    glGenTextures(1, &gltexture_); gl_check();
    sync_alias();
  }
  ~OpenGLInternalTilesetAlias_() {
    SDL_DelEventWatch(on_sdl_event, this);
    if (gltexture_) {
      glDeleteTextures(1, &gltexture_); gl_check();
    }
  }
  GLuint gltexture_ = 0;
 protected:
  virtual void on_tileset_changed(
      const std::vector<std::pair<int, Tile&>> &changes) override
  {
    sync_alias();
  }
 private:
  void sync_alias()
  {
    const std::vector<Tile>& tiles = tileset_->get_tiles();
    int tile_width = tileset_->get_tile_width();
    int tile_height = tileset_->get_tile_height();
    int width = static_cast<int>(tile_width * tiles.size());
    int height = tile_height;
    Image alias(width, height);
    for (int i = 0; i < tiles.size(); ++i) {
      Image tile = tiles.at(i).get_image();
      for (int y = 0; y < tile_height; ++y) {
        for (int x = 0; x < tile_width; ++x) {
          alias.at(x + tile_width * i, y) = tile.at(x, y);
        }
      }
    }
    glBindTexture(GL_TEXTURE_2D, gltexture_); gl_check();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl_check();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, alias.data()); gl_check();
  }
  static int on_sdl_event(void* userdata, SDL_Event* event)
  {
    auto this_ = static_cast<OpenGLInternalTilesetAlias_*>(userdata);
    switch (event->type) {
      case SDL_RENDER_DEVICE_RESET: // Lost all textures.
        this_->sync_alias();
        break;
    }
    return 0;
  }
};
static AliasPool<OpenGLInternalTilesetAlias_, const Tileset*,
                 const std::shared_ptr<Tileset>&> opengl_alias_pool_;
OpenGLTilesetAlias::OpenGLTilesetAlias(std::shared_ptr<Tileset> tileset)
: alias_(opengl_alias_pool_.get(tileset.get(), tileset))
{}
const std::shared_ptr<Tileset>& OpenGLTilesetAlias::get_tileset()
{
  return alias_->get_tileset();
}
uint32_t OpenGLTilesetAlias::get_alias_texture()
{
  return alias_->gltexture_;
}
std::array<int, 2> OpenGLTilesetAlias::get_alias_shape()
{
  return {
      static_cast<int>(alias_->get_tileset()->get_tiles().size()),
      1,
  };
}
std::array<int, 2> OpenGLTilesetAlias::get_alias_size()
{
  auto shape = get_alias_shape();
  return {
      shape[0] * get_tileset()->get_tile_width(),
      shape[1] * get_tileset()->get_tile_height(),
  };
}
std::array<int, 2> OpenGLTilesetAlias::get_tile_position(int codepoint)
{
  auto& charmap = alias_->get_tileset()->get_character_map();
  int tile_index = 0;
  if (static_cast<unsigned int>(codepoint) < charmap.size()) {
    tile_index = charmap.at(codepoint);
  }
  return {tile_index, 1};
}
} // namespace sdl2
} // namespace tcod
