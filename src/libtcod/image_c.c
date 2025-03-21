/* BSD 3-Clause License
 *
 * Copyright © 2008-2025, Jice and the libtcod contributors.
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
#ifndef NO_SDL
#include <SDL3/SDL.h>
#endif  // NO_SDL
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "console.h"
#include "image.h"
#include "libtcod_int.h"
#include "portability.h"
#include "utility.h"

static void TCOD_image_invalidate_mipmaps(TCOD_Image* image) {
  if (!image) {
    return;
  }
  for (int i = 1; i < image->nb_mipmaps; ++i) {
    image->mipmaps[i].dirty = true;
  }
}

/**
    Return true if `x` and `y` are in the bounds of `image`.
 */
static bool TCOD_image_in_bounds(const TCOD_Image* image, int x, int y) {
  if (!image) return false;
  return (0 <= x && 0 <= y && x < image->mipmaps[0].width && y < image->mipmaps[0].height);
}

static int TCOD_image_get_mipmap_levels(int width, int height) {
  int cur_w = width;
  int cur_h = height;
  int nb_mipmap = 0;
  while (cur_w > 0 && cur_h > 0) {
    ++nb_mipmap;
    cur_w >>= 1;
    cur_h >>= 1;
  }
  return nb_mipmap;
}

static void TCOD_image_generate_mip(TCOD_Image* image, int mip) {
  if (!image) {
    return;
  }
  struct TCOD_mipmap_* orig = &image->mipmaps[0];
  struct TCOD_mipmap_* cur = &image->mipmaps[mip];
  if (!cur->buf) {
    cur->buf = malloc(sizeof(*cur->buf) * cur->width * cur->height);
  }
  cur->dirty = false;
  for (int x = 0; x < cur->width; ++x) {
    for (int y = 0; y < cur->height; ++y) {
      int r = 0;
      int g = 0;
      int b = 0;
      int count = 0;
      for (int sx = x << mip; sx < (x + 1) << mip; ++sx) {
        for (int sy = y << mip; sy < (y + 1) << mip; ++sy) {
          int offset = sx + sy * orig->width;
          ++count;
          r += orig->buf[offset].r;
          g += orig->buf[offset].g;
          b += orig->buf[offset].b;
        }
      }
      cur->buf[x + y * cur->width] = (struct TCOD_ColorRGB){
          (uint8_t)(r / count),
          (uint8_t)(g / count),
          (uint8_t)(b / count),
      };
    }
  }
}

void TCOD_image_clear(TCOD_Image* image, TCOD_color_t color) {
  if (!image) {
    return;
  }
  for (int i = 0; i < image->mipmaps[0].width * image->mipmaps[0].height; ++i) {
    image->mipmaps[0].buf[i] = color;
  }
  TCOD_image_invalidate_mipmaps(image);
}

TCOD_Image* TCOD_image_new(int width, int height) {
  TCOD_Image* ret = calloc(1, sizeof(*ret));
  if (!ret) {
    return NULL;
  }
  ret->nb_mipmaps = TCOD_image_get_mipmap_levels(width, height);
  ret->mipmaps = calloc(ret->nb_mipmaps, sizeof(*ret->mipmaps));
  if (!ret->mipmaps) {
    TCOD_image_delete(ret);
    return NULL;
  }
  ret->mipmaps[0].buf = malloc(sizeof(*ret->mipmaps->buf) * width * height);
  if (!ret->mipmaps[0].buf) {
    TCOD_image_delete(ret);
    return NULL;
  }

  for (int i = 0; i < width * height; ++i) {
    ret->mipmaps[0].buf[i] = (TCOD_ColorRGB){0, 0, 0};
  }
  float fw = (float)width;
  float fh = (float)height;
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
void TCOD_image_get_size(const TCOD_Image* image, int* w, int* h) {
  if (w) *w = 0;
  if (h) *h = 0;
  if (!image) return;
  if (w) *w = image->mipmaps[0].width;
  if (h) *h = image->mipmaps[0].height;
}

TCOD_color_t TCOD_image_get_pixel(const TCOD_Image* image, int x, int y) {
  if (!image) {
    return (TCOD_ColorRGB){0, 0, 0};
  }

  if (TCOD_image_in_bounds(image, x, y)) {
    return image->mipmaps[0].buf[x + y * image->mipmaps[0].width];
  }
  return (TCOD_ColorRGB){0, 0, 0};
}

int TCOD_image_get_alpha(const TCOD_Image* image, int x, int y) {
  if (!image) {
    return 0;
  }
  return 255;
}

TCOD_color_t TCOD_image_get_mipmap_pixel(TCOD_Image* image, float x0, float y0, float x1, float y1) {
  if (!image) {
    return (TCOD_ColorRGB){0, 0, 0};
  }
  int cur_size = 1;
  int mip = 0;
  int texel_x_size = (int)(x1 - x0);
  int texel_y_size = (int)(y1 - y0);
  int texel_size = texel_x_size < texel_y_size ? texel_y_size : texel_x_size;
  while (mip < image->nb_mipmaps - 1 && cur_size < texel_size) {
    ++mip;
    cur_size <<= 1;
  }
  if (mip > 0) {
    --mip;
  }
  int texel_x = (int)(x0 * (image->mipmaps[mip].width) / image->mipmaps[0].fwidth);
  int texel_y = (int)(y0 * (image->mipmaps[mip].height) / image->mipmaps[0].fheight);
  if (image->mipmaps[mip].buf == NULL || image->mipmaps[mip].dirty) {
    TCOD_image_generate_mip(image, mip);
  }
  if (texel_x < 0 || texel_y < 0 || texel_x >= image->mipmaps[mip].width || texel_y >= image->mipmaps[mip].height) {
    return (TCOD_ColorRGB){0, 0, 0};
  }
  return image->mipmaps[mip].buf[texel_x + texel_y * image->mipmaps[mip].width];
}

void TCOD_image_put_pixel(TCOD_Image* image, int x, int y, TCOD_color_t col) {
  if (!image) {
    return;
  }
  if (TCOD_image_in_bounds(image, x, y)) {
    image->mipmaps[0].buf[x + y * image->mipmaps[0].width] = col;
    TCOD_image_invalidate_mipmaps(image);
  }
}

static void TCOD_image_delete_internal(TCOD_Image* image) {
  if (!image) {
    return;
  }
  if (image->mipmaps) {
    for (int i = 0; i < image->nb_mipmaps; ++i) {
      if (image->mipmaps[i].buf) {
        free(image->mipmaps[i].buf);
      }
    }
    free(image->mipmaps);
    image->mipmaps = NULL;
  }
}

void TCOD_image_delete(TCOD_Image* image) {
  if (!image) {
    return;
  }
  TCOD_image_delete_internal(image);
  free(image);
}

bool TCOD_image_is_pixel_transparent(const TCOD_Image* image, int x, int y) {
  if (!image) {
    return false;
  }
  TCOD_color_t col = TCOD_image_get_pixel(image, x, y);
  if (image->has_key_color && image->key_color.r == col.r && image->key_color.g == col.g &&
      image->key_color.b == col.b) {
    return true;
  }
  if (TCOD_image_get_alpha(image, x, y) == 0) {
    return true;
  }
  return false;
}

void TCOD_image_blit(
    TCOD_Image* image,
    TCOD_Console* console,
    float x,
    float y,
    TCOD_bkgnd_flag_t bkgnd_flag,
    float scale_x,
    float scale_y,
    float angle) {
  if (!image) {
    return;
  }
  console = TCOD_console_validate_(console);
  if (!console) {
    return;
  }
  if (scale_x == 0.0f || scale_y == 0.0f || bkgnd_flag == TCOD_BKGND_NONE) {
    return;
  }
  int width, height;
  TCOD_image_get_size(image, &width, &height);
  float rx_ = x - width * 0.5f;
  float ry_ = y - height * 0.5f;
  if (scale_x == 1.0f && scale_y == 1.0f && angle == 0.0f && rx_ == (int)rx_ && ry_ == (int)ry_) {
    /* clip the image */
    int ix = (int)(x - width * 0.5f);
    int iy = (int)(y - height * 0.5f);
    int min_x = TCOD_MAX(ix, 0);
    int min_y = TCOD_MAX(iy, 0);
    int max_x = TCOD_MIN(ix + width, TCOD_console_get_width(console));
    int max_y = TCOD_MIN(iy + height, TCOD_console_get_height(console));
    int offset_x = 0;
    int offset_y = 0;
    if (ix < 0) {
      offset_x = -ix;
    }
    if (iy < 0) {
      offset_y = -iy;
    }
    for (int cx = min_x; cx < max_x; ++cx) {
      for (int cy = min_y; cy < max_y; ++cy) {
        TCOD_color_t col = TCOD_image_get_pixel(image, cx - min_x + offset_x, cy - min_y + offset_y);
        if (!image->has_key_color || image->key_color.r != col.r || image->key_color.g != col.g ||
            image->key_color.b != col.b) {
          TCOD_console_set_char_background(console, cx, cy, col, bkgnd_flag);
        }
      }
    }
  } else {
    float iw = width / 2 * scale_x;
    float ih = height / 2 * scale_y;
    /* get the coordinates of the image corners in the console */
    float new_xx = cosf(angle);
    float new_xy = -sinf(angle);
    float new_yx = new_xy;
    float new_yy = -new_xx;
    // image corners coordinates
    /* 0 = P - w/2 x' +h/2 y' */
    int x0 = (int)(x - iw * new_xx + ih * new_yx);
    int y0 = (int)(y - iw * new_xy + ih * new_yy);
    /* 1 = P + w/2 x' + h/2 y' */
    int x1 = (int)(x + iw * new_xx + ih * new_yx);
    int y1 = (int)(y + iw * new_xy + ih * new_yy);
    /* 2 = P + w/2 x' - h/2 y' */
    int x2 = (int)(x + iw * new_xx - ih * new_yx);
    int y2 = (int)(y + iw * new_xy - ih * new_yy);
    /* 3 = P - w/2 x' - h/2 y' */
    int x3 = (int)(x - iw * new_xx - ih * new_yx);
    int y3 = (int)(y - iw * new_xy - ih * new_yy);
    /* get the affected rectangular area in the console */
    int rx = TCOD_MIN(TCOD_MIN(x0, x1), TCOD_MIN(x2, x3));
    int ry = TCOD_MIN(TCOD_MIN(y0, y1), TCOD_MIN(y2, y3));
    int rw = TCOD_MAX(TCOD_MAX(x0, x1), TCOD_MAX(x2, x3)) - rx;
    int rh = TCOD_MAX(TCOD_MAX(y0, y1), TCOD_MAX(y2, y3)) - ry;
    /* clip it */
    int min_x = TCOD_MAX(rx, 0);
    int min_y = TCOD_MAX(ry, 0);
    int max_x = TCOD_MIN(rx + rw, TCOD_console_get_width(console));
    int max_y = TCOD_MIN(ry + rh, TCOD_console_get_height(console));
    float inv_scale_x = 1.0f / scale_x;
    float inv_scale_y = 1.0f / scale_y;
    for (int cx = min_x; cx < max_x; ++cx) {
      for (int cy = min_y; cy < max_y; ++cy) {
        /* map the console pixel to the image world */
        float ix = (iw + (cx - x) * new_xx + (cy - y) * (-new_yx)) * inv_scale_x;
        float iy = (ih + (cx - x) * new_xy - (cy - y) * new_yy) * inv_scale_y;
        TCOD_color_t col = TCOD_image_get_pixel(image, (int)ix, (int)iy);
        if (!image->has_key_color || image->key_color.r != col.r || image->key_color.g != col.g ||
            image->key_color.b != col.b) {
          if (scale_x < 1.0f || scale_y < 1.0f) {
            col = TCOD_image_get_mipmap_pixel(image, ix, iy, ix + 1.0f, iy + 1.0f);
          }
          TCOD_console_set_char_background(console, cx, cy, col, bkgnd_flag);
        }
      }
    }
  }
}

void TCOD_image_blit_rect(
    TCOD_Image* image, TCOD_Console* console, int x, int y, int w, int h, TCOD_bkgnd_flag_t bkgnd_flag) {
  if (!image) {
    return;
  }
  console = TCOD_console_validate_(console);
  if (!console) {
    return;
  }
  int width, height;
  TCOD_image_get_size(image, &width, &height);
  if (w == -1) {
    w = width;
  }
  if (h == -1) {
    h = height;
  }
  if (w <= 0 || h <= 0 || bkgnd_flag == TCOD_BKGND_NONE) {
    return;
  }
  float scale_x = (float)w / width;
  float scale_y = (float)h / height;
  TCOD_image_blit(image, console, x + w * 0.5f, y + h * 0.5f, bkgnd_flag, scale_x, scale_y, 0.0f);
}

TCOD_Image* TCOD_image_from_console(const TCOD_Console* console) {
  console = TCOD_console_validate_(console);
  if (!console) {
    return NULL;
  }
  if (!TCOD_ctx.tileset) {
    return NULL;
  }
  TCOD_Image* ret = TCOD_image_new(
      TCOD_console_get_width(console) * TCOD_ctx.tileset->tile_width,
      TCOD_console_get_height(console) * TCOD_ctx.tileset->tile_height);
  if (!ret) {
    return NULL;
  }
  TCOD_image_refresh_console(ret, console);
  return ret;
}
void TCOD_image_refresh_console(TCOD_Image* __restrict image, const TCOD_Console* __restrict console) {
  if (!image) {
    return;
  }
  if (!TCOD_ctx.tileset) {
    return;
  }
  console = TCOD_console_validate_(console);
  if (!console) {
    return;
  }
  for (int console_y = 0; console_y < console->h; ++console_y) {
    for (int console_x = 0; console_x < console->w; ++console_x) {
      // Get the console index and tileset graphic.
      int console_i = console_y * console->w + console_x;
      const struct TCOD_ConsoleTile* tile = &console->tiles[console_i];
      const TCOD_ColorRGBA* __restrict graphic = TCOD_tileset_get_tile(TCOD_ctx.tileset, tile->ch);
      for (int y = 0; y < TCOD_ctx.tileset->tile_height; ++y) {
        for (int x = 0; x < TCOD_ctx.tileset->tile_width; ++x) {
          struct TCOD_ColorRGBA out_rgba = tile->bg;
          if (graphic) {
            // Multiply the foreground and tileset colors, then blend with bg.
            int graphic_i = y * TCOD_ctx.tileset->tile_width + x;
            struct TCOD_ColorRGBA fg = {
                tile->fg.r * graphic[graphic_i].r / 255,
                tile->fg.g * graphic[graphic_i].g / 255,
                tile->fg.b * graphic[graphic_i].b / 255,
                tile->fg.a * graphic[graphic_i].a / 255,
            };
            TCOD_color_alpha_blend(&out_rgba, &fg);
          }
          int out_x = console_x * TCOD_ctx.tileset->tile_width + x;
          int out_y = console_y * TCOD_ctx.tileset->tile_width + y;
          struct TCOD_ColorRGB out_rgb = {out_rgba.r, out_rgba.g, out_rgba.b};
          TCOD_image_put_pixel(image, out_x, out_y, out_rgb);
        }
      }
    }
  }
}
#ifndef NO_SDL
TCOD_Image* TCOD_image_load(const char* filename) {
  TCOD_Image* image = NULL;
  SDL_Surface* surface = TCOD_sys_load_image(filename);
  if (surface) {
    image = TCOD_image_new(surface->w, surface->h);
    if (image) {
      SDL_ConvertPixels(
          surface->w,
          surface->h,
          surface->format,
          surface->pixels,
          surface->pitch,
          SDL_PIXELFORMAT_RGB24,
          image->mipmaps[0].buf,
          (int)sizeof(image->mipmaps[0].buf[0]) * surface->w);
      TCOD_image_invalidate_mipmaps(image);
    }
    SDL_DestroySurface(surface);
  }
  return image;
}
TCOD_Error TCOD_image_save(const TCOD_Image* image, const char* filename) {
  if (!image) {
    TCOD_set_errorv("Image parameter must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  struct SDL_Surface* bitmap = SDL_CreateSurfaceFrom(
      image->mipmaps[0].width,
      image->mipmaps[0].height,
      SDL_PIXELFORMAT_RGB24,
      image->mipmaps[0].buf,
      (int)sizeof(image->mipmaps[0].buf[0]) * image->mipmaps[0].width);
  if (!bitmap) {
    return TCOD_set_errorvf("SDL error: %s", SDL_GetError());
  }
  TCOD_Error err = TCOD_sys_save_bitmap(bitmap, filename);
  SDL_DestroySurface(bitmap);
  return err;
}
#endif  // NO_SDL
void TCOD_image_set_key_color(TCOD_Image* image, TCOD_color_t key_color) {
  if (!image) {
    return;
  }
  image->has_key_color = true;
  image->key_color = key_color;
}

void TCOD_image_invert(TCOD_Image* image) {
  if (!image) {
    return;
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
  TCOD_image_invalidate_mipmaps(image);
}

void TCOD_image_hflip(TCOD_Image* image) {
  if (!image) {
    return;
  }
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

void TCOD_image_vflip(TCOD_Image* image) {
  if (!image) {
    return;
  }
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

void TCOD_image_rotate90(TCOD_Image* image, int numRotations) {
  if (!image) {
    return;
  }
  numRotations = numRotations % 4;
  if (numRotations == 0) {
    return;
  }
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
    image->nb_mipmaps = img2->nb_mipmaps;
    free(img2);
  } else if (numRotations == 2) {
    /* rotate 180 degrees */
    int max_y = height / 2 + ((height & 1) == 1 ? 1 : 0);
    for (int px = 0; px < width; ++px) {
      for (int py = 0; py < max_y; ++py) {
        if (py != height - 1 - py || px < width / 2) {
          TCOD_color_t col1 = TCOD_image_get_pixel(image, px, py);
          TCOD_color_t col2 = TCOD_image_get_pixel(image, width - 1 - px, height - 1 - py);
          TCOD_image_put_pixel(image, px, py, col2);
          TCOD_image_put_pixel(image, width - 1 - px, height - 1 - py, col1);
        }
      }
    }
  } else if (numRotations == 3) {
    /* rotate 270 degrees */
    TCOD_Image* new_image = TCOD_image_new(height, width);
    for (int px = 0; px < width; ++px) {
      for (int py = 0; py < height; ++py) {
        TCOD_color_t col1 = TCOD_image_get_pixel(image, px, py);
        TCOD_image_put_pixel(new_image, py, width - 1 - px, col1);
      }
    }
    TCOD_image_delete_internal(image);
    /* update image with the new image content */
    image->mipmaps = new_image->mipmaps;
    image->nb_mipmaps = new_image->nb_mipmaps;
    free(new_image);
  }
}

void TCOD_image_scale(TCOD_Image* image, int new_w, int new_h) {
  if (!image) {
    return;
  }
  int width, height;
  TCOD_image_get_size(image, &width, &height);
  if (new_w == width && new_h == height) {
    return;
  }
  if (new_w == 0 || new_h == 0) {
    return;
  }
  TCOD_Image* new_image = TCOD_image_new(new_w, new_h);

  if (new_w < width && new_h < height) {
    /* scale down image, using supersampling */
    for (int py = 0; py < new_h; ++py) {
      float y0 = (float)py * height / new_h;
      float y0floor = floorf(y0);
      float y0weight = 1.0f - (y0 - y0floor);
      int iy0 = (int)y0floor;

      float y1 = (float)(py + 1) * height / new_h;
      float y1floor = floorf(y1 - 0.00001f);
      float y1weight = (y1 - y1floor);
      int iy1 = (int)y1floor;

      for (int px = 0; px < new_w; ++px) {
        float x0 = (float)px * width / new_w;
        float x0floor = floorf(x0);
        float x0weight = 1.0f - (x0 - x0floor);
        int ix0 = (int)x0floor;

        float x1 = (float)(px + 1) * width / new_w;
        float x1floor = floorf(x1 - 0.00001f);
        float x1weight = (x1 - x1floor);
        int ix1 = (int)x1floor;

        float r = 0, g = 0, b = 0, sum_weight = 0.0f;
        /* left & right fractional edges */
        for (int srcy = (int)y0 + 1; srcy < (int)y1; ++srcy) {
          TCOD_color_t col_left = TCOD_image_get_pixel(image, ix0, srcy);
          TCOD_color_t col_right = TCOD_image_get_pixel(image, ix1, srcy);
          r += col_left.r * x0weight + col_right.r * x1weight;
          g += col_left.g * x0weight + col_right.g * x1weight;
          b += col_left.b * x0weight + col_right.b * x1weight;
          sum_weight += x0weight + x1weight;
        }
        /* top & bottom fractional edges */
        for (int srcx = (int)x0 + 1; srcx < (int)x1; ++srcx) {
          TCOD_color_t col_top = TCOD_image_get_pixel(image, srcx, iy0);
          TCOD_color_t col_bottom = TCOD_image_get_pixel(image, srcx, iy1);
          r += col_top.r * y0weight + col_bottom.r * y1weight;
          g += col_top.g * y0weight + col_bottom.g * y1weight;
          b += col_top.b * y0weight + col_bottom.b * y1weight;
          sum_weight += y0weight + y1weight;
        }
        /* center */
        for (int srcy = (int)y0 + 1; srcy < (int)y1; ++srcy) {
          for (int srcx = (int)x0 + 1; srcx < (int)x1; ++srcx) {
            TCOD_color_t sample = TCOD_image_get_pixel(image, srcx, srcy);
            r += sample.r;
            g += sample.g;
            b += sample.b;
            sum_weight += 1.0f;
          }
        }
        /* corners */
        TCOD_color_t col = TCOD_image_get_pixel(image, ix0, iy0);
        r += col.r * (x0weight * y0weight);
        g += col.g * (x0weight * y0weight);
        b += col.b * (x0weight * y0weight);
        sum_weight += x0weight * y0weight;
        col = TCOD_image_get_pixel(image, ix0, iy1);
        r += col.r * (x0weight * y1weight);
        g += col.g * (x0weight * y1weight);
        b += col.b * (x0weight * y1weight);
        sum_weight += x0weight * y1weight;
        col = TCOD_image_get_pixel(image, ix1, iy1);
        r += col.r * (x1weight * y1weight);
        g += col.g * (x1weight * y1weight);
        b += col.b * (x1weight * y1weight);
        sum_weight += x1weight * y1weight;
        col = TCOD_image_get_pixel(image, ix1, iy0);
        r += col.r * (x1weight * y0weight);
        g += col.g * (x1weight * y0weight);
        b += col.b * (x1weight * y0weight);
        sum_weight += x1weight * y0weight;
        sum_weight = 1.0f / sum_weight;
        r = r * sum_weight + 0.5f;
        g = g * sum_weight + 0.5f;
        b = b * sum_weight + 0.5f;
        col.r = (uint8_t)r;
        col.g = (uint8_t)g;
        col.b = (uint8_t)b;
        TCOD_image_put_pixel(new_image, px, py, col);
      }
    }
  } else {
    /* scale up image, using nearest neighbor */
    for (int py = 0; py < new_h; ++py) {
      int srcy = py * height / new_h;
      for (int px = 0; px < new_w; ++px) {
        int srcx = px * width / new_w;
        TCOD_image_put_pixel(new_image, px, py, TCOD_image_get_pixel(image, srcx, srcy));
      }
    }
  }
  /* destroy old image */
  TCOD_image_delete_internal(image);
  /* update image with the new image content */
  image->mipmaps = new_image->mipmaps;
  image->nb_mipmaps = new_image->nb_mipmaps;
  free(new_image);
}

// Return the squared distance between two colors.
static int rgb_squared_distance(const TCOD_ColorRGB* c1, const TCOD_ColorRGB* c2) {
  const int dr = (int)c1->r - c2->r;
  const int dg = (int)c1->g - c2->g;
  const int db = (int)c1->b - c2->b;
  return dr * dr + dg * dg + db * db;
}

// Return a new tile graphic from 4 quadrant colors.
static TCOD_ConsoleTile generate_quadrant_graphic(const TCOD_ColorRGB desired[4]) {
  // adapted from Jeff Lait's code posted on r.g.r.d
  /*
    A quadrant bitmask.  A raised bit means the quadrant is part of the foreground.
    Quadrants are arranged with the following bits:
      X 1
      2 4
   */
  int quadrant_mask = 0;
  // Maps a mask of quadrants to a codepoint.  A negative codepoint means to swap the fg/bg values.
  static int quadrant_to_codepoint[8] = {
      0,
      0x259D,  // Quadrant upper right.
      0x2597,  // Quadrant lower left.
      -0x259A,  // Quadrant upper left and lower right.
      0x2596,  // Quadrant lower right.
      0x2590,  // Right half block.
      -0x2580,  // Upper half block.
      -0x2598  // Quadrant upper left.
  };
  int quadrant_index;  // The active color quadrant being checked.
  // Ignore all quadrants matching the first color.
  for (quadrant_index = 1; quadrant_index < 4; ++quadrant_index) {
    if (!TCOD_color_equals(desired[quadrant_index], desired[0])) {
      break;  // Found a second color, will continue to check for colors from this index.
    }
  }
  if (quadrant_index == 4) {  // This tile is a solid color.
    return (TCOD_ConsoleTile){
        .ch = ' ',
        .fg = {desired[0].r, desired[0].g, desired[0].b, 255},
        .bg = {desired[0].r, desired[0].g, desired[0].b, 255},
    };
  }
  TCOD_ColorRGB palette[2] = {desired[0], desired[quadrant_index]};  // The current color palette: {bg color, fg color}.
  int weight[2] = {quadrant_index, 1};  // Number of quadrants that each pallette color is assigned, respectively.
  quadrant_mask |= 1 << (quadrant_index - 1);
  /* remaining colours */
  ++quadrant_index;
  while (quadrant_index < 4) {
    if (TCOD_color_equals(desired[quadrant_index], palette[0])) {
      // Assign to the background color.
      ++weight[0];
    } else if (TCOD_color_equals(desired[quadrant_index], palette[1])) {
      // Assign to the foreground color.
      quadrant_mask |= 1 << (quadrant_index - 1);
      ++weight[1];
    } else {
      // No more than two colors can be supported, so merge colors based on the smallest differences.
      const int dist_0_q = rgb_squared_distance(&desired[quadrant_index], &palette[0]);
      const int dist_1_q = rgb_squared_distance(&desired[quadrant_index], &palette[1]);
      const int dist_0_1 = rgb_squared_distance(&palette[0], &palette[1]);
      if (dist_0_q < dist_1_q) {
        if (dist_0_q <= dist_0_1) {
          // Merge 0 and quadrant_index.
          palette[0] = TCOD_color_lerp(desired[quadrant_index], palette[0], weight[0] / (1.0f + weight[0]));
          ++weight[0];
        } else {
          // Merge 0 and 1.
          palette[0] = TCOD_color_lerp(palette[0], palette[1], (float)weight[1] / (weight[0] + weight[1]));
          ++weight[0];
          palette[1] = desired[quadrant_index];
          quadrant_mask = 1 << (quadrant_index - 1);
        }
      } else {
        if (dist_1_q <= dist_0_1) {
          // Merge 1 and quadrant_index.
          palette[1] = TCOD_color_lerp(desired[quadrant_index], palette[1], weight[1] / (1.0f + weight[1]));
          ++weight[1];
          quadrant_mask |= 1 << (quadrant_index - 1);
        } else {
          // Merge 0 and 1.
          palette[0] = TCOD_color_lerp(palette[0], palette[1], (float)weight[1] / (weight[0] + weight[1]));
          ++weight[0];
          palette[1] = desired[quadrant_index];
          quadrant_mask = 1 << (quadrant_index - 1);
        }
      }
    }
    ++quadrant_index;
  }
  if (quadrant_to_codepoint[quadrant_mask] >= 0) {
    return (TCOD_ConsoleTile){
        .ch = quadrant_to_codepoint[quadrant_mask],
        .fg = (TCOD_ColorRGBA){palette[1].r, palette[1].g, palette[1].b, 255},
        .bg = (TCOD_ColorRGBA){palette[0].r, palette[0].g, palette[0].b, 255},
    };
  } else {  // A negative codepoint means we invert the bg/fg colors.
    return (TCOD_ConsoleTile){
        .ch = -quadrant_to_codepoint[quadrant_mask],
        .fg = (TCOD_ColorRGBA){palette[0].r, palette[0].g, palette[0].b, 255},
        .bg = (TCOD_ColorRGBA){palette[1].r, palette[1].g, palette[1].b, 255},
    };
  }
}

void TCOD_image_blit_2x(
    const TCOD_Image* __restrict image,
    TCOD_Console* __restrict console,
    int dest_x,
    int dest_y,
    int src_x,
    int src_y,
    int src_width,
    int src_height) {
  if (!image) return;
  console = TCOD_console_validate_(console);
  if (!console) return;

  int img_width, img_height;
  TCOD_image_get_size(image, &img_width, &img_height);
  if (src_width == -1) src_width = img_width;
  if (src_height == -1) src_height = img_height;

  /* check that the src_x,src_y/src_width,src_height rectangle is inside the image */
  TCOD_ASSERT(src_x >= 0 && src_y >= 0 && src_x + src_width <= img_width && src_y + src_height <= img_height);
  TCOD_IFNOT(src_width > 0 && src_height > 0) { return; }

  src_x = TCOD_MAX(0, src_x);
  src_y = TCOD_MAX(0, src_y);
  src_width = TCOD_MIN(src_width, img_width - src_x);
  src_height = TCOD_MIN(src_height, img_height - src_y);

  int max_x = dest_x + src_width / 2 <= console->w ? src_width : (console->w - dest_x) * 2;
  int max_y = dest_y + src_height / 2 <= console->h ? src_height : (console->h - dest_y) * 2;
  /* check that the image is not blitted outside the console */
  TCOD_IFNOT(dest_x + max_x / 2 >= 0 && dest_y + max_y / 2 >= 0 && dest_x < console->w && dest_y < console->h) {
    return;
  }
  max_x += src_x;
  max_y += src_y;

  for (int img_x = src_x; img_x < max_x; img_x += 2) {
    for (int img_y = src_y; img_y < max_y; img_y += 2) {
      TCOD_ColorRGB grid[4];
      /* get the 2x2 super pixel colors from the image */
      const int console_x = dest_x + (img_x - src_x) / 2;
      const int console_y = dest_y + (img_y - src_y) / 2;
      TCOD_ColorRGB consoleBack = TCOD_console_get_char_background(console, console_x, console_y);
      grid[0] = TCOD_image_get_pixel(image, img_x, img_y);
      if (image->has_key_color && TCOD_color_equals(grid[0], image->key_color)) {
        grid[0] = consoleBack;
      }
      if (img_x < max_x - 1) {
        grid[1] = TCOD_image_get_pixel(image, img_x + 1, img_y);
        if (image->has_key_color && TCOD_color_equals(grid[1], image->key_color)) {
          grid[1] = consoleBack;
        }
      } else {
        grid[1] = consoleBack;
      }
      if (img_y < max_y - 1) {
        grid[2] = TCOD_image_get_pixel(image, img_x, img_y + 1);
        if (image->has_key_color && TCOD_color_equals(grid[2], image->key_color)) {
          grid[2] = consoleBack;
        }
      } else {
        grid[2] = consoleBack;
      }
      if (img_x < max_x - 1 && img_y < max_y - 1) {
        grid[3] = TCOD_image_get_pixel(image, img_x + 1, img_y + 1);
        if (image->has_key_color && TCOD_color_equals(grid[3], image->key_color)) {
          grid[3] = consoleBack;
        }
      } else {
        grid[3] = consoleBack;
      }
      /* analyze color, posterize, get pattern */
      console->tiles[console_y * console->w + console_x] = generate_quadrant_graphic(grid);
    }
  }
}
