/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _TCOD_IMAGE_H
#define _TCOD_IMAGE_H

#include "portability.h"
#include "color.h"

#ifdef TCOD_IMAGE_SUPPORT

#include "console_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct TCOD_mipmap_{
  int width, height;
  float fwidth, fheight;
  TCOD_color_t* buf;
  bool dirty;
};

typedef struct TCOD_Image {
  struct SDL_Surface* sys_img;
  int nb_mipmaps;
  struct TCOD_mipmap_* mipmaps;
  TCOD_color_t key_color;
  bool has_key_color;
} TCOD_Image;

typedef TCOD_Image *TCOD_image_t;

TCODLIB_API TCOD_Image* TCOD_image_new(int width, int height);
#ifdef TCOD_CONSOLE_SUPPORT
TCODLIB_API TCOD_Image* TCOD_image_from_console(TCOD_console_t console);
TCODLIB_API void TCOD_image_refresh_console(TCOD_Image* image, TCOD_console_t console);
#endif
TCODLIB_API TCOD_Image* TCOD_image_load(const char *filename);
TCODLIB_API void TCOD_image_clear(TCOD_Image* image, TCOD_color_t color);
TCODLIB_API void TCOD_image_invert(TCOD_Image* image);
TCODLIB_API void TCOD_image_hflip(TCOD_Image* image);
TCODLIB_API void TCOD_image_rotate90(TCOD_Image* image, int numRotations);
TCODLIB_API void TCOD_image_vflip(TCOD_Image* image);
TCODLIB_API void TCOD_image_scale(TCOD_Image* image, int neww, int newh);
TCODLIB_API void TCOD_image_save(const TCOD_Image* image, const char *filename);
TCODLIB_API void TCOD_image_get_size(const TCOD_Image* image, int *w,int *h);
TCODLIB_API TCOD_color_t TCOD_image_get_pixel(const TCOD_Image* image,
                                              int x, int y);
TCODLIB_API int TCOD_image_get_alpha(const TCOD_Image* image, int x, int y);
TCODLIB_API TCOD_color_t TCOD_image_get_mipmap_pixel(
    const TCOD_Image* image, float x0, float y0, float x1, float y1);
TCODLIB_API void TCOD_image_put_pixel(
    TCOD_Image* image, int x, int y, TCOD_color_t col);
#ifdef TCOD_CONSOLE_SUPPORT
TCODLIB_API void TCOD_image_blit(
    const TCOD_Image* image, TCOD_console_t console, float x, float y,
    TCOD_bkgnd_flag_t bkgnd_flag, float scalex, float scaley, float angle);
TCODLIB_API void TCOD_image_blit_rect(
    const TCOD_Image* image, TCOD_console_t console,
    int x, int y, int w, int h, TCOD_bkgnd_flag_t bkgnd_flag);
TCODLIB_API void TCOD_image_blit_2x(
    const TCOD_Image* image, TCOD_console_t dest,
    int dx, int dy, int sx, int sy, int w, int h);
#endif
TCODLIB_API void TCOD_image_delete(TCOD_Image* image);
TCODLIB_API void TCOD_image_set_key_color(TCOD_Image* image,
                                          TCOD_color_t key_color);
TCODLIB_API bool TCOD_image_is_pixel_transparent(
    const TCOD_Image* image, int x, int y);

#ifdef __cplusplus
}
#endif

#endif /* TCOD_IMAGE_SUPPORT */

#endif /* _TCOD_IMAGE_H */
