#include <catch2/catch_all.hpp>

#include "libtcod/image.hpp"

TEST_CASE("TCODImage") {
  auto img = TCODImage{};
  img = TCODImage{3, 2};
  REQUIRE(img.get_data());
  REQUIRE(img.getPixel(0, 0) == TCODColor{0, 0, 0});
  img.clear({0, 0, 0});
  img.putPixel(0, 0, {1, 2, 3});
  REQUIRE(img.getPixel(0, 0) == TCODColor{1, 2, 3});
  {
    int w{};
    int h{};
    img.getSize(&w, &h);
    REQUIRE(w == 3);
    REQUIRE(h == 2);
    REQUIRE(img.getSize() == std::array{w, h});
  }
}
