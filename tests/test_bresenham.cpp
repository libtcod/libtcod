
#include <libtcod/error.h>

#include <algorithm>
#include <array>
#include <catch2/catch_all.hpp>
#include <climits>
#include <libtcod/bresenham.hpp>
#include <vector>

extern "C" int test_bresenham_c(void);

TEST_CASE("Buffered Bresenham output links from a C translation unit.", "[bresenham]") {
  CHECK(test_bresenham_c() == 0);
}

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

TEST_CASE("Buffered Bresenham output preserves coordinates and buffer bounds.", "[bresenham]") {
  const auto origin = GENERATE(std::array<int, 2>{0, 0}, std::array<int, 2>{17, -23});
  const auto delta_x = GENERATE(-11, -2, -1, 0, 1, 2, 11);
  const auto delta_y = GENERATE(-11, -2, -1, 0, 1, 2, 11);
  const std::array<int, 2> dest{{origin[0] + delta_x, origin[1] + delta_y}};
  const auto expected = generate_line(origin, dest);
  const auto length = static_cast<int>(expected.size());
  REQUIRE(TCOD_bresenham(origin[0], origin[1], dest[0], dest[1], 0, nullptr) == length);
  CHECK(TCOD_bresenham(origin[0], origin[1], dest[0], dest[1], length, nullptr) == length);
  for (int capacity : {0, 1, 2, length - 1, length, length + 3}) {
    CAPTURE(origin, dest, capacity);
    const int canary = -777;
    std::vector<int> out(2 * (length + 3) + 2, canary);
    REQUIRE(TCOD_bresenham(origin[0], origin[1], dest[0], dest[1], capacity, out.data() + 1) == length);
    const int written = std::min(capacity, length);
    CHECK(out.front() == canary);
    for (int i = 0; i < written; ++i) {
      CHECK(out[1 + i * 2] == expected[i][0]);
      CHECK(out[2 + i * 2] == expected[i][1]);
    }
    for (size_t i = 1 + written * 2; i < out.size(); ++i) CHECK(out[i] == canary);
  }
}

TEST_CASE("Buffered Bresenham output supports translated integer boundaries.", "[bresenham]") {
  const int origin = GENERATE(INT_MIN, INT_MAX - 3);
  const auto expected = generate_line({origin, origin}, {origin + 3, origin + 1});
  std::array<int, 8> out{};
  REQUIRE(TCOD_bresenham(origin, origin, origin + 3, origin + 1, 4, out.data()) == 4);
  for (int i = 0; i < 4; ++i) {
    CHECK(out[i * 2] == expected[i][0]);
    CHECK(out[i * 2 + 1] == expected[i][1]);
  }
}

TEST_CASE("Buffered Bresenham queries and clipping do not traverse the entire line.", "[bresenham]") {
  REQUIRE(TCOD_bresenham(INT_MIN, INT_MAX, -2, 1, 0, nullptr) == INT_MAX);
  std::array<int, 8> out{};
  out.fill(-777);
  REQUIRE(TCOD_bresenham(INT_MIN, INT_MAX, -2, 1, 3, out.data() + 1) == INT_MAX);
  for (int i = 0; i < 3; ++i) {
    CHECK(out[1 + i * 2] == INT_MIN + i);
    CHECK(out[2 + i * 2] == INT_MAX - i);
  }
  CHECK(out.front() == -777);
  CHECK(out.back() == -777);
}

TEST_CASE("Buffered Bresenham output handles a wide non-diagonal slope.", "[bresenham]") {
  const std::array<int, 8> expected{{0, 0, 1, 0, 2, 1, 3, 1}};
  std::array<int, 8> out{};
  REQUIRE(TCOD_bresenham(0, 0, INT_MAX - 1, (INT_MAX - 1) / 2, 4, out.data()) == INT_MAX);
  CHECK(out == expected);
  REQUIRE(TCOD_bresenham(INT_MAX - 1, (INT_MAX - 1) / 2, 0, 0, 4, out.data()) == INT_MAX);
  for (int i = 0; i < 4; ++i) {
    CHECK(out[i * 2] == INT_MAX - 1 - expected[i * 2]);
    CHECK(out[i * 2 + 1] == (INT_MAX - 1) / 2 - expected[i * 2 + 1]);
  }
}

TEST_CASE("Invalid Bresenham output requests do not write the buffer.", "[bresenham]") {
  std::array<int, 4> out{};
  out.fill(-777);
  const auto unchanged = out;
  SECTION("Negative capacity.") {
    TCOD_clear_error();
    CHECK(TCOD_bresenham(0, 0, 3, 1, -1, out.data()) == TCOD_E_INVALID_ARGUMENT);
    CHECK(out == unchanged);
    CHECK(TCOD_get_error()[0] != '\0');
    TCOD_clear_error();
    CHECK(TCOD_bresenham(0, 0, 3, 1, -1, nullptr) == TCOD_E_INVALID_ARGUMENT);
    CHECK(TCOD_get_error()[0] != '\0');
  }
  SECTION("Unrepresentable length.") {
    const auto origin = GENERATE(
        std::array<int, 2>{INT_MIN, 0},
        std::array<int, 2>{INT_MAX, 0},
        std::array<int, 2>{0, INT_MIN},
        std::array<int, 2>{0, 0});
    const std::array<int, 2> dest{{INT_MAX, INT_MAX}};
    TCOD_clear_error();
    CHECK(TCOD_bresenham(origin[0], origin[1], dest[0], dest[1], 1, out.data()) == TCOD_E_INVALID_ARGUMENT);
    CHECK(out == unchanged);
    CHECK(TCOD_get_error()[0] != '\0');
    TCOD_clear_error();
    CHECK(TCOD_bresenham(origin[0], origin[1], dest[0], dest[1], 0, nullptr) == TCOD_E_INVALID_ARGUMENT);
    CHECK(TCOD_get_error()[0] != '\0');
  }
}

TEST_CASE("bresenham benchmarks", "[.benchmark]") {
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
  BENCHMARK("BresenhamLine (iterate)") {
    std::array<int, 2> out{0, 0};
    for (auto&& xy : tcod::BresenhamLine({0, 0}, {11, 3})) {
      out = xy;
    }
    return out;
  };
  BENCHMARK("BresenhamLine (to vector)") {
    tcod::BresenhamLine it{{0, 0}, {11, 3}};
    return std::vector<std::array<int, 2>>{it.begin(), it.end()};
  };
}

TEST_CASE("BresenhamLine") {
  using Point2_Vector = std::vector<tcod::BresenhamLine::Point2>;
  const auto dest_x = GENERATE(-11, -3, 0, 3, 11);
  const auto dest_y = GENERATE(-11, -3, 0, 3, 11);
  const auto EXPECTED = generate_line({0, 0}, {dest_x, dest_y});
  SECTION("BresenhamLine compares equal to the original algorithm.") {
    const tcod::BresenhamLine bresenham_iterator{{0, 0}, {dest_x, dest_y}};
    const Point2_Vector line{bresenham_iterator.begin(), bresenham_iterator.end()};
    REQUIRE(line == EXPECTED);
  }
  SECTION("BresenhamLine endpoints can be clipped.") {
    const auto EXPECTED_CLIPPED =
        Point2_Vector{EXPECTED.begin() + 1, std::max(EXPECTED.begin() + 1, EXPECTED.end() - 1)};
    const auto bresenham_iterator = tcod::BresenhamLine({0, 0}, {dest_x, dest_y}).adjust_range(1, -1);
    const Point2_Vector line{bresenham_iterator.begin(), bresenham_iterator.end()};
    REQUIRE(line == EXPECTED_CLIPPED);
  }
}
