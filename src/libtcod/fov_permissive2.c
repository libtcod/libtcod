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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fov.h"
#include "libtcod_int.h"
#include "utility.h"

/* The size of each square in units */
#define STEP_SIZE 16

/* Jonathon Duerig enhanced permissive FOV */
typedef struct Line {
  int xi, yi, xf, yf;
} Line;

typedef struct ViewBump {
  int x, y;
  struct ViewBump* parent;
} ViewBump;

typedef struct View {
  Line shallow_line;
  Line steep_line;
  ViewBump* shallow_bump;
  ViewBump* steep_bump;
} View;

typedef struct ViewBumpContainer {
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

typedef struct ActiveViewArray {  // Active view container.
  ptrdiff_t count;  // Number of elements in the array.
  View** __restrict view_ptrs;  // Preallocated array of pointers to View*.
} ActiveViewArray;

/// @brief Push a view onto the active view array.
static void view_array_push(ActiveViewArray* view_array, View* view_ptr) {
  view_array->view_ptrs[view_array->count++] = view_ptr;
}

/// @brief Remove a view from the active views and shift the remaining elements.
static void view_array_remove(ActiveViewArray* view_array, ptrdiff_t index) {
  for (ptrdiff_t i = index; i < view_array->count - 1; ++i) view_array->view_ptrs[i] = view_array->view_ptrs[i + 1];
  --view_array->count;
}

/// @brief Insert a view into the active views array.
static void view_array_insert(ActiveViewArray* view_array, ptrdiff_t index, View* view_ptr) {
  ++view_array->count;
  for (ptrdiff_t i = view_array->count - 1; i >= index; --i) view_array->view_ptrs[i + 1] = view_array->view_ptrs[i];
  view_array->view_ptrs[index] = view_ptr;
}

/// @brief Return a past-the-end pointer for the active view array.
static View** view_array_end(const ActiveViewArray* view_array) { return view_array->view_ptrs + view_array->count; }

/// @brief Set a maps FOV-bit and return true if the tile is blocked.
static bool is_blocked(TCOD_Map* map, int pov_x, int pov_y, int x, int y, int dx, int dy, bool light_walls) {
  const int pos_x = x * dx / STEP_SIZE + pov_x;
  const int pos_y = y * dy / STEP_SIZE + pov_y;
  const int cells_offset = pos_x + pos_y * map->width;
  const bool blocked = !map->cells[cells_offset].transparent;
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

static bool check_view(ActiveViewArray* active_views, View** it, int offset, int limit) {
  const View* view = *it;
  const Line* shallow_line = &view->shallow_line;
  const Line* steep_line = &view->steep_line;
  if (LINE_COLINEAR(shallow_line, steep_line) &&
      (COLINEAR(shallow_line, offset, limit) || COLINEAR(shallow_line, limit, offset))) {
    /*printf ("deleting view %x\n",it); */
    view_array_remove(active_views, it - active_views->view_ptrs);  // Slow element removal!
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
    ActiveViewArray* active_views,
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
  while (*current_view != view_array_end(active_views)) {
    view = **current_view;
    if (!BELOW_OR_COLINEAR(&view->steep_line, brx, bry)) {
      break;
    }
    (*current_view)++;
  }
  if (*current_view == view_array_end(active_views) || ABOVE_OR_COLINEAR(&view->shallow_line, tlx, tly)) {
    return; /* no more active view */
  }
  if (!is_blocked(map, pov_x, pov_y, x, y, dx, dy, light_walls)) {
    return;
  }
  if (ABOVE(&view->shallow_line, brx, bry) && BELOW(&view->steep_line, tlx, tly)) {
    /* view blocked */
    view_array_remove(active_views, *current_view - active_views->view_ptrs);  // Slow element removal!
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
    const ptrdiff_t view_index = *current_view - active_views->view_ptrs;
    View** shallower_view_it;
    View** steeper_view_it;
    *shallower_view = ***current_view;
    view_array_insert(active_views, view_index, shallower_view);  // Slow insertion!
    shallower_view_it = active_views->view_ptrs + view_index;
    steeper_view_it = shallower_view_it + 1;
    *current_view = shallower_view_it;
    add_steep_bump(brx, bry, shallower_view, bumps);
    if (!check_view(active_views, shallower_view_it, offset, limit)) {
      steeper_view_it--;
    }
    add_shallow_bump(tlx, tly, *steeper_view_it, bumps);
    check_view(active_views, steeper_view_it, offset, limit);
    if (view_index > active_views->count) {
      *current_view = view_array_end(active_views);
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
    ViewBumpContainer* __restrict bumps,
    ActiveViewArray* __restrict active_views) {
  // Reset temporary data storage arrays
  bumps->count = 0;
  active_views->count = 0;

  const Line shallow_line = {offset, limit, extent_x * STEP_SIZE, 0};
  const Line steep_line = {limit, offset, 0, extent_y * STEP_SIZE};
  View* view = &views[pov_x + pov_y * map->width];

  view->shallow_line = shallow_line;
  view->steep_line = steep_line;
  view->shallow_bump = NULL;
  view->steep_bump = NULL;
  view_array_push(active_views, view);
  const int max_i = extent_x + extent_y;
  for (int i = 1; i <= max_i; ++i) {
    if (!active_views->count) {
      break;
    }
    View** current_view = active_views->view_ptrs;
    const int start_j = TCOD_MAX(i - extent_x, 0);
    const int max_j = TCOD_MIN(i, extent_y);
    for (int j = start_j; j <= max_j; ++j) {
      if (!active_views->count || current_view == view_array_end(active_views)) {
        break;
      }
      const int x = (i - j) * STEP_SIZE;
      const int y = j * STEP_SIZE;
      visit_coords(
          map, pov_x, pov_y, x, y, dx, dy, active_views, &current_view, light_walls, offset, limit, views, bumps);
    }
  }
}

TCOD_Error TCOD_map_compute_fov_permissive2(
    TCOD_Map* __restrict map, int pov_x, int pov_y, int max_radius, bool light_walls, int permissiveness) {
  if (!(0 <= permissiveness && permissiveness <= 8)) {
    TCOD_set_errorvf("Bad permissiveness %d for FOV_PERMISSIVE. Accepted range is [0,8].", permissiveness);
    return TCOD_E_INVALID_ARGUMENT;
  }
  /* Defines the parameters of the permissiveness */
  /* Derived values defining the actual part of the square used as a range. */
  const int offset = 8 - permissiveness;
  const int limit = 8 + permissiveness;

  if (!TCOD_map_in_bounds(map, pov_x, pov_y)) {
    TCOD_set_errorvf("Point of view {%i, %i} is out of bounds.", pov_x, pov_y);
    return TCOD_E_INVALID_ARGUMENT;
  }
  map->cells[pov_x + pov_y * map->width].fov = 1;

  // Preallocate views and bumps, assuming there will be no more bumps or active views than the number of map tiles.
  View* views = malloc(map->width * map->height * sizeof(*views));
  ViewBumpContainer bumps = {.data = malloc(map->width * map->height * sizeof(*bumps.data))};
  ActiveViewArray active_views = {.view_ptrs = malloc(map->width * map->height * sizeof(*active_views.view_ptrs))};
  if (!views || !bumps.data || !active_views.view_ptrs) {
    free(bumps.data);
    free(views);
    free(active_views.view_ptrs);
    TCOD_set_errorv("Out of memory.");
    return TCOD_E_OUT_OF_MEMORY;
  }
  /* set the fov range */
  int min_x = pov_x;
  int max_x = map->width - pov_x - 1;
  int min_y = pov_y;
  int max_y = map->height - pov_y - 1;
  if (max_radius > 0) {
    min_x = TCOD_MIN(min_x, max_radius);
    max_x = TCOD_MIN(max_x, max_radius);
    min_y = TCOD_MIN(min_y, max_radius);
    max_y = TCOD_MIN(max_y, max_radius);
  }
  /* calculate fov. precise permissive field of view */
  check_quadrant(map, pov_x, pov_y, 1, 1, max_x, max_y, light_walls, offset, limit, views, &bumps, &active_views);
  check_quadrant(map, pov_x, pov_y, 1, -1, max_x, min_y, light_walls, offset, limit, views, &bumps, &active_views);
  check_quadrant(map, pov_x, pov_y, -1, -1, min_x, min_y, light_walls, offset, limit, views, &bumps, &active_views);
  check_quadrant(map, pov_x, pov_y, -1, 1, min_x, max_y, light_walls, offset, limit, views, &bumps, &active_views);
  free(bumps.data);
  free(views);
  free(active_views.view_ptrs);
  return TCOD_E_OK;
}
