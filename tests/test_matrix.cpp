#include <catch2/catch_all.hpp>
#include <libtcod/matrix.hpp>

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
  {
    auto view = tcod::MatrixView<int, 2>{matrix};
    REQUIRE(view[{0, 0}] == 1);
    REQUIRE(view.at({1, 0}) == 2);
    REQUIRE(view.at({0, 1}) == 3);
    REQUIRE(view.at({1, 1}) == 4);
  }
  const auto const_matrix = tcod::Matrix<int, 2>({2, 2}, 7);
  REQUIRE(const_matrix.at({1, 1}) == 7);
  REQUIRE(const_matrix[{1, 1}] == 7);
  {
    auto view = tcod::MatrixView<const int, 2>{const_matrix};
    REQUIRE(view[{1, 1}] == 7);
    REQUIRE(view.at({1, 1}) == 7);
  }
}
