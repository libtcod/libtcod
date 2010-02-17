/*
* libtcod 1.5.1
* Copyright (c) 2008,2009,2010 Jice & Mingos
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice or Mingos may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY JICE AND MINGOS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE OR MINGOS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "libtcod.hpp"
#include "libtcod_int.h"

// grey levels
const TCODColor TCODColor::black(TCOD_BLACK);
const TCODColor TCODColor::darkerGrey(TCOD_DARKER_GREY);
const TCODColor TCODColor::darkGrey(TCOD_DARK_GREY);
const TCODColor TCODColor::grey(TCOD_GREY);
const TCODColor TCODColor::lightGrey(TCOD_LIGHT_GREY);
const TCODColor TCODColor::white(TCOD_WHITE);

// standard colors
const TCODColor TCODColor::red(TCOD_RED);
const TCODColor TCODColor::orange(TCOD_ORANGE);
const TCODColor TCODColor::yellow(TCOD_YELLOW);
const TCODColor TCODColor::chartreuse(TCOD_CHARTREUSE);
const TCODColor TCODColor::green(TCOD_GREEN);
const TCODColor TCODColor::sea(TCOD_SEA);
const TCODColor TCODColor::cyan(TCOD_CYAN);
const TCODColor TCODColor::sky(TCOD_SKY);
const TCODColor TCODColor::blue(TCOD_BLUE);
const TCODColor TCODColor::violet(TCOD_VIOLET);
const TCODColor TCODColor::magenta(TCOD_MAGENTA);
const TCODColor TCODColor::pink(TCOD_PINK);

// dark colors
const TCODColor TCODColor::darkRed(TCOD_DARK_RED);
const TCODColor TCODColor::darkOrange(TCOD_DARK_ORANGE);
const TCODColor TCODColor::darkYellow(TCOD_DARK_YELLOW);
const TCODColor TCODColor::darkChartreuse(TCOD_DARK_CHARTREUSE);
const TCODColor TCODColor::darkGreen(TCOD_DARK_GREEN);
const TCODColor TCODColor::darkSea(TCOD_DARK_SEA);
const TCODColor TCODColor::darkCyan(TCOD_DARK_CYAN);
const TCODColor TCODColor::darkSky(TCOD_DARK_SKY);
const TCODColor TCODColor::darkBlue(TCOD_DARK_BLUE);
const TCODColor TCODColor::darkViolet(TCOD_DARK_VIOLET);
const TCODColor TCODColor::darkMagenta(TCOD_DARK_MAGENTA);
const TCODColor TCODColor::darkPink(TCOD_DARK_PINK);

// darker colors
const TCODColor TCODColor::darkerRed(TCOD_DARKER_RED);
const TCODColor TCODColor::darkerOrange(TCOD_DARKER_ORANGE);
const TCODColor TCODColor::darkerYellow(TCOD_DARKER_YELLOW);
const TCODColor TCODColor::darkerChartreuse(TCOD_DARKER_CHARTREUSE);
const TCODColor TCODColor::darkerGreen(TCOD_DARKER_GREEN);
const TCODColor TCODColor::darkerSea(TCOD_DARKER_SEA);
const TCODColor TCODColor::darkerCyan(TCOD_DARKER_CYAN);
const TCODColor TCODColor::darkerSky(TCOD_DARKER_SKY);
const TCODColor TCODColor::darkerBlue(TCOD_DARKER_BLUE);
const TCODColor TCODColor::darkerViolet(TCOD_DARKER_VIOLET);
const TCODColor TCODColor::darkerMagenta(TCOD_DARKER_MAGENTA);
const TCODColor TCODColor::darkerPink(TCOD_DARKER_PINK);

// light colors
const TCODColor TCODColor::lightRed(TCOD_LIGHT_RED);
const TCODColor TCODColor::lightOrange(TCOD_LIGHT_ORANGE);
const TCODColor TCODColor::lightYellow(TCOD_LIGHT_YELLOW);
const TCODColor TCODColor::lightChartreuse(TCOD_LIGHT_CHARTREUSE);
const TCODColor TCODColor::lightGreen(TCOD_LIGHT_GREEN);
const TCODColor TCODColor::lightSea(TCOD_LIGHT_SEA);
const TCODColor TCODColor::lightCyan(TCOD_LIGHT_CYAN);
const TCODColor TCODColor::lightSky(TCOD_LIGHT_SKY);
const TCODColor TCODColor::lightBlue(TCOD_LIGHT_BLUE);
const TCODColor TCODColor::lightViolet(TCOD_LIGHT_VIOLET);
const TCODColor TCODColor::lightMagenta(TCOD_LIGHT_MAGENTA);
const TCODColor TCODColor::lightPink(TCOD_LIGHT_PINK);

// desaturated colors
const TCODColor TCODColor::desaturatedRed(TCOD_DESATURATED_RED);
const TCODColor TCODColor::desaturatedOrange(TCOD_DESATURATED_ORANGE);
const TCODColor TCODColor::desaturatedYellow(TCOD_DESATURATED_YELLOW);
const TCODColor TCODColor::desaturatedChartreuse(TCOD_DESATURATED_CHARTREUSE);
const TCODColor TCODColor::desaturatedGreen(TCOD_DESATURATED_GREEN);
const TCODColor TCODColor::desaturatedSea(TCOD_DESATURATED_SEA);
const TCODColor TCODColor::desaturatedCyan(TCOD_DESATURATED_CYAN);
const TCODColor TCODColor::desaturatedSky(TCOD_DESATURATED_SKY);
const TCODColor TCODColor::desaturatedBlue(TCOD_DESATURATED_BLUE);
const TCODColor TCODColor::desaturatedViolet(TCOD_DESATURATED_VIOLET);
const TCODColor TCODColor::desaturatedMagenta(TCOD_DESATURATED_MAGENTA);
const TCODColor TCODColor::desaturatedPink(TCOD_DESATURATED_PINK);

//special
const TCODColor TCODColor::silver(TCOD_SILVER);
const TCODColor TCODColor::gold(TCOD_GOLD);

// color array
const TCODColor TCODColor::colors[TCOD_COLOR_NB][TCOD_COLOR_LEVELS] = {
 {TCODColor(TCOD_DESATURATED_RED),TCODColor(TCOD_LIGHT_RED),TCODColor(TCOD_RED),TCODColor(TCOD_DARK_RED),TCODColor(TCOD_DARKER_RED)},
 {TCODColor(TCOD_DESATURATED_ORANGE),TCODColor(TCOD_LIGHT_ORANGE),TCODColor(TCOD_ORANGE),TCODColor(TCOD_DARK_ORANGE),TCODColor(TCOD_DARKER_ORANGE)},
 {TCODColor(TCOD_DESATURATED_YELLOW),TCODColor(TCOD_LIGHT_YELLOW),TCODColor(TCOD_YELLOW),TCODColor(TCOD_DARK_YELLOW),TCODColor(TCOD_DARKER_YELLOW)},
 {TCODColor(TCOD_DESATURATED_CHARTREUSE),TCODColor(TCOD_LIGHT_CHARTREUSE),TCODColor(TCOD_CHARTREUSE),TCODColor(TCOD_DARK_CHARTREUSE),TCODColor(TCOD_DARKER_CHARTREUSE)},
 {TCODColor(TCOD_DESATURATED_GREEN),TCODColor(TCOD_LIGHT_GREEN),TCODColor(TCOD_GREEN),TCODColor(TCOD_DARK_GREEN),TCODColor(TCOD_DARKER_GREEN)},
 {TCODColor(TCOD_DESATURATED_SEA),TCODColor(TCOD_LIGHT_SEA),TCODColor(TCOD_SEA),TCODColor(TCOD_DARK_SEA),TCODColor(TCOD_DARKER_SEA)},
 {TCODColor(TCOD_DESATURATED_CYAN),TCODColor(TCOD_LIGHT_CYAN),TCODColor(TCOD_CYAN),TCODColor(TCOD_DARK_CYAN),TCODColor(TCOD_DARKER_CYAN)},
 {TCODColor(TCOD_DESATURATED_SKY),TCODColor(TCOD_LIGHT_SKY),TCODColor(TCOD_SKY),TCODColor(TCOD_DARK_SKY),TCODColor(TCOD_DARKER_SKY)},
 {TCODColor(TCOD_DESATURATED_BLUE),TCODColor(TCOD_LIGHT_BLUE),TCODColor(TCOD_BLUE),TCODColor(TCOD_DARK_BLUE),TCODColor(TCOD_DARKER_BLUE)},
 {TCODColor(TCOD_DESATURATED_VIOLET),TCODColor(TCOD_LIGHT_VIOLET),TCODColor(TCOD_VIOLET),TCODColor(TCOD_DARK_VIOLET),TCODColor(TCOD_DARKER_VIOLET)},
 {TCODColor(TCOD_DESATURATED_MAGENTA),TCODColor(TCOD_LIGHT_MAGENTA),TCODColor(TCOD_MAGENTA),TCODColor(TCOD_DARK_MAGENTA),TCODColor(TCOD_DARKER_MAGENTA)},
 {TCODColor(TCOD_DESATURATED_PINK),TCODColor(TCOD_LIGHT_PINK),TCODColor(TCOD_PINK),TCODColor(TCOD_DARK_PINK),TCODColor(TCOD_DARKER_PINK)},
};

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

