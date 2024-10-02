/* BSD 3-Clause License
 *
 * Copyright © 2008-2023, Jice and the libtcod contributors.
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
#ifndef TCODFOV_BRESENHAM_H_
#define TCODFOV_BRESENHAM_H_

#include <stdbool.h>

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 *  \brief A callback to be passed to TCODFOV_line
 *
 *  The points given to the callback include both the starting and ending
 *  positions.
 *
 *  \param x
 *  \param y
 *  \return As long as this callback returns true it will be called with the
 *          next x,y point on the line.
 */
typedef bool (*TCODFOV_line_listener_t)(int x, int y);

TCODFOV_PUBLIC
TCODFOV_DEPRECATED("This function is not reentrant. Use TCODFOV_line_init_mt instead.")
void TCODFOV_line_init(int xFrom, int yFrom, int xTo, int yTo);

/** advance one step. returns true if we reach destination */
TCODFOV_PUBLIC TCODFOV_DEPRECATED("This function is not reentrant.") bool TCODFOV_line_step(
    int* __restrict xCur, int* __restrict yCur);

/* atomic callback function. Stops when the callback returns false */
TCODFOV_PUBLIC bool TCODFOV_line(int xFrom, int yFrom, int xTo, int yTo, TCODFOV_line_listener_t listener);
/**
 *  \brief A struct used for computing a bresenham line.
 */
typedef struct TCODFOV_bresenham_data_t {
  int stepx;
  int stepy;
  int e;
  int deltax;
  int deltay;
  int origx;
  int origy;
  int destx;
  int desty;
} TCODFOV_bresenham_data_t;

TCODFOV_PUBLIC void TCODFOV_line_init_mt(int xFrom, int yFrom, int xTo, int yTo, TCODFOV_bresenham_data_t* data);
TCODFOV_PUBLIC bool TCODFOV_line_step_mt(
    int* __restrict xCur, int* __restrict yCur, TCODFOV_bresenham_data_t* __restrict data);

TCODFOV_DEPRECATED("Use TCODFOV_line instead.")
TCODFOV_PUBLIC bool TCODFOV_line_mt(
    int xFrom, int yFrom, int xTo, int yTo, TCODFOV_line_listener_t listener, TCODFOV_bresenham_data_t* data);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
#endif  // TCODFOV_BRESENHAM_H_
