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

TEST_CASE("A* estimates cardinal and diagonal step costs", "[path][heuristic]") {
  const auto [diagonal, even_cost, odd_cost, mixed_cost] = GENERATE(
      table<float, float, float, float>({
          {0.0f, 2.0f, 3.0f, 4.0f},
          {0.25f, 0.5f, 1.5f, 0.75f},
          {0.5f, 1.0f, 2.0f, 1.5f},
          {0.75f, 1.5f, 2.5f, 2.25f},
          {1.0f, 2.0f, 3.0f, 3.0f},
          {1.25f, 2.0f, 3.0f, 3.25f},
          {1.5f, 2.0f, 3.0f, 3.5f},
          {2.0f, 2.0f, 3.0f, 4.0f},
          {3.0f, 2.0f, 3.0f, 4.0f},
      }));
  const auto cost = [](int, int, int, int, void*) { return 1.0f; };
  for (int goal = 0; goal < 3; ++goal) {
    const int goal_x = goal == 0 ? 2 : 3;
    const int goal_y = goal == 2 ? 1 : 0;
    const float expected = goal == 0 ? even_cost : (goal == 1 ? odd_cost : mixed_cost);
    auto* path = TCOD_path_new_using_function(4, 3, cost, nullptr, diagonal);
    REQUIRE(path);
    REQUIRE(TCOD_path_compute(path, 0, 0, goal_x, goal_y));
    float actual = 0;
    int previous_x = 0;
    int previous_y = 0;
    for (int i = 0; i < TCOD_path_size(path); ++i) {
      int x, y;
      TCOD_path_get(path, i, &x, &y);
      actual += x != previous_x && y != previous_y ? diagonal : 1.0f;
      previous_x = x;
      previous_y = y;
    }
    TCOD_path_delete(path);
    REQUIRE(actual == expected);
  }
}
