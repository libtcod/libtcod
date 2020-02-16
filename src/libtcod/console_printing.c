/* BSD 3-Clause License
 *
 * Copyright © 2008-2020, Jice and the libtcod contributors.
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
#include "console_printing.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#ifndef NO_UNICODE
#include <wchar.h>
#include <wctype.h>
#endif

#include "console_drawing.h"
#include "console.h"
#include "libtcod_int.h"
#include "utility.h"
#include "../vendor/utf8proc/utf8proc.h"
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
      msg[i] = calloc(sizeof(char), INITIAL_SIZE);
    }
  }
  do {
    /* warning ! depending on the compiler, vsnprintf return -1 or
     the expected string length if the buffer is not big enough */
    va_list ap_clone;
    va_copy(ap_clone, ap);
    int len = vsnprintf(msg[curbuf], buflen[curbuf], fmt, ap_clone);
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
      msg[curbuf] = calloc(sizeof(char), buflen[curbuf]);
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
    xs = x + (w - (int)(strlen(title)) - 2) / 2;
    TCOD_color_t tmp;
    tmp = con->fore;
    con->fore = con->back;
    con->back = tmp;
    TCOD_console_print_ex(con, xs, y, TCOD_BKGND_SET, TCOD_LEFT,
                          " %s ", title);
    tmp = con->fore;
    con->fore = con->back;
    con->back = tmp;
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
  unsigned char *c = (unsigned char*)msg;
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
  if (rh > 0) { maxy = MIN(maxy, y + rh - 1); }
  switch (align) {
    case TCOD_LEFT:
      minx = MAX(0,x);
      maxx = MIN(con->w - 1, x + rw - 1);
      break;
    case TCOD_RIGHT:
      minx = MAX(0, x - rw + 1);
      maxx = MIN(con->w - 1, x);
      break;
    case TCOD_CENTER: default:
      minx = MAX(0, x - rw / 2);
      maxx = MIN(con->w - 1, x + rw / 2);
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
      msg[i] = calloc(sizeof(wchar_t), INITIAL_SIZE);
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
      msg[curbuf] = calloc(sizeof(wchar_t), buflen[curbuf]);
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
  if (rh > 0) maxy = MIN(maxy, y + rh - 1);
  switch (align) {
    case TCOD_LEFT:
      minx = MAX(0,x);
      maxx = MIN(con->w-1,x+rw-1);
      break;
    case TCOD_RIGHT:
      minx = MAX(0, x - rw + 1);
      maxx = MIN(con->w - 1, x);
      break;
    case TCOD_CENTER: default:
      minx = MAX(0, x - rw / 2);
      maxx = MIN(con->w - 1, x + rw / 2);
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
          con->fore = color_control_fore[(int)(*c) - 1];
          con->back = color_control_back[(int)(*c) - 1];
        } else if (*c == TCOD_COLCTRL_FORE_RGB) {
          c++;
          con->fore.r = (uint8_t)(*c++);
          con->fore.g = (uint8_t)(*c++);
          con->fore.b = (uint8_t)(*c);
        } else if (*c == TCOD_COLCTRL_BACK_RGB) {
          c++;
          con->back.r = (uint8_t)(*c++);
          con->back.g = (uint8_t)(*c++);
          con->back.b = (uint8_t)(*c);
        } else if (*c == TCOD_COLCTRL_STOP) {
          con->fore = oldFore;
          con->back = oldBack;
        } else {
          if (!count_only) {
            TCOD_console_put_char(con, cx, cy, (int)*c, flag);
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
// ----------------------------------------------------------------------------
// New UTF-8 parser.
/**
 *  Allocates the formatted string to `out` and returns the size.
 *
 *  Returns a negative number on error.
 */
static int vsprint_(char** out, const char *fmt, va_list ap)
{
  if (!fmt) { return -1; }
  va_list ap_clone;
  va_copy(ap_clone, ap);
  int size = vsnprintf(NULL, 0, fmt, ap_clone);
  va_end(ap_clone);
  if (size < 0) { return size; }
  *out = malloc(size + 1);
  if (!*out) { return -1; }
  vsprintf(*out, fmt, ap);
  return size;
}
struct FormattedPrinter {
  const unsigned char* string;
  const unsigned char* end;
  struct TCOD_ColorRGBA fg;
  struct TCOD_ColorRGBA bg;
  const struct TCOD_ColorRGBA default_fg;
  const struct TCOD_ColorRGBA default_bg;
};
static int fp_peak(const struct FormattedPrinter* printer)
{
  int codepoint;
  utf8proc_iterate(printer->string, printer->end - printer->string, &codepoint);
  return codepoint;
}
static int fp_next_raw(struct FormattedPrinter* printer)
{
  int codepoint;
  int len = utf8proc_iterate(printer->string, printer->end - printer->string, &codepoint);
  if (len > 0) { printer->string += len; }
  return codepoint;
}
static struct TCOD_ColorRGBA fp_next_rgba(struct FormattedPrinter* printer)
{
  struct TCOD_ColorRGBA rgb = {
      fp_next_raw(printer),
      fp_next_raw(printer),
      fp_next_raw(printer),
      255,
  };
  return rgb;
}
static void fp_handle_special_codes(struct FormattedPrinter* printer)
{
  while(printer->string < printer->end) {
    int codepoint = fp_peak(printer);
    switch (codepoint) {
      case -1:
        return;
      case TCOD_COLCTRL_STOP:
        fp_next_raw(printer);
        printer->fg = printer->default_fg;
        printer->bg = printer->default_bg;
        break;
      case TCOD_COLCTRL_FORE_RGB:
        fp_next_raw(printer);
        printer->fg = fp_next_rgba(printer);
        break;
      case TCOD_COLCTRL_BACK_RGB:
        fp_next_raw(printer);
        printer->bg = fp_next_rgba(printer);
        break;
      default:
        if (TCOD_COLCTRL_1 <= codepoint && codepoint <= TCOD_COLCTRL_NUMBER) {
          fp_next_raw(printer);
          int color_index = codepoint - TCOD_COLCTRL_1;
          *(TCOD_ColorRGB*)&printer->fg = color_control_fore[color_index];
          printer->fg.a = 255;
          *(TCOD_ColorRGB*)&printer->bg = color_control_back[color_index];
          printer->bg.a = 255;
        } else {
          return;
        }
        break;
    }
  }
}
static int fp_next(struct FormattedPrinter* printer)
{
  fp_handle_special_codes(printer);
  return fp_next_raw(printer);
}
/*
 *  Check if the specified character is any line-break character
 */
static bool is_newline(int codepoint)
{
  const utf8proc_property_t* property = utf8proc_get_property(codepoint);
  switch (property->category) {
      case UTF8PROC_CATEGORY_ZL: /* Separator, line */
      case UTF8PROC_CATEGORY_ZP: /* Separator, paragraph */
        return true;
      case UTF8PROC_CATEGORY_CC: /* Other, control */
        switch(property->boundclass) {
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
/**
 *  Get the next line-break or null terminator, or break the string before
 *  `max_width`.
 *
 *  `break_point` is the pointer to the line end position.
 *
 *  `break_width` is the width of the line.
 *
 *  Returns true if this function is breaking a line, or false where the line
 *  doesn't break or breaks on its own (line a new-line.)
 */
static bool next_split_(
    const struct FormattedPrinter* printer,
    int max_width,
    int can_split,
    const unsigned char** break_point,
    int* break_width)
{
  struct FormattedPrinter it = *printer;
  // The break point and width of the line.
  *break_point = it.end;
  *break_width = 0;
  // The current line width.
  int char_width = 0;
  bool separating = false; // True if the last iteration was breakable.
  while (it.string != it.end) {
    int codepoint = fp_peak(&it);
    const utf8proc_property_t* property = utf8proc_get_property(codepoint);
    if (can_split && char_width > 0) {
      switch (property->category) {
        default:
          if (char_width + (int)property->charwidth > max_width) {
            // The next character would go over the max width, so return now.
            if (*break_point != it.end) {
              // Use latest line break if one exists.
              return 1;
            } else {
              // Force a line break here.
              *break_point = it.string;
              *break_width = char_width;
              return 1;
            }
          }
          separating = false;
          break;
        case UTF8PROC_CATEGORY_PD: // Punctuation, dash
          if (char_width + (int)property->charwidth > max_width) {
            *break_point = it.string;
            *break_width = char_width;
            return 1;
          } else {
            char_width += property->charwidth;
            fp_next(&it);
            *break_point = it.string;
            *break_width = char_width;
            separating = true;
            continue;
          }
          break;
        case UTF8PROC_CATEGORY_ZS: // Separator, space
          if (!separating) {
            *break_point = it.string;
            *break_width = char_width;
            separating = true;
          }
          break;
      }
    }
    if (is_newline(codepoint)) {
      // Always break on newlines.
      *break_point = it.string;
      *break_width = char_width;
      return 0;
    }
    char_width += property->charwidth;
    fp_next(&it);
  }
  // Return end of iteration.
  *break_point = it.string;
  *break_width = char_width;
  return 0;
}
static int print_internal_(
    TCOD_Console* con,
    int x,
    int y,
    int width,
    int height,
    int n,
    const char* string,
    const TCOD_color_t* fg_in,
    const TCOD_color_t* bg_in,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t align,
    int can_split,
    int count_only)
{
  static const TCOD_ColorRGBA color_default = {255, 255, 255, 0};
  TCOD_ColorRGBA fg = color_default;
  TCOD_ColorRGBA bg = color_default;
  if (fg_in) {
    fg.r = fg_in->r;
    fg.g = fg_in->g;
    fg.b = fg_in->b;
    fg.a = 255;
  }
  if (bg_in) {
    bg.r = bg_in->r;
    bg.g = bg_in->g;
    bg.b = bg_in->b;
    bg.a = 255;
  }
  struct FormattedPrinter printer = {
      .string = (const unsigned char*)string,
      .end = (const unsigned char*)string + n,
      .fg = fg,
      .bg = bg,
      .default_fg = fg,
      .default_bg = bg,
  };
  if (!can_split && align == TCOD_RIGHT) {
    // In general `can_split = false` is deprecated.
    x -= con->w - 1;
    width = con->w;
  }
  // Expand the width/height of 0 to the edge of the console.
  if (!width) { width = con->w - x; }
  if (!height) { height = con->h - y; }
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
  while (printer.string != printer.end && top < bottom && top < con->h) {
    int codepoint = fp_peak(&printer);
    const utf8proc_property_t* property = utf8proc_get_property(codepoint);
    // Check for newlines.
    if(is_newline(codepoint)) {
      if(property->category == UTF8PROC_CATEGORY_ZP) {
        top += 2;
      } else {
        top += 1;
      }
      fp_next(&printer);
      continue;
    }
    // Get the next line of characters.
    const unsigned char* line_break;
    int line_width;
    int split_status = next_split_(&printer, width, can_split, &line_break, &line_width);
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
    for (;
         printer.string < line_break;
         cursor_x += utf8proc_get_property(fp_peak(&printer))->charwidth, fp_next(&printer)) {
      if(count_only) { continue; }
      if (can_split && (left > cursor_x || cursor_x >= right)) { continue; }
      if (!can_split && (0 > cursor_x || cursor_x >= con->w)) { continue; }
      // Actually render this line of characters.
      TCOD_ColorRGB* fg_rgb = printer.fg.a ? (TCOD_ColorRGB*)&printer.fg : NULL;
      TCOD_ColorRGB* bg_rgb = printer.bg.a ? (TCOD_ColorRGB*)&printer.bg : NULL;
      TCOD_console_put_rgb(con, cursor_x, top, fp_peak(&printer), fg_rgb, bg_rgb, flag);
    }
    // Ignore any extra spaces.
    while (printer.string != printer.end) {
      // Separator, space
      if (utf8proc_get_property(fp_peak(&printer))->category != UTF8PROC_CATEGORY_ZS) {
        break;
      }
      fp_next(&printer);
    }
    // If there was an automatic split earlier then the top is moved down.
    if (split_status == 1) { top += 1; }
  }
  return MIN(top, bottom) - y + 1;
}
/**
 *  Normalize rectangle values using old libtcod rules where alignment can move
 *  the rectangle position.
 */
static void normalize_old_rect_(
    TCOD_Console* console,
    TCOD_alignment_t alignment,
    int* x,
    int* y,
    int* width,
    int* height)
{
  // Set default width/height if either is zero.
  if (*width == 0) { *width = console->w; }
  if (*height == 0) { *height = console->h - *y; }
  switch(alignment) {
    default:
    case TCOD_LEFT:
      break;
    case TCOD_RIGHT:
      *x -= *width;
      break;
    case TCOD_CENTER:
      *x -= *width / 2;
      break;
  }
  return;
}
void TCOD_console_printn(
    TCOD_Console* con,
    int x,
    int y,
    int n,
    const char* str,
    const TCOD_color_t* fg,
    const TCOD_color_t* bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment)
{
  con = TCOD_console_validate_(con);
  if (!con) { return; }
  print_internal_(con, x, y, con->w, con->h, n, str, fg, bg, flag, alignment, false, false);
}
int TCOD_console_printn_rect(
    TCOD_Console *con,
    int x,
    int y,
    int width,
    int height,
    int n,
    const char* str,
    const TCOD_color_t* fg,
    const TCOD_color_t* bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment)
{
  con = TCOD_console_validate_(con);
  if (!con) { return 0; }
  return print_internal_(con, x, y, width, height, n, str, fg, bg, flag, alignment, true, false);
}

int TCOD_console_get_height_rect_n(
    TCOD_Console *console,
    int x,
    int y,
    int width,
    int height,
    int n,
    const char* str)
{
  console = TCOD_console_validate_(console);
  if (!console) { return 0; }
  return print_internal_(console, x, y, width, height, n, str, NULL, NULL,
                         TCOD_BKGND_NONE, TCOD_LEFT, true, true);
}
int TCOD_console_get_height_rect_wn(
    int width,
    int n,
    const char* str)
{
  TCOD_Console console = { .w = width, .h = INT_MAX };
  return TCOD_console_get_height_rect_n(&console, 0, 0, width, INT_MAX, n, str);
}
void TCOD_console_printn_frame(
    struct TCOD_Console *con,
    int x,
    int y,
    int width,
    int height,
    int n,
    const char* title,
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
  TCOD_console_put_rgb(con, left, top, 0x250C, fg, bg, flag); // ┌
  TCOD_console_put_rgb(con, right, top, 0x2510, fg, bg, flag); // ┐
  TCOD_console_put_rgb(con, left, bottom, 0x2514, fg, bg, flag); // └
  TCOD_console_put_rgb(con, right, bottom, 0x2518, fg, bg, flag); // ┘
  TCOD_console_draw_rect_rgb(
      con, x + 1, y, width - 2, 1, 0x2500, fg, bg, flag); // ─
  TCOD_console_draw_rect_rgb(
      con, x + 1, y + height - 1, width - 2, 1, 0x2500, fg, bg, flag);
  TCOD_console_draw_rect_rgb(
      con, x, y + 1, 1, height - 2, 0x2502, fg, bg, flag); // │
  TCOD_console_draw_rect_rgb(
      con, x + width - 1, y + 1, 1, height - 2, 0x2502, fg, bg, flag);
  if (empty) {
    TCOD_console_draw_rect_rgb(
        con, x + 1, y + 1, width - 2, height - 2, 0x20, fg, bg, flag);
  }
  if (n > 0 && title) {
    char* tmp_string = malloc(n + 2);
    if (!tmp_string) { return; }
    memcpy(&tmp_string[1], title, n);
    tmp_string[0] = ' ';
    tmp_string[n + 1] = ' ';
    TCOD_console_printn_rect(
        con, x, y, width, 1, n + 2, tmp_string, bg, fg, TCOD_BKGND_SET, TCOD_CENTER);
    free(tmp_string);
  }
}
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
  char* str = NULL;
  int len = vsprint_(&str, fmt, ap);
  va_end(ap);
  if (len < 0) { return; }
  TCOD_console_printn(con, x, y, len, str, &con->fore, &con->back, flag, alignment);
  free(str);
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
  char* str = NULL;
  int len = vsprint_(&str, fmt, ap);
  va_end(ap);
  if (len < 0) { return; }
  TCOD_console_printn(con, x, y, len, str, &con->fore, &con->back, con->bkgnd_flag, con->alignment);
  free(str);
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
  normalize_old_rect_(con, alignment, &x, &y, &w, &h);
  va_list ap;
  va_start(ap, fmt);
  char* str = NULL;
  int len = vsprint_(&str, fmt, ap);
  va_end(ap);
  if (len < 0) { return 0; }
  int ret = TCOD_console_printn_rect(con, x, y, w, h, len, str, &con->fore, &con->back, flag, alignment);
  free(str);
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
  normalize_old_rect_(con, con->alignment, &x, &y, &w, &h);
  va_list ap;
  va_start(ap, fmt);
  char* str = NULL;
  int len = vsprint_(&str, fmt, ap);
  va_end(ap);
  if (len < 0) { return 0; }
  int ret = TCOD_console_printn_rect( con, x, y, w, h, len, str, &con->fore, &con->back, con->bkgnd_flag, con->alignment);
  free(str);
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
  normalize_old_rect_(con, TCOD_LEFT, &x, &y, &w, &h);
  va_list ap;
  va_start(ap, fmt);
  char* str = NULL;
  int len = vsprint_(&str, fmt, ap);
  va_end(ap);
  if (len < 0) { return 0; }
  int ret = TCOD_console_get_height_rect_n(con, x, y, w, h, len, str);
  free(str);
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
  char* str = NULL;
  int len = vsprint_(&str, fmt, ap);
  va_end(ap);
  if (len < 0) { return; }
  TCOD_console_printn_frame(con, x, y, width, height, len, str, &con->fore, &con->back, flag, empty);
  free(str);
}
