/*
* libtcod 1.5.1
* Copyright (c) 2008,2009,2010 Jice & Mingos
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice or Mingos may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY JICE AND MINGOS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE OR MINGOS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
	WARNING ! This file is not part of libtcod anymore.
	This algorithm has been removed because it competes poorly against the others.
	This code does not compile with MS Visual Studio.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "libtcod.h"
#include "libtcod_int.h"
#ifdef TCOD_WINDOWS
#include <malloc.h>
#define alloca _alloca
#else
#include <alloca.h>
#endif

#define CCW(x1,y1,x2,y2,x3,y3) ((x1)*(y2) + (x2)*(y3) + (x3)*(y1) - (x1)*(y3) - (x2)*(y1) - (x3)*(y2))

void draw (map_t *m,int cx, int cy, int dis, int px, int py, bool light_walls) {
	if ((unsigned)cx >= (unsigned)m->width || (unsigned)cy >= (unsigned)m->height) return;
	if ((cx-px)*(cx-px) + (cy-py)*(cy-py) <= dis*dis + 1) {	// circular view - can be changed if you like
		if (m->cells[cx+cy*m->width].transparent || light_walls) {
			m->cells[cx+cy*m->width].fov=1;
		}
	}
}


static void trace(map_t *m,int dir, int n, int h, int px, int py, bool light_walls) {
	// convex hull of obstructions
	int *topx = alloca(n+2);
	int *topy = alloca(n+2);
	int *botx = alloca(n+2);
	int *boty = alloca(n+2);
	int cx[2];
	int cy[2];
	int curt = 0, curb = 0;	// size of top and bottom convex hulls
	int s[2][2] = {{0, 0}, {0, 0}};	// too lazy to think of real variable names, four critical points on the convex hulls - these four points determine what is visible
	int ad1,ad2[2]={0,0},eps[2] = {0, n-1},i;
	topx[0] = botx[0] = boty[0] = 0, topy[0] = 1;
	for (ad1 = 1; ad1 <= n; ++ad1) {
		for(i=0; i <2; i++) {
			eps[i] += h;	// good old Bresenham
			if (eps[i] >= n) {
				eps[i] -= n;
				++ad2[i];
			}
		}
		for(i=0; i <2; i++) if (CCW(topx[s[!i][1]], topy[s[!i][1]], botx[s[i][0]], boty[s[i][0]], ad1, ad2[i]+i) <= 0) return;	// the relevant region is no longer visible. If we don't exit the loop now, strange things happen.
		cx[0] = ad1;
		cx[1] = ad1;
		cy[0] = ad2[0];
		cy[1] = ad2[1];
		for(i=0; i <2; i++) {
			if (dir&1) cx[i] = -cx[i];
			if (dir&2) cy[i] = -cy[i];
			if (dir&4) cx[i] ^= cy[i], cy[i] ^= cx[i], cx[i] ^= cy[i];
			cx[i] += px, cy[i] += py;
			
			if (CCW(topx[s[i][1]], topy[s[i][1]], botx[s[!i][0]], boty[s[!i][0]], ad1, ad2[i]+1-i) > 0) {
				draw(m,cx[i], cy[i], n,px,py,light_walls);
			}
		}
		if ( (unsigned)cx[0] < (unsigned)m->width && (unsigned)cy[0] < (unsigned)m->height)
		if (!m->cells[m->width*cy[0]+cx[0]].transparent) {	// new obstacle, update convex hull
			++curb;
			botx[curb] = ad1, boty[curb] = ad2[0]+1;
			if (CCW(botx[s[0][0]], boty[s[0][0]], topx[s[1][1]], topy[s[1][1]], ad1, ad2[0]+1) >= 0) return;	// the obstacle obscures everything
			if (CCW(topx[s[0][1]], topy[s[0][1]], botx[s[1][0]], boty[s[1][0]], ad1, ad2[0]+1) >= 0) {
				s[1][0] = curb;	// updating visible region
				while (s[0][1] < curt && CCW(topx[s[0][1]], topy[s[0][1]], topx[s[0][1]+1], topy[s[0][1]+1], ad1, ad2[0]+1) >= 0) ++s[0][1];
			}
			while (curb > 1 && CCW(botx[curb-2], boty[curb-2], botx[curb-1], boty[curb-1], ad1, ad2[0]+1) >= 0) {	// not convex anymore, delete a point
				if (s[1][0] == curb) --s[1][0];	// s[0][0] won't be a problem
				--curb;
				botx[curb] = botx[curb+1], boty[curb] = boty[curb+1];
			}
		}
		
		if ( (unsigned)cx[1] < (unsigned)m->width && (unsigned)cy[1] < (unsigned)m->height)
		if (!m->cells[m->width*cy[1]+cx[1]].transparent) {	// same as above
			++curt;
			topx[curt] = ad1, topy[curt] = ad2[1];
			if (CCW(botx[s[1][0]], boty[s[1][0]], topx[s[0][1]], topy[s[0][1]], ad1, ad2[1]) >= 0) return;
			if (CCW(topx[s[1][1]], topy[s[1][1]], botx[s[0][0]], boty[s[0][0]], ad1, ad2[1]) >= 0) {
				s[1][1] = curt;
				while (s[0][0] < curb && CCW(botx[s[0][0]], boty[s[0][0]], botx[s[0][0]+1], boty[s[0][0]+1], ad1, ad2[1]) <= 0) ++s[0][0];
			}
			while (curt > 1 && CCW(topx[curt-2], topy[curt-2], topx[curt-1], topy[curt-1], ad1, ad2[1]) <= 0) {
				if (s[1][1] == curt) --s[1][1];
				--curt;
				topx[curt] = topx[curt+1], topy[curt] = topy[curt+1];
			}
		}
	}
}

void TCOD_map_compute_fov_digital(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls) {
	int c,r2,dir,i;
	map_t *m = (map_t *)map;
	// clean the map
	for (c=m->nbcells-1; c >= 0; c--) {
		m->cells[c].fov=0;
	}
	m->cells[player_x+player_y*m->width].fov=1;
	// set the fov range
	if ( max_radius == 0 ) {
		int max_radius_x=m->width-player_x;
		int max_radius_y=MAX(max_radius,player_y);
		max_radius_x=MAX(max_radius_x,player_x);
		max_radius_y=MAX(max_radius_y,m->height-player_y);
		max_radius = (int)(sqrt(max_radius_x*max_radius_x+max_radius_y*max_radius_y))+1;
	}
	r2=max_radius*max_radius;
	// calculate fov using digital lines
	for (dir=0; dir < 8; dir++) {
		for (i =0; i < max_radius+1; i++) {
			trace(m,dir,max_radius,i,player_x,player_y,light_walls);
		}
	}
}

