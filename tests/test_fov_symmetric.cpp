#include <libtcod/fov.h>

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <utility>

namespace {
std::pair<int, int> rotate(int x, int y, int turns) {
  for (int i = 0; i < turns; ++i) {
    const int old_x = x;
    x = y;
    y = 6 - old_x;
  }
  return {x, y};
}
}  // namespace

TEST_CASE("symmetric shadowcasting rounds negative sector edges", "[fov][symmetric]") {
  const std::array<std::string_view, 7> dungeon = {
      "#######", "###...#", "###.#.#", "#.....#", "#.#.#.#", "#.#..##", "#######"};
  // Exact rational-slope reference, with the origin at (3, 1).
  const std::array<std::string_view, 7> visible = {
      "0011111", "0011111", "0011111", "0011100", "0011100", "0011000", "0011100"};
  for (int turns = 0; turns < 4; ++turns) {
    for (bool light_walls : {false, true}) {
      CAPTURE(turns, light_walls);
      TCOD_Map* map = TCOD_map_new(7, 7);
      REQUIRE(map != nullptr);
      for (int y = 0; y < 7; ++y) {
        for (int x = 0; x < 7; ++x) {
          const auto [rx, ry] = rotate(x, y, turns);
          const bool floor = dungeon[y][x] == '.';
          TCOD_map_set_properties(map, rx, ry, floor, floor);
        }
      }
      const auto [pov_x, pov_y] = rotate(3, 1, turns);
      REQUIRE(TCOD_map_compute_fov(map, pov_x, pov_y, 0, light_walls, FOV_SYMMETRIC_SHADOWCAST) == TCOD_E_OK);
      for (int y = 0; y < 7; ++y) {
        for (int x = 0; x < 7; ++x) {
          CAPTURE(x, y);
          const auto [rx, ry] = rotate(x, y, turns);
          const bool expected = visible[y][x] == '1' && (light_walls || dungeon[y][x] == '.');
          CHECK(TCOD_map_is_in_fov(map, rx, ry) == expected);
        }
      }
      TCOD_map_delete(map);
    }
  }
}
