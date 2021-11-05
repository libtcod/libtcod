
#include <libtcod/console_drawing.h>

#include <catch2/catch.hpp>

#include "common.h"

TEST_CASE("Console rect") {
  auto console = tcod::Console{32, 32};
  tcod::draw_rect(console, {2, 2, 24, 24}, 0, {}, {{255, 0, 0}});
  tcod::draw_rect(console, {8, 8, 16, 1}, '-', tcod::ColorRGB{255, 255, 255}, {});
}
