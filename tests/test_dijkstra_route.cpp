#include <catch2/catch_all.hpp>

#include "libtcod/path.h"

TEST_CASE("Dijkstra routes include the final step cost", "[path]") {
  const float diagonal = GENERATE(0.5f, 1.0f, 1.41f, 1.5f, 2.0f, 2.5f);
  float destination_cost = GENERATE(1.0f, 5.0f);
  const auto cost = [](int, int, int x, int y, void* data) {
    return x == 1 && y == 1 ? *static_cast<float*>(data) : 1.0f;
  };
  auto* path = TCOD_dijkstra_new_using_function(2, 2, cost, &destination_cost, diagonal);
  REQUIRE(path);
  TCOD_dijkstra_compute(path, 0, 0);
  REQUIRE(TCOD_dijkstra_path_set(path, 1, 1));
  int total = 0;
  int previous_x = 0;
  int previous_y = 0;
  const int diagonal_cost = static_cast<int>(diagonal * 100.0f + 0.1f);
  for (int i = 0; i < TCOD_dijkstra_size(path); ++i) {
    int x = 0;
    int y = 0;
    TCOD_dijkstra_get(path, i, &x, &y);
    const int step_cost = x != previous_x && y != previous_y ? diagonal_cost : 100;
    total += static_cast<int>(cost(previous_x, previous_y, x, y, &destination_cost) * step_cost);
    previous_x = x;
    previous_y = y;
  }
  const float distance = TCOD_dijkstra_get_distance(path, 1, 1);
  TCOD_dijkstra_delete(path);
  REQUIRE(total / 100.0f == Catch::Approx(distance));
}

TEST_CASE("Dijkstra routes follow callback direction", "[path]") {
  const auto cost = [](int from_x, int from_y, int to_x, int to_y, void*) {
    if (from_x == 1 && from_y == 0 && to_x == 1 && to_y == 1) return 0.0f;
    return to_x == 0 && to_y == 1 ? 2.0f : 1.0f;
  };
  auto* path = TCOD_dijkstra_new_using_function(2, 2, cost, nullptr, 0);
  REQUIRE(path);
  TCOD_dijkstra_compute(path, 0, 0);
  const float distance = TCOD_dijkstra_get_distance(path, 1, 1);
  REQUIRE(TCOD_dijkstra_path_set(path, 1, 1));
  int x = 0;
  int y = 0;
  TCOD_dijkstra_get(path, 0, &x, &y);
  TCOD_dijkstra_delete(path);
  REQUIRE(distance == 3.0f);
  REQUIRE(x == 0);
  REQUIRE(y == 1);
}
