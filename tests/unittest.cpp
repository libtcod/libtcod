
#include <climits>
#include <cstddef>
#include <iostream>
#include <utility>

#include <libtcod.h>
#include <libtcod.hpp>
#include <libtcod/pathfinding/dijkstra.h>
#include <libtcod/tileset/fallback.h>

#include "catch.hpp"

namespace std {
ostream& operator<<(ostream &out, const array<ptrdiff_t, 2>& data)
{
  return out << '{' << data.at(0) << ',' << ' ' << data.at(1) << '}';
}
} // namespace std

const int WIDTH = 20;
const int HEIGHT = 10;
const char *TITLE = "Unittest";

void test_renderer(TCOD_renderer_t renderer)
{
  TCOD_console_set_custom_font(
      "data/fonts/terminal8x8_gs_ro.png",
      TCOD_FONT_LAYOUT_CP437 | TCOD_FONT_TYPE_GREYSCALE,
      0, 0);
  tcod::console::init_root(WIDTH, HEIGHT, TITLE, 0, renderer);
  REQUIRE(TCOD_console_get_width(NULL) == WIDTH);
  REQUIRE(TCOD_console_get_height(NULL) == HEIGHT);
  TCOD_console_delete(NULL);
}

TEST_CASE("SDL2 Renderer") {
  test_renderer(TCOD_RENDERER_SDL2);
}
TEST_CASE("SDL Renderer") {
  test_renderer(TCOD_RENDERER_SDL);
}
TEST_CASE("OPENGL Renderer", "[!nonportable]") {
  test_renderer(TCOD_RENDERER_OPENGL);
}
TEST_CASE("GLSL Renderer", "[!nonportable]") {
  test_renderer(TCOD_RENDERER_SDL);
}
TEST_CASE("OPENGL2 Renderer", "[!nonportable]") {
  test_renderer(TCOD_RENDERER_OPENGL2);
}

TEST_CASE("Console ascii") {
  TCODConsole console = TCODConsole(5, 1);
  console.print(0, 0, "Test");
  for (int i = 0; i < 5; ++i) {
    CHECK(console.getChar(i, 0) == static_cast<int>("Test "[i]));
  }
}

TEST_CASE("Console eascii") {
  TCODConsole console = TCODConsole(2, 1);
  const char test_str[] = { static_cast<char>(0xff), static_cast<char>(0x00) };
  console.print(0, 0, test_str);
  CHECK(console.getChar(0, 0) == 0xff);
  CHECK(console.getChar(1, 0) == 0x20);
}

TEST_CASE("Console UTF-8 BMP") {
  TCODConsole console = TCODConsole(2, 1);
  console.printf(0, 0, "☃");
  CHECK(console.getChar(0, 0) == 0x2603);
  CHECK(console.getChar(1, 0) == 0x20);
}

TEST_CASE("Console UTF-8 SMP") {
  TCODConsole console = TCODConsole(2, 1);
  console.printf(0, 0, "🌍");
  CHECK(console.getChar(0, 0) == 0x1F30D);
  CHECK(console.getChar(1, 0) == 0x20);
}

TEST_CASE("Console wchar BMP", "[!nonportable]") {
  TCODConsole console = TCODConsole(2, 1);
  console.print(0, 0, L"\u2603");
  CHECK(console.getChar(0, 0) == 0x2603);
  CHECK(console.getChar(1, 0) == 0x20);
}

/* Fails when sizeof(wchar_t) == 2 */
TEST_CASE("Console wchar SMP", "[!nonportable][!mayfail][!hide]") {
  TCODConsole console = TCODConsole(2, 1);
  console.print(0, 0, L"\U0001F30D");
  CHECK(console.getChar(0, 0) == 0x1F30D);
  CHECK(console.getChar(1, 0) == 0x20);
}

TEST_CASE("New Dijkstra")
{
  using Catch::Matchers::Equals;
  using index_type = std::array<ptrdiff_t, 2>;
  auto dist = tcod::Vector2<int>(3, 2, INT_MAX);
  auto path_map = tcod::Vector2<index_type>(3, 2);
  tcod::pathfinding::path_clear(path_map);
  dist.atf(0, 0) = 0;
  tcod::Vector2<int> cost{
      {1, 1, 1},
      {0, 1, 2},
  };
  tcod::pathfinding::dijkstra_compute(dist, cost, 2, 3, &path_map);
  const tcod::Vector2<int> EXPECTED_DIST{
      {0, 2, 4},
      {INT_MAX, 3, 7},
  };
  CHECK(dist == EXPECTED_DIST);
  const tcod::Vector2<index_type> EXPECTED_PATH_MAP{
      {{0, 0}, {0, 0}, {1, 0}},
      {{0, 1}, {0, 0}, {1, 1}},
  };
  CHECK(path_map == EXPECTED_PATH_MAP);
  const std::vector<index_type> EXPECTED_TRAVEL_PATH{{1, 1}, {0, 0}};
  CHECK_THAT(tcod::pathfinding::get_path(path_map, {2, 1}),
             Equals(EXPECTED_TRAVEL_PATH));
}

TEST_CASE("Fallback font.")
{
  REQUIRE(tcod::tileset::new_fallback_tileset({{0, 12}}));
}
