
#include <catch2/catch_all.hpp>
#include <libtcod/color.hpp>
#include <libtcod/console.hpp>
#include <sstream>

/// @brief String output for TCODColor.
std::ostream& operator<<(std::ostream& os, const TCODColor& color) {
  return os << "{" << std::to_string(color.r) << ", " << std::to_string(color.g) << ", " << std::to_string(color.b)
            << "}";
}

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
TEST_CASE("TCODColor::genMap") {
  static constexpr auto map_constexpr = TCODColor::genMap<16>(
      std::array{tcod::ColorRGB{0, 0, 0}, tcod::ColorRGB{255, 0, 0}, tcod::ColorRGB{255, 255, 255}},
      std::array{0, 8, 15});
  CHECK(map_constexpr.at(0) == tcod::ColorRGB{0, 0, 0});
  CHECK(map_constexpr.at(8) == tcod::ColorRGB{255, 0, 0});
  CHECK(map_constexpr.at(15) == tcod::ColorRGB{255, 255, 255});

  auto old_map = std::array<TCODColor, 16>{};
  TCODColor::genMap(
      old_map.data(),
      3,
      std::array{TCODColor{0, 0, 0}, TCODColor{255, 0, 0}, TCODColor{255, 255, 255}}.data(),
      std::array{0, 8, 15}.data());
  for (size_t i{0}; i < map_constexpr.size(); ++i) {
    CHECK(map_constexpr.at(i) == tcod::ColorRGB{old_map.at(i)});
  }
}
TEST_CASE("Color math") {
  CHECK(TCODColor{0, 1, 200} + TCODColor{0, 2, 200} == TCODColor{0, 3, 255});

  CHECK(TCODColor{100, 100, 100} - TCODColor{0, 1, 200} == TCODColor{100, 99, 0});

  CHECK(TCODColor{128, 128, 128} * TCODColor{0, 128, 255} == TCODColor{0, 64, 128});

  CHECK(TCODColor{0, 10, 100} * 10.0f == TCODColor{0, 100, 255});
  CHECK(TCODColor{0, 10, 100} * -10.0f == TCODColor{0, 0, 0});
}
