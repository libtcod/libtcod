#include <libtcod/console_printing.h>

#include <catch2/catch.hpp>

#include "common.h"

TEST_CASE("tcod::print") {
  tcod::ConsolePtr console = tcod::new_console(5, 1);
  tcod::print(*console, 0, 0, "Test", nullptr, nullptr);
  CHECK(to_string(*console) == "Test ");
}

TEST_CASE("tcod::printf") {
  tcod::ConsolePtr console = tcod::new_console(5, 1);
  tcod::printf(*console, 0, 0, nullptr, nullptr, TCOD_BKGND_SET, TCOD_LEFT, "%s", "Test");
  CHECK(to_string(*console) == "Test ");
}
