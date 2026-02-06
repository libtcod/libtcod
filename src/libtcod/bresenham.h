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
/// @file bresenham.h
/// Bresenham line module.
#pragma once
#ifndef TCOD_BRESENHAM_H_
#define TCOD_BRESENHAM_H_

#include "portability.h"

/// @defgroup Bresenham Bresenham lines
/// @{
#ifdef __cplusplus
extern "C" {
#endif
/**
 *  \brief A callback to be passed to TCOD_line
 *
 *  The points given to the callback include both the starting and ending
 *  positions.
 *
 *  \param x
 *  \param y
 *  \return As long as this callback returns true it will be called with the
 *          next x,y point on the line.
 */
typedef bool (*TCOD_line_listener_t)(int x, int y);
/**
 *  \brief Initialize a line using a global state.
 *
 *  \param xFrom The starting x position.
 *  \param yFrom The starting y position.
 *  \param xTo The ending x position.
 *  \param yTo The ending y position.
 *
 *  \verbatim embed:rst:leading-asterisk
 *  .. deprecated:: 1.6.6
 *    This function is not reentrant and will fail if a new line is started
 *    before the last is finished processing.
 *
 *    Use TCOD_line_init_mt instead.
 *  \endverbatim
 */
TCODLIB_API TCOD_DEPRECATED("This function is not reentrant. Use TCOD_line_init_mt instead.") void TCOD_line_init(
    int xFrom, int yFrom, int xTo, int yTo);
/**
 *  \brief Advance to the next point in a line, returns true once the line has ended.
 *
 *  \param xCur An int pointer to fill with the next x position.
 *  \param yCur An int pointer to fill with the next y position.
 *  \return true once the ending point has been reached.
 *
 *  The starting point is excluded by this function.
 *  After the ending point is reached, the next call will return true.
 *
 *  \verbatim embed:rst:leading-asterisk
 *  .. deprecated:: 1.6.6
 *    This function is not reentrant and will fail if a new line is started
 *    before the last is finished processing.
 *
 *    Use TCOD_line_step_mt instead.
 *  \endverbatim
 */
TCODLIB_API TCOD_DEPRECATED("This function is not reentrant.") bool TCOD_line_step(
    int* __restrict xCur, int* __restrict yCur);
/**
 *  \brief Iterate over a line using a callback.
 *
 *  \param xo The origin x position.
 *  \param yo The origin y position.
 *  \param xd The destination x position.
 *  \param yd The destination y position.
 *  \param listener A TCOD_line_listener_t callback. Iteration stops early if this callback returns false.
 *  \return true if the line was completely exhausted by the callback.
 *
 *  \verbatim embed:rst:leading-asterisk
 *  .. versionchanged:: 1.6.6
 *    This function is now reentrant.
 *  \endverbatim
 */
TCODLIB_API bool TCOD_line(int xFrom, int yFrom, int xTo, int yTo, TCOD_line_listener_t listener);
/**
 *  \brief A struct used for computing a bresenham line.
 */
typedef struct {
  int stepx;
  int stepy;
  int e;
  int deltax;
  int deltay;
  int origx;
  int origy;
  int destx;
  int desty;
} TCOD_bresenham_data_t;
/**
 *  \brief Initialize a TCOD_bresenham_data_t struct.
 *
 *  \param xFrom The starting x position.
 *  \param yFrom The starting y position.
 *  \param xTo The ending x position.
 *  \param yTo The ending y position.
 *  \param data Pointer to a TCOD_bresenham_data_t struct.
 *
 *  After calling this function you use TCOD_line_step_mt to iterate
 *  over the individual points on the line.
 */
TCODLIB_API void TCOD_line_init_mt(int xFrom, int yFrom, int xTo, int yTo, TCOD_bresenham_data_t* data);
/**
 *  \brief Get the next point on a line, returns true once the line has ended.
 *
 *  \param xCur An int pointer to fill with the next x position.
 *  \param yCur An int pointer to fill with the next y position.
 *  \param data Pointer to a initialized TCOD_bresenham_data_t struct.
 *  \return true after the ending point has been reached.
 *
 *  The starting point is excluded by this function.
 *  After the ending point is reached, the next call will return true.
 */
TCODLIB_API bool TCOD_line_step_mt(int* __restrict xCur, int* __restrict yCur, TCOD_bresenham_data_t* __restrict data);
/**
 *  \brief Iterate over a line using a callback.
 *
 *  \param xo The origin x position.
 *  \param yo The origin y position.
 *  \param xd The destination x position.
 *  \param yd The destination y position.
 *  \param listener A TCOD_line_listener_t callback.
 *  \param data Pointer to a TCOD_bresenham_data_t struct.
 *  \return true if the line was completely exhausted by the callback.
 *
 *  \verbatim embed:rst:leading-asterisk
 *  .. deprecated:: 1.6.6
 *    The `data` parameter for this call is redundant, you should call
 *    TCOD_line instead.
 *  \endverbatim
 */
TCOD_DEPRECATED("Use TCOD_line instead.")
TCODLIB_API bool TCOD_line_mt(
    int xFrom, int yFrom, int xTo, int yTo, TCOD_line_listener_t listener, TCOD_bresenham_data_t* data);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
/// @}
#endif  // TCOD_BRESENHAM_H_
