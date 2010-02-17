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

#ifndef _TCOD_COLOR_H
#define _TCOD_COLOR_H

typedef struct {
	uint8 r,g,b;
} TCOD_color_t;
	
TCODLIB_API bool TCOD_color_equals (TCOD_color_t c1, TCOD_color_t c2);
TCODLIB_API TCOD_color_t TCOD_color_add (TCOD_color_t c1, TCOD_color_t c2);
TCODLIB_API TCOD_color_t TCOD_color_subtract (TCOD_color_t c1, TCOD_color_t c2);
TCODLIB_API TCOD_color_t TCOD_color_multiply (TCOD_color_t c1, TCOD_color_t c2);
TCODLIB_API TCOD_color_t TCOD_color_multiply_scalar (TCOD_color_t c1, float value);
TCODLIB_API TCOD_color_t TCOD_color_lerp(TCOD_color_t c1, TCOD_color_t c2, float coef);
TCODLIB_API void TCOD_color_set_HSV(TCOD_color_t *c,float h, float s, float v);
TCODLIB_API void TCOD_color_get_HSV(TCOD_color_t c,float * h, float * s, float * v);
TCODLIB_API void TCOD_color_gen_map(TCOD_color_t *map, int nb_key, TCOD_color_t const *key_color, int const *key_index);

#define TCOD_COLOR_NB 12
#define TCOD_COLOR_LEVELS 6

// color names
enum {
	TCOD_COLOR_RED,
	TCOD_COLOR_ORANGE,
	TCOD_COLOR_YELLOW,
	TCOD_COLOR_CHARTREUSE,
	TCOD_COLOR_GREEN,
	TCOD_COLOR_SEA,
	TCOD_COLOR_CYAN,
	TCOD_COLOR_SKY,
	TCOD_COLOR_BLUE,
	TCOD_COLOR_VIOLET,
	TCOD_COLOR_MAGENTA,
	TCOD_COLOR_PINK,
};

// color levels
enum {
	TCOD_COLOR_DESATURATED,
	TCOD_COLOR_LIGHT,
	TCOD_COLOR_NORMAL,
	TCOD_COLOR_DARK,
	TCOD_COLOR_DARKER,
};

// color array
extern TCODLIB_API const TCOD_color_t  TCOD_colors[TCOD_COLOR_NB][TCOD_COLOR_LEVELS];

// grey levels
extern TCODLIB_API const TCOD_color_t TCOD_black;
extern TCODLIB_API const TCOD_color_t TCOD_darker_grey;
extern TCODLIB_API const TCOD_color_t TCOD_dark_grey;
extern TCODLIB_API const TCOD_color_t TCOD_grey;
extern TCODLIB_API const TCOD_color_t TCOD_light_grey;
extern TCODLIB_API const TCOD_color_t TCOD_darker_gray;
extern TCODLIB_API const TCOD_color_t TCOD_dark_gray;
extern TCODLIB_API const TCOD_color_t TCOD_gray;
extern TCODLIB_API const TCOD_color_t TCOD_light_gray;
extern TCODLIB_API const TCOD_color_t TCOD_white;

// standard colors
extern TCODLIB_API const TCOD_color_t TCOD_red;
extern TCODLIB_API const TCOD_color_t TCOD_orange;
extern TCODLIB_API const TCOD_color_t TCOD_yellow;
extern TCODLIB_API const TCOD_color_t TCOD_chartreuse;
extern TCODLIB_API const TCOD_color_t TCOD_green;
extern TCODLIB_API const TCOD_color_t TCOD_sea;
extern TCODLIB_API const TCOD_color_t TCOD_cyan;
extern TCODLIB_API const TCOD_color_t TCOD_sky;
extern TCODLIB_API const TCOD_color_t TCOD_blue;
extern TCODLIB_API const TCOD_color_t TCOD_violet;
extern TCODLIB_API const TCOD_color_t TCOD_magenta;
extern TCODLIB_API const TCOD_color_t TCOD_pink;

// dark colors
extern TCODLIB_API const TCOD_color_t TCOD_dark_red;
extern TCODLIB_API const TCOD_color_t TCOD_dark_orange;
extern TCODLIB_API const TCOD_color_t TCOD_dark_yellow;
extern TCODLIB_API const TCOD_color_t TCOD_dark_chartreuse;
extern TCODLIB_API const TCOD_color_t TCOD_dark_green;
extern TCODLIB_API const TCOD_color_t TCOD_dark_sea;
extern TCODLIB_API const TCOD_color_t TCOD_dark_cyan;
extern TCODLIB_API const TCOD_color_t TCOD_dark_sky;
extern TCODLIB_API const TCOD_color_t TCOD_dark_blue;
extern TCODLIB_API const TCOD_color_t TCOD_dark_violet;
extern TCODLIB_API const TCOD_color_t TCOD_dark_magenta;
extern TCODLIB_API const TCOD_color_t TCOD_dark_pink;

// darker colors
extern TCODLIB_API const TCOD_color_t TCOD_darker_red;
extern TCODLIB_API const TCOD_color_t TCOD_darker_orange;
extern TCODLIB_API const TCOD_color_t TCOD_darker_yellow;
extern TCODLIB_API const TCOD_color_t TCOD_darker_chartreuse;
extern TCODLIB_API const TCOD_color_t TCOD_darker_green;
extern TCODLIB_API const TCOD_color_t TCOD_darker_sea;
extern TCODLIB_API const TCOD_color_t TCOD_darker_cyan;
extern TCODLIB_API const TCOD_color_t TCOD_darker_sky;
extern TCODLIB_API const TCOD_color_t TCOD_darker_blue;
extern TCODLIB_API const TCOD_color_t TCOD_darker_violet;
extern TCODLIB_API const TCOD_color_t TCOD_darker_magenta;
extern TCODLIB_API const TCOD_color_t TCOD_darker_pink;

// light colors
extern TCODLIB_API const TCOD_color_t TCOD_light_red;
extern TCODLIB_API const TCOD_color_t TCOD_light_orange;
extern TCODLIB_API const TCOD_color_t TCOD_light_yellow;
extern TCODLIB_API const TCOD_color_t TCOD_light_chartreuse;
extern TCODLIB_API const TCOD_color_t TCOD_light_green;
extern TCODLIB_API const TCOD_color_t TCOD_light_sea;
extern TCODLIB_API const TCOD_color_t TCOD_light_cyan;
extern TCODLIB_API const TCOD_color_t TCOD_light_sky;
extern TCODLIB_API const TCOD_color_t TCOD_light_blue;
extern TCODLIB_API const TCOD_color_t TCOD_light_violet;
extern TCODLIB_API const TCOD_color_t TCOD_light_magenta;
extern TCODLIB_API const TCOD_color_t TCOD_light_pink;

// desaturated
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_red;
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_orange;
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_yellow;
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_chartreuse;
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_green;
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_sea;
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_cyan;
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_sky;
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_blue;
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_violet;
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_magenta;
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_pink;

// special
extern TCODLIB_API const TCOD_color_t TCOD_silver;
extern TCODLIB_API const TCOD_color_t TCOD_gold;

#endif
