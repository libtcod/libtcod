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
#include <math.h>
#include "libtcod.h"

TCOD_color_t TCOD_black={0,0,0};
TCOD_color_t TCOD_dark_grey={96,96,96};
TCOD_color_t TCOD_grey={196,196,196};
TCOD_color_t TCOD_dark_gray={96,96,96};
TCOD_color_t TCOD_gray={196,196,196};
TCOD_color_t TCOD_white={255,255,255};
TCOD_color_t TCOD_dark_blue={40,40,128};
TCOD_color_t TCOD_light_blue={120,120,255};
TCOD_color_t TCOD_dark_red={128,0,0};
TCOD_color_t TCOD_light_red={255,100,50};
TCOD_color_t TCOD_dark_brown={32,16,0};
TCOD_color_t TCOD_light_yellow={255,255,150};
TCOD_color_t TCOD_yellow={255,255,0};
TCOD_color_t TCOD_dark_yellow={164,164,0};
TCOD_color_t TCOD_green={0,220,0};
TCOD_color_t TCOD_orange={255,150,0};
TCOD_color_t TCOD_red={255,0,0};
TCOD_color_t TCOD_silver={203,203,203};
TCOD_color_t TCOD_gold={255,255,102};
TCOD_color_t TCOD_purple={204,51,153};
TCOD_color_t TCOD_dark_purple={51,0,51};

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


