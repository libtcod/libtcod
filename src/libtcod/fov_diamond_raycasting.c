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

typedef struct _ray_data_t {
  int x_loc, y_loc;                    /* position */
  int xob, yob;                        /* obscurity vector */
  int x_err, y_err;                    /* bresenham error */
  struct _ray_data_t *xinput, *yinput; /* offset of input rays */
  bool added;                          /* already in the fov */
  bool ignore;                         /* non visible. don't bother processing it */
} ray_data_t;

static int ray_length_sq(ray_data_t* ray) { return (ray->x_loc * ray->x_loc) + (ray->y_loc * ray->y_loc); }

typedef struct _fov_t {
  struct TCOD_Map* map;
  TCOD_list_t perimeter;

  int origx, origy;    /* fov origin */
  ray_data_t** raymap; /* result rays */
  ray_data_t* raymap2; /* temporary rays */
} fov_t;

static ray_data_t* new_ray(fov_t* fov, int rx, int ry) {
  struct TCOD_Map* map = fov->map;
  const int x = fov->origx + rx;
  const int y = fov->origy + ry;

  if (x < 0 || y < 0) return NULL;
  if (x >= map->width) return NULL;
  if (y >= map->height) return NULL;

  ray_data_t* r = &fov->raymap2[x + (y * map->width)];
  r->x_loc = rx;
  r->y_loc = ry;
  return r;
}

static void processRay(fov_t* fov, ray_data_t* new_ray, ray_data_t* input_ray) {
  if (new_ray) {
    const int map_x = fov->origx + new_ray->x_loc;
    const int map_y = fov->origy + new_ray->y_loc;
    const int newrayidx = map_x + (map_y * fov->map->width);
    if (new_ray->y_loc == input_ray->y_loc) {
      new_ray->xinput = input_ray;
    } else {
      new_ray->yinput = input_ray;
    }
    if (!new_ray->added) {
      TCOD_list_push(fov->perimeter, new_ray);
      new_ray->added = true;
      fov->raymap[newrayidx] = new_ray;
    }
  }
}

static bool is_obscure(ray_data_t* r) {
  return (r->x_err > 0 && r->x_err <= r->xob) || (r->y_err > 0 && r->y_err <= r->yob);
}

static void process_x_input(ray_data_t* new_ray, ray_data_t* xinput) {
  if (xinput->xob == 0 && xinput->yob == 0) return;
  if (xinput->x_err > 0 && new_ray->xob == 0) {
    new_ray->x_err = xinput->x_err - xinput->yob;
    new_ray->y_err = xinput->y_err + xinput->yob;
    new_ray->xob = xinput->xob;
    new_ray->yob = xinput->yob;
  }
  if (xinput->y_err <= 0 && xinput->yob > 0 && xinput->x_err > 0) {
    new_ray->y_err = xinput->y_err + xinput->yob;
    new_ray->x_err = xinput->x_err - xinput->yob;
    new_ray->xob = xinput->xob;
    new_ray->yob = xinput->yob;
  }
}

static void process_y_input(ray_data_t* new_ray, ray_data_t* yinput) {
  if (yinput->xob == 0 && yinput->yob == 0) return;
  if (yinput->y_err > 0 && new_ray->yob == 0) {
    new_ray->y_err = yinput->y_err - yinput->xob;
    new_ray->x_err = yinput->x_err + yinput->xob;
    new_ray->xob = yinput->xob;
    new_ray->yob = yinput->yob;
  }
  if (yinput->x_err <= 0 && yinput->xob > 0 && yinput->y_err > 0) {
    new_ray->y_err = yinput->y_err - yinput->xob;
    new_ray->x_err = yinput->x_err + yinput->xob;
    new_ray->xob = yinput->xob;
    new_ray->yob = yinput->yob;
  }
}

static void merge_input(fov_t* fov, ray_data_t* r) {
  TCOD_Map* map = fov->map;
  const int x = r->x_loc + fov->origx;
  const int y = r->y_loc + fov->origy;
  const int rayidx = x + y * map->width;

  ray_data_t* xi = r->xinput;
  ray_data_t* yi = r->yinput;
  if (xi) process_x_input(r, xi);
  if (yi) process_y_input(r, yi);
  if (!xi) {
    if (is_obscure(yi)) r->ignore = true;
  } else if (!yi) {
    if (is_obscure(xi)) r->ignore = true;
  } else if (is_obscure(xi) && is_obscure(yi)) {
    r->ignore = true;
  }
  if (!r->ignore && !map->cells[rayidx].transparent) {
    r->x_err = r->xob = ABS(r->x_loc);
    r->y_err = r->yob = ABS(r->y_loc);
  }
}

static void expandPerimeterFrom(fov_t* fov, ray_data_t* r) {
  const int rx = r->x_loc;
  const int ry = r->y_loc;
  if (rx >= 0) {
    processRay(fov, new_ray(fov, rx + 1, ry), r);
  }
  if (rx <= 0) {
    processRay(fov, new_ray(fov, rx - 1, ry), r);
  }
  if (ry >= 0) {
    processRay(fov, new_ray(fov, rx, ry + 1), r);
  }
  if (ry <= 0) {
    processRay(fov, new_ray(fov, rx, ry - 1), r);
  }
}

void TCOD_map_compute_fov_diamond_raycasting(
    TCOD_map_t m, int player_x, int player_y, int max_radius, bool light_walls) {
  const int radius_sq = max_radius * max_radius;
  const int nbcells = m->nbcells;

  fov_t fov;
  fov.map = m;
  fov.perimeter = TCOD_list_allocate(nbcells);

  fov.origx = player_x;
  fov.origy = player_y;
  fov.raymap = calloc(sizeof(*fov.raymap), nbcells);
  fov.raymap2 = calloc(sizeof(*fov.raymap2), nbcells);

  expandPerimeterFrom(&fov, new_ray(&fov, 0, 0));

  for (int perimeter_idx = 0; perimeter_idx < TCOD_list_size(fov.perimeter); perimeter_idx++) {
    ray_data_t* ray = (ray_data_t*)TCOD_list_get(fov.perimeter, perimeter_idx);
    if (radius_sq == 0 || ray_length_sq(ray) <= radius_sq) {
      merge_input(&fov, ray);
      if (!ray->ignore) expandPerimeterFrom(&fov, ray);
    } else {
      ray->ignore = true;
    }
  }

  /* set fov data */
  for (int i = 0; i != nbcells; ++i) {
    struct TCOD_MapCell* cell = &m->cells[i];
    ray_data_t* ray = fov.raymap[i];

    cell->fov = false;
    if (ray == NULL) continue;
    if (ray->ignore) continue;
    if (ray->x_err > 0 && ray->x_err <= ray->xob) continue;
    if (ray->y_err > 0 && ray->y_err <= ray->yob) continue;
    cell->fov = true;
  }
  m->cells[player_x + player_y * m->width].fov = true;

  /* light walls */
  if (light_walls) {
    int xmin = 0, ymin = 0, xmax = m->width, ymax = m->height;
    if (max_radius > 0) {
      xmin = MAX(0, player_x - max_radius);
      ymin = MAX(0, player_y - max_radius);
      xmax = MIN(m->width, player_x + max_radius + 1);
      ymax = MIN(m->height, player_y + max_radius + 1);
    }
    TCOD_map_postproc(m, xmin, ymin, player_x, player_y, -1, -1);
    TCOD_map_postproc(m, player_x, ymin, xmax - 1, player_y, 1, -1);
    TCOD_map_postproc(m, xmin, player_y, player_x, ymax - 1, -1, 1);
    TCOD_map_postproc(m, player_x, player_y, xmax - 1, ymax - 1, 1, 1);
  }

  free(fov.raymap);
  free(fov.raymap2);
  TCOD_list_delete(fov.perimeter);
}
