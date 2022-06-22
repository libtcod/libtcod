#define CATCH_CONFIG_MAIN

#include <libtcod/logging.h>

#include <catch2/catch_all.hpp>
#include <clocale>
#include <cstddef>
#include <iostream>
#include <libtcod.hpp>
#include <libtcod/tileset_fallback.hpp>
#include <limits>
#include <random>
#include <string>
#include <utility>

#include "common.hpp"

/// Captures libtcod log output on tests.
struct HandleLogging : Catch::EventListenerBase {
  using EventListenerBase::EventListenerBase;  // inherit constructor
  /// Register logger before each test.
  void testCaseStarting(Catch::TestCaseInfo const&) override {
    TCOD_set_log_level(TCOD_LOG_DEBUG);
    TCOD_set_log_callback(&catch_log, nullptr);
  }
  static void catch_log(const TCOD_LogMessage* message, void*) {
    INFO("libtcod:" << message->source << ":" << message->lineno << ":" << message->level << ":" << message->message);
  }
};
CATCH_REGISTER_LISTENER(HandleLogging)

std::ostream& operator<<(std::ostream& out, const std::array<ptrdiff_t, 2>& data) {
  return out << '{' << data.at(0) << ',' << ' ' << data.at(1) << '}';
}

TEST_CASE("Pathfinder Benchmarks", "[.benchmark]") {
  const int SIZE = 50;
  BENCHMARK("Classic libtcod A* 50x50") {
    TCOD_Map* map = TCOD_map_new(SIZE, SIZE);
    TCOD_map_clear(map, 1, 1);
    TCOD_Path* astar = TCOD_path_new_using_map(map, 1.0f);
    TCOD_path_compute(astar, 0, 0, SIZE - 1, SIZE - 1);
    TCOD_path_delete(astar);
    TCOD_map_delete(map);
  };
}

TEST_CASE("Fallback font.", "[!mayfail]") { REQUIRE(tcod::tileset::new_fallback_tileset()); }

TEST_CASE("Heap test.") {
  struct TCOD_Heap heap;
  TCOD_heap_init(&heap, sizeof(int));
  std::vector<int> INPUT{0, 1, 2, 3, 4, 5, 6, 7};
  for (int& it : INPUT) {
    TCOD_minheap_push(&heap, it, &it);
  }
  std::vector<int> output;
  while (heap.size) {
    int out;
    TCOD_minheap_pop(&heap, &out);
    output.emplace_back(out);
  }
  REQUIRE(INPUT == output);
  TCOD_heap_uninit(&heap);
}

TEST_CASE("Noise Benchmarks", "[.benchmark]") {
  TCOD_Random* rng = TCOD_random_new_from_seed(TCOD_RNG_MT, 0);
  TCOD_Noise* noise1d = TCOD_noise_new(1, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, rng);
  TCOD_Noise* noise2d = TCOD_noise_new(2, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, rng);
  TCOD_Noise* noise3d = TCOD_noise_new(3, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, rng);
  TCOD_Noise* noise4d = TCOD_noise_new(4, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, rng);
  const float POINT[4] = {0.5f, 0.5f, 0.5f, 0.5f};
  (void)!TCOD_noise_get_ex(noise1d, POINT, TCOD_NOISE_WAVELET);  // Pre-generate wavelet data.
  (void)!TCOD_noise_get_ex(noise2d, POINT, TCOD_NOISE_WAVELET);
  (void)!TCOD_noise_get_ex(noise3d, POINT, TCOD_NOISE_WAVELET);
  BENCHMARK("Perlin 1D") { (void)!TCOD_noise_get_ex(noise1d, POINT, TCOD_NOISE_PERLIN); };
  BENCHMARK("Perlin 2D") { (void)!TCOD_noise_get_ex(noise2d, POINT, TCOD_NOISE_PERLIN); };
  BENCHMARK("Perlin 3D") { (void)!TCOD_noise_get_ex(noise3d, POINT, TCOD_NOISE_PERLIN); };
  BENCHMARK("Perlin 4D") { (void)!TCOD_noise_get_ex(noise4d, POINT, TCOD_NOISE_PERLIN); };
  BENCHMARK("Simplex 1D") { (void)!TCOD_noise_get_ex(noise1d, POINT, TCOD_NOISE_SIMPLEX); };
  BENCHMARK("Simplex 2D") { (void)!TCOD_noise_get_ex(noise2d, POINT, TCOD_NOISE_SIMPLEX); };
  BENCHMARK("Simplex 3D") { (void)!TCOD_noise_get_ex(noise3d, POINT, TCOD_NOISE_SIMPLEX); };
  BENCHMARK("Simplex 4D") { (void)!TCOD_noise_get_ex(noise4d, POINT, TCOD_NOISE_SIMPLEX); };
  BENCHMARK("Wavelet 1D") { (void)!TCOD_noise_get_ex(noise1d, POINT, TCOD_NOISE_WAVELET); };
  BENCHMARK("Wavelet 2D") { (void)!TCOD_noise_get_ex(noise2d, POINT, TCOD_NOISE_WAVELET); };
  BENCHMARK("Wavelet 3D") { (void)!TCOD_noise_get_ex(noise3d, POINT, TCOD_NOISE_WAVELET); };

  BENCHMARK("Simplex fbM 1D octaves=4") { (void)!TCOD_noise_get_fbm_ex(noise1d, POINT, 4, TCOD_NOISE_SIMPLEX); };
  BENCHMARK("Simplex fbM 2D octaves=4") { (void)!TCOD_noise_get_fbm_ex(noise2d, POINT, 4, TCOD_NOISE_SIMPLEX); };
  BENCHMARK("Simplex fbM 3D octaves=4") { (void)!TCOD_noise_get_fbm_ex(noise3d, POINT, 4, TCOD_NOISE_SIMPLEX); };
  BENCHMARK("Simplex fbM 4D octaves=4") { (void)!TCOD_noise_get_fbm_ex(noise4d, POINT, 4, TCOD_NOISE_SIMPLEX); };
  BENCHMARK("Simplex turbulence 1D octaves=4") {
    (void)!TCOD_noise_get_turbulence_ex(noise1d, POINT, 4, TCOD_NOISE_SIMPLEX);
  };
  BENCHMARK("Simplex turbulence 2D octaves=4") {
    (void)!TCOD_noise_get_turbulence_ex(noise2d, POINT, 4, TCOD_NOISE_SIMPLEX);
  };
  BENCHMARK("Simplex turbulence 3D octaves=4") {
    (void)!TCOD_noise_get_turbulence_ex(noise3d, POINT, 4, TCOD_NOISE_SIMPLEX);
  };
  BENCHMARK("Simplex turbulence 4D octaves=4") {
    (void)!TCOD_noise_get_turbulence_ex(noise4d, POINT, 4, TCOD_NOISE_SIMPLEX);
  };

  TCOD_noise_delete(noise4d);
  TCOD_noise_delete(noise3d);
  TCOD_noise_delete(noise2d);
  TCOD_noise_delete(noise1d);
  TCOD_random_delete(rng);
}
TEST_CASE("Noise Vectorized Benchmarks", "[.benchmark]") {
  TCOD_Random* rng = TCOD_random_new_from_seed(TCOD_RNG_MT, 0);
  TCOD_Noise* noise1d = TCOD_noise_new(1, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, rng);
  TCOD_Noise* noise2d = TCOD_noise_new(2, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, rng);
  TCOD_Noise* noise3d = TCOD_noise_new(3, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, rng);
  TCOD_Noise* noise4d = TCOD_noise_new(4, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, rng);
  constexpr int ARRAY_SIZE = 1024;
  {
    const float POINT[4] = {0.5f, 0.5f, 0.5f, 0.5f};
    (void)!TCOD_noise_get_ex(noise1d, POINT, TCOD_NOISE_WAVELET);  // Pre-generate wavelet data.
    (void)!TCOD_noise_get_ex(noise2d, POINT, TCOD_NOISE_WAVELET);
    (void)!TCOD_noise_get_ex(noise3d, POINT, TCOD_NOISE_WAVELET);
  }
  float x[ARRAY_SIZE];
  float y[ARRAY_SIZE];
  float z[ARRAY_SIZE];
  float w[ARRAY_SIZE];
  float out[ARRAY_SIZE];
  BENCHMARK("Perlin 2Dx256") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[2] = {i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_ex(noise2d, point, TCOD_NOISE_PERLIN);
    }
  };
  BENCHMARK("Perlin 2Dx256 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = i * 0.1f;
    }
    TCOD_noise_get_vectorized(noise2d, TCOD_NOISE_PERLIN, ARRAY_SIZE, x, y, NULL, NULL, out);
  };
  BENCHMARK("Perlin 3Dx256") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[3] = {i * 0.1f, i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_ex(noise3d, point, TCOD_NOISE_PERLIN);
    }
  };
  BENCHMARK("Perlin 3Dx256 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = z[i] = i * 0.1f;
    }
    TCOD_noise_get_vectorized(noise3d, TCOD_NOISE_PERLIN, ARRAY_SIZE, x, y, z, NULL, out);
  };
  BENCHMARK("Perlin 4Dx256") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[4] = {i * 0.1f, i * 0.1f, i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_ex(noise4d, point, TCOD_NOISE_PERLIN);
    }
  };
  BENCHMARK("Perlin 4Dx256 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = z[i] = w[i] = i * 0.1f;
    }
    TCOD_noise_get_vectorized(noise4d, TCOD_NOISE_PERLIN, ARRAY_SIZE, x, y, z, w, out);
  };
  BENCHMARK("Simplex 2Dx256") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[2] = {i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_ex(noise2d, point, TCOD_NOISE_SIMPLEX);
    }
  };
  BENCHMARK("Simplex 2Dx256 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = i * 0.1f;
    }
    TCOD_noise_get_vectorized(noise2d, TCOD_NOISE_SIMPLEX, ARRAY_SIZE, x, y, NULL, NULL, out);
  };
  BENCHMARK("Simplex 3Dx256") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[3] = {i * 0.1f, i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_ex(noise3d, point, TCOD_NOISE_SIMPLEX);
    }
  };
  BENCHMARK("Simplex 3Dx256 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = z[i] = i * 0.1f;
    }
    TCOD_noise_get_vectorized(noise3d, TCOD_NOISE_SIMPLEX, ARRAY_SIZE, x, y, z, NULL, out);
  };
  BENCHMARK("Simplex 4Dx256") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[4] = {i * 0.1f, i * 0.1f, i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_ex(noise4d, point, TCOD_NOISE_SIMPLEX);
    }
  };
  BENCHMARK("Simplex 4Dx256 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = z[i] = w[i] = i * 0.1f;
    }
    TCOD_noise_get_vectorized(noise4d, TCOD_NOISE_SIMPLEX, ARRAY_SIZE, x, y, z, w, out);
  };
  BENCHMARK("Wavelet 2Dx256") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[2] = {i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_ex(noise2d, point, TCOD_NOISE_WAVELET);
    }
  };
  BENCHMARK("Wavelet 2Dx256 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = i * 0.1f;
    }
    TCOD_noise_get_vectorized(noise2d, TCOD_NOISE_WAVELET, ARRAY_SIZE, x, y, NULL, NULL, out);
  };
  BENCHMARK("Wavelet 3Dx256") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[3] = {i * 0.1f, i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_ex(noise3d, point, TCOD_NOISE_WAVELET);
    }
  };
  BENCHMARK("Wavelet 3Dx256 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = z[i] = i * 0.1f;
    }
    TCOD_noise_get_vectorized(noise3d, TCOD_NOISE_WAVELET, ARRAY_SIZE, x, y, z, NULL, out);
  };
  BENCHMARK("Perlin fBm 2Dx256 octaves=4") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[2] = {i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_fbm_ex(noise2d, point, 4, TCOD_NOISE_PERLIN);
    }
  };
  BENCHMARK("Perlin fBm 2Dx256 octaves=4 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = i * 0.1f;
    }
    TCOD_noise_get_fbm_vectorized(noise2d, TCOD_NOISE_PERLIN, 4, ARRAY_SIZE, x, y, NULL, NULL, out);
  };
  BENCHMARK("Perlin turbulence 2Dx256 octaves=4") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[2] = {i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_turbulence_ex(noise2d, point, 4, TCOD_NOISE_PERLIN);
    }
  };
  BENCHMARK("Perlin turbulence 2Dx256 octaves=4 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = i * 0.1f;
    }
    TCOD_noise_get_turbulence_vectorized(noise2d, TCOD_NOISE_PERLIN, 4, ARRAY_SIZE, x, y, NULL, NULL, out);
  };
  TCOD_noise_delete(noise4d);
  TCOD_noise_delete(noise3d);
  TCOD_noise_delete(noise2d);
  TCOD_noise_delete(noise1d);
  TCOD_random_delete(rng);
}

static tcod::MapPtr_ new_map_with_radius(int radius, bool start_transparent) {
  int size = radius * 2 + 1;
  tcod::MapPtr_ map{TCOD_map_new(size, size)};
  TCOD_map_clear(map.get(), start_transparent, 0);
  return map;
}
static tcod::MapPtr_ new_empty_map(int radius) { return new_map_with_radius(radius, true); }
static tcod::MapPtr_ new_opaque_map(int radius) { return new_map_with_radius(radius, false); }
static tcod::MapPtr_ new_corridor_map(int radius) {
  tcod::MapPtr_ map{new_map_with_radius(radius, false)};
  for (int i = 0; i < radius * 2 + 1; ++i) {
    TCOD_map_set_properties(map.get(), radius, i, true, true);
    TCOD_map_set_properties(map.get(), i, radius, true, true);
  }
  return map;
}
static tcod::MapPtr_ new_forest_map(int radius) {
  // Forest map with 1 in 4 chance of a blocking tile.
  std::mt19937 rng(0);
  std::uniform_int_distribution<int> chance(0, 3);
  tcod::MapPtr_ map{new_map_with_radius(radius, true)};
  for (int i = 0; i < map->nbcells; ++i) {
    if (chance(rng) == 0) {
      map->cells[i].transparent = false;
    }
  }
  return map;
}
TEST_CASE("FOV Benchmarks", "[.benchmark]") {
  std::array<std::tuple<std::string, tcod::MapPtr_>, 16> test_maps{{
      {"empty_r4", new_empty_map(4)},
      {"empty_r10", new_empty_map(10)},
      {"empty_r50", new_empty_map(50)},
      {"empty_r300", new_empty_map(300)},
      {"opaque_r4", new_opaque_map(4)},
      {"opaque_r10", new_opaque_map(10)},
      {"opaque_r50", new_opaque_map(50)},
      {"opaque_r300", new_opaque_map(300)},
      {"corridor_r4", new_corridor_map(4)},
      {"corridor_r10", new_corridor_map(10)},
      {"corridor_r50", new_corridor_map(50)},
      {"corridor_r300", new_corridor_map(300)},
      {"forest_r4", new_forest_map(4)},
      {"forest_r10", new_forest_map(10)},
      {"forest_r50", new_forest_map(50)},
      {"forest_r300", new_forest_map(300)},
  }};
  for (auto& active_test : test_maps) {
    const std::string& map_name = std::get<0>(active_test);
    TCOD_Map* map = std::get<1>(active_test).get();
    const int radius = TCOD_map_get_width(map) / 2;
    BENCHMARK(map_name + " FOV_BASIC") { (void)!TCOD_map_compute_fov(map, radius, radius, 0, true, FOV_BASIC); };
    BENCHMARK(map_name + " FOV_DIAMOND") { (void)!TCOD_map_compute_fov(map, radius, radius, 0, true, FOV_DIAMOND); };
    BENCHMARK(map_name + " FOV_SHADOW") { (void)!TCOD_map_compute_fov(map, radius, radius, 0, true, FOV_SHADOW); };
    BENCHMARK(map_name + " FOV_RESTRICTIVE") {
      (void)!TCOD_map_compute_fov(map, radius, radius, 0, true, FOV_RESTRICTIVE);
    };
    BENCHMARK(map_name + " FOV_PERMISSIVE_8") {
      (void)!TCOD_map_compute_fov(map, radius, radius, 0, true, FOV_PERMISSIVE_8);
    };
    BENCHMARK(map_name + " FOV_SYMMETRIC_SHADOWCAST") {
      (void)!TCOD_map_compute_fov(map, radius, radius, 0, true, FOV_SYMMETRIC_SHADOWCAST);
    };
  }
}
