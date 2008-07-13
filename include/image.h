#ifndef _TCOD_IMAGE_H
#define _TCOD_IMAGE_H

typedef void *TCOD_image_t;

TCODLIB_API TCOD_image_t TCOD_image_new(int width, int height);
TCODLIB_API TCOD_image_t TCOD_image_from_console(TCOD_console_t console);
TCODLIB_API TCOD_image_t TCOD_image_load(const char *filename);
TCODLIB_API void TCOD_image_clear(TCOD_image_t image, TCOD_color_t color);
TCODLIB_API void TCOD_image_save(TCOD_image_t image, const char *filename);
TCODLIB_API void TCOD_image_get_size(TCOD_image_t image, int *w,int *h);
TCODLIB_API TCOD_color_t TCOD_image_get_pixel(TCOD_image_t image,int x, int y);
TCODLIB_API TCOD_color_t TCOD_image_get_mipmap_pixel(TCOD_image_t image,float x0,float y0, float x1, float y1);
TCODLIB_API void TCOD_image_put_pixel(TCOD_image_t image,int x, int y,TCOD_color_t col);
TCODLIB_API void TCOD_image_blit(TCOD_image_t image, TCOD_console_t console, float x, float y, 
	TCOD_bkgnd_flag_t bkgnd_flag, float scalex, float scaley, float angle);
TCODLIB_API void TCOD_image_blit_rect(TCOD_image_t image, TCOD_console_t console, int x, int y, int w, int h, 
	TCOD_bkgnd_flag_t bkgnd_flag);
TCODLIB_API void TCOD_image_delete(TCOD_image_t image);
TCODLIB_API void TCOD_image_set_key_color(TCOD_image_t image, TCOD_color_t key_color);
TCODLIB_API bool TCOD_image_is_pixel_transparent(TCOD_image_t image, int x, int y);

#endif
