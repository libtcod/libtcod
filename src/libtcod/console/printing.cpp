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
#include "printing.h"

#include <ctype.h>
#ifndef NO_UNICODE
#include <wchar.h>
#include <wctype.h>
#endif
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <tuple>

#include "drawing.h"
#include "../console.h"
#include "../libtcod_int.h"
#include "../utility.h"
#include "../../vendor/utf8proc/utf8proc.h"
static TCOD_color_t color_control_fore[TCOD_COLCTRL_NUMBER] = {
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}};
static TCOD_color_t color_control_back[TCOD_COLCTRL_NUMBER];
/**
 *  Assign a foreground and background color to a color control index.
 *
 *  \param con Index to change, e.g. `TCOD_COLCTRL_1`
 *  \param fore Foreground color to assign to this index.
 *  \param back Background color to assign to this index.
 */
void TCOD_console_set_color_control(
    TCOD_colctrl_t con, TCOD_color_t fore, TCOD_color_t back)
{
  TCOD_IFNOT(con >= TCOD_COLCTRL_1 && con <= TCOD_COLCTRL_NUMBER) return;
  color_control_fore[con - 1] = fore;
  color_control_back[con - 1] = back;
}
char *TCOD_console_vsprint(const char *fmt, va_list ap)
{
  #define NB_BUFFERS 10
  #define INITIAL_SIZE 512
  /* several static buffers in case the function is used more than once in a single function call */
  static char *msg[NB_BUFFERS] = {NULL};
  static int buflen[NB_BUFFERS] = {0};
  static int curbuf = 0;
  char *ret;
  bool ok = false;
  if (!msg[0]) {
    int i;
    for (i = 0; i < NB_BUFFERS; i++) {
      buflen[i] = INITIAL_SIZE;
      msg[i] = static_cast<char*>(calloc(sizeof(char), INITIAL_SIZE));
    }
  }
  do {
    /* warning ! depending on the compiler, vsnprintf return -1 or
     the expected string length if the buffer is not big enough */
    va_list ap_clone;
    va_copy(ap_clone, ap);
    int len = std::vsnprintf(msg[curbuf], buflen[curbuf], fmt, ap_clone);
    va_end(ap_clone);
    ok=true;
    if (len < 0 || len >= buflen[curbuf]) {
      /* buffer too small. */
      if (len > 0) {
        while (buflen[curbuf] < len + 1) { buflen[curbuf] *= 2; }
      } else {
        buflen[curbuf] *= 2;
      }
      free(msg[curbuf]);
      msg[curbuf] = static_cast<char*>(calloc(sizeof(char), buflen[curbuf]));
      ok = false;
    }
  } while (!ok);
  ret = msg[curbuf];
  curbuf = (curbuf + 1) % NB_BUFFERS;
  return ret;
}
/**
 *  Print a titled, framed region on a console, using default colors and
 *  alignment.
 *
 *  \param con A console pointer.
 *  \param x The starting X coordinate, the left-most position being 0.
 *  \param y The starting Y coordinate, the top-most position being 0.
 *  \param w The width of the frame.
 *  \param h The height of the frame.
 *  \param empty If true the characters inside of the frame will be cleared
 *               with spaces.
 *  \param flag The blending flag.
 *  \param fmt A format string as if passed to printf.
 *  \param ... Variadic arguments as if passed to printf.
 *
 *  This function makes assumptions about the fonts character encoding.
 *  It will fail if the font encoding is not `cp437`.
 */
void TCOD_console_print_frame(
    TCOD_Console* con,int x, int y, int w, int h,
    bool empty, TCOD_bkgnd_flag_t flag, const char* fmt, ...)
{
  con = TCOD_console_validate_(con);
  if (!con) { return; }
  TCOD_console_put_char(con, x, y, TCOD_CHAR_NW, flag);
  TCOD_console_put_char(con, x + w - 1, y, TCOD_CHAR_NE, flag);
  TCOD_console_put_char(con, x, y + h - 1 , TCOD_CHAR_SW, flag);
  TCOD_console_put_char(con, x + w - 1, y + h - 1, TCOD_CHAR_SE, flag);
  TCOD_console_hline(con, x + 1, y, w - 2, flag);
  TCOD_console_hline(con, x + 1, y + h - 1, w - 2, flag);
  if (h > 2) {
    TCOD_console_vline(con, x,y + 1, h - 2, flag);
    TCOD_console_vline(con, x + w - 1, y + 1, h - 2, flag);
    if (empty) {
      TCOD_console_rect(con, x + 1, y + 1, w - 2, h - 2, true, flag);
    }
  }
  if (fmt) {
    va_list ap;
    int xs;
    char *title;
    va_start(ap, fmt);
    title = TCOD_console_vsprint(fmt, ap);
    va_end(ap);
    title[w - 3] = 0; /* truncate if needed */
    xs = x + (w - static_cast<int>(strlen(title)) - 2) / 2;
    std::swap(con->fore, con->back);
    TCOD_console_print_ex(con, xs, y, TCOD_BKGND_SET, TCOD_LEFT,
                          " %s ", title);
    std::swap(con->fore, con->back);
  }
}
/**
 *  Print a string on a console, using default colors and alignment.
 *
 *  \param con A console pointer.
 *  \param x The starting X coordinate, the left-most position being 0.
 *  \param y The starting Y coordinate, the top-most position being 0.
 *  \param fmt A format string as if passed to printf.
 *  \param ... Variadic arguments as if passed to printf.
 */
void TCOD_console_print(TCOD_Console* con, int x, int y, const char* fmt, ...)
{
  va_list ap;
  con = TCOD_console_validate_(con);
  if (!con) { return; }
  va_start(ap, fmt);
  TCOD_console_print_internal(con, x, y, 0, 0, con->bkgnd_flag,
      con->alignment, TCOD_console_vsprint(fmt, ap), false, false);
  va_end(ap);
}
/**
 *  Print a string on a console, using default colors.
 *
 *  \param con A console pointer.
 *  \param x The starting X coordinate, the left-most position being 0.
 *  \param y The starting Y coordinate, the top-most position being 0.
 *  \param flag The blending flag.
 *  \param alignment The font alignment to use.
 *  \param fmt A format string as if passed to printf.
 *  \param ... Variadic arguments as if passed to printf.
 */
void TCOD_console_print_ex(TCOD_Console* con,int x, int y,
    TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  TCOD_console_print_internal(con, x, y, 0, 0, flag, alignment,
      TCOD_console_vsprint(fmt, ap), false, false);
  va_end(ap);
}
/**
 *  Print a string on a console constrained to a rectangle, using default
 *  colors and alignment.
 *
 *  \param con A console pointer.
 *  \param x The starting X coordinate, the left-most position being 0.
 *  \param y The starting Y coordinate, the top-most position being 0.
 *  \param w The width of the region.
 *           If 0 then the maximum width will be used.
 *  \param h The height of the region.
 *           If 0 then the maximum height will be used.
 *  \param fmt A format string as if passed to printf.
 *  \param ... Variadic arguments as if passed to printf.
 *  \return The number of lines actually printed.
 */
int TCOD_console_print_rect(TCOD_Console* con, int x, int y, int w, int h,
                            const char *fmt, ...)
{
  int ret;
  va_list ap;
  con = TCOD_console_validate_(con);
  if (!con) { return 0; }
  va_start(ap, fmt);
  ret = TCOD_console_print_internal(con, x, y, w, h, con->bkgnd_flag,
      con->alignment, TCOD_console_vsprint(fmt, ap), true, false);
  va_end(ap);
  return ret;
}
/**
 *  Print a string on a console constrained to a rectangle, using default
 *  colors.
 *
 *  \param con A console pointer.
 *  \param x The starting X coordinate, the left-most position being 0.
 *  \param y The starting Y coordinate, the top-most position being 0.
 *  \param w The width of the region.
 *           If 0 then the maximum width will be used.
 *  \param h The height of the region.
 *           If 0 then the maximum height will be used.
 *  \param flag The blending flag.
 *  \param alignment The font alignment to use.
 *  \param fmt A format string as if passed to printf.
 *  \param ... Variadic arguments as if passed to printf.
 *  \return The number of lines actually printed.
 */
int TCOD_console_print_rect_ex(TCOD_Console* con, int x, int y, int w, int h,
    TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char *fmt, ...)
{
  int ret;
  va_list ap;
  va_start(ap, fmt);
  ret = TCOD_console_print_internal(con, x, y, w, h, flag, alignment,
      TCOD_console_vsprint(fmt, ap), true, false);
  va_end(ap);
  return ret;
}
/**
 *  Return the number of lines that would be printed by the
 *
 *  \param con A console pointer.
 *  \param x The starting X coordinate, the left-most position being 0.
 *  \param y The starting Y coordinate, the top-most position being 0.
 *  \param w The width of the region.
 *           If 0 then the maximum width will be used.
 *  \param h The height of the region.
 *           If 0 then the maximum height will be used.
 *  \param fmt A format string as if passed to printf.
 *  \param ... Variadic arguments as if passed to printf.
 *  \return The number of lines that would have been printed.
 */
int TCOD_console_get_height_rect(TCOD_Console* con,
    int x, int y, int w, int h, const char *fmt, ...)
{
  int ret;
  va_list ap;
  va_start(ap, fmt);
  ret = TCOD_console_print_internal(con, x, y, w, h, TCOD_BKGND_NONE,
      TCOD_LEFT, TCOD_console_vsprint(fmt, ap), true, true);
  va_end(ap);
  return ret;
}
/* non public methods */
int TCOD_console_stringLength(const unsigned char *s)
{
  int l = 0;
  while (*s) {
    if (*s == TCOD_COLCTRL_FORE_RGB || *s == TCOD_COLCTRL_BACK_RGB) {
      s += 3;
    } else if (*s > TCOD_COLCTRL_STOP) {
      l++;
    }
    s++;
  }
  return l;
}
unsigned char * TCOD_console_forward(unsigned char *s,int l)
{
  while (*s && l > 0) {
    if (*s == TCOD_COLCTRL_FORE_RGB || *s == TCOD_COLCTRL_BACK_RGB) {
      s += 3;
    } else if (*s > TCOD_COLCTRL_STOP) {
      l--;
    }
    s++;
  }
  return s;
}
unsigned char *TCOD_console_strchr(unsigned char *s, unsigned char c)
{
  while (*s && *s != c) {
    if (*s == TCOD_COLCTRL_FORE_RGB || *s == TCOD_COLCTRL_BACK_RGB) {
      s += 3;
    }
    s++;
  }
  return (*s ? s : NULL);
}

int TCOD_console_print_internal(
    TCOD_Console* con, int x,int y, int rw, int rh, TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t align, char *msg, bool can_split, bool count_only)
{
  unsigned char *c = reinterpret_cast<unsigned char *>(msg);
  int cx = 0;
  int cy = y;
  int minx, maxx, miny, maxy;
  TCOD_color_t oldFore;
  TCOD_color_t oldBack;
  con = TCOD_console_validate_(con);
  if (!con) { return 0; }
  TCOD_IFNOT(TCOD_console_is_index_valid_(con, x, y)) { return 0; }
  TCOD_IFNOT(msg != NULL) { return 0; }
  if (rh == 0) { rh = con->h - y; }
  if (rw == 0) {
    switch(align) {
      case TCOD_LEFT: rw = con->w - x; break;
      case TCOD_RIGHT: rw = x + 1; break;
      case TCOD_CENTER: default: rw = con->w; break;
    }
  }
  oldFore = con->fore;
  oldBack = con->back;
  miny = y;
  maxy = con->h - 1;
  if (rh > 0) { maxy = std::min(maxy, y + rh - 1); }
  switch (align) {
    case TCOD_LEFT:
      minx = std::max(0,x);
      maxx = std::min(con->w - 1, x + rw - 1);
      break;
    case TCOD_RIGHT:
      minx = std::max(0, x - rw + 1);
      maxx = std::min(con->w - 1, x);
      break;
    case TCOD_CENTER: default:
      minx = std::max(0, x - rw / 2);
      maxx = std::min(con->w - 1, x + rw / 2);
      break;
  }

  do {
    /* get \n delimited sub-message */
    unsigned char *end = TCOD_console_strchr(c, '\n');
    char bak = 0;
    int cl;
    unsigned char *split = NULL;
    if (end) { *end=0; }
    cl = TCOD_console_stringLength(c);
    /* find starting x */
    switch (align) {
      case TCOD_LEFT : cx = x; break;
      case TCOD_RIGHT : cx = x - cl + 1; break;
      case TCOD_CENTER : cx = x - cl / 2; break;
    }
    /* check if the string is completely out of the minx,miny,maxx,maxy frame */
    if (cy >= miny && cy <= maxy && cx <= maxx && cx + cl -1 >= minx) {
      if (can_split && cy <= maxy) {
        /* if partially out of screen, try to split the sub-message */
        if (cx < minx) {
          split = TCOD_console_forward(c, (align == TCOD_CENTER
                                           ? cl - 2 * (minx - cx)
                                           : cl - (minx - cx)));
        } else if (align == TCOD_CENTER) {
          if (cx + cl / 2 > maxx + 1) {
            split = TCOD_console_forward(c, maxx + 1 - cx);
          }
        } else {
          if (cx + cl > maxx + 1) {
            split = TCOD_console_forward(c, maxx + 1 - cx);
          }
        }
      }
      if (split) {
        unsigned char *oldsplit = split;
        while (!isspace(*split) && split > c) { split--; }
        if (end) { *end = '\n'; }
        if (!isspace(*split)) {
          split = oldsplit;
        }
        end = split;
        bak = *split;
        *split = 0;
        cl = TCOD_console_stringLength(c);
        switch (align) {
          case TCOD_LEFT : cx = x; break;
          case TCOD_RIGHT : cx = x - cl + 1; break;
          case TCOD_CENTER : cx = x - cl / 2; break;
        }
      }
      if (cx < minx) {
        /* truncate left part */
        c += minx-cx;
        cl -= minx-cx;
        cx = minx;
      }
      if (cx + cl > maxx + 1) {
        /* truncate right part */
        split = TCOD_console_forward(c, maxx + 1 - cx);
        *split = 0;
      }
      /* render the sub-message */
      if (cy >= 0 && cy < con->h) {
        while (*c) {
          if (*c >= TCOD_COLCTRL_1 && *c <= TCOD_COLCTRL_NUMBER) {
            con->fore = color_control_fore[*c - 1];
            con->back = color_control_back[*c - 1];
          } else if (*c == TCOD_COLCTRL_FORE_RGB) {
            c++;
            con->fore.r = *c++;
            con->fore.g = *c++;
            con->fore.b = *c;
          } else if (*c == TCOD_COLCTRL_BACK_RGB) {
            c++;
            con->back.r = *c++;
            con->back.g = *c++;
            con->back.b = *c;
          } else if (*c == TCOD_COLCTRL_STOP) {
            con->fore = oldFore;
            con->back = oldBack;
          } else {
            if (!count_only) {
              TCOD_console_put_char(con, cx, cy, *c, flag);
            }
            cx++;
          }
          c++;
        }
      }
    }
    if (end) {
      /* next line */
      if (split && ! isspace(bak)) {
        *end = bak;
        c = end;
      } else {
        c = end + 1;
      }
      cy++;
    } else {
      c = NULL;
    }
  } while (c && cy < con->h && (rh == 0 || cy < y + rh));
  return cy - y + 1;
}
#ifndef NO_UNICODE
wchar_t *TCOD_console_strchr_utf(wchar_t *s, char c)
{
  while (*s && *s != c) {
    if (*s == TCOD_COLCTRL_FORE_RGB || *s == TCOD_COLCTRL_BACK_RGB) {
      s += 3;
    }
    s++;
  }
  return (*s ? s : NULL);
}
void TCOD_console_map_string_to_font_utf(const wchar_t *s,
                                         int fontCharX, int fontCharY)
{
  TCOD_IFNOT(s != NULL) return;
  while (*s) {
    TCOD_sys_map_ascii_to_font(*s, fontCharX, fontCharY);
    fontCharX++;
    if (fontCharX == TCOD_ctx.fontNbCharHoriz) {
      fontCharX = 0;
      fontCharY++;
    }
    s++;
  }
}
wchar_t *TCOD_console_vsprint_utf(const wchar_t *fmt, va_list ap)
{
  #define NB_BUFFERS 10
  #define INITIAL_SIZE 512
  /* several static buffers in case the function is used more than once in a single function call */
  static wchar_t *msg[NB_BUFFERS] = {NULL};
  static int buflen[NB_BUFFERS] = {0};
  static int curbuf = 0;
  wchar_t *ret;
  bool ok = false;
  if (!msg[0]) {
    int i;
    for (i = 0; i < NB_BUFFERS; i++) {
      buflen[i] = INITIAL_SIZE;
      msg[i] = static_cast<wchar_t*>(calloc(sizeof(wchar_t), INITIAL_SIZE));
    }
  }
  do {
    /* warning ! depending on the compiler, vsnprintf return -1 or
     the expected string length if the buffer is not big enough */
    int len = vswprintf(msg[curbuf], buflen[curbuf], fmt, ap);
    ok = true;
    if (len < 0 || len >= buflen[curbuf]) {
      /* buffer too small. */
      if (len > 0) {
        while (buflen[curbuf] < len + 1) { buflen[curbuf] *= 2; }
      } else {
        buflen[curbuf] *= 2;
      }
      free(msg[curbuf]);
      msg[curbuf] = static_cast<wchar_t*>(
          calloc(sizeof(wchar_t), buflen[curbuf]));
      ok = false;
    }
  } while (!ok);
  ret = msg[curbuf];
  curbuf = (curbuf + 1) % NB_BUFFERS;
  return ret;
}
int TCOD_console_stringLength_utf(const wchar_t *s)
{
  int l = 0;
  while (*s) {
    if (*s == TCOD_COLCTRL_FORE_RGB || *s == TCOD_COLCTRL_BACK_RGB) {
      s += 3;
    } else if (*s > TCOD_COLCTRL_STOP) {
      l++;
    }
    s++;
  }
  return l;
}
wchar_t * TCOD_console_forward_utf(wchar_t *s,int l)
{
  while (*s && l > 0) {
    if (*s == TCOD_COLCTRL_FORE_RGB || *s == TCOD_COLCTRL_BACK_RGB) {
      s+=3;
    } else if (*s > TCOD_COLCTRL_STOP) {
      l--;
    }
    s++;
  }
  return s;
}
int TCOD_console_print_internal_utf(
    TCOD_Console* con,int x,int y, int rw, int rh, TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t align, wchar_t *msg, bool can_split, bool count_only)
{
  wchar_t *c = msg;
  int cx = 0;
  int cy = y;
  int minx, maxx, miny, maxy;
  TCOD_color_t oldFore;
  TCOD_color_t oldBack;
  con = TCOD_console_validate_(con);
  if (!con) { return 0; }
  if (!TCOD_console_is_index_valid_(con, x, y)) { return 0; }
  TCOD_IFNOT(msg != NULL) { return 0; }
  if (rh == 0) { rh = con->h - y; }
  if (rw == 0) {
    switch(align) {
      case TCOD_LEFT: rw = con->w - x; break;
      case TCOD_RIGHT: rw = x + 1; break;
      case TCOD_CENTER: default: rw = con->w; break;
    }
  }
  oldFore = con->fore;
  oldBack = con->back;
  miny = y;
  maxy = con->h - 1;
  if (rh > 0) maxy = std::min(maxy, y + rh - 1);
  switch (align) {
    case TCOD_LEFT:
      minx = std::max(0,x);
      maxx = std::min(con->w-1,x+rw-1);
      break;
    case TCOD_RIGHT:
      minx = std::max(0, x - rw + 1);
      maxx = std::min(con->w - 1, x);
      break;
    case TCOD_CENTER: default:
      minx = std::max(0, x - rw / 2);
      maxx = std::min(con->w - 1, x + rw / 2);
      break;
  }

  do {
    /* get \n delimited sub-message */
    wchar_t *end = TCOD_console_strchr_utf(c, '\n');
    wchar_t bak = 0;
    int cl;
    wchar_t *split = NULL;
    if (end) { *end = 0; }
    cl = TCOD_console_stringLength_utf(c);
    /* find starting x */
    switch (align) {
      case TCOD_LEFT: cx = x; break;
      case TCOD_RIGHT: cx = x - cl + 1; break;
      case TCOD_CENTER: cx = x - cl / 2; break;
    }
    /* check if the string is completely out of the minx,miny,maxx,maxy frame */
    if (cy >= miny && cy <= maxy && cx <= maxx && cx + cl - 1 >= minx) {
      if (can_split && cy < maxy) {
        /* if partially out of screen, try to split the sub-message */
        if (cx < minx) {
          split = TCOD_console_forward_utf(c, (align == TCOD_CENTER
                                               ? cl - 2 * (minx - cx)
                                               : cl - (minx - cx)));
        } else if (align == TCOD_CENTER) {
          if (cx + cl / 2 > maxx + 1) {
            split = TCOD_console_forward_utf(c, maxx + 1 - cx);
          }
        } else {
          if (cx + cl > maxx + 1) {
            split = TCOD_console_forward_utf(c, maxx + 1 - cx);
          }
        }
      }
      if (split) {
        wchar_t *oldsplit = split;
        while (!iswspace(*split) && split > c) { split--; }
        if (end) { *end='\n'; }
        if (!iswspace(*split)) {
          split = oldsplit;
        }
        end = split;
        bak = *split;
        *split = 0;
        cl = TCOD_console_stringLength_utf(c);
        switch (align) {
          case TCOD_LEFT: cx = x; break;
          case TCOD_RIGHT: cx = x - cl + 1; break;
          case TCOD_CENTER: cx = x - cl / 2; break;
        }
      }
      if (cx < minx) {
        /* truncate left part */
        c += minx - cx;
        cl -= minx - cx;
        cx = minx;
      }
      if (cx + cl > maxx + 1) {
        /* truncate right part */
        split = TCOD_console_forward_utf(c, maxx + 1 - cx);
        *split = 0;
      }
      /* render the sub-message */
      if (cy >= 0 && cy < con->h)
      while (*c) {
        if (*c >= TCOD_COLCTRL_1 && *c <= TCOD_COLCTRL_NUMBER) {
          con->fore = color_control_fore[static_cast<int>(*c) - 1];
          con->back = color_control_back[static_cast<int>(*c) - 1];
        } else if (*c == TCOD_COLCTRL_FORE_RGB) {
          c++;
          con->fore.r = static_cast<uint8_t>(*c++);
          con->fore.g = static_cast<uint8_t>(*c++);
          con->fore.b = static_cast<uint8_t>(*c);
        } else if (*c == TCOD_COLCTRL_BACK_RGB) {
          c++;
          con->back.r = static_cast<uint8_t>(*c++);
          con->back.g = static_cast<uint8_t>(*c++);
          con->back.b = static_cast<uint8_t>(*c);
        } else if (*c == TCOD_COLCTRL_STOP) {
          con->fore = oldFore;
          con->back = oldBack;
        } else {
          if (!count_only) {
            TCOD_console_put_char(con, cx, cy, static_cast<int>(*c), flag);
          }
          cx++;
        }
        c++;
      }
    }
    if (end) {
      /* next line */
      if (split && !iswspace(bak)) {
        *end = bak;
        c = end;
      } else {
        c = end + 1;
      }
      cy++;
    } else {
      c = NULL;
    }
  } while (c && cy < con->h && (rh == 0 || cy < y + rh));
  return cy - y + 1;
}
/**
 *  \rst
 *  .. deprecated:: 1.8
 *    Use :any:`TCOD_console_printf` instead.
 *  \endrst
 */
void TCOD_console_print_utf(TCOD_Console* con, int x, int y,
                            const wchar_t *fmt, ...)
{
  va_list ap;
  con = TCOD_console_validate_(con);
  if (!con) { return; }
  va_start(ap, fmt);
  TCOD_console_print_internal_utf(con, x, y, 0, 0, con->bkgnd_flag,
      con->alignment, TCOD_console_vsprint_utf(fmt, ap), false, false);
  va_end(ap);
}
/**
 *  \rst
 *  .. deprecated:: 1.8
 *    Use :any:`TCOD_console_printf_ex` instead.
 *  \endrst
 */
void TCOD_console_print_ex_utf(
    TCOD_Console* con, int x, int y, TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment, const wchar_t *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  TCOD_console_print_internal_utf(
      con, x, y, 0, 0, flag, alignment, TCOD_console_vsprint_utf(fmt, ap),
      false, false);
  va_end(ap);
}

int TCOD_console_print_rect_utf(TCOD_Console* con, int x, int y, int w, int h,
                                const wchar_t *fmt, ...)
{
  con = TCOD_console_validate_(con);
  if (!con) { return 0; }
  va_list ap;
  va_start(ap, fmt);
  int ret = TCOD_console_print_internal_utf(
      con, x, y, w, h, con->bkgnd_flag, con->alignment,
      TCOD_console_vsprint_utf(fmt, ap), true, false);
  va_end(ap);
  return ret;
}
/**
 *  \rst
 *  .. deprecated:: 1.8
 *    Use :any:`TCOD_console_printf_rect_ex` instead.
 *  \endrst
 */
int TCOD_console_print_rect_ex_utf(
    TCOD_Console* con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment, const wchar_t *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  int ret = TCOD_console_print_internal_utf(
      con, x, y, w, h, flag, alignment,
      TCOD_console_vsprint_utf(fmt, ap), true, false);
  va_end(ap);
  return ret;
}
/**
 *  \rst
 *  .. deprecated:: 1.8
 *  \endrst
 */
int TCOD_console_get_height_rect_utf(
    TCOD_Console* con,int x, int y, int w, int h, const wchar_t *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  int ret = TCOD_console_print_internal_utf(
      con, x, y, w, h, TCOD_BKGND_NONE, TCOD_LEFT,
      TCOD_console_vsprint_utf(fmt, ap), true, true);
  va_end(ap);
  return ret;
}

#endif /* NO_UNICODE */
namespace tcod {
namespace console {
static auto vsprint_(const char *fmt, va_list ap) -> std::string
{
  if (!fmt) { return ""; }
  va_list ap_clone;
  va_copy(ap_clone, ap);
  std::string result(std::vsnprintf(nullptr, 0, fmt, ap_clone), 0);
  va_end(ap_clone);
  std::vsnprintf(&result[0], result.size() + 1, fmt, ap);
  return result;
}
class UnicodeIterator: public std::iterator<std::forward_iterator_tag,
                                            unsigned char> {
 public:
  UnicodeIterator()
  : p_(nullptr), end_(nullptr)
  {}
  UnicodeIterator(const utf8proc_uint8_t* start, const utf8proc_uint8_t* end)
  : p_(start), end_(end)
  {
    parse_unicode();
  }
  UnicodeIterator(const char* start, const char* end)
  : UnicodeIterator(reinterpret_cast<const utf8proc_uint8_t*>(start),
                    reinterpret_cast<const utf8proc_uint8_t*>(end))
  {}
  UnicodeIterator(const std::string& str)
  : UnicodeIterator(str.empty() ? nullptr : &str.front(),
                    str.empty() ? nullptr : &str.back() + 1)
  {}
  UnicodeIterator& operator++()
  {
    if (p_ >= end_) {
      throw std::out_of_range("Moved past the end of the string.");
    }
    p_ += code_size_;
    parse_unicode();
    return *this;
  }
  UnicodeIterator operator++(int)
  {
    UnicodeIterator tmp(*this);
    operator++();
    return tmp;
  }
  bool operator==(const UnicodeIterator& rhs) const noexcept
  {
    return p_ == rhs.p_;
  }
  bool operator!=(const UnicodeIterator& rhs) const noexcept
  {
    return p_ != rhs.p_;
  }
  bool operator<(const UnicodeIterator& rhs) const noexcept
  {
    return p_ < rhs.p_;
  }
  int operator*() const
  {
    if (p_ == end_) {
      throw std::out_of_range("Dereferenced past the end of the iterator.");
    }
    return codepoint_;
  }
  auto get_property() const noexcept -> const utf8proc_property_t*
  {
    return utf8proc_get_property(codepoint_);
  }
  bool empty() const noexcept
  {
    return p_ == end_;
  }
  auto end() const -> const UnicodeIterator
  {
    return UnicodeIterator(end_, end_);
  }
  /*
   *  Check if the specified character is any line-break character
   */
  bool is_newline() const noexcept
  {
    switch (get_property()->category) {
      case UTF8PROC_CATEGORY_ZL: /* Separator, line */
      case UTF8PROC_CATEGORY_ZP: /* Separator, paragraph */
        return true;
      case UTF8PROC_CATEGORY_CC: /* Other, control */
        switch(get_property()->boundclass) {
          case UTF8PROC_BOUNDCLASS_CR:    // carriage return - \r
          case UTF8PROC_BOUNDCLASS_LF:    // line feed - \n
            return true;
          default: break;
        }
        break;
      default: break;
    }
    return false;
  }
 private:
  void parse_unicode()
  {
    if (p_ == end_) {
      codepoint_ = 0;
      code_size_ = 1;
      return;
    }
    code_size_ = utf8proc_iterate(p_, end_ - p_, &codepoint_);
    if (code_size_ < 0) {
      throw std::logic_error(utf8proc_errmsg(code_size_));
    }
  }
  const utf8proc_uint8_t* p_;
  const utf8proc_uint8_t* end_;
  utf8proc_int32_t codepoint_;
  utf8proc_ssize_t code_size_;
};
class FormattedUnicodeIterator: public UnicodeIterator {
 public:
  FormattedUnicodeIterator()
  : UnicodeIterator(), default_fg_(nullptr), default_bg_(nullptr),
    fg_(nullptr), bg_(nullptr)
  {}
  FormattedUnicodeIterator(const std::string& str,
                           const TCOD_color_t* fg, const TCOD_color_t* bg)
  : UnicodeIterator(str), default_fg_(fg), default_bg_(bg), fg_(fg), bg_(bg)
  {
    parse_special_codes();
  }
  FormattedUnicodeIterator(const UnicodeIterator& mit,
                           const TCOD_color_t* fg, const TCOD_color_t* bg)
  : UnicodeIterator(mit), default_fg_(fg), default_bg_(bg), fg_(fg), bg_(bg)
  {
    parse_special_codes();
  }
  FormattedUnicodeIterator(const FormattedUnicodeIterator& rhs)
  : UnicodeIterator(rhs),
    default_fg_(rhs.default_fg_), default_bg_(rhs.default_bg_),
    fg_(rhs.fg_), bg_(rhs.bg_)
  {
    // Avoid pointing to the temporary colors of other iterators.
    if (fg_ == &rhs.temp_fg_) { temp_fg_ = rhs.temp_fg_; fg_ = &temp_fg_; }
    if (bg_ == &rhs.temp_bg_) { temp_bg_ = rhs.temp_bg_; bg_ = &temp_bg_; }
  }
  FormattedUnicodeIterator& operator++()
  {
    UnicodeIterator::operator++();
    parse_special_codes();
    return *this;
  }
  auto get_fg() const noexcept -> const TCOD_color_t*
  {
    return fg_;
  }
  auto get_bg() const noexcept -> const TCOD_color_t*
  {
    return bg_;
  }
 private:
  auto parse_rgb() -> TCOD_color_t {
    TCOD_color_t color;
    UnicodeIterator::operator++();
    color.r = **this;
    UnicodeIterator::operator++();
    color.g = **this;
    UnicodeIterator::operator++();
    color.b = **this;
    return color;
  }
  void parse_special_codes()
  {
    if (*this == (*this).end()) { return; }
    if (TCOD_COLCTRL_1 <= **this && **this <= TCOD_COLCTRL_NUMBER) {
      // Read colors from the color control array.
      int color_index = (**this) - TCOD_COLCTRL_1;
      fg_ = &color_control_fore[color_index];
      bg_ = &color_control_back[color_index];
      ++(*this);
      return;
    } else if (**this == TCOD_COLCTRL_STOP) {
      // Return colors to their original color.
      fg_ = default_fg_;
      bg_ = default_bg_;
      ++(*this);
      return;
    } else if (**this == TCOD_COLCTRL_FORE_RGB) {
      temp_fg_ = parse_rgb();
      fg_ = &temp_fg_;
      ++(*this);
      return;
    } else if (**this == TCOD_COLCTRL_BACK_RGB) {
      temp_bg_ = parse_rgb();
      bg_ = &temp_bg_;
      ++(*this);
      return;
    }
  }
  // Original colors
  const TCOD_color_t* default_fg_;
  const TCOD_color_t* default_bg_;
  // Current active colors.
  const TCOD_color_t* fg_;
  const TCOD_color_t* bg_;
  // Colors generated from RGB codes.
  TCOD_color_t temp_fg_;
  TCOD_color_t temp_bg_;
};
/**
 *  Get the next line-break or null terminator, or break the string before
 *  `max_width`.
 *
 *  Returns {break_point, line_width, status}
 */
static std::tuple<UnicodeIterator, int, int> next_split_(
    FormattedUnicodeIterator it,
    const UnicodeIterator& end,
    int max_width,
    int can_split)
{
  // The break point and width of the line.
  UnicodeIterator break_point(end);
  int break_width = 0;
  // The current line width.
  int char_width = 0;
  bool separating = false; // True if the last iteration was breakable.
  while (it != end) {
    if (can_split && char_width > 0) {
      switch (it.get_property()->category) {
        default:
          if (char_width + it.get_property()->charwidth > max_width) {
            // The next character would go over the max width, so return now.
            if (break_point != end) {
              // Use latest line break if one exists.
              return {break_point, break_width, 1};
            } else {
              // Force a line break here.
              return {it, char_width, 1};
            }
          }
          separating = false;
          break;
        case UTF8PROC_CATEGORY_PD: // Punctuation, dash
          if (char_width + it.get_property()->charwidth > max_width) {
            return {it, char_width, 1};
          } else {
            break_point = it;
            ++break_point;
            break_width = char_width + it.get_property()->charwidth;
            separating = true;
          }
          break;
        case UTF8PROC_CATEGORY_ZS: // Separator, space
          if (!separating) {
            break_point = it;
            break_width = char_width;
            separating = true;
          }
          break;
      }
    }
    if (it.is_newline()) {
      // Always break on newlines.
      return {it, char_width, 0};
    }
    char_width += it.get_property()->charwidth;
    ++it;
  }
  // Return end of iteration.
  return {it, char_width, 0};
}
static int print_internal_(
    TCOD_Console& con,
    int x,
    int y,
    int width,
    int height,
    const std::string& string,
    const TCOD_color_t* fg,
    const TCOD_color_t* bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t align,
    int can_split,
    int count_only)
{
  FormattedUnicodeIterator it(string, fg, bg);
  UnicodeIterator end = it.end();
  if (!can_split && align == TCOD_RIGHT) {
    // In general `can_split = false` is deprecated.
    x -= con.w - 1;
    width = con.w;
  }
  // Expand the width/height of 0 to the edge of the console.
  if (!width) { width = con.w - x; }
  if (!height) { height = con.h - y; }
  // Print bounding box.
  int left = x;
  int right = x + width;
  int top = y;
  int bottom = y + height;
  width = right - left;
  height = bottom - top;
  if (can_split && (width <= 0 || height <= 0)) {
    return 0; // The bounding box is invalid.
  }
  while (it != end && top < bottom && top < con.h) {
    // Check for newlines.
    if(it.is_newline()) {
      if(it.get_property()->category == UTF8PROC_CATEGORY_ZP) {
        top += 2;
      } else {
        top += 1;
      }
      ++it;
      continue;
    }
    // Get the next line of characters.
    UnicodeIterator line_break;
    int line_width;
    int split_status;
    std::tie(line_break, line_width, split_status) =
        next_split_(it, end, width, can_split);
    // Set cursor_x from alignment.
    int cursor_x = 0;
    switch (align) {
      default:
      case TCOD_LEFT:
        cursor_x = left;
        break;
      case TCOD_RIGHT:
        cursor_x = right - line_width;
        break;
      case TCOD_CENTER:
        if (can_split) {
          cursor_x = left + (width - line_width) / 2;
        } else {
          cursor_x = left - (line_width / 2);  // Deprecated.
        }
        break;
    }
    for (; it < line_break; cursor_x += it.get_property()->charwidth, ++it) {
      if(count_only) { continue; }
      if (can_split && (left > cursor_x || cursor_x >= right)) { continue; }
      if (!can_split && (0 > cursor_x || cursor_x >= con.w)) { continue; }
      // Actually render this line of characters.
      put(&con, cursor_x, top, *it, it.get_fg(), it.get_bg(), flag);
    }
    // Ignore any extra spaces.
    while (it != end) {
      // Separator, space
      if (it.get_property()->category != UTF8PROC_CATEGORY_ZS) { break; }
      ++it;
    }
    // If there was an automatic split earlier then the top is moved down.
    if (split_status == 1) { top += 1; }
  }
  return std::min(top, bottom) - y + 1;
}
/**
 *  Normalize rectangle values using old libtcod rules where alignment can move
 *  the rectangle position.
 */
static void normalize_old_rect_(
    TCOD_Console& console,
    TCOD_alignment_t alignment,
    int& x,
    int& y,
    int& width,
    int& height)
{
  // Set default width/height if either is zero.
  if (width == 0) { width = console.w; }
  if (height == 0) { height = console.h - y; }
  switch(alignment) {
    default:
    case TCOD_LEFT:
      break;
    case TCOD_RIGHT:
      x -= width;
      break;
    case TCOD_CENTER:
      x -= width / 2;
      break;
  }
  return;
}
void print(
    TCOD_Console* con,
    int x,
    int y,
    const std::string& str,
    const TCOD_color_t* fg,
    const TCOD_color_t* bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment)
{
  con = TCOD_console_validate_(con);
  if (!con) { return; }
  print_internal_(*con, x, y, con->w, con->h, str, fg, bg, flag, alignment,
                  false, false);
}
int print_rect(
    struct TCOD_Console *con,
    int x,
    int y,
    int width,
    int height,
    const std::string& str,
    const TCOD_color_t* fg,
    const TCOD_color_t* bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment)
{
  con = TCOD_console_validate_(con);
  if (!con) { return 0; }
  return print_internal_(*con, x, y, width, height, str, fg, bg,
                         flag, alignment, true, false);
}
int get_height_rect(
    std::array<int, 2> console_size,
    int x,
    int y,
    int width,
    int height,
    const std::string& str)
{
  struct TCOD_Console console{};
  console.w = console_size[0];
  console.h = console_size[1];
  return print_internal_(console, x, y, width, height, str, nullptr, nullptr,
                         TCOD_BKGND_NONE, TCOD_LEFT, true, true);
}
int get_height_rect(
    int width,
    const std::string& str)
{
  auto MAX_INT = std::numeric_limits<int>::max();
  return get_height_rect({width, MAX_INT}, 0, 0, width, MAX_INT, str);
}
int get_height_rect(
    struct TCOD_Console *con,
    int x,
    int y,
    int width,
    int height,
    const std::string& str)
{
  con = TCOD_console_validate_(con);
  if (!con) { return 0; }
  return get_height_rect({con->w, con->h}, x, y, width, height, str);
}
void print_frame(
    struct TCOD_Console *con,
    int x,
    int y,
    int width,
    int height,
    const std::string& title,
    const TCOD_color_t* fg,
    const TCOD_color_t* bg,
    TCOD_bkgnd_flag_t flag,
    bool empty)
{
  const int left = x;
  const int right = x + width - 1;
  const int top = y;
  const int bottom = y + height - 1;
  con = TCOD_console_validate_(con);
  if (!con) { return; }
  put(con, left, top, 0x250C, fg, bg, flag); // ┌
  put(con, right, top, 0x2510, fg, bg, flag); // ┐
  put(con, left, bottom, 0x2514, fg, bg, flag); // └
  put(con, right, bottom, 0x2518, fg, bg, flag); // ┘
  draw_rect(con, x + 1, y, width - 2, 1,
            0x2500, &con->fore, &con->back, flag); // ─
  draw_rect(con, x + 1, y + height - 1, width - 2, 1,
            0x2500, &con->fore, &con->back, flag);
  draw_rect(con, x, y + 1, 1, height - 2,
            0x2502, &con->fore, &con->back, flag); // │
  draw_rect(con, x + width - 1, y + 1, 1, height - 2,
            0x2502, &con->fore, &con->back, flag);
  if (empty) {
    draw_rect(con, x + 1, y + 1, width -2, height - 2,
              0x20, &con->fore, &con->back, flag);
  }
  if (!title.empty()) {
    print_rect(con, x, y, width, 1,
               " " + title + " ", bg, fg, TCOD_BKGND_SET, TCOD_CENTER);
  }
}
} // namespace console
} // namespace tcod
/**
 *  Format and print a UTF-8 string to a console.
 *  \rst
 *  .. versionadded:: 1.8
 *  \endrst
 */
void TCOD_console_printf_ex(
    TCOD_Console* con,
    int x,
    int y,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment,
    const char *fmt, ...)
{
  con = TCOD_console_validate_(con);
  if (!con) { return; }
  va_list ap;
  va_start(ap, fmt);
  tcod::console::print(con, x, y, tcod::console::vsprint_(fmt, ap),
                       &con->fore, &con->back, flag, alignment);
  va_end(ap);
}
/**
 *  Format and print a UTF-8 string to a console.
 *  \rst
 *  .. versionadded:: 1.8
 *  \endrst
 */
void TCOD_console_printf(TCOD_Console* con, int x, int y, const char *fmt, ...)
{
  con = TCOD_console_validate_(con);
  if (!con) { return; }
  va_list ap;
  va_start(ap, fmt);
  tcod::console::print(
      con, x, y, tcod::console::vsprint_(fmt, ap),
      &con->fore, &con->back, con->bkgnd_flag, con->alignment);
  va_end(ap);
}
/**
 *  Format and print a UTF-8 string to a console.
 *  \rst
 *  .. versionadded:: 1.8
 *  \endrst
 */
int TCOD_console_printf_rect_ex(
    struct TCOD_Console* con,
    int x, int y, int w, int h,
    TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char *fmt, ...)
{
  con = TCOD_console_validate_(con);
  if (!con) { return 0; }
  tcod::console::normalize_old_rect_(*con, alignment, x, y, w, h);
  va_list ap;
  va_start(ap, fmt);
  int ret = tcod::console::print_rect(
      con, x, y, w, h, tcod::console::vsprint_(fmt, ap),
      &con->fore, &con->back, flag, alignment);
  va_end(ap);
  return ret;
}
/**
 *  Format and print a UTF-8 string to a console.
 *  \rst
 *  .. versionadded:: 1.8
 *  \endrst
 */
int TCOD_console_printf_rect(
    struct TCOD_Console* con, int x, int y, int w, int h, const char *fmt, ...)
{
  con = TCOD_console_validate_(con);
  if (!con) { return 0; }
  tcod::console::normalize_old_rect_(*con, con->alignment, x, y, w, h);
  va_list ap;
  va_start(ap, fmt);
  int ret = tcod::console::print_rect(
      con, x, y, w, h, tcod::console::vsprint_(fmt, ap),
      &con->fore, &con->back, con->bkgnd_flag, con->alignment);
  va_end(ap);
  return ret;
}
/**
 *  Return the number of lines that would be printed by this formatted string.
 *  \rst
 *  .. versionadded:: 1.8
 *  \endrst
 */
int TCOD_console_get_height_rect_fmt(
    struct TCOD_Console* con, int x, int y, int w, int h, const char *fmt, ...)
{
  con = TCOD_console_validate_(con);
  if (!con) { return 0; }
  tcod::console::normalize_old_rect_(*con, TCOD_LEFT, x, y, w, h);
  va_list ap;
  va_start(ap, fmt);
  int ret = tcod::console::get_height_rect(con, x, y, w, h,
                                           tcod::console::vsprint_(fmt, ap));
  va_end(ap);
  return ret;
}
/**
 *  Print a framed and optionally titled region to a console, using default
 *  colors and alignment.
 *
 *  This function uses Unicode box-drawing characters and a UTF-8 formatted
 *  string.
 *  \rst
 *  .. versionadded:: 1.8
 *  \endrst
 */
void TCOD_console_printf_frame(struct TCOD_Console *con,
                               int x, int y, int width, int height, int empty,
                               TCOD_bkgnd_flag_t flag, const char *fmt, ...)
{
  con = TCOD_console_validate_(con);
  if (!con) { return; }
  va_list ap;
  va_start(ap, fmt);
  tcod::console::print_frame(
      con, x, y, width, height, tcod::console::vsprint_(fmt, ap),
      &con->fore, &con->back, flag, empty);
  va_end(ap);
}
