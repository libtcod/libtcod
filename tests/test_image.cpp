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

#ifndef NO_SDL
TEST_CASE("TCODImage load png") { auto img = TCODImage{"data/img/circle.png"}; }
#endif  // NO_SDL

TEST_CASE("Image downsampling weights each corner equally", "[image]") {
  const auto corner_x = GENERATE(0, 1);
  const auto corner_y = GENERATE(0, 1);
  auto image = TCODImage{2, 2};
  TCOD_image_put_pixel(image.get_data(), corner_x, corner_y, {255, 0, 0});
  TCOD_image_scale(image.get_data(), 1, 1);
  const auto pixel = TCOD_image_get_pixel(image.get_data(), 0, 0);
  REQUIRE(pixel.r == 64);
  REQUIRE(pixel.g == 0);
  REQUIRE(pixel.b == 0);
}

TEST_CASE("Image downsampling combines full and fractional pixels", "[image]") {
  auto image = TCODImage{3, 4};
  for (int y = 0; y < 4; ++y) {
    for (int x = 0; x < 3; ++x) {
      TCOD_image_put_pixel(image.get_data(), x, y, {uint8_t(x * 30), uint8_t(y * 30), 120});
    }
  }
  TCOD_image_scale(image.get_data(), 2, 2);
  for (int y = 0; y < 2; ++y) {
    for (int x = 0; x < 2; ++x) {
      const auto pixel = TCOD_image_get_pixel(image.get_data(), x, y);
      REQUIRE(pixel.r == 10 + x * 40);
      REQUIRE(pixel.g == 15 + y * 60);
      REQUIRE(pixel.b == 120);
    }
  }
}
