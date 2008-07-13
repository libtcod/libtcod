/*
* libtcod 1.3.2
* Copyright (c) 2007,2008 J.C.Wilk
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of J.C.Wilk may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY J.C.WILK ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL J.C.WILK BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "libtcod.h"

static int stepx;
static int stepy;
static int e;
static int deltax;
static int deltay;
static int destx; 
static int desty; 

/* ********** bresenham line drawing ********** */
void TCOD_line_init(int xFrom, int yFrom, int xTo, int yTo) {
	int x=xFrom,y=yFrom;
	destx=xTo;
	desty=yTo;
	deltax=destx-x;
	deltay=desty-y;
	if ( deltax > 0 ) {
		stepx=1;
	} else if ( deltax < 0 ){
		stepx=-1;
	} else stepx=0;
	if ( deltay > 0 ) {
		stepy=1;
	} else if ( deltay < 0 ){
		stepy=-1;
	} else stepy = 0;
	if ( stepx*deltax > stepy*deltay ) {
		e = stepx*deltax;
		deltax *= 2;
		deltay *= 2;
	} else {
		e = stepy*deltay;
		deltax *= 2;
		deltay *= 2;
	}
}

bool TCOD_line_step(int *xCur, int *yCur) {
	if ( stepx*deltax > stepy*deltay ) {
		if ( (*xCur) == destx ) return true;
		(*xCur)+=stepx;
		e -= stepy*deltay;
		if ( e < 0) {
			(*yCur)+=stepy;
			e+=stepx*deltax;
		}
	} else {
		if ( (*yCur) == desty ) return true;
		(*yCur)+=stepy;
		e -= stepx*deltax;
		if ( e < 0) {
			(*xCur)+=stepx;
			e+=stepy*deltay;
		}
	}	
	return false;
}
