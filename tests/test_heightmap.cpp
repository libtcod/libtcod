#include <catch2/catch_all.hpp>
#include <cfloat>
#include <cstdlib>

#include "libtcod/heightmap.h"
#include "libtcod/heightmap.hpp"

TEST_CASE("TCODHeightmap") {
  auto hm = TCODHeightMap(0, 0);  // Test zero size
  hm = TCODHeightMap(1, 1);  // Test assign new shape
  REQUIRE(hm.getValue(0, 0) == 0);
  hm.setValue(0, 0, 1.0f);  // Should be zeroed
  REQUIRE(hm.getValue(0, 0) == 1.0f);

  auto hm2 = TCODHeightMap(1, 1);
  hm2.setValue(0, 0, 3.0f);
  hm = hm2;  // Test same-size copy
  REQUIRE(hm.getValue(0, 0) == 3.0f);

  hm2 = TCODHeightMap(2, 3);
  hm2.setValue(0, 0, 4.0f);
  hm = std::move(hm2);  // Test move
  REQUIRE(hm.getValue(0, 0) == 4.0f);
  REQUIRE(hm.w == 2);
  REQUIRE(hm.h == 3);

  hm.clear();
  REQUIRE(hm.getValue(0, 0) == 0.0f);
}

TEST_CASE("TCODHeightmap::getMinMax", "[heightmap]") {
  auto empty = TCODHeightMap{0, 0};
  empty.getMinMax(NULL, NULL);

  float min = 0;
  float max = 0;
  empty.getMinMax(&min, &max);
  REQUIRE(min == FLT_MAX);
  REQUIRE(max == -FLT_MAX);

  auto zero_one = TCODHeightMap{2, 1};
  zero_one.setValue(1, 0, 1.0f);
  zero_one.getMinMax(NULL, NULL);
  zero_one.getMinMax(&min, NULL);
  zero_one.getMinMax(NULL, &max);
  REQUIRE(min == 0.0f);
  REQUIRE(max == 1.0f);
}

TEST_CASE("TCOD_heightmap_kernel_transform null handling", "[heightmap][kernel]") {
  // Identity kernel for simple testing
  const int dx[] = {0};
  const int dy[] = {0};
  const float weight[] = {1.0f};

  SECTION("null heightmap does not crash") {
    // Should silently return without crashing
    TCOD_heightmap_kernel_transform(nullptr, 1, dx, dy, weight, -FLT_MAX, FLT_MAX);
  }
}

TEST_CASE("TCOD_heightmap_kernel_transform_out size mismatch", "[heightmap][kernel]") {
  const int dx[] = {0};
  const int dy[] = {0};
  const float weight[] = {1.0f};

  SECTION("mismatched sizes do nothing") {
    TCOD_heightmap_t* src = TCOD_heightmap_new(3, 3);
    TCOD_heightmap_t* dst = TCOD_heightmap_new(4, 4);
    REQUIRE(src != nullptr);
    REQUIRE(dst != nullptr);

    // Set a known value in dst
    TCOD_heightmap_set_value(dst, 0, 0, 99.0f);

    // Should silently return without modifying dst
    TCOD_heightmap_kernel_transform_out(src, dst, 1, dx, dy, weight, nullptr);

    // dst should be unchanged
    REQUIRE(TCOD_heightmap_get_value(dst, 0, 0) == 99.0f);

    TCOD_heightmap_delete(src);
    TCOD_heightmap_delete(dst);
  }

  SECTION("null src does nothing") {
    TCOD_heightmap_t* dst = TCOD_heightmap_new(3, 3);
    REQUIRE(dst != nullptr);
    TCOD_heightmap_set_value(dst, 0, 0, 99.0f);

    TCOD_heightmap_kernel_transform_out(nullptr, dst, 1, dx, dy, weight, nullptr);

    REQUIRE(TCOD_heightmap_get_value(dst, 0, 0) == 99.0f);
    TCOD_heightmap_delete(dst);
  }

  SECTION("null dst does nothing") {
    TCOD_heightmap_t* src = TCOD_heightmap_new(3, 3);
    REQUIRE(src != nullptr);

    // Should not crash
    TCOD_heightmap_kernel_transform_out(src, nullptr, 1, dx, dy, weight, nullptr);

    TCOD_heightmap_delete(src);
  }
}

TEST_CASE("TCOD_heightmap_kernel_transform basic functionality", "[heightmap][kernel]") {
  // 3x3 box blur kernel
  const int dx[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
  const int dy[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
  const float weight[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

  SECTION("in-place transform produces correct convolution") {
    TCOD_heightmap_t* hm = TCOD_heightmap_new(3, 3);
    REQUIRE(hm != nullptr);

    // Set center to 9, all others to 0
    // Expected result at center: (0+0+0+0+9+0+0+0+0)/9 = 1
    TCOD_heightmap_set_value(hm, 1, 1, 9.0f);

    TCOD_heightmap_kernel_transform(hm, 9, dx, dy, weight, -FLT_MAX, FLT_MAX);

    REQUIRE(TCOD_heightmap_get_value(hm, 1, 1) == Catch::Approx(1.0f));

    TCOD_heightmap_delete(hm);
  }

  SECTION("out-of-place transform produces correct convolution") {
    TCOD_heightmap_t* src = TCOD_heightmap_new(3, 3);
    TCOD_heightmap_t* dst = TCOD_heightmap_new(3, 3);
    REQUIRE(src != nullptr);
    REQUIRE(dst != nullptr);

    TCOD_heightmap_set_value(src, 1, 1, 9.0f);

    // NULL mask = transform all cells
    TCOD_heightmap_kernel_transform_out(src, dst, 9, dx, dy, weight, nullptr);

    // Source unchanged
    REQUIRE(TCOD_heightmap_get_value(src, 1, 1) == 9.0f);
    // Destination has convolved value
    REQUIRE(TCOD_heightmap_get_value(dst, 1, 1) == Catch::Approx(1.0f));

    TCOD_heightmap_delete(src);
    TCOD_heightmap_delete(dst);
  }
}

TEST_CASE("TCOD_heightmap_threshold_mask", "[heightmap][mask]") {
  SECTION("generates correct mask from threshold range") {
    TCOD_heightmap_t* hm = TCOD_heightmap_new(3, 3);
    REQUIRE(hm != nullptr);

    // Set values 0-8
    for (int y = 0; y < 3; y++) {
      for (int x = 0; x < 3; x++) {
        TCOD_heightmap_set_value(hm, x, y, (float)(x + y * 3));
      }
    }

    uint8_t* mask = (uint8_t*)malloc(9);
    REQUIRE(mask != nullptr);

    // Threshold for values [3, 5]
    TCOD_heightmap_threshold_mask(hm, mask, 3.0f, 5.0f);

    // Check mask: positions with values 3, 4, 5 should be 1, others 0
    REQUIRE(mask[0] == 0);  // value 0
    REQUIRE(mask[1] == 0);  // value 1
    REQUIRE(mask[2] == 0);  // value 2
    REQUIRE(mask[3] == 1);  // value 3
    REQUIRE(mask[4] == 1);  // value 4
    REQUIRE(mask[5] == 1);  // value 5
    REQUIRE(mask[6] == 0);  // value 6
    REQUIRE(mask[7] == 0);  // value 7
    REQUIRE(mask[8] == 0);  // value 8

    free(mask);
    TCOD_heightmap_delete(hm);
  }

  SECTION("null heightmap does not crash") {
    uint8_t mask[9] = {0};
    TCOD_heightmap_threshold_mask(nullptr, mask, 0.0f, 1.0f);
    // mask should be unchanged
    for (int i = 0; i < 9; i++) {
      REQUIRE(mask[i] == 0);
    }
  }

  SECTION("null mask does not crash") {
    TCOD_heightmap_t* hm = TCOD_heightmap_new(3, 3);
    REQUIRE(hm != nullptr);
    TCOD_heightmap_threshold_mask(hm, nullptr, 0.0f, 1.0f);
    TCOD_heightmap_delete(hm);
  }
}

TEST_CASE("TCOD_heightmap_kernel_transform minLevel/maxLevel filtering", "[heightmap][kernel]") {
  // 3x3 blur for actual transformation
  const int blur_dx[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
  const int blur_dy[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
  const float blur_weight[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

  SECTION("values outside range are unchanged in-place") {
    TCOD_heightmap_t* hm = TCOD_heightmap_new(3, 3);
    REQUIRE(hm != nullptr);

    // Set all cells
    for (int y = 0; y < 3; y++) {
      for (int x = 0; x < 3; x++) {
        TCOD_heightmap_set_value(hm, x, y, (float)(x + y * 3));  // 0-8
      }
    }

    // Only transform values in range [3, 5]
    TCOD_heightmap_kernel_transform(hm, 9, blur_dx, blur_dy, blur_weight, 3.0f, 5.0f);

    // Values 0, 1, 2, 6, 7, 8 should be unchanged
    REQUIRE(TCOD_heightmap_get_value(hm, 0, 0) == 0.0f);  // Was 0
    REQUIRE(TCOD_heightmap_get_value(hm, 1, 0) == 1.0f);  // Was 1
    REQUIRE(TCOD_heightmap_get_value(hm, 2, 0) == 2.0f);  // Was 2
    REQUIRE(TCOD_heightmap_get_value(hm, 0, 2) == 6.0f);  // Was 6
    REQUIRE(TCOD_heightmap_get_value(hm, 1, 2) == 7.0f);  // Was 7
    REQUIRE(TCOD_heightmap_get_value(hm, 2, 2) == 8.0f);  // Was 8

    // Values 3, 4, 5 should be transformed (blurred)
    // Position (0,1) was 3, now blurred with neighbors
    // Position (1,1) was 4, blur of 0-8 = 36/9 = 4 (coincidentally same)
    // Position (2,1) was 5, now blurred with neighbors
    // At least the edge values should change since they don't average to themselves
    REQUIRE(TCOD_heightmap_get_value(hm, 0, 1) != 3.0f);
    REQUIRE(TCOD_heightmap_get_value(hm, 2, 1) != 5.0f);

    TCOD_heightmap_delete(hm);
  }

  SECTION("mask controls which cells are transformed in _out variant") {
    TCOD_heightmap_t* src = TCOD_heightmap_new(3, 3);
    TCOD_heightmap_t* dst = TCOD_heightmap_new(3, 3);
    REQUIRE(src != nullptr);
    REQUIRE(dst != nullptr);

    // Set source values
    for (int y = 0; y < 3; y++) {
      for (int x = 0; x < 3; x++) {
        TCOD_heightmap_set_value(src, x, y, (float)(x + y * 3));
      }
    }

    // Create mask for values [3, 5]
    uint8_t* mask = (uint8_t*)malloc(9);
    REQUIRE(mask != nullptr);
    TCOD_heightmap_threshold_mask(src, mask, 3.0f, 5.0f);

    TCOD_heightmap_kernel_transform_out(src, dst, 9, blur_dx, blur_dy, blur_weight, mask);

    // Values outside range should be unmodified by the transform
    REQUIRE(TCOD_heightmap_get_value(dst, 0, 0) == 0.0f);
    REQUIRE(TCOD_heightmap_get_value(dst, 1, 0) == 0.0f);
    REQUIRE(TCOD_heightmap_get_value(dst, 2, 0) == 0.0f);
    REQUIRE(TCOD_heightmap_get_value(dst, 0, 2) == 0.0f);
    REQUIRE(TCOD_heightmap_get_value(dst, 1, 2) == 0.0f);
    REQUIRE(TCOD_heightmap_get_value(dst, 2, 2) == 0.0f);

    // Source unchanged
    REQUIRE(TCOD_heightmap_get_value(src, 0, 0) == 0.0f);
    REQUIRE(TCOD_heightmap_get_value(src, 1, 1) == 4.0f);

    free(mask);
    TCOD_heightmap_delete(src);
    TCOD_heightmap_delete(dst);
  }
}

TEST_CASE("TCOD_heightmap_kernel_transform edge handling", "[heightmap][kernel]") {
  // Kernel that extends beyond edges
  const int dx[] = {-1, 0, 1};
  const int dy[] = {0, 0, 0};
  const float weight[] = {1, 2, 1};

  SECTION("edge cells only use in-bounds neighbors") {
    TCOD_heightmap_t* hm = TCOD_heightmap_new(3, 1);
    REQUIRE(hm != nullptr);

    // Set values: [1, 2, 3]
    TCOD_heightmap_set_value(hm, 0, 0, 1.0f);
    TCOD_heightmap_set_value(hm, 1, 0, 2.0f);
    TCOD_heightmap_set_value(hm, 2, 0, 3.0f);

    TCOD_heightmap_kernel_transform(hm, 3, dx, dy, weight, -FLT_MAX, FLT_MAX);

    // Left edge: only center (weight 2) and right (weight 1) are in bounds
    // Result: (1*2 + 2*1) / (2+1) = 4/3
    REQUIRE(TCOD_heightmap_get_value(hm, 0, 0) == Catch::Approx(4.0f / 3.0f));

    // Center: all in bounds
    // Result: (1*1 + 2*2 + 3*1) / (1+2+1) = 8/4 = 2
    REQUIRE(TCOD_heightmap_get_value(hm, 1, 0) == Catch::Approx(2.0f));

    // Right edge: only left (weight 1) and center (weight 2) are in bounds
    // Result: (2*1 + 3*2) / (1+2) = 8/3
    REQUIRE(TCOD_heightmap_get_value(hm, 2, 0) == Catch::Approx(8.0f / 3.0f));

    TCOD_heightmap_delete(hm);
  }
}
