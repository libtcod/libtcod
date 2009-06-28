/*
* libtcod 1.4.1
* Copyright (c) 2008,2009 Jice
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Jice ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Jice BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _TCOD_COLOR_HPP
#define _TCOD_COLOR_HPP

class TCODLIB_API TCODColor {
public :
	uint8 r,g,b;

	TCODColor() : r(0),g(0),b(0) {}
	TCODColor(uint8 r, uint8 g, uint8 b) :r(r),g(g),b(b) {}
	TCODColor(int r, int g, int b) :r(r),g(g),b(b) {}
	TCODColor(const TCOD_color_t &col) :r(col.r),g(col.g),b(col.b) {}
	bool operator == (const TCODColor & c) const {
		return (c.r == r && c.g == g && c.b == b);
	}
	bool operator != (const TCODColor & c) const {
		return (c.r != r || c.g != g || c.b != b);
	}
	TCODColor operator * (const TCODColor & a) const {
		TCODColor ret;
		ret.r=(uint8)(((int)r)*a.r/255);
		ret.g=(uint8)(((int)g)*a.g/255);
		ret.b=(uint8)(((int)b)*a.b/255);
		return ret;
	}
	TCODColor operator *(float value) const {
		TCOD_color_t ret;
		int r,g,b;
		r = (int)(this->r * value);
		g = (int)(this->g * value);
		b = (int)(this->b * value);
		r = CLAMP(0,255,r);
		g = CLAMP(0,255,g);
		b = CLAMP(0,255,b);
		ret.r=(uint8)r;
		ret.g=(uint8)g;
		ret.b=(uint8)b;
		return ret;
	}
	TCODColor operator + (const TCODColor & a) const {
		TCODColor ret;
		int r=(int)(this->r)+a.r;
		int g=(int)(this->g)+a.g;
		int b=(int)(this->b)+a.b;
		r = MIN(255,r);
		g = MIN(255,g);
		b = MIN(255,b);
		ret.r=(uint8)r;
		ret.g=(uint8)g;
		ret.b=(uint8)b;
		return ret;
	}
	TCODColor operator - (const TCODColor & a) const {
		TCODColor ret;
		int r=(int)(this->r)-a.r;
		int g=(int)(this->g)-a.g;
		int b=(int)(this->b)-a.b;
		r = MAX(0,r);
		g = MAX(0,g);
		b = MAX(0,b);
		ret.r=(uint8)r;
		ret.g=(uint8)g;
		ret.b=(uint8)b;
		return ret;
	}
	void setHSV(float h, float s, float v);
	void getHSV(float *h, float *s, float *v) const;
	
	static TCODColor lerp(const TCODColor &a, const TCODColor &b, float coef) {
		TCODColor ret;
		ret.r=(uint8)(a.r+(b.r-a.r)*coef);
		ret.g=(uint8)(a.g+(b.g-a.g)*coef);
		ret.b=(uint8)(a.b+(b.b-a.b)*coef);
		return ret;
	}
	static void genMap(TCODColor *map, int nbKey, TCODColor const *keyColor, int const *keyIndex);

	// color array
	static const TCODColor colors[TCOD_COLOR_NB][TCOD_COLOR_LEVELS];

	// grey levels
	static const TCODColor black;
	static const TCODColor darkerGrey;
	static const TCODColor darkGrey;
	static const TCODColor grey;
	static const TCODColor lightGrey;
	static const TCODColor white;
	
	// standard colors
	static const TCODColor red;
	static const TCODColor orange;
	static const TCODColor yellow;
	static const TCODColor chartreuse;
	static const TCODColor green;
	static const TCODColor sea;
	static const TCODColor cyan;
	static const TCODColor sky;
	static const TCODColor blue;
	static const TCODColor violet;
	static const TCODColor magenta;
	static const TCODColor pink;
	
	// dark colors
	static const TCODColor darkRed;
	static const TCODColor darkOrange;
	static const TCODColor darkYellow;
	static const TCODColor darkChartreuse;
	static const TCODColor darkGreen;
	static const TCODColor darkSea;
	static const TCODColor darkCyan;
	static const TCODColor darkSky;
	static const TCODColor darkBlue;
	static const TCODColor darkViolet;
	static const TCODColor darkMagenta;
	static const TCODColor darkPink;
	
	// darker colors
	static const TCODColor darkerRed;
	static const TCODColor darkerOrange;
	static const TCODColor darkerYellow;
	static const TCODColor darkerChartreuse;
	static const TCODColor darkerGreen;
	static const TCODColor darkerSea;
	static const TCODColor darkerCyan;
	static const TCODColor darkerSky;
	static const TCODColor darkerBlue;
	static const TCODColor darkerViolet;
	static const TCODColor darkerMagenta;
	static const TCODColor darkerPink;
	
	// light colors
	static const TCODColor lightRed;
	static const TCODColor lightOrange;
	static const TCODColor lightYellow;
	static const TCODColor lightChartreuse;
	static const TCODColor lightGreen;
	static const TCODColor lightSea;
	static const TCODColor lightCyan;
	static const TCODColor lightSky;
	static const TCODColor lightBlue;
	static const TCODColor lightViolet;
	static const TCODColor lightMagenta;
	static const TCODColor lightPink;
	
	// desaturated colors
	static const TCODColor desaturatedRed;
	static const TCODColor desaturatedOrange;
	static const TCODColor desaturatedYellow;
	static const TCODColor desaturatedChartreuse;
	static const TCODColor desaturatedGreen;
	static const TCODColor desaturatedSea;
	static const TCODColor desaturatedCyan;
	static const TCODColor desaturatedSky;
	static const TCODColor desaturatedBlue;
	static const TCODColor desaturatedViolet;
	static const TCODColor desaturatedMagenta;
	static const TCODColor desaturatedPink;	
	
	// special
	static const TCODColor silver;
	static const TCODColor gold;
};

TCODLIB_API TCODColor operator *(float value, const TCODColor &c);

#endif
