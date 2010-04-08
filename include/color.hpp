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

#ifndef _TCOD_COLOR_HPP
#define _TCOD_COLOR_HPP

/**
 @PageName color
 @PageTitle Colors
 */	

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
	void scaleHSV (float sscale, float vscale);
	
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
	static const TCODColor darkestGrey;
	static const TCODColor darkerGrey;
	static const TCODColor darkGrey;
	static const TCODColor grey;
	static const TCODColor lightGrey;
	static const TCODColor lighterGrey;
	static const TCODColor lightestGrey;
	static const TCODColor white;
	
	//sepia
	static const TCODColor darkestSepia;
	static const TCODColor darkerSepia;
	static const TCODColor darkSepia;
	static const TCODColor sepia;
	static const TCODColor lightSepia;
	static const TCODColor lighterSepia;
	static const TCODColor lightestSepia;
	
	// standard colors
	static const TCODColor red;
  static const TCODColor flame;
  static const TCODColor orange;
  static const TCODColor amber;
  static const TCODColor yellow;
  static const TCODColor lime;
  static const TCODColor chartreuse;
  static const TCODColor green;
  static const TCODColor sea;
  static const TCODColor turquoise;
  static const TCODColor cyan;
  static const TCODColor sky;
  static const TCODColor azure;
  static const TCODColor blue;
  static const TCODColor han;
  static const TCODColor violet;
  static const TCODColor purple;
  static const TCODColor fuchsia;
  static const TCODColor magenta;
  static const TCODColor pink;
  static const TCODColor crimson;
	
	// dark colors
	static const TCODColor darkRed;
  static const TCODColor darkFlame;
  static const TCODColor darkOrange;
  static const TCODColor darkAmber;
  static const TCODColor darkYellow;
  static const TCODColor darkLime;
  static const TCODColor darkChartreuse;
  static const TCODColor darkGreen;
  static const TCODColor darkSea;
  static const TCODColor darkTurquoise;
  static const TCODColor darkCyan;
  static const TCODColor darkSky;
  static const TCODColor darkAzure;
  static const TCODColor darkBlue;
  static const TCODColor darkHan;
  static const TCODColor darkViolet;
  static const TCODColor darkPurple;
  static const TCODColor darkFuchsia;
  static const TCODColor darkMagenta;
  static const TCODColor darkPink;
  static const TCODColor darkCrimson;
	
	// darker colors
	static const TCODColor darkerRed;
  static const TCODColor darkerFlame;
  static const TCODColor darkerOrange;
  static const TCODColor darkerAmber;
  static const TCODColor darkerYellow;
  static const TCODColor darkerLime;
  static const TCODColor darkerChartreuse;
  static const TCODColor darkerGreen;
  static const TCODColor darkerSea;
  static const TCODColor darkerTurquoise;
  static const TCODColor darkerCyan;
  static const TCODColor darkerSky;
  static const TCODColor darkerAzure;
  static const TCODColor darkerBlue;
  static const TCODColor darkerHan;
  static const TCODColor darkerViolet;
  static const TCODColor darkerPurple;
  static const TCODColor darkerFuchsia;
  static const TCODColor darkerMagenta;
  static const TCODColor darkerPink;
  static const TCODColor darkerCrimson;
  
  // darkest colors
  static const TCODColor darkestRed;
  static const TCODColor darkestFlame;
  static const TCODColor darkestOrange;
  static const TCODColor darkestAmber;
  static const TCODColor darkestYellow;
  static const TCODColor darkestLime;
  static const TCODColor darkestChartreuse;
  static const TCODColor darkestGreen;
  static const TCODColor darkestSea;
  static const TCODColor darkestTurquoise;
  static const TCODColor darkestCyan;
  static const TCODColor darkestSky;
  static const TCODColor darkestAzure;
  static const TCODColor darkestBlue;
  static const TCODColor darkestHan;
  static const TCODColor darkestViolet;
  static const TCODColor darkestPurple;
  static const TCODColor darkestFuchsia;
  static const TCODColor darkestMagenta;
  static const TCODColor darkestPink;
  static const TCODColor darkestCrimson;
	
	// light colors
	static const TCODColor lightRed;
  static const TCODColor lightFlame;
  static const TCODColor lightOrange;
  static const TCODColor lightAmber;
  static const TCODColor lightYellow;
  static const TCODColor lightLime;
  static const TCODColor lightChartreuse;
  static const TCODColor lightGreen;
  static const TCODColor lightSea;
  static const TCODColor lightTurquoise;
  static const TCODColor lightCyan;
  static const TCODColor lightSky;
  static const TCODColor lightAzure;
  static const TCODColor lightBlue;
  static const TCODColor lightHan;
  static const TCODColor lightViolet;
  static const TCODColor lightPurple;
  static const TCODColor lightFuchsia;
  static const TCODColor lightMagenta;
  static const TCODColor lightPink;
  static const TCODColor lightCrimson;

  //lighter colors
  static const TCODColor lighterRed;
  static const TCODColor lighterFlame;
  static const TCODColor lighterOrange;
  static const TCODColor lighterAmber;
  static const TCODColor lighterYellow;
  static const TCODColor lighterLime;
  static const TCODColor lighterChartreuse;
  static const TCODColor lighterGreen;
  static const TCODColor lighterSea;
  static const TCODColor lighterTurquoise;
  static const TCODColor lighterCyan;
  static const TCODColor lighterSky;
  static const TCODColor lighterAzure;
  static const TCODColor lighterBlue;
  static const TCODColor lighterHan;
  static const TCODColor lighterViolet;
  static const TCODColor lighterPurple;
  static const TCODColor lighterFuchsia;
  static const TCODColor lighterMagenta;
  static const TCODColor lighterPink;
  static const TCODColor lighterCrimson;

  // lightest colors
  static const TCODColor lightestRed;
  static const TCODColor lightestFlame;
  static const TCODColor lightestOrange;
  static const TCODColor lightestAmber;
  static const TCODColor lightestYellow;
  static const TCODColor lightestLime;
  static const TCODColor lightestChartreuse;
  static const TCODColor lightestGreen;
  static const TCODColor lightestSea;
  static const TCODColor lightestTurquoise;
  static const TCODColor lightestCyan;
  static const TCODColor lightestSky;
  static const TCODColor lightestAzure;
  static const TCODColor lightestBlue;
  static const TCODColor lightestHan;
  static const TCODColor lightestViolet;
  static const TCODColor lightestPurple;
  static const TCODColor lightestFuchsia;
  static const TCODColor lightestMagenta;
  static const TCODColor lightestPink;
  static const TCODColor lightestCrimson;
	
	// desaturated colors
  static const TCODColor desaturatedRed;
  static const TCODColor desaturatedFlame;
  static const TCODColor desaturatedOrange;
  static const TCODColor desaturatedAmber;
  static const TCODColor desaturatedYellow;
  static const TCODColor desaturatedLime;
  static const TCODColor desaturatedChartreuse;
  static const TCODColor desaturatedGreen;
  static const TCODColor desaturatedSea;
  static const TCODColor desaturatedTurquoise;
  static const TCODColor desaturatedCyan;
  static const TCODColor desaturatedSky;
  static const TCODColor desaturatedAzure;
  static const TCODColor desaturatedBlue;
  static const TCODColor desaturatedHan;
  static const TCODColor desaturatedViolet;
  static const TCODColor desaturatedPurple;
  static const TCODColor desaturatedFuchsia;
  static const TCODColor desaturatedMagenta;
  static const TCODColor desaturatedPink;
  static const TCODColor desaturatedCrimson;	
	
	// metallic
	static const TCODColor brass;
	static const TCODColor copper;
	static const TCODColor gold;
	static const TCODColor silver;
	
	// miscellaneous
	static const TCODColor celadon;
	static const TCODColor peach;
};

TCODLIB_API TCODColor operator *(float value, const TCODColor &c);

#endif
