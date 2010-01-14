/*
* libtcod 1.5.0
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
#include <math.h>
#include "libtcod.h"
#include "libtcod_int.h"

// grey levels
const TCOD_color_t TCOD_black={TCOD_BLACK};
const TCOD_color_t TCOD_darker_grey={TCOD_DARKER_GREY};
const TCOD_color_t TCOD_dark_grey={TCOD_DARK_GREY};
const TCOD_color_t TCOD_grey={TCOD_GREY};
const TCOD_color_t TCOD_light_grey={TCOD_LIGHT_GREY};
const TCOD_color_t TCOD_darker_gray={TCOD_DARKER_GREY};
const TCOD_color_t TCOD_dark_gray={TCOD_DARK_GREY};
const TCOD_color_t TCOD_gray={TCOD_GREY};
const TCOD_color_t TCOD_light_gray={TCOD_LIGHT_GREY};
const TCOD_color_t TCOD_white={TCOD_WHITE};

//standard colors
const TCOD_color_t TCOD_red={TCOD_RED};
const TCOD_color_t TCOD_orange={TCOD_ORANGE};
const TCOD_color_t TCOD_yellow={TCOD_YELLOW};
const TCOD_color_t TCOD_chartreuse={TCOD_CHARTREUSE};
const TCOD_color_t TCOD_green={TCOD_GREEN};
const TCOD_color_t TCOD_sea={TCOD_SEA};
const TCOD_color_t TCOD_cyan={TCOD_CYAN};
const TCOD_color_t TCOD_sky={TCOD_SKY};
const TCOD_color_t TCOD_blue={TCOD_BLUE};
const TCOD_color_t TCOD_violet={TCOD_VIOLET};
const TCOD_color_t TCOD_magenta={TCOD_MAGENTA};
const TCOD_color_t TCOD_pink={TCOD_PINK};

// dark colors
const TCOD_color_t TCOD_dark_red={TCOD_DARK_RED};
const TCOD_color_t TCOD_dark_orange={TCOD_DARK_ORANGE};
const TCOD_color_t TCOD_dark_yellow={TCOD_DARK_YELLOW};
const TCOD_color_t TCOD_dark_chartreuse={TCOD_DARK_CHARTREUSE};
const TCOD_color_t TCOD_dark_green={TCOD_DARK_GREEN};
const TCOD_color_t TCOD_dark_sea={TCOD_DARK_SEA};
const TCOD_color_t TCOD_dark_cyan={TCOD_DARK_CYAN};
const TCOD_color_t TCOD_dark_sky={TCOD_DARK_SKY};
const TCOD_color_t TCOD_dark_blue={TCOD_DARK_BLUE};
const TCOD_color_t TCOD_dark_violet={TCOD_DARK_VIOLET};
const TCOD_color_t TCOD_dark_magenta={TCOD_DARK_MAGENTA};
const TCOD_color_t TCOD_dark_pink={TCOD_DARK_PINK};

// darker colors
const TCOD_color_t TCOD_darker_red={TCOD_DARKER_RED};
const TCOD_color_t TCOD_darker_orange={TCOD_DARKER_ORANGE};
const TCOD_color_t TCOD_darker_yellow={TCOD_DARKER_YELLOW};
const TCOD_color_t TCOD_darker_chartreuse={TCOD_DARKER_CHARTREUSE};
const TCOD_color_t TCOD_darker_green={TCOD_DARKER_GREEN};
const TCOD_color_t TCOD_darker_sea={TCOD_DARKER_SEA};
const TCOD_color_t TCOD_darker_cyan={TCOD_DARKER_CYAN};
const TCOD_color_t TCOD_darker_sky={TCOD_DARKER_SKY};
const TCOD_color_t TCOD_darker_blue={TCOD_DARKER_BLUE};
const TCOD_color_t TCOD_darker_violet={TCOD_DARKER_VIOLET};
const TCOD_color_t TCOD_darker_magenta={TCOD_DARKER_MAGENTA};
const TCOD_color_t TCOD_darker_pink={TCOD_DARKER_PINK};

// light colors
const TCOD_color_t TCOD_light_red={TCOD_LIGHT_RED};
const TCOD_color_t TCOD_light_orange={TCOD_LIGHT_ORANGE};
const TCOD_color_t TCOD_light_yellow={TCOD_LIGHT_YELLOW};
const TCOD_color_t TCOD_light_chartreuse={TCOD_LIGHT_CHARTREUSE};
const TCOD_color_t TCOD_light_green={TCOD_LIGHT_GREEN};
const TCOD_color_t TCOD_light_sea={TCOD_LIGHT_SEA};
const TCOD_color_t TCOD_light_cyan={TCOD_LIGHT_CYAN};
const TCOD_color_t TCOD_light_sky={TCOD_LIGHT_SKY};
const TCOD_color_t TCOD_light_blue={TCOD_LIGHT_BLUE};
const TCOD_color_t TCOD_light_violet={TCOD_LIGHT_VIOLET};
const TCOD_color_t TCOD_light_magenta={TCOD_LIGHT_MAGENTA};
const TCOD_color_t TCOD_light_pink={TCOD_LIGHT_PINK};

// desaturated colors
const TCOD_color_t TCOD_desaturated_red={TCOD_DESATURATED_RED};
const TCOD_color_t TCOD_desaturated_orange={TCOD_DESATURATED_ORANGE};
const TCOD_color_t TCOD_desaturated_yellow={TCOD_DESATURATED_YELLOW};
const TCOD_color_t TCOD_desaturated_chartreuse={TCOD_DESATURATED_CHARTREUSE};
const TCOD_color_t TCOD_desaturated_green={TCOD_DESATURATED_GREEN};
const TCOD_color_t TCOD_desaturated_sea={TCOD_DESATURATED_SEA};
const TCOD_color_t TCOD_desaturated_cyan={TCOD_DESATURATED_CYAN};
const TCOD_color_t TCOD_desaturated_sky={TCOD_DESATURATED_SKY};
const TCOD_color_t TCOD_desaturated_blue={TCOD_DESATURATED_BLUE};
const TCOD_color_t TCOD_desaturated_violet={TCOD_DESATURATED_VIOLET};
const TCOD_color_t TCOD_desaturated_magenta={TCOD_DESATURATED_MAGENTA};
const TCOD_color_t TCOD_desaturated_pink={TCOD_DESATURATED_PINK};

// special
const TCOD_color_t TCOD_silver={TCOD_SILVER};
const TCOD_color_t TCOD_gold={TCOD_GOLD};

// color array
const TCOD_color_t TCOD_colors[TCOD_COLOR_NB][TCOD_COLOR_LEVELS] = {
 {{TCOD_DESATURATED_RED},{TCOD_LIGHT_RED},{TCOD_RED},{TCOD_DARK_RED},{TCOD_DARKER_RED}},
 {{TCOD_DESATURATED_ORANGE},{TCOD_LIGHT_ORANGE},{TCOD_ORANGE},{TCOD_DARK_ORANGE},{TCOD_DARKER_ORANGE}},
 {{TCOD_DESATURATED_YELLOW},{TCOD_LIGHT_YELLOW},{TCOD_YELLOW},{TCOD_DARK_YELLOW},{TCOD_DARKER_YELLOW}},
 {{TCOD_DESATURATED_CHARTREUSE},{TCOD_LIGHT_CHARTREUSE},{TCOD_CHARTREUSE},{TCOD_DARK_CHARTREUSE},{TCOD_DARKER_CHARTREUSE}},
 {{TCOD_DESATURATED_GREEN},{TCOD_LIGHT_GREEN},{TCOD_GREEN},{TCOD_DARK_GREEN},{TCOD_DARKER_GREEN}},
 {{TCOD_DESATURATED_SEA},{TCOD_LIGHT_SEA},{TCOD_SEA},{TCOD_DARK_SEA},{TCOD_DARKER_SEA}},
 {{TCOD_DESATURATED_CYAN},{TCOD_LIGHT_CYAN},{TCOD_CYAN},{TCOD_DARK_CYAN},{TCOD_DARKER_CYAN}},
 {{TCOD_DESATURATED_SKY},{TCOD_LIGHT_SKY},{TCOD_SKY},{TCOD_DARK_SKY},{TCOD_DARKER_SKY}},
 {{TCOD_DESATURATED_BLUE},{TCOD_LIGHT_BLUE},{TCOD_BLUE},{TCOD_DARK_BLUE},{TCOD_DARKER_BLUE}},
 {{TCOD_DESATURATED_VIOLET},{TCOD_LIGHT_VIOLET},{TCOD_VIOLET},{TCOD_DARK_VIOLET},{TCOD_DARKER_VIOLET}},
 {{TCOD_DESATURATED_MAGENTA},{TCOD_LIGHT_MAGENTA},{TCOD_MAGENTA},{TCOD_DARK_MAGENTA},{TCOD_DARKER_MAGENTA}},
 {{TCOD_DESATURATED_PINK},{TCOD_LIGHT_PINK},{TCOD_PINK},{TCOD_DARK_PINK},{TCOD_DARKER_PINK}},
};


bool TCOD_color_equals (TCOD_color_t c1, TCOD_color_t  c2) {
	return (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b);
}

TCOD_color_t TCOD_color_add (TCOD_color_t c1, TCOD_color_t c2) {
	TCOD_color_t ret;
	int r,g,b;
	r = (int)(c1.r) + c2.r;
	g = (int)(c1.g) + c2.g;
	b = (int)(c1.b) + c2.b;
	r=MIN(255,r);
	g=MIN(255,g);
	b=MIN(255,b);
	ret.r=(uint8)r;
	ret.g=(uint8)g;
	ret.b=(uint8)b;
	return ret;
}

TCOD_color_t TCOD_color_subtract (TCOD_color_t c1, TCOD_color_t c2) {
	TCOD_color_t ret;
	int r,g,b;
	r = (int)(c1.r) - c2.r;
	g = (int)(c1.g) - c2.g;
	b = (int)(c1.b) - c2.b;
	r=MAX(0,r);
	g=MAX(0,g);
	b=MAX(0,b);
	ret.r=(uint8)r;
	ret.g=(uint8)g;
	ret.b=(uint8)b;
	return ret;
}

TCOD_color_t TCOD_color_multiply (TCOD_color_t c1, TCOD_color_t c2) {
	TCOD_color_t ret;
	ret.r=(uint8)(((int)c1.r)*c2.r/255);
	ret.g=(uint8)(((int)c1.g)*c2.g/255);
	ret.b=(uint8)(((int)c1.b)*c2.b/255);
	return ret;
}

TCOD_color_t TCOD_color_multiply_scalar (TCOD_color_t c1, float value) {
	TCOD_color_t ret;
	int r,g,b;
	r = (int)(c1.r * value);
	g = (int)(c1.g * value);
	b = (int)(c1.b * value);
	ret.r=(uint8)CLAMP(0,255,r);
	ret.g=(uint8)CLAMP(0,255,g);
	ret.b=(uint8)CLAMP(0,255,b);
	return ret;
}

TCOD_color_t TCOD_color_lerp(TCOD_color_t c1, TCOD_color_t c2, float coef) {
	TCOD_color_t ret;
	ret.r=(uint8)(c1.r+(c2.r-c1.r)*coef);
	ret.g=(uint8)(c1.g+(c2.g-c1.g)*coef);
	ret.b=(uint8)(c1.b+(c2.b-c1.b)*coef);
	return ret;
}

// 0<= h < 360, 0 <= s <= 1, 0 <= v <= 1 
void TCOD_color_set_HSV(TCOD_color_t *c, float h, float s, float v)
{
	int i;
	float f, p, q, t;

	if( s == 0 ) {
		// achromatic (grey)
		c->r = c->g = c->b = (uint8)(v*255);
		return;
	}

	h /= 60;			// sector 0 to 5
	i = (int)floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch( i ) {
		case 0:
			c->r = (uint8)(v*255);
			c->g = (uint8)(t*255);
			c->b = (uint8)(p*255);
			break;
		case 1:
			c->r = (uint8)(q*255);
			c->g = (uint8)(v*255);
			c->b = (uint8)(p*255);
			break;
		case 2:
			c->r = (uint8)(p*255);
			c->g = (uint8)(v*255);
			c->b = (uint8)(t*255);
			break;
		case 3:
			c->r = (uint8)(p*255);
			c->g = (uint8)(q*255);
			c->b = (uint8)(v*255);
			break;
		case 4:
			c->r = (uint8)(t*255);
			c->g = (uint8)(p*255);
			c->b = (uint8)(v*255);
			break;
		default:
			c->r = (uint8)(v*255);
			c->g = (uint8)(p*255);
			c->b = (uint8)(q*255);
			break;
	}
}

void TCOD_color_get_HSV(TCOD_color_t c, float *h, float *s, float *v)
{
  uint8 imax,imin;
	float min, max, delta;

	imax = ( c.r > c.g ? 
			( c.r > c.b ? c.r : c.b )
			: ( c.g > c.b ? c.g : c.b) );
	imin = ( c.r < c.g ? 
			( c.r < c.b ? c.r : c.b )
			: ( c.g < c.b ? c.g : c.b) );
	max = imax/255.0f;
	min = imin/255.0f;
	*v = max; // v
	
	delta = max - min;
	if( max != 0 ) *s = delta / max; // s
	else 
	{
		*s = 0; // s
		*h= -1; // h
		return;
	}
	
	if( c.r == imax ) *h = ( c.g - c.b ) / (255 * delta);		// between yellow & magenta
	else if( c.g == imax )	*h = 2 + ( c.b - c.r ) / (255 * delta);	// between cyan & yellow
	else *h = 4 + ( c.r - c.g ) / (255 * delta);	// between magenta & cyan
	
	*h *= 60;				// degrees
	if( *h < 0 ) *h += 360;
}

void TCOD_color_gen_map(TCOD_color_t *map, int nb_key, TCOD_color_t const  *key_color, int const  *key_index) {
	int segment=0;
	for (segment=0; segment < nb_key-1; segment++) {
		int idx_start=key_index[segment];
		int idx_end=key_index[segment+1];
		int idx;
		for ( idx=idx_start;idx <= idx_end; idx++) {
			map[idx]=TCOD_color_lerp(key_color[segment],key_color[segment+1],(float)(idx-idx_start)/(idx_end-idx_start));
		}
	}
}


