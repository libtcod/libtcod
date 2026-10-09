#include <array>
#include <catch2/catch_all.hpp>
#include <libtcod/color.hpp>
#include <libtcod/tileset.hpp>
#include <libtcod/tileset_bdf.hpp>
#ifndef NO_SDL
#include <SDL3/SDL.h>
#include <libtcod/tileset_render.h>

#include <memory>
#endif

#include "common.hpp"

TEST_CASE("Load a color-key tilesheet with a mapped space.", "[tileset]") {
  const int space_tile = GENERATE(0, 1, 2, 3);
  const TCOD_ColorRGBA key{255, 0, 255, 255};
  const TCOD_ColorRGBA foreground{255, 255, 255, 255};
  auto pixels = std::array<TCOD_ColorRGBA, 16>{};
  pixels.fill(key);
  auto charmap = std::array<int, 4>{'A', 'B', 'C', 'D'};
  charmap.at(space_tile) = ' ';
  for (int tile = 0; tile < 4; ++tile) {
    if (tile != space_tile) pixels.at((tile / 2) * 8 + (tile % 2) * 2) = foreground;
  }
  auto tileset = tcod::TilesetPtr{TCOD_tileset_load_raw(4, 4, pixels.data(), 2, 2, 4, charmap.data())};
  REQUIRE(tileset);
  for (int tile = 0; tile < 4; ++tile) {
    const auto* glyph = TCOD_tileset_get_tile(tileset.get(), charmap.at(tile));
    REQUIRE(glyph);
    CHECK(glyph[0] == (tile == space_tile ? TCOD_ColorRGBA{0, 0, 0, 0} : foreground));
    for (int i = 1; i < 4; ++i) CHECK(glyph[i] == TCOD_ColorRGBA{0, 0, 0, 0});
  }
}

TEST_CASE("Load a color-key tilesheet with a partial image column.", "[tileset]") {
  const std::array<TCOD_ColorRGBA, 10> pixels{
      {{255, 255, 255, 255},
       {255, 0, 255, 255},
       {255, 255, 255, 255},
       {255, 0, 255, 255},
       {255, 0, 255, 255},
       {255, 0, 255, 255},
       {255, 255, 255, 255},
       {255, 0, 255, 255},
       {0, 255, 0, 255},
       {0, 255, 0, 255}}};
  const std::array<int, 4> charmap{{'A', 'C', ' ', 'B'}};
  auto tileset = tcod::TilesetPtr{TCOD_tileset_load_raw(5, 2, pixels.data(), 2, 2, 4, charmap.data())};
  REQUIRE(tileset);
  CHECK(TCOD_tileset_get_tile(tileset.get(), ' ')[0] == TCOD_ColorRGBA{0, 0, 0, 0});
  CHECK(TCOD_tileset_get_tile(tileset.get(), ' ')[1] == TCOD_ColorRGBA{0, 0, 0, 0});
  CHECK(TCOD_tileset_get_tile(tileset.get(), 'A')[1] == TCOD_ColorRGBA{0, 0, 0, 0});
  CHECK(TCOD_tileset_get_tile(tileset.get(), 'B')[0] == TCOD_ColorRGBA{255, 255, 255, 255});
  CHECK(TCOD_tileset_get_tile(tileset.get(), 'B')[1] == TCOD_ColorRGBA{0, 0, 0, 0});
}

TEST_CASE("Load a color-key tilesheet without a character map.", "[tileset]") {
  auto pixels = std::array<TCOD_ColorRGBA, 66>{};
  pixels.fill({255, 0, 255, 255});
  pixels[0] = {255, 255, 255, 255};
  auto tileset = tcod::TilesetPtr{TCOD_tileset_load_raw(66, 1, pixels.data(), 33, 1, 0, nullptr)};
  REQUIRE(tileset);
  CHECK(TCOD_tileset_get_tile(tileset.get(), 32)[0] == TCOD_ColorRGBA{0, 0, 0, 0});
  CHECK(TCOD_tileset_get_tile(tileset.get(), 0)[1] == TCOD_ColorRGBA{0, 0, 0, 0});
}

TEST_CASE("Load a tilesheet without a mapped space uses the first tile.", "[tileset]") {
  const std::array<TCOD_ColorRGBA, 4> pixels{
      {{255, 0, 255, 255}, {255, 0, 255, 255}, {255, 255, 255, 255}, {255, 0, 255, 255}}};
  const std::array<int, 2> charmap{{'A', 'B'}};
  auto tileset = tcod::TilesetPtr{TCOD_tileset_load_raw(4, 1, pixels.data(), 2, 1, 2, charmap.data())};
  REQUIRE(tileset);
  CHECK(TCOD_tileset_get_tile(tileset.get(), 'A')[0] == TCOD_ColorRGBA{0, 0, 0, 0});
  CHECK(TCOD_tileset_get_tile(tileset.get(), 'B')[0] == TCOD_ColorRGBA{255, 255, 255, 255});
  CHECK(TCOD_tileset_get_tile(tileset.get(), 'B')[1] == TCOD_ColorRGBA{0, 0, 0, 0});
}

TEST_CASE("Load a colored tilesheet with a nonuniform space.", "[tileset]") {
  const std::array<TCOD_ColorRGBA, 4> pixels{
      {{255, 0, 255, 255}, {255, 0, 255, 255}, {255, 0, 255, 255}, {0, 255, 0, 128}}};
  const std::array<int, 2> charmap{{'A', ' '}};
  auto tileset = tcod::TilesetPtr{TCOD_tileset_load_raw(4, 1, pixels.data(), 2, 1, 2, charmap.data())};
  REQUIRE(tileset);
  CHECK(TCOD_tileset_get_tile(tileset.get(), 'A')[0] == pixels[0]);
  CHECK(TCOD_tileset_get_tile(tileset.get(), ' ')[0] == pixels[2]);
  CHECK(TCOD_tileset_get_tile(tileset.get(), ' ')[1] == pixels[3]);
}

TEST_CASE("Load a tilesheet uses the last mapping of a space.", "[tileset]") {
  const std::array<TCOD_ColorRGBA, 6> pixels{
      {{0, 255, 0, 255},
       {0, 255, 0, 255},
       {255, 255, 255, 255},
       {255, 0, 255, 255},
       {255, 0, 255, 255},
       {255, 0, 255, 255}}};
  const std::array<int, 3> charmap{{' ', 'A', ' '}};
  auto tileset = tcod::TilesetPtr{TCOD_tileset_load_raw(6, 1, pixels.data(), 3, 1, 3, charmap.data())};
  REQUIRE(tileset);
  CHECK(TCOD_tileset_get_tile(tileset.get(), ' ')[0] == TCOD_ColorRGBA{0, 0, 0, 0});
  CHECK(TCOD_tileset_get_tile(tileset.get(), 'A')[1] == TCOD_ColorRGBA{0, 0, 0, 0});
}

TEST_CASE("Load a tilesheet only considers assigned characters.", "[tileset]") {
  const std::array<TCOD_ColorRGBA, 4> pixels{
      {{0, 255, 0, 255}, {0, 255, 0, 255}, {255, 0, 255, 255}, {255, 0, 255, 255}}};
  const std::array<int, 2> charmap{{'A', ' '}};
  auto tileset = tcod::TilesetPtr{TCOD_tileset_load_raw(4, 1, pixels.data(), 2, 1, 1, charmap.data())};
  REQUIRE(tileset);
  CHECK(TCOD_tileset_get_tile(tileset.get(), 'A')[0] == TCOD_ColorRGBA{0, 0, 0, 0});
}

TEST_CASE("Load a tilesheet preserves grayscale normalization.", "[tileset]") {
  const std::array<TCOD_ColorRGBA, 4> pixels{
      {{255, 255, 255, 255}, {128, 128, 128, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}}};
  const std::array<int, 2> charmap{{'A', ' '}};
  auto tileset = tcod::TilesetPtr{TCOD_tileset_load_raw(4, 1, pixels.data(), 2, 1, 2, charmap.data())};
  REQUIRE(tileset);
  CHECK(TCOD_tileset_get_tile(tileset.get(), 'A')[0] == TCOD_ColorRGBA{255, 255, 255, 255});
  CHECK(TCOD_tileset_get_tile(tileset.get(), 'A')[1] == TCOD_ColorRGBA{255, 255, 255, 128});
}

#ifndef NO_SDL
TEST_CASE("Render a color-key tilesheet over the console background.", "[tileset]") {
  const std::array<TCOD_ColorRGBA, 4> pixels{
      {{255, 255, 255, 255}, {255, 0, 255, 255}, {255, 0, 255, 255}, {255, 0, 255, 255}}};
  const std::array<int, 2> charmap{{'A', ' '}};
  auto tileset = tcod::TilesetPtr{TCOD_tileset_load_raw(4, 1, pixels.data(), 2, 1, 2, charmap.data())};
  REQUIRE(tileset);
  auto console = tcod::Console{1, 1};
  console.at({0, 0}) = {'A', {255, 255, 255, 255}, {0, 0, 255, 255}};
  SDL_Surface* surface = nullptr;
  const auto result = TCOD_tileset_render_to_surface(tileset.get(), console.get(), nullptr, &surface);
  auto owned_surface = std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)>{surface, SDL_DestroySurface};
  REQUIRE(result == TCOD_E_OK);
  REQUIRE(owned_surface);
  const auto* rendered = static_cast<const TCOD_ColorRGBA*>(surface->pixels);
  CHECK(rendered[0] == TCOD_ColorRGBA{255, 255, 255, 255});
  CHECK(rendered[1] == TCOD_ColorRGBA{0, 0, 255, 255});
}
#endif

#ifndef TCOD_NO_PNG
TEST_CASE("Load tilesheet.") {
  auto tileset = tcod::load_tilesheet(get_file("fonts/terminal8x8_gs_ro.png"), {16, 16}, tcod::CHARMAP_CP437);
  REQUIRE(tileset.get());
  tileset = tcod::load_tilesheet(get_file("fonts/dejavu8x8_gs_tc.png"), {32, 8}, tcod::CHARMAP_TCOD);
  REQUIRE(tileset.get());
}

TEST_CASE("Missing tilesheet.", "[!throws]") {
  REQUIRE_THROWS(tcod::load_tilesheet("/nonexistant.file", {16, 16}, tcod::CHARMAP_CP437));
}
#endif  // TCOD_NO_PNG

TEST_CASE("Load BDF.") {
  auto tileset = tcod::load_bdf(get_file("fonts/ucs-fonts/4x6.bdf"));
  REQUIRE(tileset);
  tileset = tcod::load_bdf(get_file("fonts/Tamzen5x9r.bdf"));
  REQUIRE(tileset);
}
