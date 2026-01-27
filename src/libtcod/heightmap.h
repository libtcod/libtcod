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
/// @file heightmap.h
/// Terrain heightmap module.
#pragma once
#ifndef TCOD_HEIGHTMAP_H_
#define TCOD_HEIGHTMAP_H_

#include <stdint.h>

#include "mersenne_types.h"
#include "noise.h"
#include "portability.h"
#ifdef __cplusplus
extern "C" {
#endif
/// @defgroup Heightmap Heightmaps (C)
/// @{

/// @brief A contigious 2D array of float values.
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
TCODLIB_API void TCOD_heightmap_get_normal(
    const TCOD_heightmap_t* __restrict hm, float x, float y, float n[3], float waterLevel);
TCODLIB_API int TCOD_heightmap_count_cells(const TCOD_heightmap_t* hm, float min, float max);
TCODLIB_API bool TCOD_heightmap_has_land_on_border(const TCOD_heightmap_t* hm, float waterLevel);
/***************************************************************************
    @brief Output the highest and lowest values in `heightmap` to `min` and `max`

    @param heightmap Input heightmap, can be NULL
    @param min_out The lowest value in `heightmap` or `FLT_MAX` if `heightmap` has no values, can be NULL
    @param max_out The highest value in `heightmap` or `-FLT_MAX` if `heightmap` has no values, can be NULL
 */
TCODLIB_API void TCOD_heightmap_get_minmax(
    const TCOD_heightmap_t* __restrict heightmap, float* __restrict min_out, float* __restrict max_out);

TCODLIB_API void TCOD_heightmap_copy(
    const TCOD_heightmap_t* __restrict hm_source, TCOD_heightmap_t* __restrict hm_dest);
TCODLIB_API void TCOD_heightmap_add(TCOD_heightmap_t* hm, float value);
TCODLIB_API void TCOD_heightmap_scale(TCOD_heightmap_t* hm, float value);
TCODLIB_API void TCOD_heightmap_clamp(TCOD_heightmap_t* hm, float min, float max);
TCODLIB_API void TCOD_heightmap_normalize(TCOD_heightmap_t* hm, float min, float max);
TCODLIB_API void TCOD_heightmap_clear(TCOD_heightmap_t* __restrict hm);
TCODLIB_API void TCOD_heightmap_lerp_hm(
    const TCOD_heightmap_t* __restrict hm1,
    const TCOD_heightmap_t* __restrict hm2,
    TCOD_heightmap_t* __restrict out,
    float coef);
TCODLIB_API void TCOD_heightmap_add_hm(
    const TCOD_heightmap_t* __restrict hm1, const TCOD_heightmap_t* __restrict hm2, TCOD_heightmap_t* __restrict out);
TCODLIB_API void TCOD_heightmap_multiply_hm(
    const TCOD_heightmap_t* __restrict hm1, const TCOD_heightmap_t* __restrict hm2, TCOD_heightmap_t* __restrict out);

TCODLIB_API void TCOD_heightmap_add_hill(TCOD_heightmap_t* hm, float hx, float hy, float h_radius, float h_height);
TCODLIB_API void TCOD_heightmap_dig_hill(TCOD_heightmap_t* hm, float hx, float hy, float h_radius, float h_height);
TCODLIB_API void TCOD_heightmap_dig_bezier(
    TCOD_heightmap_t* __restrict hm,
    int px[4],
    int py[4],
    float startRadius,
    float startDepth,
    float endRadius,
    float endDepth);
TCODLIB_API void TCOD_heightmap_rain_erosion(
    TCOD_heightmap_t* __restrict hm,
    int nbDrops,
    float erosionCoef,
    float sedimentationCoef,
    TCOD_Random* __restrict rnd);
/* TCODLIB_API void TCOD_heightmap_heat_erosion(TCOD_heightmap_t *hm, int nbPass,float minSlope,float erosionCoef,float
 * sedimentationCoef,TCOD_Random* rnd); */
TCODLIB_API void TCOD_heightmap_kernel_transform(
    TCOD_heightmap_t* __restrict hm,
    int kernel_size,
    const int* __restrict dx,
    const int* __restrict dy,
    const float* __restrict weight,
    float minLevel,
    float maxLevel);
/**
    @brief Generate a mask from heightmap values within a threshold range.

    Sets mask values to 1 where heightmap values are in [minLevel, maxLevel],
    and 0 elsewhere. The mask array must be allocated by the caller with
    size (hm->w * hm->h) bytes, stored in row-major order.

    @param hm Source heightmap to threshold.
    @param mask Destination mask array (must be hm->w * hm->h bytes, row-major).
    @param minLevel Minimum value (inclusive) for cells to be marked.
    @param maxLevel Maximum value (inclusive) for cells to be marked.

    @versionadded{Unreleased}
 */
TCODLIB_API void TCOD_heightmap_threshold_mask(
    const TCOD_heightmap_t* __restrict hm, uint8_t* __restrict mask, float minLevel, float maxLevel);
/**
    @brief Apply a sparse kernel convolution from source to destination heightmap.

    This function reads from the source heightmap and writes results to a separate
    destination heightmap. This avoids the need for an internal copy when the caller
    already has separate source and destination buffers.

    The kernel is defined by parallel arrays of x-offsets, y-offsets, and weights.
    For each cell, the weighted sum of neighboring values (as defined by the kernel)
    is computed and normalized by the total weight of in-bounds neighbors.

    If mask is non-NULL, only cells with non-zero mask values are transformed;
    cells with zero mask values are unmodified by the transformation.
    If mask is NULL, all cells are transformed.

    @param hm_src Source heightmap (read-only). Must not alias hm_dst.
    @param hm_dst Destination heightmap (must be same size as source). Must not alias hm_src.
    @param kernel_size Number of elements in the kernel arrays.
    @param dx Array of x-offsets for kernel positions.
    @param dy Array of y-offsets for kernel positions.
    @param weight Array of weights for each kernel position.
    @param mask Optional mask array (hm->w * hm->h bytes, row-major). NULL transforms all cells.

    @versionadded{Unreleased}
 */
TCODLIB_API void TCOD_heightmap_kernel_transform_out(
    const TCOD_heightmap_t* __restrict hm_src,
    TCOD_heightmap_t* __restrict hm_dst,
    int kernel_size,
    const int* __restrict dx,
    const int* __restrict dy,
    const float* __restrict weight,
    const uint8_t* __restrict mask);
TCODLIB_API void TCOD_heightmap_add_voronoi(
    TCOD_heightmap_t* __restrict hm,
    int nbPoints,
    int nbCoef,
    const float* __restrict coef,
    TCOD_Random* __restrict rnd);
TCODLIB_API void TCOD_heightmap_mid_point_displacement(
    TCOD_heightmap_t* __restrict hm, TCOD_Random* __restrict rnd, float roughness);
TCODLIB_API void TCOD_heightmap_add_fbm(
    TCOD_heightmap_t* __restrict hm,
    TCOD_Noise* __restrict noise,
    float mul_x,
    float mul_y,
    float add_x,
    float add_y,
    float octaves,
    float delta,
    float scale);
TCODLIB_API void TCOD_heightmap_scale_fbm(
    TCOD_heightmap_t* __restrict hm,
    TCOD_Noise* __restrict noise,
    float mul_x,
    float mul_y,
    float add_x,
    float add_y,
    float octaves,
    float delta,
    float scale);
TCOD_DEPRECATED("This function does nothing and will be removed.")
TCODLIB_API void TCOD_heightmap_islandify(TCOD_heightmap_t* __restrict hm, float seaLevel, TCOD_Random* __restrict rnd);
/// @}
#ifdef __cplusplus
}  // extern "C"
#endif
#endif  // TCOD_HEIGHTMAP_H_
