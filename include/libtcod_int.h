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

#ifndef _TCODLIB_INT_H
#define _TCODLIB_INT_H
#include <stdarg.h>
#include <assert.h>
/* tcodlib internal stuff */
#ifdef __cplusplus
extern "C" {
#endif

/* a cell in the console */
typedef struct {
	int c;		/* character ascii code */
	int cf;		/* character number in font */
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

/* fov internal stuff */
typedef struct {
	unsigned int transparent:1;
	unsigned int walkable:1;
	unsigned int fov:1;
} cell_t;
typedef struct {
	int width;
	int height;
	int nbcells;
	cell_t *cells;
} map_t;

/* pseudorandom number generator toolkit */
typedef struct {
	/* algorithm identifier */
	TCOD_random_algo_t algo;
	/* Mersenne Twister stuff */
	uint32 mt[624];
	int cur_mt;
	/* Complementary-Multiply-With-Carry stuff */
	/* shared with Generalised Feedback Shift Register */
	uint32 Q[4096], c;
    int cur;
} mersenne_data_t;

#ifdef NDEBUG
#define TCOD_IF(x) if (x)
#define TCOD_IFNOT(x) if (!(x))
#define TCOD_ASSERT(x)
#else
#include <assert.h>
#define TCOD_IF(x) assert(x);
#define TCOD_IFNOT(x) assert(x); if (0)
#define TCOD_ASSERT(x) assert(x)
#endif

#ifndef NO_OPENGL
/* opengl utilities */
void TCOD_opengl_init_attributes();
bool TCOD_opengl_init_state(int conw, int conh, void *font_tex);
bool TCOD_opengl_init_shaders();
bool TCOD_opengl_render(int oldFade, bool *ascii_updated, char_t *console_buffer, char_t *prev_console_buffer);
void TCOD_opengl_swap();
extern bool TCOD_use_open_gl;
extern bool TCOD_use_glsl;
#endif

/* fov internal stuff */
void TCOD_map_compute_fov_circular_raycasting(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls);
void TCOD_map_compute_fov_diamond_raycasting(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls);
void TCOD_map_compute_fov_recursive_shadowcasting(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls);
void TCOD_map_compute_fov_permissive2(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls, int fovType);
void TCOD_map_compute_fov_restrictive_shadowcasting(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls);
void TCOD_map_postproc(map_t *map,int x0,int y0, int x1, int y1, int dx, int dy);

/* TCODConsole non public methods*/
bool TCOD_console_init(TCOD_console_t con,const char *title, bool fullscreen);
int TCOD_console_print(TCOD_console_t con,int x,int y, int w, int h, TCOD_bkgnd_flag_t flag, alignment_t align, char *msg, bool can_split, bool count_only);
int TCOD_console_stringLength(const char *s);
char * TCOD_console_forward(char *s,int l);
void TCOD_console_set_window_closed();
char *TCOD_console_vsprint(const char *fmt, va_list ap);
char_t *TCOD_console_get_buf(TCOD_console_t con);
/* fatal errors */
void TCOD_fatal(const char *fmt, ...);
void TCOD_fatal_nopar(const char *msg);

extern int fontNbCharHoriz;
extern int fontNbCharVertic;
extern bool fontTcodLayout;
extern int *TCOD_ascii_to_tcod;
extern bool TCOD_font_is_in_row;
extern TCOD_console_t TCOD_root;
extern int TCOD_max_font_chars;
extern int TCOD_font_width;
extern int TCOD_font_height;
extern bool TCOD_fullscreen_on;
extern int TCOD_fullscreen_offsetx;
extern int TCOD_fullscreen_offsety;


/* TCODSystem non public methods */
void TCOD_sys_startup();
bool TCOD_sys_init(int w,int h, char_t *buf, char_t *oldbuf, bool fullscreen);
void TCOD_sys_set_custom_font(const char *font_name,int nb_ch, int nb_cv,int flags);
void TCOD_sys_map_ascii_to_font(int asciiCode, int fontCharX, int fontCharY);
void *TCOD_sys_create_bitmap_for_console(TCOD_console_t console);
void TCOD_sys_save_bitmap(void *bitmap, const char *filename);
void *TCOD_sys_create_bitmap(int width, int height, TCOD_color_t *buf);
void TCOD_sys_delete_bitmap(void *bitmap);
void TCOD_sys_console_to_bitmap(void *bitmap, int console_width, int console_height, char_t *console_buffer, char_t *prev_console_buffer);
void TCOD_sys_set_keyboard_repeat(int initial_delay, int interval);
void *TCOD_sys_get_surface(int width, int height, bool alpha);
void TCOD_sys_save_fps();
void TCOD_sys_restore_fps();

/* switch fullscreen mode */
void TCOD_sys_set_fullscreen(bool fullscreen);
void TCOD_sys_flush(bool render);
TCOD_key_t TCOD_sys_check_for_keypress(int flags);
TCOD_key_t TCOD_sys_wait_for_keypress(bool flush);
bool TCOD_sys_is_key_pressed(TCOD_keycode_t key);
void TCOD_sys_set_window_title(const char *title);
/* close the window */
void TCOD_sys_term();

/* UTF-8 stuff */
#ifndef NO_UNICODE
wchar_t *TCOD_console_vsprint_utf(const wchar_t *fmt, va_list ap);
int TCOD_console_print_utf(TCOD_console_t con,int x,int y, int rw, int rh, TCOD_bkgnd_flag_t flag,
	alignment_t align, wchar_t *msg, bool can_split, bool count_only);
#endif

/* image manipulation */
void *TCOD_sys_load_image(const char *filename);
void TCOD_sys_get_image_size(const void *image, int *w,int *h);
TCOD_color_t TCOD_sys_get_image_pixel(const void *image,int x, int y);
int TCOD_sys_get_image_alpha(const void *image,int x, int y);
bool TCOD_sys_check_magic_number(const char *filename, int size, uint8 *data);

/* TCOD_list nonpublic methods */
void TCOD_list_set_size(TCOD_list_t l, int size);

/* color values */
#define TCOD_BLACK 0,0,0
#define TCOD_DARKER_GREY 31,31,31
#define TCOD_DARK_GREY 63,63,63
#define TCOD_GREY 128,128,128
#define TCOD_LIGHT_GREY 191,191,191
#define TCOD_WHITE 255,255,255

//standard colors
#define TCOD_RED 255,0,0
#define TCOD_ORANGE 255,127,0
#define TCOD_YELLOW 255,255,0
#define TCOD_CHARTREUSE 127,255,0
#define TCOD_GREEN 0,255,0
#define TCOD_SEA 0,255,127
#define TCOD_CYAN 0,255,255
#define TCOD_SKY 0,127,255
#define TCOD_BLUE 0,0,255
#define TCOD_VIOLET 127,0,255
#define TCOD_MAGENTA 255,0,255
#define TCOD_PINK 255,0,127

// dark colors
#define TCOD_DARK_RED 127,0,0
#define TCOD_DARK_ORANGE 127,63,0
#define TCOD_DARK_YELLOW 127,127,0
#define TCOD_DARK_CHARTREUSE 63,127,0
#define TCOD_DARK_GREEN 0,127,0
#define TCOD_DARK_SEA 0,127,63
#define TCOD_DARK_CYAN 0,127,127
#define TCOD_DARK_SKY 0,63,127
#define TCOD_DARK_BLUE 0,0,127
#define TCOD_DARK_VIOLET 63,0,127
#define TCOD_DARK_MAGENTA 127,0,127
#define TCOD_DARK_PINK 127,0,63

// darker colors
#define TCOD_DARKER_RED 63,0,0
#define TCOD_DARKER_ORANGE 63,31,0
#define TCOD_DARKER_YELLOW 63,63,0
#define TCOD_DARKER_CHARTREUSE 31,63,0
#define TCOD_DARKER_GREEN 0,63,0
#define TCOD_DARKER_SEA 0,63,31
#define TCOD_DARKER_CYAN 0,63,63
#define TCOD_DARKER_SKY 0,31,63
#define TCOD_DARKER_BLUE 0,0,63
#define TCOD_DARKER_VIOLET 31,0,63
#define TCOD_DARKER_MAGENTA 63,0,63
#define TCOD_DARKER_PINK 63,0,31

// light colors
#define TCOD_LIGHT_RED 255,127,127
#define TCOD_LIGHT_ORANGE 255,191,127
#define TCOD_LIGHT_YELLOW 255,255,127
#define TCOD_LIGHT_CHARTREUSE 191,255,127
#define TCOD_LIGHT_GREEN 127,255,127
#define TCOD_LIGHT_SEA 127,255,191
#define TCOD_LIGHT_CYAN 127,255,255
#define TCOD_LIGHT_SKY 127,191,255
#define TCOD_LIGHT_BLUE 127,127,255
#define TCOD_LIGHT_VIOLET 191,127,255
#define TCOD_LIGHT_MAGENTA 255,127,255
#define TCOD_LIGHT_PINK 255,127,191

// desaturated colors
#define TCOD_DESATURATED_RED 127,63,63
#define TCOD_DESATURATED_ORANGE 127,95,63
#define TCOD_DESATURATED_YELLOW 127,127,63
#define TCOD_DESATURATED_CHARTREUSE 95,127,63
#define TCOD_DESATURATED_GREEN 63,127,63
#define TCOD_DESATURATED_SEA 63,127,95
#define TCOD_DESATURATED_CYAN 63,127,127
#define TCOD_DESATURATED_SKY 63,95,127
#define TCOD_DESATURATED_BLUE 63,63,127
#define TCOD_DESATURATED_VIOLET 95,63,127
#define TCOD_DESATURATED_MAGENTA 127,63,127
#define TCOD_DESATURATED_PINK 127,63,95

// special
#define TCOD_SILVER 203,203,203
#define TCOD_GOLD 255,255,102


#ifdef __cplusplus
}
#endif
#endif

