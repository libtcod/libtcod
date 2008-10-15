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
		r=MIN(255,r);
		g=MIN(255,g);
		b=MIN(255,b);
		r=MAX(0,r);
		g=MAX(0,g);
		b=MAX(0,b);
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
		r=MAX(0,r);
		g=MAX(0,g);
		b=MAX(0,b);
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

	static TCODColor black;
	static TCODColor darkGrey;
	static TCODColor grey;
	static TCODColor darkGray;
	static TCODColor gray;
	static TCODColor white;
	static TCODColor darkBlue;
	static TCODColor lightBlue;
	static TCODColor darkRed;
	static TCODColor lightRed;
	static TCODColor darkBrown;
	static TCODColor lightYellow;
	static TCODColor yellow;
	static TCODColor darkYellow;
	static TCODColor green;
	static TCODColor orange;
	static TCODColor red;
	static TCODColor silver;
	static TCODColor gold;
	static TCODColor purple;
	static TCODColor darkPurple;
};

TCODLIB_API TCODColor operator *(float value, const TCODColor &c);

#endif
