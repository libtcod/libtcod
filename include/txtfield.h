#ifndef _TCOD_TEXT_H_
#define _TCOD_TEXT_H_

#include "libtcod.h"

typedef void * TCOD_text_t;

TCOD_text_t TCOD_text_init (int x, int y, int w, int h, int max_chars,
                            int cursor_char, int blink_interval, char * prompt,
                            TCOD_color_t fore, TCOD_color_t back, float back_transparency);
bool TCOD_text_update (TCOD_text_t txt, TCOD_key_t key);
void TCOD_text_render (TCOD_console_t con, TCOD_text_t txt);
char * TCOD_text_get (TCOD_text_t txt);
void TCOD_text_reset (TCOD_text_t txt);
void TCOD_text_delete (TCOD_text_t txt);

#endif
