#define CATCH_CONFIG_MAIN

#include <libtcod/fov.h>
#include <libtcod/logging.h>

#include <catch2/catch_all.hpp>
#include <clocale>
#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <utility>

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
