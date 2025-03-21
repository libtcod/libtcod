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
#ifndef TCOD_HEIGHTMAP_H_
#define TCOD_HEIGHTMAP_H_

#include "mersenne_types.h"
#include "noise.h"
#include "portability.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
  int w, h;
  float* __restrict values;
} TCOD_heightmap_t;

TCODLIB_API TCOD_heightmap_t* TCOD_heightmap_new(int w, int h);
TCODLIB_API void TCOD_heightmap_delete(TCOD_heightmap_t* hm);

TCODLIB_API float TCOD_heightmap_get_value(const TCOD_heightmap_t* hm, int x, int y);
TCODLIB_API float TCOD_heightmap_get_interpolated_value(const TCOD_heightmap_t* hm, float x, float y);
TCODLIB_API void TCOD_heightmap_set_value(TCOD_heightmap_t* hm, int x, int y, float value);
TCODLIB_API float TCOD_heightmap_get_slope(const TCOD_heightmap_t* hm, int x, int y);
TCODLIB_API void TCOD_heightmap_get_normal(const TCOD_heightmap_t* hm, float x, float y, float n[3], float waterLevel);
TCODLIB_API int TCOD_heightmap_count_cells(const TCOD_heightmap_t* hm, float min, float max);
TCODLIB_API bool TCOD_heightmap_has_land_on_border(const TCOD_heightmap_t* hm, float waterLevel);
TCODLIB_API void TCOD_heightmap_get_minmax(const TCOD_heightmap_t* hm, float* min, float* max);

TCODLIB_API void TCOD_heightmap_copy(const TCOD_heightmap_t* hm_source, TCOD_heightmap_t* hm_dest);
TCODLIB_API void TCOD_heightmap_add(TCOD_heightmap_t* hm, float value);
TCODLIB_API void TCOD_heightmap_scale(TCOD_heightmap_t* hm, float value);
TCODLIB_API void TCOD_heightmap_clamp(TCOD_heightmap_t* hm, float min, float max);
TCODLIB_API void TCOD_heightmap_normalize(TCOD_heightmap_t* hm, float min, float max);
TCODLIB_API void TCOD_heightmap_clear(TCOD_heightmap_t* hm);
TCODLIB_API void TCOD_heightmap_lerp_hm(
    const TCOD_heightmap_t* hm1, const TCOD_heightmap_t* hm2, TCOD_heightmap_t* out, float coef);
TCODLIB_API void TCOD_heightmap_add_hm(const TCOD_heightmap_t* hm1, const TCOD_heightmap_t* hm2, TCOD_heightmap_t* out);
TCODLIB_API void TCOD_heightmap_multiply_hm(
    const TCOD_heightmap_t* hm1, const TCOD_heightmap_t* hm2, TCOD_heightmap_t* out);

TCODLIB_API void TCOD_heightmap_add_hill(TCOD_heightmap_t* hm, float hx, float hy, float h_radius, float h_height);
TCODLIB_API void TCOD_heightmap_dig_hill(TCOD_heightmap_t* hm, float hx, float hy, float h_radius, float h_height);
TCODLIB_API void TCOD_heightmap_dig_bezier(
    TCOD_heightmap_t* hm, int px[4], int py[4], float startRadius, float startDepth, float endRadius, float endDepth);
TCODLIB_API void TCOD_heightmap_rain_erosion(
    TCOD_heightmap_t* hm, int nbDrops, float erosionCoef, float sedimentationCoef, TCOD_Random* rnd);
/* TCODLIB_API void TCOD_heightmap_heat_erosion(TCOD_heightmap_t *hm, int nbPass,float minSlope,float erosionCoef,float
 * sedimentationCoef,TCOD_Random* rnd); */
TCODLIB_API void TCOD_heightmap_kernel_transform(
    TCOD_heightmap_t* hm,
    int kernel_size,
    const int* dx,
    const int* dy,
    const float* weight,
    float minLevel,
    float maxLevel);
TCODLIB_API void TCOD_heightmap_add_voronoi(
    TCOD_heightmap_t* hm, int nbPoints, int nbCoef, const float* coef, TCOD_Random* rnd);
TCODLIB_API void TCOD_heightmap_mid_point_displacement(TCOD_heightmap_t* hm, TCOD_Random* rnd, float roughness);
TCODLIB_API void TCOD_heightmap_add_fbm(
    TCOD_heightmap_t* hm,
    TCOD_noise_t noise,
    float mul_x,
    float mul_y,
    float add_x,
    float add_y,
    float octaves,
    float delta,
    float scale);
TCODLIB_API void TCOD_heightmap_scale_fbm(
    TCOD_heightmap_t* hm,
    TCOD_noise_t noise,
    float mul_x,
    float mul_y,
    float add_x,
    float add_y,
    float octaves,
    float delta,
    float scale);
TCOD_DEPRECATED("This function does nothing and will be removed.")
TCODLIB_API void TCOD_heightmap_islandify(TCOD_heightmap_t* hm, float seaLevel, TCOD_Random* rnd);
#ifdef __cplusplus
}
#endif
#endif  // TCOD_HEIGHTMAP_H_
