/* BSD 3-Clause License
 *
 * Copyright © 2008-2025, Jice and the libtcod contributors.
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
    Octant transformation matrixes.

    {xx, xy, yx, yy}
 */
static const int matrix_table[8][4] = {
    {1, 0, 0, 1},
    {0, 1, 1, 0},
    {0, -1, 1, 0},
    {-1, 0, 0, 1},
    {-1, 0, 0, -1},
    {0, -1, -1, 0},
    {0, 1, -1, 0},
    {1, 0, 0, -1},
};
/**
    Cast visiblity using shadowcasting.
 */
static void cast_light(
    struct TCOD_Map* __restrict map,
    int pov_x,
    int pov_y,
    int distance,  // Polar distance from POV.
    float view_slope_high,
    float view_slope_low,
    int max_radius,
    int octant,
    bool light_walls) {
  const int xx = matrix_table[octant][0];
  const int xy = matrix_table[octant][1];
  const int yx = matrix_table[octant][2];
  const int yy = matrix_table[octant][3];
  const int radius_squared = max_radius * max_radius;
  if (view_slope_high < view_slope_low) {
    return;  // View is invalid.
  }
  if (distance > max_radius) {
    return;  // Distance is out-of-range.
  }
  if (!TCOD_map_in_bounds(map, pov_x + distance * xy, pov_y + distance * yy)) {
    return;  // Distance is out-of-bounds.
  }
  bool prev_tile_blocked = false;
  for (int angle = distance; angle >= 0; --angle) {  // Polar angle coordinates from high to low.
    const float tile_slope_high = (angle + 0.5f) / (distance - 0.5f);
    const float tile_slope_low = (angle - 0.5f) / (distance + 0.5f);
    const float prev_tile_slope_low = (angle + 0.5f) / (distance + 0.5f);
    if (tile_slope_low > view_slope_high) {
      continue;  // Tile is not in the view yet.
    } else if (tile_slope_high < view_slope_low) {
      break;  // Tiles will no longer be in view.
    }
    // Current tile is in view.
    const int map_x = pov_x + angle * xx + distance * xy;
    const int map_y = pov_y + angle * yx + distance * yy;
    if (!TCOD_map_in_bounds(map, map_x, map_y)) {
      continue;  // Angle is out-of-bounds.
    }
    const int map_index = map_x + map_y * map->width;
    if (angle * angle + distance * distance <= radius_squared && (light_walls || map->cells[map_index].transparent)) {
      map->cells[map_index].fov = true;
    }
    if (prev_tile_blocked && map->cells[map_index].transparent) {  // Wall -> floor.
      view_slope_high = prev_tile_slope_low;  // Reduce the view size.
    }
    if (!prev_tile_blocked && !map->cells[map_index].transparent) {  // Floor -> wall.
      // Get the last sequence of floors as a view and recurse into them.
      cast_light(map, pov_x, pov_y, distance + 1, view_slope_high, tile_slope_high, max_radius, octant, light_walls);
    }
    prev_tile_blocked = !map->cells[map_index].transparent;
  }
  if (!prev_tile_blocked) {
    // Tail-recurse into the current view.
    cast_light(map, pov_x, pov_y, distance + 1, view_slope_high, view_slope_low, max_radius, octant, light_walls);
  }
}

TCOD_Error TCOD_map_compute_fov_recursive_shadowcasting(
    TCOD_Map* __restrict map, int pov_x, int pov_y, int max_radius, bool light_walls) {
  if (!TCOD_map_in_bounds(map, pov_x, pov_y)) {
    TCOD_set_errorvf("Point of view {%i, %i} is out of bounds.", pov_x, pov_y);
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (max_radius <= 0) {
    int max_radius_x = TCOD_MAX(map->width - pov_x, pov_x);
    int max_radius_y = TCOD_MAX(map->height - pov_y, pov_y);
    max_radius = (int)(sqrt(max_radius_x * max_radius_x + max_radius_y * max_radius_y)) + 1;
  }
  /* recursive shadow casting */
  for (int octant = 0; octant < 8; ++octant) {
    cast_light(map, pov_x, pov_y, 1, 1.0, 0.0, max_radius, octant, light_walls);
  }
  map->cells[pov_x + pov_y * map->width].fov = 1;
  return TCOD_E_OK;
}
