#include <libtcod/fov.h>

#include <catch2/catch_all.hpp>
#include <utility>
#include <vector>

// Regression for the view_array_insert off-by-one: it writes one past the
// active_views buffer (sized width*height). The only trigger is a 2x2 map at
// FOV_PERMISSIVE_8; under AddressSanitizer this aborts before the fix.
TEST_CASE("permissive FOV does not overflow active_views on tiny maps", "[fov]") {
  for (int w = 1; w <= 3; ++w)
    for (int h = 1; h <= 3; ++h)
      for (int pov = 0; pov < w * h; ++pov) {
        TCOD_Map* map = TCOD_map_new(w, h);
        TCOD_map_clear(map, false, true);  // every cell opaque
        CHECK(TCOD_map_compute_fov(map, pov % w, pov / w, 0, true, FOV_PERMISSIVE_8) == TCOD_E_OK);
        TCOD_map_delete(map);
      }
}

// Regression for the undersized per-quadrant `bumps` buffer (sized width*height).
// Reproduced on 2x3 / 3x2 maps, which never reach the active-view count that
// triggers bug #1, so this isolates the bumps overflow. ASan aborts pre-fix.
TEST_CASE("permissive FOV does not overflow bumps on tiny maps", "[fov]") {
  const int dims[][2] = {{2, 3}, {3, 2}};
  for (const auto& d : dims) {
    const int n = d[0] * d[1];
    for (int pattern = 0; pattern < (1 << n); ++pattern)
      for (int pov = 0; pov < n; ++pov) {
        if (pattern & (1 << pov)) continue;
        for (int light_walls = 0; light_walls <= 1; ++light_walls) {
          TCOD_Map* map = TCOD_map_new(d[0], d[1]);
          TCOD_map_clear(map, true, true);
          for (int i = 0; i < n; ++i)
            if (pattern & (1 << i)) TCOD_map_set_properties(map, i % d[0], i / d[0], false, true);
          CHECK(TCOD_map_compute_fov(map, pov % d[0], pov / d[0], 0, (bool)light_walls, FOV_PERMISSIVE_8) == TCOD_E_OK);
          TCOD_map_delete(map);
        }
      }
  }
}

// Regression for the undersized obstacle buffer in MRPAS restrictive FOV
// (TCOD_map_compute_fov_restrictive_shadowcasting). Its angle buffers are sized
// `max_obstacles = nbcells / 7`, which floors below the true per-quadrant
// obstacle count on small maps; the scan then writes one obstacle past the end
// of start_angle / end_angle -- always a single 8-byte (one double) overrun.
// Unpatched, AddressSanitizer aborts at fov_restrictive.c compute_quadrant.

// Part 1 -- exhaustive sweep of small maps: every wall pattern x every POV x
// both light_walls settings. Each of these shapes overflows pre-fix (peak
// obstacles = nbcells/7 + 1). Includes non-square shapes (2x6, 3x4) to show the
// bug is not limited to squares.
TEST_CASE("restrictive FOV: small-map obstacle overflow (exhaustive)", "[fov]") {
  const int dims[][2] = {{2, 2}, {2, 3}, {3, 2}, {3, 3}, {2, 6}, {3, 4}, {4, 4}};
  for (const auto& d : dims) {
    const int w = d[0], h = d[1], n = w * h;
    for (long pattern = 0; pattern < (1L << n); ++pattern)
      for (int pov = 0; pov < n; ++pov) {
        if (pattern & (1L << pov)) continue;
        for (int light_walls = 0; light_walls <= 1; ++light_walls) {
          TCOD_Map* map = TCOD_map_new(w, h);
          TCOD_map_clear(map, true, true);
          for (int i = 0; i < n; ++i)
            if (pattern & (1L << i)) TCOD_map_set_properties(map, i % w, i / w, false, true);
          CHECK(TCOD_map_compute_fov(map, pov % w, pov / w, 0, (bool)light_walls, FOV_RESTRICTIVE) == TCOD_E_OK);
          TCOD_map_delete(map);
        }
      }
  }
}

// Part 2 -- the overflow envelope is shape-dependent, not a 5x5 box. The peak
// obstacle count saturates with the map's smaller dimension while nbcells/7
// grows with area, so each width has a band of heights that overflow. Exhaustive
// search is infeasible past ~16 cells (2^25+ patterns), so these are concrete
// adversarial layouts -- the largest overflowing shape per width -- found by a
// peak-obstacle search and verified to overflow the unpatched library. The
// largest is 4x8 (32 cells): a 5th obstacle written into a 4-slot (32-byte)
// buffer. All use light_walls=true and an unbounded radius (0).
namespace {
struct Layout {
  const char* name;
  int w, h, pov_x, pov_y;
  std::vector<std::pair<int, int>> walls;
};
}  // namespace

TEST_CASE("restrictive FOV: larger non-square overflow envelope", "[fov][bug3]") {
  const std::vector<Layout> layouts = {
      {"5x5", 5, 5, 0, 4, {{0, 0}, {3, 0}, {4, 0}, {1, 1}, {2, 1}, {4, 1}, {1, 2}, {4, 2}, {0, 3}, {4, 3}}},
      {"3x8", 3, 8, 2, 0, {{1, 0}, {0, 1}, {0, 2}, {0, 4}, {0, 7}, {1, 7}}},
      {"4x6", 4, 6, 3, 0, {{0, 0}, {0, 1}, {1, 1}, {1, 2}, {0, 5}, {1, 5}, {2, 5}}},
      {"3x9", 3, 9, 2, 7, {{0, 0}, {1, 0}, {0, 3}, {0, 5}, {0, 7}, {1, 7}, {2, 8}}},
      {"4x8", 4, 8, 0, 7, {{1, 0}, {2, 0}, {3, 0}, {3, 2}, {3, 4}, {3, 6}, {2, 7}}},
  };
  for (const auto& L : layouts) {
    CAPTURE(L.name);
    TCOD_Map* map = TCOD_map_new(L.w, L.h);
    TCOD_map_clear(map, true, true);
    for (const auto& wall : L.walls) TCOD_map_set_properties(map, wall.first, wall.second, false, true);
    CHECK(TCOD_map_compute_fov(map, L.pov_x, L.pov_y, 0, true, FOV_RESTRICTIVE) == TCOD_E_OK);
    TCOD_map_delete(map);
  }
}
