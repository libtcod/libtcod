#include <algorithm>
#include <array>
#include <catch2/catch_all.hpp>
#include <cmath>

#include "libtcod/heightmap.hpp"
#include "libtcod/mersenne.hpp"

TEST_CASE("Voronoi terrain includes distant sites", "[heightmap][voronoi]") {
  const int width = GENERATE(20001, 100001);
  auto expected_rng = TCODRandom(42U, TCOD_RNG_MT);
  const int site_x = expected_rng.getInt(0, width - 1);
  const int site_y = expected_rng.getInt(0, 1);
  auto rng = TCODRandom(42U, TCOD_RNG_MT);
  auto hm = TCODHeightMap(width, 2);
  const float coef[] = {1e-8f};
  hm.addVoronoi(1, 1, coef, &rng);
  for (int y = 0; y < hm.h; ++y) {
    for (int x = 0; x < hm.w; ++x) {
      const double distance = std::hypot(double(x - site_x), double(y - site_y));
      const double expected = double(coef[0]) * distance * distance;
      REQUIRE(hm.getValue(x, y) == Catch::Approx(expected).epsilon(2e-6).margin(1e-7));
    }
  }
}

TEST_CASE("Voronoi terrain ranks distant sites", "[heightmap][voronoi]") {
  constexpr int width = 30001;
  auto expected_rng = TCODRandom(1U, TCOD_RNG_MT);
  std::array<std::array<int, 2>, 3> sites{};
  for (auto& site : sites) {
    site[0] = expected_rng.getInt(0, width - 1);
    site[1] = expected_rng.getInt(0, 1);
  }
  auto rng = TCODRandom(1U, TCOD_RNG_MT);
  auto hm = TCODHeightMap(width, 2);
  const float coef[] = {1e-8f, -5e-9f, 2.5e-9f};
  hm.addVoronoi(3, 3, coef, &rng);
  for (int x : {0, width / 2, width - 1}) {
    std::array<double, 3> distances{};
    for (std::size_t i = 0; i < sites.size(); ++i) {
      const double distance = std::hypot(double(x - sites[i][0]), double(sites[i][1]));
      distances[i] = distance * distance;
    }
    std::sort(distances.begin(), distances.end());
    double expected = 0;
    for (std::size_t i = 0; i < distances.size(); ++i) expected += double(coef[i]) * distances[i];
    REQUIRE(hm.getValue(x, 0) == Catch::Approx(expected).epsilon(2e-6).margin(1e-7));
  }
}
