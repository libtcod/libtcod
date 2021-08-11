
#include <libtcod/console.h>

#include <catch2/catch.hpp>
#include <libtcod/console.hpp>

#include "common.h"

TEST_CASE("Console basics") {
  auto console = tcod::new_console({{3, 2}});
  for (const auto& tile : *console) {
    REQUIRE(tile.ch == 0x20);
    REQUIRE(tile.fg == TCOD_ColorRGBA{255, 255, 255, 255});
    REQUIRE(tile.bg == TCOD_ColorRGBA{0, 0, 0, 255});
  }
  for (int y = 0; y < console->h; ++y) {
    for (int x = 0; x < console->w; ++x) {
      REQUIRE(console->in_bounds({x, y}));
      console->at({x, y}) = {0x20, {255, 255, 255, 255}, {static_cast<uint8_t>(x), static_cast<uint8_t>(y), 0, 255}};
    }
  }
  REQUIRE(console->at(2, 1) == (*console)[{2, 1}]);
  REQUIRE(console->at({1, 1}) == (*console)[{1, 1}]);
  REQUIRE(console->in_bounds({1000, 1000}) == false);
}

TEST_CASE("Console bounds", "[!throws]") {
  auto console = tcod::new_console({{3, 2}});
  REQUIRE_THROWS(console->at({1000, 1000}));
}

TEST_CASE("TCODConsole conversion") {
  auto console = TCODConsole(3, 2);
  tcod::print(static_cast<TCOD_Console&>(console), {0, 0}, "@", nullptr, nullptr);
  REQUIRE(static_cast<const TCOD_Console&>(console).at(0, 0).ch == '@');
}
