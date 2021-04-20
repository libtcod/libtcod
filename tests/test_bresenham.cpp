
#include <libtcod/bresenham.h>

#include <array>
#include <catch2/catch.hpp>
#include <vector>

/**
    Return a vector of bresenham coordinates, including both endpoints.
 */
std::vector<std::array<int, 2>> generate_line(const std::array<int, 2>& begin, const std::array<int, 2>& end) {
  TCOD_bresenham_data_t bresenham_stack_data;
  std::vector<std::array<int, 2>> line;
  int x = begin.at(0);
  int y = begin.at(1);
  TCOD_line_init_mt(begin.at(0), begin.at(1), end.at(0), end.at(1), &bresenham_stack_data);
  line.push_back(begin);
  while (!TCOD_line_step_mt(&x, &y, &bresenham_stack_data)) {
    line.push_back({x, y});
  }
  return line;
}

/** Dummy callback for older bresenham functions. */
bool null_bresenham_callback(int x, int y) { return true; }

TEST_CASE("TCOD_line_step_mt") {
  const std::vector<std::array<int, 2>> EXPECTED = {
      {0, 0}, {1, 0}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 2}, {7, 2}, {8, 2}, {9, 2}, {10, 3}, {11, 3}};
  REQUIRE(generate_line({0, 0}, {11, 3}) == EXPECTED);

  const std::vector<std::array<int, 2>> EXPECTED2 = {
      {11, 3}, {10, 3}, {9, 2}, {8, 2}, {7, 2}, {6, 2}, {5, 1}, {4, 1}, {3, 1}, {2, 1}, {1, 0}, {0, 0}};
  REQUIRE(generate_line({11, 3}, {0, 0}) == EXPECTED2);
}

TEST_CASE("bresenham benchmarks", "[benchmark]") {
  BENCHMARK("TCOD_line_step_mt") {
    TCOD_bresenham_data_t bresenham_stack_data;
    int x;
    int y;
    TCOD_line_init_mt(0, 0, 11, 3, &bresenham_stack_data);
    while (!TCOD_line_step_mt(&x, &y, &bresenham_stack_data)) {
    };
    return x;
  };
  BENCHMARK("TCOD_line") { return TCOD_line(0, 0, 11, 3, null_bresenham_callback); };
}
