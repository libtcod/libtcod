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

#include "bresenham.h"
#include "fov.h"
#include "libtcod_int.h"
#include "utility.h"
/**
    Cast a Bresenham ray marking tiles along the line as lit.

    `radius_squared` is the max distance or zero if there is no limit.

    If `light_walls` is true then blocking walls are marked as visible.
 */
static void cast_ray(
    struct TCOD_Map* __restrict map,
    int x_origin,
    int y_origin,
    int x_dest,
    int y_dest,
    int radius_squared,
    bool light_walls) {
  TCOD_bresenham_data_t bresenham_data;
  int current_x;
  int current_y;
  TCOD_line_init_mt(x_origin, y_origin, x_dest, y_dest, &bresenham_data);
  while (!TCOD_line_step_mt(&current_x, &current_y, &bresenham_data)) {
    if (!TCOD_map_in_bounds(map, current_x, current_y)) {
      return;  // Out of bounds.
    }
    if (radius_squared > 0) {
      const int current_radius =
          (current_x - x_origin) * (current_x - x_origin) + (current_y - y_origin) * (current_y - y_origin);
      if (current_radius > radius_squared) {
        return;  // Outside of radius.
      }
    }
    const int map_index = current_x + current_y * map->width;
    if (!map->cells[map_index].transparent) {
      if (light_walls) {
        map->cells[map_index].fov = true;
      }
      return;  // Blocked by wall.
    }
    // Tile is transparent.
    map->cells[map_index].fov = true;
  }
}
TCOD_Error TCOD_map_compute_fov_circular_raycasting(
    TCOD_Map* __restrict map, int pov_x, int pov_y, int max_radius, bool light_walls) {
  int x_min = 0;  // Field-of-view bounds.
  int y_min = 0;
  int x_max = map->width;
  int y_max = map->height;
  if (max_radius > 0) {
    x_min = TCOD_MAX(x_min, pov_x - max_radius);
    y_min = TCOD_MAX(y_min, pov_y - max_radius);
    x_max = TCOD_MIN(x_max, pov_x + max_radius + 1);
    y_max = TCOD_MIN(y_max, pov_y + max_radius + 1);
  }
  if (!TCOD_map_in_bounds(map, pov_x, pov_y)) {
    TCOD_set_errorvf("Point of view {%i, %i} is out of bounds.", pov_x, pov_y);
    return TCOD_E_INVALID_ARGUMENT;
  }
  map->cells[pov_x + pov_y * map->width].fov = true;  // Mark point-of-view as visible.

  // Cast rays along the perimeter.
  const int radius_squared = max_radius * max_radius;
  for (int x = x_min; x < x_max; ++x) {
    cast_ray(map, pov_x, pov_y, x, y_min, radius_squared, light_walls);
  }
  for (int y = y_min + 1; y < y_max; ++y) {
    cast_ray(map, pov_x, pov_y, x_max - 1, y, radius_squared, light_walls);
  }
  for (int x = x_max - 2; x >= x_min; --x) {
    cast_ray(map, pov_x, pov_y, x, y_max - 1, radius_squared, light_walls);
  }
  for (int y = y_max - 2; y > y_min; --y) {
    cast_ray(map, pov_x, pov_y, x_min, y, radius_squared, light_walls);
  }
  if (light_walls) {
    TCOD_map_postprocess(map, pov_x, pov_y, max_radius);
  }
  return TCOD_E_OK;
}
