/*
* libtcod 1.4.0
* Copyright (c) 2008 J.C.Wilk
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of J.C.Wilk may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY J.C.WILK ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL J.C.WILK BE LIABLE FOR ANY
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

// what string
#if defined( VISUAL_STUDIO ) || defined( BORLANDC )
static const char *version_string ="libtcod 1.4.0";
#else
static const char *version_string __attribute__((unused)) ="libtcod 1.4.0";
#endif

static bool windowClosed=false;
static TCOD_console_t root = NULL;

static TCOD_color_t color_control_fore[TCOD_COLCTRL_NUMBER];
static TCOD_color_t color_control_back[TCOD_COLCTRL_NUMBER];

static TCOD_color_t fading_color={0,0,0};
static uint8 fade=255;

void TCOD_console_set_color_control(TCOD_colctrl_t con, TCOD_color_t fore, TCOD_color_t back) {
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
	TCOD_console_data_t *con=(TCOD_console_data_t *)calloc(sizeof(TCOD_console_data_t),1);
	con->w=w;
	con->h=h;
	TCOD_console_init(con,NULL,false);
	return (TCOD_console_t)con;
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
	TCOD_console_data_t *dat=(TCOD_console_data_t *)(root);
	TCOD_sys_set_fullscreen(fullscreen);
	dat->fullscreen=fullscreen;
}

bool TCOD_console_is_fullscreen() {
	TCOD_console_data_t *dat=(TCOD_console_data_t *)(root);
	return dat->fullscreen;
}

void TCOD_console_delete(TCOD_console_t con) {
    TCOD_console_data_t *dat;
	if (! con ) {
		con=root;
		TCOD_sys_term();
		root=NULL;
	}
	dat=(TCOD_console_data_t *)(con);
	free(dat->buf);
	free(dat->oldbuf);
	free(dat);
}

void TCOD_console_blit(TCOD_console_t srcCon,int xSrc, int ySrc, int wSrc, int hSrc,
	TCOD_console_t dstCon, int xDst, int yDst, int fade_val) {
    TCOD_console_data_t *src;
    TCOD_console_data_t *dst;
	int cx,cy;
	if (! srcCon ) srcCon=root;
	if (! dstCon ) dstCon=root;
	src=(TCOD_console_data_t *)(srcCon);
	dst=(TCOD_console_data_t *)(dstCon);
	for (cx = xSrc; cx < xSrc+wSrc; cx++) {
		for (cy = ySrc; cy < ySrc+hSrc; cy++) {
			char_t srcChar=src->buf[cy * src->w+cx];
			if (! src->haskey || srcChar.back.r != src->key.r
				|| srcChar.back.g != src->key.g || srcChar.back.b != src->key.b ) {
				int dx=cx-xSrc+xDst;
				int dy=cy-ySrc+yDst;
				char_t dstChar;
				if ( fade_val == 255 ) {
					dstChar=srcChar;
				} else {
					float ffade=(float)fade_val/255.0f;
					dstChar=dst->buf[dy * dst->w + dx];

					dstChar.back = TCOD_color_lerp(dstChar.back,srcChar.back,ffade);
					if ( srcChar.c == ' ' ) {
						dstChar.fore = TCOD_color_lerp(dstChar.fore,srcChar.back,ffade);
					} else if (dstChar.c == ' ') {
						dstChar.c=srcChar.c;
						dstChar.cf=srcChar.cf;
						dstChar.fore = TCOD_color_lerp(dstChar.back,srcChar.fore,ffade);
					} else if (dstChar.c == srcChar.c) {
						dstChar.fore = TCOD_color_lerp(dstChar.fore,srcChar.fore,ffade);
					} else {
						if ( fade_val < 128 ) {
							dstChar.fore=TCOD_color_lerp(dstChar.fore,dstChar.back,
								(float)(fade_val)/127);
						} else {
							dstChar.c=srcChar.c;
							dstChar.cf=srcChar.cf;
							dstChar.fore=TCOD_color_lerp(dstChar.back,srcChar.fore,
								(float)(fade_val-128)/127);
						}
					}
				}
				dst->buf[dy * dst->w + dx] = dstChar;
			}
		}
	}
}


void TCOD_console_flush() {
	TCOD_console_data_t *dat=(TCOD_console_data_t *)root;
	TCOD_sys_flush(true);
	memcpy(dat->oldbuf,dat->buf,sizeof(char_t)*
		dat->w*dat->h);
}

void TCOD_console_set_fade(uint8 val, TCOD_color_t fadecol) {
	fade=val;
	fading_color=fadecol;
}

uint8 TCOD_console_get_fade() {
	return fade;
}

TCOD_color_t TCOD_console_get_fading_color() {
	return fading_color;
}


void TCOD_console_put_char(TCOD_console_t con,int x, int y, int c, TCOD_bkgnd_flag_t flag) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	if ( (unsigned)(x) >= (unsigned)dat->w || (unsigned)(y) >= (unsigned)dat->h ) return;
	dat->buf[ y * dat->w + x ].c = c;
	dat->buf[ y * dat->w + x ].cf = ascii_to_tcod[c];
	dat->buf[ y * dat->w + x ].fore=dat->fore;
	TCOD_console_set_back(con,x,y,dat->back,(TCOD_bkgnd_flag_t)flag);
}

void TCOD_console_clear(TCOD_console_t con) {
	TCOD_console_data_t *dat;
	int x,y;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	for (x=0; x < dat->w;x++) {
		for (y=0; y < dat->h; y++) {
			int off=x+dat->w*y;
			dat->buf[off].dirt=0;
			dat->buf[off].c=' ';
			dat->buf[off].cf=ascii_to_tcod[' '];
			dat->buf[off].fore=dat->fore;
			dat->buf[off].back=dat->back;
		}
	}
}

TCOD_color_t TCOD_console_get_back(TCOD_console_t con,int x, int y) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	return dat->buf[ y * dat->w + x ].back;
}
void TCOD_console_set_fore(TCOD_console_t con,int x,int y, TCOD_color_t col) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	if ( (unsigned)(x) >= (unsigned)dat->w || (unsigned)(y) >= (unsigned)dat->h ) return;
	dat->buf[ y * dat->w + x ].fore=col;
}
TCOD_color_t TCOD_console_get_fore(TCOD_console_t con,int x, int y) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	return dat->buf[ y * dat->w + x ].fore;
}

int TCOD_console_get_char(TCOD_console_t con,int x, int y) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	return dat->buf[ y * dat->w + x ].c;
}

void TCOD_console_set_back(TCOD_console_t con,int x, int y, TCOD_color_t col, TCOD_bkgnd_flag_t flag) {
	TCOD_console_data_t *dat;
	TCOD_color_t *back;
	int newr,newg,newb;
	int alpha;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	if ( (unsigned)(x) >= (unsigned)dat->w || (unsigned)(y) >= (unsigned)dat->h ) return;
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
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	if ( (unsigned)(x) >= (unsigned)dat->w || (unsigned)(y) >= (unsigned)dat->h ) return;
	dat->buf[ y * dat->w + x ].c=c;
	dat->buf[ y * dat->w + x ].cf = ascii_to_tcod[c];
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
    TCOD_console_data_t *dat;
	int cx,cy;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)(con);
	TCOD_console_clamp(0,0,dat->w,dat->h,&x,&y,&rw,&rh);
	for (cx=x;cx < x+rw; cx++) {
		for (cy=y;cy<y+rh;cy++) {
			TCOD_console_set_back(con,cx,cy,dat->back,flag);
			if ( clear ) {
				dat->buf[cx + cy*dat->w].c=' ';
				dat->buf[cx + cy*dat->w].cf=ascii_to_tcod[' '];
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

void TCOD_console_print_frame(TCOD_console_t con,int x,int y,int w,int h, bool empty, const char *fmt, ...) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_console_put_char(con,x,y,TCOD_CHAR_NW,TCOD_BKGND_SET);
	TCOD_console_put_char(con,x+w-1,y,TCOD_CHAR_NE,TCOD_BKGND_SET);
	TCOD_console_put_char(con,x,y+h-1,TCOD_CHAR_SW,TCOD_BKGND_SET);
	TCOD_console_put_char(con,x+w-1,y+h-1,TCOD_CHAR_SE,TCOD_BKGND_SET);
	TCOD_console_hline(con,x+1,y,w-2, TCOD_BKGND_SET);
	TCOD_console_hline(con,x+1,y+h-1,w-2, TCOD_BKGND_SET);
	if ( h > 2 ) {
		TCOD_console_vline(con,x,y+1,h-2, TCOD_BKGND_SET);
		TCOD_console_vline(con,x+w-1,y+1,h-2, TCOD_BKGND_SET);
		if ( empty ) {
			TCOD_console_rect(con,x+1,y+1,w-2,h-2,true,TCOD_BKGND_SET);
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
	TCOD_console_data_t *dat;
	va_list ap;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	va_start(ap,fmt);
	TCOD_console_print(con,x,y,dat->w-x,dat->h-y,flag,LEFT,TCOD_console_vsprint(fmt,ap), false);
	va_end(ap);
}

void TCOD_console_print_right(TCOD_console_t con,int x, int y, TCOD_bkgnd_flag_t flag, const char *fmt, ...) {
	TCOD_console_data_t *dat;
	va_list ap;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	va_start(ap,fmt);
	TCOD_console_print(con,x,y,x+1,dat->h-y,flag,RIGHT,TCOD_console_vsprint(fmt,ap), false);
	va_end(ap);
}

void TCOD_console_print_center(TCOD_console_t con,int x, int y, TCOD_bkgnd_flag_t flag, const char *fmt, ...) {
	TCOD_console_data_t *dat;
	va_list ap;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	va_start(ap,fmt);
	TCOD_console_print(con,x,y,dat->w,dat->h-y,flag,CENTER,TCOD_console_vsprint(fmt,ap), false);
	va_end(ap);
}

int TCOD_console_print_left_rect(TCOD_console_t con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret = TCOD_console_print(con,x,y,w,h,flag,LEFT,TCOD_console_vsprint(fmt,ap), true);
	va_end(ap);
	return ret;
}

int TCOD_console_print_right_rect(TCOD_console_t con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret=TCOD_console_print(con,x,y,w,h,flag,RIGHT,TCOD_console_vsprint(fmt,ap), true);
	va_end(ap);
	return ret;
}

int TCOD_console_print_center_rect(TCOD_console_t con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret=TCOD_console_print(con,x,y,w,h,flag,CENTER,TCOD_console_vsprint(fmt,ap), true);
	va_end(ap);
	return ret;
}

// non public methods
int TCOD_console_stringLength(const char *s) {
	int l=0;
	while (*s) {
		if ( *s == (int)TCOD_COLCTRL_FORE_RGB || *s == (int)TCOD_COLCTRL_BACK_RGB ) s+=3;
		else if ( *s > (int)TCOD_COLCTRL_STOP ) l++;
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

int TCOD_console_print(TCOD_console_t con,int x,int y, int rw, int rh, TCOD_bkgnd_flag_t flag,
	alignment_t align, char *msg, bool can_split) {
	TCOD_console_data_t *dat;
	char *c=msg;
	int cx=0,cy=y;
	int minx,maxx,miny,maxy;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)(con);
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
		char *end=strchr(c,'\n');
		char bak=0;
		int cl;
		char *split=NULL;
		TCOD_color_t oldFore=dat->fore;
		TCOD_color_t oldBack=dat->back;
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
					TCOD_console_put_char(con,cx,cy,(unsigned char)(*c),flag);
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

void TCOD_console_init_root(int w, int h, const char*title, bool fullscreen) {
	TCOD_console_data_t *con=(TCOD_console_data_t *)calloc(sizeof(TCOD_console_data_t),1);
	int i;
	con->w=w;
	con->h=h;
	root=con;
	for (i=0; i < TCOD_COLCTRL_NUMBER; i++) {
		color_control_fore[i]=TCOD_white;
		color_control_back[i]=TCOD_black;
	}
	TCOD_console_init((TCOD_console_t)con,title,fullscreen);
}

bool TCOD_console_init(TCOD_console_t con,const char *title, bool fullscreen) {
	TCOD_console_data_t *dat;
	int i;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	dat->fore=TCOD_white;
	dat->back=TCOD_black;
	dat->fade=255;
	dat->fullscreen=fullscreen;
	dat->windowed=(title != NULL);
	dat->buf = (char_t *)calloc(sizeof(char_t),dat->w*dat->h);
	dat->oldbuf = (char_t *)calloc(sizeof(char_t),dat->w*dat->h);
	for (i=0; i< dat->w*dat->h; i++) {
		dat->buf[i].c=' ';
		dat->buf[i].cf=ascii_to_tcod[' '];
	}
	windowClosed = false;
	if ( title ) {
		if (! TCOD_sys_init(dat->w,dat->h,dat->buf,dat->oldbuf,fullscreen) ) return false;
		TCOD_sys_set_window_title(title);
	}
	return true;
}

void TCOD_console_set_foreground_color(TCOD_console_t con,TCOD_color_t col) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	dat->fore=col;
}

void TCOD_console_set_background_color(TCOD_console_t con,TCOD_color_t col) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	dat->back=col;
}

TCOD_color_t TCOD_console_get_foreground_color(TCOD_console_t con) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	if (! con ) return TCOD_white; /* return white if init_root was not called */
	dat=(TCOD_console_data_t *)con;
	return dat->fore;
}

TCOD_color_t TCOD_console_get_background_color(TCOD_console_t con) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	if (! con ) return TCOD_black; /* return black if init_root was not called */
	dat=(TCOD_console_data_t *)con;
	return dat->back;
}

int TCOD_console_get_width(TCOD_console_t con) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	return dat->w;
}

int TCOD_console_get_height(TCOD_console_t con) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	return dat->h;
}

char_t *TCOD_console_get_buf(TCOD_console_t con) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	return dat->buf;
}

void TCOD_console_set_custom_font(const char *fontFile,int char_width, int char_height, int flags) {
	TCOD_sys_set_custom_font(fontFile, char_width, char_height, flags);
}

void TCOD_console_map_ascii_code_to_font(int asciiCode, int fontCharX, int fontCharY) {
	TCOD_sys_map_ascii_to_font(asciiCode, fontCharX, fontCharY);
}

void TCOD_console_map_ascii_codes_to_font(int asciiCode, int nbCodes, int fontCharX, int fontCharY) {
	int c;
	for (c=asciiCode; c < asciiCode+nbCodes; c++ ) {
		TCOD_sys_map_ascii_to_font(c, fontCharX, fontCharY);
		fontCharX++;
		if ( fontCharX == fontNbCharHoriz ) {
			fontCharX=0;
			fontCharY++;
		}
	}
}

void TCOD_console_map_string_to_font(const char *s, int fontCharX, int fontCharY) {
	while (*s) {
		TCOD_console_map_ascii_code_to_font(*s, fontCharX, fontCharY);
		fontCharX++;
		if ( fontCharX == fontNbCharHoriz ) {
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
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	dat->key = col;
	dat->haskey=true;
}

void TCOD_console_credits() {
	bool end=false;
	int x=TCOD_console_get_width(NULL)/2-6;
	int y=TCOD_console_get_height(NULL)/2;
	int fade=260;
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
	TCOD_sys_set_fps(0);
}

bool TCOD_console_credits_render(int x, int y, bool alpha) {
	static char poweredby[128];
	static float char_heat[128];
	static int char_x[128];
	static int char_y[128];
	static bool init=false;
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
	int i,xc,yc;
	float sparklex,sparkley,sparklerad,sparklerad2,noisex;

	if (!init) {
		// initialize all static data, colormaps, ...
		int curx,cury;
		xstr=-4.0f;
		TCOD_color_gen_map(colmap,4,colkeys,colpos);
		TCOD_color_gen_map(colmap_light,4,colkeys_light,colpos);
		cury=y;
		cw=TCOD_console_get_width(NULL);
		ch=TCOD_console_get_height(NULL);
		sprintf(poweredby,"Powered by\n%s",version_string);
		len=strlen(poweredby);
		len1=11; // sizeof "Powered by\n"
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
		noise=TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST,TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
		init=true;
	}
	// draw the text
	for (i=0; i < len ;i++) {
		if ( char_heat[i] >= 0.0f && poweredby[i]!='\n') {
			int colidx=(int)(64*char_heat[i]);
			TCOD_color_t col;
			if ( colidx > 63 ) colidx=63;
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
	sparklerad2=sparklerad*sparklerad;
	// draw the light
	for (xc=MAX(x-4,0); xc < MIN(x+len,cw-1); xc++) {
		for (yc=MAX(y-4,0); yc < MAX(y+6,ch-1); yc++) {
			float dist=((xc-sparklex)*(xc-sparklex)+(yc-sparkley)*(yc-sparkley));
			if ( sparklerad >= 0.0f && dist < sparklerad2 ) {
				int colidx=63-(int)(63*(sparklerad2-dist)/sparklerad2);
				TCOD_console_set_back(NULL,xc,yc,colmap_light[colidx],alpha ? TCOD_BKGND_ADD: TCOD_BKGND_SET);
			} else {
				TCOD_console_set_back(NULL,xc,yc,TCOD_black,alpha ? TCOD_BKGND_ADD: TCOD_BKGND_SET);
			}
		}
	}
	// update letters heat
	xstr += TCOD_sys_get_last_frame_length() * 4;
	for (i=0; i < (int)(xstr+0.5f); i++) {
		char_heat[i]=(xstr-i)/(len/2);
	}
	return xstr > 2*len;
}


