/* libtcod
 * Copyright © 2008-2019 Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The name of copyright holder nor the names of its contributors may not
 *       be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "console.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <algorithm>

#include <SDL.h>
#include "console.hpp"
#include "noise.h"
#include "mersenne.h"
#include "libtcod_int.h"
#include "utility.h"
#include "version.h"
#include "console/drawing.h"
#include "engine/globals.h"
#include "tileset/tileset.h"
#include "tileset/tilesheet.h"

#ifdef TCOD_CONSOLE_SUPPORT

#if defined( TCOD_VISUAL_STUDIO )
static const char *version_string = "libtcod " TCOD_STRVERSION;
#else
static const char *version_string __attribute__((unused)) = "libtcod " TCOD_STRVERSION;
#endif

TCOD_internal_context_t TCOD_ctx={
	/* number of characters in the bitmap font */
	16,16,
	/* font type and layout */
	false,false,false,
  0,
  /* character size in font */
  0, 0,
	"terminal.png","",
	NULL,NULL,NULL,0,false,0,0,0,0,0,0,
#ifndef TCOD_BARE
	/* default renderer to use */
	TCOD_RENDERER_GLSL,
	NULL,
#endif
	/* fading data */
	{0,0,0},255,
	/*key state*/
	{},
	/* window closed ? */
	false,
	/* mouse focus ? */
	false,
	/* application active ? */
	true,
};
/**
 *  Return a new console with a specific number of columns and rows.
 *
 *  \param w Number of columns.
 *  \param h Number of columns.
 *  \return A pointer to the new console, or NULL on error.
 */
TCOD_Console* TCOD_console_new(int w, int h)
{
  TCOD_IFNOT(w > 0 && h > 0 ) { return NULL; }
  struct TCOD_Console *con = static_cast<struct TCOD_Console*>(
      calloc(sizeof(*con), 1));
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
/**
 *  Return immediately with a recently pressed key.
 *
 *  \param flags A TCOD_event_t bit-field, for example: `TCOD_EVENT_KEY_PRESS`
 *  \return A TCOD_key_t struct with a recently pressed key.
 *          If no event exists then the `vk` attribute will be `TCODK_NONE`
 */
TCOD_key_t TCOD_console_check_for_keypress(int flags) {
	return TCOD_sys_check_for_keypress(flags);
}
/**
 *  Wait for a key press event, then return it.
 *
 *  \param flush If 1 then the event queue will be cleared before waiting for
 *               the next event.  This should always be 0.
 *  \return A TCOD_key_t struct with the most recent key data.
 *
 *  Do not solve input lag issues by arbitrarily dropping events!
 */
TCOD_key_t TCOD_console_wait_for_keypress(bool flush) {
	return TCOD_sys_wait_for_keypress(flush);
}
/**
 *  Set a consoles default background flag.
 *
 *  \param con A console pointer.
 *  \param flag One of `TCOD_bkgnd_flag_t`.
 */
void TCOD_console_set_background_flag(TCOD_Console* con,
                                      TCOD_bkgnd_flag_t flag)
{
  con = TCOD_console_validate_(con);
  if (con) { con->bkgnd_flag = flag; }
}
/**
 *  Return a consoles default background flag.
 */
TCOD_bkgnd_flag_t TCOD_console_get_background_flag(TCOD_Console* con)
{
  con = TCOD_console_validate_(con);
  return (con ? con->bkgnd_flag : TCOD_BKGND_NONE);
}
/**
 *  Set a consoles default alignment.
 *
 *  \param con A console pointer.
 *  \param alignment One of TCOD_alignment_t
 */
void TCOD_console_set_alignment(TCOD_Console* con, TCOD_alignment_t alignment)
{
  con = TCOD_console_validate_(con);
  if (con) { con->alignment = alignment; }
}
/**
 *  Return a consoles default alignment.
 */
TCOD_alignment_t TCOD_console_get_alignment(TCOD_console_t con)
{
  con = TCOD_console_validate_(con);
  return (con ? con->alignment : TCOD_LEFT);
}

void TCOD_console_data_free(struct TCOD_Console *con)
{
  if (!con) { return; }
  free(con->ch_array);
  free(con->fg_array);
  free(con->bg_array);
  con->ch_array = NULL;
  con->fg_array = NULL;
  con->bg_array = NULL;
}
/**
 *  Delete a console.
 *
 *  \param con A console pointer.
 *
 *  If the console being deleted is the root console, then the display will be
 *  uninitialized.
 */
void TCOD_console_delete(TCOD_Console* con)
{
  if (con) {
    TCOD_console_data_free(con);
    free(con);
  } else {
    TCOD_sys_shutdown();
    con=TCOD_ctx.root;
    TCOD_ctx.root=NULL;
  }
}
void TCOD_console_blit_key_color(
    TCOD_Console* src, int xSrc, int ySrc, int wSrc, int hSrc,
    TCOD_Console* dst, int xDst, int yDst,
    float foreground_alpha, float background_alpha,
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
      int dst_idx = dy * dst->w + dx;
      int src_idx = cy * src->w + cx;
      if (!TCOD_console_is_index_valid_(src, cx, cy)) { continue; }
      if (!TCOD_console_is_index_valid_(dst, dx, dy)) { continue; }
      /* Source references. */
      const int& src_ch = src->ch_array[src_idx];
      const TCOD_color_t& src_fg = src->fg_array[src_idx];
      const TCOD_color_t& src_bg = src->bg_array[src_idx];
      /* Check if source pixel is transparent. */
      if (key_color
          && src_bg.r == key_color->r
          && src_bg.g == key_color->g
          && src_bg.b == key_color->b) { continue; }
      /* Destination references. */
      int& dst_ch = dst->ch_array[dst_idx];
      TCOD_color_t& dst_fg = dst->fg_array[dst_idx];
      TCOD_color_t& dst_bg = dst->bg_array[dst_idx];
      if (foreground_alpha == 1.0f && background_alpha == 1.0f) {
        /* No alpha, so perform a plain copy. */
        dst_ch = src_ch;
        dst_fg = src_fg;
        dst_bg = src_bg;
        continue;
      }
      dst_bg = TCOD_color_lerp(dst_bg, src_bg, background_alpha);
      if (src_ch == ' ') {
        /* Source is space, so keep the current glyph. */
        dst_fg = TCOD_color_lerp(dst_fg, src_bg, background_alpha);
      } else if (dst_ch == ' ') {
        /* Destination is space, so use the glyph from source. */
        dst_ch = src_ch;
        dst_fg = TCOD_color_lerp(dst_bg, src_fg, foreground_alpha);
      } else if (dst_ch == src_ch) {
        dst_fg = TCOD_color_lerp(dst_fg, src_fg, foreground_alpha);
      } else {
        /* Pick the glyph based on foreground_alpha. */
        if (foreground_alpha < 0.5f) {
          dst_fg = TCOD_color_lerp(dst_fg, dst_bg, foreground_alpha * 2);
        } else {
          dst_ch = src_ch;
          dst_fg = TCOD_color_lerp(
              dst_bg, src_fg, (foreground_alpha - 0.5f) * 2);
        }
      }
    }
  }
}
/**
 *  Blit from one console to another.
 *
 *  \param srcCon Pointer to the source console.
 *  \param xSrc The left region of the source console to blit from.
 *  \param ySrc The top region of the source console to blit from.
 *  \param wSrc The width of the region to blit from.
 *              If 0 then it will fill to the maximum width.
 *  \param hSrc The height of the region to blit from.
 *              If 0 then it will fill to the maximum height.
 *  \param dstCon Pointer to the destination console.
 *  \param xDst The left corner to blit onto the destination console.
 *  \param yDst The top corner to blit onto the destination console.
 *  \param foreground_alpha Foreground blending alpha.
 *  \param background_alpha Background blending alpha.
 *
 *  If the source console has a key color, this function will use it.
 */
void TCOD_console_blit(
    TCOD_Console* src, int xSrc, int ySrc, int wSrc, int hSrc,
    TCOD_Console* dst, int xDst, int yDst,
    float foreground_alpha, float background_alpha) {
  src = TCOD_console_validate_(src);
  if (!src) { return; }
  TCOD_console_blit_key_color(
      src, xSrc, ySrc, wSrc, hSrc, dst, xDst, yDst,
      foreground_alpha, background_alpha,
      (src->has_key_color ? &src->key_color : NULL));
  }
/**
 *  Render and present the root console to the active display.
 */
void TCOD_console_flush(void) {
  TCOD_sys_flush(true);
}
/**
 *  Fade the color of the display.
 *
 *  \param val Where at 255 colors are normal and at 0 colors are completely
 *             faded.
 *  \param fadecol Color to fade towards.
 */
void TCOD_console_set_fade(uint8_t val, TCOD_color_t fadecol) {
	TCOD_ctx.fade=val;
	TCOD_ctx.fading_color=fadecol;
}
/**
 *  Return the fade value.
 *
 *  \return At 255 colors are normal and at 0 colors are completely faded.
 */
uint8_t TCOD_console_get_fade(void) {
	return TCOD_ctx.fade;
}
/**
 *  Return the fade color.
 *
 *  \return The current fading color.
 */
TCOD_color_t TCOD_console_get_fading_color(void) {
	return TCOD_ctx.fading_color;
}
/**
 *  Draw a character on a console using the default colors.
 *
 *  \param con A console pointer.
 *  \param x The X coordinate, the left-most position being 0.
 *  \param y The Y coordinate, the top-most position being 0.
 *  \param c The character code to place.
 *  \param flag A TCOD_bkgnd_flag_t flag.
 */
void TCOD_console_put_char(TCOD_Console* con, int x, int y, int c,
                           TCOD_bkgnd_flag_t flag)
{
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) { return; }
  con->ch_array[y * con->w + x] = c;
  TCOD_console_set_char_foreground(con, x, y, con->fore);
  TCOD_console_set_char_background(con, x, y, con->back, flag);
}
/**
 *  Draw a character on the console with the given colors.
 *
 *  \param con A console pointer.
 *  \param x The X coordinate, the left-most position being 0.
 *  \param y The Y coordinate, the top-most position being 0.
 *  \param c The character code to place.
 *  \param fore The foreground color.
 *  \param back The background color.  This color will not be blended.
 */
void TCOD_console_put_char_ex(TCOD_console_t con, int x, int y, int c,
                              TCOD_color_t fore, TCOD_color_t back)
{
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) { return; }
  con->ch_array[y * con->w + x] = c;
  TCOD_console_set_char_foreground(con, x, y, fore);
  TCOD_console_set_char_background(con, x, y, back, TCOD_BKGND_SET);
}
/**
 *  Manually mark a region of a console as dirty.
 */
void TCOD_console_set_dirty(int dx, int dy, int dw, int dh) {
	TCOD_sys_set_dirty(dx, dy, dw, dh);
}
/**
 *  Clear a console to its default colors and the space character code.
 */
void TCOD_console_clear(TCOD_console_t con)
{
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con) { return; }
  for (int i = 0; i < con->w * con->h; ++i) {
    con->ch_array[i] = ' ';
    con->fg_array[i] = con->fore;
    con->bg_array[i] = con->back;
  }
  /* clear the sdl renderer cache */
  TCOD_sys_set_dirty(0, 0, con->w, con->h);
}
/**
 *  Return the background color of a console at x,y
 *
 *  \param con A console pointer.
 *  \param x The X coordinate, the left-most position being 0.
 *  \param y The Y coordinate, the top-most position being 0.
 *  \return A TCOD_color_t struct with a copy of the background color.
 */
TCOD_color_t TCOD_console_get_char_background(const TCOD_Console* con,
                                              int x, int y)
{
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) { return TCOD_black; }
  return con->bg_array[y * con->w + x];
}
/**
 *  Change the foreground color of a console tile.
 *
 *  \param con A console pointer.
 *  \param x The X coordinate, the left-most position being 0.
 *  \param y The Y coordinate, the top-most position being 0.
 *  \param col The foreground color to set.
 */
void TCOD_console_set_char_foreground(TCOD_Console* con,
                                      int x, int y, TCOD_color_t col)
{
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) { return; }
  con->fg_array[y * con->w + x] = col;
}
/**
 *  Return the foreground color of a console at x,y
 *
 *  \param con A console pointer.
 *  \param x The X coordinate, the left-most position being 0.
 *  \param y The Y coordinate, the top-most position being 0.
 *  \return A TCOD_color_t struct with a copy of the foreground color.
 */
TCOD_color_t TCOD_console_get_char_foreground(const TCOD_Console* con,
                                              int x, int y)
{
	con = TCOD_console_validate_(con);
	if (!TCOD_console_is_index_valid_(con, x, y)) { return TCOD_white; }
	return con->fg_array[y * con->w + x];
}
/**
 *  Return a character code of a console at x,y
 *
 *  \param con A console pointer.
 *  \param x The X coordinate, the left-most position being 0.
 *  \param y The Y coordinate, the top-most position being 0.
 *  \return The character code.
 */
int TCOD_console_get_char(const TCOD_Console* con, int x, int y)
{
  con = TCOD_console_validate_(con);
	if (!TCOD_console_is_index_valid_(con, x, y)) { return 0; }
	return con->ch_array[y * con->w + x];
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
static constexpr TCOD_color_t blend_color_(
    const TCOD_color_t& bg, const TCOD_color_t& fg, const F& lambda)
{
  return {
      clamp_color_(lambda(bg.r, fg.r)),
      clamp_color_(lambda(bg.g, fg.g)),
      clamp_color_(lambda(bg.b, fg.b)),
  };
}
/**
 *  Blend a background color onto a console tile.
 *
 *  \param con A console pointer.
 *  \param x The X coordinate, the left-most position being 0.
 *  \param y The Y coordinate, the top-most position being 0.
 *  \param col The background color to blend.
 *  \param flag The blend mode to use.
 */
void TCOD_console_set_char_background(
    TCOD_Console* con, int x, int y, TCOD_color_t col, TCOD_bkgnd_flag_t flag)
{
  con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) { return; }
	TCOD_color_t& bg = con->bg_array[y * con->w + x];
  if (flag == TCOD_BKGND_DEFAULT) { flag = con->bkgnd_flag; }
  int alpha = flag >> 8;
  switch (flag & 0xff) {
    case TCOD_BKGND_SET:
      bg = col;
      break;
    case TCOD_BKGND_MULTIPLY:
      bg = TCOD_color_multiply(bg, col);
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
      bg = TCOD_color_lerp(bg, col, alpha / 255.0f);
      break;
    default: break;
  }
}
/**
 *  Change a character on a console tile, without changing its colors.
 *
 *  \param con A console pointer.
 *  \param x The X coordinate, the left-most position being 0.
 *  \param y The Y coordinate, the top-most position being 0.
 *  \param c The character code to set.
 */
void TCOD_console_set_char(TCOD_console_t con, int x, int y, int c)
{
	con = TCOD_console_validate_(con);
  if (!TCOD_console_is_index_valid_(con, x, y)) { return; }
	con->ch_array[y * con->w + x] = c;
}
static void TCOD_console_data_alloc(struct TCOD_Console *con)
{
  if (!con->ch_array) {
    con->ch_array = static_cast<int*>(
        calloc(sizeof(con->ch_array[0]), con->w * con->h));
  }
  if (!con->fg_array) {
    con->fg_array = static_cast<TCOD_color_t*>(
        calloc(sizeof(con->fg_array[0]), con->w * con->h));
  }
  if (!con->bg_array) {
    con->bg_array = static_cast<TCOD_color_t*>(
        calloc(sizeof(con->bg_array[0]), con->w * con->h));
  }
}

bool TCOD_console_init(TCOD_Console* con)
{
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con) { return false; }
  con->fore = TCOD_white;
  con->back = TCOD_black;

  TCOD_console_data_alloc(con);

  con->bkgnd_flag = TCOD_BKGND_NONE;
  con->alignment = TCOD_LEFT;
  for (int i = 0; i < con->w * con->h; ++i) {
    con->ch_array[i] = ' ';
  }
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

TCOD_color_t TCOD_console_get_default_background(TCOD_Console* con) {
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con) { return TCOD_black; }
  return con->back;
}
/**
 *  Return the width of a console.
 */
int TCOD_console_get_width(const TCOD_Console* con)
{
  con = TCOD_console_validate_(con);
  if (!con) { return 0; }
  return con->w;
}
/**
 *  Return the height of a console.
 */
int TCOD_console_get_height(const TCOD_Console* con)
{
  con = TCOD_console_validate_(con);
  if (!con) { return 0; }
  return con->h;
}
/**
 *  \brief Set a font image to be loaded during initialization.
 *
 *  \param fontFile The path to a font image.
 *  \param flags A TCOD_font_flags_t bit-field describing the font image
 *               contents.
 *  \param nb_char_horiz The number of columns in the font image.
 *  \param nb_char_vertic The number of rows in the font image.
 *
 *  `fontFile` will be case-sensitive depending on the platform.
 */
void TCOD_console_set_custom_font(const char *fontFile, int flags,
                                  int nb_char_horiz, int nb_char_vertic)
{
  strcpy(TCOD_ctx.font_file, fontFile);
  /* if layout not defined, assume ASCII_INCOL */
  if (!(flags & (TCOD_FONT_LAYOUT_ASCII_INCOL
                 | TCOD_FONT_LAYOUT_ASCII_INROW | TCOD_FONT_LAYOUT_TCOD))) {
    flags |= TCOD_FONT_LAYOUT_ASCII_INCOL;
  }
  TCOD_ctx.font_in_row = ((flags & TCOD_FONT_LAYOUT_ASCII_INROW) != 0);
  TCOD_ctx.font_greyscale = ((flags & TCOD_FONT_TYPE_GREYSCALE) != 0);
  TCOD_ctx.font_tcod_layout = ((flags & TCOD_FONT_LAYOUT_TCOD) != 0);
  TCOD_ctx.font_flags = flags;
  if (nb_char_horiz > 0 && nb_char_vertic > 0) {
    TCOD_ctx.fontNbCharHoriz = nb_char_horiz;
    TCOD_ctx.fontNbCharVertic = nb_char_vertic;
  } else {
    if (flags & TCOD_FONT_LAYOUT_TCOD) {
      TCOD_ctx.fontNbCharHoriz = nb_char_horiz = 32;
      TCOD_ctx.fontNbCharVertic = nb_char_vertic = 8;
    } else {
      TCOD_ctx.fontNbCharHoriz = nb_char_horiz = 16;
      TCOD_ctx.fontNbCharVertic = nb_char_vertic = 16;
    }
  }
  if (TCOD_ctx.font_tcod_layout) { TCOD_ctx.font_in_row = true; }
  TCOD_sys_set_custom_font(fontFile, nb_char_horiz, nb_char_vertic, flags);

  using tcod::image::Image;
  using tcod::tileset::Tileset;
  using tcod::tileset::Tilesheet;

  try {
    Image image = tcod::image::load(TCOD_ctx.font_file);
    auto tilesheet = std::make_shared<Tilesheet>(
        image, std::make_pair(nb_char_horiz, nb_char_vertic));
    tcod::engine::set_tilesheet(tilesheet);

    auto tileset = std::make_shared<Tileset>(tilesheet->get_tile_width(),
                                             tilesheet->get_tile_height());
    tcod::engine::set_tileset(tileset);
  } catch (const std::runtime_error&) {
    // Ignore any runtime error, will likely catch a missing file error.
    return;
  } catch (const std::logic_error&) {
    return; // Ignore logic errors from constructing objects.
  }
  TCOD_sys_decode_font_();
}
/**
 *  \brief Remap a character code to a tile.
 *
 *  \param asciiCode Character code to modify.
 *  \param fontCharX X tile-coordinate, starting from the left at zero.
 *  \param fontCharY Y tile-coordinate, starting from the top at zero.
 *
 *  X,Y parameters are the coordinate of the tile, not pixel-coordinates.
 */
void TCOD_console_map_ascii_code_to_font(int asciiCode,
                                         int fontCharX, int fontCharY)
{
  /* cannot change mapping before initRoot is called */
  TCOD_IFNOT(TCOD_ctx.root) { return; }
  TCOD_sys_map_ascii_to_font(asciiCode, fontCharX, fontCharY);
}
/**
 *  \brief Remap a series of character codes to a row of tiles.
 *
 *  \param asciiCode The starting character code.
 *  \param nbCodes Number of character codes to assign.
 *  \param fontCharX First X tile-coordinate, starting from the left at zero.
 *  \param fontCharY First Y tile-coordinate, starting from the top at zero.
 *
 *  This function always assigns tiles in row-major order, even if the
 *  TCOD_FONT_LAYOUT_ASCII_INCOL flag was set.
 */
void TCOD_console_map_ascii_codes_to_font(int asciiCode, int nbCodes,
                                          int fontCharX, int fontCharY)
{
  /* cannot change mapping before initRoot is called */
  TCOD_IFNOT(TCOD_ctx.root) { return; }
  TCOD_IFNOT(asciiCode >= 0 && asciiCode+nbCodes <= TCOD_ctx.max_font_chars) {
    return;
  }
  for (int c = asciiCode; c < asciiCode + nbCodes; ++c) {
    TCOD_sys_map_ascii_to_font(c, fontCharX, fontCharY);
    ++fontCharX;
    if (fontCharX == TCOD_ctx.fontNbCharHoriz) {
      fontCharX = 0;
      ++fontCharY;
    }
  }
}
/**
 *  \brief Remap a string of character codes to a row of tiles.
 *
 *  \param s A null-terminated string.
 *  \param fontCharX First X tile-coordinate, starting from the left at zero.
 *  \param fontCharY First Y tile-coordinate, starting from the top at zero.
 *
 *  This function always assigns tiles in row-major order, even if the
 *  TCOD_FONT_LAYOUT_ASCII_INCOL flag was set.
 */
void TCOD_console_map_string_to_font(const char *s,
                                     int fontCharX, int fontCharY)
{
  TCOD_IFNOT(s) { return; }
  /* cannot change mapping before initRoot is called */
  TCOD_IFNOT(TCOD_ctx.root) { return; }
  while (*s) {
    TCOD_console_map_ascii_code_to_font(*s, fontCharX, fontCharY);
    ++fontCharX;
    if (fontCharX == TCOD_ctx.fontNbCharHoriz) {
      fontCharX = 0;
      ++fontCharY;
    }
    ++s;
  }
}

bool TCOD_console_is_key_pressed(TCOD_keycode_t key) {
	return TCOD_sys_is_key_pressed(key);
}
void TCOD_console_set_key_color(TCOD_Console* con,TCOD_color_t col) {
  con = TCOD_console_validate_(con);
  if (!con) { return; }
  con->has_key_color = 1;
  con->key_color = col;
}

void TCOD_console_credits(void) {
	bool end=false;
	int x=TCOD_console_get_width(NULL)/2-6;
	int y=TCOD_console_get_height(NULL)/2;
	int fade=260;
	TCOD_sys_save_fps();
	TCOD_sys_set_fps(25);
	while (!end ) {
		TCOD_key_t k;
		end=TCOD_console_credits_render(x,y,false);
		TCOD_sys_check_for_event(TCOD_EVENT_KEY_PRESS,&k,NULL);
		if ( fade == 260 && k.vk != TCODK_NONE ) {
			fade -= 10;
		}
		TCOD_console_flush();
		if ( fade < 260 ) {
			fade -= 10;
			TCOD_console_set_fade(fade,TCOD_black);
			if ( fade == 0 ) end=true;
		}
	}
	TCOD_console_set_fade(255,TCOD_black);
	TCOD_sys_restore_fps();
}

static bool init2=false;

void TCOD_console_credits_reset(void) {
	init2=false;
}

bool TCOD_console_credits_render(int x, int y, bool alpha) {
	static char poweredby[128];
	static float char_heat[128];
	static int char_x[128];
	static int char_y[128];
	static bool init1=false;
	static int len,len1,cw=-1,ch=-1;
	static float xstr;
	static TCOD_color_t colmap[64];
	static TCOD_color_t colmap_light[64];
	static TCOD_noise_t noise;
	static TCOD_color_t colkeys[4] = {
		{255,255,204},
		{255,204,0},
		{255,102,0},
		{102,153,255},
	};
	static TCOD_color_t colkeys_light[4] = {
		{255,255,204},
		{128,128,77},
		{51,51,31},
		{0,0,0},
	};
	static int colpos[4]={
		0,21,42,63
	};
	static TCOD_image_t img=NULL;
	int i,xc,yc,xi,yi,j;
	static int left,right,top,bottom;
	float sparklex,sparkley,sparklerad,sparklerad2,noisex;
	/* mini particule system */
#define MAX_PARTICULES 50
	static float pheat[MAX_PARTICULES];
	static float px[MAX_PARTICULES],py[MAX_PARTICULES], pvx[MAX_PARTICULES],pvy[MAX_PARTICULES];
	static int nbpart=0, firstpart=0;
	static float partDelay=0.1f;
	float elapsed=TCOD_sys_get_last_frame_length();
	TCOD_color_t fbackup; /* backup fg color */

	if (!init1) {
		/* initialize all static data, colormaps, ... */
		TCOD_color_t col;
		TCOD_color_gen_map(colmap,4,colkeys,colpos);
		TCOD_color_gen_map(colmap_light,4,colkeys_light,colpos);
		sprintf(poweredby,"Powered by\n%s",version_string);
		noise=TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST,TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
		len=static_cast<int>(strlen(poweredby));
		len1=11; /* sizeof "Powered by\n" */
		left=MAX(x-4,0);
		top=MAX(y-4,0);
		col= TCOD_console_get_default_background(NULL);
		TCOD_console_set_default_background(NULL,TCOD_black);
		TCOD_console_set_default_background(NULL,col);
		init1=true;
	}
	if (!init2) {
		/* reset the credits vars ... */
		int curx,cury;
		xstr=-4.0f;
		curx=x;
		cury=y;
		for (i=0; i < len ;i++) {
			char_heat[i]=-1;
			char_x[i]=curx;
			char_y[i]=cury;
			curx++;
			if ( poweredby[i] == '\n' ) {
				curx=x;
				cury++;
			}
		}
		nbpart=firstpart=0;
		init2=true;
	}
	if (TCOD_console_get_width(NULL) != cw || TCOD_console_get_height(NULL)!=ch) {
		/* console size has changed */
		int width,height;
		cw=TCOD_console_get_width(NULL);
		ch=TCOD_console_get_height(NULL);
		right=MIN(x+len,cw-1);
		bottom=MIN(y+6,ch-1);
		width=right - left + 1;
		height=bottom - top + 1;
		if ( img ) TCOD_image_delete(img);
		img = TCOD_image_new(width*2,height*2);
	}
	fbackup=TCOD_console_get_default_foreground(NULL);
	if ( xstr < len1 ) {
		sparklex=x+xstr;
		sparkley=y;
	} else {
		sparklex=x-len1+xstr;
		sparkley=y+1;
	}
	noisex=xstr*6;
	sparklerad=3.0f+2*TCOD_noise_get(noise,&noisex);
	if ( xstr >= len-1 ) sparklerad -= (xstr-len+1)*4.0f;
	else if ( xstr < 0.0f ) sparklerad += xstr*4.0f;
	else if ( poweredby[ static_cast<int>(xstr+0.5f) ] == ' ' || poweredby[ static_cast<int>(xstr+0.5f) ] == '\n' ) sparklerad/=2;
	sparklerad2=sparklerad*sparklerad*4;

	/* draw the light */
	for (xc=left*2,xi=0; xc < (right+1)*2; xc++,xi++) {
		for (yc=top*2,yi=0; yc < (bottom+1)*2; yc++,yi++) {
			float dist=((xc-2*sparklex)*(xc-2*sparklex)+(yc-2*sparkley)*(yc-2*sparkley));
			TCOD_color_t pixcol;
			if ( sparklerad >= 0.0f && dist < sparklerad2 ) {
				int colidx=63-static_cast<int>(63*(sparklerad2-dist)/sparklerad2) + TCOD_random_get_int(NULL,-10,10);
				colidx=CLAMP(0,63,colidx);
				pixcol=colmap_light[colidx];
			} else {
				pixcol=TCOD_black;
			}
			if ( alpha ) {
				/*	console cells have following flag values :
				  		1 2
				  		4 8
				  	flag indicates which subcell uses foreground color */
				static int asciiToFlag[] = {
					1, /* TCOD_CHAR_SUBP_NW */
					2, /* TCOD_CHAR_SUBP_NE */
					3, /* TCOD_CHAR_SUBP_N */
					8, /* TCOD_CHAR_SUBP_SE */
					9, /* TCOD_CHAR_SUBP_DIAG */
					10, /* TCOD_CHAR_SUBP_E */
					4, /* TCOD_CHAR_SUBP_SW */
				};
				int conc= TCOD_console_get_char(NULL,xc/2,yc/2);
				TCOD_color_t bk=TCOD_console_get_char_background(NULL,xc/2,yc/2);
				if ( conc >= TCOD_CHAR_SUBP_NW && conc <= TCOD_CHAR_SUBP_SW ) {
					/* merge two subcell chars...
					   get the flag for the existing cell on root console */
					int bkflag=asciiToFlag[conc - TCOD_CHAR_SUBP_NW ];
					int xflag = (xc & 1);
					int yflag = (yc & 1);
					/* get the flag for the current subcell */
					int credflag = (1+3*yflag) * (xflag+1);
					if ( (credflag & bkflag) != 0 ) {
						/* the color for this subcell on root console
						   is foreground, not background */
						bk = TCOD_console_get_char_foreground(NULL,xc/2,yc/2);
					}
				}
				pixcol.r = std::min<int>(255, bk.r + pixcol.r);
				pixcol.g = std::min<int>(255, bk.g + pixcol.g);
				pixcol.b = std::min<int>(255, bk.b + pixcol.b);
			}
			TCOD_image_put_pixel(img,xi,yi,pixcol);
		}
	}

	/* draw and update the particules */
	j=nbpart;i=firstpart;
	while (j > 0) {
		int colidx = static_cast<int>(64 * (1.0f - pheat[i]));
		TCOD_color_t col;
		colidx=std::min(63, colidx);
		col=colmap[colidx];
		if (py[i] < (bottom - top + 1) * 2) {
			int ipx = static_cast<int>(px[i]);
			int ipy = static_cast<int>(py[i]);
			float fpx = px[i]-ipx;
			float fpy = py[i]-ipy;
			TCOD_color_t col2=TCOD_image_get_pixel(img,ipx,ipy);
			col2=TCOD_color_lerp(col,col2,0.5f*(fpx+fpy));
			TCOD_image_put_pixel(img,ipx,ipy,col2);
			col2=TCOD_image_get_pixel(img,ipx+1,ipy);
			col2=TCOD_color_lerp(col2,col,fpx);
			TCOD_image_put_pixel(img,ipx+1,ipy,col2);
			col2=TCOD_image_get_pixel(img,ipx,ipy+1);
			col2=TCOD_color_lerp(col2,col,fpy);
			TCOD_image_put_pixel(img,ipx,ipy+1,col2);
		} else pvy[i]=-pvy[i] * 0.5f;
		pvx[i] *= (1.0f-elapsed);
		pvy[i] += (1.0f-pheat[i])*elapsed*300.0f;
		px[i] += pvx[i]*elapsed;
		py[i] += pvy[i]*elapsed;
		pheat[i] -= elapsed*0.3f;
		if ( pheat[i] < 0.0f ) {
			firstpart = (firstpart+1)%MAX_PARTICULES;
			nbpart--;
		}
		i = (i+1)%MAX_PARTICULES;
		j--;
	}
	partDelay -= elapsed;
	if ( partDelay < 0.0f && nbpart < MAX_PARTICULES && sparklerad > 2.0f ) {
		/* fire a new particule */
		int lastpart = firstpart;
		int nb=nbpart;
		while (nb > 0 ) {
			lastpart = ( lastpart + 1 )%MAX_PARTICULES;
			nb--;
		}
		nbpart++;
		px[lastpart] = 2*(sparklex-left);
		py[lastpart] = 2*(sparkley-top)+2;
		pvx[lastpart] = TCOD_random_get_float(NULL,-5.0f,5.0f);
		pvy[lastpart] = TCOD_random_get_float(NULL,-0.5f, -15.0f);
		pheat[lastpart] = 1.0f;
		partDelay += 0.1f;
	}
	TCOD_image_blit_2x(img,NULL,left,top,0,0,-1,-1);
	/* draw the text */
	for (i=0; i < len ;i++) {
		if ( char_heat[i] >= 0.0f && poweredby[i]!='\n') {
			int colidx = static_cast<int>(64 * char_heat[i]);
			TCOD_color_t col;
			colidx=MIN(63,colidx);
			col=colmap[colidx];
			if ( xstr >= len  ) {
				float coef=(xstr-len)/len;
				if ( alpha ) {
					TCOD_color_t fore=TCOD_console_get_char_background(NULL,char_x[i],char_y[i]);
					int r = static_cast<int>(coef * fore.r + (1.0f - coef) * col.r);
					int g = static_cast<int>(coef * fore.g + (1.0f - coef) * col.g);
					int b = static_cast<int>(coef * fore.b + (1.0f - coef) * col.b);
					col.r = std::max(0, std::min(r, 255));
					col.g = std::max(0, std::min(g, 255));
					col.b = std::max(0, std::min(b, 255));
					TCOD_console_set_char_foreground(NULL,char_x[i],char_y[i],col);
				} else {
					col=TCOD_color_lerp(col,TCOD_black,coef);
				}
			}
			TCOD_console_set_char(NULL,char_x[i],char_y[i],poweredby[i]);
			TCOD_console_set_char_foreground(NULL,char_x[i],char_y[i],col);
		}
	}
	/* update letters heat */
	xstr += elapsed * 4;
	for (i = 0; i < static_cast<int>(xstr+0.5f); ++i) {
		char_heat[i]=(xstr-i)/(len/2);
	}
	/* restore fg color */
	TCOD_console_set_default_foreground(NULL,fbackup);
	if ( xstr <= 2*len ) return false;
	init2=false;
	return true;
}

static void TCOD_console_read_asc(TCOD_console_t con, FILE *f,
                                  int width, int height, float version)
{
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con) { return; }
  while(fgetc(f) != '#');
  for(int x = 0; x < width; ++x) {
      for(int y = 0; y < height; ++y) {
        TCOD_color_t fore, back;
        int c = fgetc(f);
        fore.r = fgetc(f);
        fore.g = fgetc(f);
        fore.b = fgetc(f);
        back.r = fgetc(f);
        back.g = fgetc(f);
        back.b = fgetc(f);
        /* skip solid/walkable info */
        if (version >= 0.3f) {
          fgetc(f);
          fgetc(f);
        }
        TCOD_console_put_char_ex(con, x, y, c, fore, back);
      }
    }
    fclose(f);
}

static int string_ends_with(const char *str, const char *suffix) {
	size_t str_len = strlen(str);
	size_t suffix_len = strlen(suffix);
	return
		(str_len >= suffix_len) &&
		(0 == strcmp(str + (str_len-suffix_len), suffix));
}

TCOD_console_t TCOD_console_from_file(const char *filename) {
	float version;
	int width,height;
	TCOD_console_t con;
	FILE *f;
	TCOD_IFNOT( filename != NULL ) {
		return NULL;
	}
	if (string_ends_with(filename, ".xp")) {
		return TCOD_console_from_xp(filename);
	}
	f=fopen(filename,"rb");
	TCOD_IFNOT( f!=NULL ) {
		return NULL;
	}
	if (fscanf(f, "ASCII-Paint v%g", &version) != 1 ) {
		fclose(f);
		return NULL;
	}
	if (fscanf(f, "%i %i", &width, &height) != 2 ) {
		fclose(f);
		return NULL;
	}
	TCOD_IFNOT ( width > 0 && height > 0) {
		fclose(f);
		return NULL;
	}
	con=TCOD_console_new(width,height);
	if (string_ends_with(filename, ".asc")) {
		TCOD_console_read_asc(con,f,width,height,version);
	}
	return con;
}

bool TCOD_console_load_asc(TCOD_console_t pcon, const char *filename) {
	float version;
	int width,height;
	FILE *f;
	struct TCOD_Console *con = TCOD_console_validate_(pcon);
	TCOD_IFNOT(con != NULL) return false;
	TCOD_IFNOT( filename != NULL ) {
		return false;
	}
	f=fopen(filename,"rb");
	TCOD_IFNOT( f!=NULL ) {
		return false;
	}
	if (fscanf(f, "ASCII-Paint v%g", &version) != 1 ) {
		fclose(f);
		return false;
	}
	if (fscanf(f, "%i %i", &width, &height) != 2 ) {
		fclose(f);
		return false;
	}
	TCOD_IFNOT ( width > 0 && height > 0) {
		fclose(f);
		return false;
	}
	if ( con->w != width || con->h != height ) {
		/* resize console */
		TCOD_console_data_free(con);
		con->w = width;
		con->h = height;
		TCOD_console_data_alloc(con);
	}
	TCOD_console_read_asc(con,f,width,height,version);
	return true;
}

bool TCOD_console_save_asc(TCOD_console_t pcon, const char *filename) {
	static float version = 0.3f;
	FILE *f;
	int x,y;
	struct TCOD_Console *con = TCOD_console_validate_(pcon);
	TCOD_IFNOT(con != NULL) return false;
	TCOD_IFNOT( filename != NULL ) {
		return false;
	}
	TCOD_IFNOT(con->w > 0 && con->h > 0) return false;
	f=fopen(filename,"wb");
	TCOD_IFNOT( f != NULL ) return false;
	fprintf(f, "ASCII-Paint v%g\n", static_cast<double>(version));
	fprintf(f, "%i %i\n", con->w, con->h);
	fputc('#', f);
	for(x = 0; x < con->w; x++) {
		for(y = 0; y < con->h; y++) {
			TCOD_color_t fore,back;
			int c=TCOD_console_get_char(con,x,y);
			fore=TCOD_console_get_char_foreground(con,x,y);
			back=TCOD_console_get_char_background(con,x,y);
			fputc(c, f);
			fputc(fore.r,f);
			fputc(fore.g,f);
			fputc(fore.b,f);
			fputc(back.r,f);
			fputc(back.g,f);
			fputc(back.b,f);
			fputc(0,f); /* solid */
			fputc(1,f); /* walkable */
		}
	}
	fclose(f);
	return true;
}

static bool hasDetectedBigEndianness = false;
static bool isBigEndian;
void detectBigEndianness(void) {
	if (!hasDetectedBigEndianness){
		uint32_t Value32;
		uint8_t *VPtr = reinterpret_cast<uint8_t *>(&Value32);
		VPtr[0] = VPtr[1] = VPtr[2] = 0; VPtr[3] = 1;
		if(Value32 == 1) isBigEndian = true;
		else isBigEndian = false;
		hasDetectedBigEndianness = true;
	}
}

uint16_t bswap16(uint16_t s){
	uint8_t* ps = reinterpret_cast<uint8_t*>(&s);
	uint16_t res;
	uint8_t* pres = reinterpret_cast<uint8_t*>(&res);
	pres[0] = ps[1];
	pres[1] = ps[0];
	return res;
}

uint32_t bswap32(uint32_t s){
	uint8_t *ps = reinterpret_cast<uint8_t*>(&s);
	uint32_t res;
	uint8_t *pres = reinterpret_cast<uint8_t*>(&res);
	pres[0]=ps[3];
	pres[1]=ps[2];
	pres[2]=ps[1];
	pres[3]=ps[0];
	return res;
}

uint16_t l16(uint16_t s){
	if (isBigEndian) return bswap16(s); else return s;
}

uint32_t l32(uint32_t s){
	if (isBigEndian) return bswap32(s); else return s;
}

/* fix the endianness */
void fix16(uint16_t* u){
	*u = l16(*u);
}

void fix32(uint32_t* u){
	*u = l32(*u);
}

/************ RIFF helpers  */

uint32_t fourCC(const char* c){
	return (*reinterpret_cast<const uint32_t*>(c));
}

/* checks if u equals str */
bool fourCCequals(uint32_t u, const char* str){
	return fourCC(str)==u;
}

void fromFourCC(uint32_t u, char*s){
	const char* c = reinterpret_cast<const char*>(&u);
	s[0]=c[0];
	s[1]=c[1];
	s[2]=c[2];
	s[3]=c[3];
	s[4]=0;
}

void put8(uint8_t d, FILE* fp){
	fwrite(&d,1,1,fp);
}

void put16(uint16_t d, FILE* fp){
	fwrite(&d,2,1,fp);
}

void put32(uint32_t d, FILE* fp){
	fwrite(&d,4,1,fp);
}

void putFourCC(const char* c, FILE* fp){
	put32(fourCC(c),fp);
}

void putData(void* what, int length, FILE* fp){
	fwrite(what,length,1,fp);
}

bool get8(uint8_t* u, FILE* fp){
	return 1==fread(u, sizeof(uint8_t),1,fp);
}

bool get16(uint16_t* u, FILE* fp){
	return 1==fread(u, sizeof(uint16_t),1,fp);
}

bool get32(uint32_t* u, FILE* fp){
	return 1==fread(u, sizeof(uint32_t),1,fp);
}

bool getData(void* u, size_t sz, FILE* fp){
	return 1==fread(u, sz,1,fp);
}


/********* APF RIFF structures */

typedef struct {
	uint32_t show_grid;
	uint32_t grid_width;
	uint32_t grid_height;
} SettingsDataV1;

#define FILTER_TYPE_UNCOMPRESSED 0
#define FORMAT_TYPE_CRGBRGB 0

typedef struct {
	uint32_t width;
	uint32_t height;
	uint32_t filter;
	uint32_t format;
} ImageDetailsV1;

/* Layers */

typedef struct {
	uint32_t name;
	uint32_t mode;
	uint32_t index;
	uint32_t dataSize;
} LayerV1 ;

typedef struct {
	uint32_t name;
	uint32_t mode;
	uint32_t fgalpha;
	uint32_t bgalpha;
	uint32_t visible;
	uint32_t index;
	uint32_t dataSize;
} LayerV2;

/* fix the endianness */
void fixSettings(SettingsDataV1* s){
	fix32(&s->show_grid);
	fix32(&s->grid_width);
	fix32(&s->grid_height);
}

void fixImage(ImageDetailsV1* v){
	fix32(&v->width);
	fix32(&v->height);
	fix32(&v->filter);
	fix32(&v->format);
}

void fixLayerv1(LayerV1* l){
	fix32(&l->mode);
	fix32(&l->index);
	fix32(&l->dataSize);
}

void fixLayerv2(LayerV2* l){
	fix32(&l->mode);
	fix32(&l->fgalpha);
	fix32(&l->bgalpha);
	fix32(&l->visible);
	fix32(&l->index);
	fix32(&l->dataSize);
}


/*********** ApfFile */

bool TCOD_console_save_apf(TCOD_console_t pcon, const char *filename) {
	struct TCOD_Console *con = TCOD_console_validate_(pcon);
	FILE* fp ;
	TCOD_IFNOT(con != NULL) return false;
	detectBigEndianness();

	fp = fopen(filename, "wb");
	if(fp == NULL) {
		return false;
	}
	else {
		int x,y;
		uint32_t riffSize = 0;
		uint32_t imgDetailsSize ;
		SettingsDataV1 settingsData;
		ImageDetailsV1 imgData;
		fpos_t posRiffSize;
		uint32_t settingsSz ;
		uint32_t layerImageSize ;
		uint32_t layerChunkSize ;
		/*  riff header*/
		putFourCC("RIFF",fp);
		fgetpos(fp,&posRiffSize);
		put32(0,fp);

			/* APF_ header */
			putFourCC("apf ",fp);
			riffSize += 4;

				/* settings */
				settingsData.show_grid = 0;
				settingsData.grid_width = 8;
				settingsData.grid_height = 8;
				settingsSz = sizeof(uint32_t) + sizeof settingsData;
				putFourCC("sett",fp);
				put32(l32(settingsSz),fp);
				put32(l32(1),fp);
				putData(&settingsData,sizeof settingsData,fp);
				if (settingsSz&1){
					put8(0,fp);
					riffSize++;
				}
				riffSize += 4+4+settingsSz;

				/* image details */
				imgData.width = con->w;
				imgData.height = con->h;
				imgData.filter = 0;
				imgData.format = 0;
				imgDetailsSize = sizeof(uint32_t) + sizeof imgData;
				putFourCC("imgd",fp);
				put32(l32(imgDetailsSize),fp);
				put32(l32(1),fp);
				putData(&imgData,sizeof imgData,fp);
				if (imgDetailsSize&1){
					put8(0,fp);
					riffSize++;
				}
				riffSize += 4+4+imgDetailsSize;

				/* now write the layers as a RIFF list
				   the first layer is the lowest layer
				   Assume imgData filter = uncompressed, and imgData format = CRGB */
				layerImageSize = imgData.width*imgData.height*7;
				layerChunkSize = sizeof(uint32_t) /* version */
						+ sizeof(LayerV2) /* header */
						+ layerImageSize; /* data */

				putFourCC("layr",fp); /* layer */
				put32(l32(layerChunkSize),fp);
					/* VERSION -> */
					put32(l32(2),fp);
					/* Data */
					putFourCC("LAY0",fp);
					put32(l32(0),fp);
					put32(l32(255),fp);
					put32(l32(255),fp);
					put32(l32(1),fp);
					put32(l32(0),fp);
					put32(l32(layerImageSize),fp);

					/* now write out the data */

					for(x = 0; x < con->w; x++) {
						for(y = 0; y < con->h; y++) {
							TCOD_color_t fore,back;
							int c=TCOD_console_get_char(con,x,y);
							fore=TCOD_console_get_char_foreground(con,x,y);
							back=TCOD_console_get_char_background(con,x,y);
							put8(c, fp);
							put8(fore.r,fp);
							put8(fore.g,fp);
							put8(fore.b,fp);
							put8(back.r,fp);
							put8(back.g,fp);
							put8(back.b,fp);
						}
					}

					if (layerChunkSize&1){
						put8(0,fp); /* padding bit */
						riffSize++;
					}

				riffSize += 2*sizeof(uint32_t)+layerChunkSize;

		fsetpos(fp,&posRiffSize);
		put32(l32(riffSize),fp);
	}

	fclose(fp);
	return true;
}

typedef struct {
	LayerV1 headerv1;
	LayerV2 headerv2;
	uint8_t* data; /* dynamically allocated */
}  LayerData;

typedef struct {
	ImageDetailsV1 details;
	SettingsDataV1 settings;
	LayerData layer;
} Data;

bool TCOD_console_load_apf(TCOD_console_t pcon, const char *filename) {
	uint32_t sett = fourCC("sett");
	uint32_t imgd = fourCC("imgd");
	/*
	uint32_t LIST = fourCC("LIST");
	uint32_t LAYR = fourCC("LAYR");
	*/
	uint32_t layr = fourCC("layr");
	FILE* fp ;
	Data data;
	struct TCOD_Console *con = TCOD_console_validate_(pcon);
	TCOD_IFNOT(con != NULL) return false;

	detectBigEndianness();
	data.details.width = 1;
	data.details.height = 1;
	data.details.filter = 0;
	data.details.format = 0;

	data.settings.show_grid = true;
	data.settings.grid_width = 10;
	data.settings.grid_height = 10;

	#define ERR(x) {printf("Error: %s\n. Aborting operation.",x); return false;}
	#define ERR_NEWER(x) {printf("Error: It looks like this file was made with a newer version of Ascii-Paint\n. In particular the %s field. Aborting operation.",x); return false;}

	fp = fopen(filename, "rb");
	if(fp == NULL) {
		printf("The file %s could not be loaded.\n", filename);
		return false;
	}
	else {
		/* read the header */
		uint32_t riff;
		uint32_t riffSize;
		int index = 0;
		int x,y;
		uint8_t *imgData;
		bool keepGoing = true;
		if (! get32(&riff,fp) || ! fourCCequals(riff,"RIFF")){
			ERR("File doesn't have a RIFF header");
		}
		if (!get32(&riffSize,fp)) ERR("No RIFF size field!");
		fix32(&riffSize);

		while(keepGoing && fp){ /* for each subfield, try to find the APF_ field */
			uint32_t apf;
			if (! get32(&apf,fp)) break;
			if (fourCCequals(apf,"apf ") || fourCCequals(apf,"APF ")){
				/* Process APF segment */
				while(keepGoing && fp){
					uint32_t seg;
					if (! get32(&seg,fp)){
						keepGoing = false;
						break;
					}
					else {
						if (seg==sett){
							/* size */
							uint32_t sz;
							uint32_t ver;
							SettingsDataV1 settingsData;

							get32(&sz,fp);
							fix32(&sz);
							/* version */
							get32(&ver,fp);
							fix32(&ver);
							if (ver!=1) ERR_NEWER("settings");
							/* ver must be 1 */
							if (! getData(&settingsData,sizeof settingsData,fp)) ERR("Can't read settings.");
							data.settings = settingsData;
							fixSettings(&data.settings);

						}
						else if (seg==imgd){
							/* sz */
							uint32_t sz;
							uint32_t ver;
							ImageDetailsV1 dets;

							get32(&sz,fp);
							fix32(&sz);
							/* version */
							get32(&ver,fp);
							fix32(&ver);
							if (ver!=1) ERR_NEWER("image details");
							/* ver must be 1 */
							if (! getData(&dets, sizeof dets, fp)) ERR("Can't read image details.");
							data.details = dets;
							fixImage(&data.details);

							/* get canvas ready */
							TCOD_IFNOT ( data.details.width > 0 && data.details.height > 0) {
								fclose(fp);
								return false;
							}
							if (con->w != static_cast<int>(data.details.width) || con->h != static_cast<int>(data.details.height)) {
								/* resize console */
								TCOD_console_data_free(con);
								con->w = data.details.width;
								con->h = data.details.height;
								TCOD_console_data_alloc(con);
							}

						}
						else if (seg==layr){
							uint32_t sz;
							uint32_t ver;

							get32(&sz,fp);
							fix32(&sz);
							/* version */
							get32(&ver,fp);
							fix32(&ver);
							if (ver>2) ERR_NEWER("layer spec");

							if (ver==1){
								if (! getData(static_cast<void*>(&data.layer.headerv1), sizeof( LayerV1 ), fp)) ERR("Can't read layer header.");
								fixLayerv1(&data.layer.headerv1);

								/* Read in the data chunk*/
								data.layer.data = static_cast<uint8_t*>(malloc(sizeof(uint8_t)*data.layer.headerv1.dataSize));
								getData(static_cast<void*>(data.layer.data), data.layer.headerv1.dataSize, fp);
							}
							else if (ver==2){
								if (! getData(static_cast<void*>(&data.layer.headerv2), sizeof( LayerV2 ), fp)) ERR("Can't read layer header.");
								fixLayerv2(&data.layer.headerv2);

								/* Read in the data chunk */
								data.layer.data = static_cast<uint8_t*>(malloc(sizeof(uint8_t)*data.layer.headerv2.dataSize));
								getData(static_cast<void*>(data.layer.data), data.layer.headerv2.dataSize, fp);

							}
						}
						else {
							/* skip unknown segment */
							uint32_t sz;
							get32(&sz,fp);
							fix32(&sz);
							fseek(fp,sz,SEEK_CUR);
						}
					}
				}

				/* we're done! */
				keepGoing = false;
			}
			else {
				/* skip this segment */
				uint32_t sz;
				get32(&sz,fp);
				fseek(fp,sz,SEEK_CUR);
			}
		}

		imgData = data.layer.data;
		for(x = 0; x < con->w; x++) {
			for(y = 0; y < con->h; y++) {
	    	TCOD_color_t fore,back;
		    int c = imgData[index++];
		    fore.r = imgData[index++];
		    fore.g = imgData[index++];
		    fore.b = imgData[index++];
		    back.r = imgData[index++];
		    back.g = imgData[index++];
		    back.b = imgData[index++];
		    TCOD_console_put_char_ex(con,x,y,c,fore,back);
			}
		}

		free (data.layer.data);
	}
	fclose(fp);

	return true;
}
/*

bool ApfFile::Load(std::string filename){
	detectBigEndianness();

	uint32_t sett = fourCC("sett");
	uint32_t imgd = fourCC("imgd");
	uint32_t LIST = fourCC("LIST");
	uint32_t LAYR = fourCC("LAYR");
	uint32_t layr = fourCC("layr");

	Data data; // File data

	data.details.width = 1;
	data.details.height = 1;
	data.details.filter = FILTER_TYPE_UNCOMPRESSED;
	data.details.format = FORMAT_TYPE_CRGBRGB;

	data.settings.show_grid = true;
	data.settings.grid_width = 10;
	data.settings.grid_height = 10;

	data.currentLayer = NULL;

	#define ERR(x) {printf("Error: %s\n. Aborting operation.",x); return false;}
	#define ERR_NEWER(x) {printf("Error: It looks like this file was made with a newer version of Ascii-Paint\n. In particular the %s field. Aborting operation.",x); return false;}

	FILE* fp = fopen(filename.c_str(), "rb");
	if(fp == NULL) {
		printf("The file %s could not be loaded.\n", filename.c_str());
		return false;
	}
	else {
		// read the header
		uint32_t riff;
		if (not get32(&riff,fp)
			or
			not fourCCequals(riff,"RIFF")){
			ERR("File doesn't have a RIFF header");
		}
		// else
		uint32_t riffSize;
		if (!get32(&riffSize,fp)) ERR("No RIFF size field!");
		fix(&riffSize);

		bool keepGoing = true;
		while(keepGoing and fp){ // for each subfield, try to find the APF_ field
			uint32_t apf;
			if (not get32(&apf,fp)) break;
			if (fourCCequals(apf,"apf ") or fourCCequals(apf,"APF ")){
				// Process APF segment
				while(keepGoing and fp){
					uint32_t seg;
					if (not get32(&seg,fp)){
						keepGoing = false;
						break;
					}
					else {
						if (seg==sett){
							// size
							uint32_t sz;
							get32(&sz,fp);
							fix(&sz);
							// version
							uint32_t ver;
							get32(&ver,fp);
							fix(&ver);
							if (ver!=1) ERR_NEWER("settings");
							// ver must be 1
							SettingsDataV1 settingsData;
							if (not getData((void*)&settingsData,sizeof settingsData,fp)) ERR("Can't read settings.");
							data.settings = settingsData;
							fix(&data.settings);

							// Change app settings
							app->setGridDimensions(data.settings.grid_width,data.settings.grid_height);
							app->setShowGrid(data.settings.show_grid==1);
						}
						else if (seg==imgd){
							// sz
							uint32_t sz;
							get32(&sz,fp);
							fix(&sz);
							// version
							uint32_t ver;
							get32(&ver,fp);
							fix(&ver);
							if (ver!=1) ERR_NEWER("image details");
							// ver must be 1
							ImageDetailsV1 dets;
							if (not getData((void*)&dets, sizeof dets, fp)) ERR("Can't read image details.");
							data.details = dets;
							fix(&data.details);

							// get canvas ready
							app->canvasWidth = data.details.width;
							app->canvasHeight = data.details.height;
							app->initCanvas();

							// delete new layer
							app->deleteLayer(app->getCurrentLayer()->name);

						}
						else if (seg==layr){
							// printf("Found a layer\n");

							// sz
							uint32_t sz;
							get32(&sz,fp);
							fix(&sz);
							// version
							uint32_t ver;
							get32(&ver,fp);
							fix(&ver);
							if (ver>2) ERR_NEWER("layer spec");

							if (ver==1){
								LayerV1 layerHeader;
								if (not getData((void*)&layerHeader, sizeof layerHeader, fp)) ERR("Can't read layer header.");
								fix(&layerHeader);

								// creat new layer data
								LayerData* ld = new LayerData;
								ld->header = layerHeader; // already fix'd
								ld->data = new uint8[ld->header.dataSize];

								// Read in the data chunk
								getData((void*) ld->data, ld->header.dataSize, fp);

								// push layer onto the list
								data.currentLayer = ld;
								data.layers.push(ld);
							}
							else if (ver==2){
								LayerV2 layerHeader;
								if (not getData((void*)&layerHeader, sizeof layerHeader, fp)) ERR("Can't read layer header.");
								fix(&layerHeader);

								// creat new layer data
								LayerData* ld = new LayerData;
								ld->header = layerHeader; // already fix'd
								ld->data = new uint8[ld->header.dataSize];

								// Read in the data chunk
								getData((void*) ld->data, ld->header.dataSize, fp);

								// push layer onto the list
								data.currentLayer = ld;
								data.layers.push(ld);
							}
						}
						else {
							// skip unknown segment
							uint32_t sz;
							get32(&sz,fp);
							fix(&sz);
							fseek(fp,sz,SEEK_CUR);
						}
					}
				}

				// we're done!
				keepGoing = false;
			}
			else {
				// skip this segment
				uint32_t sz;
				get32(&sz,fp);
				fseek(fp,sz,SEEK_CUR);
			}
		}

		// finally, copy the layers into the current document
		for(int i=0;i<data.layers.size();i++){
			// for now, just load the first layer
			LayerData* ld = data.layers.get(i);

			// canvas width/height have already been set...
			Layer* l = app->addNewLayer();

			// Parse layer header
			l->name = fromFourCC(ld->header.name);
			l->fgalpha = ld->header.fgalpha;
			l->bgalpha = ld->header.bgalpha;
			l->visible = (ld->header.visible==1);
			// l->compositingMode =

			// Copy data into currently selected canvas
			uint8_t* imgData = ld->data;
			CanvasImage *img = new CanvasImage;
			// Write the brush data for every brush in the image
			int index = 0;
			for(int x = 0; x < app->canvasWidth; x++) {
				for(int y = 0; y < app->canvasHeight; y++) {
					Brush b;
					b.symbol = (unsigned char)(imgData[index++]);
					b.fore.r = (uint8_t)(imgData[index++]);
					b.fore.g = (uint8_t)(imgData[index++]);
					b.fore.b = (uint8_t)(imgData[index++]);
					b.back.r = (uint8_t)(imgData[index++]);
					b.back.g = (uint8_t)(imgData[index++]);
					b.back.b = (uint8_t)(imgData[index++]);
					b.solid = true; // deprecated
					b.walkable = true; // deprecated
					img->push_back(b);
				}
			}

			app->setCanvasImage(*img);
			delete img;
		}

		// then free all the temporary layer data
		for(int i=0;i<data.layers.size();i++){
			delete[]data.layers.get(i)->data;
			delete data.layers.get(i);
		}

		// and update the layer widget
		app->gui->layerWidget->regenerateLayerList();
	}
	fclose(fp);

	return true;
}
*/

#endif /* TCOD_CONSOLE_SUPPORT */
