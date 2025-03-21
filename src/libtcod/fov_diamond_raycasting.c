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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "fov.h"
#include "libtcod_int.h"
#include "utility.h"
/**
    A discrete diamond raycast tile.
 */
typedef struct RaycastTile {
  int x_relative, y_relative;  // Ray position relative to the POV.
  int x_obscurity, y_obscurity;  // Obscurity vector.
  int x_error, y_error;  // Bresenham error.
  const struct RaycastTile* __restrict x_input;  // Pointer to the x-adjacent source ray.
  const struct RaycastTile* __restrict y_input;  // Pointer to the y-adjacent source ray.
  struct RaycastTile* perimeter_next;  // The next queued raycast of the perimeter.
  bool touched;  // Becomes true once this ray is added to the perimeter.
  bool ignore;  // Marked as non visible.
} RaycastTile;
/**
    Return a rays squared distance from the origin POV.
 */
static int ray_length_sq(const RaycastTile* __restrict ray) {
  return (ray->x_relative * ray->x_relative) + (ray->y_relative * ray->y_relative);
}
/**
    The diamond raycast state.
 */
typedef struct DiamondFov {
  TCOD_Map* __restrict const map;
  const int pov_x, pov_y;  // Fov origin point, the POV.
  RaycastTile* __restrict const raymap_grid;  // Grid of temporary rays.
  RaycastTile* perimeter_last;  // Pointer to the last tile on the perimeter.
} DiamondFov;
/**
    Return a pointer to the tile belonging relative to the POV.

    Returns NULL if the tile would be out-of-bounds.
 */
static RaycastTile* get_ray(DiamondFov* __restrict fov, int relative_x, int relative_y) {
  const int x = fov->pov_x + relative_x;
  const int y = fov->pov_y + relative_y;
  if (!TCOD_map_in_bounds(fov->map, x, y)) {
    return NULL;
  }
  RaycastTile* ray = &fov->raymap_grid[x + (y * fov->map->width)];
  ray->x_relative = relative_x;
  ray->y_relative = relative_y;
  return ray;
}
/**
    Configure the relationships of `new_ray` and add it to the perimeter.

    `input_ray` is the source tile for `new_ray`.
 */
static void process_ray(DiamondFov* fov, RaycastTile* __restrict new_ray, const RaycastTile* __restrict input_ray) {
  if (!new_ray) {
    return;
  }
  if (new_ray->y_relative == input_ray->y_relative) {
    new_ray->x_input = input_ray;
  } else {
    new_ray->y_input = input_ray;
  }
  if (!new_ray->touched) {
    // Add this new tile to the perimeter.
    fov->perimeter_last->perimeter_next = new_ray;
    fov->perimeter_last = new_ray;
    new_ray->touched = true;
  }
}
/**
    Return true if this tile is obstructed.
 */
static bool is_obscured(const RaycastTile* __restrict ray) {
  return (ray->x_error > 0 && ray->x_error <= ray->x_obscurity) ||
         (ray->y_error > 0 && ray->y_error <= ray->y_obscurity);
}
static void process_x_input(RaycastTile* __restrict new_ray, const RaycastTile* __restrict x_input) {
  if (x_input->x_obscurity == 0 && x_input->y_obscurity == 0) {
    return;
  }
  if (x_input->x_error > 0 && new_ray->x_obscurity == 0) {
    new_ray->x_error = x_input->x_error - x_input->y_obscurity;
    new_ray->y_error = x_input->y_error + x_input->y_obscurity;
    new_ray->x_obscurity = x_input->x_obscurity;
    new_ray->y_obscurity = x_input->y_obscurity;
  }
  if (x_input->y_error <= 0 && x_input->y_obscurity > 0 && x_input->x_error > 0) {
    new_ray->y_error = x_input->y_error + x_input->y_obscurity;
    new_ray->x_error = x_input->x_error - x_input->y_obscurity;
    new_ray->x_obscurity = x_input->x_obscurity;
    new_ray->y_obscurity = x_input->y_obscurity;
  }
}
static void process_y_input(RaycastTile* __restrict new_ray, const RaycastTile* __restrict y_input) {
  if (y_input->x_obscurity == 0 && y_input->y_obscurity == 0) {
    return;
  }
  if (y_input->y_error > 0 && new_ray->y_obscurity == 0) {
    new_ray->y_error = y_input->y_error - y_input->x_obscurity;
    new_ray->x_error = y_input->x_error + y_input->x_obscurity;
    new_ray->x_obscurity = y_input->x_obscurity;
    new_ray->y_obscurity = y_input->y_obscurity;
  }
  if (y_input->x_error <= 0 && y_input->x_obscurity > 0 && y_input->y_error > 0) {
    new_ray->y_error = y_input->y_error - y_input->x_obscurity;
    new_ray->x_error = y_input->x_error + y_input->x_obscurity;
    new_ray->x_obscurity = y_input->x_obscurity;
    new_ray->y_obscurity = y_input->y_obscurity;
  }
}
/**
    Combine this rays source tiles to tell how obscured `ray` is.
 */
static void merge_input(const DiamondFov* __restrict fov, RaycastTile* __restrict ray) {
  const TCOD_Map* map = fov->map;
  const int x = ray->x_relative + fov->pov_x;
  const int y = ray->y_relative + fov->pov_y;
  const int ray_index = x + y * map->width;

  if (ray->x_input) {
    process_x_input(ray, ray->x_input);
  }
  if (ray->y_input) {
    process_y_input(ray, ray->y_input);
  }
  if (!ray->x_input) {
    if (is_obscured(ray->y_input)) {
      ray->ignore = true;
    }
  } else if (!ray->y_input) {
    if (is_obscured(ray->x_input)) {
      ray->ignore = true;
    }
  } else if (is_obscured(ray->x_input) && is_obscured(ray->y_input)) {
    ray->ignore = true;
  }
  if (!ray->ignore && !map->cells[ray_index].transparent) {
    ray->x_error = ray->x_obscurity = TCOD_ABS(ray->x_relative);
    ray->y_error = ray->y_obscurity = TCOD_ABS(ray->y_relative);
  }
}
/**
    Expand the perimeter outwards from this tile.
 */
static void expand_perimeter_from(DiamondFov* __restrict fov, const RaycastTile* __restrict ray) {
  if (ray->ignore) {
    return;  // This tile was excluded from the perimeter.
  }
  if (ray->x_relative >= 0) {
    process_ray(fov, get_ray(fov, ray->x_relative + 1, ray->y_relative), ray);
  }
  if (ray->x_relative <= 0) {
    process_ray(fov, get_ray(fov, ray->x_relative - 1, ray->y_relative), ray);
  }
  if (ray->y_relative >= 0) {
    process_ray(fov, get_ray(fov, ray->x_relative, ray->y_relative + 1), ray);
  }
  if (ray->y_relative <= 0) {
    process_ray(fov, get_ray(fov, ray->x_relative, ray->y_relative - 1), ray);
  }
}
TCOD_Error TCOD_map_compute_fov_diamond_raycasting(
    TCOD_Map* __restrict map, int pov_x, int pov_y, int max_radius, bool light_walls) {
  const int radius_squared = max_radius * max_radius;

  if (!TCOD_map_in_bounds(map, pov_x, pov_y)) {
    TCOD_set_errorvf("Point of view {%i, %i} is out of bounds.", pov_x, pov_y);
    return TCOD_E_INVALID_ARGUMENT;
  }
  map->cells[pov_x + pov_y * map->width].fov = true;

  DiamondFov fov = {
      .map = map,
      .pov_x = pov_x,
      .pov_y = pov_y,
      .raymap_grid = calloc(map->nbcells, sizeof(*fov.raymap_grid)),
  };

  if (!fov.raymap_grid) {
    TCOD_set_errorv("Out of memory.");
    return TCOD_E_OUT_OF_MEMORY;
  }

  // Add the origin ray tile to start the process.
  RaycastTile* current_ray = fov.perimeter_last = get_ray(&fov, 0, 0);
  current_ray->touched = true;
  expand_perimeter_from(&fov, current_ray);

  // Iterative over the diamond perimeter.
  while ((current_ray = current_ray->perimeter_next) != NULL) {
    if (radius_squared <= 0 || ray_length_sq(current_ray) <= radius_squared) {
      merge_input(&fov, current_ray);
    } else {
      current_ray->ignore = true;  // Mark out-of-range tiles as ignored.
    }
    expand_perimeter_from(&fov, current_ray);

    // Check if this tile is visible.
    // current_ray->touched is true.
    if (current_ray->ignore) {
      continue;
    }
    if (current_ray->x_error > 0 && current_ray->x_error <= current_ray->x_obscurity) {
      continue;
    }
    if (current_ray->y_error > 0 && current_ray->y_error <= current_ray->y_obscurity) {
      continue;
    }
    const int map_x = pov_x + current_ray->x_relative;
    const int map_y = pov_y + current_ray->y_relative;
    map->cells[map_x + map_y * map->width].fov = true;
  }
  free(fov.raymap_grid);
  if (light_walls) {
    TCOD_map_postprocess(map, pov_x, pov_y, max_radius);
  }
  return TCOD_E_OK;
}
