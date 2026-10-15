#include <catch2/catch_all.hpp>

#include "libtcod/path.h"

TEST_CASE("A* settles the destination before finishing", "[path]") {
  const auto cost = [](int, int, int x, int y, void*) { return x == 2 && y == 2 ? 5.0f : 1.0f; };
  auto* path = TCOD_path_new_using_function(3, 3, cost, nullptr, 1.5f);
  REQUIRE(path);
  REQUIRE(TCOD_path_compute(path, 0, 0, 2, 2));
  float total = 0;
  int previous_x = 0;
  int previous_y = 0;
  for (int i = 0; i < TCOD_path_size(path); ++i) {
    int x = 0;
    int y = 0;
    TCOD_path_get(path, i, &x, &y);
    total += cost(previous_x, previous_y, x, y, nullptr) * (x != previous_x && y != previous_y ? 1.5f : 1.0f);
    previous_x = x;
    previous_y = y;
  }
  TCOD_path_delete(path);
  REQUIRE(total == 7.5f);
}
