/*
* libtcod
* Copyright (c) 2008-2018 Jice & Mingos & rmtew
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice or Mingos may not be used to endorse or promote
*       products derived from this software without specific prior written
*       permission.
*
* THIS SOFTWARE IS PROVIDED BY JICE, MINGOS AND RMTEW ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE, MINGOS OR RMTEW BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <console.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#ifndef NO_UNICODE
#include <wchar.h>
#include <wctype.h>
#endif

#include "vendor/stb_sprintf.h"
#include "vendor/utf8proc/utf8proc.h"
#include <libtcod_int.h>
#include <libtcod_utility.h>
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
void TCOD_console_set_color_control(TCOD_colctrl_t con, TCOD_color_t fore, TCOD_color_t back) {
  TCOD_IFNOT(con >= TCOD_COLCTRL_1 && con <= TCOD_COLCTRL_NUMBER ) return;
  color_control_fore[con-1]=fore;
  color_control_back[con-1]=back;
}
char *TCOD_console_vsprint(const char *fmt, va_list ap) {
  #define NB_BUFFERS 10
  #define INITIAL_SIZE 512
  /* several static buffers in case the function is used more than once in a single function call */
  static char *msg[NB_BUFFERS]={NULL};
  static int buflen[NB_BUFFERS]={0};
  static int curbuf=0;
  char *ret;
  bool ok=false;
  if (!msg[0]) {
    int i;
    for (i=0; i < NB_BUFFERS; i++) {
      buflen[i]=INITIAL_SIZE;
      msg[i]=(char *)calloc(sizeof(char),INITIAL_SIZE);
    }
  }
  do {
    /* warning ! depending on the compiler, vsnprintf return -1 or
     the expected string length if the buffer is not big enough */
    int len = stbsp_vsnprintf(msg[curbuf],buflen[curbuf],fmt,ap);
    ok=true;
    if (len < 0 || len >= buflen[curbuf]) {
      /* buffer too small. */
      if ( len > 0 ) {
        while ( buflen[curbuf] < len+1 ) buflen[curbuf]*=2;
      } else {
        buflen[curbuf]*=2;
      }
      free( msg[curbuf] );
      msg[curbuf]=(char *)calloc(sizeof(char),buflen[curbuf]);
      ok=false;
    }
  } while (! ok);
  ret=msg[curbuf];
  curbuf = (curbuf+1)%NB_BUFFERS;
  return ret;
}
/**
 *  Print a string inside of a framed region on a console, using default
 *  colors and alignment.
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
void TCOD_console_print_frame(TCOD_console_t con,int x,int y,int w,int h, bool empty, TCOD_bkgnd_flag_t flag, const char *fmt, ...) {
  struct TCOD_Console *dat=con ? (struct TCOD_Console *)con : TCOD_ctx.root;
  TCOD_console_put_char(con,x,y,TCOD_CHAR_NW,flag);
  TCOD_console_put_char(con,x+w-1,y,TCOD_CHAR_NE,flag);
  TCOD_console_put_char(con,x,y+h-1,TCOD_CHAR_SW,flag);
  TCOD_console_put_char(con,x+w-1,y+h-1,TCOD_CHAR_SE,flag);
  TCOD_console_hline(con,x+1,y,w-2,flag);
  TCOD_console_hline(con,x+1,y+h-1,w-2,flag);
  if ( h > 2 ) {
    TCOD_console_vline(con,x,y+1,h-2,flag);
    TCOD_console_vline(con,x+w-1,y+1,h-2,flag);
    if ( empty ) {
      TCOD_console_rect(con,x+1,y+1,w-2,h-2,true,flag);
    }
  }
  if (fmt) {
    va_list ap;
    int xs;
    TCOD_color_t tmp;
    char *title;
    va_start(ap,fmt);
    title = TCOD_console_vsprint(fmt,ap);
    va_end(ap);
    title[w-3]=0; /* truncate if needed */
    xs = x + (w-(int)strlen(title)-2)/2;
    tmp=dat->back; /* swap colors */
    dat->back=dat->fore;
    dat->fore=tmp;
    TCOD_console_print_ex(con,xs,y,TCOD_BKGND_SET,TCOD_LEFT," %s ",title);
    tmp=dat->back; /* swap colors */
    dat->back=dat->fore;
    dat->fore=tmp;
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
void TCOD_console_print(TCOD_console_t con,int x, int y, const char *fmt, ...) {
  va_list ap;
  struct TCOD_Console *dat=con ? (struct TCOD_Console *)con : TCOD_ctx.root;
  TCOD_IFNOT ( dat != NULL ) return;
  va_start(ap,fmt);
  TCOD_console_print_internal(con,x,y,0,0,dat->bkgnd_flag,
    dat->alignment,TCOD_console_vsprint(fmt,ap), false, false);
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
void TCOD_console_print_ex(TCOD_console_t con,int x, int y,
  TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char *fmt, ...) {
  va_list ap;
  struct TCOD_Console *dat=con ? (struct TCOD_Console *)con : TCOD_ctx.root;
  TCOD_IFNOT ( dat != NULL ) return;
  va_start(ap,fmt);
  TCOD_console_print_internal(con,x,y,0,0,flag,alignment,
    TCOD_console_vsprint(fmt,ap), false, false);
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
int TCOD_console_print_rect(TCOD_console_t con,int x, int y, int w, int h, const char *fmt, ...) {
  int ret;
  va_list ap;
  struct TCOD_Console *dat=con ? (struct TCOD_Console *)con : TCOD_ctx.root;
  TCOD_IFNOT ( dat != NULL ) return 0;
  va_start(ap,fmt);
  ret = TCOD_console_print_internal(con,x,y,w,h,dat->bkgnd_flag,dat->alignment,
    TCOD_console_vsprint(fmt,ap), true, false);
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
int TCOD_console_print_rect_ex(TCOD_console_t con,int x, int y, int w, int h,
  TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment,const char *fmt, ...) {
  int ret;
  va_list ap;
  va_start(ap,fmt);
  ret = TCOD_console_print_internal(con,x,y,w,h,flag,alignment,TCOD_console_vsprint(fmt,ap), true, false);
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
int TCOD_console_get_height_rect(TCOD_console_t con,int x, int y, int w, int h, const char *fmt, ...) {
  int ret;
  va_list ap;
  va_start(ap,fmt);
  ret = TCOD_console_print_internal(con,x,y,w,h,TCOD_BKGND_NONE,TCOD_LEFT,TCOD_console_vsprint(fmt,ap), true, true);
  va_end(ap);
  return ret;
}

/* non public methods */
int TCOD_console_stringLength(const unsigned char *s) {
  int l=0;
  while (*s) {
    if ( *s == (int)TCOD_COLCTRL_FORE_RGB || *s == (int)TCOD_COLCTRL_BACK_RGB ) s+=3;
    else if ( (unsigned)(*s) > (unsigned)TCOD_COLCTRL_STOP ) l++;
    s++;
  }
  return l;
}

unsigned char * TCOD_console_forward(unsigned char *s,int l) {
  while ( *s && l > 0 ) {
    if ( *s == (int)TCOD_COLCTRL_FORE_RGB || *s == (int)TCOD_COLCTRL_BACK_RGB ) s+=3;
    else if ( *s > (int)TCOD_COLCTRL_STOP ) l--;
    s++;
  }
  return s;
}

unsigned char *TCOD_console_strchr(unsigned char *s, unsigned char c) {
  while ( *s && *s != c ) {
    if ( *s == (int)TCOD_COLCTRL_FORE_RGB || *s == (int)TCOD_COLCTRL_BACK_RGB ) s+=3;
    s++;
  }
  return (*s ? s : NULL);
}

int TCOD_console_print_internal(TCOD_console_t con,int x,int y, int rw, int rh, TCOD_bkgnd_flag_t flag,
  TCOD_alignment_t align, char *msg, bool can_split, bool count_only) {
  unsigned char *c=(unsigned char *)msg;
  int cx=0,cy=y;
  int minx,maxx,miny,maxy;
  TCOD_color_t oldFore;
  TCOD_color_t oldBack;
  struct TCOD_Console *dat=con ? (struct TCOD_Console *)con : TCOD_ctx.root;
  TCOD_IFNOT ( dat != NULL
    && (unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h )
    return 0;
  TCOD_IFNOT(msg != NULL) return 0;
  if ( rh == 0 ) rh = dat->h-y;
  if ( rw == 0 ) switch(align) {
    case TCOD_LEFT : rw = dat->w-x; break;
    case TCOD_RIGHT : rw=x+1; break;
    case TCOD_CENTER : default : rw=dat->w; break;
  }
  oldFore=dat->fore;
  oldBack=dat->back;
  miny=y;
  maxy=dat->h-1;
  if (rh > 0) maxy=MIN(maxy,y+rh-1);
  switch (align) {
    case TCOD_LEFT : minx=MAX(0,x); maxx=MIN(dat->w-1,x+rw-1); break;
    case TCOD_RIGHT : minx=MAX(0,x-rw+1); maxx=MIN(dat->w-1,x); break;
    case TCOD_CENTER : default : minx=MAX(0,x-rw/2); maxx=MIN(dat->w-1,x+rw/2); break;
  }

  do {
    /* get \n delimited sub-message */
    unsigned char *end=TCOD_console_strchr(c,'\n');
    char bak=0;
    int cl;
    unsigned char *split=NULL;
    if ( end ) *end=0;
    cl= TCOD_console_stringLength(c);
    /* find starting x */
    switch (align) {
      case TCOD_LEFT : cx=x; break;
      case TCOD_RIGHT : cx=x-cl+1; break;
      case TCOD_CENTER : cx= x-cl/2;break;
    }
    /* check if the string is completely out of the minx,miny,maxx,maxy frame */
    if ( cy >= miny && cy <= maxy && cx <= maxx && cx+cl -1 >= minx ) {
      if ( can_split && cy <= maxy ) {
        /* if partially out of screen, try to split the sub-message */
        if ( cx < minx ) split = TCOD_console_forward(c, align == TCOD_CENTER ? cl-2*(minx-cx) : cl-(minx-cx));
        else if ( align == TCOD_CENTER ) {
          if ( cx + cl/2 > maxx+1 ) split = TCOD_console_forward(c, maxx+1 - cx);
        } else {
          if ( cx + cl > maxx+1 ) split = TCOD_console_forward(c, maxx+1 - cx);
        }
      }
      if ( split ) {
        unsigned char *oldsplit=split;
        while ( ! isspace(*split) && split > c ) split --;
        if (end) *end='\n';
        if (!isspace(*split) ) {
          split=oldsplit;
        }
        end=split;
        bak=*split;
        *split=0;
        cl=TCOD_console_stringLength(c);
        switch (align) {
          case TCOD_LEFT : cx=x; break;
          case TCOD_RIGHT : cx=x-cl+1; break;
          case TCOD_CENTER : cx= x-cl/2;break;
        }
      }
      if ( cx < minx ) {
        /* truncate left part */
        c += minx-cx;
        cl -= minx-cx;
        cx=minx;
      }
      if ( cx + cl > maxx+1 ) {
        /* truncate right part */
        split = TCOD_console_forward(c, maxx+1 - cx);
        *split=0;
      }
      /* render the sub-message */
      if ( cy >= 0 && cy < dat->h )
      while (*c) {
        if ( *c >= TCOD_COLCTRL_1 && *c <= TCOD_COLCTRL_NUMBER ) {
          dat->fore=color_control_fore[(int)(*c)-1];
          dat->back=color_control_back[(int)(*c)-1];
        } else if ( *c == TCOD_COLCTRL_FORE_RGB ) {
          c++;
          dat->fore.r=*c++;
          dat->fore.g=*c++;
          dat->fore.b=*c;
        } else if ( *c == TCOD_COLCTRL_BACK_RGB ) {
          c++;
          dat->back.r=*c++;
          dat->back.g=*c++;
          dat->back.b=*c;
        } else if ( *c == TCOD_COLCTRL_STOP ) {
          dat->fore=oldFore;
          dat->back=oldBack;
        } else {
          if (! count_only) TCOD_console_put_char(con,cx,cy,(int)(*c),flag);
          cx++;
        }
        c++;
      }
    }
    if ( end ) {
      /* next line */
      if ( split && ! isspace(bak) ) {
        *end=bak;
        c=end;
      } else {
        c=end+1;
      }
      cy++;
    } else c=NULL;
  } while ( c && cy < dat->h && (rh == 0 || cy < y+rh) );
  return cy-y+1;
}

#ifndef NO_UNICODE

wchar_t *TCOD_console_strchr_utf(wchar_t *s, char c) {
  while ( *s && *s != c ) {
    if ( *s == (int)TCOD_COLCTRL_FORE_RGB || *s == (int)TCOD_COLCTRL_BACK_RGB ) s+=3;
    s++;
  }
  return (*s ? s : NULL);
}

void TCOD_console_map_string_to_font_utf(const wchar_t *s, int fontCharX, int fontCharY) {
  TCOD_IFNOT(s != NULL) return;
  while (*s) {
    TCOD_sys_map_ascii_to_font(*s, fontCharX, fontCharY);
    fontCharX++;
    if ( fontCharX == TCOD_ctx.fontNbCharHoriz ) {
      fontCharX=0;
      fontCharY++;
    }
    s++;
  }
}

wchar_t *TCOD_console_vsprint_utf(const wchar_t *fmt, va_list ap) {
  #define NB_BUFFERS 10
  #define INITIAL_SIZE 512
  /* several static buffers in case the function is used more than once in a single function call */
  static wchar_t *msg[NB_BUFFERS]={NULL};
  static int buflen[NB_BUFFERS]={0};
  static int curbuf=0;
  wchar_t *ret;
  bool ok=false;
  if (!msg[0]) {
    int i;
    for (i=0; i < NB_BUFFERS; i++) {
      buflen[i]=INITIAL_SIZE;
      msg[i]=(wchar_t *)calloc(sizeof(wchar_t),INITIAL_SIZE);
    }
  }
  do {
    /* warning ! depending on the compiler, vsnprintf return -1 or
     the expected string length if the buffer is not big enough */
    int len = vswprintf(msg[curbuf],buflen[curbuf],fmt,ap);
    ok=true;
    if (len < 0 || len >= buflen[curbuf]) {
      /* buffer too small. */
      if ( len > 0 ) {
        while ( buflen[curbuf] < len+1 ) buflen[curbuf]*=2;
      } else {
        buflen[curbuf]*=2;
      }
      free( msg[curbuf] );
      msg[curbuf]=(wchar_t *)calloc(sizeof(wchar_t),buflen[curbuf]);
      ok=false;
    }
  } while (! ok);
  ret=msg[curbuf];
  curbuf = (curbuf+1)%NB_BUFFERS;
  return ret;
}


int TCOD_console_stringLength_utf(const wchar_t *s) {
  int l=0;
  while (*s) {
    if ( *s == (int)TCOD_COLCTRL_FORE_RGB || *s == (int)TCOD_COLCTRL_BACK_RGB ) s+=3;
    else if ( *s > (int)TCOD_COLCTRL_STOP ) l++;
    s++;
  }
  return l;
}

wchar_t * TCOD_console_forward_utf(wchar_t *s,int l) {
  while ( *s && l > 0 ) {
    if ( *s == (int)TCOD_COLCTRL_FORE_RGB || *s == (int)TCOD_COLCTRL_BACK_RGB ) s+=3;
    else if ( *s > (int)TCOD_COLCTRL_STOP ) l--;
    s++;
  }
  return s;
}

int TCOD_console_print_internal_utf(TCOD_console_t con,int x,int y, int rw, int rh, TCOD_bkgnd_flag_t flag,
  TCOD_alignment_t align, wchar_t *msg, bool can_split, bool count_only) {
  wchar_t *c=msg;
  int cx=0,cy=y;
  int minx,maxx,miny,maxy;
  TCOD_color_t oldFore;
  TCOD_color_t oldBack;
  struct TCOD_Console *dat=con ? (struct TCOD_Console *)con : TCOD_ctx.root;
  TCOD_IFNOT ( dat != NULL
    && (unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h )
    return 0;
  TCOD_IFNOT(msg != NULL) return 0;
  if ( rh == 0 ) rh = dat->h-y;
  if ( rw == 0 ) switch(align) {
    case TCOD_LEFT : rw = dat->w-x; break;
    case TCOD_RIGHT : rw=x+1; break;
    case TCOD_CENTER : default : rw=dat->w; break;
  }
  oldFore=dat->fore;
  oldBack=dat->back;
  miny=y;
  maxy=dat->h-1;
  if (rh > 0) maxy=MIN(maxy,y+rh-1);
  switch (align) {
    case TCOD_LEFT : minx=MAX(0,x); maxx=MIN(dat->w-1,x+rw-1); break;
    case TCOD_RIGHT : minx=MAX(0,x-rw+1); maxx=MIN(dat->w-1,x); break;
    case TCOD_CENTER : default : minx=MAX(0,x-rw/2); maxx=MIN(dat->w-1,x+rw/2); break;
  }

  do {
    /* get \n delimited sub-message */
    wchar_t *end=TCOD_console_strchr_utf(c,'\n');
    wchar_t bak=0;
    int cl;
    wchar_t *split=NULL;
    if ( end ) *end=0;
    cl= TCOD_console_stringLength_utf(c);
    /* find starting x */
    switch (align) {
      case TCOD_LEFT : cx=x; break;
      case TCOD_RIGHT : cx=x-cl+1; break;
      case TCOD_CENTER : cx= x-cl/2;break;
    }
    /* check if the string is completely out of the minx,miny,maxx,maxy frame */
    if ( cy >= miny && cy <= maxy && cx <= maxx && cx+cl -1 >= minx ) {
      if ( can_split && cy < maxy ) {
        /* if partially out of screen, try to split the sub-message */
        if ( cx < minx ) split = TCOD_console_forward_utf(c, align == TCOD_CENTER ? cl-2*(minx-cx) : cl-(minx-cx));
        else if ( align==TCOD_CENTER ) {
          if ( cx + cl/2 > maxx+1 ) split = TCOD_console_forward_utf(c, maxx+1 - cx);
        } else {
          if ( cx + cl > maxx+1 ) split = TCOD_console_forward_utf(c, maxx+1 - cx);
        }
      }
      if ( split ) {
        wchar_t *oldsplit=split;
        while ( ! iswspace(*split) && split > c ) split --;
        if (end) *end='\n';
        if (!iswspace(*split) ) {
          split=oldsplit;
        }
        end=split;
        bak=*split;
        *split=0;
        cl=TCOD_console_stringLength_utf(c);
        switch (align) {
          case TCOD_LEFT : cx=x; break;
          case TCOD_RIGHT : cx=x-cl+1; break;
          case TCOD_CENTER : cx= x-cl/2;break;
        }
      }
      if ( cx < minx ) {
        /* truncate left part */
        c += minx-cx;
        cl -= minx-cx;
        cx=minx;
      }
      if ( cx + cl > maxx+1 ) {
        /* truncate right part */
        split = TCOD_console_forward_utf(c, maxx+1 - cx);
        *split=0;
      }
      /* render the sub-message */
      if ( cy >= 0 && cy < dat->h )
      while (*c) {
        if ( *c >= TCOD_COLCTRL_1 && *c <= TCOD_COLCTRL_NUMBER ) {
          dat->fore=color_control_fore[(int)(*c)-1];
          dat->back=color_control_back[(int)(*c)-1];
        } else if ( *c == TCOD_COLCTRL_FORE_RGB ) {
          c++;
          dat->fore.r=(uint8_t)(*c++);
          dat->fore.g=(uint8_t)(*c++);
          dat->fore.b=(uint8_t)(*c);
        } else if ( *c == TCOD_COLCTRL_BACK_RGB ) {
          c++;
          dat->back.r=(uint8_t)(*c++);
          dat->back.g=(uint8_t)(*c++);
          dat->back.b=(uint8_t)(*c);
        } else if ( *c == TCOD_COLCTRL_STOP ) {
          dat->fore=oldFore;
          dat->back=oldBack;
        } else {
          if (! count_only) TCOD_console_put_char(con,cx,cy,(int)(*c),flag);
          cx++;
        }
        c++;
      }
    }
    if ( end ) {
      /* next line */
      if ( split && ! iswspace(bak) ) {
        *end=bak;
        c=end;
      } else {
        c=end+1;
      }
      cy++;
    } else c=NULL;
  } while ( c && cy < dat->h && (rh == 0 || cy < y+rh) );
  return cy-y+1;
}
/**
 *  /rst
 *  .. deprecated:: 1.8
 *    Use :any:`TCOD_console_printf` instead.
 *  /endrst
 */
void TCOD_console_print_utf(TCOD_console_t con,int x, int y, const wchar_t *fmt, ...) {
  va_list ap;
  struct TCOD_Console *dat=con ? (struct TCOD_Console *)con : TCOD_ctx.root;
  TCOD_IFNOT ( dat != NULL ) return;
  va_start(ap,fmt);
  TCOD_console_print_internal_utf(con,x,y,0,0,dat->bkgnd_flag,dat->alignment,
    TCOD_console_vsprint_utf(fmt,ap), false, false);
  va_end(ap);
}
/**
 *  /rst
 *  .. deprecated:: 1.8
 *    Use :any:`TCOD_console_printf_ex` instead.
 *  /endrst
 */
void TCOD_console_print_ex_utf(TCOD_console_t con,int x, int y,
  TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const wchar_t *fmt, ...) {
  va_list ap;
  struct TCOD_Console *dat=con ? (struct TCOD_Console *)con : TCOD_ctx.root;
  TCOD_IFNOT ( dat != NULL ) return;
  va_start(ap,fmt);
  TCOD_console_print_internal_utf(con,x,y,0,0,flag,alignment,TCOD_console_vsprint_utf(fmt,ap), false, false);
  va_end(ap);
}

int TCOD_console_print_rect_utf(TCOD_console_t con,int x, int y, int w, int h,
  const wchar_t *fmt, ...) {
  int ret;
  va_list ap;
  struct TCOD_Console *dat=con ? (struct TCOD_Console *)con : TCOD_ctx.root;
  TCOD_IFNOT ( dat != NULL ) return 0;
  va_start(ap,fmt);
  ret = TCOD_console_print_internal_utf(con,x,y,w,h,dat->bkgnd_flag,dat->alignment,
    TCOD_console_vsprint_utf(fmt,ap), true, false);
  va_end(ap);
  return ret;
}
/**
 *  /rst
 *  .. deprecated:: 1.8
 *    Use :any:`TCOD_console_printf_rect_ex` instead.
 *  /endrst
 */
int TCOD_console_print_rect_ex_utf(TCOD_console_t con,int x, int y, int w, int h,
  TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const wchar_t *fmt, ...) {
  int ret;
  va_list ap;
  va_start(ap,fmt);
  ret=TCOD_console_print_internal_utf(con,x,y,w,h,flag,alignment,TCOD_console_vsprint_utf(fmt,ap), true, false);
  va_end(ap);
  return ret;
}
/**
 *  /rst
 *  .. deprecated:: 1.8
 *  /endrst
 */
int TCOD_console_get_height_rect_utf(TCOD_console_t con,int x, int y, int w, int h, const wchar_t *fmt, ...) {
  int ret;
  va_list ap;
  va_start(ap,fmt);
  ret = TCOD_console_print_internal_utf(con,x,y,w,h,TCOD_BKGND_NONE,TCOD_LEFT,TCOD_console_vsprint_utf(fmt,ap), true, true);
  va_end(ap);
  return ret;
}

#endif /* NO_UNICODE */
/**
 *  Outputs the bounding box used for printing.
 *
 *  If `max_width` or `max_height` are zero, they will be set automatically.
 */
static int TCOD_get_print_bounds(
    const struct TCOD_Console *con, TCOD_alignment_t align, int x, int y,
    int *max_width, int *max_height,
    int *left, int *right, int *top, int *bottom) {
  if (!con) { return -1; }
  /* Set default width/height if either is zero. */
  if (*max_height == 0) { *max_height = con->h - y; }
  if (*max_width == 0 ) {
    switch(align) {
      default: case TCOD_LEFT:
        *max_width = con->w - x;
        break;
      case TCOD_RIGHT:
        *max_width = x + 1;
        break;
      case TCOD_CENTER:
        *max_width = con->w;
        break;
    }
  }
  /* Return printing boundary. */
  *top = y;
  *bottom = MIN(con->h - 1, y + *max_height - 1);
  switch (align) {
    case TCOD_LEFT:
      *left = x;
      *right = x + *max_width - 1;
      break;
    case TCOD_RIGHT:
      *left = x - *max_width + 1;
      *right = x;
      break;
    case TCOD_CENTER: default:
      *left = x - *max_width / 2;
      *right = x + *max_width / 2;
      break;
  }
  *left = MAX(0, *left);
  *right = MIN(*right, con->w - 1);
  return 0;
}
/**
 *  Parse ALL color codes on a utf8 string, returns the total length of the
 *  string parsed.
 *
 *  Any `TCOD_color_t` parameters can be NULL.
 */
static utf8proc_ssize_t TCOD_utf8_parse_color(
    const unsigned char *str, TCOD_color_t *fg_color, TCOD_color_t *bg_color,
    const TCOD_color_t *old_fg_color, const TCOD_color_t *old_bg_color) {
  const unsigned char *start = str;
  utf8proc_int32_t codepoint;
  utf8proc_ssize_t code_size;
  code_size = utf8proc_iterate(str, -1, &codepoint);
  str += code_size;
  if (TCOD_COLCTRL_1 <= codepoint && codepoint <= TCOD_COLCTRL_NUMBER) {
    /* Read colors from the color control array. */
    int color_index = codepoint - TCOD_COLCTRL_1;
    if (fg_color) { *fg_color = color_control_fore[color_index]; }
    if (bg_color) { *bg_color = color_control_back[color_index]; }
    return code_size + TCOD_utf8_parse_color(str, fg_color, bg_color,
                                             old_fg_color, old_bg_color);
  } else if (codepoint == TCOD_COLCTRL_STOP) {
    /* Return colors to their original color. */
    if (fg_color && old_fg_color) { *fg_color = *old_fg_color; }
    if (bg_color && old_bg_color) { *bg_color = *old_bg_color; }
    return code_size + TCOD_utf8_parse_color(str, fg_color, bg_color,
                                             old_fg_color, old_bg_color);
  } else if (codepoint == TCOD_COLCTRL_FORE_RGB
      || codepoint == TCOD_COLCTRL_BACK_RGB) {
    /* Read the next 3 code-points as 8-bit RGB color values. */
    unsigned char *color_array = NULL;
    int i;
    if (codepoint == TCOD_COLCTRL_FORE_RGB && fg_color) {
      color_array = (unsigned char *)fg_color;
    } else if (codepoint == TCOD_COLCTRL_BACK_RGB && bg_color) {
      color_array = (unsigned char *)bg_color;
    }
    for (i = 0; i < 3; ++i) {
      code_size = utf8proc_iterate(str, -1, &codepoint);
      if (code_size < 0) { break; }
      if (color_array) { color_array[i] = (unsigned char)codepoint; }
      str += code_size;
    }
    code_size = str - start;
    return code_size + TCOD_utf8_parse_color(str, fg_color, bg_color,
                                             old_fg_color, old_bg_color);
  } else {
    return 0; /* `str` is not on any color codes. */
  }
}
/**
 *  Check for and return the length of ALL control codes at `str`.
 */
static utf8proc_ssize_t TCOD_utf8_skip_control_codes(
    const unsigned char *str) {
  return TCOD_utf8_parse_color(str, NULL, NULL, NULL, NULL);
}
/**
 *  Get the next line-break or null terminator, or break the string before
 *  `max_width`.
 *
 *  Return 0 if a line-break or null terminator was found, or 1 if the line was
 *  automatically split.
 */
int TCOD_utf8_next_split(
    const unsigned char *str, int max_width, int can_split,
    const unsigned char **endpoint, utf8proc_ssize_t *total_width) {
  const unsigned char *break_point = NULL;
  *total_width = 0;
  *endpoint = str;
  while (*str != '\0') {
    utf8proc_int32_t codepoint;
    utf8proc_ssize_t code_size;
    const utf8proc_property_t *property;

    str += TCOD_utf8_skip_control_codes(str);
    code_size = utf8proc_iterate(str, -1, &codepoint);
    property = utf8proc_get_property(codepoint);
    if (code_size <= 0) { break; }
    if (can_split && *total_width > 0
        && *total_width + (int)property->charwidth > max_width) {
      *endpoint = str;
      if (break_point) { *endpoint = break_point; }
      return 1;
    }
    switch (property->category) {
      case UTF8PROC_CATEGORY_PD: /* Punctuation, dash */
      case UTF8PROC_CATEGORY_ZS: /* Separator, space */
        break_point = str;
        break;
      case UTF8PROC_CATEGORY_ZL: /* Separator, line */
      case UTF8PROC_CATEGORY_ZP: /* Separator, paragraph */
        *endpoint = str;
        return 0;
      default:
        break;
    }
    str += code_size;
    *total_width += 1;
  }
  *endpoint = str;
  return 0;
}
/**
 *  Internal UTF-8 printer.
 */
int TCOD_console_print_internal_utf8_(
    TCOD_console_t con, int x, int y, int max_width, int max_height,
    TCOD_bkgnd_flag_t flag, TCOD_alignment_t align,
    const unsigned char *string, int can_split, int count_only) {
  int left, right, top, bottom; /* Print bounding box. */
  TCOD_color_t old_fg, old_bg;
  int cursor_x = 0;
  con = con ? con : TCOD_ctx.root;
  if (!con || !string) { return 0; }
  old_fg = con->fore;
  old_bg = con->back;
  TCOD_get_print_bounds(con, align, x, y, &max_width, &max_height,
                        &left, &right, &top, &bottom);
  while (*string != '\0' && top >= bottom) {
    const unsigned char *line_break;
    utf8proc_ssize_t line_width;
    utf8proc_int32_t codepoint;
    utf8proc_ssize_t code_size;
    const utf8proc_property_t *property;
    int split_status;
    code_size = utf8proc_iterate(string, -1, &codepoint);
    if (code_size < 0) { return -1; }
    property = utf8proc_get_property(codepoint);
    /* Check for newlines. */
    if (property->category == UTF8PROC_CATEGORY_ZL) { /* Separator, line */
      string += code_size;
      top += 1;
      continue;
    }
    if (property->category == UTF8PROC_CATEGORY_ZP) { /* Separator,paragraph */
      string += code_size;
      top += 2;
      continue;
    }
    /* Get the next line of characters. */
    split_status = TCOD_utf8_next_split(string, max_width, can_split,
                                        &line_break, &line_width);
    switch (align) {
      default:
      case TCOD_LEFT:
        cursor_x = x;
        break;
      case TCOD_RIGHT:
        cursor_x = x - line_width;
        break;
      case TCOD_CENTER:
        cursor_x = x - line_width / 2;
        break;
    }
    while (string < line_break) {
      /* Actually render this line of characters. */
      string += TCOD_utf8_parse_color(string, &con->fore, &con->back,
                                      &old_fg, &old_bg);
      string += code_size = utf8proc_iterate(string, -1, &codepoint);
      if (code_size < 0) { return -1; }
      if (!count_only && left <= cursor_x && cursor_x <= right) {
        TCOD_console_put_char(con, cursor_x, top, codepoint, flag);
      }
      cursor_x += 1;
    }
    /* Ignore any extra spaces. */
    while (string != '\0') {
      code_size = utf8proc_iterate(string, -1, &codepoint);
      if (code_size < 0) { return -1; }
      property = utf8proc_get_property(codepoint);
      if (property->category == UTF8PROC_CATEGORY_ZS) { /* Separator, space */
        string += code_size;
      } else {
        break;
      }
    }
    /* If there was an automatic split earlier then the top is moved down. */
    if (split_status == 1) {
      top += 1;
    }
  }
  return MIN(top, bottom) - y + 1;
}
/**
 *  Format and print a UTF-8 string to a console.
 *  /rst
 *  .. versionadded:: 1.8
 *  /endrst
 */
void TCOD_console_printf_ex(struct TCOD_Console *con, int x, int y,
      TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment,
      const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  TCOD_console_print_internal_utf8_(
      con, x, y, 0, 0, flag, alignment,
      (const unsigned char *)TCOD_console_vsprint(fmt, ap), false, false);
  va_end(ap);
}
/**
 *  Format and print a UTF-8 string to a console.
 *  /rst
 *  .. versionadded:: 1.8
 *  /endrst
 */
void TCOD_console_printf(struct TCOD_Console *con, int x, int y,
                         const char *fmt, ...) {
  va_list ap;
  con = con ? con : TCOD_ctx.root;
  if (!con) { return; }
  va_start(ap, fmt);
  TCOD_console_print_internal_utf8_(
      con, x, y, 0, 0, con->bkgnd_flag, con->alignment,
      (const unsigned char *)TCOD_console_vsprint(fmt, ap), false, false);
  va_end(ap);
}
/**
 *  Format and print a UTF-8 string to a console.
 *  /rst
 *  .. versionadded:: 1.8
 *  /endrst
 */
int TCOD_console_printf_rect_ex(struct TCOD_Console *con,
    int x, int y, int w, int h,
    TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment,const char *fmt, ...) {
  int ret;
  va_list ap;
  va_start(ap, fmt);
  ret = TCOD_console_print_internal_utf8_(
      con, x, y, w, h, flag, alignment,
      (const unsigned char *)TCOD_console_vsprint(fmt, ap), true, false);
  va_end(ap);
  return ret;
}
/**
 *  Format and print a UTF-8 string to a console.
 *  /rst
 *  .. versionadded:: 1.8
 *  /endrst
 */
int TCOD_console_printf_rect(struct TCOD_Console *con,
    int x, int y, int w, int h, const char *fmt, ...) {
  int ret;
  va_list ap;
  con = con ? con : TCOD_ctx.root;
  if (!con) { return 0; }
  va_start(ap, fmt);
  ret = TCOD_console_print_internal_utf8_(
      con, x, y, w, h, con->bkgnd_flag,con->alignment,
      (const unsigned char *)TCOD_console_vsprint(fmt, ap), true, false);
  va_end(ap);
  return ret;
}
