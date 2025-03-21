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
#ifndef TCOD_RANDOM_TYPES_H_
#define TCOD_RANDOM_TYPES_H_
#include <stdint.h>

#include "config.h"

/* dice roll */
typedef struct {
  int nb_rolls;
  int nb_faces;
  float multiplier;
  float addsub;
} TCOD_dice_t;

/* PRNG algorithms */
typedef enum {
  /***************************************************************************
      @brief Mersenne Twister implementation.
   */
  TCOD_RNG_MT,
  /***************************************************************************
      @brief Complementary-Multiply-With-Carry implementation.
   */
  TCOD_RNG_CMWC
} TCOD_random_algo_t;

typedef enum {
  TCOD_DISTRIBUTION_LINEAR,
  TCOD_DISTRIBUTION_GAUSSIAN,
  TCOD_DISTRIBUTION_GAUSSIAN_RANGE,
  TCOD_DISTRIBUTION_GAUSSIAN_INVERSE,
  TCOD_DISTRIBUTION_GAUSSIAN_RANGE_INVERSE
} TCOD_distribution_t;

// Old RNG, this struct and its attribute are private.
struct TCOD_Random_MT_CMWC {
  TCOD_random_algo_t algorithm;  // algorithm identifier
  TCOD_distribution_t distribution;  // distribution
  // Mersenne Twister stuff
  uint32_t mt[624];
  int cur_mt;
  // Complementary-Multiply-With-Carry stuff
  // shared with Generalised Feedback Shift Register
  uint32_t Q[4096], c;
  int cur;
};
/***************************************************************************
    @brief Pseudorandom number generator toolkit, all attributes are private.
 */
typedef union TCOD_Random {
  TCOD_random_algo_t algorithm;
  struct TCOD_Random_MT_CMWC mt_cmwc;
} TCOD_Random;

TCOD_DEPRECATED("This type hides indirection.  Use TCOD_Random* instead.")
typedef union TCOD_Random* TCOD_random_t;

#endif  // TCOD_RANDOM_TYPES_H_
