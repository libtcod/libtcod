
#include <libtcod/color.h>
#include <libtcod/console.h>

#include <catch2/catch.hpp>
#include <libtcod/color.hpp>
#include <sstream>

TEST_CASE("Color conversions") {
  {
    const auto tile = TCOD_ConsoleTile{
        0x20,
        tcod::ColorRGB{1, 2, 3},  // Implicit cast to RGBA, with alpha=255.
        tcod::ColorRGBA{4, 5, 6, 7},  // Also implicit cast to TCOD_ColorRGBA.
    };
    CHECK(tile.fg == tcod::ColorRGBA{1, 2, 3, 255});
    CHECK(tile.bg == tcod::ColorRGBA{4, 5, 6, 7});
  }
  {
    auto rgba = TCOD_ColorRGBA{0, 0, 0, 0};
    rgba = TCOD_ColorRGB{1, 2, 3};
    CHECK(rgba == tcod::ColorRGBA{1, 2, 3, 255});
  }
  {
    auto console = tcod::Console{4, 2};
    console.clear({0x20, tcod::ColorRGB(1, 2, 3), tcod::ColorRGBA(4, 5, 6, 7)});
    CHECK(console.at({0, 0}) == TCOD_ConsoleTile{0x20, {1, 2, 3, 255}, {4, 5, 6, 7}});
  }
}
TEST_CASE("Color IO") {
  {
    std::stringstream stream;
    stream << tcod::ColorRGB{1, 2, 3};
    CHECK(stream.str() == "{1, 2, 3}");
    tcod::ColorRGB rgb{9, 9, 9};
    stream >> rgb;
    CHECK(rgb == tcod::ColorRGB{1, 2, 3});
  }
  {
    std::stringstream stream;
    stream << tcod::ColorRGBA{1, 2, 3, 4};
    CHECK(stream.str() == "{1, 2, 3, 4}");
    tcod::ColorRGBA rgba{9, 9, 9, 9};
    stream >> rgba;
    CHECK(rgba == tcod::ColorRGBA{1, 2, 3, 4});
  }
  {
    std::stringstream stream;
    stream << TCOD_ColorRGB{1, 2, 3};
    CHECK(stream.str() == "{1, 2, 3}");
    TCOD_ColorRGB rgb{9, 9, 9};
    stream >> rgb;
    CHECK(rgb == TCOD_ColorRGB{1, 2, 3});
  }
  {
    std::stringstream stream;
    stream << TCOD_ColorRGBA{1, 2, 3, 4};
    CHECK(stream.str() == "{1, 2, 3, 4}");
    TCOD_ColorRGBA rgba{9, 9, 9, 9};
    stream >> rgba;
    CHECK(rgba == TCOD_ColorRGBA{1, 2, 3, 4});
  }
}
