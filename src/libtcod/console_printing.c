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
#include "console_printing.h"

#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef NO_UNICODE
#include <wchar.h>
#include <wctype.h>
#endif

#ifndef TCOD_NO_UNICODE
#include <utf8proc.h>
#endif  // TCOD_NO_UNICODE

#include "console.h"
#include "console_drawing.h"
#include "libtcod_int.h"
#include "utility.h"

static TCOD_color_t color_control_fore[TCOD_COLCTRL_NUMBER] = {
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}};
static TCOD_color_t color_control_back[TCOD_COLCTRL_NUMBER];
/**
 *  Assign a foreground and background color to a color control index.
 *
 *  \param con Index to change, e.g. `TCOD_COLCTRL_1`
 *  \param fore Foreground color to assign to this index.
 *  \param back Background color to assign to this index.
 */
void TCOD_console_set_color_control(TCOD_colctrl_t con, TCOD_color_t fore, TCOD_color_t back) {
  TCOD_IFNOT(con >= TCOD_COLCTRL_1 && con <= TCOD_COLCTRL_NUMBER) return;
  color_control_fore[con - 1] = fore;
  color_control_back[con - 1] = back;
}
char* TCOD_console_vsprint(const char* fmt, va_list ap) {
#define NB_BUFFERS 10
#define INITIAL_SIZE 512
  /* several static buffers in case the function is used more than once in a single function call */
  static char* msg[NB_BUFFERS] = {NULL};
  static int buflen[NB_BUFFERS] = {0};
  static int current_buf = 0;
  char* ret;
  bool ok = false;
  if (!msg[0]) {
    int i;
    for (i = 0; i < NB_BUFFERS; i++) {
      buflen[i] = INITIAL_SIZE;
      msg[i] = calloc(INITIAL_SIZE, sizeof(char));
    }
  }
  do {
    /* warning ! depending on the compiler, vsnprintf return -1 or
     the expected string length if the buffer is not big enough */
    va_list ap_clone;
    va_copy(ap_clone, ap);
    int len = vsnprintf(msg[current_buf], buflen[current_buf], fmt, ap_clone);
    va_end(ap_clone);
    ok = true;
    if (len < 0 || len >= buflen[current_buf]) {
      /* buffer too small. */
      if (len > 0) {
        while (buflen[current_buf] < len + 1) {
          buflen[current_buf] *= 2;
        }
      } else {
        buflen[current_buf] *= 2;
      }
      free(msg[current_buf]);
      msg[current_buf] = calloc(buflen[current_buf], sizeof(char));
      ok = false;
    }
  } while (!ok);
  ret = msg[current_buf];
  current_buf = (current_buf + 1) % NB_BUFFERS;
  return ret;
}
void TCOD_console_print_frame(
    TCOD_Console* con, int x, int y, int w, int h, bool empty, TCOD_bkgnd_flag_t flag, const char* fmt, ...) {
  con = TCOD_console_validate_(con);
  if (!con) {
    return;
  }
  TCOD_console_put_char(con, x, y, 0x250C, flag);
  TCOD_console_put_char(con, x + w - 1, y, 0x2510, flag);
  TCOD_console_put_char(con, x, y + h - 1, 0x2514, flag);
  TCOD_console_put_char(con, x + w - 1, y + h - 1, 0x2518, flag);
  TCOD_console_hline(con, x + 1, y, w - 2, flag);
  TCOD_console_hline(con, x + 1, y + h - 1, w - 2, flag);
  if (h > 2) {
    TCOD_console_vline(con, x, y + 1, h - 2, flag);
    TCOD_console_vline(con, x + w - 1, y + 1, h - 2, flag);
    if (empty) {
      TCOD_console_rect(con, x + 1, y + 1, w - 2, h - 2, true, flag);
    }
  }
  if (fmt) {
    va_list ap;
    int xs;
    char* title;
    va_start(ap, fmt);
    title = TCOD_console_vsprint(fmt, ap);
    va_end(ap);
    title[w - 3] = 0; /* truncate if needed */
    xs = x + (w - (int)(strlen(title)) - 2) / 2;
    TCOD_color_t tmp;
    tmp = con->fore;
    con->fore = con->back;
    con->back = tmp;
    TCOD_console_print_ex(con, xs, y, TCOD_BKGND_SET, TCOD_LEFT, " %s ", title);
    tmp = con->fore;
    con->fore = con->back;
    con->back = tmp;
  }
}
void TCOD_console_print(TCOD_Console* con, int x, int y, const char* fmt, ...) {
  va_list ap;
  con = TCOD_console_validate_(con);
  if (!con) {
    return;
  }
  va_start(ap, fmt);
  TCOD_console_print_internal(
      con, x, y, 0, 0, con->bkgnd_flag, con->alignment, TCOD_console_vsprint(fmt, ap), false, false);
  va_end(ap);
}
void TCOD_console_print_ex(
    TCOD_Console* con, int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  TCOD_console_print_internal(con, x, y, 0, 0, flag, alignment, TCOD_console_vsprint(fmt, ap), false, false);
  va_end(ap);
}
int TCOD_console_print_rect(TCOD_Console* con, int x, int y, int w, int h, const char* fmt, ...) {
  int ret;
  va_list ap;
  con = TCOD_console_validate_(con);
  if (!con) {
    return 0;
  }
  va_start(ap, fmt);
  ret = TCOD_console_print_internal(
      con, x, y, w, h, con->bkgnd_flag, con->alignment, TCOD_console_vsprint(fmt, ap), true, false);
  va_end(ap);
  return ret;
}
int TCOD_console_print_rect_ex(
    TCOD_Console* con,
    int x,
    int y,
    int w,
    int h,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment,
    const char* fmt,
    ...) {
  int ret;
  va_list ap;
  va_start(ap, fmt);
  ret = TCOD_console_print_internal(con, x, y, w, h, flag, alignment, TCOD_console_vsprint(fmt, ap), true, false);
  va_end(ap);
  return ret;
}
int TCOD_console_get_height_rect(TCOD_Console* con, int x, int y, int w, int h, const char* fmt, ...) {
  int ret;
  va_list ap;
  va_start(ap, fmt);
  ret = TCOD_console_print_internal(
      con, x, y, w, h, TCOD_BKGND_NONE, TCOD_LEFT, TCOD_console_vsprint(fmt, ap), true, true);
  va_end(ap);
  return ret;
}
/* non public methods */
int TCOD_console_stringLength(const unsigned char* s) {
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
unsigned char* TCOD_console_forward(unsigned char* s, int l) {
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
unsigned char* TCOD_console_strchr(unsigned char* s, unsigned char c) {
  while (*s && *s != c) {
    if (*s == TCOD_COLCTRL_FORE_RGB || *s == TCOD_COLCTRL_BACK_RGB) {
      s += 3;
    }
    s++;
  }
  return (*s ? s : NULL);
}

int TCOD_console_print_internal(
    TCOD_Console* con,
    int x,
    int y,
    int rw,
    int rh,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t align,
    char* msg,
    bool can_split,
    bool count_only) {
  unsigned char* c = (unsigned char*)msg;
  int cx = 0;
  int cy = y;
  int minx, maxx, miny, maxy;
  TCOD_color_t oldFore;
  TCOD_color_t oldBack;
  con = TCOD_console_validate_(con);
  if (!con) {
    return 0;
  }
  TCOD_IFNOT(TCOD_console_is_index_valid_(con, x, y)) { return 0; }
  TCOD_IFNOT(msg != NULL) { return 0; }
  if (rh == 0) {
    rh = con->h - y;
  }
  if (rw == 0) {
    switch (align) {
      case TCOD_LEFT:
        rw = con->w - x;
        break;
      case TCOD_RIGHT:
        rw = x + 1;
        break;
      case TCOD_CENTER:
      default:
        rw = con->w;
        break;
    }
  }
  oldFore = con->fore;
  oldBack = con->back;
  miny = y;
  maxy = con->h - 1;
  if (rh > 0) {
    maxy = TCOD_MIN(maxy, y + rh - 1);
  }
  switch (align) {
    case TCOD_LEFT:
      minx = TCOD_MAX(0, x);
      maxx = TCOD_MIN(con->w - 1, x + rw - 1);
      break;
    case TCOD_RIGHT:
      minx = TCOD_MAX(0, x - rw + 1);
      maxx = TCOD_MIN(con->w - 1, x);
      break;
    case TCOD_CENTER:
    default:
      minx = TCOD_MAX(0, x - rw / 2);
      maxx = TCOD_MIN(con->w - 1, x + rw / 2);
      break;
  }

  do {
    /* get \n delimited sub-message */
    unsigned char* end = TCOD_console_strchr(c, '\n');
    char bak = 0;
    int cl;
    unsigned char* split = NULL;
    if (end) {
      *end = 0;
    }
    cl = TCOD_console_stringLength(c);
    /* find starting x */
    switch (align) {
      case TCOD_LEFT:
        cx = x;
        break;
      case TCOD_RIGHT:
        cx = x - cl + 1;
        break;
      case TCOD_CENTER:
        cx = x - cl / 2;
        break;
    }
    /* check if the string is completely out of the minx,miny,maxx,maxy frame */
    if (cy >= miny && cy <= maxy && cx <= maxx && cx + cl - 1 >= minx) {
      if (can_split && cy <= maxy) {
        /* if partially out of screen, try to split the sub-message */
        if (cx < minx) {
          split = TCOD_console_forward(c, (align == TCOD_CENTER ? cl - 2 * (minx - cx) : cl - (minx - cx)));
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
        unsigned char* old_split = split;
        while (!isspace(*split) && split > c) {
          split--;
        }
        if (end) {
          *end = '\n';
        }
        if (!isspace(*split)) {
          split = old_split;
        }
        end = split;
        bak = *split;
        *split = 0;
        cl = TCOD_console_stringLength(c);
        switch (align) {
          case TCOD_LEFT:
            cx = x;
            break;
          case TCOD_RIGHT:
            cx = x - cl + 1;
            break;
          case TCOD_CENTER:
            cx = x - cl / 2;
            break;
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
      if (split && !isspace(bak)) {
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
wchar_t* TCOD_console_strchr_utf(wchar_t* s, char c) {
  while (*s && *s != c) {
    if (*s == TCOD_COLCTRL_FORE_RGB || *s == TCOD_COLCTRL_BACK_RGB) {
      s += 3;
    }
    s++;
  }
  return (*s ? s : NULL);
}
wchar_t* TCOD_console_vsprint_utf(const wchar_t* fmt, va_list ap) {
#define NB_BUFFERS 10
#define INITIAL_SIZE 512
  /* several static buffers in case the function is used more than once in a single function call */
  static wchar_t* msg[NB_BUFFERS] = {NULL};
  static int buflen[NB_BUFFERS] = {0};
  static int current_buf = 0;
  wchar_t* ret;
  bool ok = false;
  if (!msg[0]) {
    int i;
    for (i = 0; i < NB_BUFFERS; i++) {
      buflen[i] = INITIAL_SIZE;
      msg[i] = calloc(INITIAL_SIZE, sizeof(wchar_t));
    }
  }
  do {
    /* warning ! depending on the compiler, vsnprintf return -1 or
     the expected string length if the buffer is not big enough */
    int len = vswprintf(msg[current_buf], buflen[current_buf], fmt, ap);
    ok = true;
    if (len < 0 || len >= buflen[current_buf]) {
      /* buffer too small. */
      if (len > 0) {
        while (buflen[current_buf] < len + 1) {
          buflen[current_buf] *= 2;
        }
      } else {
        buflen[current_buf] *= 2;
      }
      free(msg[current_buf]);
      msg[current_buf] = calloc(buflen[current_buf], sizeof(wchar_t));
      ok = false;
    }
  } while (!ok);
  ret = msg[current_buf];
  current_buf = (current_buf + 1) % NB_BUFFERS;
  return ret;
}
int TCOD_console_stringLength_utf(const wchar_t* s) {
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
wchar_t* TCOD_console_forward_utf(wchar_t* s, int l) {
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
int TCOD_console_print_internal_utf(
    TCOD_Console* con,
    int x,
    int y,
    int rw,
    int rh,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t align,
    wchar_t* msg,
    bool can_split,
    bool count_only) {
  wchar_t* c = msg;
  int cx = 0;
  int cy = y;
  int minx, maxx, miny, maxy;
  TCOD_color_t oldFore;
  TCOD_color_t oldBack;
  con = TCOD_console_validate_(con);
  if (!con) {
    return 0;
  }
  if (!TCOD_console_is_index_valid_(con, x, y)) {
    return 0;
  }
  TCOD_IFNOT(msg != NULL) { return 0; }
  if (rh == 0) {
    rh = con->h - y;
  }
  if (rw == 0) {
    switch (align) {
      case TCOD_LEFT:
        rw = con->w - x;
        break;
      case TCOD_RIGHT:
        rw = x + 1;
        break;
      case TCOD_CENTER:
      default:
        rw = con->w;
        break;
    }
  }
  oldFore = con->fore;
  oldBack = con->back;
  miny = y;
  maxy = con->h - 1;
  if (rh > 0) maxy = TCOD_MIN(maxy, y + rh - 1);
  switch (align) {
    case TCOD_LEFT:
      minx = TCOD_MAX(0, x);
      maxx = TCOD_MIN(con->w - 1, x + rw - 1);
      break;
    case TCOD_RIGHT:
      minx = TCOD_MAX(0, x - rw + 1);
      maxx = TCOD_MIN(con->w - 1, x);
      break;
    case TCOD_CENTER:
    default:
      minx = TCOD_MAX(0, x - rw / 2);
      maxx = TCOD_MIN(con->w - 1, x + rw / 2);
      break;
  }

  do {
    /* get \n delimited sub-message */
    wchar_t* end = TCOD_console_strchr_utf(c, '\n');
    wchar_t bak = 0;
    int cl;
    wchar_t* split = NULL;
    if (end) {
      *end = 0;
    }
    cl = TCOD_console_stringLength_utf(c);
    /* find starting x */
    switch (align) {
      case TCOD_LEFT:
        cx = x;
        break;
      case TCOD_RIGHT:
        cx = x - cl + 1;
        break;
      case TCOD_CENTER:
        cx = x - cl / 2;
        break;
    }
    /* check if the string is completely out of the minx,miny,maxx,maxy frame */
    if (cy >= miny && cy <= maxy && cx <= maxx && cx + cl - 1 >= minx) {
      if (can_split && cy < maxy) {
        /* if partially out of screen, try to split the sub-message */
        if (cx < minx) {
          split = TCOD_console_forward_utf(c, (align == TCOD_CENTER ? cl - 2 * (minx - cx) : cl - (minx - cx)));
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
        wchar_t* old_split = split;
        while (!iswspace(*split) && split > c) {
          split--;
        }
        if (end) {
          *end = '\n';
        }
        if (!iswspace(*split)) {
          split = old_split;
        }
        end = split;
        bak = *split;
        *split = 0;
        cl = TCOD_console_stringLength_utf(c);
        switch (align) {
          case TCOD_LEFT:
            cx = x;
            break;
          case TCOD_RIGHT:
            cx = x - cl + 1;
            break;
          case TCOD_CENTER:
            cx = x - cl / 2;
            break;
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
void TCOD_console_print_utf(TCOD_Console* con, int x, int y, const wchar_t* fmt, ...) {
  va_list ap;
  con = TCOD_console_validate_(con);
  if (!con) {
    return;
  }
  va_start(ap, fmt);
  TCOD_console_print_internal_utf(
      con, x, y, 0, 0, con->bkgnd_flag, con->alignment, TCOD_console_vsprint_utf(fmt, ap), false, false);
  va_end(ap);
}
void TCOD_console_print_ex_utf(
    TCOD_Console* con, int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const wchar_t* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  TCOD_console_print_internal_utf(con, x, y, 0, 0, flag, alignment, TCOD_console_vsprint_utf(fmt, ap), false, false);
  va_end(ap);
}

int TCOD_console_print_rect_utf(TCOD_Console* con, int x, int y, int w, int h, const wchar_t* fmt, ...) {
  con = TCOD_console_validate_(con);
  if (!con) {
    return 0;
  }
  va_list ap;
  va_start(ap, fmt);
  int ret = TCOD_console_print_internal_utf(
      con, x, y, w, h, con->bkgnd_flag, con->alignment, TCOD_console_vsprint_utf(fmt, ap), true, false);
  va_end(ap);
  return ret;
}
int TCOD_console_print_rect_ex_utf(
    TCOD_Console* con,
    int x,
    int y,
    int w,
    int h,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment,
    const wchar_t* fmt,
    ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret =
      TCOD_console_print_internal_utf(con, x, y, w, h, flag, alignment, TCOD_console_vsprint_utf(fmt, ap), true, false);
  va_end(ap);
  return ret;
}
int TCOD_console_get_height_rect_utf(TCOD_Console* con, int x, int y, int w, int h, const wchar_t* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = TCOD_console_print_internal_utf(
      con, x, y, w, h, TCOD_BKGND_NONE, TCOD_LEFT, TCOD_console_vsprint_utf(fmt, ap), true, true);
  va_end(ap);
  return ret;
}

#endif /* NO_UNICODE */
#ifndef TCOD_NO_UNICODE
// ----------------------------------------------------------------------------
// New UTF-8 parser.
/**
 *  Allocates the formatted string to `out` and returns the size.
 *
 *  Returns a negative number on error.
 */
static int vsprint_(char** out, const char* fmt, va_list ap) {
  if (!fmt) {
    return -1;
  }
  va_list ap_clone;
  va_copy(ap_clone, ap);
  int size = vsnprintf(NULL, 0, fmt, ap_clone);
  va_end(ap_clone);
  if (size < 0) {
    return size;
  }
  *out = malloc(size + 1);
  if (!*out) {
    return -1;
  }
  vsprintf(*out, fmt, ap);
  return size;
}
typedef struct FormattedPrinter {
  const unsigned char* __restrict string;
  const unsigned char* end;
  struct TCOD_ColorRGBA fg;
  struct TCOD_ColorRGBA bg;
  const struct TCOD_ColorRGBA default_fg;
  const struct TCOD_ColorRGBA default_bg;
} FormattedPrinter;
static TCOD_Error utf8_report_error(utf8proc_ssize_t err) {
  switch (err) {
    case UTF8PROC_ERROR_NOMEM:
      TCOD_set_errorv("Out of memory while parsing a UTF-8 string.");
      return TCOD_E_OUT_OF_MEMORY;
    case UTF8PROC_ERROR_INVALIDUTF8:
      TCOD_set_errorv("UTF-8 string is malformed.");
      return TCOD_E_ERROR;
    default:
      if (err < 0) {
        TCOD_set_errorvf("Unexpected error while processing UTF-8 string: %d", (int)err);
        return TCOD_E_ERROR;
      }
      return TCOD_E_OK;
  }
}
/**
    Set `*out` to the next codepoint and advance the string pointer.

    An error may be returned.  `out` can be NULL.
 */
TCOD_NODISCARD
static TCOD_Error fp_next_raw(FormattedPrinter* __restrict printer, int* __restrict out) {
  int codepoint;
  utf8proc_ssize_t len = utf8proc_iterate(printer->string, printer->end - printer->string, &codepoint);
  if (len < 0) {
    return utf8_report_error(len);
  }
  printer->string += len;
  if (out) {
    *out = codepoint;
  }
  return TCOD_E_OK;
}
/**
    Output the next 3 codepoints to a TCOD_ColorRGBA struct and advance.
 */
TCOD_NODISCARD
static TCOD_Error fp_next_rgba(FormattedPrinter* __restrict printer, struct TCOD_ColorRGBA* __restrict out) {
  int r, g, b;
  TCOD_Error err;
  if ((err = fp_next_raw(printer, &r)) < 0 || (err = fp_next_raw(printer, &g)) < 0 ||
      (err = fp_next_raw(printer, &b)) < 0) {
    return err;
  }
  if (out) {
    *out = (struct TCOD_ColorRGBA){(uint8_t)r, (uint8_t)g, (uint8_t)b, 255};
  }
  return TCOD_E_OK;
}
/**
    Apply and special formatting codes to this printer and advance.

    Special codes are the libtcod color formatting codes, these change the state
    if the FormattedPrinter struct.

    If the string ends with formatting with no plain characters afterwards
    then `*out` is set to a zero-width space.
 */
TCOD_NODISCARD
static TCOD_Error fp_next(FormattedPrinter* __restrict printer, int* __restrict out) {
  do {
    int codepoint;
    TCOD_Error err = fp_next_raw(printer, &codepoint);
    if (err < 0) {
      return err;
    }
    switch (codepoint) {
      case TCOD_COLCTRL_STOP:
        printer->fg = printer->default_fg;
        printer->bg = printer->default_bg;
        break;
      case TCOD_COLCTRL_FORE_RGB:
        if ((err = fp_next_rgba(printer, &printer->fg)) < 0) {
          return err;
        }
        break;
      case TCOD_COLCTRL_BACK_RGB:
        if ((err = fp_next_rgba(printer, &printer->bg)) < 0) {
          return err;
        }
        break;
      default:
        if (TCOD_COLCTRL_1 <= codepoint && codepoint <= TCOD_COLCTRL_NUMBER) {
          int color_index = codepoint - TCOD_COLCTRL_1;
          *(TCOD_ColorRGB*)&printer->fg = color_control_fore[color_index];
          printer->fg.a = 255;
          *(TCOD_ColorRGB*)&printer->bg = color_control_back[color_index];
          printer->bg.a = 255;
        } else {
          // Non-formatting character.
          if (out) {
            *out = codepoint;
          }
          return TCOD_E_OK;
        }
        break;
    }
  } while (printer->string < printer->end);
  // The string ended with formatting without a plain character afterwards.
  if (out) {
    *out = 0x200B;  // Zero-width space.
  }
  return TCOD_E_OK;
}
/**
    Return the next non-special codepoint without advancing the string pointer.
 */
TCOD_NODISCARD
static TCOD_Error fp_peek(const FormattedPrinter* __restrict printer, int* __restrict out) {
  FormattedPrinter temp = *printer;
  return fp_next(&temp, out);
}
/*
    Check if the specified character is any line-break character
 */
TCOD_NODISCARD
static bool is_newline(int codepoint) {
  const utf8proc_property_t* property = utf8proc_get_property(codepoint);
  switch (property->category) {
    case UTF8PROC_CATEGORY_ZL: /* Separator, line */
    case UTF8PROC_CATEGORY_ZP: /* Separator, paragraph */
      return true;
    case UTF8PROC_CATEGORY_CC: /* Other, control */
      switch (property->boundclass) {
        case UTF8PROC_BOUNDCLASS_CR:  // carriage return - \r
        case UTF8PROC_BOUNDCLASS_LF:  // line feed - \n
          return true;
        default:
          break;
      }
      break;
    default:
      break;
  }
  return false;
}
/**
    A variable that toggles double wide character handing in print functions.

    In theory this option could be made public.
 */
static const bool TCOD_double_width_print_mode = 0;
/**
    Return the tile-width for this character.

    The result for normally double-width characters will depend on
    `TCOD_double_width_print_mode`.
 */
TCOD_NODISCARD
static int get_character_width(int codepoint) {
  const utf8proc_property_t* property = utf8proc_get_property(codepoint);
  if (property->category == UTF8PROC_CATEGORY_CO) {  // Private Use Area.
    return 1;  // Width would otherwise be zero.
  }
  switch (property->charwidth) {
    default:
      return (int)property->charwidth;
    case 2:
      return TCOD_double_width_print_mode ? 2 : 1;
  }
}
/**
    Get the next line-break or null terminator, or break the string before
    `max_width`.

    `break_point` is the pointer to the line end position.

    `break_width` is the width of the line.

    `add_line_break` will be set to true if this function is breaking a line,
    or false where the line doesn't break or breaks on its own.
 */
TCOD_NODISCARD
static TCOD_Error next_split_(
    const FormattedPrinter* __restrict printer,
    int max_width,
    int can_split,
    const unsigned char** __restrict break_point,
    int* __restrict break_width,
    bool* __restrict add_line_break) {
  FormattedPrinter it = *printer;
  // The break point and width of the line.
  *break_point = it.end;
  *break_width = 0;
  *add_line_break = false;
  // The current line width.
  int char_width = 0;
  bool separating = false;  // True if the last iteration was breakable.
  while (it.string != it.end) {
    int codepoint;
    TCOD_Error err;
    if ((err = fp_peek(&it, &codepoint)) < 0) {
      return err;
    }
    const utf8proc_property_t* property = utf8proc_get_property(codepoint);
    if (can_split && char_width > 0) {
      switch (property->category) {
        default:
          if (char_width + get_character_width(codepoint) > max_width) {
            // The next character would go over the max width, so return now.
            if (*break_point != it.end) {
              // Use latest line break if one exists.
              *add_line_break = true;
              return 1;
            } else {
              // Force a line break here.
              *break_point = it.string;
              *break_width = char_width;
              *add_line_break = true;
              return TCOD_E_OK;
            }
          }
          separating = false;
          break;
        case UTF8PROC_CATEGORY_PD:  // Punctuation, dash
          if (char_width + get_character_width(codepoint) > max_width) {
            *break_point = it.string;
            *break_width = char_width;
            *add_line_break = true;
            return TCOD_E_OK;
          } else {
            char_width += get_character_width(codepoint);
            if ((err = fp_next(&it, NULL)) < 0) {
              return err;
            }
            *break_point = it.string;
            *break_width = char_width;
            separating = true;
            continue;
          }
          break;
        case UTF8PROC_CATEGORY_ZS:  // Separator, space
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
      return TCOD_E_OK;
    }
    char_width += get_character_width(codepoint);
    if ((err = fp_next(&it, NULL)) < 0) {
      return err;
    }
  }
  // Return end of iteration.
  *break_point = it.string;
  *break_width = char_width;
  return TCOD_E_OK;
}
/**
    A parameters struct for internal printing functions.
 */
typedef struct PrintParams {
  TCOD_Console* __restrict console;  // Can not be NULL.
  int x;  // Cursor starting position.
  int y;
  int width;
  int height;
  const TCOD_ColorRGB* __restrict rgb_fg;  // Can be NULL.
  const TCOD_ColorRGB* __restrict rgb_bg;
  TCOD_bkgnd_flag_t flag;
  TCOD_alignment_t alignment;
  bool can_split;  // In general `can_split = false` is deprecated.
  bool count_only;  // True if console is read-only.
} PrintParams;
TCOD_NODISCARD
static int printn_internal_(const PrintParams* __restrict params, size_t n, const char* __restrict string) {
  if (!params->console) {
    TCOD_set_errorv("Console pointer must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (n <= 0) {
    return TCOD_E_OK;
  }
  int x = params->x;  // Cursor position.
  int y = params->y;
  int width = params->width;  // Bounds size, used to calculate the boundary.
  int height = params->height;
  static const TCOD_ColorRGBA color_default = {255, 255, 255, 0};
  TCOD_ColorRGBA fg = color_default;
  TCOD_ColorRGBA bg = color_default;
  if (params->rgb_fg) {
    fg = (TCOD_ColorRGBA){params->rgb_fg->r, params->rgb_fg->g, params->rgb_fg->b, 255};
  }
  if (params->rgb_bg) {
    bg = (TCOD_ColorRGBA){params->rgb_bg->r, params->rgb_bg->g, params->rgb_bg->b, 255};
  }
  FormattedPrinter printer = {
      .string = (const unsigned char*)string,
      .end = (const unsigned char*)string + n,
      .fg = fg,
      .bg = bg,
      .default_fg = fg,
      .default_bg = bg,
  };
  if (!params->can_split) {
    // Default sizes for `can_split = false`.
    width = width ? width : params->console->w;
    height = height ? height : params->console->h;
  }
  if (!params->can_split && params->alignment == TCOD_RIGHT) {
    x -= params->console->w - 1;
    width = params->console->w;
  }
  // Expand the width/height of 0 to the edge of the console.
  width = width ? width : params->console->w - x;
  height = height ? height : params->console->h - y;
  // Print bounding box.
  int left = x;
  int right = x + width;
  int top = y;
  int bottom = y + height;
  width = right - left;
  height = bottom - top;
  if (params->can_split && (width <= 0 || height <= 0)) {
    return 0;  // The bounding box is invalid.
  }
  while (printer.string != printer.end && top < bottom && top < params->console->h) {
    int codepoint;
    TCOD_Error err = fp_peek(&printer, &codepoint);
    if (err < 0) {
      return err;
    }
    const utf8proc_property_t* property = utf8proc_get_property(codepoint);
    // Check for newlines.
    if (is_newline(codepoint)) {
      if (property->category == UTF8PROC_CATEGORY_ZP) {
        top += 2;
      } else {
        top += 1;
      }
      if ((err = fp_next(&printer, NULL)) < 0) {
        return err;
      }
      continue;
    }
    // Get the next line of characters.
    const unsigned char* line_break;
    int line_width;
    bool add_line_break;
    if ((err = next_split_(&printer, width, params->can_split, &line_break, &line_width, &add_line_break)) < 0) {
      return err;
    }
    // Set cursor_x from alignment.
    int cursor_x = 0;
    switch (params->alignment) {
      default:
      case TCOD_LEFT:
        cursor_x = left;
        break;
      case TCOD_RIGHT:
        cursor_x = right - line_width;
        break;
      case TCOD_CENTER:
        if (params->can_split) {
          cursor_x = left + (width - line_width) / 2;
        } else {
          cursor_x = left - (line_width / 2);  // Deprecated.
        }
        break;
    }
    // True clipping area.  Prevent drawing outside of these bounds.
    int clip_left = left;
    int clip_right = right;
    if (!params->can_split) {
      // Bounds are ignored if splitting is off.
      clip_left = 0;
      clip_right = params->console->w;
    }
    while (printer.string < line_break) {
      // Iterate over a line of characters.
      if ((err = fp_next(&printer, &codepoint)) < 0) {  // Advances printer.string.
        return err;
      }
      if (params->count_only) {
        continue;  // Console is read-only.
      }
      if (get_character_width(codepoint) == 0) {
        continue;
      }
      if (clip_left <= cursor_x && cursor_x < clip_right) {
        // Actually render this line of characters.
        TCOD_ColorRGB* fg_rgb = printer.fg.a ? (TCOD_ColorRGB*)&printer.fg : NULL;
        TCOD_ColorRGB* bg_rgb = printer.bg.a ? (TCOD_ColorRGB*)&printer.bg : NULL;
        TCOD_console_put_rgb(params->console, cursor_x, top, codepoint, fg_rgb, bg_rgb, params->flag);
      }
      cursor_x += get_character_width(codepoint);
    }
    // Ignore any extra spaces.
    while (printer.string != printer.end) {
      // Separator, space
      if ((err = fp_peek(&printer, &codepoint)) < 0) {
        return err;
      }
      if (utf8proc_get_property(codepoint)->category != UTF8PROC_CATEGORY_ZS) {
        break;
      }
      if ((err = fp_next(&printer, NULL)) < 0) {
        return err;
      }
    }
    // If there was an automatic split earlier then the top is moved down.
    if (add_line_break) {
      top += 1;
    }
  }
  return TCOD_MIN(top, bottom) - y + 1;
}
TCOD_NODISCARD
static int vprintf_internal_(const PrintParams* __restrict params, const char* __restrict fmt, va_list args) {
  char stack_buffer[512];  // This buffer attempts to prevent a heap allocation.
  char* heap_buffer = NULL;  // Heap buffer to hold the result in case of overflow.
  char* active_buffer = stack_buffer;  // Which buffer will be passed down.
  va_list args_copy;
  va_copy(args_copy, args);
  int str_length = vsnprintf(stack_buffer, sizeof(stack_buffer), fmt, args_copy);
  va_end(args_copy);
  if (str_length >= (int)sizeof(stack_buffer)) {
    active_buffer = heap_buffer = malloc(str_length + 1);
    if (!heap_buffer) {
      TCOD_set_errorv("Out of memory.");
      return TCOD_E_OUT_OF_MEMORY;
    }
    str_length = vsnprintf(heap_buffer, str_length + 1, fmt, args);
  }
  if (str_length < 0) {
    TCOD_set_errorvf("vsnprintf error: %i", str_length);
    free(heap_buffer);
    return TCOD_E_ERROR;
  }
  int err = printn_internal_(params, str_length, active_buffer);
  free(heap_buffer);
  return err;
}
/**
 *  Normalize rectangle values using old libtcod rules where alignment can move
 *  the rectangle position.
 */
static void normalize_old_rect_(
    TCOD_Console* __restrict console,
    TCOD_alignment_t alignment,
    int* __restrict x,
    int* __restrict y,
    int* __restrict width,
    int* __restrict height) {
  // Set default width/height if either is zero.
  if (*width == 0) {
    *width = console->w;
  }
  if (*height == 0) {
    *height = console->h - *y;
  }
  switch (alignment) {
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
TCOD_Error TCOD_console_printn(
    TCOD_Console* __restrict con,
    int x,
    int y,
    size_t n,
    const char* __restrict str,
    const TCOD_color_t* __restrict fg,
    const TCOD_color_t* __restrict bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment) {
  const PrintParams params = {
      .console = TCOD_console_validate_(con),
      .x = x,
      .y = y,
      .width = 0,
      .height = 0,
      .rgb_fg = fg,
      .rgb_bg = bg,
      .flag = flag,
      .alignment = alignment,
      .can_split = false,
      .count_only = false,
  };
  int err = printn_internal_(&params, n, str);
  return err < 0 ? (TCOD_Error)err : TCOD_E_OK;
}
int TCOD_console_printn_rect(
    TCOD_Console* __restrict con,
    int x,
    int y,
    int width,
    int height,
    size_t n,
    const char* __restrict str,
    const TCOD_color_t* __restrict fg,
    const TCOD_color_t* __restrict bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment) {
  const PrintParams params = {
      .console = TCOD_console_validate_(con),
      .x = x,
      .y = y,
      .width = width,
      .height = height,
      .rgb_fg = fg,
      .rgb_bg = bg,
      .flag = flag,
      .alignment = alignment,
      .can_split = true,
      .count_only = false,
  };
  return printn_internal_(&params, n, str);
}
int TCOD_console_get_height_rect_n(
    TCOD_Console* console, int x, int y, int width, int height, size_t n, const char* __restrict str) {
  const PrintParams params = {
      .console = TCOD_console_validate_(console),
      .x = x,
      .y = y,
      .width = width,
      .height = height,
      .flag = TCOD_BKGND_NONE,
      .alignment = TCOD_LEFT,
      .can_split = true,
      .count_only = true,
  };
  return printn_internal_(&params, n, str);
}
int TCOD_console_get_height_rect_wn(int width, size_t n, const char* __restrict str) {
  TCOD_Console console = {.w = width, .h = INT_MAX};  // get_height functions don't need a fully defined console.
  return TCOD_console_get_height_rect_n(&console, 0, 0, width, INT_MAX, n, str);
}
TCOD_Error TCOD_console_printn_frame(
    struct TCOD_Console* __restrict con,
    int x,
    int y,
    int width,
    int height,
    size_t n,
    const char* __restrict title,
    const TCOD_color_t* __restrict fg,
    const TCOD_color_t* __restrict bg,
    TCOD_bkgnd_flag_t flag,
    bool clear) {
  con = TCOD_console_validate_(con);
  if (!con) {
    TCOD_set_errorv("Console pointer must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  TCOD_console_draw_frame_rgb(con, x, y, width, height, NULL, fg, bg, flag, clear);
  if (n > 0 && title) {
    char* tmp_string = malloc(n + 2);
    if (!tmp_string) {
      TCOD_set_errorv("Out of memory.");
      return TCOD_E_OUT_OF_MEMORY;
    }
    memcpy(&tmp_string[1], title, n);
    tmp_string[0] = ' ';
    tmp_string[n + 1] = ' ';
    int err = TCOD_console_printn_rect(con, x, y, width, 1, n + 2, tmp_string, bg, fg, TCOD_BKGND_SET, TCOD_CENTER);
    free(tmp_string);
    if (err < 0) {
      return (TCOD_Error)err;
    }
  }
  return TCOD_E_OK;
}
TCOD_Error TCOD_console_vprintf(
    TCOD_Console* __restrict console,
    int x,
    int y,
    const TCOD_color_t* __restrict fg,
    const TCOD_color_t* __restrict bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment,
    const char* __restrict fmt,
    va_list args) {
  const PrintParams params = {
      .console = TCOD_console_validate_(console),
      .x = x,
      .y = y,
      .rgb_fg = fg,
      .rgb_bg = bg,
      .flag = flag,
      .alignment = alignment,
      .can_split = false,
  };
  int err = vprintf_internal_(&params, fmt, args);
  return err < 0 ? (TCOD_Error)err : TCOD_E_OK;
}
TCOD_Error TCOD_console_printf_ex(
    TCOD_Console* __restrict con,
    int x,
    int y,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment,
    const char* __restrict fmt,
    ...) {
  con = TCOD_console_validate_(con);
  if (!con) {
    TCOD_set_errorv("Console pointer must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  va_list args;
  va_start(args, fmt);
  TCOD_Error err = TCOD_console_vprintf(con, x, y, &con->fore, &con->back, flag, alignment, fmt, args);
  va_end(args);
  return err;
}
TCOD_Error TCOD_console_printf(TCOD_Console* __restrict con, int x, int y, const char* __restrict fmt, ...) {
  con = TCOD_console_validate_(con);
  if (!con) {
    TCOD_set_errorv("Console pointer must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  va_list args;
  va_start(args, fmt);
  TCOD_Error err = TCOD_console_vprintf(con, x, y, &con->fore, &con->back, con->bkgnd_flag, con->alignment, fmt, args);
  va_end(args);
  return err;
}
int TCOD_console_vprintf_rect(
    struct TCOD_Console* __restrict con,
    int x,
    int y,
    int width,
    int height,
    const TCOD_color_t* __restrict fg,
    const TCOD_color_t* __restrict bg,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment,
    const char* fmt,
    va_list args) {
  const PrintParams params = {
      .console = TCOD_console_validate_(con),
      .x = x,
      .y = y,
      .width = width,
      .height = height,
      .rgb_fg = fg,
      .rgb_bg = bg,
      .flag = flag,
      .alignment = alignment,
      .can_split = true,
  };
  return vprintf_internal_(&params, fmt, args);
}
int TCOD_console_printf_rect_ex(
    TCOD_Console* __restrict con,
    int x,
    int y,
    int width,
    int height,
    TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment,
    const char* __restrict fmt,
    ...) {
  con = TCOD_console_validate_(con);
  if (!con) {
    TCOD_set_errorv("Console pointer must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  normalize_old_rect_(con, alignment, &x, &y, &width, &height);
  va_list args;
  va_start(args, fmt);
  int err = TCOD_console_vprintf_rect(con, x, y, width, height, &con->fore, &con->back, flag, alignment, fmt, args);
  va_end(args);
  return err;
}
int TCOD_console_printf_rect(
    TCOD_Console* __restrict con, int x, int y, int width, int height, const char* __restrict fmt, ...) {
  con = TCOD_console_validate_(con);
  if (!con) {
    TCOD_set_errorv("Console pointer must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  normalize_old_rect_(con, con->alignment, &x, &y, &width, &height);
  va_list args;
  va_start(args, fmt);
  int err = TCOD_console_vprintf_rect(
      con, x, y, width, height, &con->fore, &con->back, con->bkgnd_flag, con->alignment, fmt, args);
  va_end(args);
  return err;
}
int TCOD_console_get_height_rect_fmt(
    TCOD_Console* __restrict con, int x, int y, int width, int height, const char* __restrict fmt, ...) {
  con = TCOD_console_validate_(con);
  if (!con) {
    TCOD_set_errorv("Console pointer must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  normalize_old_rect_(con, TCOD_LEFT, &x, &y, &width, &height);
  const PrintParams params = {
      .console = TCOD_console_validate_(con),
      .x = x,
      .y = y,
      .width = width,
      .height = height,
      .flag = TCOD_BKGND_NONE,
      .alignment = TCOD_LEFT,
      .can_split = true,
      .count_only = true,
  };
  va_list args;
  va_start(args, fmt);
  int result = vprintf_internal_(&params, fmt, args);
  va_end(args);
  return result;
}
TCOD_Error TCOD_console_printf_frame(
    TCOD_Console* __restrict con,
    int x,
    int y,
    int width,
    int height,
    int empty,
    TCOD_bkgnd_flag_t flag,
    const char* __restrict fmt,
    ...) {
  con = TCOD_console_validate_(con);
  if (!con) {
    TCOD_set_errorv("Console pointer must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  char* str = NULL;
  int len = 0;
  if (fmt) {
    va_list ap;
    va_start(ap, fmt);
    len = vsprint_(&str, fmt, ap);
    va_end(ap);
    if (len < 0) {
      TCOD_set_errorv("Error while resolving formatting string.");
      return TCOD_E_ERROR;
    }
  }
  TCOD_Error err = TCOD_console_printn_frame(con, x, y, width, height, len, str, &con->fore, &con->back, flag, empty);
  free(str);
  return err;
}
int TCOD_printf_rgb(TCOD_Console* __restrict console, TCOD_PrintParamsRGB params, const char* __restrict fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int err = TCOD_vprintf_rgb(console, params, fmt, args);
  va_end(args);
  return err;
}
int TCOD_printn_rgb(TCOD_Console* __restrict console, TCOD_PrintParamsRGB params, int n, const char* __restrict str) {
  PrintParams internal_params = {
      .console = TCOD_console_validate_(console),
      .x = params.x,
      .y = params.y,
      .width = params.width,
      .height = params.height,
      .rgb_fg = params.fg,
      .rgb_bg = params.bg,
      .flag = params.flag ? params.flag : TCOD_BKGND_SET,
      .alignment = params.alignment,
      true,
      false};
  int err = printn_internal_(&internal_params, n, str);
  return err;
}
TCOD_PUBLIC int TCOD_vprintf_rgb(
    TCOD_Console* __restrict console, TCOD_PrintParamsRGB params, const char* __restrict fmt, va_list args) {
  PrintParams internal_params = {
      .console = TCOD_console_validate_(console),
      .x = params.x,
      .y = params.y,
      .width = params.width,
      .height = params.height,
      .rgb_fg = params.fg,
      .rgb_bg = params.bg,
      .flag = params.flag ? params.flag : TCOD_BKGND_SET,
      .alignment = params.alignment,
      true,
      false};
  int err = vprintf_internal_(&internal_params, fmt, args);
  return err;
}
#endif  // TCOD_NO_UNICODE
