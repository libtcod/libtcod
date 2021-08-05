#include <libtcod/matrix.h>

#include <catch2/catch.hpp>

TEST_CASE("Matrix basics") {
  auto matrix = tcod::Matrix<int, 2>({2, 2});
  REQUIRE(matrix.get_shape() == std::array<int, 2>{2, 2});
  REQUIRE(matrix.at({0, 0}) == 0);  // Matrix is zero-initialized.
  REQUIRE(matrix.in_bounds({0, 0}));
  matrix[{0, 0}] = 1;
  matrix.at({1, 0}) = 2;
  matrix.at({0, 1}) = 3;
  matrix.at({1, 1}) = 4;
  REQUIRE(matrix.get_container() == std::vector<int>{1, 2, 3, 4});
}
