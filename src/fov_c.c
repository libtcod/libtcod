/*
* libtcod 1.4.3
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

#include <stdlib.h>
#include "libtcod.h"
#include "libtcod_int.h"
#include <string.h>

TCOD_map_t TCOD_map_new(int width, int height) {
	map_t *map=(map_t *)calloc(sizeof(map_t),1);
	map->width=width;
	map->height=height;
	map->nbcells=width*height;
	map->cells=(cell_t *)calloc(sizeof(cell_t),map->nbcells);
	return map;
}

TCODLIB_API void TCOD_map_copy(TCOD_map_t source, TCOD_map_t dest) {
	map_t *source_int = (map_t *)source;
	map_t *dest_int = (map_t *)dest;
	if ( dest_int->nbcells != source_int->nbcells ) {
		free(dest_int->cells);
		dest_int->cells=(cell_t *)malloc(sizeof(cell_t)*dest_int->nbcells);
	}
	dest_int->width=source_int->width;
	dest_int->height=source_int->height;
	dest_int->nbcells=source_int->nbcells;
	memcpy(dest_int->cells, source_int->cells, sizeof(cell_t) * source_int->nbcells);
}

void TCOD_map_clear(TCOD_map_t map) {
	map_t *m = (map_t *)map;
	memset(m->cells,0,sizeof(cell_t)*m->width*m->height);
}

void TCOD_map_set_properties(TCOD_map_t map, int x, int y, bool is_transparent, bool is_walkable) {
	map_t *m = (map_t *)map;
	m->cells[x+y*m->width].transparent=is_transparent;
	m->cells[x+y*m->width].walkable=is_walkable;
}

void TCOD_map_delete(TCOD_map_t map) {
	map_t *m = (map_t *)map;
	free(m->cells);
	free(m);
}

void TCOD_map_compute_fov(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls, TCOD_fov_algorithm_t algo) {
  switch(algo) {
    case FOV_BASIC : TCOD_map_compute_fov_circular_raycasting(map,player_x,player_y,max_radius,light_walls); break;
    case FOV_DIAMOND : TCOD_map_compute_fov_diamond_raycasting(map,player_x,player_y,max_radius,light_walls); break;
    case FOV_SHADOW : TCOD_map_compute_fov_recursive_shadowcasting(map,player_x,player_y,max_radius,light_walls); break;
    case FOV_PERMISSIVE_0 : 
    case FOV_PERMISSIVE_1 : 
    case FOV_PERMISSIVE_2 : 
    case FOV_PERMISSIVE_3 : 
    case FOV_PERMISSIVE_4 : 
    case FOV_PERMISSIVE_5 : 
    case FOV_PERMISSIVE_6 : 
    case FOV_PERMISSIVE_7 : 
    case FOV_PERMISSIVE_8 : 
	TCOD_map_compute_fov_permissive2(map,player_x,player_y,max_radius,light_walls, algo-FOV_PERMISSIVE_0); 
	break;
	case FOV_RESTRICTIVE : TCOD_map_compute_fov_restrictive_shadowcasting(map,player_x,player_y,max_radius,light_walls); break;
    default:break;
  }
}

bool TCOD_map_is_in_fov(TCOD_map_t map, int x, int y) {
	map_t *m = (map_t *)map;
	return m->cells[x+y*m->width].fov == 1;
}

void TCOD_map_set_in_fov(TCOD_map_t map, int x, int y, bool fov) {
	map_t *m = (map_t *)map;
	m->cells[x+y*m->width].fov = fov ? 1:0;
}

bool TCOD_map_is_transparent(TCOD_map_t map, int x, int y) {
	map_t *m = (map_t *)map;
	return m->cells[x+y*m->width].transparent;
}

bool TCOD_map_is_walkable(TCOD_map_t map, int x, int y) {
	map_t *m = (map_t *)map;
	return m->cells[x+y*m->width].walkable;
}
int TCOD_map_get_width(TCOD_map_t map) {
	map_t *m = (map_t *)map;
	return m->width;
}

int TCOD_map_get_height(TCOD_map_t map) {
	map_t *m = (map_t *)map;
	return m->height;
}

int TCOD_map_get_nb_cells(TCOD_map_t map) {
	map_t *m = (map_t *)map;
	return m->nbcells;
}

