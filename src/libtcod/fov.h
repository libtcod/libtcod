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
#pragma once
#ifndef TCOD_FOV_H_
#define TCOD_FOV_H_

#include <stdbool.h>
#ifdef __cplusplus
#include <memory>
#endif  // __cplusplus
#include "config.h"
#include "error.h"
#include "fov_types.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
    Return a new TCOD_Map with `width` and `height`.
 */
TCOD_PUBLIC TCOD_Map* TCOD_map_new(int width, int height);
/**
    Set all cell values on `map` to the given parameters.

    This call also zeroes out the field-of-view attribute.
 */
TCOD_PUBLIC void TCOD_map_clear(TCOD_Map* map, bool transparent, bool walkable);
/**
    Clone map data from `source` to `dest`.

    `dest` will be resized to match `source` if necessary.
 */
TCOD_PUBLIC TCOD_Error TCOD_map_copy(const TCOD_Map* __restrict source, TCOD_Map* __restrict dest);
/**
    Change the properties of a single cell.
 */
TCOD_PUBLIC void TCOD_map_set_properties(TCOD_Map* map, int x, int y, bool is_transparent, bool is_walkable);
/**
    Free a TCOD_Map object.
 */
TCOD_PUBLIC void TCOD_map_delete(TCOD_Map* map);
/**
    Calculate the field-of-view.

    \rst
    `pov_x` and `pov_y` are the used as the field-of-view source.
    These coordinates must be within the map.

    `max_radius` is the maximum distance for the field-of-view algorithm.

    If `light_walls` is false then only transparent cells will be touched by
    the field-of-view.

    `algo` is one of the :any:`TCOD_fov_algorithm_t` algorithms.

    After this call you may check if a cell is within the field-of-view by
    calling :any:`TCOD_map_is_in_fov`.

    Returns an error code on failure.  See :any:`TCOD_get_error` for details.
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_map_compute_fov(
    TCOD_Map* __restrict map, int pov_x, int pov_y, int max_radius, bool light_walls, TCOD_fov_algorithm_t algo);
/**
    Return true if this cell was touched by the current field-of-view.
 */
TCOD_PUBLIC bool TCOD_map_is_in_fov(const TCOD_Map* map, int x, int y);
/**
    Set the fov flag on a specific cell.
 */
TCOD_PUBLIC void TCOD_map_set_in_fov(TCOD_Map* map, int x, int y, bool fov);
/**
    Return true if this cell is transparent.
 */
TCOD_PUBLIC bool TCOD_map_is_transparent(const TCOD_Map* map, int x, int y);
/**
    Return true if this cell is walkable.
 */
TCOD_PUBLIC bool TCOD_map_is_walkable(TCOD_Map* map, int x, int y);
/**
    Return the width of `map`.
 */
TCOD_PUBLIC int TCOD_map_get_width(const TCOD_Map* map);
/**
    Return the height of `map`.
 */
TCOD_PUBLIC int TCOD_map_get_height(const TCOD_Map* map);
/**
    Return the total number of cells in `map`.
 */
TCOD_PUBLIC int TCOD_map_get_nb_cells(const TCOD_Map* map);
#ifdef __cplusplus
}  // extern "C"
namespace tcod {
struct MapDeleter_ {
  void operator()(TCOD_Map* map) const { TCOD_map_delete(map); }
};
typedef std::unique_ptr<struct TCOD_Map, MapDeleter_> MapPtr_;
}  // namespace tcod
#endif  // __cplusplus
#endif  // TCOD_FOV_H_
