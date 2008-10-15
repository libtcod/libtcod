/*
* libtcod 1.4.0
* Copyright (c) 2008 J.C.Wilk
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of J.C.Wilk may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY J.C.WILK ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL J.C.WILK BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>
#include "libtcod.h"
#include <string.h>

typedef struct {
	unsigned int transparent:1;
	unsigned int walkable:1;
	unsigned int in_fov:1;
} cell_t;

typedef struct {
	int width;
	int height;
	int nbcells;
	cell_t *cells;
} map_t;

TCODLIB_API TCOD_map_t TCOD_map_new(int width, int height) {
	map_t *map=(map_t *)calloc(sizeof(map_t),1);
	map->width=width;
	map->height=height;
	map->nbcells=width*height;
	map->cells=(cell_t *)calloc(sizeof(cell_t),map->nbcells);
	return map;
}

TCODLIB_API void TCOD_map_clear(TCOD_map_t map) {
	map_t *m = (map_t *)map;
	memset(m->cells,0,sizeof(cell_t)*m->width*m->height);
}

TCODLIB_API void TCOD_map_set_properties(TCOD_map_t map, int x, int y, bool is_transparent, bool is_walkable) {
	map_t *m = (map_t *)map;
	m->cells[x+y*m->width].transparent=is_transparent;
	m->cells[x+y*m->width].walkable=is_walkable;
}

TCODLIB_API void TCOD_map_delete(TCOD_map_t map) {
	map_t *m = (map_t *)map;
	free(m->cells);
	free(m);
}

static void cast_ray(map_t *map, int xo, int yo, int xd, int yd, int r2) {
	int curx=xo,cury=yo;
	bool in=false;
	bool blocked=false;
	bool end=false;
	int offset;
	TCOD_line_init(xo,yo,xd,yd);
	offset=curx+cury*map->width;
	if ( 0 <= offset && offset < map->nbcells ) {
		in=true;
		map->cells[offset].in_fov=1;
	}
	while (!end) {
		end = TCOD_line_step(&curx,&cury);	// reached xd,yd
		offset=curx+cury*map->width;
		if ( r2 > 0 ) {
			// check radius
			int cur_radius=(curx-xo)*(curx-xo)+(cury-yo)*(cury-yo);
			if ( cur_radius > r2 ) return;
		}
		if ( 0 <= offset && offset < map->nbcells ) {
			in=true;
			if ( !blocked && ! map->cells[offset].transparent ) {
				blocked=true;
			} else if ( blocked ) {
					return; // wall
			}
			map->cells[offset].in_fov=1;
		} else if (in) return; // ray out of map
	}
}

static void postproc(map_t *map,int x0,int y0, int x1, int y1, int dx, int dy) {
	int cx,cy;
	for (cx=x0; cx <= x1; cx++) {
		for (cy=y0;cy <= y1; cy ++ ) {
			int x2 = cx+dx;
			int y2 = cy+dy;
			unsigned int offset=cx+cy*map->width;
			if ( offset < (unsigned)map->nbcells && map->cells[offset].in_fov && map->cells[offset].walkable ) {
				if ( x2 >= x0 && x2 <= x1 ) {
					unsigned int offset2=x2+cy*map->width;
					if ( offset2 < (unsigned)map->nbcells && ! map->cells[offset2].walkable )
						map->cells[offset2].in_fov=1;
				}
				if ( y2 >= y0 && y2 <= y1 ) {
					unsigned int offset2=cx+y2*map->width;
					if ( offset2 < (unsigned)map->nbcells && ! map->cells[offset2].walkable )
						map->cells[offset2].in_fov=1;
				}
				if ( x2 >= x0 && x2 <= x1 && y2 >= y0 && y2 <= y1 ) {
					unsigned int offset2=x2+y2*map->width;
					if ( offset2 < (unsigned)map->nbcells && ! map->cells[offset2].walkable )
						map->cells[offset2].in_fov=1;
				}
			}
		}
	}
}

TCODLIB_API void TCOD_map_compute_fov(TCOD_map_t map, int player_x, int player_y, int max_radius) {
	map_t *m = (map_t *)map;
	// circular ray casting
	int xmin=0, ymin=0, xmax=m->width, ymax=m->height;
	int xo,yo,c;
	int r2=max_radius*max_radius;
	if ( max_radius > 0 ) {
		xmin=MAX(0,player_x-max_radius);
		ymin=MAX(0,player_y-max_radius);
		xmax=MIN(m->width,player_x+max_radius+1);
		ymax=MIN(m->height,player_y+max_radius+1);
	}
	for (c=m->nbcells-1; c >= 0; c--) {
		m->cells[c].in_fov=0;
	}
	xo=xmin; yo=ymin;
	while ( xo < xmax ) {
		cast_ray(m,player_x,player_y,xo++,yo,r2);
	}
	xo=xmax-1;yo=ymin+1;
	while ( yo < ymax ) {
		cast_ray(m,player_x,player_y,xo,yo++,r2);
	}
	yo=ymax-1;xo=xmax-2;
	while ( xo >= 0 ) {
		cast_ray(m,player_x,player_y,xo--,yo,r2);
	}
	xo=xmin;yo=ymax-2;
	while ( yo > 0 ) {
		cast_ray(m,player_x,player_y,xo,yo--,r2);
	}
	// post-processing artefact fix
	postproc(m,xmin,ymin,player_x,player_y,-1,-1);
	postproc(m,player_x,ymin,xmax-1,player_y,1,-1);
	postproc(m,xmin,player_y,player_x,ymax-1,-1,1);
	postproc(m,player_x,player_y,xmax-1,ymax-1,1,1);
}

TCODLIB_API bool TCOD_map_is_in_fov(TCOD_map_t map, int x, int y) {
	map_t *m = (map_t *)map;
	return m->cells[x+y*m->width].in_fov;
}
TCODLIB_API bool TCOD_map_is_transparent(TCOD_map_t map, int x, int y) {
	map_t *m = (map_t *)map;
	return m->cells[x+y*m->width].transparent;
}

TCODLIB_API bool TCOD_map_is_walkable(TCOD_map_t map, int x, int y) {
	map_t *m = (map_t *)map;
	return m->cells[x+y*m->width].walkable;
}
TCODLIB_API int TCOD_map_get_width(TCOD_map_t map) {
	map_t *m = (map_t *)map;
	return m->width;
}

TCODLIB_API int TCOD_map_get_height(TCOD_map_t map) {
	map_t *m = (map_t *)map;
	return m->height;
}

