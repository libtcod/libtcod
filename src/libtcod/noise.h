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
#ifndef _TCOD_PERLIN_H
#define _TCOD_PERLIN_H

#include "portability.h"
#include "mersenne_types.h"
#include "noise_defaults.h"

typedef enum {
	TCOD_NOISE_PERLIN = 1,
	TCOD_NOISE_SIMPLEX = 2,
	TCOD_NOISE_WAVELET = 4,
	TCOD_NOISE_DEFAULT = 0
} TCOD_noise_type_t;

typedef struct TCOD_Noise {
  int ndim;
  /** Randomized map of indexes into buffer */
  unsigned char map[256];
  /** Random 256 x ndim buffer */
  float buffer[256][TCOD_NOISE_MAX_DIMENSIONS];
  /* fractal stuff */
  float H;
  float lacunarity;
  float exponent[TCOD_NOISE_MAX_OCTAVES];
  float *waveletTileData;
  TCOD_random_t rand;
  /* noise type */
  TCOD_noise_type_t noise_type;
} TCOD_Noise;
typedef TCOD_Noise *TCOD_noise_t;
#ifdef __cplusplus
extern "C" {
#endif
/* create a new noise object */
TCODLIB_API TCOD_noise_t TCOD_noise_new(int dimensions, float hurst, float lacunarity, TCOD_random_t random);

/* simplified API */
TCODLIB_API void TCOD_noise_set_type (TCOD_noise_t noise, TCOD_noise_type_t type);
TCODLIB_API float TCOD_noise_get_ex (TCOD_noise_t noise, float *f, TCOD_noise_type_t type);
TCODLIB_API float TCOD_noise_get_fbm_ex (TCOD_noise_t noise, float *f, float octaves, TCOD_noise_type_t type);
TCODLIB_API float TCOD_noise_get_turbulence_ex (TCOD_noise_t noise, float *f, float octaves, TCOD_noise_type_t type);
TCODLIB_API float TCOD_noise_get (TCOD_noise_t noise, float *f);
TCODLIB_API float TCOD_noise_get_fbm (TCOD_noise_t noise, float *f, float octaves);
TCODLIB_API float TCOD_noise_get_turbulence (TCOD_noise_t noise, float *f, float octaves);
/* delete the noise object */
TCODLIB_API void TCOD_noise_delete(TCOD_noise_t noise);
#ifdef __cplusplus
}
#endif
#endif
