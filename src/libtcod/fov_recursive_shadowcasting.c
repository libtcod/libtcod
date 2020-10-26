/* BSD 3-Clause License
 *
 * Copyright © 2008-2020, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "fov.h"
#include "libtcod_int.h"
#include "utility.h"
/**
    Quadrant matrixes.
 */
static const int matrix_table[4][8] = {
    {1, 0, 0, -1, -1, 0, 0, 1},
    {0, 1, -1, 0, 0, -1, 1, 0},
    {0, 1, 1, 0, 0, -1, -1, 0},
    {1, 0, 0, 1, -1, 0, 0, -1},
};
/**
    Cast visiblity using shadowcasting.
 */
static void cast_light(
    struct TCOD_Map* __restrict map,
    int pov_x,
    int pov_y,
    int distance,      // Polar distance from POV.
    float slope_high,  // Main slope for this call to check.
    float slope_low,
    int max_radius,
    int octant,
    bool light_walls) {
  const int xx = matrix_table[0][octant];
  const int xy = matrix_table[1][octant];
  const int yx = matrix_table[2][octant];
  const int yy = matrix_table[3][octant];
  const int radius_squared = max_radius * max_radius;
  float new_high_slope = 0.0f;
  if (slope_high < slope_low) {
    return;
  }
  for (; distance < max_radius + 1; ++distance) {
    bool blocked = false;
    for (int angle = distance; angle >= 0; --angle) {  // Polar angle coordinates from top to bottom.
      const float tile_slope_high = (angle + 0.5f) / (distance - 0.5f);
      const float tile_slope_low = (angle - 0.5f) / (distance + 0.5f);
      if (tile_slope_low > slope_high) {
        continue;
      } else if (tile_slope_high < slope_low) {
        break;
      }
      const int map_x = pov_x + angle * xx + distance * xy;
      const int map_y = pov_y + angle * yx + distance * yy;
      if (!TCOD_map_in_bounds(map, map_x, map_y)) {
        continue;  // Distance or angle is out-of-bounds.
      }
      const int map_index = map_x + map_y * map->width;
      if (angle * angle + distance * distance <= radius_squared && (light_walls || map->cells[map_index].transparent)) {
        map->cells[map_index].fov = 1;
      }
      if (blocked) {
        if (!map->cells[map_index].transparent) {
          new_high_slope = tile_slope_low;
          continue;
        } else {
          blocked = false;
          slope_high = new_high_slope;
        }
      } else {
        if (!map->cells[map_index].transparent && distance < max_radius) {
          blocked = true;
          cast_light(map, pov_x, pov_y, distance + 1, slope_high, tile_slope_high, max_radius, octant, light_walls);
          new_high_slope = tile_slope_low;
        }
      }
    }
    if (blocked) {
      break;
    }
  }
}

TCOD_Error TCOD_map_compute_fov_recursive_shadowcasting(
    TCOD_Map* __restrict map, int pov_x, int pov_y, int max_radius, bool light_walls) {
  if (!TCOD_map_in_bounds(map, pov_x, pov_y)) {
    TCOD_set_errorvf("Point of view {%i, %i} is out of bounds.", pov_x, pov_y);
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (max_radius <= 0) {
    int max_radius_x = MAX(map->width - pov_x, pov_x);
    int max_radius_y = MAX(map->height - pov_y, pov_y);
    max_radius = (int)(sqrt(max_radius_x * max_radius_x + max_radius_y * max_radius_y)) + 1;
  }
  /* recursive shadow casting */
  for (int octant = 0; octant < 8; ++octant) {
    cast_light(map, pov_x, pov_y, 1, 1.0, 0.0, max_radius, octant, light_walls);
  }
  map->cells[pov_x + pov_y * map->width].fov = 1;
  return TCOD_E_OK;
}
