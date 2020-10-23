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
static int mult[4][8] = {
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
    int row,
    float start,
    float end,
    int radius,
    int xx,
    int xy,
    int yx,
    int yy,
    bool light_walls) {
  const int radius_squared = radius * radius;
  float new_start = 0.0f;
  if (start < end) {
    return;
  }
  for (; row < radius + 1; ++row) {
    bool blocked = false;
    const int dy = -row;
    for (int dx = -row; dx <= 0; ++dx) {
      const int map_x = pov_x + dx * xx + dy * xy;
      const int map_y = pov_y + dx * yx + dy * yy;
      if (0 <= map_x && 0 <= map_y && map_x < map->width && map_y < map->height) {
        const int offset = map_x + map_y * map->width;
        const float l_slope = (dx - 0.5f) / (dy + 0.5f);
        const float r_slope = (dx + 0.5f) / (dy - 0.5f);
        if (start < r_slope) {
          continue;
        } else if (end > l_slope) {
          break;
        }
        if (dx * dx + dy * dy <= radius_squared && (light_walls || map->cells[offset].transparent)) {
          map->cells[offset].fov = 1;
        }
        if (blocked) {
          if (!map->cells[offset].transparent) {
            new_start = r_slope;
            continue;
          } else {
            blocked = false;
            start = new_start;
          }
        } else {
          if (!map->cells[offset].transparent && row < radius) {
            blocked = true;
            cast_light(map, pov_x, pov_y, row + 1, start, l_slope, radius, xx, xy, yx, yy, light_walls);
            new_start = r_slope;
          }
        }
      }
    }
    if (blocked) {
      break;
    }
  }
}

void TCOD_map_compute_fov_recursive_shadowcasting(
    TCOD_Map* __restrict map, int player_x, int player_y, int max_radius, bool light_walls) {
  for (int i = 0; i < map->nbcells; ++i) {
    map->cells[i].fov = false;
  }
  if (max_radius <= 0) {
    int max_radius_x = MAX(map->width - player_x, player_x);
    int max_radius_y = MAX(map->height - player_y, player_y);
    max_radius = (int)(sqrt(max_radius_x * max_radius_x + max_radius_y * max_radius_y)) + 1;
  }
  /* recursive shadow casting */
  for (int octant = 0; octant < 8; ++octant) {
    cast_light(
        map,
        player_x,
        player_y,
        1,
        1.0,
        0.0,
        max_radius,
        mult[0][octant],
        mult[1][octant],
        mult[2][octant],
        mult[3][octant],
        light_walls);
  }
  map->cells[player_x + player_y * map->width].fov = 1;
}
