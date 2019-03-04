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
#ifndef _TCOD_FOV_H
#define _TCOD_FOV_H

#include "portability.h"
#include "fov_types.h"

#ifdef __cplusplus
extern "C" {
#endif
/* allocate a new map */
TCODLIB_API TCOD_Map *TCOD_map_new(int width, int height);
/* set all cells as solid rock (cannot see through nor walk) */
TCODLIB_API void TCOD_map_clear(TCOD_Map *map,
                                bool transparent, bool walkable);
/* copy a map to another, reallocating it when needed */
TCODLIB_API void TCOD_map_copy(const TCOD_Map *source, TCOD_Map *dest);
/* change a cell properties */
TCODLIB_API void TCOD_map_set_properties(
    TCOD_Map *map, int x, int y, bool is_transparent, bool is_walkable);
/* destroy a map */
TCODLIB_API void TCOD_map_delete(TCOD_Map *map);

/* calculate the field of view (potentially visible cells from player_x,player_y) */
TCODLIB_API void TCOD_map_compute_fov(
    TCOD_Map *map, int player_x, int player_y, int max_radius,
    bool light_walls, TCOD_fov_algorithm_t algo);
/* check if a cell is in the last computed field of view */
TCODLIB_API bool TCOD_map_is_in_fov(const TCOD_Map *map, int x, int y);
TCODLIB_API void TCOD_map_set_in_fov(TCOD_Map *map, int x, int y, bool fov);

/* retrieve properties from the map */
TCODLIB_API bool TCOD_map_is_transparent(const TCOD_Map *map, int x, int y);
TCODLIB_API bool TCOD_map_is_walkable(TCOD_Map *map, int x, int y);
TCODLIB_API int TCOD_map_get_width(const TCOD_Map *map);
TCODLIB_API int TCOD_map_get_height(const TCOD_Map *map);
TCODLIB_API int TCOD_map_get_nb_cells(const TCOD_Map *map);
#ifdef __cplusplus
}
#endif
#endif
