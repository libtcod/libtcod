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
/** \file
    Symmetric Shadowcasting algorithm.

    Based on: https://www.albertford.com/shadowcasting/
 */
#include <float.h>
#include <math.h>
#include <stdbool.h>

#include "fov.h"
#include "libtcod_int.h"
/**
    Quadrant transformation matrixes.

    {xx, xy, yx, yy}
 */
static const int quadrant_table[4][4] = {
    {1, 0, 0, 1},
    {0, 1, 1, 0},
    {0, -1, -1, 0},
    {-1, 0, 0, -1},
};
/**
    Information for the current active row.
 */
typedef struct Row {
  const int pov_x;  // The origin point-of-view.
  const int pov_y;
  const int quadrant;  // The quadrant index.
  int depth;  // The depth of this row.
  float slope_low;
  const float slope_high;
} Row;
/**
    Returns true if a given floor tile can be seen symmetrically from the origin.

    It returns true if the central point of the tile is in the sector swept out
    by the row’s start and end slopes. Otherwise, it returns false.
 */
static bool is_symmetric(const Row* __restrict row, int column) {
  return column >= row->depth * row->slope_low && column <= row->depth * row->slope_high;
}
/**
    Calculates new start and end slopes.

    The line is tangent to the left edge of the current tile, so we can use a
    single slope function for both start and end slopes.
 */
static float slope(int row_depth, int column) { return (2.0f * column - 1.0f) / (2.0f * row_depth); }
/**
    Round half numbers towards infinity.
 */
static int round_half_up(float n) { return (int)roundf(n * (1 + FLT_EPSILON)); }
/**
    Round half numbers towards negative infinity.
 */
static int round_half_down(float n) { return (int)roundf(n * (1 - FLT_EPSILON)); }
/**
    Scan a row and recursively scan all of its children.

    If you think of each quadrant as a tree of rows, this essentially is a depth-first tree traversal.
 */
static void scan(TCOD_Map* __restrict map, Row* __restrict row) {
  const int xx = quadrant_table[row->quadrant][0];
  const int xy = quadrant_table[row->quadrant][1];
  const int yx = quadrant_table[row->quadrant][2];
  const int yy = quadrant_table[row->quadrant][3];
  if (!TCOD_map_in_bounds(map, row->pov_x + row->depth * xx, row->pov_y + row->depth * yx)) {
    return;  // Row->depth is out-of-bounds.
  }
  const int column_min = round_half_up(row->depth * row->slope_low);
  const int column_max = round_half_down(row->depth * row->slope_high);
  bool prev_tile_is_wall = false;
  for (int column = column_min; column <= column_max; ++column) {
    const int map_x = row->pov_x + row->depth * xx + column * xy;
    const int map_y = row->pov_y + row->depth * yx + column * yy;
    if (!TCOD_map_in_bounds(map, map_x, map_y)) {
      continue;  // Tile is out-of-bounds.
    }
    struct TCOD_MapCell* map_cell = &map->cells[map_x + map_y * map->width];
    const bool is_wall = !map_cell->transparent;
    if (is_wall || is_symmetric(row, column)) {
      map_cell->fov = true;
    }
    if (prev_tile_is_wall && !is_wall) {  // Floor tile to wall tile.
      row->slope_low = slope(row->depth, column);  // Shrink the view.
    }
    if (column != column_min && !prev_tile_is_wall && is_wall) {  // Wall tile to floor tile.
      // Track the slopes of the last transparent tiles and recurse into them.
      Row next_row = {
          .pov_x = row->pov_x,
          .pov_y = row->pov_y,
          .quadrant = row->quadrant,
          .depth = row->depth + 1,
          .slope_low = row->slope_low,
          .slope_high = slope(row->depth, column),
      };
      scan(map, &next_row);
    }
    prev_tile_is_wall = is_wall;
  }
  if (!prev_tile_is_wall) {
    // Tail recuse into the next row.
    row->depth += 1;
    scan(map, row);
  }
}

TCOD_Error TCOD_map_compute_fov_symmetric_shadowcast(
    TCOD_Map* __restrict map, int pov_x, int pov_y, int max_radius, bool light_walls) {
  if (!map) {
    TCOD_set_errorv("Map must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!TCOD_map_in_bounds(map, pov_x, pov_y)) {
    TCOD_set_errorvf("Point of view {%i, %i} is out of bounds.", pov_x, pov_y);
    return TCOD_E_INVALID_ARGUMENT;
  }
  map->cells[pov_x + pov_y * map->width].fov = true;
  for (int quadrant = 0; quadrant < 4; ++quadrant) {
    Row row = {
        .pov_x = pov_x,
        .pov_y = pov_y,
        .quadrant = quadrant,
        .depth = 1,
        .slope_low = -1.0f,
        .slope_high = 1.0f,
    };
    scan(map, &row);
  }
  const int radius_squared = max_radius * max_radius;
  for (int y = 0; y < map->height; ++y) {
    for (int x = 0; x < map->width; ++x) {
      int i = x + y * map->width;
      if (!light_walls && !map->cells[i].transparent) {
        map->cells[i].fov = false;
      }
      if (max_radius > 0) {
        const int dx = x - pov_x;
        const int dy = y - pov_y;
        if (dx * dx + dy * dy >= radius_squared) {
          map->cells[i].fov = false;
        }
      }
    }
  }
  return TCOD_E_OK;
}
