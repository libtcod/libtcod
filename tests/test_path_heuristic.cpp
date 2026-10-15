#include <catch2/catch_all.hpp>

#include "libtcod/path.h"

TEST_CASE("A* with equal diagonal costs follows the shortest route", "[path][heuristic]") {
  int grid[6][6] = {
      {1, 1, 1, 1, 1, 1},
      {1, 0, 1, 0, 1, 1},
      {0, 1, 0, 1, 1, 1},
      {0, 0, 1, 0, 1, 1},
      {1, 1, 0, 1, 0, 0},
      {1, 0, 0, 1, 1, 1},
  };
  const auto cost = [](int, int, int x, int y, void* data) {
    return static_cast<float>(static_cast<const int (*)[6]>(data)[y][x]);
  };
  auto* path = TCOD_path_new_using_function(6, 6, cost, grid, 1.0f);
  REQUIRE(path);
  REQUIRE(TCOD_path_compute(path, 0, 0, 5, 5));
  const int length = TCOD_path_size(path);
  TCOD_path_delete(path);
  REQUIRE(length == 6);
}
