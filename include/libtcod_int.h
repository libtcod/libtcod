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

#ifndef _TCODLIB_INT_H
#define _TCODLIB_INT_H
#include <stdarg.h>
/* tcodlib internal stuff */
#ifdef __cplusplus
extern "C" {
#endif

/* a cell in the console */
typedef struct {
	int c;		/* character ascii code */
	int cf;		/* character number if font */
	TCOD_color_t fore;	/* foreground color */
	TCOD_color_t back;	/* background color */
	uint8 dirt;	/* cell modified since last flush ? */
} char_t;

/* TCODConsole non public data */
typedef struct {
	char_t *buf; /* current console */
	char_t *oldbuf; /* console for last frame */
	uint8 fade;
	bool windowed;
	bool fullscreen;
	bool haskey; /* a key color has been defined */
	/* foreground (text), background and key colors */
	TCOD_color_t fore,back,key;
	/* console width and height (in characters,not pixels) */
	int w,h;
} TCOD_console_data_t;

typedef enum {
	LEFT,CENTER,RIGHT
} alignment_t;

/* TCODConsole non public methods*/
bool TCOD_console_init(TCOD_console_t con,const char *title, bool fullscreen);
int TCOD_console_print(TCOD_console_t con,int x,int y, int w, int h, TCOD_bkgnd_flag_t flag, alignment_t align, char *msg, bool can_split);
int TCOD_console_stringLength(const char *s);
char * TCOD_console_forward(char *s,int l);
void TCOD_console_set_window_closed();
char *TCOD_console_vsprint(const char *fmt, va_list ap);
char_t *TCOD_console_get_buf(TCOD_console_t con);
/* fatal errors */
void TCOD_fatal(const char *fmt, ...);
void TCOD_fatal_nopar(const char *msg);

/* TCODSystem non public methods */
#define TCOD_MAX_FONT_CHARS 1024

extern int fontNbCharHoriz;
extern int fontNbCharVertic;
extern bool fontTcodLayout;
extern int ascii_to_tcod[TCOD_MAX_FONT_CHARS];

bool TCOD_sys_init(int w,int h, char_t *buf, char_t *oldbuf, bool fullscreen);
void TCOD_sys_set_custom_font(const char *font_name,int cw, int ch,int flags);
void TCOD_sys_map_ascii_to_font(int asciiCode, int fontCharX, int fontCharY); 
void *TCOD_sys_create_bitmap_for_console(TCOD_console_t console);
void TCOD_sys_save_bitmap(void *bitmap, const char *filename);
void *TCOD_sys_create_bitmap(int width, int height, TCOD_color_t *buf);
void TCOD_sys_delete_bitmap(void *bitmap);
void TCOD_sys_console_to_bitmap(void *bitmap, int console_width, int console_height, char_t *console_buffer, char_t *prev_console_buffer);
void TCOD_sys_get_char_size(int *w, int *h);
void TCOD_sys_set_keyboard_repeat(int initial_delay, int interval);
void *TCOD_sys_get_surface(int width, int height, bool alpha);
/* switch fullscreen mode */
void TCOD_sys_set_fullscreen(bool fullscreen);
void TCOD_sys_flush(bool render);
TCOD_key_t TCOD_sys_check_for_keypress(int flags);
TCOD_key_t TCOD_sys_wait_for_keypress(bool flush);
bool TCOD_sys_is_key_pressed(TCOD_keycode_t key);
void TCOD_sys_set_window_title(const char *title);
/* close the window */
void TCOD_sys_term();

/* image manipulation */
void *TCOD_sys_load_image(const char *filename);
void TCOD_sys_get_image_size(const void *image, int *w,int *h);
TCOD_color_t TCOD_sys_get_image_pixel(const void *image,int x, int y);
bool TCOD_sys_check_magic_number(const char *filename, int size, uint8 *data);

/* TCOD_list nonpublic methods */
void TCOD_list_set_size(TCOD_list_t l, int size);

#ifdef __cplusplus
}
#endif
#endif

