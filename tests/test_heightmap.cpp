#include <catch2/catch_all.hpp>

#include "libtcod/heightmap.hpp"

TEST_CASE("TCODHeightmap") {
  auto hm = TCODHeightMap(0, 0);  // Test zero size
  hm = TCODHeightMap(1, 1);  // Test assign new shape
  REQUIRE(hm.getValue(0, 0) == 0);
  hm.setValue(0, 0, 1.0f);  // Should be zeroed
  REQUIRE(hm.getValue(0, 0) == 1.0f);

  auto hm2 = TCODHeightMap(1, 1);
  hm2.setValue(0, 0, 3.0f);
  hm = hm2;  // Test same-size copy
  REQUIRE(hm.getValue(0, 0) == 3.0f);

  hm2 = TCODHeightMap(2, 2);
  hm2.setValue(0, 0, 4.0f);
  hm = std::move(hm2);  // Test move
  REQUIRE(hm.getValue(0, 0) == 4.0f);
  REQUIRE(hm.w == 2);

  hm.clear();
  REQUIRE(hm.getValue(0, 0) == 0.0f);
}
