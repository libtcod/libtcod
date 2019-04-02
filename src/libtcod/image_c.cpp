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
#include "portability.h"
#ifdef TCOD_IMAGE_SUPPORT
#include "image.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <algorithm>
#include <cmath>

#include "console.h"
#include "libtcod_int.h"
#include "utility.h"

/*
Internal libtcod optimisation, direct colour manipulation in the images primary mipmap.
*/
TCOD_color_t *TCOD_image_get_colors(TCOD_Image* image) {
  return image->mipmaps[0].buf;
}

void TCOD_image_get_key_data(const TCOD_Image* image, bool* has_key_color,
                             TCOD_color_t* key_color)
{
  *has_key_color = image->has_key_color;
  *key_color = image->key_color;
}

void TCOD_image_invalidate_mipmaps(TCOD_Image* image) {
  for (int i = 1; i < image->nb_mipmaps; ++i) {
    image->mipmaps[i].dirty = true;
  }
}

static int TCOD_image_get_mipmap_levels(int width, int height)
{
  int curw = width;
  int curh = height;
  int nb_mipmap = 0;
  while (curw > 0 && curh > 0) {
    ++nb_mipmap;
    curw >>= 1;
    curh >>= 1;
  }
  return nb_mipmap;
}

static void TCOD_image_generate_mip(TCOD_Image* image, int mip) {
  struct TCOD_mipmap_* orig = &image->mipmaps[0];
  struct TCOD_mipmap_* cur = &image->mipmaps[mip];
  if (!cur->buf) {
    cur->buf = static_cast<TCOD_color_t*>(
        calloc(sizeof(TCOD_color_t), cur->width * cur->height)
    );
  }
  cur->dirty = false;
  for (int x = 0; x < cur->width; ++x) {
    for (int y = 0; y < cur->height; ++y) {
      int r = 0;
      int g = 0;
      int b = 0;
      int count=0;
      for (int sx = x << mip; sx < (x + 1) << mip; ++sx) {
        for (int sy = y << mip; sy < (y + 1) << mip; ++sy) {
          int offset = sx + sy * orig->width;
          ++count;
          r += orig->buf[offset].r;
          g += orig->buf[offset].g;
          b += orig->buf[offset].b;
        }
      }
      cur->buf[x + y * cur->width] = {
          static_cast<uint8_t>(r / count),
          static_cast<uint8_t>(g / count),
          static_cast<uint8_t>(b / count),
      };
    }
  }
}
/*
Internal way of copying rendering fg/bg color frame data.
*/
bool TCOD_image_mipmap_copy_internal(const TCOD_Image* img_src,
                                     TCOD_Image* img_dst)
{
  if (!img_src->mipmaps || img_src->sys_img || !img_dst->mipmaps
      || img_dst->sys_img) { // Both internal images.
    return false;
  }
  if (img_src->mipmaps[0].width != img_dst->mipmaps[0].width
      || img_src->mipmaps[0].height != img_dst->mipmaps[0].height) {
    return false;
  }
  // Copy all mipmaps?
  img_dst->mipmaps[0].dirty = img_src->mipmaps[0].dirty;
  memcpy(
      img_dst->mipmaps[0].buf,
      img_src->mipmaps[0].buf,
      (sizeof(TCOD_color_t)
       * (img_src->mipmaps[0].width) * (img_src->mipmaps[0].height))
  );
  for (int i = 1; i < img_src->nb_mipmaps; ++i) {
    img_dst->mipmaps[i].dirty = true;
  }
  return true;
}

static void TCOD_image_init_mipmaps(TCOD_Image* image)
{
  int w, h;
  if (!image->sys_img) { return; }
  TCOD_sys_get_image_size(image->sys_img, &w, &h);
  image->nb_mipmaps = TCOD_image_get_mipmap_levels(w, h);
  image->mipmaps = static_cast<struct TCOD_mipmap_*>(
      calloc(sizeof(struct TCOD_mipmap_), image->nb_mipmaps)
  );
  image->mipmaps[0].buf = static_cast<TCOD_color_t*>(
      calloc(sizeof(TCOD_color_t), w * h)
  );
  for (int x = 0; x < w; ++x) {
    for (int y = 0; y < h; ++y) {
      image->mipmaps[0].buf[x + y * w] =
          TCOD_sys_get_image_pixel(image->sys_img, x, y);
    }
  }
  float fw = w;
  float fh = h;
  for (int i = 0; i < image->nb_mipmaps; ++i) {
    image->mipmaps[i].width = w;
    image->mipmaps[i].height = h;
    image->mipmaps[i].fwidth = fw;
    image->mipmaps[i].fheight = fh;
    image->mipmaps[i].dirty = true;
    w >>= 1;
    h >>= 1;
    fw *= 0.5f;
    fh *= 0.5f;
  }
  image->mipmaps[0].dirty = false;
}

void TCOD_image_clear(TCOD_Image* image, TCOD_color_t color)
{
  if (!image->mipmaps && !image->sys_img) { return; } /* no image data */
  if (!image->mipmaps) {
    TCOD_image_init_mipmaps(image);
  }
  for (int i = 0; i < image->mipmaps[0].width * image->mipmaps[0].height; ++i) {
    image->mipmaps[0].buf[i] = color;
  }
  for (int i = 1; i < image->nb_mipmaps; ++i) {
    image->mipmaps[i].dirty = true;
  }
}

TCOD_Image* TCOD_image_new(int width, int height)
{
  TCOD_Image* ret=static_cast<TCOD_Image*>(calloc(sizeof(TCOD_Image), 1));
  ret->nb_mipmaps = TCOD_image_get_mipmap_levels(width, height);
  ret->mipmaps = static_cast<struct TCOD_mipmap_*>(
      calloc(sizeof(struct TCOD_mipmap_), ret->nb_mipmaps));
  ret->mipmaps[0].buf = static_cast<TCOD_color_t*>(
      calloc(sizeof(TCOD_color_t), width * height));

  for (int i = 0; i < width * height; ++i) {
    ret->mipmaps[0].buf[i] = TCOD_black;
  }
  float fw = width;
  float fh = height;
  for (int i = 0; i < ret->nb_mipmaps; ++i) {
    ret->mipmaps[i].width = width;
    ret->mipmaps[i].height = height;
    ret->mipmaps[i].fwidth = fw;
    ret->mipmaps[i].fheight = fh;
    width >>= 1;
    height >>= 1;
    fw *= 0.5f;
    fh *= 0.5f;
  }
  return ret;
}

TCOD_Image* TCOD_image_load(const char *filename) {
  TCOD_Image* ret = static_cast<TCOD_Image*>(calloc(sizeof(TCOD_Image), 1));
  ret->sys_img = TCOD_sys_load_image(filename);
  return ret;
}

void TCOD_image_get_size(const TCOD_Image* image, int *w,int *h)
{
  if (!image->mipmaps && !image->sys_img) { return; } // no image data
  if (image->mipmaps) {
    *w = image->mipmaps[0].width;
    *h = image->mipmaps[0].height;
  } else {
    TCOD_sys_get_image_size(image->sys_img, w, h);
  }
}

TCOD_color_t TCOD_image_get_pixel(const TCOD_Image* image, int x, int y)
{
  if (!image->mipmaps && !image->sys_img) {
    return TCOD_black; // no image data
  }
  if (image->mipmaps) {
    if (x >= 0 && x < image->mipmaps[0].width
        && y >= 0 && y < image->mipmaps[0].height) {
      return image->mipmaps[0].buf[x + y * image->mipmaps[0].width];
    } else {
      return TCOD_black;
    }
  } else {
    return TCOD_sys_get_image_pixel(image->sys_img, x, y);
  }
}

int TCOD_image_get_alpha(const TCOD_Image* image,int x, int y) {

  if (image->sys_img) {
    return TCOD_sys_get_image_alpha(image->sys_img, x, y);
  }
  return 255;
}

TCOD_color_t TCOD_image_get_mipmap_pixel(
    const TCOD_Image* image, float x0, float y0, float x1, float y1) {
  int cur_size = 1;
  int mip = 0;
  if (!image->mipmaps && !image->sys_img) {
    return TCOD_black; // no image data
  }
  if (!image->mipmaps) TCOD_image_init_mipmaps(const_cast<TCOD_Image*>(image));
  int texel_xsize = static_cast<int>(x1 - x0);
  int texel_ysize = static_cast<int>(y1 - y0);
  int texel_size = texel_xsize < texel_ysize ? texel_ysize : texel_xsize;
  while (mip < image->nb_mipmaps - 1 && cur_size < texel_size) {
    ++mip;
    cur_size <<= 1;
  }
  if (mip > 0) { --mip; }
  int texel_x = static_cast<int>(
      x0 * (image->mipmaps[mip].width) / image->mipmaps[0].fwidth
  );
  int texel_y = static_cast<int>(
      y0 * (image->mipmaps[mip].height) / image->mipmaps[0].fheight
  );
  if (image->mipmaps[mip].buf == NULL || image->mipmaps[mip].dirty) {
    TCOD_image_generate_mip(const_cast<TCOD_Image*>(image), mip);
  }
  if (texel_x < 0 || texel_y < 0
      || texel_x >= image->mipmaps[mip].width
      || texel_y >= image->mipmaps[mip].height) {
    return TCOD_black;
  }
  return image->mipmaps[mip].buf[texel_x
                                 + texel_y * image->mipmaps[mip].width];
}

void TCOD_image_put_pixel(TCOD_Image* image, int x, int y, TCOD_color_t col) {
  if (!image->mipmaps && !image->sys_img) { return; } /* no image data */
  if (!image->mipmaps) {
    TCOD_image_init_mipmaps(image);
  }
  if (x >= 0 && x < image->mipmaps[0].width
      && y >= 0 && y < image->mipmaps[0].height) {
    image->mipmaps[0].buf[x + y * image->mipmaps[0].width] = col;
    for (int mip = 1; mip < image->nb_mipmaps; ++mip) {
      image->mipmaps[mip].dirty = true;
    }
  }
}

void TCOD_image_delete_internal(TCOD_Image* image) {
  if (image->mipmaps) {
    for (int i=0; i < image->nb_mipmaps; ++i) {
      if (image->mipmaps[i].buf) { free(image->mipmaps[i].buf); }
    }
    free(image->mipmaps);
  }
  if (image->sys_img) {
#ifndef TCOD_BARE
    TCOD_sys_delete_bitmap(image->sys_img);
#endif
  }
}

void TCOD_image_delete(TCOD_Image* image) {
  TCOD_image_delete_internal(image);
  free(image);
}

bool TCOD_image_is_pixel_transparent(const TCOD_Image* image, int x, int y) {
  TCOD_color_t col = TCOD_image_get_pixel(image, x, y);
  if (image->has_key_color
      && image->key_color.r == col.r
      && image->key_color.g == col.g
      && image->key_color.b == col.b) {
    return true;
  }
  if (TCOD_image_get_alpha(image, x, y) == 0) { return true; }
  return false;
}

#ifdef TCOD_CONSOLE_SUPPORT

void TCOD_image_blit(
    const TCOD_Image* image, TCOD_console_t console, float x, float y,
    TCOD_bkgnd_flag_t bkgnd_flag, float scalex, float scaley, float angle)
{
  if (scalex == 0.0f || scaley == 0.0f || bkgnd_flag == TCOD_BKGND_NONE) {
    return;
  }
  int width, height;
  TCOD_image_get_size(image, &width, &height);
  float rx_ = x - width * 0.5f;
  float ry_ = y - height * 0.5f;
  if (scalex == 1.0f && scaley == 1.0f && angle == 0.0f
      && rx_ == static_cast<int>(rx_) && ry_ == static_cast<int>(ry_)) {
    /* clip the image */
    int ix = static_cast<int>(x - width * 0.5f);
    int iy = static_cast<int>(y - height * 0.5f);
    int minx = std::max(ix, 0);
    int miny = std::max(iy, 0);
    int maxx = std::min(ix + width, TCOD_console_get_width(console));
    int maxy = std::min(iy + height, TCOD_console_get_height(console));
    int offx = 0;
    int offy = 0;
    if (ix < 0) { offx = -ix; }
    if (iy < 0) { offy = -iy; }
    for (int cx = minx; cx < maxx; ++cx) {
      for (int cy = miny; cy < maxy; ++cy) {
        TCOD_color_t col = TCOD_image_get_pixel(
            image, cx - minx + offx, cy - miny + offy);
        if (!image->has_key_color
            || image->key_color.r != col.r
            || image->key_color.g != col.g
            || image->key_color.b != col.b) {
          TCOD_console_set_char_background(console, cx, cy, col, bkgnd_flag);
        }
      }
    }
  } else {
    float iw = width / 2 * scalex;
    float ih = height / 2 * scaley;
    /* get the coordinates of the image corners in the console */
    float newx_x = std::cos(angle);
    float newx_y = -std::sin(angle);
    float newy_x = newx_y;
    float newy_y = -newx_x;
    // image corners coordinates
    /* 0 = P - w/2 x' +h/2 y' */
    float x0 = x - iw * newx_x + ih * newy_x;
    float y0 = y - iw * newx_y + ih * newy_y;
    /* 1 = P + w/2 x' + h/2 y' */
    float x1 = x + iw * newx_x + ih * newy_x;
    float y1 = y + iw * newx_y + ih * newy_y;
    /* 2 = P + w/2 x' - h/2 y' */
    float x2 = x + iw * newx_x - ih * newy_x;
    float y2 = y + iw * newx_y - ih * newy_y;
    /* 3 = P - w/2 x' - h/2 y' */
    float x3 = x - iw * newx_x - ih * newy_x;
    float y3 = y - iw * newx_y - ih * newy_y;
    /* get the affected rectangular area in the console */
    int rx = std::min(std::min<int>(x0, x1), std::min<int>(x2, x3));
    int ry = std::min(std::min<int>(y0, y1), std::min<int>(y2, y3));
    int rw = std::max(std::max<int>(x0, x1), std::max<int>(x2, x3)) - rx;
    int rh = std::max(std::max<int>(y0, y1), std::max<int>(y2, y3)) - ry;
    /* clip it */
    int minx = std::max(rx, 0);
    int miny = std::max(ry, 0);
    int maxx = std::min(rx + rw, TCOD_console_get_width(console));
    int maxy = std::min(ry + rh, TCOD_console_get_height(console));
    float invscalex = 1.0f / scalex;
    float invscaley = 1.0f / scaley;
    for (int cx = minx; cx < maxx; ++cx) {
      for (int cy = miny; cy < maxy; ++cy) {
        /* map the console pixel to the image world */
        float ix = (iw + (cx - x) * newx_x + (cy - y) * (-newy_x)) * invscalex;
        float iy = (ih + (cx - x) * newx_y - (cy - y) * newy_y) * invscaley;
        TCOD_color_t col = TCOD_image_get_pixel(image, ix, iy);
        if (!image->has_key_color
            || image->key_color.r != col.r
            || image->key_color.g != col.g
            || image->key_color.b != col.b ) {
          if (scalex < 1.0f || scaley < 1.0f) {
            col = TCOD_image_get_mipmap_pixel(image,
                                              ix, iy, ix + 1.0f, iy + 1.0f);
          }
          TCOD_console_set_char_background(console, cx, cy, col, bkgnd_flag);
        }
      }
    }
  }
}

void TCOD_image_blit_rect(
    const TCOD_Image* image, TCOD_console_t console,
    int x, int y, int w, int h, TCOD_bkgnd_flag_t bkgnd_flag)
{
  int width, height;
  TCOD_image_get_size(image, &width, &height);
  if (w == -1) { w = width; }
  if (h == -1) { h = height; }
  if (w <= 0 || h <= 0 || bkgnd_flag == TCOD_BKGND_NONE) { return; }
  float scalex = static_cast<float>(w) / width;
  float scaley = static_cast<float>(h) / height;
  TCOD_image_blit(image, console, x + w * 0.5f, y + h * 0.5f, bkgnd_flag,
                  scalex, scaley, 0.0f);
}

TCOD_Image* TCOD_image_from_console(TCOD_Console* console) {
  TCOD_Image* ret = static_cast<TCOD_Image*>(calloc(sizeof(TCOD_Image), 1));
  ret->sys_img = TCOD_sys_create_bitmap_for_console(console);
  TCOD_image_refresh_console(ret, console);
  return ret;
}

void TCOD_image_refresh_console(TCOD_Image* image, TCOD_console_t console) {
  /* We're copying the state and clearing part of the copy, no need to delete/free. */
  TCOD_sys_console_to_bitmap(
    image->sys_img, TCOD_console_validate_(console), NULL);
}

#endif /* TCOD_CONSOLE_SUPPORT */

void TCOD_image_save(const TCOD_Image* image, const char *filename)
{
  SDL_Surface* bitmap = nullptr;
  bool must_free = false;
  if (image->sys_img) {
    bitmap = image->sys_img;
  } else if (image->mipmaps){
    bitmap = TCOD_sys_create_bitmap(
        image->mipmaps[0].width,
        image->mipmaps[0].height,
        image->mipmaps[0].buf
    );
    must_free=true;
  }
  if (bitmap) {
    TCOD_sys_save_bitmap(bitmap, filename);
    if (must_free) {
      TCOD_sys_delete_bitmap(bitmap);
    }
  }
}

void TCOD_image_set_key_color(TCOD_Image* image, TCOD_color_t key_color)
{
  image->has_key_color = true;
  image->key_color = key_color;
}

void TCOD_image_invert(TCOD_Image* image)
{
  if (!image->mipmaps && !image->sys_img) { return; } /* no image data */
  if (!image->mipmaps) {
    TCOD_image_init_mipmaps(image);
  }
  int width, height;
  TCOD_image_get_size(image, &width, &height);
  for (int i = 0; i < width * height; ++i) {
    TCOD_color_t col = image->mipmaps[0].buf[i];
    col.r = 255 - col.r;
    col.g = 255 - col.g;
    col.b = 255 - col.b;
    image->mipmaps[0].buf[i] = col;
  }
  for (int mip = 1; mip < image->nb_mipmaps; ++mip) {
    image->mipmaps[mip].dirty = true;
  }
}

void TCOD_image_hflip(TCOD_Image* image)
{
  int width, height;
  TCOD_image_get_size(image, &width, &height);
  for (int py = 0; py < height; ++py) {
    for (int px = 0; px < width / 2; ++px) {
      TCOD_color_t col1 = TCOD_image_get_pixel(image, px, py);
      TCOD_color_t col2 = TCOD_image_get_pixel(image, width - 1 - px, py);
      TCOD_image_put_pixel(image, px, py, col2);
      TCOD_image_put_pixel(image, width - 1 - px, py, col1);
    }
  }
}

void TCOD_image_vflip(TCOD_Image* image)
{
  int width, height;
  TCOD_image_get_size(image, &width, &height);
  for (int px = 0; px < width; ++px) {
    for (int py = 0; py < height / 2; ++py) {
      TCOD_color_t col1 = TCOD_image_get_pixel(image, px, py);
      TCOD_color_t col2 = TCOD_image_get_pixel(image, px, height - 1 - py);
      TCOD_image_put_pixel(image, px, py, col2);
      TCOD_image_put_pixel(image, px, height - 1 - py, col1);
    }
  }
}

void TCOD_image_rotate90(TCOD_Image* image, int numRotations)
{
  numRotations = numRotations % 4;
  if (numRotations == 0) { return; }
  if (numRotations < 0) numRotations += 4;
  int width, height;
  TCOD_image_get_size(image, &width, &height);
  if (numRotations == 1) {
    /* rotate 90 degrees */
    TCOD_Image* img2 = TCOD_image_new(height, width);
    for (int px = 0; px < width; ++px) {
      for (int py = 0; py < height; ++py) {
        TCOD_color_t col1 = TCOD_image_get_pixel(image, px, py);
        TCOD_image_put_pixel(img2, height - 1 - py, px, col1);
      }
    }
    TCOD_image_delete_internal(image);
    /* update image with the new image content */
    image->mipmaps = img2->mipmaps;
    image->sys_img = NULL;
    image->nb_mipmaps = img2->nb_mipmaps;
    free(img2);
  } else if (numRotations == 2) {
    /* rotate 180 degrees */
    int maxy = height / 2 + ((height & 1) == 1 ? 1 : 0 );
    for (int px = 0; px < width; ++px) {
      for (int py = 0; py < maxy; ++py) {
        if (py != height - 1 - py || px < width / 2) {
          TCOD_color_t col1 = TCOD_image_get_pixel(image, px, py);
          TCOD_color_t col2 = TCOD_image_get_pixel(
              image, width - 1 - px, height - 1 - py);
          TCOD_image_put_pixel(image, px, py, col2);
          TCOD_image_put_pixel(image, width - 1 - px, height - 1 - py, col1);
        }
      }
    }
  } else if (numRotations == 3) {
    /* rotate 270 degrees */
    TCOD_Image* newImg = TCOD_image_new(height, width);
    for (int px = 0; px < width; ++px) {
      for (int py = 0; py < height; ++py) {
        TCOD_color_t col1 = TCOD_image_get_pixel(image, px, py);
        TCOD_image_put_pixel(newImg, py, width - 1 - px, col1);
      }
    }
    TCOD_image_delete_internal(image);
    /* update image with the new image content */
    image->mipmaps = newImg->mipmaps;
    image->sys_img = NULL;
    image->nb_mipmaps = newImg->nb_mipmaps;
    free(newImg);
  }
}

void TCOD_image_scale(TCOD_Image* image, int neww, int newh)
{
  int width, height;
  TCOD_image_get_size(image, &width, &height);
  if ( neww == width && newh == height ) { return; }
  if ( neww == 0 || newh == 0 ) { return; }
  TCOD_Image* newimg = TCOD_image_new(neww, newh);

  if (neww < width && newh < height) {
    /* scale down image, using supersampling */
    for (int py = 0; py < newh; ++py) {
      float y0 = static_cast<float>(py) * height / newh;
      float y0floor = std::floor(y0);
      float y0weight = 1.0f - (y0 - y0floor);
      int iy0 = static_cast<int>(y0floor);

      float y1 = static_cast<float>(py + 1) * height / newh;
      float y1floor = std::floor(y1 - 0.00001f);
      float y1weight = (y1 - y1floor);
      int iy1 = static_cast<int>(y1floor);

      for (int px = 0; px < neww; ++px) {
        float x0 = static_cast<float>(px) * width / neww;
        float x0floor = std::floor(x0);
        float x0weight = 1.0f - (x0 - x0floor);
        int ix0 = static_cast<int>(x0floor);

        float x1 = static_cast<float>(px+1) * width / neww;
        float x1floor = std::floor(x1 - 0.00001f);
        float x1weight = (x1 - x1floor);
        int ix1 = static_cast<int>(x1floor);

        float r=0,g=0,b=0,sumweight=0.0f;
        /* left & right fractional edges */
        for (int srcy = static_cast<int>(y0 + 1);
             srcy < static_cast<int>(y1);
             ++srcy) {
          TCOD_color_t col_left = TCOD_image_get_pixel(image, ix0, srcy);
          TCOD_color_t col_right = TCOD_image_get_pixel(image, ix1, srcy);
          r += col_left.r * x0weight + col_right.r * x1weight;
          g += col_left.g * x0weight + col_right.g * x1weight;
          b += col_left.b * x0weight + col_right.b * x1weight;
          sumweight += x0weight + x1weight;
        }
        /* top & bottom fractional edges */
        for (int srcx = static_cast<int>(x0 + 1);
             srcx < static_cast<int>(x1);
             ++srcx) {
          TCOD_color_t col_top = TCOD_image_get_pixel(image, srcx, iy0);
          TCOD_color_t col_bottom = TCOD_image_get_pixel(image, srcx, iy1);
          r += col_top.r * y0weight + col_bottom.r * y1weight;
          g += col_top.g * y0weight + col_bottom.g * y1weight;
          b += col_top.b * y0weight + col_bottom.b * y1weight;
          sumweight += y0weight + y1weight;
        }
        /* center */
        for (int srcy = static_cast<int>(y0 + 1);
             srcy < static_cast<int>(y1);
             ++srcy) {
          for (int srcx = static_cast<int>(x0 + 1);
               srcx < static_cast<int>(x1);
               ++srcx) {
            TCOD_color_t sample = TCOD_image_get_pixel(image, srcx, srcy);
            r += sample.r;
            g += sample.g;
            b += sample.b;
            sumweight += 1.0f;
          }
        }
        /* corners */
        TCOD_color_t col = TCOD_image_get_pixel(image, ix0, iy0);
        r += col.r * (x0weight * y0weight);
        g += col.g * (x0weight * y0weight);
        b += col.b * (x0weight * y0weight);
        sumweight += x0weight * y0weight;
        col = TCOD_image_get_pixel(image, ix0, iy1);
        r += col.r * (x0weight * y1weight);
        g += col.g * (x0weight * y1weight);
        b += col.b * (x0weight * y1weight);
        sumweight += x0weight * y1weight;
        col = TCOD_image_get_pixel(image, ix1, iy1);
        r += col.r * (x1weight * y1weight);
        g += col.g * (x1weight * y1weight);
        b += col.b * (x1weight * y1weight);
        sumweight += x1weight * y1weight;
        col = TCOD_image_get_pixel(image, ix1, iy0);
        r += col.r * (x1weight * y0weight);
        g += col.g * (x1weight * y0weight);
        b += col.b * (x1weight * y0weight);
        sumweight += x1weight * y0weight;
        sumweight = 1.0f / sumweight;
        r = r * sumweight + 0.5f;
        g = g * sumweight + 0.5f;
        b = b * sumweight + 0.5f;
        col.r = r;
        col.g = g;
        col.b = b;
        TCOD_image_put_pixel(newimg, px, py, col);
      }
    }
  } else {
    /* scale up image, using nearest neightbor */
    for (int py = 0; py < newh; ++py) {
      int srcy = py * height / newh;
      for (int px = 0; px < neww; ++px) {
        int srcx = px * width / neww;
        TCOD_image_put_pixel(
            newimg, px, py, TCOD_image_get_pixel(image, srcx, srcy)
        );
      }
    }
  }
  /* destroy old image */
  TCOD_image_delete_internal(image);
  /* update image with the new image content */
  image->mipmaps = newimg->mipmaps;
  image->sys_img = NULL;
  image->nb_mipmaps = newimg->nb_mipmaps;
  free(newimg);
}


/* distance between two colors */
int rgbdist(const TCOD_color_t *c1, const TCOD_color_t *c2)
{
  int dr = static_cast<int>(c1->r) - c2->r;
  int dg = static_cast<int>(c1->g) - c2->g;
  int db = static_cast<int>(c1->b) - c2->b;
  return dr * dr + dg * dg + db * db;
}

void getPattern(const TCOD_color_t desired[4], TCOD_color_t palette[2],
                int* nbCols, int* ascii) {
  /* adapted from Jeff Lait's code posted on r.g.r.d */
  int flag = 0;
  /*
    pixels have following flag values :
      X 1
      2 4
    flag indicates which pixels uses foreground color (palette[1])
  */
  static int flagToAscii[8] = {
      0,
      TCOD_CHAR_SUBP_NE,
      TCOD_CHAR_SUBP_SW,
      -TCOD_CHAR_SUBP_DIAG,
      TCOD_CHAR_SUBP_SE,
      TCOD_CHAR_SUBP_E,
      -TCOD_CHAR_SUBP_N,
      -TCOD_CHAR_SUBP_NW
  };
  int weight[2] = { 0, 0 };
  int i;

  /* First colour trivial. */
  palette[0] = desired[0];

  /* Ignore all duplicates... */
  for (i = 1; i < 4; i++) {
    if (desired[i] != palette[0]) { break; }
  }

  /* All the same. */
  if (i == 4) {
    *nbCols = 1;
    return;
  }
  weight[0] = i;

  /* Found a second colour... */
  palette[1] = desired[i];
  weight[1] = 1;
  flag |= 1 << (i - 1);
  *nbCols = 2;
  /* remaining colours */
  ++i;
  while (i < 4) {
    if (desired[i] == palette[0]) {
      ++weight[0];
    } else if (desired[i] == palette[1])  {
      flag |= 1 << (i - 1);
      ++weight[1];
    } else {
      /* Bah, too many colours, */
      /* merge the two nearest */
      int dist0i = rgbdist(&desired[i], &palette[0]);
      int dist1i = rgbdist(&desired[i], &palette[1]);
      int dist01 = rgbdist(&palette[0], &palette[1]);
      if (dist0i < dist1i) {
        if (dist0i <= dist01) {
          /* merge 0 and i */
          palette[0] = TCOD_color_lerp(
              desired[i], palette[0], weight[0] / (1.0f + weight[0])
          );
          ++weight[0];
        } else {
          /* merge 0 and 1 */
          palette[0] = TCOD_color_lerp(
              palette[0],
              palette[1],
              static_cast<float>(weight[1]) / (weight[0] + weight[1])
          );
          ++weight[0];
          palette[1] = desired[i];
          flag = 1 << (i - 1);
        }
      } else {
        if (dist1i <= dist01) {
          /* merge 1 and i */
          palette[1] = TCOD_color_lerp(
              desired[i], palette[1], weight[1] / (1.0f + weight[1])
          );
          ++weight[1];
          flag |= 1 << (i - 1);
        } else {
          /* merge 0 and 1 */
          palette[0] = TCOD_color_lerp(
              palette[0],
              palette[1],
              static_cast<float>(weight[1]) / (weight[0] + weight[1])
          );
          ++weight[0];
          palette[1] = desired[i];
          flag = 1 << (i - 1);
        }
      }
    }
    ++i;
  }
  *ascii = flagToAscii[flag];
}
#ifdef TCOD_CONSOLE_SUPPORT

void TCOD_image_blit_2x(const TCOD_Image* image, TCOD_Console* con,
      int dx, int dy, int sx, int sy, int w, int h) {
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(image != NULL && con != NULL) { return; }
  TCOD_color_t grid[4];
  TCOD_color_t cols[2];

  int width, height;
  TCOD_image_get_size(image, &width, &height);
  if (w == -1) { w = width; }
  if (h == -1) { h = height; }

  /* check that the sx,sy/w,h rectangle is inside the image */
  TCOD_ASSERT(sx >= 0 && sy >= 0 && sx+w <= width && sy+h <= height);
  TCOD_IFNOT(w > 0 && h > 0) { return; }

  sx = std::max(0, sx);
  sy = std::max(0, sy);
  w = std::min(w, width - sx);
  h = std::min(h, height - sy);

  int maxx = dx + w / 2 <= con->w ? w : (con->w - dx) * 2;
  int maxy = dy + h / 2 <= con->h ? h : (con->h - dy) * 2;
  /* check that the image is not blitted outside the console */
  TCOD_IFNOT(dx + maxx / 2 >= 0 && dy + maxy / 2 >= 0
             && dx < con->w && dy < con->h) { return; }
  maxx += sx;
  maxy += sy;

  for (int cx = sx; cx < maxx; cx += 2) {
    for (int cy = sy; cy < maxy; cy += 2) {
      /* get the 2x2 super pixel colors from the image */
      int conx = dx + (cx - sx) / 2;
      int cony = dy + (cy - sy) / 2;
      TCOD_color_t consoleBack =
          TCOD_console_get_char_background(con, conx, cony);
      grid[0] = TCOD_image_get_pixel(image, cx, cy);
      if (image->has_key_color && grid[0] == image->key_color) {
        grid[0] = consoleBack;
      }
      if (cx < maxx - 1) {
        grid[1] = TCOD_image_get_pixel(image, cx + 1, cy);
        if (image->has_key_color && grid[1] == image->key_color) {
          grid[1] = consoleBack;
        }
      } else {
        grid[1] = consoleBack;
      }
      if (cy < maxy-1) {
        grid[2] = TCOD_image_get_pixel(image, cx, cy + 1);
        if (image->has_key_color && grid[2] == image->key_color) {
          grid[2] = consoleBack;
        }
      } else {
        grid[2] = consoleBack;
      }
      if (cx < maxx-1 && cy < maxy-1) {
        grid[3] = TCOD_image_get_pixel(image, cx + 1, cy + 1);
        if (image->has_key_color && grid[3] == image->key_color) {
          grid[3] = consoleBack;
        }
      } else {
        grid[3] = consoleBack;
      }
      /* analyse color, posterize, get pattern */
      int nbCols;
      int ascii;
      getPattern(grid, cols, &nbCols, &ascii);
      if (nbCols == 1) {
        /* single color */
        TCOD_console_set_char_background(
            con, conx, cony, cols[0], TCOD_BKGND_SET);
        TCOD_console_set_char(con, conx, cony, ' ');
      } else if (ascii >= 0) {
        TCOD_console_set_char_background(con, conx, cony, cols[0],
                                         TCOD_BKGND_SET);
        TCOD_console_set_char_foreground(con, conx, cony, cols[1]);
        TCOD_console_set_char(con, conx, cony, ascii);
      } else {
        /* negative ascii code means we need to invert back/fore colors */
        TCOD_console_set_char_background(con, conx, cony, cols[1],
                                         TCOD_BKGND_SET);
        TCOD_console_set_char_foreground(con, conx, cony, cols[0]);
        TCOD_console_set_char(con, conx, cony, -ascii);
      }
    }
  }
}
#endif /* TCOD_CONSOLE_SUPPORT */
#endif /* TCOD_IMAGE_SUPPORT */
