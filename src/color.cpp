/*
* libtcod 1.4.0
* Copyright (c) 2008 J.C.Wilk
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
#include "libtcod.hpp"

TCODColor TCODColor::black(0,0,0);
TCODColor TCODColor::darkGrey(96,96,96);
TCODColor TCODColor::grey(196,196,196);
TCODColor TCODColor::darkGray(96,96,96);
TCODColor TCODColor::gray(196,196,196);
TCODColor TCODColor::white(255,255,255);
TCODColor TCODColor::darkBlue(40,40,128);
TCODColor TCODColor::lightBlue(120,120,255);
TCODColor TCODColor::darkRed(128,0,0);
TCODColor TCODColor::lightRed(255,100,50);
TCODColor TCODColor::darkBrown(32,16,0);
TCODColor TCODColor::lightYellow(255,255,150);
TCODColor TCODColor::yellow(255,255,0);
TCODColor TCODColor::darkYellow(164,164,0);
TCODColor TCODColor::green(0,220,0);
TCODColor TCODColor::orange(255,150,0);
TCODColor TCODColor::red(255,0,0);
TCODColor TCODColor::silver(203,203,203);
TCODColor TCODColor::gold(255,255,102);
TCODColor TCODColor::purple(204,51,153);
TCODColor TCODColor::darkPurple(51,0,51);

void TCODColor::setHSV(float h, float s, float v) {
  TCOD_color_t c;
  TCOD_color_set_HSV(&c,h,s,v);
  r=c.r;
  g=c.g;
  b=c.b;
}

void TCODColor::getHSV(float *h, float *s, float *v) const {
  TCOD_color_t c={r,g,b};
  TCOD_color_get_HSV(c,h,s,v);
}

// non member operators
TCODColor operator *(float value, const TCODColor &c) {
	return c*value;
}
void TCODColor::genMap(TCODColor *map, int nbKey, TCODColor const *keyColor, int const *keyIndex) {
	for (int segment=0; segment < nbKey-1; segment++) {
		int idxStart=keyIndex[segment];
		int idxEnd=keyIndex[segment+1];
		int idx;
		for ( idx=idxStart;idx <= idxEnd; idx++) {
			map[idx]=TCODColor::lerp(keyColor[segment],keyColor[segment+1],(float)(idx-idxStart)/(idxEnd-idxStart));
		}
	}
}

