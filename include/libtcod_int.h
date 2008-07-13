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
	TCOD_color_t fore;	/* foreground color */
	TCOD_color_t back;	/* background color */
	uint8 dirt;	/* cell modified since last flush ? */
} char_t;

/* TCODConsole non public data */
typedef struct {
	char_t *buf;
	char_t *oldbuf;
	uint8 fade;
	bool windowed;
	bool fullscreen;
	/* foreground (text) and background colors */
	TCOD_color_t fore,back;
	/* console width and height (in characters, each character is 8x8 pixels) */
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
bool TCOD_sys_init(int w,int h, char_t *buf, bool fullscreen);
void TCOD_sys_set_custom_font(const char *font_name,int cw, int ch, int nbcw, int nbch,bool row,TCOD_color_t key_color);
void *TCOD_sys_create_bitmap_for_console(TCOD_console_t console);
void TCOD_sys_save_bitmap(void *bitmap, const char *filename);
void *TCOD_sys_create_bitmap(int width, int height, TCOD_color_t *buf);
void TCOD_sys_console_to_bitmap(void *bitmap, int console_width, int console_height, char_t *console_buffer);
void TCOD_sys_get_char_size(int *w, int *h);
void TCOD_sys_set_keyboard_repeat(int initial_delay, int interval);
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

#ifdef __cplusplus
}
#endif
#endif

