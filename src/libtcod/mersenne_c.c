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
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "libtcod_int.h"
#include "mersenne.h"
#include "utility.h"

static TCOD_Random* global_rng_instance = NULL;

// A generic macro to swap two variables.
#define GENERIC_SWAP(x, y, T) \
  {                           \
    T swap_variable__ = (x);  \
    (x) = (y);                \
    (y) = swap_variable__;    \
  }

// A generic macro to swap min and max if they are out-of-order.
#define SORT_MINMAX(min, max, T) \
  if ((min) > (max)) GENERIC_SWAP((min), (max), T);

/* initialize the mersenne twister array */
static void mt_init(uint32_t seed, uint32_t mt[624]) {
  mt[0] = seed;
  for (int i = 1; i < 624; ++i) {
    mt[i] = (1812433253 * (mt[i - 1] ^ (mt[i - 1] >> 30)) + i);
  }
}

/* get the next random value from the mersenne twister array */
static uint32_t mt_rand(uint32_t mt[624], int* cur_mt) {
  static const uint32_t MT_HIGH_BIT = 0x80000000UL;
  static const uint32_t MT_LOW_BITS = 0x7fffffffUL;
  uint32_t y;
  if (*cur_mt == 624) {
    /* our 624 sequence is finished. generate a new one */
    for (int i = 0; i < 623; ++i) {
      y = (mt[i] & MT_HIGH_BIT) | (mt[i + 1] & MT_LOW_BITS);
      if (y & 1) {
        /* odd y */
        mt[i] = mt[(i + 397) % 624] ^ (y >> 1) ^ 2567483615UL;
      } else {
        /* even y */
        mt[i] = mt[(i + 397) % 624] ^ (y >> 1);
      }
    }
    y = (mt[623] & MT_HIGH_BIT) | (mt[0] & MT_LOW_BITS);
    if (y & 1) {
      /* odd y */
      mt[623] = mt[396] ^ (y >> 1) ^ 2567483615UL;
    } else {
      mt[623] = mt[396] ^ (y >> 1);
    }

    *cur_mt = 0;
  }

  y = mt[(*cur_mt)++];
  y ^= (y >> 11);
  y ^= (y << 7) & 2636928640UL;
  y ^= (y << 15) & 4022730752UL;
  y ^= (y >> 18);
  return y;
}

/* string hashing function */
/* not used (yet)
static uint32_t hash(const char *data,int len) {
        uint32_t hash = 0;
        uint32_t x;
        int i;
        for(i = 0; i < len; data++, i++) {
                hash = (hash << 4) + (*data);
                if((x = hash & 0xF0000000L) != 0) {
                        hash ^= (x >> 24);
                        hash &= ~x;
                }
        }
        return (hash & 0x7FFFFFFF);
}
*/

/* get a random number from the CMWC */
TCOD_NODISCARD static uint32_t CMWC_GET_NUMBER(struct TCOD_Random_MT_CMWC* r) {
  r->cur = (r->cur + 1) & 4095;
  const uint64_t t = 18782LL * r->Q[r->cur] + r->c;
  r->c = (t >> 32);
  uint32_t x = (uint32_t)(t + r->c);
  if (x < r->c) {
    ++x;
    ++r->c;
  }
  if ((x + 1) == 0) {
    ++r->c;
    x = 0;
  }
  return r->Q[r->cur] = 0xfffffffe - x;
}

// Return a random uint32_t number.
static uint32_t get_random_u32(TCOD_Random* rng) {
  if (!rng) rng = TCOD_random_get_instance();
  switch (rng->algorithm) {
    case TCOD_RNG_MT:
      return mt_rand(rng->mt_cmwc.mt, &rng->mt_cmwc.cur_mt);
    case TCOD_RNG_CMWC:
      return CMWC_GET_NUMBER(&rng->mt_cmwc);
    default:
      return 0;
  }
}
// Return a random float between 0.0 and 1.0.
static float get_random_float(TCOD_Random* rng) { return get_random_u32(rng) * (1.0f / 0xffffffff); }
// Return a random double between 0.0 and 1.0.
static double get_random_double(TCOD_Random* rng) { return get_random_u32(rng) * (1.0 / 0xffffffff); }

TCOD_Random* TCOD_random_new(TCOD_random_algo_t algo) { return TCOD_random_new_from_seed(algo, (uint32_t)time(0)); }

TCOD_Random* TCOD_random_get_instance(void) {
  if (!global_rng_instance) global_rng_instance = TCOD_random_new(TCOD_RNG_CMWC);
  return global_rng_instance;
}

TCOD_Random* TCOD_random_new_from_seed(TCOD_random_algo_t algo, uint32_t seed) {
  struct TCOD_Random_MT_CMWC* r = calloc(1, sizeof(*r));
  /* Mersenne Twister */
  if (algo == TCOD_RNG_MT) {
    r->algorithm = TCOD_RNG_MT;
    r->cur_mt = 624;
    mt_init(seed, r->mt);
  } else { /* Complementary-Multiply-With-Carry or Generalised Feedback Shift Register */
    /* fill the Q array with pseudorandom seeds */
    uint32_t s = seed;
    for (int i = 0; i < 4096; ++i) r->Q[i] = s = (s * 1103515245) + 12345; /* glibc LCG */
    r->c = ((s * 1103515245) + 12345) % 809430660; /* this max value is recommended by George Marsaglia */
    r->cur = 0;
    r->algorithm = TCOD_RNG_CMWC;
  }
  r->distribution = TCOD_DISTRIBUTION_LINEAR;
  return (TCOD_Random*)r;
}

int TCOD_random_get_i(TCOD_Random* rng, int min, int max) {
  if (max == min) return min;
  SORT_MINMAX(min, max, int);
  const int delta = max - min + 1;
  return get_random_u32(rng) % delta + min;
}

float TCOD_random_get_f(TCOD_Random* rng, float min, float max) {
  if (max == min) return min;
  SORT_MINMAX(min, max, float);
  const float delta = max - min;
  return min + get_random_float(rng) * delta;
}

double TCOD_random_get_d(TCOD_Random* rng, double min, double max) {
  if (max == min) return min;
  SORT_MINMAX(min, max, double);
  if (!rng) rng = TCOD_random_get_instance();
  const double delta = max - min;
  return min + get_random_double(rng) * delta;
}

void TCOD_random_delete(TCOD_Random* rng) {
  TCOD_IFNOT(rng != NULL) return;
  if (rng == global_rng_instance) global_rng_instance = NULL;
  free(rng);
}
TCOD_Random* TCOD_random_save(TCOD_Random* rng) {
  TCOD_Random* backup = malloc(sizeof(*backup));
  if (!rng) rng = TCOD_random_get_instance();
  *backup = *rng;
  return backup;
}

void TCOD_random_restore(TCOD_Random* rng, TCOD_Random* backup) {
  if (!rng) rng = TCOD_random_get_instance();
  *rng = *backup;
}

/* Box-Muller transform (Gaussian distribution) */
double TCOD_random_get_gaussian_double(TCOD_Random* rng, double mean, double std_deviation) {
  if (!rng) rng = TCOD_random_get_instance();
  double x1, x2, w;
  do {
    x1 = get_random_double(rng) * 2.0 - 1.0;
    x2 = get_random_double(rng) * 2.0 - 1.0;
    w = x1 * x1 + x2 * x2;
  } while (w >= 1.0);
  w = sqrt((-2.0 * log(w)) / w);
  const double y1 = x1 * w;
  // const double y2 = x2 * w;  // y2 is discarded.
  return mean + y1 * std_deviation;
}

float TCOD_random_get_gaussian_float(TCOD_Random* rng, float mean, float std_deviation) {
  return (float)TCOD_random_get_gaussian_double(rng, (double)mean, (double)std_deviation);
}

int TCOD_random_get_gaussian_int(TCOD_Random* rng, int mean, int std_deviation) {
  const double num = TCOD_random_get_gaussian_double(rng, (double)mean, (double)std_deviation);
  return (num >= 0.0 ? (int)(num + 0.5) : (int)(num - 0.5));
}

/* Box-Muller, ranges */

double TCOD_random_get_gaussian_double_range(TCOD_Random* rng, double min, double max) {
  SORT_MINMAX(min, max, double);
  const double mean = (min + max) / 2;
  const double std_deviation = (max - min) / 6.0; /* 6.0 is used because of the three-sigma rule */
  const double ret = TCOD_random_get_gaussian_double(rng, mean, std_deviation);
  return TCOD_CLAMP(min, max, ret);
}

float TCOD_random_get_gaussian_float_range(TCOD_Random* rng, float min, float max) {
  SORT_MINMAX(min, max, float);
  return (float)TCOD_random_get_gaussian_double_range(rng, (double)min, (double)max);
}

int TCOD_random_get_gaussian_int_range(TCOD_Random* rng, int min, int max) {
  SORT_MINMAX(min, max, int);
  const double num = TCOD_random_get_gaussian_double_range(rng, (double)min, (double)max);
  const int ret = (num >= 0.0 ? (int)(num + 0.5) : (int)(num - 0.5));
  return TCOD_CLAMP(min, max, ret);
}

/* Box-Muller, ranges with a custom mean */

double TCOD_random_get_gaussian_double_range_custom(TCOD_Random* rng, double min, double max, double mean) {
  SORT_MINMAX(min, max, double);
  const double d1 = max - mean;
  const double d2 = mean - min;
  const double std_deviation = TCOD_MAX(d1, d2) / 3.0;
  const double ret = TCOD_random_get_gaussian_double(rng, mean, std_deviation);
  return TCOD_CLAMP(min, max, ret);
}

float TCOD_random_get_gaussian_float_range_custom(TCOD_Random* rng, float min, float max, float mean) {
  SORT_MINMAX(min, max, float);
  return (float)TCOD_random_get_gaussian_double_range_custom(rng, (double)min, (double)max, (double)mean);
}

int TCOD_random_get_gaussian_int_range_custom(TCOD_Random* rng, int min, int max, int mean) {
  SORT_MINMAX(min, max, int);
  const double num = TCOD_random_get_gaussian_double_range_custom(rng, (double)min, (double)max, (double)mean);
  const int ret = (num >= 0.0 ? (int)(num + 0.5) : (int)(num - 0.5));
  return TCOD_CLAMP(min, max, ret);
}

/* Box-Muller, inverted distribution */
double TCOD_random_get_gaussian_double_inv(TCOD_Random* rng, double mean, double std_deviation) {
  const double num = TCOD_random_get_gaussian_double(rng, mean, std_deviation);
  return (num >= mean ? num - (3 * std_deviation) : num + (3 * std_deviation));
}

float TCOD_random_get_gaussian_float_inv(TCOD_Random* rng, float mean, float std_deviation) {
  const float num = (float)TCOD_random_get_gaussian_double(rng, (double)mean, (double)std_deviation);
  return (num >= mean ? (num - (3 * std_deviation)) : (num + (3 * std_deviation)));
}

int TCOD_random_get_gaussian_int_inv(TCOD_Random* rng, int mean, int std_deviation) {
  const double num = TCOD_random_get_gaussian_double(rng, (double)mean, (double)std_deviation);
  const int i_num = (num >= 0.0 ? (int)(num + 0.5) : (int)(num - 0.5));
  return (num >= mean ? i_num - (3 * std_deviation) : i_num + (3 * std_deviation));
}

/* Box-Muller, ranges, inverted distribution */

double TCOD_random_get_gaussian_double_range_inv(TCOD_Random* rng, double min, double max) {
  SORT_MINMAX(min, max, double);
  const double mean = (min + max) / 2.0;
  const double std_deviation = (max - min) / 6.0; /* 6.0 is used because of the three-sigma rule */
  const double ret = TCOD_random_get_gaussian_double_inv(rng, mean, std_deviation);
  return TCOD_CLAMP(min, max, ret);
}

float TCOD_random_get_gaussian_float_range_inv(TCOD_Random* rng, float min, float max) {
  const float ret = (float)TCOD_random_get_gaussian_double_range_inv(rng, (double)min, (double)max);
  return TCOD_CLAMP(min, max, ret);
}

int TCOD_random_get_gaussian_int_range_inv(TCOD_Random* rng, int min, int max) {
  const double num = TCOD_random_get_gaussian_double_range_inv(rng, (double)min, (double)max);
  const int ret = (num >= 0.0 ? (int)(num + 0.5) : (int)(num - 0.5));
  return TCOD_CLAMP(min, max, ret);
}

/* Box-Muller, ranges with a custom mean, inverted distribution */

double TCOD_random_get_gaussian_double_range_custom_inv(TCOD_Random* rng, double min, double max, double mean) {
  SORT_MINMAX(min, max, double);
  const double d1 = max - mean;
  const double d2 = mean - min;
  const double std_deviation = TCOD_MAX(d1, d2) / 3.0;
  const double ret = TCOD_random_get_gaussian_double_inv(rng, mean, std_deviation);
  return TCOD_CLAMP(min, max, ret);
}

float TCOD_random_get_gaussian_float_range_custom_inv(TCOD_Random* rng, float min, float max, float mean) {
  const float ret =
      (float)TCOD_random_get_gaussian_double_range_custom_inv(rng, (double)min, (double)max, (double)mean);
  return TCOD_CLAMP(min, max, ret);
}

int TCOD_random_get_gaussian_int_range_custom_inv(TCOD_Random* rng, int min, int max, int mean) {
  const double num = TCOD_random_get_gaussian_double_range_custom_inv(rng, (double)min, (double)max, (double)mean);
  const int ret = (num >= 0.0 ? (int)(num + 0.5) : (int)(num - 0.5));
  return TCOD_CLAMP(min, max, ret);
}

void TCOD_random_set_distribution(TCOD_Random* rng, TCOD_distribution_t distribution) {
  if (!rng) rng = TCOD_random_get_instance();
  switch (rng->algorithm) {
    case TCOD_RNG_MT:
    case TCOD_RNG_CMWC:
      rng->mt_cmwc.distribution = distribution;
      break;
    default:
      break;
  }
}

static TCOD_distribution_t get_distribution(const TCOD_Random* rng) {
  if (!rng) rng = TCOD_random_get_instance();
  switch (rng->algorithm) {
    case TCOD_RNG_MT:
    case TCOD_RNG_CMWC:
      return rng->mt_cmwc.distribution;
    default:
      return TCOD_DISTRIBUTION_LINEAR;
  }
}

int TCOD_random_get_int(TCOD_Random* rng, int min, int max) {
  if (!rng) rng = TCOD_random_get_instance();
  switch (get_distribution(rng)) {
    case TCOD_DISTRIBUTION_LINEAR:
      return TCOD_random_get_i(rng, min, max);
    case TCOD_DISTRIBUTION_GAUSSIAN:
      return TCOD_random_get_gaussian_int(rng, min, max);
    case TCOD_DISTRIBUTION_GAUSSIAN_INVERSE:
      return TCOD_random_get_gaussian_int_inv(rng, min, max);
    case TCOD_DISTRIBUTION_GAUSSIAN_RANGE:
      return TCOD_random_get_gaussian_int_range(rng, min, max);
    case TCOD_DISTRIBUTION_GAUSSIAN_RANGE_INVERSE:
      return TCOD_random_get_gaussian_int_range_inv(rng, min, max);
    default:
      return TCOD_random_get_i(rng, min, max);
  }
}

float TCOD_random_get_float(TCOD_Random* rng, float min, float max) {
  if (!rng) rng = TCOD_random_get_instance();
  switch (get_distribution(rng)) {
    case TCOD_DISTRIBUTION_LINEAR:
      return TCOD_random_get_f(rng, min, max);
    case TCOD_DISTRIBUTION_GAUSSIAN:
      return TCOD_random_get_gaussian_float(rng, min, max);
    case TCOD_DISTRIBUTION_GAUSSIAN_INVERSE:
      return TCOD_random_get_gaussian_float_inv(rng, min, max);
    case TCOD_DISTRIBUTION_GAUSSIAN_RANGE:
      return TCOD_random_get_gaussian_float_range(rng, min, max);
    case TCOD_DISTRIBUTION_GAUSSIAN_RANGE_INVERSE:
      return TCOD_random_get_gaussian_float_range_inv(rng, min, max);
    default:
      return TCOD_random_get_f(rng, min, max);
  }
}

double TCOD_random_get_double(TCOD_Random* rng, double min, double max) {
  if (!rng) rng = TCOD_random_get_instance();
  switch (get_distribution(rng)) {
    case TCOD_DISTRIBUTION_LINEAR:
      return TCOD_random_get_d(rng, min, max);
    case TCOD_DISTRIBUTION_GAUSSIAN:
      return TCOD_random_get_gaussian_double(rng, min, max);
    case TCOD_DISTRIBUTION_GAUSSIAN_INVERSE:
      return TCOD_random_get_gaussian_double_inv(rng, min, max);
    case TCOD_DISTRIBUTION_GAUSSIAN_RANGE:
      return TCOD_random_get_gaussian_double_range(rng, min, max);
    case TCOD_DISTRIBUTION_GAUSSIAN_RANGE_INVERSE:
      return TCOD_random_get_gaussian_double_range_inv(rng, min, max);
    default:
      return TCOD_random_get_d(rng, min, max);
  }
}

int TCOD_random_get_int_mean(TCOD_Random* rng, int min, int max, int mean) {
  if (!rng) rng = TCOD_random_get_instance();
  switch (get_distribution(rng)) {
    case TCOD_DISTRIBUTION_GAUSSIAN_INVERSE:
    case TCOD_DISTRIBUTION_GAUSSIAN_RANGE_INVERSE:
      return TCOD_random_get_gaussian_int_range_custom_inv(rng, min, max, mean);
    default:
      return TCOD_random_get_gaussian_int_range_custom(rng, min, max, mean);
  }
}

float TCOD_random_get_float_mean(TCOD_Random* rng, float min, float max, float mean) {
  if (!rng) rng = TCOD_random_get_instance();
  switch (get_distribution(rng)) {
    case TCOD_DISTRIBUTION_GAUSSIAN_INVERSE:
    case TCOD_DISTRIBUTION_GAUSSIAN_RANGE_INVERSE:
      return TCOD_random_get_gaussian_float_range_custom_inv(rng, min, max, mean);
    default:
      return TCOD_random_get_gaussian_float_range_custom(rng, min, max, mean);
  }
}

double TCOD_random_get_double_mean(TCOD_Random* rng, double min, double max, double mean) {
  if (!rng) rng = TCOD_random_get_instance();
  switch (get_distribution(rng)) {
    case TCOD_DISTRIBUTION_GAUSSIAN_INVERSE:
    case TCOD_DISTRIBUTION_GAUSSIAN_RANGE_INVERSE:
      return TCOD_random_get_gaussian_double_range_custom_inv(rng, min, max, mean);
    default:
      return TCOD_random_get_gaussian_double_range_custom(rng, min, max, mean);
  }
}

TCOD_dice_t TCOD_random_dice_new(const char* dice_str) {
  TCOD_dice_t dice = {1, 1, 1.0f, 0.0f};
  char tmp[128] = "";
  size_t length;
  /* get multiplier */
  if ((length = strcspn(dice_str, "*x")) < strlen(dice_str)) {
    strncpy(tmp, dice_str, sizeof(tmp) - 1);
    tmp[length] = '\0';
    dice.multiplier = (float)atof(tmp);
    dice_str += length + 1;
  }
  /* get rolls */
  length = strcspn(dice_str, "dD");
  strncpy(tmp, dice_str, sizeof(tmp) - 1);
  tmp[length] = '\0';
  dice.nb_rolls = atoi(tmp);
  dice_str += length + 1;
  /* get faces */
  length = strcspn(dice_str, "-+");
  strncpy(tmp, dice_str, sizeof(tmp) - 1);
  tmp[length] = '\0';
  dice.nb_faces = atoi(tmp);
  dice_str += length;
  /* get addsub */
  if (strlen(dice_str) > 0) {
    const int sign = (*dice_str == '+') ? 1 : (-1);
    ++dice_str;
    dice.addsub = (float)(atof(dice_str) * sign);
  }
  return dice;
}

int TCOD_random_dice_roll(TCOD_Random* rng, TCOD_dice_t dice) {
  int result = 0;
  for (int rolls = 0; rolls < dice.nb_rolls; ++rolls) result += TCOD_random_get_i(rng, 1, dice.nb_faces);
  return (int)((result + dice.addsub) * dice.multiplier);
}

int TCOD_random_dice_roll_s(TCOD_Random* rng, const char* dice_str) {
  return TCOD_random_dice_roll(rng, TCOD_random_dice_new(dice_str));
}
