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
#ifndef _TCOD_RANDOM_TYPES_H
#define _TCOD_RANDOM_TYPES_H
struct TCOD_Random;
typedef struct TCOD_Random *TCOD_random_t;

/* dice roll */
typedef struct {
	int nb_rolls;
	int nb_faces;
	float multiplier;
	float addsub;
} TCOD_dice_t;

/* PRNG algorithms */
typedef enum {
    TCOD_RNG_MT,
    TCOD_RNG_CMWC
} TCOD_random_algo_t;

typedef enum {
	TCOD_DISTRIBUTION_LINEAR,
	TCOD_DISTRIBUTION_GAUSSIAN,
	TCOD_DISTRIBUTION_GAUSSIAN_RANGE,
	TCOD_DISTRIBUTION_GAUSSIAN_INVERSE,
	TCOD_DISTRIBUTION_GAUSSIAN_RANGE_INVERSE
} TCOD_distribution_t;
#endif /* _TCOD_RANDOM_TYPES_H */
