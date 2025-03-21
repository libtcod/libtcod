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
#include <float.h>
#include <math.h>
#include <stdlib.h>

#include "error.h"
#include "mersenne.h"
#include "noise.h"
#include "utility.h"

#define WAVELET_TILE_SIZE 32
#define WAVELET_ARAD 16

static const float SIMPLEX_SCALE = 0.5f;
static const float WAVELET_SCALE = 2.0f;

/**
    Common noise function pointer.

    Right now `TCOD_noise_wavelet` prevents `noise` from being const.
 */
typedef float (*TCOD_noise_func_t)(TCOD_Noise* __restrict noise, const float* __restrict f);

/**
 *  Return a floating point value clamped between -1.0f and 1.0f exclusively.
 *
 *  The return value excludes -1.0f and 1.0f to avoid rounding issues.
 */
static float clamp_signed_f(float value) {
  static const float LOW = -1.0f + FLT_EPSILON;
  static const float HIGH = 1.0f - FLT_EPSILON;
  if (value < LOW) {
    return LOW;
  }
  if (value > HIGH) {
    return HIGH;
  }
  return value;
}

static float lattice(
    const TCOD_Noise* __restrict data, int ix, float fx, int iy, float fy, int iz, float fz, int iw, float fw) {
  const int n[4] = {ix, iy, iz, iw};
  const float f[4] = {fx, fy, fz, fw};
  int nIndex = 0;
  for (int i = 0; i < data->ndim; ++i) {
    nIndex = data->map[(nIndex + n[i]) & 0xFF];
  }
  float value = 0;
  for (int i = 0; i < data->ndim; ++i) {
    value += data->buffer[nIndex][i] * f[i];
  }
  return value;
}

#define DEFAULT_SEED 0x15687436
static const float DELTA = 1e-6f;
// A generic macro to swap two variables.
#define GENERIC_SWAP(x, y, T) \
  {                           \
    T swap_variable__ = (x);  \
    (x) = (y);                \
    (y) = swap_variable__;    \
  }

#define FLOOR(a) ((a) > 0 ? (int)(a) : ((int)(a) - 1))
#define CUBIC(a) ((a) * (a) * (3 - 2 * (a)))

static void normalize(const TCOD_Noise* __restrict data, float* __restrict f) {
  float magnitude = 0;
  for (int i = 0; i < data->ndim; ++i) {
    magnitude += f[i] * f[i];
  }
  magnitude = 1.0f / sqrtf(magnitude);
  for (int i = 0; i < data->ndim; ++i) {
    f[i] *= magnitude;
  }
}

TCOD_Noise* TCOD_noise_new(int ndim, float hurst, float lacunarity, TCOD_Random* random) {
  TCOD_Noise* data = calloc(1, sizeof(*data));
  if (!data) {
    TCOD_set_errorv("Out of memory.");
    return NULL;
  }
  data->rand = random ? random : TCOD_random_get_instance();
  data->ndim = ndim;
  for (int i = 0; i < 256; ++i) {
    data->map[i] = (unsigned char)i;
    for (int j = 0; j < data->ndim; ++j) {
      data->buffer[i][j] = TCOD_random_get_float(data->rand, -0.5, 0.5);
    }
    normalize(data, data->buffer[i]);
  }

  for (int i = 255; i >= 0; --i) {
    const int j = TCOD_random_get_int(data->rand, 0, 255);
    GENERIC_SWAP(data->map[i], data->map[j], unsigned char);
  }

  data->H = hurst;
  data->lacunarity = lacunarity;
  float f = 1;
  for (int i = 0; i < TCOD_NOISE_MAX_OCTAVES; i++) {
    /*exponent[i] = powf(f, -H); */
    data->exponent[i] = 1.0f / f;
    f *= lacunarity;
  }
  data->noise_type = TCOD_NOISE_DEFAULT;
  return data;
}

static float TCOD_noise_perlin(TCOD_Noise* __restrict data, const float* __restrict f) {
  int n[TCOD_NOISE_MAX_DIMENSIONS]; /* Indexes to pass to lattice function */
  float r[TCOD_NOISE_MAX_DIMENSIONS]; /* Remainders to pass to lattice function */
  float w[TCOD_NOISE_MAX_DIMENSIONS]; /* Cubic values to pass to interpolation function */
  for (int i = 0; i < data->ndim; ++i) {
    n[i] = FLOOR(f[i]);
    r[i] = f[i] - n[i];
    w[i] = CUBIC(r[i]);
  }

  float value;
  switch (data->ndim) {
    case 1:
      value = TCOD_LERP(
          lattice(data, n[0], r[0], 0, 0, 0, 0, 0, 0), lattice(data, n[0] + 1, r[0] - 1, 0, 0, 0, 0, 0, 0), w[0]);
      break;
    case 2:
      value = TCOD_LERP(
          TCOD_LERP(
              lattice(data, n[0], r[0], n[1], r[1], 0, 0, 0, 0),
              lattice(data, n[0] + 1, r[0] - 1, n[1], r[1], 0, 0, 0, 0),
              w[0]),
          TCOD_LERP(
              lattice(data, n[0], r[0], n[1] + 1, r[1] - 1, 0, 0, 0, 0),
              lattice(data, n[0] + 1, r[0] - 1, n[1] + 1, r[1] - 1, 0, 0, 0, 0),
              w[0]),
          w[1]);
      break;
    case 3:
      value = TCOD_LERP(
          TCOD_LERP(
              TCOD_LERP(
                  lattice(data, n[0], r[0], n[1], r[1], n[2], r[2], 0, 0),
                  lattice(data, n[0] + 1, r[0] - 1, n[1], r[1], n[2], r[2], 0, 0),
                  w[0]),
              TCOD_LERP(
                  lattice(data, n[0], r[0], n[1] + 1, r[1] - 1, n[2], r[2], 0, 0),
                  lattice(data, n[0] + 1, r[0] - 1, n[1] + 1, r[1] - 1, n[2], r[2], 0, 0),
                  w[0]),
              w[1]),
          TCOD_LERP(
              TCOD_LERP(
                  lattice(data, n[0], r[0], n[1], r[1], n[2] + 1, r[2] - 1, 0, 0),
                  lattice(data, n[0] + 1, r[0] - 1, n[1], r[1], n[2] + 1, r[2] - 1, 0, 0),
                  w[0]),
              TCOD_LERP(
                  lattice(data, n[0], r[0], n[1] + 1, r[1] - 1, n[2] + 1, r[2] - 1, 0, 0),
                  lattice(data, n[0] + 1, r[0] - 1, n[1] + 1, r[1] - 1, n[2] + 1, r[2] - 1, 0, 0),
                  w[0]),
              w[1]),
          w[2]);
      break;
    case 4:
      value = TCOD_LERP(
          TCOD_LERP(
              TCOD_LERP(
                  TCOD_LERP(
                      lattice(data, n[0], r[0], n[1], r[1], n[2], r[2], n[3], r[3]),
                      lattice(data, n[0] + 1, r[0] - 1, n[1], r[1], n[2], r[2], n[3], r[3]),
                      w[0]),
                  TCOD_LERP(
                      lattice(data, n[0], r[0], n[1] + 1, r[1] - 1, n[2], r[2], n[3], r[3]),
                      lattice(data, n[0] + 1, r[0] - 1, n[1] + 1, r[1] - 1, n[2], r[2], n[3], r[3]),
                      w[0]),
                  w[1]),
              TCOD_LERP(
                  TCOD_LERP(
                      lattice(data, n[0], r[0], n[1], r[1], n[2] + 1, r[2] - 1, n[3], r[3]),
                      lattice(data, n[0] + 1, r[0] - 1, n[1], r[1], n[2] + 1, r[2] - 1, n[3], r[3]),
                      w[0]),
                  TCOD_LERP(
                      lattice(data, n[0], r[0], n[1] + 1, r[1] - 1, n[2] + 1, r[2] - 1, 0, 0),
                      lattice(data, n[0] + 1, r[0] - 1, n[1] + 1, r[1] - 1, n[2] + 1, r[2] - 1, n[3], r[3]),
                      w[0]),
                  w[1]),
              w[2]),
          TCOD_LERP(
              TCOD_LERP(
                  TCOD_LERP(
                      lattice(data, n[0], r[0], n[1], r[1], n[2], r[2], n[3] + 1, r[3] - 1),
                      lattice(data, n[0] + 1, r[0] - 1, n[1], r[1], n[2], r[2], n[3] + 1, r[3] - 1),
                      w[0]),
                  TCOD_LERP(
                      lattice(data, n[0], r[0], n[1] + 1, r[1] - 1, n[2], r[2], n[3] + 1, r[3] - 1),
                      lattice(data, n[0] + 1, r[0] - 1, n[1] + 1, r[1] - 1, n[2], r[2], n[3] + 1, r[3] - 1),
                      w[0]),
                  w[1]),
              TCOD_LERP(
                  TCOD_LERP(
                      lattice(data, n[0], r[0], n[1], r[1], n[2] + 1, r[2] - 1, n[3] + 1, r[3] - 1),
                      lattice(data, n[0] + 1, r[0] - 1, n[1], r[1], n[2] + 1, r[2] - 1, n[3] + 1, r[3] - 1),
                      w[0]),
                  TCOD_LERP(
                      lattice(data, n[0], r[0], n[1] + 1, r[1] - 1, n[2] + 1, r[2] - 1, 0, 0),
                      lattice(data, n[0] + 1, r[0] - 1, n[1] + 1, r[1] - 1, n[2] + 1, r[2] - 1, n[3] + 1, r[3] - 1),
                      w[0]),
                  w[1]),
              w[2]),
          w[3]);
      break;
    default:
      return NAN;
  }
  return clamp_signed_f(value);
}

static int absmod(int x, int n) {
  const int m = x % n;
  return m < 0 ? m + n : m;
}

/* simplex noise, adapted from Ken Perlin's presentation at Siggraph 2001 */
/* and Stefan Gustavson implementation */

#define TCOD_NOISE_SIMPLEX_GRADIENT_1D(n, h, x) \
  {                                             \
    h &= 0xF;                                   \
    float grad = 1.0f + (h & 7);                \
    if (h & 8) grad = -grad;                    \
    n = grad * x;                               \
  }

#define TCOD_NOISE_SIMPLEX_GRADIENT_2D(n, h, x, y) \
  {                                                \
    float u;                                       \
    float v;                                       \
    h &= 0x7;                                      \
    if (h < 4) {                                   \
      u = x;                                       \
      v = 2.0f * y;                                \
    } else {                                       \
      u = y;                                       \
      v = 2.0f * x;                                \
    }                                              \
    n = ((h & 1) ? -u : u) + ((h & 2) ? -v : v);   \
  }

#define TCOD_NOISE_SIMPLEX_GRADIENT_3D(n, h, x, y, z)           \
  {                                                             \
    h &= 0xF;                                                   \
    const float u = (h < 8 ? x : y);                            \
    const float v = (h < 4 ? y : (h == 12 || h == 14 ? x : z)); \
    n = ((h & 1) ? -u : u) + ((h & 2) ? -v : v);                \
  }

#define TCOD_NOISE_SIMPLEX_GRADIENT_4D(n, h, x, y, z, t)              \
  {                                                                   \
    h &= 0x1F;                                                        \
    const float u = (h < 24 ? x : y);                                 \
    const float v = (h < 16 ? y : z);                                 \
    const float w = (h < 8 ? z : t);                                  \
    n = ((h & 1) ? -u : u) + ((h & 2) ? -v : v) + ((h & 4) ? -w : w); \
  }

static const float simplex[64][4] = {
    {0, 1, 2, 3}, {0, 1, 3, 2}, {0, 0, 0, 0}, {0, 2, 3, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 2, 3, 0},
    {0, 2, 1, 3}, {0, 0, 0, 0}, {0, 3, 1, 2}, {0, 3, 2, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 3, 2, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {1, 2, 0, 3}, {0, 0, 0, 0}, {1, 3, 0, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {2, 3, 0, 1}, {2, 3, 1, 0},
    {1, 0, 2, 3}, {1, 0, 3, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {2, 0, 3, 1}, {0, 0, 0, 0}, {2, 1, 3, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {2, 0, 1, 3}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {3, 0, 1, 2}, {3, 0, 2, 1}, {0, 0, 0, 0}, {3, 1, 2, 0},
    {2, 1, 0, 3}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {3, 1, 0, 2}, {0, 0, 0, 0}, {3, 2, 0, 1}, {3, 2, 1, 0},

};

static float TCOD_noise_simplex(TCOD_Noise* __restrict data, const float* __restrict f) {
  switch (data->ndim) {
    case 1: {
      int i0 = (int)FLOOR(f[0] * SIMPLEX_SCALE);
      int i1 = i0 + 1;
      const float x0 = f[0] * SIMPLEX_SCALE - i0;
      const float x1 = x0 - 1.0f;
      float t0 = 1.0f - x0 * x0;
      float t1 = 1.0f - x1 * x1;
      t0 = t0 * t0;
      t1 = t1 * t1;
      i0 = data->map[i0 & 0xFF];
      float n0;
      TCOD_NOISE_SIMPLEX_GRADIENT_1D(n0, i0, x0);
      n0 *= t0 * t0;
      i1 = data->map[i1 & 0xFF];
      float n1;
      TCOD_NOISE_SIMPLEX_GRADIENT_1D(n1, i1, x1);
      n1 *= t1 * t1;
      return clamp_signed_f(0.25f * (n0 + n1));
    } break;
    case 2: {
      static const float F2 = 0.366025403f;  // 0.5f * (sqrtf(3.0f)-1.0f);
      static const float G2 = 0.211324865f;  // (3.0f - sqrtf(3.0f))/6.0f;
      const float s = (f[0] + f[1]) * F2 * SIMPLEX_SCALE;
      const float xs = f[0] * SIMPLEX_SCALE + s;
      const float ys = f[1] * SIMPLEX_SCALE + s;
      const int i = FLOOR(xs);
      const int j = FLOOR(ys);
      const float t = (i + j) * G2;
      const float xo = i - t;
      const float yo = j - t;
      const float x0 = f[0] * SIMPLEX_SCALE - xo;
      const float y0 = f[1] * SIMPLEX_SCALE - yo;
      const int ii = absmod(i, 256);
      const int jj = absmod(j, 256);
      int i1;
      int j1;
      if (x0 > y0) {
        i1 = 1;
        j1 = 0;
      } else {
        i1 = 0;
        j1 = 1;
      }
      const float x1 = x0 - i1 + G2;
      const float y1 = y0 - j1 + G2;
      const float x2 = x0 - 1.0f + 2.0f * G2;
      const float y2 = y0 - 1.0f + 2.0f * G2;
      float t0 = 0.5f - x0 * x0 - y0 * y0;
      float n0 = 0.0f;
      if (t0 >= 0.0f) {
        int idx = (ii + data->map[jj]) & 0xFF;
        t0 *= t0;
        idx = data->map[idx];
        TCOD_NOISE_SIMPLEX_GRADIENT_2D(n0, idx, x0, y0);
        n0 *= t0 * t0;
      }
      float t1 = 0.5f - x1 * x1 - y1 * y1;
      float n1 = 0.0f;
      if (t1 >= 0.0f) {
        int idx = (ii + i1 + data->map[(jj + j1) & 0xFF]) & 0xFF;
        t1 *= t1;
        idx = data->map[idx];
        TCOD_NOISE_SIMPLEX_GRADIENT_2D(n1, idx, x1, y1);
        n1 *= t1 * t1;
      }
      float t2 = 0.5f - x2 * x2 - y2 * y2;
      float n2 = 0.0f;
      if (t2 >= 0.0f) {
        int idx = (ii + 1 + data->map[(jj + 1) & 0xFF]) & 0xFF;
        t2 *= t2;
        idx = data->map[idx];
        TCOD_NOISE_SIMPLEX_GRADIENT_2D(n2, idx, x2, y2);
        n2 *= t2 * t2;
      }
      return clamp_signed_f(40.0f * (n0 + n1 + n2));
    } break;
    case 3: {
      static const float F3 = 0.333333333f;
      static const float G3 = 0.166666667f;
      const float s = (f[0] + f[1] + f[2]) * F3 * SIMPLEX_SCALE;
      const float xs = f[0] * SIMPLEX_SCALE + s;
      const float ys = f[1] * SIMPLEX_SCALE + s;
      const float zs = f[2] * SIMPLEX_SCALE + s;
      const int i = FLOOR(xs);
      const int j = FLOOR(ys);
      const int k = FLOOR(zs);
      const float t = (float)(i + j + k) * G3;
      const float xo = i - t;
      const float yo = j - t;
      const float zo = k - t;
      const float x0 = f[0] * SIMPLEX_SCALE - xo;
      const float y0 = f[1] * SIMPLEX_SCALE - yo;
      const float z0 = f[2] * SIMPLEX_SCALE - zo;
      int i1;
      int j1;
      int k1;
      int i2;
      int j2;
      int k2;
      if (x0 >= y0) {
        if (y0 >= z0) {
          i1 = 1;
          j1 = 0;
          k1 = 0;
          i2 = 1;
          j2 = 1;
          k2 = 0;
        } else if (x0 >= z0) {
          i1 = 1;
          j1 = 0;
          k1 = 0;
          i2 = 1;
          j2 = 0;
          k2 = 1;
        } else {
          i1 = 0;
          j1 = 0;
          k1 = 1;
          i2 = 1;
          j2 = 0;
          k2 = 1;
        }
      } else {
        if (y0 < z0) {
          i1 = 0;
          j1 = 0;
          k1 = 1;
          i2 = 0;
          j2 = 1;
          k2 = 1;
        } else if (x0 < z0) {
          i1 = 0;
          j1 = 1;
          k1 = 0;
          i2 = 0;
          j2 = 1;
          k2 = 1;
        } else {
          i1 = 0;
          j1 = 1;
          k1 = 0;
          i2 = 1;
          j2 = 1;
          k2 = 0;
        }
      }
      const float x1 = x0 - i1 + G3;
      const float y1 = y0 - j1 + G3;
      const float z1 = z0 - k1 + G3;
      const float x2 = x0 - i2 + 2.0f * G3;
      const float y2 = y0 - j2 + 2.0f * G3;
      const float z2 = z0 - k2 + 2.0f * G3;
      const float x3 = x0 - 1.0f + 3.0f * G3;
      const float y3 = y0 - 1.0f + 3.0f * G3;
      const float z3 = z0 - 1.0f + 3.0f * G3;
      const int ii = absmod(i, 256);
      const int jj = absmod(j, 256);
      const int kk = absmod(k, 256);
      float t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0;
      float n0 = 0.0f;
      if (t0 >= 0.0f) {
        int idx = data->map[(ii + data->map[(jj + data->map[kk]) & 0xFF]) & 0xFF];
        t0 *= t0;
        TCOD_NOISE_SIMPLEX_GRADIENT_3D(n0, idx, x0, y0, z0);
        n0 *= t0 * t0;
      }
      float t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;
      float n1 = 0.0f;
      if (t1 >= 0.0f) {
        int idx = data->map[(ii + i1 + data->map[(jj + j1 + data->map[(kk + k1) & 0xFF]) & 0xFF]) & 0xFF];
        t1 *= t1;
        TCOD_NOISE_SIMPLEX_GRADIENT_3D(n1, idx, x1, y1, z1);
        n1 *= t1 * t1;
      }
      float t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2;
      float n2 = 0.0f;
      if (t2 >= 0.0f) {
        int idx = data->map[(ii + i2 + data->map[(jj + j2 + data->map[(kk + k2) & 0xFF]) & 0xFF]) & 0xFF];
        t2 *= t2;
        TCOD_NOISE_SIMPLEX_GRADIENT_3D(n2, idx, x2, y2, z2);
        n2 *= t2 * t2;
      }
      float t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3;
      float n3 = 0.0f;
      if (t3 >= 0.0f) {
        int idx = data->map[(ii + 1 + data->map[(jj + 1 + data->map[(kk + 1) & 0xFF]) & 0xFF]) & 0xFF];
        t3 *= t3;
        TCOD_NOISE_SIMPLEX_GRADIENT_3D(n3, idx, x3, y3, z3);
        n3 *= t3 * t3;
      }
      return clamp_signed_f(32.0f * (n0 + n1 + n2 + n3));

    } break;
    case 4: {
      static const float F4 = 0.309016994f;  // (sqrtf(5.0f)-1.0f)/4.0f
      static const float G4 = 0.138196601f;  // (5.0f - sqrtf(5.0f))/20.0f
      const float s = (f[0] + f[1] + f[2] + f[3]) * F4 * SIMPLEX_SCALE;
      const float xs = f[0] * SIMPLEX_SCALE + s;
      const float ys = f[1] * SIMPLEX_SCALE + s;
      const float zs = f[2] * SIMPLEX_SCALE + s;
      const float ws = f[3] * SIMPLEX_SCALE + s;
      const int i = FLOOR(xs);
      const int j = FLOOR(ys);
      const int k = FLOOR(zs);
      const int l = FLOOR(ws);
      const float t = (float)(i + j + k + l) * G4;
      const float xo = i - t;
      const float yo = j - t;
      const float zo = k - t;
      const float wo = l - t;
      const float x0 = f[0] * SIMPLEX_SCALE - xo;
      const float y0 = f[1] * SIMPLEX_SCALE - yo;
      const float z0 = f[2] * SIMPLEX_SCALE - zo;
      const float w0 = f[3] * SIMPLEX_SCALE - wo;
      const int c1 = (x0 > y0 ? 32 : 0);
      const int c2 = (x0 > z0 ? 16 : 0);
      const int c3 = (y0 > z0 ? 8 : 0);
      const int c4 = (x0 > w0 ? 4 : 0);
      const int c5 = (y0 > w0 ? 2 : 0);
      const int c6 = (z0 > w0 ? 1 : 0);
      const int c = c1 + c2 + c3 + c4 + c5 + c6;

      const int i1 = simplex[c][0] >= 3 ? 1 : 0;
      const int j1 = simplex[c][1] >= 3 ? 1 : 0;
      const int k1 = simplex[c][2] >= 3 ? 1 : 0;
      const int l1 = simplex[c][3] >= 3 ? 1 : 0;

      const int i2 = simplex[c][0] >= 2 ? 1 : 0;
      const int j2 = simplex[c][1] >= 2 ? 1 : 0;
      const int k2 = simplex[c][2] >= 2 ? 1 : 0;
      const int l2 = simplex[c][3] >= 2 ? 1 : 0;

      const int i3 = simplex[c][0] >= 1 ? 1 : 0;
      const int j3 = simplex[c][1] >= 1 ? 1 : 0;
      const int k3 = simplex[c][2] >= 1 ? 1 : 0;
      const int l3 = simplex[c][3] >= 1 ? 1 : 0;

      const float x1 = x0 - i1 + G4;
      const float y1 = y0 - j1 + G4;
      const float z1 = z0 - k1 + G4;
      const float w1 = w0 - l1 + G4;
      const float x2 = x0 - i2 + 2.0f * G4;
      const float y2 = y0 - j2 + 2.0f * G4;
      const float z2 = z0 - k2 + 2.0f * G4;
      const float w2 = w0 - l2 + 2.0f * G4;
      const float x3 = x0 - i3 + 3.0f * G4;
      const float y3 = y0 - j3 + 3.0f * G4;
      const float z3 = z0 - k3 + 3.0f * G4;
      const float w3 = w0 - l3 + 3.0f * G4;
      const float x4 = x0 - 1.0f + 4.0f * G4;
      const float y4 = y0 - 1.0f + 4.0f * G4;
      const float z4 = z0 - 1.0f + 4.0f * G4;
      const float w4 = w0 - 1.0f + 4.0f * G4;

      const int ii = absmod(i, 256);
      const int jj = absmod(j, 256);
      const int kk = absmod(k, 256);
      const int ll = absmod(l, 256);

      float t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0 - w0 * w0;
      float n0 = 0.0f;
      if (t0 >= 0.0f) {
        int idx = data->map[(ii + data->map[(jj + data->map[(kk + data->map[ll]) & 0xFF]) & 0xFF]) & 0xFF];
        t0 *= t0;
        TCOD_NOISE_SIMPLEX_GRADIENT_4D(n0, idx, x0, y0, z0, w0);
        n0 *= t0 * t0;
      }
      float t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1 - w1 * w1;
      float n1 = 0.0f;
      if (t1 >= 0.0f) {
        int idx =
            data->map
                [(ii + i1 + data->map[(jj + j1 + data->map[(kk + k1 + data->map[(ll + l1) & 0xFF]) & 0xFF]) & 0xFF]) &
                 0xFF];
        t1 *= t1;
        TCOD_NOISE_SIMPLEX_GRADIENT_4D(n1, idx, x1, y1, z1, w1);
        n1 *= t1 * t1;
      }
      float t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2 - w2 * w2;
      float n2 = 0.0f;
      if (t2 >= 0.0f) {
        int idx =
            data->map
                [(ii + i2 + data->map[(jj + j2 + data->map[(kk + k2 + data->map[(ll + l2) & 0xFF]) & 0xFF]) & 0xFF]) &
                 0xFF];
        t2 *= t2;
        TCOD_NOISE_SIMPLEX_GRADIENT_4D(n2, idx, x2, y2, z2, w2);
        n2 *= t2 * t2;
      }
      float t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3 - w3 * w3;
      float n3 = 0.0f;
      if (t3 >= 0.0f) {
        int idx =
            data->map
                [(ii + i3 + data->map[(jj + j3 + data->map[(kk + k3 + data->map[(ll + l3) & 0xFF]) & 0xFF]) & 0xFF]) &
                 0xFF];
        t3 *= t3;
        TCOD_NOISE_SIMPLEX_GRADIENT_4D(n3, idx, x3, y3, z3, w3);
        n3 *= t3 * t3;
      }
      float t4 = 0.6f - x4 * x4 - y4 * y4 - z4 * z4 - w4 * w4;
      float n4 = 0.0f;
      if (t4 >= 0.0f) {
        int idx = data->map
                      [(ii + 1 + data->map[(jj + 1 + data->map[(kk + 1 + data->map[(ll + 1) & 0xFF]) & 0xFF]) & 0xFF]) &
                       0xFF];
        t4 *= t4;
        TCOD_NOISE_SIMPLEX_GRADIENT_4D(n4, idx, x4, y4, z4, w4);
        n4 *= t4 * t4;
      }
      return clamp_signed_f(27.0f * (n0 + n1 + n2 + n3 + n4));

    } break;
    default:
      return NAN;
  }
}

static float TCOD_noise_fbm_int(
    TCOD_Noise* __restrict noise, const float* __restrict f, float octaves, TCOD_noise_func_t func) {
  float tf[TCOD_NOISE_MAX_DIMENSIONS] = {0, 0, 0, 0};
  /* Initialize locals */
  for (int i = 0; i < noise->ndim; ++i) {
    tf[i] = f[i];
  }

  /* Inner loop of spectral construction, where the fractal is built */
  float value = 0;
  int i;
  for (i = 0; i < (int)octaves; ++i) {
    value += func(noise, tf) * noise->exponent[i];
    for (int j = 0; j < noise->ndim; ++j) {
      tf[j] *= noise->lacunarity;
    }
  }

  /* Take care of remainder in octaves */
  octaves -= (int)octaves;
  if (octaves > DELTA) {
    value += octaves * func(noise, tf) * noise->exponent[i];
  }
  return clamp_signed_f(value);
}

static float TCOD_noise_fbm_perlin(TCOD_Noise* __restrict noise, const float* __restrict f, float octaves) {
  return TCOD_noise_fbm_int(noise, f, octaves, TCOD_noise_perlin);
}

static float TCOD_noise_fbm_simplex(TCOD_Noise* __restrict noise, const float* __restrict f, float octaves) {
  return TCOD_noise_fbm_int(noise, f, octaves, TCOD_noise_simplex);
}

static float TCOD_noise_turbulence_int(
    TCOD_Noise* __restrict noise, const float* __restrict f, float octaves, TCOD_noise_func_t func) {
  float tf[TCOD_NOISE_MAX_DIMENSIONS] = {0, 0, 0, 0};
  /* Initialize locals */
  for (int i = 0; i < noise->ndim; ++i) tf[i] = f[i];

  /* Inner loop of spectral construction, where the fractal is built */
  int i;
  float value = 0;
  for (i = 0; i < (int)octaves; ++i) {
    const float noise_value = func(noise, tf);
    value += TCOD_ABS(noise_value) * noise->exponent[i];
    for (int j = 0; j < noise->ndim; ++j) {
      tf[j] *= noise->lacunarity;
    }
  }

  /* Take care of remainder in octaves */
  octaves -= (int)octaves;
  if (octaves > DELTA) {
    const float noise_value = func(noise, tf);
    value += octaves * TCOD_ABS(noise_value) * noise->exponent[i];
  }
  return clamp_signed_f(value);
}

static float TCOD_noise_turbulence_perlin(TCOD_Noise* __restrict noise, const float* __restrict f, float octaves) {
  return TCOD_noise_turbulence_int(noise, f, octaves, TCOD_noise_perlin);
}

static float TCOD_noise_turbulence_simplex(TCOD_Noise* __restrict noise, const float* __restrict f, float octaves) {
  return TCOD_noise_turbulence_int(noise, f, octaves, TCOD_noise_simplex);
}

/* wavelet noise, adapted from Robert L. Cook and Tony Derose 'Wavelet noise' paper */

static void TCOD_noise_wavelet_downsample(const float* __restrict from, float* __restrict to, int stride) {
  static const float a_coefficients[2 * WAVELET_ARAD] = {
      0.000334f,  -0.001528f, 0.000410f,  0.003545f,  -0.000938f, -0.008233f, 0.002172f,  0.019120f,
      -0.005040f, -0.044412f, 0.011655f,  0.103311f,  -0.025936f, -0.243780f, 0.033979f,  0.655340f,
      0.655340f,  0.033979f,  -0.243780f, -0.025936f, 0.103311f,  0.011655f,  -0.044412f, -0.005040f,
      0.019120f,  0.002172f,  -0.008233f, -0.000938f, 0.003546f,  0.000410f,  -0.001528f, 0.000334f,
  };
  static const float* a = &a_coefficients[WAVELET_ARAD];
  for (int i = 0; i < WAVELET_TILE_SIZE / 2; ++i) {
    to[i * stride] = 0;
    for (int k = 2 * i - WAVELET_ARAD; k < 2 * i + WAVELET_ARAD; ++k) {
      to[i * stride] += a[k - 2 * i] * from[absmod(k, WAVELET_TILE_SIZE) * stride];
    }
  }
}

static void TCOD_noise_wavelet_upsample(const float* __restrict from, float* __restrict to, int stride) {
  static const float p_coefficient[4] = {0.25f, 0.75f, 0.75f, 0.25f};
  static const float* p = &p_coefficient[2];
  for (int i = 0; i < WAVELET_TILE_SIZE; ++i) {
    to[i * stride] = 0;
    for (int k = i / 2; k < i / 2 + 1; ++k) {
      to[i * stride] += p[i - 2 * k] * from[absmod(k, WAVELET_TILE_SIZE / 2) * stride];
    }
  }
}

static void TCOD_noise_wavelet_init(TCOD_Noise* __restrict data) {
  static const int sz = WAVELET_TILE_SIZE * WAVELET_TILE_SIZE * WAVELET_TILE_SIZE * sizeof(float);
  float* temp1 = malloc(sz);
  float* temp2 = malloc(sz);
  float* noise = malloc(sz);
  for (int i = 0; i < WAVELET_TILE_SIZE * WAVELET_TILE_SIZE * WAVELET_TILE_SIZE; ++i) {
    noise[i] = TCOD_random_get_float(data->rand, -1.0f, 1.0f);
  }
  for (int iy = 0; iy < WAVELET_TILE_SIZE; ++iy) {
    for (int iz = 0; iz < WAVELET_TILE_SIZE; ++iz) {
      const int i = iy * WAVELET_TILE_SIZE + iz * WAVELET_TILE_SIZE * WAVELET_TILE_SIZE;
      TCOD_noise_wavelet_downsample(&noise[i], &temp1[i], 1);
      TCOD_noise_wavelet_upsample(&temp1[i], &temp2[i], 1);
    }
  }
  for (int ix = 0; ix < WAVELET_TILE_SIZE; ++ix) {
    for (int iz = 0; iz < WAVELET_TILE_SIZE; ++iz) {
      const int i = ix + iz * WAVELET_TILE_SIZE * WAVELET_TILE_SIZE;
      TCOD_noise_wavelet_downsample(&temp2[i], &temp1[i], WAVELET_TILE_SIZE);
      TCOD_noise_wavelet_upsample(&temp1[i], &temp2[i], WAVELET_TILE_SIZE);
    }
  }
  for (int ix = 0; ix < WAVELET_TILE_SIZE; ++ix) {
    for (int iy = 0; iy < WAVELET_TILE_SIZE; ++iy) {
      const int i = ix + iy * WAVELET_TILE_SIZE;
      TCOD_noise_wavelet_downsample(&temp2[i], &temp1[i], WAVELET_TILE_SIZE * WAVELET_TILE_SIZE);
      TCOD_noise_wavelet_upsample(&temp1[i], &temp2[i], WAVELET_TILE_SIZE * WAVELET_TILE_SIZE);
    }
  }
  for (int i = 0; i < WAVELET_TILE_SIZE * WAVELET_TILE_SIZE * WAVELET_TILE_SIZE; ++i) {
    noise[i] -= temp2[i];
  }
  int offset = WAVELET_TILE_SIZE / 2;
  if ((offset & 1) == 0) ++offset;
  for (int i = 0, ix = 0; ix < WAVELET_TILE_SIZE; ++ix) {
    for (int iy = 0; iy < WAVELET_TILE_SIZE; ++iy) {
      for (int iz = 0; iz < WAVELET_TILE_SIZE; ++iz) {
        temp1[i++] = noise
            [absmod(ix + offset, WAVELET_TILE_SIZE) + absmod(iy + offset, WAVELET_TILE_SIZE) * WAVELET_TILE_SIZE +
             absmod(iz + offset, WAVELET_TILE_SIZE) * WAVELET_TILE_SIZE * WAVELET_TILE_SIZE];
      }
    }
  }
  for (int i = 0; i < WAVELET_TILE_SIZE * WAVELET_TILE_SIZE * WAVELET_TILE_SIZE; ++i) {
    noise[i] += temp1[i];
  }
  data->waveletTileData = noise;
  free(temp1);
  free(temp2);
}

static float TCOD_noise_wavelet(TCOD_Noise* __restrict data, const float* __restrict f) {
  const int n = WAVELET_TILE_SIZE;
  if (data->ndim <= 0 || data->ndim > 3) {
    return NAN; /* not supported */
  }
  if (!data->waveletTileData) {
    TCOD_noise_wavelet_init(data);
  }
  float pf[3] = {0, 0, 0};
  for (int i = 0; i < data->ndim; ++i) {
    pf[i] = f[i] * WAVELET_SCALE;
  }
  int mid[3];
  float w[3][3];
  for (int i = 0; i < 3; ++i) {
    mid[i] = (int)ceil(pf[i] - 0.5f);
    const float t = mid[i] - (pf[i] - 0.5f);
    w[i][0] = t * t * 0.5f;
    w[i][2] = (1.0f - t) * (1.0f - t) * 0.5f;
    w[i][1] = 1.0f - w[i][0] - w[i][2];
  }
  float result = 0.0f;
  int p[3];
  int c[3];
  for (p[2] = -1; p[2] <= 1; ++p[2]) {
    for (p[1] = -1; p[1] <= 1; ++p[1]) {
      for (p[0] = -1; p[0] <= 1; ++p[0]) {
        float weight = 1.0f;
        for (int i = 0; i < 3; ++i) {
          c[i] = absmod(mid[i] + p[i], n);
          weight *= w[i][p[i] + 1];
        }
        result += weight * data->waveletTileData[c[2] * n * n + c[1] * n + c[0]];
      }
    }
  }
  return clamp_signed_f(result);
}

static float TCOD_noise_fbm_wavelet(TCOD_Noise* __restrict noise, const float* __restrict f, float octaves) {
  return TCOD_noise_fbm_int(noise, f, octaves, TCOD_noise_wavelet);
}

static float TCOD_noise_turbulence_wavelet(TCOD_Noise* __restrict noise, const float* __restrict f, float octaves) {
  return TCOD_noise_turbulence_int(noise, f, octaves, TCOD_noise_wavelet);
}

void TCOD_noise_set_type(TCOD_Noise* __restrict noise, TCOD_noise_type_t type) { noise->noise_type = type; }

float TCOD_noise_get_ex(TCOD_Noise* __restrict noise, const float* __restrict f, TCOD_noise_type_t type) {
  switch (type ? type : noise->noise_type) {
    case (TCOD_NOISE_PERLIN):
      return TCOD_noise_perlin(noise, f);
    case TCOD_NOISE_DEFAULT:
    case (TCOD_NOISE_SIMPLEX):
      return TCOD_noise_simplex(noise, f);
    case (TCOD_NOISE_WAVELET):
      return TCOD_noise_wavelet(noise, f);
    default:
      return NAN;
  }
}

float TCOD_noise_get_fbm_ex(
    TCOD_Noise* __restrict noise, const float* __restrict f, float octaves, TCOD_noise_type_t type) {
  switch (type ? type : noise->noise_type) {
    case (TCOD_NOISE_PERLIN):
      return TCOD_noise_fbm_perlin(noise, f, octaves);
    case TCOD_NOISE_DEFAULT:
    case (TCOD_NOISE_SIMPLEX):
      return TCOD_noise_fbm_simplex(noise, f, octaves);
    case (TCOD_NOISE_WAVELET):
      return TCOD_noise_fbm_wavelet(noise, f, octaves);
    default:
      return NAN;
  }
}

float TCOD_noise_get_turbulence_ex(
    TCOD_Noise* __restrict noise, const float* __restrict f, float octaves, TCOD_noise_type_t type) {
  switch (type ? type : noise->noise_type) {
    case (TCOD_NOISE_PERLIN):
      return TCOD_noise_turbulence_perlin(noise, f, octaves);
    case TCOD_NOISE_DEFAULT:
    case (TCOD_NOISE_SIMPLEX):
      return TCOD_noise_turbulence_simplex(noise, f, octaves);
    case (TCOD_NOISE_WAVELET):
      return TCOD_noise_turbulence_wavelet(noise, f, octaves);
    default:
      return NAN;
  }
}

float TCOD_noise_get(TCOD_Noise* __restrict noise, const float* __restrict f) {
  return TCOD_noise_get_ex(noise, f, noise->noise_type);
}

float TCOD_noise_get_fbm(TCOD_Noise* __restrict noise, const float* __restrict f, float octaves) {
  return TCOD_noise_get_fbm_ex(noise, f, octaves, noise->noise_type);
}

float TCOD_noise_get_turbulence(TCOD_Noise* __restrict noise, const float* __restrict f, float octaves) {
  return TCOD_noise_get_turbulence_ex(noise, f, octaves, noise->noise_type);
}

void TCOD_noise_delete(TCOD_Noise* __restrict noise) {
  if (noise && noise->waveletTileData) {
    free(noise->waveletTileData);
  }
  free(noise);
}

void TCOD_noise_get_vectorized(
    TCOD_Noise* __restrict noise,
    TCOD_noise_type_t type,
    int n,
    float* __restrict x,
    float* __restrict y,
    float* __restrict z,
    float* __restrict w,
    float* __restrict out) {
  for (int i = 0; i < n; ++i) {
    const float point[4] = {
        x ? x[i] : 0,
        y && noise->ndim >= 2 ? y[i] : 0,
        z && noise->ndim >= 3 ? z[i] : 0,
        w && noise->ndim >= 4 ? w[i] : 0,
    };
    switch (type ? type : noise->noise_type) {
      case (TCOD_NOISE_PERLIN):
        out[i] = TCOD_noise_perlin(noise, point);
        break;
      case TCOD_NOISE_DEFAULT:
      case (TCOD_NOISE_SIMPLEX):
        out[i] = TCOD_noise_simplex(noise, point);
        break;
      case (TCOD_NOISE_WAVELET):
        out[i] = TCOD_noise_wavelet(noise, point);
        break;
      default:
        out[i] = NAN;
        break;
    }
  }
}

void TCOD_noise_get_fbm_vectorized(
    TCOD_Noise* __restrict noise,
    TCOD_noise_type_t type,
    float octaves,
    int n,
    float* __restrict x,
    float* __restrict y,
    float* __restrict z,
    float* __restrict w,
    float* __restrict out) {
  for (int i = 0; i < n; ++i) {
    const float point[4] = {
        x ? x[i] : 0,
        y && noise->ndim >= 2 ? y[i] : 0,
        z && noise->ndim >= 3 ? z[i] : 0,
        w && noise->ndim >= 4 ? w[i] : 0,
    };
    switch (type ? type : noise->noise_type) {
      case (TCOD_NOISE_PERLIN):
        out[i] = TCOD_noise_fbm_perlin(noise, point, octaves);
        break;
      case TCOD_NOISE_DEFAULT:
      case (TCOD_NOISE_SIMPLEX):
        out[i] = TCOD_noise_fbm_simplex(noise, point, octaves);
        break;
      case (TCOD_NOISE_WAVELET):
        out[i] = TCOD_noise_fbm_wavelet(noise, point, octaves);
        break;
      default:
        out[i] = NAN;
        break;
    }
  }
}

void TCOD_noise_get_turbulence_vectorized(
    TCOD_Noise* __restrict noise,
    TCOD_noise_type_t type,
    float octaves,
    int n,
    float* __restrict x,
    float* __restrict y,
    float* __restrict z,
    float* __restrict w,
    float* __restrict out) {
  for (int i = 0; i < n; ++i) {
    const float point[4] = {
        x ? x[i] : 0,
        y && noise->ndim >= 2 ? y[i] : 0,
        z && noise->ndim >= 3 ? z[i] : 0,
        w && noise->ndim >= 4 ? w[i] : 0,
    };
    switch (type ? type : noise->noise_type) {
      case (TCOD_NOISE_PERLIN):
        out[i] = TCOD_noise_turbulence_perlin(noise, point, octaves);
        break;
      case TCOD_NOISE_DEFAULT:
      case (TCOD_NOISE_SIMPLEX):
        out[i] = TCOD_noise_turbulence_simplex(noise, point, octaves);
        break;
      case (TCOD_NOISE_WAVELET):
        out[i] = TCOD_noise_turbulence_wavelet(noise, point, octaves);
        break;
      default:
        out[i] = NAN;
        break;
    }
  }
}
