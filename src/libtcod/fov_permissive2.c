/* BSD 3-Clause License
 *
 * Copyright © 2008-2023, Jice and the libtcod contributors.
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
#include <stb_ds.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fov.h"
#include "libtcod_int.h"
#include "utility.h"

// Return a pointer to the end of an stb_ds array.
#define ds_array_end(a) (a + stbds_arrlen(a))

/* The size of each square in units */
#define STEP_SIZE 16

/* Jonathon Duerig enhanced permissive FOV */
typedef struct {
  int xi, yi, xf, yf;
} Line;

typedef struct ViewBump {
  int x, y;
  struct ViewBump* parent;
} ViewBump;

typedef struct {
  Line shallow_line;
  Line steep_line;
  ViewBump* shallow_bump;
  ViewBump* steep_bump;
} View;

typedef struct {
  int count;  // Current number of elements in this container.
  ViewBump* __restrict data;  // This array is preallocated.
} ViewBumpContainer;

static int RELATIVE_SLOPE(const Line* line, int x, int y) {
  return (line->yf - line->yi) * (line->xf - x) - (line->xf - line->xi) * (line->yf - y);
}
static bool BELOW(const Line* line, int x, int y) { return RELATIVE_SLOPE(line, x, y) > 0; }
static bool BELOW_OR_COLINEAR(const Line* line, int x, int y) { return RELATIVE_SLOPE(line, x, y) >= 0; }
static bool ABOVE(const Line* line, int x, int y) { return RELATIVE_SLOPE(line, x, y) < 0; }
static bool ABOVE_OR_COLINEAR(const Line* line, int x, int y) { return RELATIVE_SLOPE(line, x, y) <= 0; }
static bool COLINEAR(const Line* line, int x, int y) { return RELATIVE_SLOPE(line, x, y) == 0; }
static bool LINE_COLINEAR(const Line* line1, const Line* line2) {
  return COLINEAR(line1, line2->xi, line2->yi) && COLINEAR(line1, line2->xf, line2->yf);
}

static bool is_blocked(TCOD_Map* map, int pov_x, int pov_y, int x, int y, int dx, int dy, bool light_walls) {
  int pos_x = x * dx / STEP_SIZE + pov_x;
  int pos_y = y * dy / STEP_SIZE + pov_y;
  int cells_offset = pos_x + pos_y * map->width;
  bool blocked = !map->cells[cells_offset].transparent;
  if (!blocked || light_walls) {
    map->cells[cells_offset].fov = 1;
  }
  return blocked;
}

static void add_shallow_bump(int x, int y, View* view, ViewBumpContainer* bumps) {
  view->shallow_line.xf = x;
  view->shallow_line.yf = y;
  ViewBump* shallow = &bumps->data[bumps->count++];
  shallow->x = x;
  shallow->y = y;
  shallow->parent = view->shallow_bump;
  view->shallow_bump = shallow;
  const ViewBump* current_bump = view->steep_bump;
  while (current_bump) {
    if (ABOVE(&view->shallow_line, current_bump->x, current_bump->y)) {
      view->shallow_line.xi = current_bump->x;
      view->shallow_line.yi = current_bump->y;
    }
    current_bump = current_bump->parent;
  }
}

static void add_steep_bump(int x, int y, View* view, ViewBumpContainer* bumps) {
  view->steep_line.xf = x;
  view->steep_line.yf = y;
  ViewBump* steep = &bumps->data[bumps->count++];
  steep->x = x;
  steep->y = y;
  steep->parent = view->steep_bump;
  view->steep_bump = steep;
  const ViewBump* current_bump = view->shallow_bump;
  while (current_bump) {
    if (BELOW(&view->steep_line, current_bump->x, current_bump->y)) {
      view->steep_line.xi = current_bump->x;
      view->steep_line.yi = current_bump->y;
    }
    current_bump = current_bump->parent;
  }
}

static bool check_view(View*** active_views, View** it, int offset, int limit) {
  const View* view = *it;
  const Line* shallow_line = &view->shallow_line;
  const Line* steep_line = &view->steep_line;
  if (LINE_COLINEAR(shallow_line, steep_line) &&
      (COLINEAR(shallow_line, offset, limit) || COLINEAR(shallow_line, limit, offset))) {
    /*printf ("deleting view %x\n",it); */
    stbds_arrdel(*active_views, it - *active_views);  // Slow element removal!
    return false;
  }
  return true;
}

static void visit_coords(
    TCOD_Map* __restrict map,
    int pov_x,
    int pov_y,
    int x,
    int y,
    int dx,
    int dy,
    View*** active_views,  // Pointer to stb_ds View* array.
    View*** current_view,
    bool light_walls,
    int offset,
    int limit,
    View* views,
    ViewBumpContainer* bumps) {
  /* top left */
  const int tlx = x;
  const int tly = y + STEP_SIZE;
  /* bottom right */
  const int brx = x + STEP_SIZE;
  const int bry = y;
  View* view = NULL;
  while (*current_view != ds_array_end(*active_views)) {
    view = **current_view;
    if (!BELOW_OR_COLINEAR(&view->steep_line, brx, bry)) {
      break;
    }
    (*current_view)++;
  }
  if (*current_view == ds_array_end(*active_views) || ABOVE_OR_COLINEAR(&view->shallow_line, tlx, tly)) {
    return; /* no more active view */
  }
  if (!is_blocked(map, pov_x, pov_y, x, y, dx, dy, light_walls)) {
    return;
  }
  if (ABOVE(&view->shallow_line, brx, bry) && BELOW(&view->steep_line, tlx, tly)) {
    /* view blocked */
    stbds_arrdel(*active_views, *current_view - *active_views);  // Slow element removal!
  } else if (ABOVE(&view->shallow_line, brx, bry)) {
    /* shallow bump */
    add_shallow_bump(tlx, tly, view, bumps);
    check_view(active_views, *current_view, offset, limit);
  } else if (BELOW(&view->steep_line, tlx, tly)) {
    /* steep bump */
    add_steep_bump(brx, bry, view, bumps);
    check_view(active_views, *current_view, offset, limit);
  } else {
    /* view split */
    const int views_offset = pov_x + x * dx / STEP_SIZE + (pov_y + y * dy / STEP_SIZE) * map->width;
    View* shallower_view = &views[views_offset];
    const ptrdiff_t view_index = *current_view - *active_views;
    View** shallower_view_it;
    View** steeper_view_it;
    *shallower_view = ***current_view;
    stbds_arrins(*active_views, view_index, shallower_view);  // Slow insertion!
    shallower_view_it = *active_views + view_index;
    steeper_view_it = shallower_view_it + 1;
    *current_view = shallower_view_it;
    add_steep_bump(brx, bry, shallower_view, bumps);
    if (!check_view(active_views, shallower_view_it, offset, limit)) {
      steeper_view_it--;
    }
    add_shallow_bump(tlx, tly, *steeper_view_it, bumps);
    check_view(active_views, steeper_view_it, offset, limit);
    if (view_index > stbds_arrlen(*active_views)) {
      *current_view = ds_array_end(*active_views);
    }
  }
}

static void check_quadrant(
    TCOD_Map* __restrict map,
    int pov_x,
    int pov_y,
    int dx,
    int dy,
    int extent_x,
    int extent_y,
    bool light_walls,
    int offset,
    int limit,
    View* __restrict views,
    ViewBumpContainer* __restrict bumps) {
  bumps->count = 0;
  View** active_views = NULL;  // stb_ds View* array.
  Line shallow_line = {offset, limit, extent_x * STEP_SIZE, 0};
  Line steep_line = {limit, offset, 0, extent_y * STEP_SIZE};
  View* view = &views[pov_x + pov_y * map->width];

  view->shallow_line = shallow_line;
  view->steep_line = steep_line;
  view->shallow_bump = NULL;
  view->steep_bump = NULL;
  stbds_arrput(active_views, view);
  const int max_i = extent_x + extent_y;
  for (int i = 1; i <= max_i; ++i) {
    if (!stbds_arrlen(active_views)) {
      break;
    }
    View** current_view = active_views;
    const int start_j = MAX(i - extent_x, 0);
    const int max_j = MIN(i, extent_y);
    for (int j = start_j; j <= max_j; ++j) {
      if (!stbds_arrlen(active_views) || current_view == ds_array_end(active_views)) {
        break;
      }
      const int x = (i - j) * STEP_SIZE;
      const int y = j * STEP_SIZE;
      visit_coords(
          map, pov_x, pov_y, x, y, dx, dy, &active_views, &current_view, light_walls, offset, limit, views, bumps);
    }
  }
  stbds_arrfree(active_views);
}

TCOD_Error TCOD_map_compute_fov_permissive2(
    TCOD_Map* __restrict map, int pov_x, int pov_y, int max_radius, bool light_walls, int permissiveness) {
  if (!(0 <= permissiveness && permissiveness <= 8)) {
    TCOD_set_errorvf("Bad permissiveness %d for FOV_PERMISSIVE. Accepted range is [0,8].", permissiveness);
    return TCOD_E_INVALID_ARGUMENT;
  }
  /* Defines the parameters of the permissiveness */
  /* Derived values defining the actual part of the square used as a range. */
  int offset = 8 - permissiveness;
  int limit = 8 + permissiveness;

  if (!TCOD_map_in_bounds(map, pov_x, pov_y)) {
    TCOD_set_errorvf("Point of view {%i, %i} is out of bounds.", pov_x, pov_y);
    return TCOD_E_INVALID_ARGUMENT;
  }
  map->cells[pov_x + pov_y * map->width].fov = 1;
  /* preallocate views and bumps */
  View* views = malloc(sizeof(*views) * map->width * map->height);
  ViewBumpContainer bumps = {0, malloc(sizeof(*bumps.data) * map->width * map->height)};
  if (!views || !bumps.data) {
    free(bumps.data);
    free(views);
    TCOD_set_errorv("Out of memory.");
    return TCOD_E_OUT_OF_MEMORY;
  }
  /* set the fov range */
  int min_x = pov_x;
  int max_x = map->width - pov_x - 1;
  int min_y = pov_y;
  int max_y = map->height - pov_y - 1;
  if (max_radius > 0) {
    min_x = MIN(min_x, max_radius);
    max_x = MIN(max_x, max_radius);
    min_y = MIN(min_y, max_radius);
    max_y = MIN(max_y, max_radius);
  }
  /* calculate fov. precise permissive field of view */
  check_quadrant(map, pov_x, pov_y, 1, 1, max_x, max_y, light_walls, offset, limit, views, &bumps);
  check_quadrant(map, pov_x, pov_y, 1, -1, max_x, min_y, light_walls, offset, limit, views, &bumps);
  check_quadrant(map, pov_x, pov_y, -1, -1, min_x, min_y, light_walls, offset, limit, views, &bumps);
  check_quadrant(map, pov_x, pov_y, -1, 1, min_x, max_y, light_walls, offset, limit, views, &bumps);
  free(bumps.data);
  free(views);
  return TCOD_E_OK;
}
