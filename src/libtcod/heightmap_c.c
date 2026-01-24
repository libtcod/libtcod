/* BSD 3-Clause License
 *
 * Copyright © 2008-2026, Jice and the libtcod contributors.
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
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "heightmap.h"
#include "mersenne.h"
#include "utility.h"

#define GET_VALUE(hm, x, y) (hm)->values[(x) + (y) * (hm)->w]

/// @brief Return true if heightmap is valid
/// @details Valid heightmaps are non-NULL and never have negative shapes
static bool TCOD_heightmap_is_valid(const TCOD_heightmap_t* heightmap) {
  return heightmap && heightmap->values && heightmap->w >= 0 && heightmap->h >= 0;
}
/// @brief Returns true if `x`,`y` are valid coordinates for this heightmap
static bool TCOD_heightmap_in_bounds(const TCOD_heightmap_t* heightmap, int x, int y) {
  return TCOD_heightmap_is_valid(heightmap) && 0 <= x && 0 <= y && x < heightmap->w && y < heightmap->h;
}
/// @brief Return true if two heightmaps have the same shape for the purposes of a vectorized operation
static bool TCOD_heightmap_is_same_size(const TCOD_heightmap_t* hm1, const TCOD_heightmap_t* hm2) {
  return TCOD_heightmap_is_valid(hm1) && TCOD_heightmap_is_valid(hm2) && hm1->w == hm2->w && hm1->h == hm2->h;
}

TCOD_heightmap_t* TCOD_heightmap_new(int w, int h) {
  TCOD_heightmap_t* hm = malloc(sizeof(*hm));
  hm->values = calloc(w * h, sizeof(*hm->values));
  if (!hm->values) {
    free(hm);
    return NULL;
  }
  hm->w = w;
  hm->h = h;
  return hm;
}

void TCOD_heightmap_delete(TCOD_heightmap_t* hm) {
  if (hm) free(hm->values);
  free(hm);
}

void TCOD_heightmap_clear(TCOD_heightmap_t* __restrict hm) {
  if (!TCOD_heightmap_is_valid(hm)) return;
  for (size_t i = 0; i < hm->w * hm->h; ++i) hm->values[i] = 0;
}

float TCOD_heightmap_get_value(const TCOD_heightmap_t* hm, int x, int y) {
  if (!TCOD_heightmap_in_bounds(hm, x, y)) return 0.0;
  return GET_VALUE(hm, x, y);
}

void TCOD_heightmap_set_value(TCOD_heightmap_t* hm, int x, int y, float value) {
  if (TCOD_heightmap_in_bounds(hm, x, y)) GET_VALUE(hm, x, y) = value;
}

void TCOD_heightmap_get_minmax(
    const TCOD_heightmap_t* __restrict heightmap, float* __restrict min_out, float* __restrict max_out) {
  if (min_out) *min_out = FLT_MAX;
  if (max_out) *max_out = -FLT_MAX;
  if (!heightmap) return;
  for (int i = 0; i < heightmap->h * heightmap->w; ++i) {
    const float value = heightmap->values[i];
    if (min_out) *min_out = TCOD_MIN(*min_out, value);
    if (max_out) *max_out = TCOD_MAX(*max_out, value);
  }
}

void TCOD_heightmap_normalize(TCOD_heightmap_t* hm, float min, float max) {
  if (!TCOD_heightmap_is_valid(hm)) return;
  float current_min = 0;
  float current_max = 0;
  TCOD_heightmap_get_minmax(hm, &current_min, &current_max);

  if (current_max - current_min < FLT_EPSILON) {
    for (int i = 0; i != hm->w * hm->h; ++i) hm->values[i] = min;

  } else {
    const float normalize_scale = (max - min) / (current_max - current_min);
    for (int i = 0; i != hm->w * hm->h; ++i) hm->values[i] = min + (hm->values[i] - current_min) * normalize_scale;
  }
}

void TCOD_heightmap_add_hill(TCOD_heightmap_t* hm, float hx, float hy, float h_radius, float h_height) {
  if (!TCOD_heightmap_is_valid(hm)) return;
  const float h_radius2 = h_radius * h_radius;
  const float coef = h_height / h_radius2;
  const int minx = TCOD_MAX((int)(hx - h_radius), 0);
  const int miny = TCOD_MAX((int)(hy - h_radius), 0);
  const int maxx = (int)TCOD_MIN(ceilf(hx + h_radius), hm->w);
  const int maxy = (int)TCOD_MIN(ceilf(hy + h_radius), hm->h);
  for (int y = miny; y < maxy; y++) {
    const float y_dist = (y - hy) * (y - hy);
    for (int x = minx; x < maxx; x++) {
      const float x_dist = (x - hx) * (x - hx);
      const float z = h_radius2 - x_dist - y_dist;
      if (z > 0) GET_VALUE(hm, x, y) += z * coef;
    }
  }
}

void TCOD_heightmap_dig_hill(TCOD_heightmap_t* hm, float hx, float hy, float h_radius, float h_height) {
  if (!TCOD_heightmap_is_valid(hm)) return;
  const float h_radius2 = h_radius * h_radius;
  const float coef = h_height / h_radius2;
  const int minx = TCOD_MAX((int)(hx - h_radius), 0);
  const int miny = TCOD_MAX((int)(hy - h_radius), 0);
  const int maxx = (int)TCOD_MIN(ceilf(hx + h_radius), hm->w);
  const int maxy = (int)TCOD_MIN(ceilf(hy + h_radius), hm->h);
  for (int y = miny; y < maxy; y++) {
    for (int x = minx; x < maxx; x++) {
      const float x_dist = (x - hx) * (x - hx);
      const float y_dist = (y - hy) * (y - hy);
      const float dist = x_dist + y_dist;
      if (dist < h_radius2) {
        const float z = (h_radius2 - dist) * coef;
        if (h_height > 0) {
          if (GET_VALUE(hm, x, y) < z) GET_VALUE(hm, x, y) = z;
        } else {
          if (GET_VALUE(hm, x, y) > z) GET_VALUE(hm, x, y) = z;
        }
      }
    }
  }
}

void TCOD_heightmap_copy(const TCOD_heightmap_t* __restrict hm_source, TCOD_heightmap_t* __restrict hm_dest) {
  if (!TCOD_heightmap_is_same_size(hm_source, hm_dest)) return;
  for (size_t i = 0; i < hm_source->w * hm_source->h; ++i) hm_dest->values[i] = hm_source->values[i];
}

void TCOD_heightmap_add_fbm(
    TCOD_heightmap_t* __restrict hm,
    TCOD_Noise* __restrict noise,
    float mul_x,
    float mul_y,
    float add_x,
    float add_y,
    float octaves,
    float delta,
    float scale) {
  if (!TCOD_heightmap_is_valid(hm)) return;
  const float x_coefficient = mul_x / hm->w;
  const float y_coefficient = mul_y / hm->h;
  for (int y = 0; y < hm->h; y++) {
    for (int x = 0; x < hm->w; x++) {
      float f[2] = {(x + add_x) * x_coefficient, (y + add_y) * y_coefficient};
      GET_VALUE(hm, x, y) += delta + TCOD_noise_get_fbm(noise, f, octaves) * scale;
    }
  }
}

void TCOD_heightmap_scale_fbm(
    TCOD_heightmap_t* __restrict hm,
    TCOD_Noise* __restrict noise,
    float mul_x,
    float mul_y,
    float add_x,
    float add_y,
    float octaves,
    float delta,
    float scale) {
  if (!TCOD_heightmap_is_valid(hm)) return;
  const float x_coefficient = mul_x / hm->w;
  const float y_coefficient = mul_y / hm->h;
  for (int y = 0; y < hm->h; y++) {
    for (int x = 0; x < hm->w; x++) {
      float f[2] = {(x + add_x) * x_coefficient, (y + add_y) * y_coefficient};
      GET_VALUE(hm, x, y) *= delta + TCOD_noise_get_fbm(noise, f, octaves) * scale;
    }
  }
}

float TCOD_heightmap_get_interpolated_value(const TCOD_heightmap_t* hm, float x, float y) {
  if (!TCOD_heightmap_is_valid(hm)) return 0.0f;
  x = TCOD_CLAMP(0.0f, hm->w - 1, x);
  y = TCOD_CLAMP(0.0f, hm->h - 1, y);
  float fix;
  float fiy;
  float fx = modff(x, &fix);
  float fy = modff(y, &fiy);
  int ix = (int)fix;
  int iy = (int)fiy;

  if (ix >= hm->w - 1) {
    ix = hm->w - 2;
    fx = 1.0;
  }
  if (iy >= hm->h - 1) {
    iy = hm->h - 2;
    fy = 1.0;
  }
  const float c1 = GET_VALUE(hm, ix, iy);
  const float c2 = GET_VALUE(hm, ix + 1, iy);
  const float c3 = GET_VALUE(hm, ix, iy + 1);
  const float c4 = GET_VALUE(hm, ix + 1, iy + 1);
  const float top = TCOD_LERP(c1, c2, fx);
  const float bottom = TCOD_LERP(c3, c4, fx);
  return TCOD_LERP(top, bottom, fy);
}

void TCOD_heightmap_get_normal(const TCOD_heightmap_t* __restrict hm, float x, float y, float n[3], float waterLevel) {
  if (!TCOD_heightmap_is_valid(hm)) return;
  n[0] = 0.0f;
  n[1] = 0.0f;
  n[2] = 1.0f;
  if (x >= hm->w - 1 || y >= hm->h - 1) return;
  // Sample map heights at x,y; x+1,y; and x,y+1
  float height_0 = TCOD_heightmap_get_interpolated_value(hm, x, y);
  height_0 = TCOD_MAX(height_0, waterLevel);
  float height_x = TCOD_heightmap_get_interpolated_value(hm, x + 1, y);
  height_x = TCOD_MAX(height_x, waterLevel);
  float height_y = TCOD_heightmap_get_interpolated_value(hm, x, y + 1);
  height_y = TCOD_MAX(height_y, waterLevel);
  // vx = 1                 vy = 0
  //      0                      1
  //      height_x-height_0      height_y-height_0
  // vz = vx cross vy
  n[0] = 255 * (height_0 - height_x);
  n[1] = 255 * (height_0 - height_y);
  n[2] = 16.0f;
  // Normalize
  const float invlen = 1.0f / (float)sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
  n[0] *= invlen;
  n[1] *= invlen;
  n[2] *= invlen;
}

void TCOD_heightmap_dig_bezier(
    TCOD_heightmap_t* __restrict hm,
    int px[4],
    int py[4],
    float startRadius,
    float startDepth,
    float endRadius,
    float endDepth) {
  if (!TCOD_heightmap_is_valid(hm)) return;
  int xFrom = px[0];
  int yFrom = py[0];
  for (int i = 0; i <= 1000; ++i) {
    const float t = i / 1000.f;
    const float it = 1.0f - t;
    const int xTo = (int)(px[0] * it * it * it + 3 * px[1] * t * it * it + 3 * px[2] * t * t * it + px[3] * t * t * t);
    const int yTo = (int)(py[0] * it * it * it + 3 * py[1] * t * it * it + 3 * py[2] * t * t * it + py[3] * t * t * t);
    if (xTo != xFrom || yTo != yFrom) {
      float radius = startRadius + (endRadius - startRadius) * t;
      float depth = startDepth + (endDepth - startDepth) * t;
      TCOD_heightmap_dig_hill(hm, (float)xTo, (float)yTo, radius, depth);
      xFrom = xTo;
      yFrom = yTo;
    }
  }
}

bool TCOD_heightmap_has_land_on_border(const TCOD_heightmap_t* hm, float waterLevel) {
  if (!TCOD_heightmap_is_valid(hm)) return false;
  for (int x = 0; x < hm->w; x++) {
    if (GET_VALUE(hm, x, 0) > waterLevel || GET_VALUE(hm, x, hm->h - 1) > waterLevel) return true;
  }
  for (int y = 0; y < hm->h; y++) {
    if (GET_VALUE(hm, 0, y) > waterLevel || GET_VALUE(hm, hm->w - 1, y) > waterLevel) return true;
  }
  return false;
}

void TCOD_heightmap_islandify(TCOD_heightmap_t* __restrict hm, float seaLevel, TCOD_Random* __restrict rnd) {
  (void)hm;
  (void)seaLevel;
  (void)rnd;  // This function is pending removal.
}

void TCOD_heightmap_add(TCOD_heightmap_t* hm, float value) {
  if (!TCOD_heightmap_is_valid(hm)) return;
  for (int i = 0; i < hm->w * hm->h; ++i) hm->values[i] += value;
}

int TCOD_heightmap_count_cells(const TCOD_heightmap_t* hm, float min, float max) {
  if (!TCOD_heightmap_is_valid(hm)) return 0;
  int count = 0;
  for (int i = 0; i < hm->w * hm->h; ++i) {
    if (hm->values[i] >= min && hm->values[i] <= max) ++count;
  }
  return count;
}

void TCOD_heightmap_scale(TCOD_heightmap_t* hm, float value) {
  if (!TCOD_heightmap_is_valid(hm)) return;
  for (int i = 0; i < hm->w * hm->h; ++i) hm->values[i] *= value;
}

void TCOD_heightmap_clamp(TCOD_heightmap_t* hm, float min, float max) {
  if (!TCOD_heightmap_is_valid(hm)) return;
  for (int i = 0; i < hm->w * hm->h; ++i) hm->values[i] = TCOD_CLAMP(min, max, hm->values[i]);
}

void TCOD_heightmap_lerp_hm(
    const TCOD_heightmap_t* __restrict hm1,
    const TCOD_heightmap_t* __restrict hm2,
    TCOD_heightmap_t* __restrict hm_out,
    float coef) {
  if (!TCOD_heightmap_is_same_size(hm1, hm2) || !TCOD_heightmap_is_same_size(hm1, hm_out)) return;
  for (int i = 0; i < hm1->w * hm1->h; i++) hm_out->values[i] = TCOD_LERP(hm1->values[i], hm2->values[i], coef);
}

void TCOD_heightmap_add_hm(
    const TCOD_heightmap_t* __restrict hm1,
    const TCOD_heightmap_t* __restrict hm2,
    TCOD_heightmap_t* __restrict hm_out) {
  if (!TCOD_heightmap_is_same_size(hm1, hm2) || !TCOD_heightmap_is_same_size(hm1, hm_out)) return;
  for (int i = 0; i < hm1->w * hm1->h; i++) hm_out->values[i] = hm1->values[i] + hm2->values[i];
}

void TCOD_heightmap_multiply_hm(
    const TCOD_heightmap_t* __restrict hm1,
    const TCOD_heightmap_t* __restrict hm2,
    TCOD_heightmap_t* __restrict hm_out) {
  if (!TCOD_heightmap_is_same_size(hm1, hm2) || !TCOD_heightmap_is_same_size(hm1, hm_out)) return;
  for (int i = 0; i < hm1->w * hm1->h; i++) hm_out->values[i] = hm1->values[i] * hm2->values[i];
}

float TCOD_heightmap_get_slope(const TCOD_heightmap_t* hm, int x, int y) {
  if (!TCOD_heightmap_in_bounds(hm, x, y)) return 0;
  static const int dix[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
  static const int diy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
  float min_dy = 0.0f, max_dy = 0.0f;
  const float v = GET_VALUE(hm, x, y);
  for (int i = 0; i < 8; i++) {
    const int nx = x + dix[i];
    const int ny = y + diy[i];
    if (TCOD_heightmap_in_bounds(hm, nx, ny)) {
      const float n_slope = GET_VALUE(hm, nx, ny) - v;
      min_dy = TCOD_MIN(min_dy, n_slope);
      max_dy = TCOD_MAX(max_dy, n_slope);
    }
  }
  return (float)atan2(max_dy + min_dy, 1.0f);
}

void TCOD_heightmap_rain_erosion(
    TCOD_heightmap_t* __restrict hm,
    int nbDrops,
    float erosionCoef,
    float aggregationCoef,
    TCOD_Random* __restrict rnd) {
  if (!TCOD_heightmap_is_valid(hm)) return;
  while (nbDrops-- > 0) {
    int curx = TCOD_random_get_int(rnd, 0, hm->w - 1);
    int cury = TCOD_random_get_int(rnd, 0, hm->h - 1);
    static const int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    static const int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    float sediment = 0.0f;
    do {
      int next_x = 0, next_y = 0;
      float v = GET_VALUE(hm, curx, cury);
      /* calculate slope at x,y */
      float slope = -INFINITY;
      for (int i = 0; i < 8; i++) {
        const int nx = curx + dx[i];
        const int ny = cury + dy[i];
        if (!TCOD_heightmap_in_bounds(hm, nx, ny)) continue;
        const float n_slope = v - GET_VALUE(hm, nx, ny);
        if (n_slope > slope) {
          slope = n_slope;
          next_x = nx;
          next_y = ny;
        }
      }
      if (slope > 0.0f) {
        /*				GET_VALUE(hm,curx,cury) *= 1.0f - (erosionCoef * slope); */
        GET_VALUE(hm, curx, cury) -= erosionCoef * slope;
        curx = next_x;
        cury = next_y;
        sediment += slope;
      } else {
        /*				GET_VALUE(hm,curx,cury) *= 1.0f + (aggregationCoef*sediment); */
        GET_VALUE(hm, curx, cury) += aggregationCoef * sediment;
        break;
      }
    } while (1);
  }
}

#if 0
static void setMPDHeight(TCOD_heightmap_t *hm, TCOD_Random* rnd,int x,int y, float z, float offset);
static void setMDPHeightSquare(TCOD_heightmap_t *hm, TCOD_Random* rnd,int x, int y, int initsz, int sz,float offset);

void TCOD_heightmap_heat_erosion(TCOD_heightmap_t *hm, int nbPass,float minSlope,float erosionCoef,float aggregationCoef,TCOD_Random* rnd) {
	if (!TCOD_heightmap_is_valid(hm)) { return; }
	while ( nbPass-- > 0 ) {
		for (int y=0; y < hm->h; y++) {
			for (int x=0; x < hm->w; x++) {
				static const int dx[8]={-1,0,1,-1,1,-1,0,1};
				static const int dy[8]={-1,-1,-1,0,0,1,1,1};
				int next_x=0,next_y=0,i;
				const float v=GET_VALUE(hm, x, y);
				/* calculate slope at x,y */
				float slope=0.0f;
				for (i=0; i < 8; i++ ) { /* 4 : von neumann neighborhood 8 : moore neighborhood */
					const int nx=x+dx[i];
					const int ny=y+dy[i];
					if (TCOD_heightmap_in_bounds(hm, nx, ny)) {
						const float n_slope=v-GET_VALUE(hm,nx,ny);
						if ( n_slope > slope ) {
							slope=n_slope;
							next_x=nx;
							next_y=ny;
						}
					}
				}
				if ( slope > minSlope ) {
					GET_VALUE(hm,x,y) -= erosionCoef*(slope-minSlope);
					GET_VALUE(hm,next_x,next_y) += aggregationCoef*(slope-minSlope);
				}
			}
		}
	}
}
#endif

void TCOD_heightmap_threshold_mask(
    const TCOD_heightmap_t* __restrict hm, uint8_t* __restrict mask, float minLevel, float maxLevel) {
  if (!TCOD_heightmap_is_valid(hm) || !mask) return;
  for (int i = 0; i < hm->w * hm->h; i++) mask[i] = (hm->values[i] >= minLevel && hm->values[i] <= maxLevel) ? 1 : 0;
}

void TCOD_heightmap_kernel_transform_out(
    const TCOD_heightmap_t* __restrict hm_src,
    TCOD_heightmap_t* __restrict hm_dst,
    int kernel_size,
    const int* __restrict dx,
    const int* __restrict dy,
    const float* __restrict weight,
    const uint8_t* __restrict mask) {
  if (!TCOD_heightmap_is_same_size(hm_src, hm_dst)) return;
  for (int y = 0; y < hm_src->h; y++) {
    for (int x = 0; x < hm_src->w; x++) {
      const int idx = x + y * hm_src->w;
      // Transform if no mask, or mask value is non-zero
      if (!mask || mask[idx]) {
        float val = 0.0f;
        float totalWeight = 0.0f;
        for (int i = 0; i < kernel_size; i++) {
          const int nx = x + dx[i];
          const int ny = y + dy[i];
          if (TCOD_heightmap_in_bounds(hm_src, nx, ny)) {
            val += weight[i] * GET_VALUE(hm_src, nx, ny);
            totalWeight += weight[i];
          }
        }
        hm_dst->values[idx] = val / totalWeight;
      }
    }
  }
}

void TCOD_heightmap_kernel_transform(
    TCOD_heightmap_t* __restrict hm,
    int kernel_size,
    const int* __restrict dx,
    const int* __restrict dy,
    const float* __restrict weight,
    float minLevel,
    float maxLevel) {
  if (!TCOD_heightmap_is_valid(hm)) return;
  // Create a copy of the input to read from
  TCOD_heightmap_t* hm_copy = TCOD_heightmap_new(hm->w, hm->h);
  if (!hm_copy) return;
  TCOD_heightmap_copy(hm, hm_copy);

  // Generate mask if threshold range is not "all values"
  uint8_t* mask = NULL;
  if (!(minLevel <= -FLT_MAX && maxLevel >= FLT_MAX)) {
    mask = malloc((size_t)hm->w * (size_t)hm->h);
    if (!mask) {
      TCOD_heightmap_delete(hm_copy);
      return;
    }
    TCOD_heightmap_threshold_mask(hm_copy, mask, minLevel, maxLevel);
  }

  TCOD_heightmap_kernel_transform_out(hm_copy, hm, kernel_size, dx, dy, weight, mask);

  free(mask);
  TCOD_heightmap_delete(hm_copy);
}

void TCOD_heightmap_add_voronoi(
    TCOD_heightmap_t* __restrict hm,
    int nbPoints,
    int nbCoef,
    const float* __restrict coef,
    TCOD_Random* __restrict rnd) {
  if (!TCOD_heightmap_is_valid(hm)) return;
  typedef struct {
    int x, y;
    float dist;
  } point_t;
  if (nbPoints <= 0) return;
  point_t* pt = malloc(nbPoints * sizeof(*pt));
  nbCoef = TCOD_MIN(nbCoef, nbPoints);
  for (int i = 0; i < nbPoints; i++) {
    pt[i].x = TCOD_random_get_int(rnd, 0, hm->w - 1);
    pt[i].y = TCOD_random_get_int(rnd, 0, hm->h - 1);
  }
  for (int y = 0; y < hm->h; y++) {
    for (int x = 0; x < hm->w; x++) {
      /* calculate distance to voronoi points */
      for (int i = 0; i < nbPoints; i++) {
        const int dx = pt[i].x - x;
        const int dy = pt[i].y - y;
        pt[i].dist = (float)(dx * dx + dy * dy);
      }
      for (int i = 0; i < nbCoef; i++) {
        /* get closest point */
        float minDist = 1E8f;
        int idx = -1;
        for (int j = 0; j < nbPoints; j++) {
          if (pt[j].dist < minDist) {
            idx = j;
            minDist = pt[j].dist;
          }
        }
        if (idx == -1) break;
        GET_VALUE(hm, x, y) += coef[i] * pt[idx].dist;
        pt[idx].dist = 1E8f;
      }
    }
  }
  free(pt);
}

static void setMPDHeight(
    TCOD_heightmap_t* __restrict hm, TCOD_Random* __restrict rnd, int x, int y, float z, float offset) {
  z += TCOD_random_get_float(rnd, -offset, offset);
  GET_VALUE(hm, x, y) = z;
}

static void setMDPHeightSquare(
    TCOD_heightmap_t* __restrict hm, TCOD_Random* __restrict rnd, int x, int y, int initsz, int sz, float offset) {
  float z = 0;
  int count = 0;
  if (y >= sz) {
    z += GET_VALUE(hm, x, y - sz);
    count++;
  }
  if (x >= sz) {
    z += GET_VALUE(hm, x - sz, y);
    count++;
  }
  if (y + sz < initsz) {
    z += GET_VALUE(hm, x, y + sz);
    count++;
  }
  if (x + sz < initsz) {
    z += GET_VALUE(hm, x + sz, y);
    count++;
  }
  z /= count;
  setMPDHeight(hm, rnd, x, y, z, offset);
}

void TCOD_heightmap_mid_point_displacement(
    TCOD_heightmap_t* __restrict hm, TCOD_Random* __restrict rnd, float roughness) {
  if (!TCOD_heightmap_is_valid(hm)) return;
  int step = 1;
  float offset = 1.0f;
  int initsz = TCOD_MIN(hm->w, hm->h) - 1;
  int sz = initsz;
  hm->values[0] = TCOD_random_get_float(rnd, 0.0f, 1.0f);
  hm->values[sz - 1] = TCOD_random_get_float(rnd, 0.0f, 1.0f);
  hm->values[(sz - 1) * sz] = TCOD_random_get_float(rnd, 0.0f, 1.0f);
  hm->values[sz * sz - 1] = TCOD_random_get_float(rnd, 0.0f, 1.0f);
  while (sz > 0) {
    /* diamond step */
    for (int y = 0; y < step; y++) {
      for (int x = 0; x < step; x++) {
        const int diamond_x = sz / 2 + x * sz;
        const int diamond_y = sz / 2 + y * sz;
        float z = GET_VALUE(hm, x * sz, y * sz);
        z += GET_VALUE(hm, (x + 1) * sz, y * sz);
        z += GET_VALUE(hm, (x + 1) * sz, (y + 1) * sz);
        z += GET_VALUE(hm, x * sz, (y + 1) * sz);
        z *= 0.25f;
        setMPDHeight(hm, rnd, diamond_x, diamond_y, z, offset);
      }
    }
    offset *= roughness;
    /* square step */
    for (int y = 0; y < step; y++) {
      for (int x = 0; x < step; x++) {
        int diamond_x = sz / 2 + x * sz;
        int diamond_y = sz / 2 + y * sz;
        /* north */
        setMDPHeightSquare(hm, rnd, diamond_x, diamond_y - sz / 2, initsz, sz / 2, offset);
        /* south */
        setMDPHeightSquare(hm, rnd, diamond_x, diamond_y + sz / 2, initsz, sz / 2, offset);
        /* west */
        setMDPHeightSquare(hm, rnd, diamond_x - sz / 2, diamond_y, initsz, sz / 2, offset);
        /* east */
        setMDPHeightSquare(hm, rnd, diamond_x + sz / 2, diamond_y, initsz, sz / 2, offset);
      }
    }
    sz /= 2;
    step *= 2;
  }
}
