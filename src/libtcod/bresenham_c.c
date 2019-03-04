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
#include "bresenham.h"
/**
 *  \file bresenham_c.c
 *  \brief bresenham line drawing
 */
/* This static variable is deprecated since 1.6 */
static TCOD_bresenham_data_t bresenham_data;
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
void TCOD_line_init_mt(int xFrom, int yFrom, int xTo, int yTo, TCOD_bresenham_data_t *data) {
	data->origx=xFrom;
	data->origy=yFrom;
	data->destx=xTo;
	data->desty=yTo;
	data->deltax=xTo - xFrom;
	data->deltay=yTo - yFrom;
	if ( data->deltax > 0 ) {
		data->stepx=1;
	} else if ( data->deltax < 0 ){
		data->stepx=-1;
	} else data->stepx=0;
	if ( data->deltay > 0 ) {
		data->stepy=1;
	} else if ( data->deltay < 0 ){
		data->stepy=-1;
	} else data->stepy = 0;
	if ( data->stepx*data->deltax > data->stepy*data->deltay ) {
		data->e = data->stepx*data->deltax;
		data->deltax *= 2;
		data->deltay *= 2;
	} else {
		data->e = data->stepy*data->deltay;
		data->deltax *= 2;
		data->deltay *= 2;
	}
}
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
bool TCOD_line_step_mt(int *xCur, int *yCur, TCOD_bresenham_data_t *data) {
	if ( data->stepx*data->deltax > data->stepy*data->deltay ) {
		if ( data->origx == data->destx ) return true;
		data->origx+=data->stepx;
		data->e -= data->stepy*data->deltay;
		if ( data->e < 0) {
			data->origy+=data->stepy;
			data->e+=data->stepx*data->deltax;
		}
	} else {
		if ( data->origy == data->desty ) return true;
		data->origy+=data->stepy;
		data->e -= data->stepx*data->deltax;
		if ( data->e < 0) {
			data->origx+=data->stepx;
			data->e+=data->stepy*data->deltay;
		}
	}
	*xCur=data->origx;
	*yCur=data->origy;
	return false;
}
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
 *    :any:`TCOD_line` instead.
 *  \endverbatim
 */
bool TCOD_line_mt(int xo, int yo, int xd, int yd, TCOD_line_listener_t listener, TCOD_bresenham_data_t *data) {
	TCOD_line_init_mt(xo,yo,xd,yd,data);
	do {
		if (! listener(xo,yo)) return false;
	} while (! TCOD_line_step_mt(&xo,&yo,data));
	return true;
}
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
 *    Use :any:`TCOD_line_init_mt` instead.
 *  \endverbatim
 */
void TCOD_line_init(int xFrom, int yFrom, int xTo, int yTo) {
	TCOD_line_init_mt(xFrom,yFrom,xTo,yTo,&bresenham_data);
}
/**
 *  \brief Get the next point in a line, returns true once the line has ended.
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
 *    Use :any:`TCOD_line_step_mt` instead.
 *  \endverbatim
 */
bool TCOD_line_step(int *xCur, int *yCur) {
	return TCOD_line_step_mt(xCur,yCur,&bresenham_data);
}
/**
 *  \brief Iterate over a line using a callback.
 *
 *  \param xo The origin x position.
 *  \param yo The origin y position.
 *  \param xd The destination x position.
 *  \param yd The destination y position.
 *  \param listener A TCOD_line_listener_t callback.
 *  \return true if the line was completely exhausted by the callback.
 *
 *  \verbatim embed:rst:leading-asterisk
 *  .. versionchanged:: 1.6.6
 *    This function is now reentrant.
 *  \endverbatim
 */
bool TCOD_line(int xo, int yo, int xd, int yd, TCOD_line_listener_t listener) {
	TCOD_bresenham_data_t bresenham_data;
	return TCOD_line_mt(xo,yo,xd,yd,listener,&bresenham_data);
}
