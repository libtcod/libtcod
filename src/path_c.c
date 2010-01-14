/*
* libtcod 1.5.0
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
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "libtcod.h"
#include "libtcod_int.h"
enum { NORTH_WEST,NORTH, NORTH_EAST,
               WEST,NONE,EAST,
               SOUTH_WEST,SOUTH,SOUTH_EAST };
typedef unsigned char dir_t;

/* convert dir_t to dx,dy */
static int dirx[]={-1,0,1,-1,0,1,-1,0,1};	
static int diry[]={-1,-1,-1,0,0,0,1,1,1};

typedef struct {
	int ox,oy; /* coordinates of the creature position */
	int dx,dy; /* coordinates of the creature's destination */
	TCOD_list_t path; /* list of dir_t to follow the path */
	int w,h; /* map size */
	float *grid; /* wxh djikstra distance grid (covered distance) */
	float *heur; /* wxh A* score grid (covered distance + estimated remaining distance) */
	dir_t *prev; /* wxh 'previous' grid : direction to the previous cell */
	float diagonalCost;
	TCOD_list_t heap; /* min_heap used in the algorithm. stores the offset in grid/heur (offset=x+y*w) */
	TCOD_map_t map;
	TCOD_path_func_t func;
	void *user_data;
} TCOD_path_data_t;

/* small layer on top of TCOD_list_t to implement a binary heap (min_heap) */
static void heap_sift_down(TCOD_path_data_t *path, TCOD_list_t heap) {
	/* sift-down : move the first element of the heap down to its right place */
	int cur=0;
	int end = TCOD_list_size(heap)-1;
	int child=1;
	uintptr *array=(uintptr *)TCOD_list_begin(heap);
	while ( child <= end ) {
		uint32 off_cur=array[cur];
		float cur_dist=path->heur[off_cur];
		uint32 off_child=array[child];
		float child_dist=path->heur[off_child];
		if ( child < end ) {
			/* get the min between child and child+1 */
			uintptr off_child2=array[child+1];
			float child2_dist=path->heur[off_child2];
			if ( child_dist > child2_dist ) {
				child++;
				child_dist=child2_dist;
			}
		}
		if ( child_dist < cur_dist ) {
			/* get down one level */
			uintptr tmp = array[child];
			array[child]=array[cur];
			array[cur]=tmp;
			cur=child;
		} else return;
		child=cur*2+1;
	}
}

static void heap_sift_up(TCOD_path_data_t *path, TCOD_list_t heap) {
	/* sift-up : move the last element of the heap up to its right place */
	int end = TCOD_list_size(heap)-1;
	int child=end;
	uintptr *array=(uintptr *)TCOD_list_begin(heap);
	while ( child > 0 ) {
		uintptr off_child=array[child];
		float child_dist=path->heur[off_child];
		int parent = (child-1)/2;
		uintptr off_parent=array[parent];
		float parent_dist=path->heur[off_parent];
		if ( parent_dist > child_dist ) {
			/* get up one level */
			uintptr tmp = array[child];
			array[child]=array[parent];
			array[parent]=tmp;
			child=parent;
		} else return;
	}
}

/* add a coordinate pair in the heap so that the heap root always contains the minimum A* score */
static void heap_add(TCOD_path_data_t *path, TCOD_list_t heap, int x, int y) {
	/* append the new value to the end of the heap */
	uintptr off=x+y*path->w;
	TCOD_list_push(heap,(void *)off);
	/* bubble the value up to its real position */
	heap_sift_up(path,heap);
}

/* get the coordinate pair with the minimum A* score from the heap */
static uint32 heap_get(TCOD_path_data_t *path,TCOD_list_t heap) {
	/* return the first value of the heap (minimum score) */
	uintptr *array=(uintptr *)TCOD_list_begin(heap);
	int end=TCOD_list_size(heap)-1;
	uint32 off=(uint32)(array[0]);
	/* take the last element and put it at first position (heap root) */
	array[0] = array[end];
	TCOD_list_pop(heap);
	/* and bubble it down to its real position */
	heap_sift_down(path,heap);
	return off;
}

/* this is the slow part, when we change the heuristic of a cell already in the heap */
static void heap_reorder(TCOD_path_data_t *path, uint32 offset) {
	uintptr *array=(uintptr *)TCOD_list_begin(path->heap);
	uintptr *end=(uintptr *)TCOD_list_end(path->heap);
	uintptr *cur=array;
	/* find the node corresponding to offset ... SLOW !! */
	while (cur != end) {
		if (*cur == offset ) break;
		cur++;
	}
	if ( cur == end ) return;
	/* remove it... SLOW !! */
	TCOD_list_remove_iterator(path->heap,(void **)cur);
	/* put it back on the heap */
	TCOD_list_push(path->heap,(void *)(uintptr)offset);
	/* bubble the value up to its real position */
	heap_sift_up(path,path->heap);
}


/* private functions */
static void TCOD_path_push_cell(TCOD_path_data_t *path, int x, int y);
static void TCOD_path_get_cell(TCOD_path_data_t *path, int *x, int *y, float *distance);
static void TCOD_path_set_cells(TCOD_path_data_t *path);
static float TCOD_path_walk_cost(TCOD_path_data_t *path, int xFrom, int yFrom, int xTo, int yTo);

static TCOD_path_data_t *TCOD_path_new_intern(int w, int h) {
	TCOD_path_data_t *path=(TCOD_path_data_t *)calloc(sizeof(TCOD_path_data_t),1);
	path->w=w;
	path->h=h;
	path->grid=(float *)calloc(sizeof(float),w*h);
	path->heur=(float *)calloc(sizeof(float),w*h);
	path->prev=(dir_t *)calloc(sizeof(dir_t),w*h);
	if (! path->grid || ! path->heur || ! path->prev ) {
		TCOD_fatal("Fatal error : path finding module cannot allocate djikstra grids (size %dx%d)\n",w,h);
		exit(1);
	}
	path->path=TCOD_list_new();
	path->heap=TCOD_list_new();
	return path;
}

TCOD_path_t TCOD_path_new_using_map(TCOD_map_t map, float diagonalCost) {
	TCOD_path_data_t *path=TCOD_path_new_intern(TCOD_map_get_width(map),TCOD_map_get_height(map));
	path->map=map;
	path->diagonalCost=diagonalCost;
	return (TCOD_path_t)path;
}

TCOD_path_t TCOD_path_new_using_function(int map_width, int map_height, TCOD_path_func_t func, void *user_data, float diagonalCost) {
	TCOD_path_data_t *path=TCOD_path_new_intern(map_width,map_height);
	path->func=func;
	path->user_data=user_data;
	path->diagonalCost=diagonalCost;
	return (TCOD_path_t)path;
}

bool TCOD_path_compute(TCOD_path_t p, int ox,int oy, int dx, int dy) {
	TCOD_path_data_t *path=(TCOD_path_data_t *)p;
	path->ox=ox;
	path->oy=oy;
	path->dx=dx;
	path->dy=dy;
	TCOD_list_clear(path->path);
	TCOD_list_clear(path->heap);
	if ( ox == dx && oy == dy ) return true; /* trivial case */
	/* initialize djikstra grids */
	memset(path->grid,0,sizeof(float)*path->w*path->h); 
	memset(path->prev,NONE,sizeof(dir_t)*path->w*path->h); 
	path->heur[ ox + oy * path->w ] = 1.0f; /* anything != 0 */
	TCOD_path_push_cell(path,ox,oy); /* put the origin cell as a bootstrap */
	/* fill the djikstra grid until we reach dx,dy */
	TCOD_path_set_cells(path);
	if ( path->grid[dx + dy * path->w] == 0 ) return false; /* no path found */
	/* there is a path. retrieve it */
	do {
		/* walk from destination to origin, using the 'prev' array */
		int step=path->prev[ dx + dy * path->w ];
		TCOD_list_push(path->path,(void *)(uintptr)step);
		dx -= dirx[step];
		dy -= diry[step];
	} while ( dx != ox || dy != oy );
	return true;
}	

bool TCOD_path_walk(TCOD_path_t p, int *x, int *y, bool recalculate_when_needed) {
	int newx,newy;
	float can_walk;
	int d;
	TCOD_path_data_t *path=(TCOD_path_data_t *)p;
	if ( TCOD_path_is_empty(path) ) return false;
	d=(int)(uintptr)TCOD_list_pop(path->path);
	newx=path->ox + dirx[d];
	newy=path->oy + diry[d];
	/* check if the path is still valid */
	can_walk = TCOD_path_walk_cost(path,path->ox,path->oy,newx,newy);
	if ( can_walk == 0.0f ) {
		if (! recalculate_when_needed ) return false; /* don't walk */
		/* calculate a new path */
		if (! TCOD_path_compute(path, path->ox,path->oy, path->dx,path->dy) ) return false ; /* cannot find a new path */
		return TCOD_path_walk(p,x,y,true); /* walk along the new path */
	}
	*x=newx;
	*y=newy;
	path->ox=newx;
	path->oy=newy;
	return true;
}

bool TCOD_path_is_empty(TCOD_path_t p) {
	TCOD_path_data_t *path=(TCOD_path_data_t *)p;
	return TCOD_list_is_empty(path->path);
}

int TCOD_path_size(TCOD_path_t p) {
	TCOD_path_data_t *path=(TCOD_path_data_t *)p;
	return TCOD_list_size(path->path);
}

void TCOD_path_get(TCOD_path_t p, int index, int *x, int *y) {
	int pos;
	TCOD_path_data_t *path=(TCOD_path_data_t *)p;
	*x=path->ox;
	*y=path->oy;
	pos = TCOD_list_size(path->path)-1;
	do {
		int step=(int)(uintptr)TCOD_list_get(path->path,pos);
		*x += dirx[step];
		*y += diry[step];
		pos--;index--;
	} while (index >= 0);
}

void TCOD_path_delete(TCOD_path_t p) {
	TCOD_path_data_t *path=(TCOD_path_data_t *)p;
	free(path->grid);
	free(path->heur);
	free(path->prev);
	TCOD_list_delete(path->path);	
	TCOD_list_delete(path->heap);
	free(path);
}

/* private stuff */
/* add a new unvisited cells to the cells-to-treat list 
 * the list is in fact a min_heap. Cell at index i has its sons at 2*i+1 and 2*i+2
 */
static void TCOD_path_push_cell(TCOD_path_data_t *path, int x, int y) {
	heap_add(path,path->heap,x,y);
}

/* get the best cell from the heap */
static void TCOD_path_get_cell(TCOD_path_data_t *path, int *x, int *y, float *distance) {
	uint32 offset = heap_get(path,path->heap);
	*x=(offset % path->w);
	*y=(offset / path->w);
	*distance=path->grid[offset];
//printf ("get cell : %d %d %g\n",*x,*y,*distance);
}
/* fill the grid, starting from the origin until we reach the destination */
static void TCOD_path_set_cells(TCOD_path_data_t *path) {
	while ( path->grid[path->dx + path->dy * path->w ] == 0 && ! TCOD_list_is_empty(path->heap) ) {
		
		int x,y,i,imax;
		float distance;
		TCOD_path_get_cell(path,&x,&y,&distance);
		imax= ( path->diagonalCost == 0.0f ? 4 : 8) ;
		for (i=0; i < imax; i++ ) {
			/* convert i to dx,dy */
			static int idirx[]={0,-1,1,0,-1,1,-1,1};	
			static int idiry[]={-1,0,0,1,-1,-1,1,1};
			/* convert i to direction */
			static dir_t prevdirs[] = {
				NORTH, WEST, EAST, SOUTH, NORTH_WEST, NORTH_EAST,SOUTH_WEST,SOUTH_EAST
			};
			/* coordinate of the adjacent cell */
			int cx=x+idirx[i];
			int cy=y+idiry[i];
			if ( cx >= 0 && cy >= 0 && cx < path->w && cy < path->h ) {
				float walk_cost = TCOD_path_walk_cost(path,x,y,cx,cy);
				if ( walk_cost > 0.0f ) {
					/* in of the map and walkable */
					float covered=distance + walk_cost * (i>=4 ? path->diagonalCost : 1.0f);
					float previousCovered = path->grid[cx + cy * path->w ];
					if ( previousCovered == 0 ) {
						/* put a new cell in the heap */
						int offset=cx + cy * path->w;
						/* A* heuristic : remaining distance */
						float remaining=sqrtf((cx-path->dx)*(cx-path->dx)+(cy-path->dy)*(cy-path->dy));
						path->grid[ offset ] = covered;
						path->heur[ offset ] = covered + remaining;
						path->prev[ offset ] =  prevdirs[i];
	//printf ("new cell: %d %d %g (prev %g / rem %g)\n",cx,cy,path->heur[ offset ],distance + dist[i],remaining);
						TCOD_path_push_cell(path,cx,cy);
					} else if ( previousCovered > covered ) {
						/* we found a better path to a cell already in the heap */
						int offset=cx + cy * path->w;
						path->grid[ offset ] = covered;
						path->heur[ offset ] -= (previousCovered - covered); /* fix the A* score */
						path->prev[ offset ] =  prevdirs[i];
						/* reorder the heap */
						heap_reorder(path,offset);
					}
				}
			}
		}
	}	
}

/* check if a cell is walkable (from the pathfinder point of view) */
static float TCOD_path_walk_cost(TCOD_path_data_t *path, int xFrom, int yFrom, int xTo, int yTo) {
	if ( path->map ) return TCOD_map_is_walkable(path->map,xTo,yTo) ? 1.0f : 0.0f;
	return path->func(xFrom,yFrom,xTo,yTo,path->user_data);
}

void TCOD_path_get_origin(TCOD_path_t p, int *x, int *y) {
	TCOD_path_data_t *path=(TCOD_path_data_t *)p;
	*x=path->ox;
	*y=path->oy;
}

void TCOD_path_get_destination(TCOD_path_t p, int *x, int *y) {
	TCOD_path_data_t *path=(TCOD_path_data_t *)p;
	*x=path->dx;
	*y=path->dy;
}


