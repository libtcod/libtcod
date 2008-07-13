#ifndef _TCOD_CONSOLE_H
#define _TCOD_CONSOLE_H

typedef enum {
	TCODK_NONE,
	TCODK_ESCAPE,
	TCODK_BACKSPACE,
	TCODK_TAB,
	TCODK_ENTER,
	TCODK_SHIFT,
	TCODK_CONTROL,
	TCODK_ALT,
	TCODK_PAUSE,
	TCODK_CAPSLOCK,
	TCODK_PAGEUP,
	TCODK_PAGEDOWN,
	TCODK_END,
	TCODK_HOME,
	TCODK_UP,
	TCODK_LEFT,
	TCODK_RIGHT,
	TCODK_DOWN,
	TCODK_PRINTSCREEN,
	TCODK_INSERT,
	TCODK_DELETE,
	TCODK_LWIN,
	TCODK_RWIN,
	TCODK_APPS,
	TCODK_0,
	TCODK_1,
	TCODK_2,
	TCODK_3,
	TCODK_4,
	TCODK_5,
	TCODK_6,
	TCODK_7,
	TCODK_8,
	TCODK_9,
	TCODK_KP0,
	TCODK_KP1,
	TCODK_KP2,
	TCODK_KP3,
	TCODK_KP4,
	TCODK_KP5,
	TCODK_KP6,
	TCODK_KP7,
	TCODK_KP8,
	TCODK_KP9,
	TCODK_KPADD,
	TCODK_KPSUB,
	TCODK_KPDIV,
	TCODK_KPMUL,
	TCODK_KPDEC,
	TCODK_KPENTER,
	TCODK_F1,
	TCODK_F2,
	TCODK_F3,
	TCODK_F4,
	TCODK_F5,
	TCODK_F6,
	TCODK_F7,
	TCODK_F8,
	TCODK_F9,
	TCODK_F10,
	TCODK_F11,
	TCODK_F12,
	TCODK_NUMLOCK,
	TCODK_SCROLLLOCK,
	TCODK_SPACE,
	TCODK_CHAR
} TCOD_keycode_t;

/* key data : special code or character */
typedef struct {
	TCOD_keycode_t vk; /*  key code */
	char c; /* character if vk == TCODK_CHAR else 0 */
	unsigned pressed : 1; /* does this correspond to a key press or key release event ? */
	unsigned lalt : 1;
	unsigned lctrl : 1;
	unsigned ralt : 1;
	unsigned rctrl : 1;
	unsigned shift : 1;
} TCOD_key_t;

enum {
	TCOD_CHAR_HLINE=196,
	TCOD_CHAR_VLINE=179,
	TCOD_CHAR_NE=191, 
	TCOD_CHAR_NW=218, 
	TCOD_CHAR_SE=217, 
	TCOD_CHAR_SW=192,
	TCOD_CHAR_DHLINE=205,
	TCOD_CHAR_DVLINE=186,
	TCOD_CHAR_DNE=187, 
	TCOD_CHAR_DNW=201, 
	TCOD_CHAR_DSE=188, 
	TCOD_CHAR_DSW=200,
	TCOD_CHAR_TEEW=180, 
	TCOD_CHAR_TEEE=195, 
	TCOD_CHAR_TEEN=193,
	TCOD_CHAR_TEES=194,
	TCOD_CHAR_DTEEW=181, 
	TCOD_CHAR_DTEEE=198, 
	TCOD_CHAR_DTEEN=208,
	TCOD_CHAR_DTEES=210,
	TCOD_CHAR_CHECKER=178, 
	TCOD_CHAR_BLOCK=219, 
	TCOD_CHAR_BLOCK1=178, 
	TCOD_CHAR_BLOCK2=177, 
	TCOD_CHAR_BLOCK3=176, 
	TCOD_CHAR_BLOCK_B=220, 
	TCOD_CHAR_BLOCK_T=223,
	TCOD_CHAR_DS_CROSSH=216, 
	TCOD_CHAR_DS_CROSSV=215,
	TCOD_CHAR_CROSS=197,	
	TCOD_CHAR_LIGHT=15, 
	TCOD_CHAR_TREE=5,
	TCOD_CHAR_ARROW_N=24, 
	TCOD_CHAR_ARROW_S=25, 
	TCOD_CHAR_ARROW_E=26, 
	TCOD_CHAR_ARROW_W=27,
};

typedef enum { 
	TCOD_COLCTRL_1 = 1, 
	TCOD_COLCTRL_2, 
	TCOD_COLCTRL_3, 
	TCOD_COLCTRL_4, 
	TCOD_COLCTRL_5,
	TCOD_COLCTRL_NUMBER=5, 
	TCOD_COLCTRL_FORE_RGB,
	TCOD_COLCTRL_BACK_RGB,
	TCOD_COLCTRL_STOP 
} TCOD_colctrl_t;

typedef enum {
	TCOD_BKGND_NONE,
	TCOD_BKGND_SET,
	TCOD_BKGND_MULTIPLY,
	TCOD_BKGND_LIGHTEN,
	TCOD_BKGND_DARKEN,
	TCOD_BKGND_SCREEN,
	TCOD_BKGND_COLOR_DODGE,
	TCOD_BKGND_COLOR_BURN,
	TCOD_BKGND_ADD,
	TCOD_BKGND_ADDA,
	TCOD_BKGND_BURN,
	TCOD_BKGND_OVERLAY,
	TCOD_BKGND_ALPH
} TCOD_bkgnd_flag_t;

#define TCOD_BKGND_ALPHA(alpha) ((TCOD_bkgnd_flag_t)(TCOD_BKGND_ALPH|(((uint8)(alpha*255))<<8)))
#define TCOD_BKGND_ADDALPHA(alpha) ((TCOD_bkgnd_flag_t)(TCOD_BKGND_ADDA|(((uint8)(alpha*255))<<8)))

enum {
	TCOD_KEY_PRESSED=1,
	TCOD_KEY_RELEASED=2,
};

typedef void * TCOD_console_t;

TCODLIB_API void TCOD_console_init_root(int w, int h, const char * title, bool fullscreen);
TCODLIB_API void TCOD_console_set_custom_font(const char *fontFile,int char_width, int char_height, int nb_char_horiz, int nb_char_vertic, bool chars_by_row, TCOD_color_t key_color);
TCODLIB_API void TCOD_console_set_window_title(const char *title);
TCODLIB_API void TCOD_console_set_fullscreen(bool fullscreen);
TCODLIB_API bool TCOD_console_is_fullscreen();
TCODLIB_API bool TCOD_console_is_window_closed();

TCODLIB_API void TCOD_console_set_background_color(TCOD_console_t con,TCOD_color_t col);
TCODLIB_API void TCOD_console_set_foreground_color(TCOD_console_t con,TCOD_color_t col);
TCODLIB_API void TCOD_console_clear(TCOD_console_t con);
TCODLIB_API void TCOD_console_set_back(TCOD_console_t con,int x, int y, TCOD_color_t col, TCOD_bkgnd_flag_t flag);
TCODLIB_API void TCOD_console_set_fore(TCOD_console_t con,int x, int y, TCOD_color_t col);
TCODLIB_API void TCOD_console_set_char(TCOD_console_t con,int x, int y, int c);
TCODLIB_API void TCOD_console_put_char(TCOD_console_t con,int x, int y, int c, TCOD_bkgnd_flag_t flag);

TCODLIB_API void TCOD_console_print_left(TCOD_console_t con,int x, int y, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
TCODLIB_API void TCOD_console_print_right(TCOD_console_t con,int x, int y, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
TCODLIB_API void TCOD_console_print_center(TCOD_console_t con,int x, int y, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
TCODLIB_API int TCOD_console_print_left_rect(TCOD_console_t con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
TCODLIB_API int TCOD_console_print_right_rect(TCOD_console_t con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
TCODLIB_API int TCOD_console_print_center_rect(TCOD_console_t con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 

TCODLIB_API void TCOD_console_rect(TCOD_console_t con,int x, int y, int w, int h, bool clear, TCOD_bkgnd_flag_t flag);
TCODLIB_API void TCOD_console_hline(TCOD_console_t con,int x,int y, int l, TCOD_bkgnd_flag_t flag);
TCODLIB_API void TCOD_console_vline(TCOD_console_t con,int x,int y, int l, TCOD_bkgnd_flag_t flag);
TCODLIB_API void TCOD_console_print_frame(TCOD_console_t con,int x,int y,int w,int h, bool empty, const char *fmt, ...);

TCODLIB_API TCOD_color_t TCOD_console_get_background_color(TCOD_console_t con);
TCODLIB_API TCOD_color_t TCOD_console_get_foreground_color(TCOD_console_t con);
TCODLIB_API TCOD_color_t TCOD_console_get_back(TCOD_console_t con,int x, int y);
TCODLIB_API TCOD_color_t TCOD_console_get_fore(TCOD_console_t con,int x, int y);
TCODLIB_API int TCOD_console_get_char(TCOD_console_t con,int x, int y);

TCODLIB_API void TCOD_console_set_fade(uint8 val, TCOD_color_t fade);
TCODLIB_API uint8 TCOD_console_get_fade();
TCODLIB_API TCOD_color_t TCOD_console_get_fading_color();

TCODLIB_API void TCOD_console_flush();

TCODLIB_API void TCOD_console_set_color_control(TCOD_colctrl_t con, TCOD_color_t fore, TCOD_color_t back);

TCODLIB_API TCOD_key_t TCOD_console_check_for_keypress(int flags);
TCODLIB_API TCOD_key_t TCOD_console_wait_for_keypress(bool flush);
TCODLIB_API void TCOD_console_set_keyboard_repeat(int initial_delay, int interval);
TCODLIB_API void TCOD_console_disable_keyboard_repeat();
TCODLIB_API bool TCOD_console_is_key_pressed(TCOD_keycode_t key);

TCODLIB_API TCOD_console_t TCOD_console_new(int w, int h);
TCODLIB_API int TCOD_console_get_width(TCOD_console_t con);
TCODLIB_API int TCOD_console_get_height(TCOD_console_t con);
TCODLIB_API void TCOD_console_blit(TCOD_console_t src,int xSrc, int ySrc, int wSrc, int hSrc, TCOD_console_t dst, int xDst, int yDst, int fade);
TCODLIB_API void TCOD_console_delete(TCOD_console_t console);

#endif
