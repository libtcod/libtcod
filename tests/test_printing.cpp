#include <libtcod/console_printing.h>

#include <catch2/catch.hpp>

#include "common.h"

TEST_CASE("tcod::print") {
  tcod::ConsolePtr console = tcod::new_console(5, 1);
  static constexpr auto FG = TCOD_ColorRGB{1, 2, 3};
  static constexpr auto BG = TCOD_ColorRGB{4, 5, 6};
  tcod::print(*console, {0, 0}, "Test", &FG, &BG);
  CHECK(to_string(*console) == "Test ");
  CHECK(console->at(0, 0).fg == TCOD_ColorRGBA{1, 2, 3, 255});
  CHECK(console->at(0, 0).bg == TCOD_ColorRGBA{4, 5, 6, 255});
  CHECK(console->at(4, 0).fg == TCOD_ColorRGBA{255, 255, 255, 255});
  CHECK(console->at(4, 0).bg == TCOD_ColorRGBA{0, 0, 0, 255});
}

TEST_CASE("String from printf.") { CHECK(tcod::stringf("%s%s%s", "1", "2", "3") == "123"); }
