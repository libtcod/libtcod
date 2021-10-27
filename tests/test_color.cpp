
#include <libtcod/color.h>
#include <libtcod/console.h>

#include <catch2/catch.hpp>
#include <libtcod/color.hpp>

static bool rgba_equals(const TCOD_ColorRGBA* lhs, const TCOD_ColorRGBA* rhs) { return (*lhs) == (*rhs); }
static bool rgb_equals(const TCOD_ColorRGB* lhs, const TCOD_ColorRGB* rhs) { return (*lhs) == (*rhs); }

TEST_CASE("Color conversions") {
  auto tile = TCOD_ConsoleTile{
      0x20,
      tcod::ColorRGB{1, 2, 3},  // Implicit cast to RGBA, with alpha=255.
      tcod::ColorRGBA{4, 5, 6, 7},  // Also implicit cast to TCOD_ColorRGBA.
  };
  REQUIRE(tile.fg == tcod::ColorRGBA{1, 2, 3, 255});
  REQUIRE(tile.bg == tcod::ColorRGBA{4, 5, 6, 7});
  // Check implicit casts to pointers.
  REQUIRE(rgb_equals(tcod::ColorRGB{1, 2, 3}, tcod::ColorRGB{1, 2, 3}));
  REQUIRE(rgba_equals(tcod::ColorRGBA{1, 2, 3}, tcod::ColorRGBA{1, 2, 3}));
}
