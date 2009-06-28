/*
* libtcod 1.4.2
* Copyright (c) 2008,2009 Jice
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Jice ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Jice BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _TCOD_FOV_H
#define _TCOD_FOV_H

typedef void *TCOD_map_t;

// FOV_BASIC : http://roguebasin.roguelikedevelopment.org/index.php?title=Ray_casting
// FOV_DIAMOND : http://www.geocities.com/temerra/los_rays.html
// FOV_SHADOW : http://roguebasin.roguelikedevelopment.org/index.php?title=FOV_using_recursive_shadowcasting
// FOV_PERMISSIVE : http://roguebasin.roguelikedevelopment.org/index.php?title=Precise_Permissive_Field_of_View
// FOV_RESTRICTIVE : Mingos' Restrictive Precise Angle Shadowcasting (contribution by Mingos)

typedef enum {
	FOV_BASIC,
	FOV_DIAMOND,
	FOV_SHADOW,
	FOV_PERMISSIVE_0,
	FOV_PERMISSIVE_1,
	FOV_PERMISSIVE_2,
	FOV_PERMISSIVE_3,
	FOV_PERMISSIVE_4,
	FOV_PERMISSIVE_5,
	FOV_PERMISSIVE_6,
	FOV_PERMISSIVE_7,
	FOV_PERMISSIVE_8,
	FOV_RESTRICTIVE,
	NB_FOV_ALGORITHMS } TCOD_fov_algorithm_t;
#define FOV_PERMISSIVE(x) ((TCOD_fov_algorithm_t)(FOV_PERMISSIVE_0 + (x)))

// allocate a new map
TCODLIB_API TCOD_map_t TCOD_map_new(int width, int height);
// set all cells as solid rock (cannot see through nor walk)
TCODLIB_API void TCOD_map_clear(TCOD_map_t map);
// copy a map to another, reallocating it when needed
TCODLIB_API void TCOD_map_copy(TCOD_map_t source, TCOD_map_t dest);
// change a cell properties
TCODLIB_API void TCOD_map_set_properties(TCOD_map_t map, int x, int y, bool is_transparent, bool is_walkable);
// destroy a map
TCODLIB_API void TCOD_map_delete(TCOD_map_t map);

// calculate the field of view (potentially visible cells from player_x,player_y)
TCODLIB_API void TCOD_map_compute_fov(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls, TCOD_fov_algorithm_t algo);
// check if a cell is in the last computed field of view
TCODLIB_API bool TCOD_map_is_in_fov(TCOD_map_t map, int x, int y);
TCODLIB_API void TCOD_map_set_in_fov(TCOD_map_t map, int x, int y, bool fov);

// retrieve properties from the map
TCODLIB_API bool TCOD_map_is_transparent(TCOD_map_t map, int x, int y);
TCODLIB_API bool TCOD_map_is_walkable(TCOD_map_t map, int x, int y);
TCODLIB_API int TCOD_map_get_width(TCOD_map_t map);
TCODLIB_API int TCOD_map_get_height(TCOD_map_t map);
TCODLIB_API int TCOD_map_get_nb_cells(TCOD_map_t map);
#endif
