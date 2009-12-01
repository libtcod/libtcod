/*
* libtcod 1.4.3
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
#include <string.h>
#include "libtcod.h"
#include "libtcod_int.h"

#include "wrappers.h"

#define RED_MASK   0xFF0000
#define GREEN_MASK 0x00FF00
#define BLUE_MASK  0x0000FF

TCOD_color_t int_to_color (colornum_t col) {
  TCOD_color_t ret;
  ret.r = (col & RED_MASK) >> 16;
  ret.g = (col & GREEN_MASK) >> 8;
  ret.b = col & BLUE_MASK;
  return ret;
}

#define color_to_int(col) (int)(((int)((col).r) << 16) | ((col).g << 8) | (col).b)

bool TCOD_color_equals_wrapper (colornum_t c1, colornum_t c2) {
  return TCOD_color_equals (int_to_color(c1), int_to_color(c2));
}

colornum_t TCOD_color_add_wrapper (colornum_t c1, colornum_t c2) {
  return color_to_int(TCOD_color_add (int_to_color(c1),
				      int_to_color(c2)));
}

colornum_t TCOD_color_subtract_wrapper (colornum_t c1, colornum_t c2) {
  return color_to_int(TCOD_color_subtract (int_to_color(c1),
				      int_to_color(c2)));
}

colornum_t TCOD_color_multiply_wrapper (colornum_t c1, colornum_t c2)
{
  return color_to_int(TCOD_color_multiply (int_to_color(c1), int_to_color(c2)));
}

colornum_t TCOD_color_multiply_scalar_wrapper (colornum_t c1, float value)
{
  return color_to_int(TCOD_color_multiply_scalar (int_to_color(c1),
						  value));
}

colornum_t TCOD_color_lerp_wrapper(colornum_t c1, colornum_t c2, float coef)
{
  return color_to_int(TCOD_color_lerp (int_to_color(c1),int_to_color(c2),
				       coef));
}

void TCOD_color_get_HSV_wrapper(colornum_t c,float * h,
				float * s, float * v)
{
  TCOD_color_get_HSV (int_to_color(c), h, s, v);
}

/* void TCOD_console_set_custom_font_wrapper(const char *fontFile, */
/* 					  int char_width, int char_height, */
/* 					  int flags) */
/* /\* int nb_char_horiz, */
/*                        int nb_char_vertic, bool chars_by_row, */
/*                        colornum_t key_color) *\/ */
/* { */
/*   TCOD_console_set_custom_font (fontFile, */
/* 				char_width, char_height, nb_char_horiz, */
/* 				nb_char_vertic, chars_by_row, */
/* 				int_to_color(key_color)); */
/* } */


colornum_t TCOD_console_get_background_color_wrapper(TCOD_console_t con)
{
  return color_to_int(TCOD_console_get_background_color (con));
}


colornum_t TCOD_console_get_foreground_color_wrapper(TCOD_console_t con)
{
  return color_to_int(TCOD_console_get_foreground_color (con));
}


void TCOD_console_set_background_color_wrapper(TCOD_console_t con,
						   colornum_t col)
{
   TCOD_console_set_background_color (con,
				      int_to_color(col));
}

void TCOD_console_set_foreground_color_wrapper(TCOD_console_t con,
						   colornum_t col)
{
  TCOD_console_set_foreground_color (con,
				     int_to_color(col));
}

colornum_t TCOD_console_get_back_wrapper(TCOD_console_t con,
						       int x, int y)
{
  return color_to_int(TCOD_console_get_back (con, x, y));
}

void TCOD_console_set_back_wrapper(TCOD_console_t con,int x, int y,
                                      colornum_t col,
                                      TCOD_bkgnd_flag_t flag)
{
  TCOD_console_set_back (con, x, y, int_to_color(col), flag);
}


void TCOD_console_set_fore_wrapper(TCOD_console_t con,int x, int y,
				   colornum_t col)
{
  TCOD_console_set_fore (con, x, y, int_to_color(col));
}

void TCOD_console_put_char_ex_wrapper(TCOD_console_t con, int x, int y,
                                       int c, colornum_t fore, colornum_t back)
{
  TCOD_console_put_char_ex (con, x, y, c, int_to_color(fore),
                             int_to_color(back));
}

colornum_t TCOD_console_get_fore_wrapper(TCOD_console_t con,
                                              int x, int y)
{
  return color_to_int(TCOD_console_get_fore (con, x, y));
}

void TCOD_console_set_fade_wrapper(uint8 val, colornum_t fade)
{
  TCOD_console_set_fade (val, int_to_color(fade));
}

colornum_t
TCOD_console_get_fading_color_wrapper ()
{
  return color_to_int(TCOD_console_get_fading_color());
}

void TCOD_console_set_color_control_wrapper(TCOD_colctrl_t con,
						colornum_t fore,
						colornum_t back)
{
  TCOD_console_set_color_control (con,
				  int_to_color(fore),
				  int_to_color(back));
}

void TCOD_image_clear_wrapper(TCOD_image_t image,
					  colornum_t color)
{
  TCOD_image_clear (image,
		    int_to_color(color));
}

colornum_t TCOD_image_get_pixel_wrapper(TCOD_image_t image,
						      int x, int y)
{
  return color_to_int(TCOD_image_get_pixel (image, x, y));
}

colornum_t TCOD_image_get_mipmap_pixel_wrapper(TCOD_image_t image,
				 float x0,float y0, float x1, float y1)
{
  return color_to_int(TCOD_image_get_mipmap_pixel (image,
						   x0, y0, x1, y1));
}

void TCOD_image_put_pixel_wrapper(TCOD_image_t image,int x, int y,
				      colornum_t col)
{
   TCOD_image_put_pixel (image, x, y,
			 int_to_color( col));
}

void TCOD_image_set_key_color_wrapper(TCOD_image_t image,
					  colornum_t key_color)
{
  TCOD_image_set_key_color (image,
			    int_to_color(key_color));
}


bool TCOD_console_check_for_keypress_wrapper (TCOD_key_t *holder, int flags)
{
  *holder = TCOD_console_check_for_keypress(flags);
  return (holder->vk != TCODK_NONE);
}


void TCOD_console_wait_for_keypress_wrapper (TCOD_key_t *holder, bool flush)
{
  *holder = TCOD_console_wait_for_keypress(flush);
}

void TCOD_mouse_get_status_wrapper(TCOD_mouse_t *holder)
{
  *holder = TCOD_mouse_get_status();
}

/* Routines to draw hlines, vlines and frames using the double-lined
 * characters. */

static TCOD_console_t root = NULL;

void TCOD_console_double_hline(TCOD_console_t con,int x,int y, int l, TCOD_bkgnd_flag_t flag) {
	int i;
	for (i=x; i< x+l; i++) TCOD_console_put_char(con,i,y,TCOD_CHAR_DHLINE,flag);
}

void TCOD_console_double_vline(TCOD_console_t con,int x,int y, int l, TCOD_bkgnd_flag_t flag) {
	int i;
	for (i=y; i< y+l; i++) TCOD_console_put_char(con,x,i,TCOD_CHAR_DVLINE,flag);
}


void TCOD_console_print_double_frame(TCOD_console_t con,int x,int y,int w,int h, bool empty, TCOD_bkgnd_flag_t flag, const char *fmt, ...) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_console_put_char(con,x,y,TCOD_CHAR_DNW,flag);
	TCOD_console_put_char(con,x+w-1,y,TCOD_CHAR_DNE,flag);
	TCOD_console_put_char(con,x,y+h-1,TCOD_CHAR_DSW,flag);
	TCOD_console_put_char(con,x+w-1,y+h-1,TCOD_CHAR_DSE,flag);
	TCOD_console_double_hline(con,x+1,y,w-2, flag);
	TCOD_console_double_hline(con,x+1,y+h-1,w-2, flag);
	TCOD_console_double_vline(con,x,y+1,h-2, flag);
	TCOD_console_double_vline(con,x+w-1,y+1,h-2, flag);
	if ( h > 2 ) {
		TCOD_console_vline(con,x,y+1,h-2,flag);
		TCOD_console_vline(con,x+w-1,y+1,h-2,flag);
		if ( empty ) {
			TCOD_console_rect(con,x+1,y+1,w-2,h-2,true,flag);
		}
	}
	if (fmt) {
		va_list ap;
		int xs;
		TCOD_color_t tmp;
		char *title;
		va_start(ap,fmt);
		title = TCOD_console_vsprint(fmt,ap);
		va_end(ap);
		title[w-3]=0; // truncate if needed
		xs = x + (w-strlen(title)-2)/2;
		tmp=dat->back; // swap colors
		dat->back=dat->fore;
		dat->fore=tmp;
		TCOD_console_print_left(con,xs,y,TCOD_BKGND_SET," %s ",title);
		tmp=dat->back; // swap colors
		dat->back=dat->fore;
		dat->fore=tmp;
	}
}


char *TCOD_console_print_return_string(TCOD_console_t con,int x,int y, int rw,
				     int rh, TCOD_bkgnd_flag_t flag,
	alignment_t align, char *msg, bool can_split, bool count_only) {
  TCOD_console_print(con,x,y,rw,rh,flag,align,msg,can_split,count_only);
  return msg;
}

colornum_t TCOD_parser_get_color_property_wrapper(TCOD_parser_t parser, const char *name) {
	return color_to_int(TCOD_parser_get_color_property(parser,name));
}

