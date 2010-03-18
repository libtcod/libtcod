/*
* libtcod 1.5.1
* Copyright (c) 2008,2009,2010 Jice & Mingos
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice or Mingos may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY JICE AND MINGOS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE OR MINGOS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libtcod.h"
#include "libtcod_int.h"
#ifndef NO_UNICODE
#include <wchar.h>
#include <wctype.h>
#endif

#if defined( TCOD_VISUAL_STUDIO )
static const char *version_string ="libtcod "TCOD_STRVERSION;
#else
static const char *version_string __attribute__((unused)) ="libtcod "TCOD_STRVERSION;
#endif

static bool windowClosed=false;
TCOD_internal_context_t TCOD_ctx={
	/* number of characters in the bitmap font */
	16,16,
	/* font type and layout */
	false,false,false,
	/* character size in font */
	8,8,
	"terminal.png","",
	NULL,NULL,0,false,0,0,0,0,0,0,
	/* default renderer to use */
	TCOD_RENDERER_GLSL,
	NULL,
	/* fading data */
	{0,0,0},255,
};

static TCOD_color_t color_control_fore[TCOD_COLCTRL_NUMBER];
static TCOD_color_t color_control_back[TCOD_COLCTRL_NUMBER];

void TCOD_console_set_color_control(TCOD_colctrl_t con, TCOD_color_t fore, TCOD_color_t back) {
	TCOD_IFNOT(con >= TCOD_COLCTRL_1 && con <= TCOD_COLCTRL_NUMBER ) return;
	color_control_fore[con-1]=fore;
	color_control_back[con-1]=back;
}

void TCOD_fatal(const char *fmt, ...) {
	va_list ap;
	TCOD_sys_term();
	printf("%s\n",version_string);
	va_start(ap,fmt);
	vprintf(fmt,ap);
	va_end(ap);
	printf ("\n");
	exit (1);
}

void TCOD_fatal_nopar(const char *msg) {
	TCOD_sys_term();
	printf("%s\n%s\n",version_string,msg);
	exit (1);
}

TCOD_console_t TCOD_console_new(int w, int h)  {
	TCOD_IFNOT(w > 0 && h > 0 ) {
		return NULL;
	} else {
		TCOD_console_data_t *con=(TCOD_console_data_t *)calloc(sizeof(TCOD_console_data_t),1);
		con->w=w;
		con->h=h;
		TCOD_console_init(con,NULL,false);
		return (TCOD_console_t)con;
	}
}

TCOD_key_t TCOD_console_check_for_keypress(int flags) {
	return TCOD_sys_check_for_keypress(flags);
}

TCOD_key_t TCOD_console_wait_for_keypress(bool flush) {
	return TCOD_sys_wait_for_keypress(flush);
}

bool TCOD_console_is_window_closed() {
	return windowClosed;
}

void TCOD_console_set_window_closed() {
	windowClosed=true;
}

void TCOD_console_set_window_title(const char *title) {
	TCOD_sys_set_window_title(title);
}

void TCOD_console_set_fullscreen(bool fullscreen) {
	TCOD_IFNOT(TCOD_ctx.root != NULL) return;
	TCOD_sys_set_fullscreen(fullscreen);
	TCOD_ctx.fullscreen=fullscreen;
}

bool TCOD_console_is_fullscreen() {
	return TCOD_ctx.fullscreen;
}

void TCOD_console_delete(TCOD_console_t con) {
    TCOD_console_data_t *dat=(TCOD_console_data_t *)(con);
	if (! dat ) {
		dat=TCOD_ctx.root;
		TCOD_sys_term();
		TCOD_ctx.root=NULL;
	}
	free(dat->buf);
	free(dat->oldbuf);
	free(dat);
}

void TCOD_console_blit(TCOD_console_t srcCon,int xSrc, int ySrc, int wSrc, int hSrc,
	TCOD_console_t dstCon, int xDst, int yDst, float foreground_alpha, float background_alpha) {
    TCOD_console_data_t *src=srcCon ? (TCOD_console_data_t *)srcCon : TCOD_ctx.root;
    TCOD_console_data_t *dst=dstCon ? (TCOD_console_data_t *)dstCon : TCOD_ctx.root;
	int cx,cy;
	TCOD_IFNOT(wSrc > 0 && hSrc > 0 ) return;
	TCOD_IFNOT(xDst+wSrc >= 0 && yDst+hSrc >= 0 && xDst < dst->w && yDst < dst->h) return;
	for (cx = xSrc; cx < xSrc+wSrc; cx++) {
		for (cy = ySrc; cy < ySrc+hSrc; cy++) {
			// check if we're outside the dest console
			int dx=cx-xSrc+xDst;
			int dy=cy-ySrc+yDst;
			uint8 dirt;
			char_t srcChar,dstChar;
			if ( (unsigned) cx >= (unsigned) src->w || (unsigned)cy >= (unsigned) src->h ) continue;
			if ( (unsigned) dx >= (unsigned) dst->w || (unsigned)dy >= (unsigned) dst->h ) continue;
			// check if source pixel is transparent
			srcChar=src->buf[cy * src->w+cx];
			if ( src->haskey && srcChar.back.r == src->key.r
				&& srcChar.back.g == src->key.g && srcChar.back.b == src->key.b ) continue;
			if ( foreground_alpha == 1.0f && background_alpha == 1.0f ) {
				dstChar=srcChar;
			} else {
				dstChar=dst->buf[dy * dst->w + dx];

				dstChar.back = TCOD_color_lerp(dstChar.back,srcChar.back,background_alpha);
				if ( srcChar.c == ' ' ) {
					dstChar.fore = TCOD_color_lerp(dstChar.fore,srcChar.back,background_alpha);
				} else if (dstChar.c == ' ') {
					dstChar.c=srcChar.c;
					dstChar.cf=srcChar.cf;
					dstChar.fore = TCOD_color_lerp(dstChar.back,srcChar.fore,foreground_alpha);
				} else if (dstChar.c == srcChar.c) {
					dstChar.fore = TCOD_color_lerp(dstChar.fore,srcChar.fore,foreground_alpha);
				} else {
					if ( foreground_alpha < 0.5f ) {
						dstChar.fore=TCOD_color_lerp(dstChar.fore,dstChar.back,
							foreground_alpha*2);
					} else {
						dstChar.c=srcChar.c;
						dstChar.cf=srcChar.cf;
						dstChar.fore=TCOD_color_lerp(dstChar.back,srcChar.fore,
							(foreground_alpha-0.5f)*2);
					}
				}
			}
			dirt=dst->buf[dy * dst->w + dx].dirt;
			dst->buf[dy * dst->w + dx] = dstChar;
			dst->buf[dy * dst->w + dx].dirt=dirt;
		}
	}
}


void TCOD_console_flush() {
	TCOD_console_data_t *dat=TCOD_ctx.root;
	TCOD_IFNOT(TCOD_ctx.root != NULL) return;
	TCOD_sys_flush(true);
	memcpy(dat->oldbuf,dat->buf,sizeof(char_t)*
		dat->w*dat->h);
}

void TCOD_console_set_fade(uint8 val, TCOD_color_t fadecol) {
	TCOD_ctx.fade=val;
	TCOD_ctx.fading_color=fadecol;
}

uint8 TCOD_console_get_fade() {
	return TCOD_ctx.fade;
}

TCOD_color_t TCOD_console_get_fading_color() {
	return TCOD_ctx.fading_color;
}


void TCOD_console_put_char(TCOD_console_t con,int x, int y, int c, TCOD_bkgnd_flag_t flag) {
	int offset;
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT ( dat != NULL && (unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h ) return;
	TCOD_IFNOT (c >= 0 && c < TCOD_ctx.max_font_chars ) return;
	offset = y * dat->w + x;
	dat->buf[ offset ].c = c;
	dat->buf[ offset ].cf = TCOD_ctx.ascii_to_tcod[c];
	dat->buf[ offset ].fore = dat->fore;
	TCOD_console_set_back(con,x,y,dat->back,(TCOD_bkgnd_flag_t)flag);
}

void TCOD_console_put_char_ex(TCOD_console_t con,int x, int y, int c, TCOD_color_t fore, TCOD_color_t back) {
	int offset;
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT ( dat != NULL && (unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h ) return;
	TCOD_IFNOT (c >= 0 && c < TCOD_ctx.max_font_chars ) return;
	offset = y * dat->w + x;
	dat->buf[ offset ].c = c;
	dat->buf[ offset ].cf = TCOD_ctx.ascii_to_tcod[c];
	dat->buf[ offset ].fore = fore;
	dat->buf[ offset ].back = back;
}

void TCOD_console_set_dirty(int dx, int dy, int dw, int dh) {
	int x,y;
	TCOD_console_data_t *dat=TCOD_ctx.root;
	TCOD_IFNOT(dat != NULL) return;
	TCOD_IFNOT(dx < dat->w && dy < dat->h && dx+dw >= 0 && dy+dh >= 0 ) return;
	TCOD_IFNOT( dx >= 0 ) {
		dw += dx;
		dx = 0;
	}
	TCOD_IFNOT( dy >= 0 ) {
		dh += dy;
		dy = 0;
	}
	TCOD_IFNOT( dx+dw <= dat->w ) dw = dat->w-dx;
	TCOD_IFNOT( dy+dh <= dat->h ) dh = dat->h-dy;

	for (x=dx; x < dx+dw;x++) {
		for (y=dy; y < dy+dh; y++) {
			int off=x+dat->w*y;
			dat->buf[off].dirt=1;
		}
	}
}

void TCOD_console_clear(TCOD_console_t con) {
	int x,y;
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT(dat != NULL) return;
	for (x=0; x < dat->w;x++) {
		for (y=0; y < dat->h; y++) {
			int off=x+dat->w*y;
			dat->buf[off].dirt=0;
			dat->buf[off].c=' ';
			dat->buf[off].cf=TCOD_ctx.ascii_to_tcod[' '];
			dat->buf[off].fore=dat->fore;
			dat->buf[off].back=dat->back;
		}
	}
}

TCOD_color_t TCOD_console_get_back(TCOD_console_t con,int x, int y) {
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT ( dat != NULL
		&& (unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h )
		return TCOD_black;
	return dat->buf[ y * dat->w + x ].back;
}

void TCOD_console_set_fore(TCOD_console_t con,int x,int y, TCOD_color_t col) {
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	if ( (unsigned)(x) >= (unsigned)dat->w || (unsigned)(y) >= (unsigned)dat->h ) return;
	TCOD_IFNOT ( dat != NULL
		&& (unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h )
		return;
	dat->buf[ y * dat->w + x ].fore=col;
}

TCOD_color_t TCOD_console_get_fore(TCOD_console_t con,int x, int y) {
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT ( dat != NULL
		&& (unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h )
		return TCOD_white;
	return dat->buf[ y * dat->w + x ].fore;
}

int TCOD_console_get_char(TCOD_console_t con,int x, int y) {
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT ( dat != NULL
		&& (unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h )
		return 0;
	return dat->buf[ y * dat->w + x ].c;
}

void TCOD_console_set_back(TCOD_console_t con,int x, int y, TCOD_color_t col, TCOD_bkgnd_flag_t flag) {
	TCOD_color_t *back;
	int newr,newg,newb;
	int alpha;
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT ( dat != NULL
		&& (unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h )
		return;
	back=&dat->buf[y*dat->w+x].back;
	switch ( flag & 0xff ) {
		case TCOD_BKGND_SET : *back = col; break;
		case TCOD_BKGND_MULTIPLY  : *back= TCOD_color_multiply(*back, col); break;
		case TCOD_BKGND_LIGHTEN :
			back->r = MAX(back->r,col.r);
			back->g = MAX(back->g,col.g);
			back->b = MAX(back->b,col.b);
		break;
		case TCOD_BKGND_DARKEN :
			back->r = MIN(back->r,col.r);
			back->g = MIN(back->g,col.g);
			back->b = MIN(back->b,col.b);
		break;
		case TCOD_BKGND_SCREEN :
			/* newbk = white - (white - oldbk) * (white - curbk) */
			back->r = (uint8)(255 - (int)(255 - back->r)*(255 - col.r)/255);
			back->g = (uint8)(255 - (int)(255 - back->g)*(255 - col.g)/255);
			back->b = (uint8)(255 - (int)(255 - back->b)*(255 - col.b)/255);
		break;
		case TCOD_BKGND_COLOR_DODGE :
			/* newbk = curbk / (white - oldbk) */
			if ( back->r != 255 ) newr = (int)(255 * col.r) / (255 - back->r);
			else newr=255;
			if ( back->g != 255 ) newg = (int)(255 * col.g) / (255 - back->g);
			else newg=255;
			if ( back->b != 255 ) newb = (int)(255 * col.b) / (255 - back->b);
			else newb=255;
			back->r=(uint8)CLAMP(0,255,newr);
			back->g=(uint8)CLAMP(0,255,newg);
			back->b=(uint8)CLAMP(0,255,newb);
		break;
		case TCOD_BKGND_COLOR_BURN :
			/* newbk = white - (white - oldbk) / curbk */
			if ( col.r > 0 ) newr = 255 - (int)(255 * (255 - back->r) )/col.r;
			else newr = 0;
			if ( col.g > 0 ) newg = 255 - (int)(255 * (255 - back->g)) /col.g;
			else newg = 0;
			if ( col.b > 0 ) newb = 255 - (int)(255 * (255 - back->b)) /col.b;
			else newb = 0;
			back->r=(uint8)CLAMP(0,255,newr);
			back->g=(uint8)CLAMP(0,255,newg);
			back->b=(uint8)CLAMP(0,255,newb);
		break;
		case TCOD_BKGND_ADD :
			/* newbk = oldbk + curbk */
			newr=(int)(back->r)+col.r;
			newg=(int)(back->g)+col.g;
			newb=(int)(back->b)+col.b;
			back->r=(uint8)CLAMP(0,255,newr);
			back->g=(uint8)CLAMP(0,255,newg);
			back->b=(uint8)CLAMP(0,255,newb);
		break;
		case TCOD_BKGND_ADDA :
			alpha=(flag >> 8);
			/* newbk = oldbk + alpha * curbk */
			newr=(int)(back->r)+alpha * col.r / 255;
			newg=(int)(back->g)+alpha * col.g / 255;
			newb=(int)(back->b)+alpha * col.b / 255;
			back->r=(uint8)CLAMP(0,255,newr);
			back->g=(uint8)CLAMP(0,255,newg);
			back->b=(uint8)CLAMP(0,255,newb);
		break;
		case TCOD_BKGND_BURN :
			/* newbk = oldbk + curbk - white */
			newr=(int)(back->r)+col.r-255;
			newg=(int)(back->g)+col.g-255;
			newb=(int)(back->b)+col.b-255;
			back->r=(uint8)CLAMP(0,255,newr);
			back->g=(uint8)CLAMP(0,255,newg);
			back->b=(uint8)CLAMP(0,255,newb);
		break;
		case TCOD_BKGND_OVERLAY :
			/* newbk = curbk.x <= 0.5 ? 2*curbk*oldbk : white - 2*(white-curbk)*(white-oldbk) */
			newr = col.r <= 128 ? 2 * (int)(col.r) * back->r / 255 : 255 - 2*(int)(255 - col.r)*(255-back->r)/255;
			newg = col.g <= 128 ? 2 * (int)(col.g) * back->g / 255 : 255 - 2*(int)(255 - col.g)*(255-back->g)/255;
			newb = col.b <= 128 ? 2 * (int)(col.b) * back->b / 255 : 255 - 2*(int)(255 - col.b)*(255-back->b)/255;
			back->r=(uint8)CLAMP(0,255,newr);
			back->g=(uint8)CLAMP(0,255,newg);
			back->b=(uint8)CLAMP(0,255,newb);
		break;
		case TCOD_BKGND_ALPH :
			/* newbk = (1.0f-alpha)*oldbk + alpha*(curbk-oldbk) */
			alpha=(flag >> 8);
			*back = TCOD_color_lerp(*back,col,(float)(alpha/255.0f));
		break;
		default : break;
	}
}

void TCOD_console_set_char(TCOD_console_t con,int x, int y, int c) {
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	if ( (unsigned)(x) >= (unsigned)dat->w || (unsigned)(y) >= (unsigned)dat->h ) return;
	dat->buf[ y * dat->w + x ].c=c;
	dat->buf[ y * dat->w + x ].cf = TCOD_ctx.ascii_to_tcod[c];
}

static void TCOD_console_clamp(int cx, int cy, int cw, int ch, int *x, int *y, int *w, int *h) {
	if ( *x + *w > cw ) *w = cw - *x;
	if ( *y + *h > ch ) *h = ch - *y;
	if (*x < cx ) {
		*w -= cx - *x;
		*x = cx;
	}
	if (*y < cy ) {
		*h -= cy - *y;
		*y = cy;
	}
}

void TCOD_console_rect(TCOD_console_t con,int x,int y, int rw, int rh, bool clear, TCOD_bkgnd_flag_t flag) {
	int cx,cy;
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT ( dat != NULL ) return;
	TCOD_ASSERT((unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h );
	TCOD_ASSERT(x+rw <= dat->w && y+rh <= dat->h );

	TCOD_console_clamp(0,0,dat->w,dat->h,&x,&y,&rw,&rh);
	TCOD_IFNOT(rw > 0 && rh > 0) return;
	for (cx=x;cx < x+rw; cx++) {
		for (cy=y;cy<y+rh;cy++) {
			TCOD_console_set_back(con,cx,cy,dat->back,flag);
			if ( clear ) {
				dat->buf[cx + cy*dat->w].c=' ';
				dat->buf[cx + cy*dat->w].cf=TCOD_ctx.ascii_to_tcod[' '];
			}
		}
	}
}

void TCOD_console_hline(TCOD_console_t con,int x,int y, int l, TCOD_bkgnd_flag_t flag) {
	int i;
	for (i=x; i< x+l; i++) TCOD_console_put_char(con,i,y,TCOD_CHAR_HLINE,flag);
}

void TCOD_console_vline(TCOD_console_t con,int x,int y, int l, TCOD_bkgnd_flag_t flag) {
	int i;
	for (i=y; i< y+l; i++) TCOD_console_put_char(con,x,i,TCOD_CHAR_VLINE,flag);
}

char *TCOD_console_vsprint(const char *fmt, va_list ap) {
	#define NB_BUFFERS 10
	#define INITIAL_SIZE 512
	// several static buffers in case the function is used more than once in a single function call
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
		int len = vsnprintf(msg[curbuf],buflen[curbuf],fmt,ap);
		ok=true;
		if (len < 0 || len >= buflen[curbuf]) {
			// buffer too small.
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

void TCOD_console_print_frame(TCOD_console_t con,int x,int y,int w,int h, bool empty, TCOD_bkgnd_flag_t flag, const char *fmt, ...) {
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
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
		title[w-3]=0; // truncate if needed
		xs = x + (w-strlen(title)-2)/2;
		tmp=dat->back; // swap colors
		dat->back=dat->fore;
		dat->fore=tmp;
		TCOD_console_print_left(con,xs,y,TCOD_BKGND_SET," %s ",title);
		tmp=dat->back; // swap colors
		dat->back=dat->fore;
		dat->fore=tmp;
	}
}

void TCOD_console_print_left(TCOD_console_t con,int x, int y, TCOD_bkgnd_flag_t flag, const char *fmt, ...) {
	va_list ap;
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	va_start(ap,fmt);
	TCOD_console_print(con,x,y,dat->w-x,dat->h-y,flag,LEFT,TCOD_console_vsprint(fmt,ap), false, false);
	va_end(ap);
}

void TCOD_console_print_right(TCOD_console_t con,int x, int y, TCOD_bkgnd_flag_t flag, const char *fmt, ...) {
	va_list ap;
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	va_start(ap,fmt);
	TCOD_console_print(con,x,y,x+1,dat->h-y,flag,RIGHT,TCOD_console_vsprint(fmt,ap), false, false);
	va_end(ap);
}

void TCOD_console_print_center(TCOD_console_t con,int x, int y, TCOD_bkgnd_flag_t flag, const char *fmt, ...) {
	va_list ap;
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	va_start(ap,fmt);
	TCOD_console_print(con,x,y,dat->w,dat->h-y,flag,CENTER,TCOD_console_vsprint(fmt,ap), false, false);
	va_end(ap);
}

int TCOD_console_print_left_rect(TCOD_console_t con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret = TCOD_console_print(con,x,y,w,h,flag,LEFT,TCOD_console_vsprint(fmt,ap), true, false);
	va_end(ap);
	return ret;
}

int TCOD_console_print_right_rect(TCOD_console_t con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret=TCOD_console_print(con,x,y,w,h,flag,RIGHT,TCOD_console_vsprint(fmt,ap), true, false);
	va_end(ap);
	return ret;
}

int TCOD_console_print_center_rect(TCOD_console_t con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret=TCOD_console_print(con,x,y,w,h,flag,CENTER,TCOD_console_vsprint(fmt,ap), true, false);
	va_end(ap);
	return ret;
}

int TCOD_console_height_left_rect(TCOD_console_t con,int x, int y, int w, int h, const char *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret = TCOD_console_print(con,x,y,w,h,TCOD_BKGND_NONE,LEFT,TCOD_console_vsprint(fmt,ap), true, true);
	va_end(ap);
	return ret;
}

int TCOD_console_height_right_rect(TCOD_console_t con,int x, int y, int w, int h, const char *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret=TCOD_console_print(con,x,y,w,h,TCOD_BKGND_NONE,RIGHT,TCOD_console_vsprint(fmt,ap), true, true);
	va_end(ap);
	return ret;
}

int TCOD_console_height_center_rect(TCOD_console_t con,int x, int y, int w, int h, const char *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret=TCOD_console_print(con,x,y,w,h,TCOD_BKGND_NONE,CENTER,TCOD_console_vsprint(fmt,ap), true, true);
	va_end(ap);
	return ret;
}

// non public methods
int TCOD_console_stringLength(const char *s) {
	int l=0;
	while (*s) {
		if ( *s == (int)TCOD_COLCTRL_FORE_RGB || *s == (int)TCOD_COLCTRL_BACK_RGB ) s+=3;
		else if ( (unsigned)(*s) > (unsigned)TCOD_COLCTRL_STOP ) l++;
		s++;
	}
	return l;
}

char * TCOD_console_forward(char *s,int l) {
	while ( *s && l > 0 ) {
		if ( *s == (int)TCOD_COLCTRL_FORE_RGB || *s == (int)TCOD_COLCTRL_BACK_RGB ) s+=3;
		else if ( *s > (int)TCOD_COLCTRL_STOP ) l--;
		s++;
	}
	return s;
}

char *TCOD_console_strchr(char *s, char c) {
	while ( *s && *s != c ) {
		if ( *s == (int)TCOD_COLCTRL_FORE_RGB || *s == (int)TCOD_COLCTRL_BACK_RGB ) s+=3;
		s++;
	}
	return (*s ? s : NULL);
}

int TCOD_console_print(TCOD_console_t con,int x,int y, int rw, int rh, TCOD_bkgnd_flag_t flag,
	alignment_t align, char *msg, bool can_split, bool count_only) {
	char *c=msg;
	int cx=0,cy=y;
	int minx,maxx,miny,maxy;
	TCOD_color_t oldFore;
	TCOD_color_t oldBack;
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT ( dat != NULL
		&& (unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h )
		return 0;
	TCOD_IFNOT(msg != NULL) return 0;
	oldFore=dat->fore;
	oldBack=dat->back;
	miny=y;
	maxy=dat->h-1;
	if (rh > 0) maxy=MIN(maxy,y+rh-1);
	switch (align) {
		case LEFT : minx=MAX(0,x); maxx=MIN(dat->w-1,x+rw-1); break;
		case RIGHT : minx=MAX(0,x-rw+1); maxx=MIN(dat->w-1,x); break;
		case CENTER : default : minx=MAX(0,x-rw/2); maxx=MIN(dat->w-1,x+rw/2); break;
	}

	do {
		// get \n delimited sub-message
		char *end=TCOD_console_strchr(c,'\n');
		char bak=0;
		int cl;
		char *split=NULL;
		if ( end ) *end=0;
		cl= TCOD_console_stringLength(c);
		// find starting x
		switch (align) {
			case LEFT : cx=x; break;
			case RIGHT : cx=x-cl+1; break;
			case CENTER : cx= x-cl/2;break;
		}
		// check if the string is completely out of the minx,miny,maxx,maxy frame
		if ( cy >= miny && cy <= maxy && cx <= maxx && cx+cl -1 >= minx ) {
			if ( can_split && cy < maxy ) {
				// if partially out of screen, try to split the sub-message
				if ( cx < minx ) split = TCOD_console_forward(c, align == CENTER ? cl-2*(minx-cx) : cl-(minx-cx));
				else if ( align==CENTER ) {
					if ( cx + cl/2 > maxx+1 ) split = TCOD_console_forward(c, maxx+1 - cx);
				} else {
					if ( cx + cl > maxx+1 ) split = TCOD_console_forward(c, maxx+1 - cx);
				}
			}
			if ( split ) {
				char *oldsplit=split;
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
					case LEFT : cx=x; break;
					case RIGHT : cx=x-cl+1; break;
					case CENTER : cx= x-cl/2;break;
				}
			}
			if ( cx < minx ) {
				// truncate left part
				c += minx-cx;
				cl -= minx-cx;
				cx=minx;
			}
			if ( cx + cl > maxx+1 ) {
				// truncate right part
				split = TCOD_console_forward(c, maxx+1 - cx);
				*split=0;
			}
			// render the sub-message
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
					if (! count_only) TCOD_console_put_char(con,cx,cy,(unsigned char)(*c),flag);
					cx++;
				}
				c++;
			}
		}
		if ( end ) {
			// next line
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
	// several static buffers in case the function is used more than once in a single function call
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
		int len = vsnwprintf(msg[curbuf],buflen[curbuf],fmt,ap);
		ok=true;
		if (len < 0 || len >= buflen[curbuf]) {
			// buffer too small.
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

int TCOD_console_print_utf(TCOD_console_t con,int x,int y, int rw, int rh, TCOD_bkgnd_flag_t flag,
	alignment_t align, wchar_t *msg, bool can_split, bool count_only) {
	wchar_t *c=msg;
	int cx=0,cy=y;
	int minx,maxx,miny,maxy;
	TCOD_color_t oldFore;
	TCOD_color_t oldBack;
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT ( dat != NULL
		&& (unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h )
		return 0;
	TCOD_IFNOT(msg != NULL) return 0;
	oldFore=dat->fore;
	oldBack=dat->back;
	miny=y;
	maxy=dat->h-1;
	if (rh > 0) maxy=MIN(maxy,y+rh-1);
	switch (align) {
		case LEFT : minx=MAX(0,x); maxx=MIN(dat->w-1,x+rw-1); break;
		case RIGHT : minx=MAX(0,x-rw+1); maxx=MIN(dat->w-1,x); break;
		case CENTER : default : minx=MAX(0,x-rw/2); maxx=MIN(dat->w-1,x+rw/2); break;
	}

	do {
		// get \n delimited sub-message
		wchar_t *end=TCOD_console_strchr_utf(c,'\n');
		wchar_t bak=0;
		int cl;
		wchar_t *split=NULL;
		if ( end ) *end=0;
		cl= TCOD_console_stringLength_utf(c);
		// find starting x
		switch (align) {
			case LEFT : cx=x; break;
			case RIGHT : cx=x-cl+1; break;
			case CENTER : cx= x-cl/2;break;
		}
		// check if the string is completely out of the minx,miny,maxx,maxy frame
		if ( cy >= miny && cy <= maxy && cx <= maxx && cx+cl -1 >= minx ) {
			if ( can_split && cy < maxy ) {
				// if partially out of screen, try to split the sub-message
				if ( cx < minx ) split = TCOD_console_forward_utf(c, align == CENTER ? cl-2*(minx-cx) : cl-(minx-cx));
				else if ( align==CENTER ) {
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
					case LEFT : cx=x; break;
					case RIGHT : cx=x-cl+1; break;
					case CENTER : cx= x-cl/2;break;
				}
			}
			if ( cx < minx ) {
				// truncate left part
				c += minx-cx;
				cl -= minx-cx;
				cx=minx;
			}
			if ( cx + cl > maxx+1 ) {
				// truncate right part
				split = TCOD_console_forward_utf(c, maxx+1 - cx);
				*split=0;
			}
			// render the sub-message
			if ( cy >= 0 && cy < dat->h )
			while (*c) {
				if ( *c >= TCOD_COLCTRL_1 && *c <= TCOD_COLCTRL_NUMBER ) {
					dat->fore=color_control_fore[(int)(*c)-1];
					dat->back=color_control_back[(int)(*c)-1];
				} else if ( *c == TCOD_COLCTRL_FORE_RGB ) {
					c++;
					dat->fore.r=(uint8)(*c++);
					dat->fore.g=(uint8)(*c++);
					dat->fore.b=(uint8)(*c);
				} else if ( *c == TCOD_COLCTRL_BACK_RGB ) {
					c++;
					dat->back.r=(uint8)(*c++);
					dat->back.g=(uint8)(*c++);
					dat->back.b=(uint8)(*c);
				} else if ( *c == TCOD_COLCTRL_STOP ) {
					dat->fore=oldFore;
					dat->back=oldBack;
				} else {
					if (! count_only) TCOD_console_put_char(con,cx,cy,(unsigned char)(*c),flag);
					cx++;
				}
				c++;
			}
		}
		if ( end ) {
			// next line
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

void TCOD_console_print_left_utf(TCOD_console_t con,int x, int y, TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...) {
	va_list ap;
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	va_start(ap,fmt);
	TCOD_console_print_utf(con,x,y,dat->w-x,dat->h-y,flag,LEFT,TCOD_console_vsprint_utf(fmt,ap), false, false);
	va_end(ap);
}

void TCOD_console_print_right_utf(TCOD_console_t con,int x, int y, TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...) {
	va_list ap;
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	va_start(ap,fmt);
	TCOD_console_print_utf(con,x,y,x+1,dat->h-y,flag,RIGHT,TCOD_console_vsprint_utf(fmt,ap), false, false);
	va_end(ap);
}

void TCOD_console_print_center_utf(TCOD_console_t con,int x, int y, TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...) {
	va_list ap;
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	va_start(ap,fmt);
	TCOD_console_print_utf(con,x,y,dat->w,dat->h-y,flag,CENTER,TCOD_console_vsprint_utf(fmt,ap), false, false);
	va_end(ap);
}

int TCOD_console_print_left_rect_utf(TCOD_console_t con,int x, int y, int w, int h,TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret = TCOD_console_print_utf(con,x,y,w,h,flag,LEFT,TCOD_console_vsprint_utf(fmt,ap), true, false);
	va_end(ap);
	return ret;
}

int TCOD_console_print_right_rect_utf(TCOD_console_t con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret=TCOD_console_print_utf(con,x,y,w,h,flag,RIGHT,TCOD_console_vsprint_utf(fmt,ap), true, false);
	va_end(ap);
	return ret;
}

int TCOD_console_print_center_rect_utf(TCOD_console_t con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret=TCOD_console_print_utf(con,x,y,w,h,flag,CENTER,TCOD_console_vsprint_utf(fmt,ap), true, false);
	va_end(ap);
	return ret;
}

int TCOD_console_height_left_rect_utf(TCOD_console_t con,int x, int y, int w, int h, const wchar_t *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret = TCOD_console_print_utf(con,x,y,w,h,TCOD_BKGND_NONE,LEFT,TCOD_console_vsprint_utf(fmt,ap), true, true);
	va_end(ap);
	return ret;
}

int TCOD_console_height_right_rect_utf(TCOD_console_t con,int x, int y, int w, int h, const wchar_t *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret=TCOD_console_print_utf(con,x,y,w,h,TCOD_BKGND_NONE,RIGHT,TCOD_console_vsprint_utf(fmt,ap), true, true);
	va_end(ap);
	return ret;
}

int TCOD_console_height_center_rect_utf(TCOD_console_t con,int x, int y, int w, int h, const wchar_t *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret=TCOD_console_print_utf(con,x,y,w,h,TCOD_BKGND_NONE,CENTER,TCOD_console_vsprint_utf(fmt,ap), true, true);
	va_end(ap);
	return ret;
}


#endif

void TCOD_console_init_root(int w, int h, const char*title, bool fullscreen, TCOD_renderer_t renderer) {
	TCOD_IF(w > 0 && h > 0) {
		TCOD_console_data_t *con=(TCOD_console_data_t *)calloc(sizeof(TCOD_console_data_t),1);
		int i;
		con->w=w;
		con->h=h;
		TCOD_ctx.root=con;
		TCOD_ctx.renderer=renderer;
		for (i=0; i < TCOD_COLCTRL_NUMBER; i++) {
			color_control_fore[i]=TCOD_white;
			color_control_back[i]=TCOD_black;
		}
		TCOD_console_init((TCOD_console_t)con,title,fullscreen);
	}
}

bool TCOD_console_init(TCOD_console_t con,const char *title, bool fullscreen) {
	int i;
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT(dat != NULL) return false;
	dat->fore=TCOD_white;
	dat->back=TCOD_black;
	dat->fade=255;
	dat->buf = (char_t *)calloc(sizeof(char_t),dat->w*dat->h);
	dat->oldbuf = (char_t *)calloc(sizeof(char_t),dat->w*dat->h);
	windowClosed = false;
	for (i=0; i< dat->w*dat->h; i++) {
		dat->buf[i].c=' ';
		dat->buf[i].cf=-1;
	}
	if ( title ) {
		if (! TCOD_sys_init(dat->w,dat->h,dat->buf,dat->oldbuf,fullscreen) ) return false;
		TCOD_sys_set_window_title(title);
	}
	return true;
}

void TCOD_console_set_foreground_color(TCOD_console_t con,TCOD_color_t col) {
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT(dat != NULL) return;
	dat->fore=col;
}

void TCOD_console_set_background_color(TCOD_console_t con,TCOD_color_t col) {
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT(dat != NULL) return;
	dat->back=col;
}

TCOD_color_t TCOD_console_get_foreground_color(TCOD_console_t con) {
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT(dat != NULL) return TCOD_white;
	return dat->fore;
}

TCOD_color_t TCOD_console_get_background_color(TCOD_console_t con) {
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT(dat != NULL) return TCOD_black;
	return dat->back;
}

int TCOD_console_get_width(TCOD_console_t con) {
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT(dat != NULL) return 0;
	return dat->w;
}

int TCOD_console_get_height(TCOD_console_t con) {
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT(dat != NULL) return 0;
	return dat->h;
}

char_t *TCOD_console_get_buf(TCOD_console_t con) {
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT(dat != NULL) return NULL;
	return dat->buf;
}

void TCOD_console_set_custom_font(const char *fontFile, int flags,int nb_char_horiz, int nb_char_vertic) {
	TCOD_sys_set_custom_font(fontFile, nb_char_horiz, nb_char_vertic, flags);
}

void TCOD_console_map_ascii_code_to_font(int asciiCode, int fontCharX, int fontCharY) {
	TCOD_sys_map_ascii_to_font(asciiCode, fontCharX, fontCharY);
}

void TCOD_console_map_ascii_codes_to_font(int asciiCode, int nbCodes, int fontCharX, int fontCharY) {
	int c;
	TCOD_IFNOT(asciiCode >= 0 && asciiCode+nbCodes <= TCOD_ctx.max_font_chars) return;
	for (c=asciiCode; c < asciiCode+nbCodes; c++ ) {
		TCOD_sys_map_ascii_to_font(c, fontCharX, fontCharY);
		fontCharX++;
		if ( fontCharX == TCOD_ctx.fontNbCharHoriz ) {
			fontCharX=0;
			fontCharY++;
		}
	}
}

void TCOD_console_map_string_to_font(const char *s, int fontCharX, int fontCharY) {
	TCOD_IFNOT(s != NULL) return;
	while (*s) {
		TCOD_console_map_ascii_code_to_font(*s, fontCharX, fontCharY);
		fontCharX++;
		if ( fontCharX == TCOD_ctx.fontNbCharHoriz ) {
			fontCharX=0;
			fontCharY++;
		}
		s++;
	}
}

void TCOD_console_set_keyboard_repeat(int initial_delay, int interval) {
	TCOD_sys_set_keyboard_repeat(initial_delay,interval);
}

void TCOD_console_disable_keyboard_repeat() {
	TCOD_sys_set_keyboard_repeat(0,0);
}

bool TCOD_console_is_key_pressed(TCOD_keycode_t key) {
	return TCOD_sys_is_key_pressed(key);
}
void TCOD_console_set_key_color(TCOD_console_t con,TCOD_color_t col) {
	TCOD_console_data_t *dat=con ? (TCOD_console_data_t *)con : TCOD_ctx.root;
	TCOD_IFNOT(dat != NULL) return;
	dat->key = col;
	dat->haskey=true;
}

void TCOD_console_credits() {
	bool end=false;
	int x=TCOD_console_get_width(NULL)/2-6;
	int y=TCOD_console_get_height(NULL)/2;
	int fade=260;
	TCOD_sys_save_fps();
	TCOD_sys_set_fps(25);
	while (!end ) {
		TCOD_key_t k;
		end=TCOD_console_credits_render(x,y,false);
		k=TCOD_console_check_for_keypress(TCOD_KEY_PRESSED);
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

void TCOD_console_credits_reset() {
	init2=false;
}

bool TCOD_console_credits_render(int x, int y, bool alpha) {
	static char poweredby[128];
	static float char_heat[128];
	static int char_x[128];
	static int char_y[128];
	static bool init1=false;
	static int len,len1,cw,ch;
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
	// mini particule system
#define MAX_PARTICULES 50
	static float pheat[MAX_PARTICULES];
	static float px[MAX_PARTICULES],py[MAX_PARTICULES], pvx[MAX_PARTICULES],pvy[MAX_PARTICULES];
	static int nbpart=0, firstpart=0;
	static float partDelay=0.1f;
	float elapsed=TCOD_sys_get_last_frame_length();
	TCOD_color_t fbackup; // backup fg color

	if (!init1) {
		// initialize all static data, colormaps, ...
		int width,height;
		TCOD_color_t col;
		TCOD_color_gen_map(colmap,4,colkeys,colpos);
		TCOD_color_gen_map(colmap_light,4,colkeys_light,colpos);
		cw=TCOD_console_get_width(NULL);
		ch=TCOD_console_get_height(NULL);
		sprintf(poweredby,"Powered by\n%s",version_string);
		noise=TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST,TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
		len=strlen(poweredby);
		len1=11; // sizeof "Powered by\n"
		left=MAX(x-4,0);
		right=MIN(x+len,cw-1);
		top=MAX(y-4,0);
		bottom=MIN(y+6,ch-1);
		width=right - left + 1;
		height=bottom - top + 1;
		col= TCOD_console_get_background_color(NULL);
		TCOD_console_set_background_color(NULL,TCOD_black);
		img = TCOD_image_new(width*2,height*2);
		TCOD_console_set_background_color(NULL,col);
		init1=true;
	}
	if (!init2) {
		// reset the credits vars ...
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
	fbackup=TCOD_console_get_foreground_color(NULL);
	if ( xstr < (float)len1 ) {
		sparklex=x+xstr;
		sparkley=(float)y;
	} else {
		sparklex=x-len1+xstr;
		sparkley=(float)y+1;
	}
	noisex=xstr*6;
	sparklerad=3.0f+2*TCOD_noise_simplex(noise,&noisex);
	if ( xstr >= len-1 ) sparklerad -= (xstr-len+1)*4.0f;
	else if ( xstr < 0.0f ) sparklerad += xstr*4.0f;
	else if ( poweredby[ (int)(xstr+0.5f) ] == ' ' || poweredby[ (int)(xstr+0.5f) ] == '\n' ) sparklerad/=2;
	sparklerad2=sparklerad*sparklerad*4;

	// draw the light
	for (xc=left*2,xi=0; xc < (right+1)*2; xc++,xi++) {
		for (yc=top*2,yi=0; yc < (bottom+1)*2; yc++,yi++) {
			float dist=((xc-2*sparklex)*(xc-2*sparklex)+(yc-2*sparkley)*(yc-2*sparkley));
			TCOD_color_t pixcol;
			if ( sparklerad >= 0.0f && dist < sparklerad2 ) {
				int colidx=63-(int)(63*(sparklerad2-dist)/sparklerad2) + TCOD_random_get_int(NULL,-10,10);
				colidx=CLAMP(0,63,colidx);
				pixcol=colmap_light[colidx];
			} else {
				pixcol=TCOD_black;
			}
			if ( alpha ) {
				//	console cells have following flag values :
				//		1 2
				//		4 8
				//	flag indicates which subcell uses foreground color
				static int asciiToFlag[] = {
					1, // TCOD_CHAR_SUBP_NW
					2, // TCOD_CHAR_SUBP_NE
					3, // TCOD_CHAR_SUBP_N
					8, // TCOD_CHAR_SUBP_SE
					9, // TCOD_CHAR_SUBP_DIAG
					10, // TCOD_CHAR_SUBP_E
					4, // TCOD_CHAR_SUBP_SW
				};
				int conc= TCOD_console_get_char(NULL,xc/2,yc/2);
				TCOD_color_t bk=TCOD_console_get_back(NULL,xc/2,yc/2);
				if ( conc >= TCOD_CHAR_SUBP_NW && conc <= TCOD_CHAR_SUBP_SW ) {
					// merge two subcell chars...
					// get the flag for the existing cell on root console
					int bkflag=asciiToFlag[conc - TCOD_CHAR_SUBP_NW ];
					int xflag = (xc & 1);
					int yflag = (yc & 1);
					// get the flag for the current subcell
					int credflag = (1+3*yflag) * (xflag+1);
					if ( (credflag & bkflag) != 0 ) {
						// the color for this subcell on root console
						// is foreground, not background
						bk = TCOD_console_get_fore(NULL,xc/2,yc/2);
					}
				}
				pixcol.r = MIN(255,(int)(bk.r)+pixcol.r);
				pixcol.g = MIN(255,(int)(bk.g)+pixcol.g);
				pixcol.b = MIN(255,(int)(bk.b)+pixcol.b);
			}
			TCOD_image_put_pixel(img,xi,yi,pixcol);
		}
	}

	// draw and update the particules
	j=nbpart;i=firstpart;
	while (j > 0) {
		int colidx=(int)(64*(1.0f-pheat[i]));
		TCOD_color_t col;
		colidx=MIN(63,colidx);
		col=colmap[colidx];
		if ( (int)py[i]< (bottom-top+1)*2 ) {
			int ipx = (int)px[i];
			int ipy = (int)py[i];
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
		// fire a new particule
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
	// draw the text
	for (i=0; i < len ;i++) {
		if ( char_heat[i] >= 0.0f && poweredby[i]!='\n') {
			int colidx=(int)(64*char_heat[i]);
			TCOD_color_t col;
			colidx=MIN(63,colidx);
			col=colmap[colidx];
			if ( xstr >= len  ) {
				float coef=(xstr-len)/len;
				if ( alpha ) {
					TCOD_color_t fore=TCOD_console_get_back(NULL,char_x[i],char_y[i]);
					int r=(int)(coef*fore.r + (1.0f-coef)*col.r);
					int g=(int)(coef*fore.g + (1.0f-coef)*col.g);
					int b=(int)(coef*fore.b + (1.0f-coef)*col.b);
					col.r = CLAMP(0,255,r);
					col.g = CLAMP(0,255,g);
					col.b = CLAMP(0,255,b);
					TCOD_console_set_fore(NULL,char_x[i],char_y[i],col);
				} else {
					col=TCOD_color_lerp(col,TCOD_black,coef);
				}
			}
			TCOD_console_set_char(NULL,char_x[i],char_y[i],poweredby[i]);
			TCOD_console_set_fore(NULL,char_x[i],char_y[i],col);
		}
	}
	// update letters heat
	xstr += elapsed * 4;
	for (i=0; i < (int)(xstr+0.5f); i++) {
		char_heat[i]=(xstr-i)/(len/2);
	}
	// restore fg color
	TCOD_console_set_foreground_color(NULL,fbackup);
	if ( xstr <= 2*len ) return false;
	init2=false;
	return true;
}


