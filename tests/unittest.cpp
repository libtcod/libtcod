
#include <climits>
#include <cstddef>
#include <iostream>
#include <utility>

#include <libtcod.h>
#include <libtcod.hpp>
#include <libtcod/pathfinding/dijkstra.h>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "catch_reporter_automake.hpp"
#include "catch_reporter_tap.hpp"

namespace std {
ostream& operator<<(ostream &out,
                         const pair<ptrdiff_t, ptrdiff_t>& data)
{
  return out << '{' << data.first << ',' << ' ' << data.second << '}';
}
} // namespace std

const int WIDTH = 20;
const int HEIGHT = 10;
const char *TITLE = "Unittest";

TEST_CASE("SDL renderer") {
  tcod::console::init_root(WIDTH, HEIGHT, TITLE, 0, TCOD_RENDERER_SDL);
  REQUIRE(TCOD_console_get_width(NULL) == WIDTH);
  REQUIRE(TCOD_console_get_height(NULL) == HEIGHT);

  TCOD_console_delete(NULL);
}

TEST_CASE("Console ascii") {
  TCODConsole console = TCODConsole(5, 1);
  console.print(0, 0, "Test");
  for (int i = 0; i < 5; ++i) {
    CHECK(console.getChar(i, 0) == (int)("Test "[i]));
  }
}

TEST_CASE("Console eascii") {
  TCODConsole console = TCODConsole(2, 1);
  const char test_str[] = { (char)0xff, (char)0x00 };
  console.print(0, 0, test_str);
  CHECK(console.getChar(0, 0) == 0xff);
  CHECK(console.getChar(1, 0) == 0x20);
}

TEST_CASE("Console UTF-8 BMP") {
  TCODConsole console = TCODConsole(2, 1);
  console.printf(0, 0, u8"\u2603");
  CHECK(console.getChar(0, 0) == 0x2603);
  CHECK(console.getChar(1, 0) == 0x20);
}

TEST_CASE("Console UTF-8 SMP") {
  TCODConsole console = TCODConsole(2, 1);
  console.printf(0, 0, u8"\U0001F30D");
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
TEST_CASE("Console wchar SMP", "[!nonportable][!mayfail]") {
  TCODConsole console = TCODConsole(2, 1);
  console.print(0, 0, L"\U0001F30D");
  CHECK(console.getChar(0, 0) == 0x1F30D);
  CHECK(console.getChar(1, 0) == 0x20);
}

TEST_CASE("New Dijkstra")
{
  using Catch::Matchers::Equals;
  using index_type = std::pair<ptrdiff_t, ptrdiff_t>;
  auto dist = tcod::Vector2<int>(3, 2, INT_MAX);
  auto path_map = tcod::Vector2<index_type>(3, 2);
  tcod::pathfinding::path_clear(path_map);
  dist.at(0, 0) = 0;
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
