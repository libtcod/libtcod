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
#include "console.h"
#include "../libtcod_int.h"
TCOD_Console* TCOD_console_new(int w, int h)
{
  TCOD_IFNOT(w > 0 && h > 0 ) { return NULL; }
  struct TCOD_Console *con = new TCOD_Console{};
  if (!con) { return NULL; }
  con->w = w;
  con->h = h;
  TCOD_console_init(con);
  if (TCOD_ctx.root) {
    con->alignment = TCOD_ctx.root->alignment;
    con->bkgnd_flag = TCOD_ctx.root->bkgnd_flag;
  }
  return con;
}
static void TCOD_console_data_alloc(struct TCOD_Console& console)
{
  int size = console.w * console.h;
  if (!console.tiles) { console.tiles = new struct TCOD_ConsoleTile[size](); }
}
static void TCOD_console_data_free(struct TCOD_Console *con)
{
  if (!con) { return; }
  delete[] con->tiles;
  con->tiles = nullptr;
}
bool TCOD_console_init(TCOD_Console* con)
{
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con) { return false; }
  con->fore = TCOD_white;
  con->back = TCOD_black;

  TCOD_console_data_alloc(*con);

  con->bkgnd_flag = TCOD_BKGND_NONE;
  con->alignment = TCOD_LEFT;
  TCOD_console_clear(con);
  return true;
}
bool TCOD_console_init(TCOD_Console* con, const std::string& title,
                       bool fullscreen)
{
  if (!TCOD_console_init(con)) { return false; }
  if (!TCOD_sys_init(con, fullscreen) ) { return false; }
  TCOD_sys_set_window_title(title.c_str());
  return true;
}
void TCOD_console_delete(TCOD_Console* con)
{
  TCOD_Console* console = (con ? con : TCOD_ctx.root);
  if (console) {
    TCOD_console_data_free(console);
    delete console;
  }
  if (console == TCOD_ctx.root) {
    TCOD_ctx.root = nullptr;
  }
  if (con == nullptr) {
    TCOD_sys_shutdown();
  }
}
void TCOD_console_resize_(TCOD_Console* console, int width, int height)
{
  console = TCOD_console_validate_(console);
  if (!console) { return; }
  if (console->w == width && console->h == height) { return; }
  TCOD_console_data_free(console);
  console->w = width;
  console->h = height;
  TCOD_console_data_alloc(*console);
}
int TCOD_console_get_width(const TCOD_Console* con)
{
  con = TCOD_console_validate_(con);
  return (con ? con->w : 0);
}
int TCOD_console_get_height(const TCOD_Console* con)
{
  con = TCOD_console_validate_(con);
  return (con ? con->h : 0);
}
void TCOD_console_set_background_flag(TCOD_Console* con,
                                      TCOD_bkgnd_flag_t flag)
{
  con = TCOD_console_validate_(con);
  if (con) { con->bkgnd_flag = flag; }
}
TCOD_bkgnd_flag_t TCOD_console_get_background_flag(TCOD_Console* con)
{
  con = TCOD_console_validate_(con);
  return (con ? con->bkgnd_flag : TCOD_BKGND_NONE);
}
void TCOD_console_set_alignment(TCOD_Console* con, TCOD_alignment_t alignment)
{
  con = TCOD_console_validate_(con);
  if (con) { con->alignment = alignment; }
}
TCOD_alignment_t TCOD_console_get_alignment(TCOD_console_t con)
{
  con = TCOD_console_validate_(con);
  return (con ? con->alignment : TCOD_LEFT);
}
/**
 *  A modified lerp operation which can accept RGBA types.
 */
static auto TCOD_console_blit_lerp_(
    const struct TCOD_ColorRGBA color1,
    const struct TCOD_ColorRGBA color2,
    float interp)
-> struct TCOD_ColorRGBA
{
  return tcod::ColorRGBA{
      TCOD_color_lerp(tcod::ColorRGB(color1), tcod::ColorRGB(color2), interp),
      color1.a,
  };
}
/**
 *  Return the tile for a blit operation between src and dst.
 */
static auto TCOD_console_blit_cell_(
    const struct TCOD_ConsoleTile& src,
    const struct TCOD_ConsoleTile& dst,
    float fg_alpha,
    float bg_alpha,
    const TCOD_ColorRGB* key_color)
-> struct TCOD_ConsoleTile
{
  if (key_color && tcod::ColorRGB(src.bg) == *key_color) {
    return dst; // Source pixel is transparent.
  }
  if (fg_alpha >= 1.0f && bg_alpha >= 1.0f) {
    return src; // No alpha. Perform a plain copy.
  }
  int ch = dst.ch;
  tcod::ColorRGBA bg(dst.bg);
  tcod::ColorRGBA fg(dst.fg);
  bg = TCOD_console_blit_lerp_(bg, src.bg, bg_alpha);
  if (src.ch == ' ') {
    // Source is space, so keep the current glyph.
    fg = TCOD_console_blit_lerp_(fg, src.bg, bg_alpha);
  } else if (ch == ' ') {
    // Destination is space, so use the glyph from source.
    ch = src.ch;
    fg = TCOD_console_blit_lerp_(bg, src.fg, fg_alpha);
  } else if (ch == src.ch) {
    fg = TCOD_console_blit_lerp_(fg, src.fg, fg_alpha);
  } else {
    /* Pick the glyph based on foreground_alpha. */
    if (fg_alpha < 0.5f) {
      fg = TCOD_console_blit_lerp_(fg, bg, fg_alpha * 2);
    } else {
      ch = src.ch;
      fg = TCOD_console_blit_lerp_(bg, src.fg, (fg_alpha - 0.5f) * 2);
    }
  }
  return {ch, fg, bg};
}
void TCOD_console_blit_key_color(
    const TCOD_Console* src,
    int xSrc,
    int ySrc,
    int wSrc,
    int hSrc,
    TCOD_Console* dst,
    int xDst,
    int yDst,
    float foreground_alpha,
    float background_alpha,
    const TCOD_color_t* key_color)
{
  src = TCOD_console_validate_(src);
  dst = TCOD_console_validate_(dst);
  if (!src || !dst) { return; }
  if (wSrc == 0) { wSrc = src->w; }
  if (hSrc == 0) { hSrc = src->h; }
  TCOD_IFNOT(wSrc > 0 && hSrc > 0) { return; }
  TCOD_IFNOT(xDst + wSrc >= 0 && yDst + hSrc >= 0
             && xDst < dst->w && yDst < dst->h) { return; }
  for (int cx = xSrc; cx < xSrc + wSrc; ++cx) {
    for (int cy = ySrc; cy < ySrc + hSrc; ++cy) {
      /* Check if we're outside the dest console. */
      int dx = cx - xSrc + xDst;
      int dy = cy - ySrc + yDst;
      if (!src->in_bounds(cy, cx)) { continue; }
      if (!dst->in_bounds(dy, dx)) { continue; }
      dst->at(dy, dx) = TCOD_console_blit_cell_(
          src->at(cy, cx),
          dst->at(dy, dx),
          foreground_alpha,
          background_alpha,
          key_color);
    }
  }
}
void TCOD_console_blit(
    const TCOD_Console* src,
    int xSrc,
    int ySrc,
    int wSrc,
    int hSrc,
    TCOD_Console* dst,
    int xDst,
    int yDst,
    float foreground_alpha,
    float background_alpha)
{
  src = TCOD_console_validate_(src);
  if (!src) { return; }
  TCOD_console_blit_key_color(
      src, xSrc, ySrc, wSrc, hSrc, dst, xDst, yDst,
      foreground_alpha, background_alpha,
      (src->has_key_color ? &src->key_color : NULL));
}
void TCOD_console_put_char(TCOD_Console* con, int x, int y, int c,
                           TCOD_bkgnd_flag_t flag)
{
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) { return; }
  con->tiles[y * con->w + x].ch = c;
  TCOD_console_set_char_foreground(con, x, y, con->fore);
  TCOD_console_set_char_background(con, x, y, con->back, flag);
}
void TCOD_console_put_char_ex(TCOD_console_t con, int x, int y, int c,
                              TCOD_color_t fore, TCOD_color_t back)
{
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) { return; }
  con->tiles[y * con->w + x].ch = c;
  TCOD_console_set_char_foreground(con, x, y, fore);
  TCOD_console_set_char_background(con, x, y, back, TCOD_BKGND_SET);
}
void TCOD_console_clear(TCOD_console_t con)
{
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con) { return; }
  for (auto& tile : *con) {
    tile = {' ', tcod::ColorRGBA(con->fore), tcod::ColorRGBA(con->back)};
  }
  /* clear the sdl renderer cache */
  TCOD_sys_set_dirty(0, 0, con->w, con->h);
}
TCOD_color_t TCOD_console_get_char_background(const TCOD_Console* con,
                                              int x, int y)
{
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) { return TCOD_black; }
  return tcod::ColorRGB(con->tiles[y * con->w + x].bg);
}
void TCOD_console_set_char_foreground(TCOD_Console* con,
                                      int x, int y, TCOD_color_t col)
{
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) { return; }
  con->tiles[y * con->w + x].fg = tcod::ColorRGBA(col);
}
TCOD_color_t TCOD_console_get_char_foreground(const TCOD_Console* con,
                                              int x, int y)
{
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) { return TCOD_white; }
  return tcod::ColorRGB(con->tiles[y * con->w + x].fg);
}
int TCOD_console_get_char(const TCOD_Console* con, int x, int y)
{
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) { return 0; }
  return con->tiles[y * con->w + x].ch;
}
/**
 *  Clamp colors channels that are outside of uint8_t's range.
 */
static constexpr uint8_t clamp_color_(uint8_t c) noexcept { return c; }
static constexpr uint8_t clamp_color_(int c) noexcept
{
  return static_cast<uint8_t>(std::max<int>(0, std::min<int>(c, 255)));
}
/**
 *  Mix two colors using a lambda.
 */
template <typename F>
static constexpr TCOD_ColorRGBA blend_color_(
    const TCOD_ColorRGBA& bg, const TCOD_color_t& fg, const F& lambda)
{
  return {
      clamp_color_(lambda(bg.r, fg.r)),
      clamp_color_(lambda(bg.g, fg.g)),
      clamp_color_(lambda(bg.b, fg.b)),
      bg.a,
  };
}
void TCOD_console_set_char_background(
    TCOD_Console* con, int x, int y, TCOD_color_t col, TCOD_bkgnd_flag_t flag)
{
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) { return; }
  TCOD_ColorRGBA& bg = con->tiles[y * con->w + x].bg;
  if (flag == TCOD_BKGND_DEFAULT) { flag = con->bkgnd_flag; }
  int alpha = flag >> 8;
  switch (flag & 0xff) {
    case TCOD_BKGND_SET:
      bg = tcod::ColorRGBA(col, bg.a);
      break;
    case TCOD_BKGND_MULTIPLY:
      bg = tcod::ColorRGBA(TCOD_color_multiply(tcod::ColorRGB(bg), col), bg.a);
      break;
    case TCOD_BKGND_LIGHTEN:
      bg = blend_color_(bg, col,
          [](uint8_t a, uint8_t b){ return std::max<uint8_t>(a, b); });
      break;
    case TCOD_BKGND_DARKEN:
      bg = blend_color_(bg, col,
          [](uint8_t a, uint8_t b){ return std::min<uint8_t>(a, b); });
      break;
    case TCOD_BKGND_SCREEN:
      // newbk = white - (white - oldbk) * (white - curbk)
      bg = blend_color_(bg, col,
          [](int a, int b){ return 255 - (255 - a) * (255 - b) / 255; });
      break;
    case TCOD_BKGND_COLOR_DODGE:
      // newbk = curbk / (white - oldbk)
      bg = blend_color_(bg, col,
          [](int a, int b){ return (a == 255 ? 255 : 255 * b / (255 - a)); });
      break;
    case TCOD_BKGND_COLOR_BURN:
      // newbk = white - (white - oldbk) / curbk
      bg = blend_color_(bg, col,
          [](int a, int b){
              return (b == 0 ? 0 : 255 - (255 * (255 - a)) / b); });
      break;
    case TCOD_BKGND_ADD:
      // newbk = oldbk + curbk
      bg = blend_color_(bg, col, [](int a, int b){ return a + b; });
      break;
    case TCOD_BKGND_ADDA:
      // newbk = oldbk + alpha * curbk
      bg = blend_color_(bg, col,
          [=](int a, int b){ return a + alpha * b / 255; });
      break;
    case TCOD_BKGND_BURN:
      // newbk = oldbk + curbk - white
      bg = blend_color_(bg, col, [](int a, int b){ return a + b - 255; });
      break;
    case TCOD_BKGND_OVERLAY:
      // newbk = curbk.x <= 0.5 ? 2*curbk*oldbk
      //                        : white - 2*(white-curbk)*(white-oldbk)
      bg = blend_color_(bg, col, [](int a, int b){
        return (b <= 128 ? 2 * b * a / 255
                         : 255 - 2 * (255 - b) * (255 - a) / 255); });
      break;
    case TCOD_BKGND_ALPH:
      // newbk = (1.0f-alpha)*oldbk + alpha*(curbk-oldbk)
      bg = tcod::ColorRGBA(TCOD_color_lerp(tcod::ColorRGB(bg), col, alpha / 255.0f), bg.a);
      break;
    default: break;
  }
}
void TCOD_console_set_char(TCOD_console_t con, int x, int y, int c)
{
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) { return; }
  con->tiles[y * con->w + x].ch = c;
}
void TCOD_console_set_default_foreground(TCOD_Console* con, TCOD_color_t col)
{
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con) { return; }
  con->fore = col;
}
void TCOD_console_set_default_background(TCOD_Console* con,TCOD_color_t col)
{
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con) { return; }
  con->back = col;
}
TCOD_color_t TCOD_console_get_default_foreground(TCOD_Console* con)
{
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con) { return TCOD_white; }
  return con->fore;
}
TCOD_color_t TCOD_console_get_default_background(TCOD_Console* con)
{
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con) { return TCOD_black; }
  return con->back;
}
void TCOD_console_set_fade(uint8_t val, TCOD_color_t fadecol)
{
  TCOD_ctx.fade = val;
  TCOD_ctx.fading_color = fadecol;
}
uint8_t TCOD_console_get_fade(void)
{
  return TCOD_ctx.fade;
}
TCOD_color_t TCOD_console_get_fading_color(void)
{
  return TCOD_ctx.fading_color;
}
