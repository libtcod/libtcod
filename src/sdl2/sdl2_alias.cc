
#include "sdl2_alias.h"

#include <map>
#include <memory>
#include <stdexcept>
#include <tuple>

#include <SDL.h>
namespace tcod {
namespace sdl2 {
using image::Image;
std::map<std::tuple<const Tileset*, const struct SDL_Renderer*>,
         std::shared_ptr<SDL2InternalTilesetAlias_>> sdl2_alias_pool = {};

class SDL2InternalTilesetAlias_: public TilesetObserver {
 public:
  using key_type = std::tuple<const Tileset*, const struct SDL_Renderer*>;
  SDL2InternalTilesetAlias_(struct SDL_Renderer* renderer,
                            std::shared_ptr<Tileset> tileset)
  : TilesetObserver(tileset), renderer_(renderer)
  {
    if (!renderer_) {
      throw std::invalid_argument("renderer cannot be nullptr.");
    }
    sync_alias();
  }
  SDL_Texture* get_texture_alias()
  {
    return texture_;
  }
  struct SDL_Renderer* renderer_;
  struct SDL_Texture* texture_;
 protected:
  virtual void on_tileset_changed(
      const std::vector<std::pair<int, Tile&>> &changes) override
  {
    sync_alias();
  }
 private:
  void clear_alias()
  {
    if (texture_) { SDL_DestroyTexture(texture_); }
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
    texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA32,
                                 SDL_TEXTUREACCESS_STATIC, width, height);
    Image alias(width, height);
    for (size_t i = 0; i < tiles.size(); ++i) {
      Image tile = tiles.at(i).get_image();
      for (int y = 0; y < tile_height; ++y) {
        for (int x = 0; x < tile_width; ++x) {
          alias.at(x + tile_width * i, y) = tile.at(x, y);
        }
      }
    }
    SDL_UpdateTexture(texture_, nullptr, alias.data(),
                      sizeof(alias.at(0,0)) * alias.width());
  }
};

SDL2TilesetAlias::SDL2TilesetAlias(struct SDL_Renderer* renderer,
                                   std::shared_ptr<Tileset> tileset)
{
  SDL2InternalTilesetAlias_::key_type key =
      std::make_tuple(tileset.get(), renderer);
  auto alias_it = sdl2_alias_pool.find(key);
  if (alias_it == sdl2_alias_pool.end()) {
    alias_it = sdl2_alias_pool.emplace(
        key,
        std::make_shared<SDL2InternalTilesetAlias_>(renderer, tileset)).first;
  }
  alias_ = alias_it->second;
}

std::shared_ptr<Tileset>& SDL2TilesetAlias::get_tileset()
{
  return alias_->get_tileset();
}

SDL_Texture*& SDL2TilesetAlias::get_texture_alias()
{
  return alias_->texture_;
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
