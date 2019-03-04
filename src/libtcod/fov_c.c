/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
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
#include "fov.h"

#include <stdlib.h>
#include <string.h>

#include "libtcod_int.h"
/**
 *  Return a new TCOD_Map with `width` and `height`.
 */
struct TCOD_Map *TCOD_map_new(int width, int height) {
  struct TCOD_Map *map = NULL;
  if (width <= 0 || height <= 0) { return NULL; }
  map = (struct TCOD_Map *)calloc(sizeof(struct TCOD_Map), 1);
  map->width = width;
  map->height = height;
  map->nbcells = width * height;
  map->cells = (struct TCOD_MapCell *)calloc(sizeof(struct TCOD_MapCell),
                                             map->nbcells);
  return map;
}
/**
 *  Clone map data from `source` to `dest`.
 *
 *  `dest` will be resized to match `source` if necessary.
 */
void TCOD_map_copy(const struct TCOD_Map *source, struct TCOD_Map *dest) {
  if (!source || !dest) { return; }
  if (dest->nbcells != source->nbcells) {
    free(dest->cells);
    dest->cells = (struct TCOD_MapCell *)malloc(sizeof(struct TCOD_MapCell)
                                                * dest->nbcells);
  }
  dest->width = source->width;
  dest->height = source->height;
  dest->nbcells = source->nbcells;
  memcpy(dest->cells, source->cells, sizeof(struct TCOD_MapCell)
                                     * source->nbcells);
}
/**
 *  Set all cell values on `map` to the given parameters.
 *
 *  This call also zeroes out the field of view flag.
 */
void TCOD_map_clear(struct TCOD_Map *map, bool transparent, bool walkable) {
  int i;
  if (!map) { return; }
  for (i = 0; i < map->nbcells; ++i) {
    map->cells[i].transparent = transparent;
    map->cells[i].walkable = walkable;
    map->cells[i].fov = 0;
  }
}
/**
 *  Return true if `x` and `y` are in the boundaries of `map`.
 *
 *  Returns false if `map` is NULL.
 */
static int TCOD_map_in_bounds(const struct TCOD_Map *map, int x, int y) {
  return map && 0 <= x && x < map->width && 0 <= y && y < map->height;
}
/**
 *  Set the properties of a single cell.
 */
void TCOD_map_set_properties(struct TCOD_Map *map, int x, int y,
                             bool is_transparent, bool is_walkable) {
  if (!TCOD_map_in_bounds(map, x, y)) { return; }
  map->cells[x + y * map->width].transparent = is_transparent;
  map->cells[x + y * map->width].walkable = is_walkable;
}
/**
 *  Free all memory belonging to `map`.
 */
void TCOD_map_delete(struct TCOD_Map *map) {
  if (!map) { return; }
  free(map->cells);
  free(map);
}
/**
 *  Calculate the field-of-view.
 *
 *  \rst
 *  `player_x` and `player_y` are the used as the field-of-view source.
 *
 *  `max_radius` is the maximum distance for the field-of-view algorithm.
 *
 *  If `light_walls` is false then only transparent cells will be touched by
 *  the field-of-view.
 *
 *  `algo` is one of the :any:`TCOD_fov_algorithm_t` algorithms.
 *
 *  After this call you may check if a cell is within the field-of-view by
 *  calling :any:`TCOD_map_is_in_fov`.
 *  \endrst
 */
void TCOD_map_compute_fov(struct TCOD_Map *map, int player_x, int player_y,
                          int max_radius, bool light_walls,
                          TCOD_fov_algorithm_t algo) {
  if (!map) { return; }
  switch(algo) {
    case FOV_BASIC:
      TCOD_map_compute_fov_circular_raycasting(map, player_x, player_y,
                                               max_radius, light_walls);
      return;
    case FOV_DIAMOND:
      TCOD_map_compute_fov_diamond_raycasting(map, player_x, player_y,
                                              max_radius, light_walls);
      return;
    case FOV_SHADOW:
      TCOD_map_compute_fov_recursive_shadowcasting(map, player_x, player_y,
                                                   max_radius, light_walls);
      return;
    case FOV_PERMISSIVE_0:
    case FOV_PERMISSIVE_1:
    case FOV_PERMISSIVE_2:
    case FOV_PERMISSIVE_3:
    case FOV_PERMISSIVE_4:
    case FOV_PERMISSIVE_5:
    case FOV_PERMISSIVE_6:
    case FOV_PERMISSIVE_7:
    case FOV_PERMISSIVE_8:
      TCOD_map_compute_fov_permissive2(map, player_x, player_y, max_radius,
                                       light_walls, algo - FOV_PERMISSIVE_0);
      return;
    case FOV_RESTRICTIVE:
      TCOD_map_compute_fov_restrictive_shadowcasting(map, player_x, player_y,
                                                     max_radius, light_walls);
      return;
    default: return;
  }
}
/**
 *  Return true if this cell was touched by the current field-of-view.
 */
bool TCOD_map_is_in_fov(const struct TCOD_Map *map, int x, int y) {
  if (!TCOD_map_in_bounds(map, x, y)) { return 0; }
  return map->cells[x + y * map->width].fov;
}
/**
 *  Set the fov flag on a specific cell.
 */
void TCOD_map_set_in_fov(struct TCOD_Map *map, int x, int y, bool fov) {
  if (!TCOD_map_in_bounds(map, x, y)) { return; }
  map->cells[x + y * map->width].fov = fov;
}
/**
 *  Return true if this cell is transparent.
 */
bool TCOD_map_is_transparent(const struct TCOD_Map *map, int x, int y) {
  if (!TCOD_map_in_bounds(map, x, y)) { return 0; }
  return map->cells[x + y * map->width].transparent;
}
/**
 *  Return true if this cell is walkable.
 */
bool TCOD_map_is_walkable(struct TCOD_Map *map, int x, int y) {
  if (!TCOD_map_in_bounds(map, x, y)) { return 0; }
  return map->cells[x + y * map->width].walkable;
}
/**
 *  Return the width of `map`.
 */
int TCOD_map_get_width(const struct TCOD_Map *map) {
  if (!map) { return 0; }
  return map->width;
}
/**
 *  Return the height of `map`.
 */
int TCOD_map_get_height(const struct TCOD_Map *map) {
  if (!map) { return 0; }
  return map->height;
}
/**
 *  Return the total number of cells in `map`.
 */
int TCOD_map_get_nb_cells(const struct TCOD_Map *map) {
  if (!map) { return 0; }
  return map->nbcells;
}
