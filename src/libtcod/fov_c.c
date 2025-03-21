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
#include <stdlib.h>
#include <string.h>

#include "fov.h"
#include "libtcod_int.h"
#include "utility.h"
struct TCOD_Map* TCOD_map_new(int width, int height) {
  if (width <= 0 || height <= 0) {
    return NULL;
  }
  struct TCOD_Map* map = calloc(1, sizeof(*map));
  map->width = width;
  map->height = height;
  map->nbcells = width * height;
  map->cells = calloc(map->nbcells, sizeof(*map->cells));
  return map;
}
TCOD_Error TCOD_map_copy(const struct TCOD_Map* __restrict source, struct TCOD_Map* __restrict dest) {
  if (!source || !dest) {
    TCOD_set_errorv("source and dest must be non-NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (dest->nbcells != source->nbcells) {
    struct TCOD_MapCell* new_cells = malloc(sizeof(*dest->cells) * dest->nbcells);
    if (!new_cells) {
      TCOD_set_errorv("Out of memory while reallocating dest.");
      return TCOD_E_OUT_OF_MEMORY;
    }
    free(dest->cells);
    dest->cells = new_cells;
  }
  dest->width = source->width;
  dest->height = source->height;
  dest->nbcells = source->nbcells;
  memcpy(dest->cells, source->cells, sizeof(*dest->cells) * source->nbcells);
  return TCOD_E_OK;
}
void TCOD_map_clear(struct TCOD_Map* map, bool transparent, bool walkable) {
  int i;
  if (!map) {
    return;
  }
  for (i = 0; i < map->nbcells; ++i) {
    map->cells[i].transparent = transparent;
    map->cells[i].walkable = walkable;
    map->cells[i].fov = 0;
  }
}
void TCOD_map_set_properties(struct TCOD_Map* map, int x, int y, bool is_transparent, bool is_walkable) {
  if (!TCOD_map_in_bounds(map, x, y)) {
    return;
  }
  map->cells[x + y * map->width].transparent = is_transparent;
  map->cells[x + y * map->width].walkable = is_walkable;
}
void TCOD_map_delete(struct TCOD_Map* map) {
  if (!map) {
    return;
  }
  free(map->cells);
  free(map);
}
/**
    Spread lighting to walls to avoid lighting artifacts.

    `x0`, `y0` are the lower bounds.  `x1`, `y1` are the upper bounds.

    `dx`, `dy` is the cast direction.
 */
static void TCOD_map_postprocess_quadrant(TCOD_Map* __restrict map, int x0, int y0, int x1, int y1, int dx, int dy) {
  if (abs(dx) != 1 || abs(dy) != 1) {
    return;  // Bad parameters.
  }
  for (int cx = x0; cx <= x1; cx++) {
    for (int cy = y0; cy <= y1; cy++) {
      const int x2 = cx + dx;
      const int y2 = cy + dy;
      const int offset = cx + cy * map->width;
      if (offset < map->nbcells && map->cells[offset].fov == 1 && map->cells[offset].transparent) {
        if (x2 >= x0 && x2 <= x1) {
          const int offset2 = x2 + cy * map->width;
          if (offset2 < map->nbcells && !map->cells[offset2].transparent) {
            map->cells[offset2].fov = 1;
          }
        }
        if (y2 >= y0 && y2 <= y1) {
          const int offset2 = cx + y2 * map->width;
          if (offset2 < map->nbcells && !map->cells[offset2].transparent) {
            map->cells[offset2].fov = 1;
          }
        }
        if (x2 >= x0 && x2 <= x1 && y2 >= y0 && y2 <= y1) {
          const int offset2 = x2 + y2 * map->width;
          if (offset2 < map->nbcells && !map->cells[offset2].transparent) {
            map->cells[offset2].fov = 1;
          }
        }
      }
    }
  }
}
/**
    Spread lighting to walls to avoid lighting artifacts.
 */
TCOD_Error TCOD_map_postprocess(TCOD_Map* __restrict map, int pov_x, int pov_y, int radius) {
  int x_min = 0;
  int y_min = 0;
  int x_max = map->width;
  int y_max = map->height;
  if (radius > 0) {
    x_min = TCOD_MAX(x_min, pov_x - radius);
    y_min = TCOD_MAX(y_min, pov_y - radius);
    x_max = TCOD_MIN(x_max, pov_x + radius + 1);
    y_max = TCOD_MIN(y_max, pov_y + radius + 1);
  }
  TCOD_map_postprocess_quadrant(map, x_min, y_min, pov_x, pov_y, -1, -1);
  TCOD_map_postprocess_quadrant(map, pov_x, y_min, x_max - 1, pov_y, 1, -1);
  TCOD_map_postprocess_quadrant(map, x_min, pov_y, pov_x, y_max - 1, -1, 1);
  TCOD_map_postprocess_quadrant(map, pov_x, pov_y, x_max - 1, y_max - 1, 1, 1);
  return TCOD_E_OK;
}
/**
    Reset the map FOV flag to zeros.
 */
static void TCOD_map_clear_fov(TCOD_Map* __restrict map) {
  if (!map) {
    return;
  }
  for (int i = 0; i < map->nbcells; ++i) {
    map->cells[i].fov = 0;
  }
}
TCOD_Error TCOD_map_compute_fov(
    struct TCOD_Map* __restrict map,
    int pov_x,
    int pov_y,
    int max_radius,
    bool light_walls,
    TCOD_fov_algorithm_t algo) {
  if (!map) {
    TCOD_set_errorv("Map must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!TCOD_map_in_bounds(map, pov_x, pov_y)) {
    TCOD_set_errorvf("Point of view {%i, %i} is out of bounds.", pov_x, pov_y);
    return TCOD_E_INVALID_ARGUMENT;
  }
  TCOD_map_clear_fov(map);
  switch (algo) {
    case FOV_BASIC:
      return TCOD_map_compute_fov_circular_raycasting(map, pov_x, pov_y, max_radius, light_walls);
    case FOV_DIAMOND:
      return TCOD_map_compute_fov_diamond_raycasting(map, pov_x, pov_y, max_radius, light_walls);
    case FOV_SHADOW:
      return TCOD_map_compute_fov_recursive_shadowcasting(map, pov_x, pov_y, max_radius, light_walls);
    case FOV_PERMISSIVE_0:
    case FOV_PERMISSIVE_1:
    case FOV_PERMISSIVE_2:
    case FOV_PERMISSIVE_3:
    case FOV_PERMISSIVE_4:
    case FOV_PERMISSIVE_5:
    case FOV_PERMISSIVE_6:
    case FOV_PERMISSIVE_7:
    case FOV_PERMISSIVE_8:
      return TCOD_map_compute_fov_permissive2(map, pov_x, pov_y, max_radius, light_walls, algo - FOV_PERMISSIVE_0);
    case FOV_RESTRICTIVE:
      return TCOD_map_compute_fov_restrictive_shadowcasting(map, pov_x, pov_y, max_radius, light_walls);
    case FOV_SYMMETRIC_SHADOWCAST:
      return TCOD_map_compute_fov_symmetric_shadowcast(map, pov_x, pov_y, max_radius, light_walls);
    default:
      return TCOD_E_INVALID_ARGUMENT;
  }
}
bool TCOD_map_is_in_fov(const struct TCOD_Map* map, int x, int y) {
  if (!TCOD_map_in_bounds(map, x, y)) {
    return 0;
  }
  return map->cells[x + y * map->width].fov;
}
void TCOD_map_set_in_fov(struct TCOD_Map* map, int x, int y, bool fov) {
  if (!TCOD_map_in_bounds(map, x, y)) {
    return;
  }
  map->cells[x + y * map->width].fov = fov;
}
bool TCOD_map_is_transparent(const struct TCOD_Map* map, int x, int y) {
  if (!TCOD_map_in_bounds(map, x, y)) {
    return 0;
  }
  return map->cells[x + y * map->width].transparent;
}
bool TCOD_map_is_walkable(struct TCOD_Map* map, int x, int y) {
  if (!TCOD_map_in_bounds(map, x, y)) {
    return 0;
  }
  return map->cells[x + y * map->width].walkable;
}
int TCOD_map_get_width(const struct TCOD_Map* map) {
  if (!map) {
    return 0;
  }
  return map->width;
}
int TCOD_map_get_height(const struct TCOD_Map* map) {
  if (!map) {
    return 0;
  }
  return map->height;
}
int TCOD_map_get_nb_cells(const struct TCOD_Map* map) {
  if (!map) {
    return 0;
  }
  return map->nbcells;
}
