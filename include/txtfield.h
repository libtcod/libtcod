#include "libtcod.h"

#ifndef _TCOD_TEXT_H_
#define _TCOD_TEXT_H_

typedef void * TCOD_text_t;

TCODLIB_API TCOD_text_t TCOD_text_init (int x, int y, int w, int h, int max_chars);
TCODLIB_API void TCOD_text_set_properties (TCOD_text_t txt, int cursor_char, int blink_interval, char * prompt);
TCODLIB_API void TCOD_text_set_colors (TCOD_text_t txt, TCOD_color_t fore, TCOD_color_t back, float back_transparency);
TCODLIB_API bool TCOD_text_update (TCOD_text_t txt, TCOD_key_t key);
TCODLIB_API void TCOD_text_render (TCOD_console_t con, TCOD_text_t txt);
TCODLIB_API char * TCOD_text_get (TCOD_text_t txt);
TCODLIB_API void TCOD_text_reset (TCOD_text_t txt);
TCODLIB_API void TCOD_text_delete (TCOD_text_t txt);

#endif
