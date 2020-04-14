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
#include "heightmap.h"

#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "utility.h"
#include "mersenne.h"

#define GET_VALUE(hm,x,y) (hm)->values[(x)+(y)*(hm)->w]
/**
    Returns true if `x`,`y` are valid coordinates for this heightmap.
 */
static bool in_bounds(const TCOD_heightmap_t *hm, int x, int y) {
	if (!hm) { return false; } // No valid coordinates on a NULL pointer.
	if (x < 0 || x >= hm->w) return false;
	if (y < 0 || y >= hm->h) return false;
	return true;
}
/**
    Returns true if these heighmaps have the same shape and are non-NULL.
 */
static bool is_same_size(const TCOD_heightmap_t *hm1, const TCOD_heightmap_t *hm2) {
	return hm1 && hm2 && hm1->w == hm2->w && hm1->h == hm2->h;
}

TCOD_heightmap_t *TCOD_heightmap_new(int w,int h) {
	TCOD_heightmap_t *hm=malloc(sizeof(*hm));
	hm->values = calloc(sizeof(*hm->values),w*h);
	hm->w=w;
	hm->h=h;
	return hm;
}

void TCOD_heightmap_delete(TCOD_heightmap_t *hm) {
	free(hm->values);
	free(hm);
}

void TCOD_heightmap_clear(TCOD_heightmap_t *hm) {
	memset(hm->values,0,hm->w*hm->h*sizeof(float));
}

float TCOD_heightmap_get_value(const TCOD_heightmap_t *hm, int x, int y) {
	if (in_bounds(hm, x, y)) {
		return GET_VALUE(hm, x, y);
	} else {
		return 0.0;
	}
}

void TCOD_heightmap_set_value(TCOD_heightmap_t *hm, int x, int y, float value) {
	if (in_bounds(hm, x, y)) {
		GET_VALUE(hm, x, y) = value;
	}
}

void TCOD_heightmap_get_minmax(const TCOD_heightmap_t *hm, float *min, float *max) {
	if (!in_bounds(hm, 0, 0)) { return; }
	if (min) { *min = hm->values[0]; }
	if (max) { *max = hm->values[0]; }
	for (int i=0; i != hm->h*hm->w; i++) {
		const float value = hm->values[i];
		if (min) { *min = MIN(*min, value); }
		if (max) { *max = MAX(*max, value); }
	}
}

void TCOD_heightmap_normalize(TCOD_heightmap_t *hm, float min, float max) {
	float curmin,curmax;
	TCOD_heightmap_get_minmax(hm,&curmin,&curmax);

	if (curmax - curmin < FLT_EPSILON) {
		for (int i = 0; i != hm->w*hm->h; ++i) {
			hm->values[i] = min;
		}
	} else {
		const float invmax = (max - min) / (curmax - curmin);
		for (int i = 0; i != hm->w*hm->h; ++i) {
			hm->values[i] = min + (hm->values[i] - curmin) * invmax;
		}
	}
}

void TCOD_heightmap_add_hill(TCOD_heightmap_t *hm, float hx, float hy, float hradius, float hheight) {
	const float hradius2=hradius*hradius;
	const float coef=hheight / hradius2;
	const int minx=MAX(floorf(hx-hradius),0);
	const int miny=MAX(floorf(hy-hradius),0);
	const int maxx=MIN(ceilf(hx+hradius),hm->w);
	const int maxy=MIN(ceilf(hy+hradius),hm->h);
	for (int y = miny; y < maxy; y++) {
		const float ydist = (y - hy)*(y - hy);
		for (int x = minx; x < maxx; x++) {
			const float xdist = (x - hx)*(x - hx);
			const float z = hradius2 - xdist - ydist;
			if (z > 0) { GET_VALUE(hm, x, y) += z * coef; }
		}
	}
}

void TCOD_heightmap_dig_hill(TCOD_heightmap_t *hm, float hx, float hy, float hradius, float hheight) {
	const float hradius2=hradius*hradius;
	const float coef=hheight / hradius2;
	const int minx=MAX(floorf(hx-hradius),0);
	const int miny=MAX(floorf(hy-hradius),0);
	const int maxx=MIN(ceilf(hx+hradius),hm->w);
	const int maxy=MIN(ceilf(hy+hradius),hm->h);
	for (int y = miny; y < maxy; y++) {
		for (int x = minx; x < maxx; x++) {
			const float xdist = (x - hx)*(x - hx);
			const float ydist = (y - hy)*(y - hy);
			const float dist = xdist + ydist;
			if ( dist < hradius2 ) {
				const float z = (hradius2 - dist) * coef;
				if (hheight > 0) {
					if ( GET_VALUE(hm,x,y) < z ) GET_VALUE(hm,x,y) = z;
				} else {
					if ( GET_VALUE(hm,x,y) > z ) GET_VALUE(hm,x,y) = z;
				}
			}
		}
	}
}

void TCOD_heightmap_copy(const TCOD_heightmap_t *hm_source,TCOD_heightmap_t *hm_dest) {
	if ( hm_source->w != hm_dest->w || hm_source->h != hm_dest->h) return;
	memcpy(hm_dest->values,hm_source->values,sizeof(float)*hm_source->w*hm_source->h);
}

void TCOD_heightmap_add_fbm(TCOD_heightmap_t *hm, TCOD_noise_t noise,float mulx, float muly, float addx, float addy, float octaves, float delta, float scale) {
	const float xcoef=mulx / hm->w;
	const float ycoef=muly / hm->h;
	for (int y=0; y < hm->h; y++) {
		for (int x=0; x < hm->w; x++) {
			float f[2] = {
				(x + addx) * xcoef,
				(y + addy) * ycoef
			};
			GET_VALUE(hm, x, y) += delta+TCOD_noise_get_fbm(noise,f,octaves)*scale;
		}
	}
}

void TCOD_heightmap_scale_fbm(TCOD_heightmap_t *hm, TCOD_noise_t noise,float mulx, float muly, float addx, float addy, float octaves, float delta, float scale) {
	const float xcoef=mulx / hm->w;
	const float ycoef=muly / hm->h;
	for (int y = 0; y < hm->h; y++) {
		for (int x=0; x < hm->w; x++) {
			float f[2] = {
				(x + addx) * xcoef,
				(y + addy) * ycoef
			};
			GET_VALUE(hm, x, y) *= delta+TCOD_noise_get_fbm(noise,f,octaves)*scale;
		}
	}
}

float TCOD_heightmap_get_interpolated_value(const TCOD_heightmap_t *hm, float x, float y) {
	x = CLAMP(0.0f, hm->w-1, x);
	y = CLAMP(0.0f, hm->h-1, y);
	float fix;
	float fiy;
	float fx = modff(x, &fix);
	float fy = modff(y, &fiy);
	int ix = fix;
	int iy = fiy;

	if (ix >= hm->w - 1) {
		ix = hm->w - 2;
		fx = 1.0;
	}
	if (iy >= hm->h - 1) {
		iy = hm->h - 2;
		fy = 1.0;
	}
	const float c1 = GET_VALUE(hm,ix,iy);
	const float c2 = GET_VALUE(hm,ix+1,iy);
	const float c3 = GET_VALUE(hm,ix,iy+1);
	const float c4 = GET_VALUE(hm,ix+1,iy+1);
	const float top = LERP(c1, c2, fx);
	const float bottom = LERP(c3, c4, fx);
	return LERP(top, bottom, fy);
}

void TCOD_heightmap_get_normal(const TCOD_heightmap_t *hm, float x, float y, float n[3], float waterLevel) {
	float h0,hx,hy,invlen; /* map heights at x,y x+1,y and x,y+1 */
	n[0]=0.0f;n[1]=0.0f;n[2]=1.0f;
	if ( x >= hm->w-1 || y >= hm->h-1 ) return;
	h0 = TCOD_heightmap_get_interpolated_value(hm,x,y);
	if ( h0 < waterLevel ) h0=waterLevel;
	hx = TCOD_heightmap_get_interpolated_value(hm,x+1,y);
	if ( hx < waterLevel ) hx=waterLevel;
	hy = TCOD_heightmap_get_interpolated_value(hm,x,y+1);
	if ( hy < waterLevel ) hy=waterLevel;
	/* vx = 1       vy = 0 */
	/*      0            1 */
	/*      hx-h0        hy-h0 */
	/* vz = vx cross vy */
	n[0] = 255*(h0-hx);
	n[1] = 255*(h0-hy);
	n[2] = 16.0f;
	/* normalize */
	invlen=1.0f / (float)sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]);
	n[0]*=invlen;
	n[1]*=invlen;
	n[2]*=invlen;
}

void TCOD_heightmap_dig_bezier(TCOD_heightmap_t *hm, int px[4], int py[4], float startRadius, float startDepth, float endRadius, float endDepth) {
	int xFrom=px[0];
	int yFrom=py[0];
	for ( int i=0;i <= 1000 ; ++i) {
		const float t=i/1000.f;
		const float it=1.0f-t;
		const int xTo=(px[0]*it*it*it + 3*px[1]*t*it*it + 3*px[2]*t*t*it + px[3]*t*t*t);
		const int yTo=(py[0]*it*it*it + 3*py[1]*t*it*it + 3*py[2]*t*t*it + py[3]*t*t*t);
		if ( xTo != xFrom || yTo != yFrom ) {
			float radius=startRadius+(endRadius-startRadius)*t;
			float depth=startDepth+(endDepth-startDepth)*t;
			TCOD_heightmap_dig_hill(hm,xTo,yTo,radius,depth);
			xFrom=xTo;
			yFrom=yTo;
		}
	}
}

bool TCOD_heightmap_has_land_on_border(const TCOD_heightmap_t *hm, float waterLevel) {
	for (int x=0; x < hm->w; x++) {
		if ( GET_VALUE(hm,x,0) > waterLevel
			|| GET_VALUE(hm,x,hm->h-1) > waterLevel ) {
			return true;
		}
	}
	for (int y=0; y < hm->h; y++) {
		if ( GET_VALUE(hm,0,y) > waterLevel
			|| GET_VALUE(hm,hm->w-1,y) > waterLevel ) {
			return true;
		}
	}
	return false;
}

void TCOD_heightmap_islandify(TCOD_heightmap_t *hm, float seaLevel,TCOD_random_t rnd) {
  (void)hm; (void)seaLevel; (void)rnd; // This function is pending removal.
}

void TCOD_heightmap_add(TCOD_heightmap_t *hm, float value) {
	int i;
	for (i=0; i < hm->w*hm->h; i++ ) hm->values[i]+=value;
}

int TCOD_heightmap_count_cells(const TCOD_heightmap_t *hm, float min, float max) {
	int count=0,i;
	for (i=0; i < hm->w*hm->h; i++ ) if (hm->values[i] >= min && hm->values[i] <= max) count++;
	return count;
}

void TCOD_heightmap_scale(TCOD_heightmap_t *hm, float value) {
	int i;
	for (i=0; i < hm->w*hm->h; i++ ) hm->values[i]*=value;
}

void TCOD_heightmap_clamp(TCOD_heightmap_t *hm, float min, float max) {
	int i;
	for (i=0; i < hm->w*hm->h; i++ ) {
		hm->values[i]=CLAMP(min,max,hm->values[i]);
	}
}

void TCOD_heightmap_lerp_hm(const TCOD_heightmap_t *hm1, const TCOD_heightmap_t *hm2, TCOD_heightmap_t *hmres, float coef) {
	if (!is_same_size(hm1, hm2) || !is_same_size(hm1, hmres)) {
		return;
	}
	for (int i=0; i < hm1->w*hm1->h; i++ ) {
		hmres->values[i]=LERP(hm1->values[i], hm2->values[i], coef);
	}
}

void TCOD_heightmap_add_hm(const TCOD_heightmap_t *hm1, const TCOD_heightmap_t *hm2, TCOD_heightmap_t *hmres) {
	if (!is_same_size(hm1, hm2) || !is_same_size(hm1, hmres)) {
		return;
	}
	for (int i=0; i < hm1->w*hm1->h; i++ ) {
		hmres->values[i]=hm1->values[i]+hm2->values[i];
	}
}

void TCOD_heightmap_multiply_hm(const TCOD_heightmap_t *hm1, const TCOD_heightmap_t *hm2, TCOD_heightmap_t *hmres) {
	if (!is_same_size(hm1, hm2) || !is_same_size(hm1, hmres)) {
		return;
	}
	for (int i=0; i < hm1->w*hm1->h; i++ ) {
		hmres->values[i]=hm1->values[i]*hm2->values[i];
	}
}

float TCOD_heightmap_get_slope(const TCOD_heightmap_t *hm, int x, int y) {
	static const int dix[8]={-1,0,1,-1,1,-1,0,1};
	static const int diy[8]={-1,-1,-1,0,0,1,1,1};
	float mindy=0.0f,maxdy=0.0f;
	if (!in_bounds(hm, x, y)) {
		return 0;
	}
	const float v=GET_VALUE(hm,x,y);
	for (int i=0; i < 8; i++ ) {
		const int nx=x+dix[i];
		const int ny=y+diy[i];
		if ( in_bounds(hm, nx, ny) ) {
			const float nslope=GET_VALUE(hm,nx,ny)-v;
			mindy = MIN(mindy, nslope);
			maxdy = MAX(maxdy, nslope);
		}
	}
	return (float)atan2(maxdy+mindy,1.0f);
}

void TCOD_heightmap_rain_erosion(TCOD_heightmap_t *hm, int nbDrops,float erosionCoef,float agregationCoef,TCOD_random_t rnd) {
	while (nbDrops-- > 0) {
		int curx=TCOD_random_get_int(rnd,0,hm->w-1);
		int cury=TCOD_random_get_int(rnd,0,hm->h-1);
		static const int dx[8]={-1,0,1,-1,1,-1,0,1};
		static const int dy[8]={-1,-1,-1,0,0,1,1,1};
		float sediment=0.0f;
		do {
			int nextx=0,nexty=0;
			float v=GET_VALUE(hm,curx,cury);
			/* calculate slope at x,y */
			float slope = -INFINITY;
			for (int i=0; i < 8; i++ ) {
				const int nx=curx+dx[i];
				const int ny=cury+dy[i];
				if (!in_bounds(hm, nx, ny)) continue;
				const float nslope=v-GET_VALUE(hm,nx,ny);
				if (nslope > slope) {
					slope=nslope;
					nextx=nx;
					nexty=ny;
				}
			}
			if ( slope > 0.0f ) {
/*				GET_VALUE(hm,curx,cury) *= 1.0f - (erosionCoef * slope); */
				GET_VALUE(hm,curx,cury) -= erosionCoef * slope;
				curx=nextx;
				cury=nexty;
				sediment+=slope;
			} else {
/*				GET_VALUE(hm,curx,cury) *= 1.0f + (agregationCoef*sediment); */
				GET_VALUE(hm,curx,cury) += agregationCoef*sediment;
				break;
			}
		} while (1);
	}
}

#if 0
static void setMPDHeight(TCOD_heightmap_t *hm, TCOD_random_t rnd,int x,int y, float z, float offset);
static void setMDPHeightSquare(TCOD_heightmap_t *hm, TCOD_random_t rnd,int x, int y, int initsz, int sz,float offset);

void TCOD_heightmap_heat_erosion(TCOD_heightmap_t *hm, int nbPass,float minSlope,float erosionCoef,float agregationCoef,TCOD_random_t rnd) {
	while ( nbPass-- > 0 ) {
		for (int y=0; y < hm->h; y++) {
			for (int x=0; x < hm->w; x++) {
				static const int dx[8]={-1,0,1,-1,1,-1,0,1};
				static const int dy[8]={-1,-1,-1,0,0,1,1,1};
				int nextx=0,nexty=0,i;
				const float v=GET_VALUE(hm, x, y);
				/* calculate slope at x,y */
				float slope=0.0f;
				for (i=0; i < 8; i++ ) { /* 4 : von neumann neighbourhood 8 : moore neighbourhood */
					const int nx=x+dx[i];
					const int ny=y+dy[i];
					if (in_bounds(hm, nx, ny)) {
						const float nslope=v-GET_VALUE(hm,nx,ny);
						if ( nslope > slope ) {
							slope=nslope;
							nextx=nx;
							nexty=ny;
						}
					}
				}
				if ( slope > minSlope ) {
					GET_VALUE(hm,x,y) -= erosionCoef*(slope-minSlope);
					GET_VALUE(hm,nextx,nexty) += agregationCoef*(slope-minSlope);
				}
			}
		}
	}
}
#endif

void TCOD_heightmap_kernel_transform(TCOD_heightmap_t *hm, int kernelsize, const int *dx, const int *dy, const float *weight, float minLevel,float maxLevel) {
	for (int y=0; y < hm->h; y++) {
		for (int x=0; x < hm->w; x++) {
			if (GET_VALUE(hm, x, y) >= minLevel && GET_VALUE(hm, x, y) <= maxLevel) {
				float val=0.0f;
				float totalWeight=0.0f;
				for (int i=0; i < kernelsize; i++ ) {
					const int nx=x+dx[i];
					const int ny=y+dy[i];
					if ( in_bounds(hm, nx, ny) ) {
						val += weight[i] * GET_VALUE(hm,nx,ny);
						totalWeight += weight[i];
					}
				}
				GET_VALUE(hm, x, y) = val/totalWeight;
			}
		}
	}

}

void TCOD_heightmap_add_voronoi(TCOD_heightmap_t *hm, int nbPoints, int nbCoef, const float *coef,TCOD_random_t rnd) {
	typedef struct {
		int x,y;
		float dist;
	} point_t;
	if ( nbPoints <= 0 ) return;
	point_t *pt = malloc(sizeof(point_t)*nbPoints);
	nbCoef = MIN(nbCoef, nbPoints);
	for (int i=0; i < nbPoints; i++ ) {
		pt[i].x = TCOD_random_get_int(rnd,0,hm->w-1);
		pt[i].y = TCOD_random_get_int(rnd,0,hm->h-1);
	}
	for (int y=0; y < hm->h; y++) {
		for (int x=0; x < hm->w; x++) {
			/* calculate distance to voronoi points */
			for (int i=0; i < nbPoints; i++ ) {
				const int dx = pt[i].x - x;
				const int dy = pt[i].y - y;
				pt[i].dist = dx*dx + dy*dy;
			}
			for (int i=0; i < nbCoef; i++ ) {
				/* get closest point */
				float minDist=1E8f;
				int idx=-1;
				for (int j=0; j < nbPoints; j++ ) {
					if ( pt[j].dist < minDist ) {
						idx=j;
						minDist=pt[j].dist;
					}
				}
				if (idx == -1) break;
				GET_VALUE(hm, x, y)+=coef[i]*pt[idx].dist;
				pt[idx].dist=1E8f;
			}
		}
	}
	free(pt);
}

static void setMPDHeight(TCOD_heightmap_t *hm, TCOD_random_t rnd,int x,int y, float z, float offset);
static void setMDPHeightSquare(TCOD_heightmap_t *hm, TCOD_random_t rnd,int x, int y, int initsz, int sz,float offset);

void TCOD_heightmap_mid_point_displacement(TCOD_heightmap_t *hm, TCOD_random_t rnd, float roughness) {
	int step = 1;
	float offset = 1.0f;
	int initsz = MIN(hm->w,hm->h)-1;
	int sz = initsz;
	hm->values[0] = TCOD_random_get_float(rnd,0.0f,1.0f);
	hm->values[sz-1] = TCOD_random_get_float(rnd,0.0f,1.0f);
	hm->values[(sz-1)*sz] = TCOD_random_get_float(rnd,0.0f,1.0f);
	hm->values[sz*sz-1] = TCOD_random_get_float(rnd,0.0f,1.0f);
	while (sz > 0) {
		/* diamond step */
		for (int y=0; y < step; y++ ) {
			for (int x=0; x < step; x++ ) {
				const int diamondx = sz / 2 + x * sz;
				const int diamondy = sz / 2 + y * sz;
				float z = GET_VALUE(hm,x*sz,y*sz);
				z += GET_VALUE(hm,(x+1)*sz,y*sz);
				z += GET_VALUE(hm,(x+1)*sz,(y+1)*sz);
				z += GET_VALUE(hm,x*sz,(y+1)*sz);
				z *= 0.25f;
				setMPDHeight(hm,rnd,diamondx,diamondy,z,offset);
			}
		}
		offset*=roughness;
		/* square step */
		for (int y=0; y < step; y++ ) {
			for (int x=0; x < step; x++ ) {
				int diamondx = sz / 2 + x * sz;
				int diamondy = sz / 2 + y * sz;
				/* north */
				setMDPHeightSquare(hm, rnd, diamondx, diamondy-sz/2, initsz,sz/2, offset);
				/* south */
				setMDPHeightSquare(hm, rnd, diamondx, diamondy+sz/2, initsz,sz/2, offset);
				/* west */
				setMDPHeightSquare(hm, rnd, diamondx-sz/2, diamondy, initsz,sz/2, offset);
				/* east */
				setMDPHeightSquare(hm, rnd, diamondx+sz/2, diamondy, initsz,sz/2, offset);
			}
		}
		sz /= 2;
		step *= 2;
	}
}

/* private stuff */
static void setMPDHeight(TCOD_heightmap_t *hm, TCOD_random_t rnd,int x,int y, float z, float offset) {
	z += TCOD_random_get_float(rnd,-offset,offset);
	GET_VALUE(hm,x,y)=z;
}

static void setMDPHeightSquare(TCOD_heightmap_t *hm, TCOD_random_t rnd,int x, int y, int initsz, int sz,float offset) {
	float z=0;
	int count=0;
	if ( y >= sz ) {
		z += GET_VALUE(hm,x,y-sz);
		count++;
	}
	if ( x >= sz ) {
		z += GET_VALUE(hm,x-sz,y);
		count++;
	}
	if ( y+sz < initsz ) {
		z += GET_VALUE(hm,x,y+sz);
		count++;
	}
	if ( x+sz < initsz ) {
		z += GET_VALUE(hm,x+sz,y);
		count++;
	}
	z /= count;
	setMPDHeight(hm,rnd,x,y,z,offset);
}
