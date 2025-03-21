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
#include "console.h"

#include <stdlib.h>

#include "libtcod_int.h"
#include "utility.h"

static TCOD_Error TCOD_console_data_alloc(struct TCOD_Console* console) {
  if (!console) {
    return TCOD_E_ERROR;
  }
  if (console->tiles) {
    return TCOD_E_ERROR;
  }
  console->tiles = calloc(console->elements, sizeof(*console->tiles));
  return TCOD_E_OK;
}
static void TCOD_console_data_free(struct TCOD_Console* con) {
  if (!con) {
    return;
  }
  if (con->tiles) {
    free(con->tiles);
    con->tiles = NULL;
  }
}
static bool TCOD_console_init_(TCOD_Console* con) {
  con = TCOD_console_validate_(con);
  if (!con) {
    return false;
  }
  con->fore = TCOD_white;
  con->back = TCOD_black;

  TCOD_console_data_alloc(con);

  con->bkgnd_flag = TCOD_BKGND_NONE;
  con->alignment = TCOD_LEFT;
  TCOD_console_clear(con);
  return true;
}
TCOD_Console* TCOD_console_new(int w, int h) {
  if (w < 0 || h < 0) {
    TCOD_set_errorvf("Width and height can not be negative: got %i,%i", w, h);
    return NULL;
  }
  struct TCOD_Console* con = calloc(1, sizeof(*con));
  if (!con) {
    TCOD_set_errorv("Could not allocate memory for a console.");
    return NULL;
  }
  con->w = w;
  con->h = h;
  con->elements = w * h;
  TCOD_console_init_(con);
  if (TCOD_ctx.root) {
    // Default values for the root console are set by TCOD_console_init_.
    con->alignment = TCOD_ctx.root->alignment;
    con->bkgnd_flag = TCOD_ctx.root->bkgnd_flag;
  }
  return con;
}
void TCOD_console_delete(TCOD_Console* con) {
  TCOD_Console* console = (con ? con : TCOD_ctx.root);
  if (console) {
    if (console->on_delete) {
      console->on_delete(console);
    }
    TCOD_console_data_free(console);
    free(console);
  }
  if (console == TCOD_ctx.root) {
    TCOD_ctx.root = NULL;
  }
  if (con == NULL) {
    TCOD_sys_shutdown();
  }
}
void TCOD_console_resize_(TCOD_Console* console, int width, int height) {
  console = TCOD_console_validate_(console);
  if (!console) {
    return;
  }
  if (console->w == width && console->h == height) {
    return;
  }
  TCOD_console_data_free(console);
  console->w = width;
  console->h = height;
  console->elements = width * height;
  TCOD_console_data_alloc(console);
}
int TCOD_console_get_width(const TCOD_Console* con) {
  con = TCOD_console_validate_(con);
  return (con ? con->w : 0);
}
int TCOD_console_get_height(const TCOD_Console* con) {
  con = TCOD_console_validate_(con);
  return (con ? con->h : 0);
}
void TCOD_console_set_background_flag(TCOD_Console* con, TCOD_bkgnd_flag_t flag) {
  con = TCOD_console_validate_(con);
  if (con) {
    con->bkgnd_flag = flag;
  }
}
TCOD_bkgnd_flag_t TCOD_console_get_background_flag(TCOD_Console* con) {
  con = TCOD_console_validate_(con);
  return (con ? con->bkgnd_flag : TCOD_BKGND_NONE);
}
void TCOD_console_set_alignment(TCOD_Console* con, TCOD_alignment_t alignment) {
  con = TCOD_console_validate_(con);
  if (con) {
    con->alignment = alignment;
  }
}
TCOD_alignment_t TCOD_console_get_alignment(TCOD_console_t con) {
  con = TCOD_console_validate_(con);
  return (con ? con->alignment : TCOD_LEFT);
}
/**
 *  Perform alpha blending on a single channel.
 */
static uint8_t alpha_blend(int src_c, int src_a, int dst_c, int dst_a, int out_a) {
  return (uint8_t)(((src_c * src_a) + (dst_c * dst_a * (255 - src_a) / 255)) / out_a);
}
/**
 *  A modified lerp operation which can accept RGBA types.
 */
static struct TCOD_ColorRGBA TCOD_console_blit_lerp_(
    const struct TCOD_ColorRGBA dst, const struct TCOD_ColorRGBA src, float interp) {
  uint8_t out_a = (uint8_t)(src.a + dst.a * (255 - src.a) / 255);
  if (out_a == 0) {  // This would cause division by zero.
    return dst;  // Ignore alpha compositing and leave dst unchanged.
  }
  uint8_t src_a = (uint8_t)(src.a * interp);
  struct TCOD_ColorRGBA out = {
      alpha_blend(src.r, src_a, dst.r, dst.a, out_a),
      alpha_blend(src.g, src_a, dst.g, dst.a, out_a),
      alpha_blend(src.b, src_a, dst.b, dst.a, out_a),
      out_a,
  };
  return out;
}
/**
 *  Return the tile for a blit operation between src and dst.
 */
static struct TCOD_ConsoleTile TCOD_console_blit_cell_(
    const struct TCOD_ConsoleTile* __restrict src,
    const struct TCOD_ConsoleTile* __restrict dst,
    float fg_alpha,
    float bg_alpha,
    const struct TCOD_ColorRGB* __restrict key_color) {
  if (key_color && key_color->r == src->bg.r && key_color->g == src->bg.g && key_color->b == src->bg.b) {
    return *dst;  // Source pixel is transparent.
  }
  fg_alpha *= src->fg.a / 255.0f;
  bg_alpha *= src->bg.a / 255.0f;
  if (fg_alpha > 254.5f / 255.0f && bg_alpha > 254.5f / 255.0f) {
    return *src;  // No alpha. Perform a plain copy.
  }
  struct TCOD_ConsoleTile out = *dst;
  out.bg = TCOD_console_blit_lerp_(out.bg, src->bg, bg_alpha);
  if (src->ch == ' ') {
    // Source is space, so keep the current glyph.
    out.fg = TCOD_console_blit_lerp_(out.fg, src->bg, bg_alpha);
  } else if (out.ch == ' ') {
    // Destination is space, so use the glyph from source.
    out.ch = src->ch;
    out.fg = TCOD_console_blit_lerp_(out.bg, src->fg, fg_alpha);
  } else if (out.ch == src->ch) {
    out.fg = TCOD_console_blit_lerp_(out.fg, src->fg, fg_alpha);
  } else {
    /* Pick the glyph based on foreground_alpha. */
    if (fg_alpha < 0.5f) {
      out.fg = TCOD_console_blit_lerp_(out.fg, out.bg, fg_alpha * 2);
    } else {
      out.ch = src->ch;
      out.fg = TCOD_console_blit_lerp_(out.bg, src->fg, (fg_alpha - 0.5f) * 2);
    }
  }
  return out;
}
void TCOD_console_blit_key_color(
    const TCOD_Console* __restrict src,
    int xSrc,
    int ySrc,
    int wSrc,
    int hSrc,
    TCOD_Console* __restrict dst,
    int xDst,
    int yDst,
    float foreground_alpha,
    float background_alpha,
    const TCOD_color_t* key_color) {
  src = TCOD_console_validate_(src);
  dst = TCOD_console_validate_(dst);
  if (!src || !dst) {
    return;
  }
  if (wSrc == 0) {
    wSrc = src->w;
  }
  if (hSrc == 0) {
    hSrc = src->h;
  }
  if (wSrc <= 0 || hSrc <= 0) {
    return;
  }
  if (xDst + wSrc < 0 || yDst + hSrc < 0 || xDst >= dst->w || yDst >= dst->h) {
    return;
  }
  for (int cx = xSrc; cx < xSrc + wSrc; ++cx) {
    for (int cy = ySrc; cy < ySrc + hSrc; ++cy) {
      /* Check if we're outside the dest console. */
      int dx = cx - xSrc + xDst;
      int dy = cy - ySrc + yDst;
      if (!TCOD_console_is_index_valid_(src, cx, cy)) {
        continue;
      }
      if (!TCOD_console_is_index_valid_(dst, dx, dy)) {
        continue;
      }
      dst->tiles[dy * dst->w + dx] = TCOD_console_blit_cell_(
          &src->tiles[cy * src->w + cx], &dst->tiles[dy * dst->w + dx], foreground_alpha, background_alpha, key_color);
    }
  }
}
void TCOD_console_blit(
    const TCOD_Console* __restrict src,
    int xSrc,
    int ySrc,
    int wSrc,
    int hSrc,
    TCOD_Console* __restrict dst,
    int xDst,
    int yDst,
    float foreground_alpha,
    float background_alpha) {
  src = TCOD_console_validate_(src);
  if (!src) {
    return;
  }
  TCOD_console_blit_key_color(
      src,
      xSrc,
      ySrc,
      wSrc,
      hSrc,
      dst,
      xDst,
      yDst,
      foreground_alpha,
      background_alpha,
      (src->has_key_color ? &src->key_color : NULL));
}
void TCOD_console_put_char(TCOD_Console* con, int x, int y, int c, TCOD_bkgnd_flag_t flag) {
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) {
    return;
  }
  con->tiles[y * con->w + x].ch = c;
  TCOD_console_set_char_foreground(con, x, y, con->fore);
  TCOD_console_set_char_background(con, x, y, con->back, flag);
}
void TCOD_console_put_char_ex(TCOD_console_t con, int x, int y, int c, TCOD_color_t fore, TCOD_color_t back) {
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) {
    return;
  }
  con->tiles[y * con->w + x].ch = c;
  TCOD_console_set_char_foreground(con, x, y, fore);
  TCOD_console_set_char_background(con, x, y, back, TCOD_BKGND_SET);
}
void TCOD_console_clear(TCOD_console_t con) {
  con = TCOD_console_validate_(con);
  if (!con) {
    return;
  }
  struct TCOD_ConsoleTile fill = {
      ' ',
      {con->fore.r, con->fore.g, con->fore.b, 255},
      {con->back.r, con->back.g, con->back.b, 255},
  };
  for (int i = 0; i < con->elements; ++i) {
    con->tiles[i] = fill;
  }
}
TCOD_color_t TCOD_console_get_char_background(const TCOD_Console* con, int x, int y) {
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) {
    return TCOD_black;
  }
  const struct TCOD_ColorRGBA* color = &con->tiles[y * con->w + x].bg;
  struct TCOD_ColorRGB out = {color->r, color->g, color->b};
  return out;
}
void TCOD_console_set_char_foreground(TCOD_Console* con, int x, int y, TCOD_color_t col) {
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) {
    return;
  }
  struct TCOD_ColorRGBA* out = &con->tiles[y * con->w + x].fg;
  out->r = col.r;
  out->g = col.g;
  out->b = col.b;
  out->a = 255;
}
TCOD_color_t TCOD_console_get_char_foreground(const TCOD_Console* con, int x, int y) {
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) {
    return TCOD_white;
  }
  const struct TCOD_ColorRGBA* color = &con->tiles[y * con->w + x].fg;
  struct TCOD_ColorRGB out = {color->r, color->g, color->b};
  return out;
}
int TCOD_console_get_char(const TCOD_Console* con, int x, int y) {
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) {
    return 0;
  }
  return con->tiles[y * con->w + x].ch;
}
/**
 *  Clamp colors channels that are outside of uint8_t's range.
 */
static uint8_t clamp_color_(int c) { return (uint8_t)(TCOD_MAX(0, TCOD_MIN(c, 255))); }
/**
 *  Mix two colors using a lambda.
 */
static struct TCOD_ColorRGBA blend_color_(
    const struct TCOD_ColorRGBA* bg, const struct TCOD_ColorRGB* fg, int (*lambda)(uint8_t, uint8_t)) {
  struct TCOD_ColorRGBA out = {
      clamp_color_(lambda(bg->r, fg->r)),
      clamp_color_(lambda(bg->g, fg->g)),
      clamp_color_(lambda(bg->b, fg->b)),
      bg->a,
  };
  return out;
}
static int channel_multiply(uint8_t dst, uint8_t src) { return (int)dst * (int)src / 255; }
static int channel_lighten(uint8_t dst, uint8_t src) { return TCOD_MAX(dst, src); }
static int channel_darken(uint8_t dst, uint8_t src) { return TCOD_MIN(dst, src); }
static int channel_screen(uint8_t dst, uint8_t src) {
  // newbk = white - (white - oldbk) * (white - curbk)
  return 255 - (255 - (int)dst) * (255 - (int)src) / 255;
}
static int channel_color_dodge(uint8_t dst, uint8_t src) {
  // newbk = curbk / (white - oldbk)
  return (int)dst == 255 ? 255 : 255 * (int)src / (255 - (int)dst);
}
static int channel_color_burn(uint8_t dst, uint8_t src) {
  // newbk = white - (white - oldbk) / curbk
  return src == 0 ? 0 : 255 - (255 * (255 - (int)dst)) / (int)src;
}
static int channel_add(uint8_t dst, uint8_t src) { return (int)dst + (int)src; }
static int channel_burn(uint8_t dst, uint8_t src) {
  // newbk = oldbk + curbk - white
  return (int)dst + (int)src - 255;
}
static int channel_overlay(uint8_t dst, uint8_t src) {
  // newbk = curbk.x <= 0.5 ? 2*curbk*oldbk
  //                        : white - 2*(white-curbk)*(white-oldbk)
  return ((int)src <= 128 ? 2 * (int)src * (int)dst / 255 : 255 - 2 * (255 - (int)src) * (255 - (int)dst) / 255);
}
void TCOD_console_set_char_background(TCOD_Console* con, int x, int y, TCOD_color_t col, TCOD_bkgnd_flag_t flag) {
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) {
    return;
  }
  struct TCOD_ColorRGBA* bg = &con->tiles[y * con->w + x].bg;
  if (flag == TCOD_BKGND_DEFAULT) {
    flag = con->bkgnd_flag;
  }
  uint8_t alpha = (flag >> 8) & 0xFF;
  switch (flag & 0xff) {
    case TCOD_BKGND_SET:
      bg->r = col.r;
      bg->g = col.g;
      bg->b = col.b;
      break;
    case TCOD_BKGND_MULTIPLY:
      *bg = blend_color_(bg, &col, channel_multiply);
      break;
    case TCOD_BKGND_LIGHTEN:
      *bg = blend_color_(bg, &col, channel_lighten);
      break;
    case TCOD_BKGND_DARKEN:
      *bg = blend_color_(bg, &col, channel_darken);
      break;
    case TCOD_BKGND_SCREEN:
      // newbk = white - (white - oldbk) * (white - curbk)
      *bg = blend_color_(bg, &col, channel_screen);
      break;
    case TCOD_BKGND_COLOR_DODGE:
      // newbk = curbk / (white - oldbk)
      *bg = blend_color_(bg, &col, channel_color_dodge);
      break;
    case TCOD_BKGND_COLOR_BURN:
      // newbk = white - (white - oldbk) / curbk
      *bg = blend_color_(bg, &col, channel_color_burn);
      break;
    case TCOD_BKGND_ADD:
      // newbk = oldbk + curbk
      *bg = blend_color_(bg, &col, channel_add);
      break;
    case TCOD_BKGND_ADDA:
      // newbk = oldbk + alpha * curbk
      bg->r = clamp_color_((int)bg->r + alpha * (int)col.r / 255);
      bg->g = clamp_color_((int)bg->g + alpha * (int)col.g / 255);
      bg->b = clamp_color_((int)bg->b + alpha * (int)col.b / 255);
      break;
    case TCOD_BKGND_BURN:
      // newbk = oldbk + curbk - white
      *bg = blend_color_(bg, &col, channel_burn);
      break;
    case TCOD_BKGND_OVERLAY:
      // newbk = curbk.x <= 0.5 ? 2*curbk*oldbk
      //                        : white - 2*(white-curbk)*(white-oldbk)
      *bg = blend_color_(bg, &col, channel_overlay);
      break;
    case TCOD_BKGND_ALPH: {
      // newbk = (1.0f-alpha)*oldbk + alpha*(curbk-oldbk)
      struct TCOD_ColorRGBA col_rgba = {col.r, col.g, col.b, alpha};
      *bg = TCOD_console_blit_lerp_(*bg, col_rgba, 1.0f);
      break;
    }
    default:
      break;
  }
}
void TCOD_console_set_char(TCOD_console_t con, int x, int y, int c) {
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) {
    return;
  }
  con->tiles[y * con->w + x].ch = c;
}
void TCOD_console_set_default_foreground(TCOD_Console* con, TCOD_color_t col) {
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con) { return; }
  con->fore = col;
}
void TCOD_console_set_default_background(TCOD_Console* con, TCOD_color_t col) {
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con) { return; }
  con->back = col;
}
TCOD_color_t TCOD_console_get_default_foreground(TCOD_Console* con) {
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con) { return TCOD_white; }
  return con->fore;
}
TCOD_color_t TCOD_console_get_default_background(TCOD_Console* con) {
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con) { return TCOD_black; }
  return con->back;
}
void TCOD_console_set_fade(uint8_t val, TCOD_color_t fade_color) {
  TCOD_ctx.fade = val;
  TCOD_ctx.fading_color = fade_color;
}
uint8_t TCOD_console_get_fade(void) { return TCOD_ctx.fade; }
TCOD_color_t TCOD_console_get_fading_color(void) { return TCOD_ctx.fading_color; }
