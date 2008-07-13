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

extern TCODLIB_API TCOD_color_t TCOD_black;
extern TCODLIB_API TCOD_color_t TCOD_dark_grey;
extern TCODLIB_API TCOD_color_t TCOD_grey;
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
