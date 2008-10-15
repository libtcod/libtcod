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

#ifndef _TCOD_COLOR_H
#define _TCOD_COLOR_H

typedef struct {
	uint8 r,g,b;
} TCOD_color_t;
	
TCODLIB_API bool TCOD_color_equals (TCOD_color_t c1, TCOD_color_t c2);
TCODLIB_API TCOD_color_t TCOD_color_add (TCOD_color_t c1, TCOD_color_t c2);
TCODLIB_API TCOD_color_t TCOD_color_multiply (TCOD_color_t c1, TCOD_color_t c2);
TCODLIB_API TCOD_color_t TCOD_color_multiply_scalar (TCOD_color_t c1, float value);
TCODLIB_API TCOD_color_t TCOD_color_lerp(TCOD_color_t c1, TCOD_color_t c2, float coef);
TCODLIB_API void TCOD_color_set_HSV(TCOD_color_t *c,float h, float s, float v);
TCODLIB_API void TCOD_color_get_HSV(TCOD_color_t c,float * h, float * s, float * v);
TCODLIB_API void TCOD_color_gen_map(TCOD_color_t *map, int nb_key, TCOD_color_t const *key_color, int const *key_index);

extern TCODLIB_API TCOD_color_t TCOD_black;
extern TCODLIB_API TCOD_color_t TCOD_dark_grey;
extern TCODLIB_API TCOD_color_t TCOD_grey;
extern TCODLIB_API TCOD_color_t TCOD_dark_gray;
extern TCODLIB_API TCOD_color_t TCOD_gray;
extern TCODLIB_API TCOD_color_t TCOD_white;
extern TCODLIB_API TCOD_color_t TCOD_dark_blue;
extern TCODLIB_API TCOD_color_t TCOD_light_blue;
extern TCODLIB_API TCOD_color_t TCOD_dark_red;
extern TCODLIB_API TCOD_color_t TCOD_light_red;
extern TCODLIB_API TCOD_color_t TCOD_dark_brown;
extern TCODLIB_API TCOD_color_t TCOD_light_yellow;
extern TCODLIB_API TCOD_color_t TCOD_yellow;
extern TCODLIB_API TCOD_color_t TCOD_dark_yellow;
extern TCODLIB_API TCOD_color_t TCOD_green;
extern TCODLIB_API TCOD_color_t TCOD_orange;
extern TCODLIB_API TCOD_color_t TCOD_red;
extern TCODLIB_API TCOD_color_t TCOD_silver;
extern TCODLIB_API TCOD_color_t TCOD_gold;
extern TCODLIB_API TCOD_color_t TCOD_purple;
extern TCODLIB_API TCOD_color_t TCOD_dark_purple;

#endif
