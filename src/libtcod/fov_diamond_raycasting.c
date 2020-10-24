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
#include <stdlib.h>
#include <string.h>

#include "fov.h"
#include "libtcod_int.h"
#include "utility.h"
/**
    A diamond raycast tile.
 */
typedef struct RayData {
  int x_relative, y_relative;               // Ray position relative to the POV.
  int x_obscurity, y_obscurity;             // Obscurity vector.
  int x_error, y_error;                     // Bresenham error.
  const struct RayData *x_input, *y_input;  // Offset of input rays.
  bool added;                               // Already in the fov.
  bool ignore;                              // Non visible.  Don't bother processing it.
} RayData;
/**
    Return a rays squared distance from the origin POV.
 */
static int ray_length_sq(const RayData* ray) {
  return (ray->x_relative * ray->x_relative) + (ray->y_relative * ray->y_relative);
}
/**
    The diamond raycast state.
 */
typedef struct DiamondFov {
  struct TCOD_Map* __restrict const map;
  TCOD_list_t perimeter;
  const int pov_x, pov_y;          // Fov origin point, the POV.
  RayData** const raymap_results;  // Grid of result rays.
  RayData* const raymap_temp;      // Grid of temporary rays.
} DiamondFov;

static RayData* new_ray(DiamondFov* fov, int ray_x, int ray_y) {
  const struct TCOD_Map* map = fov->map;
  const int x = fov->pov_x + ray_x;
  const int y = fov->pov_y + ray_y;
  if (x < 0 || y < 0 || x >= map->width || y >= map->height) {
    return NULL;
  }
  RayData* ray = &fov->raymap_temp[x + (y * map->width)];
  ray->x_relative = ray_x;
  ray->y_relative = ray_y;
  return ray;
}

static void process_ray(DiamondFov* fov, RayData* new_ray, const RayData* input_ray) {
  if (new_ray) {
    const int map_x = fov->pov_x + new_ray->x_relative;
    const int map_y = fov->pov_y + new_ray->y_relative;
    const int new_ray_index = map_x + (map_y * fov->map->width);
    if (new_ray->y_relative == input_ray->y_relative) {
      new_ray->x_input = input_ray;
    } else {
      new_ray->y_input = input_ray;
    }
    if (!new_ray->added) {
      TCOD_list_push(fov->perimeter, new_ray);
      new_ray->added = true;
      fov->raymap_results[new_ray_index] = new_ray;
    }
  }
}

static bool is_obscure(const RayData* ray) {
  return (ray->x_error > 0 && ray->x_error <= ray->x_obscurity) ||
         (ray->y_error > 0 && ray->y_error <= ray->y_obscurity);
}

static void process_x_input(RayData* new_ray, const RayData* x_input) {
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

static void process_y_input(RayData* new_ray, const RayData* y_input) {
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

static void merge_input(const DiamondFov* fov, RayData* ray) {
  const TCOD_Map* map = fov->map;
  const int x = ray->x_relative + fov->pov_x;
  const int y = ray->y_relative + fov->pov_y;
  const int ray_index = x + y * map->width;

  const RayData* xi = ray->x_input;
  const RayData* yi = ray->y_input;
  if (xi) {
    process_x_input(ray, xi);
  }
  if (yi) {
    process_y_input(ray, yi);
  }
  if (!xi) {
    if (is_obscure(yi)) {
      ray->ignore = true;
    }
  } else if (!yi) {
    if (is_obscure(xi)) {
      ray->ignore = true;
    }
  } else if (is_obscure(xi) && is_obscure(yi)) {
    ray->ignore = true;
  }
  if (!ray->ignore && !map->cells[ray_index].transparent) {
    ray->x_error = ray->x_obscurity = ABS(ray->x_relative);
    ray->y_error = ray->y_obscurity = ABS(ray->y_relative);
  }
}

static void expand_perimeter_from(DiamondFov* fov, const RayData* ray) {
  if (ray->x_relative >= 0) {
    process_ray(fov, new_ray(fov, ray->x_relative + 1, ray->y_relative), ray);
  }
  if (ray->x_relative <= 0) {
    process_ray(fov, new_ray(fov, ray->x_relative - 1, ray->y_relative), ray);
  }
  if (ray->y_relative >= 0) {
    process_ray(fov, new_ray(fov, ray->x_relative, ray->y_relative + 1), ray);
  }
  if (ray->y_relative <= 0) {
    process_ray(fov, new_ray(fov, ray->x_relative, ray->y_relative - 1), ray);
  }
}

void TCOD_map_compute_fov_diamond_raycasting(
    TCOD_Map* __restrict map, int player_x, int player_y, int max_radius, bool light_walls) {
  const int radius_squared = max_radius * max_radius;
  const int nbcells = map->nbcells;

  DiamondFov fov = {
      .map = map,
      .perimeter = TCOD_list_allocate(nbcells),
      .pov_x = player_x,
      .pov_y = player_y,
      .raymap_results = calloc(sizeof(*fov.raymap_results), nbcells),
      .raymap_temp = calloc(sizeof(*fov.raymap_temp), nbcells),
  };

  expand_perimeter_from(&fov, new_ray(&fov, 0, 0));

  for (int perimeter_idx = 0; perimeter_idx < TCOD_list_size(fov.perimeter); perimeter_idx++) {
    RayData* ray = (RayData*)TCOD_list_get(fov.perimeter, perimeter_idx);
    if (radius_squared == 0 || ray_length_sq(ray) <= radius_squared) {
      merge_input(&fov, ray);
      if (!ray->ignore) {
        expand_perimeter_from(&fov, ray);
      }
    } else {
      ray->ignore = true;
    }
  }

  /* set fov data */
  for (int i = 0; i < nbcells; ++i) {
    struct TCOD_MapCell* cell = &map->cells[i];
    const RayData* ray = fov.raymap_results[i];

    cell->fov = false;
    if (ray == NULL) {
      continue;
    }
    if (ray->ignore) {
      continue;
    }
    if (ray->x_error > 0 && ray->x_error <= ray->x_obscurity) {
      continue;
    }
    if (ray->y_error > 0 && ray->y_error <= ray->y_obscurity) {
      continue;
    }
    cell->fov = true;
  }
  map->cells[player_x + player_y * map->width].fov = true;

  /* light walls */
  if (light_walls) {
    int xmin = 0;
    int ymin = 0;
    int xmax = map->width;
    int ymax = map->height;
    if (max_radius > 0) {
      xmin = MAX(xmin, player_x - max_radius);
      ymin = MAX(ymin, player_y - max_radius);
      xmax = MIN(xmax, player_x + max_radius + 1);
      ymax = MIN(ymax, player_y + max_radius + 1);
    }
    TCOD_map_postproc(map, xmin, ymin, player_x, player_y, -1, -1);
    TCOD_map_postproc(map, player_x, ymin, xmax - 1, player_y, 1, -1);
    TCOD_map_postproc(map, xmin, player_y, player_x, ymax - 1, -1, 1);
    TCOD_map_postproc(map, player_x, player_y, xmax - 1, ymax - 1, 1, 1);
  }

  free(fov.raymap_results);
  free(fov.raymap_temp);
  TCOD_list_delete(fov.perimeter);
}
