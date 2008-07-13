/*
* libtcod 1.3.2
* Copyright (c) 2007,2008 J.C.Wilk
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

#include <SDL/SDL.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "libtcod.h"
#include "libtcod_int.h"

static SDL_Surface* screen;
static SDL_Surface* charmap=NULL;
static SDL_Surface* charcolmap=NULL;
static int consoleWidth=0;
static int consoleHeight=0;
static char_t *consoleBuffer=NULL;

// size of a character in the bitmap font
static int fontWidth=8;
static int fontHeight=8;
// number of characters in the bitmap font
static int fontNbCharHoriz=16;
static int fontNbCharVertic=16;
// font layout (character 0 to 15 on the first row or the first column)
static bool fontInRow=false;
// font transparent color
static TCOD_color_t fontKeyCol={0,0,0};

static int fullscreen_width=0;
static int fullscreen_height=0;
static int actual_fullscreen_width=0;
static int actual_fullscreen_height=0;
static int fullscreen_offsetx=0;
static int fullscreen_offsety=0;

static char font_file[512]="terminal.bmp";
static char window_title[512]="";

static bool fullscreen_on;

// minimum length for a frame (when fps are limited)
static int min_frame_length=0;
// number of frames in the last second
static int fps=0;
// current number of frames
static int cur_fps=0;
// length of the last rendering loop
static float last_frame_length=0.0f;

static TCOD_color_t *charcols=NULL;
static bool key_status[TCODK_CHAR+1];

// convert SDL vk to a char (depends on the keyboard layout)
static char vk_to_c[SDLK_LAST];

int nbBlit=0;
int nbMiss=0;

void TCOD_sys_set_custom_font(const char *fontFile,int cw, int ch, int nbcw, int nbch,bool row,TCOD_color_t key_color) {
	strcpy(font_file,fontFile);
	fontWidth=cw;
	fontHeight=ch;
	fontNbCharHoriz=nbcw;
	fontNbCharVertic=nbch;
	fontInRow=row;
	fontKeyCol=key_color;
}

static void find_resolution() {
	SDL_Rect **modes;
	int i,bestw,besth,wantedw,wantedh;
	wantedw=fullscreen_width>consoleWidth*fontWidth?fullscreen_width:consoleWidth*fontWidth;
	wantedh=fullscreen_height>consoleHeight*fontHeight?fullscreen_height:consoleHeight*fontHeight;
	actual_fullscreen_width=wantedw;
	actual_fullscreen_height=wantedh;
	modes=SDL_ListModes(NULL, SDL_FULLSCREEN);

	bestw=99999;
	besth=99999;
	if(modes != (SDL_Rect **)0 && modes != (SDL_Rect **)-1){
		for(i=0;modes[i];++i) {
			if (modes[i]->w >= wantedw && modes[i]->w <= bestw
				&& modes[i]->h >= wantedh && modes[i]->h <= besth
				&& SDL_VideoModeOK(modes[i]->w, modes[i]->h, 32, SDL_FULLSCREEN)) {
				bestw=modes[i]->w;
				besth=modes[i]->h;
			}
		}
	}
	if ( bestw != 99999) {
		actual_fullscreen_width=bestw;
		actual_fullscreen_height=besth;
	}
	//printf ("resolution : %dx%d =>%dx%d\n",wantedw,wantedh,bestw,besth);
}

void *TCOD_sys_create_bitmap_for_console(TCOD_console_t console) {
	int w,h;
	SDL_Surface *bitmap;
	w = TCOD_console_get_width(console) * fontWidth;
	h = TCOD_console_get_height(console) * fontHeight;
	bitmap=SDL_CreateRGBSurface(SDL_SWSURFACE,w,h,charmap->format->BitsPerPixel,
		charmap->format->Rmask,charmap->format->Gmask,charmap->format->Bmask,charmap->format->Amask);
	return (void *)bitmap;
}

void TCOD_sys_console_to_bitmap(void *vbitmap, int console_width, int console_height, char_t *console_buffer) {
	int x,y;
	SDL_Surface *bitmap=(SDL_Surface *)vbitmap;
	TCOD_color_t curbrush=TCOD_white;
	Uint32 sdl_back=0,sdl_fore=0, sdl_key=0;
	TCOD_color_t fading_color = TCOD_console_get_fading_color();
	uint8 fade = TCOD_console_get_fade();
	if ( SDL_MUSTLOCK( bitmap ) && SDL_LockSurface( bitmap ) < 0 ) return;

	SDL_SetColorKey(charcolmap,SDL_SRCCOLORKEY|SDL_RLEACCEL,SDL_MapRGB(charcolmap->format,fontKeyCol.r,fontKeyCol.g,fontKeyCol.b));
	sdl_key=SDL_MapRGB(charcolmap->format,fontKeyCol.r,fontKeyCol.g,fontKeyCol.b);
	for (y=0;y<console_height;y++) {
		for (x=0; x<console_width; x++) {
			SDL_Rect srcRect,dstRect;
			char_t *c=&console_buffer[x+y*console_width];
			int ascii=(int)c->c;
			TCOD_color_t *curtext = &charcols[ascii];
			TCOD_color_t b=c->back;
			TCOD_color_t f=c->fore;
			// text
			b.r = ((int)b.r) * fade / 255 + ((int)fading_color.r) * (255-fade)/255;
			b.g = ((int)b.g) * fade / 255  + ((int)fading_color.g) * (255-fade)/255;
			b.b = ((int)b.b) * fade / 255 + ((int)fading_color.b) * (255-fade)/255;
			if ( curbrush.r != b.r || curbrush.g != b.g || curbrush.b != b.b ) {
				sdl_back=SDL_MapRGB(charcolmap->format,b.r,b.g,b.b);
				curbrush=b;
			}
			if ( c->c != ' ' ) {
				f.r = ((int)f.r) * fade / 255 + ((int)fading_color.r) * (255-fade)/255;
				f.g = ((int)f.g) * fade / 255 + ((int)fading_color.g) * (255-fade)/255;
				f.b = ((int)f.b) * fade / 255 + ((int)fading_color.b) * (255-fade)/255;
				if ( f.r == 0 && f.g == 0 && f.b == 0 ) f.r=1;
				if ( curtext->r != f.r || curtext->g != f.g || curtext->b!=f.b ) {
					int xc0,yc0,xc,yc;
				    Uint8 bpp = charcolmap->format->BytesPerPixel;
					nbMiss++;
					sdl_fore=SDL_MapRGB(charcolmap->format,f.r,f.g,f.b);
					*curtext=f;
					if (fontInRow) {
						xc0=(uint32)(ascii%fontNbCharHoriz)*fontWidth;
						yc0=(uint32)(ascii/fontNbCharHoriz)*fontHeight;
					} else {
						xc0=(uint32)(ascii/fontNbCharVertic)*fontWidth;
						yc0=(uint32)(ascii%fontNbCharVertic)*fontHeight;
					}
					if ( SDL_MUSTLOCK(charcolmap) ) {
				        if ( SDL_LockSurface(charcolmap) < 0 ) return;
				    }
					for (xc=xc0; xc < xc0+fontWidth; xc++) {
						for (yc=yc0; yc < yc0+fontHeight; yc++) {
						    Uint8 *bits = ((Uint8 *)charcolmap->pixels)+yc*charcolmap->pitch+xc*bpp;
						    switch(bpp) {
						        case 1:
						        	if ( *((Uint8 *)(bits)) != (Uint8)sdl_key )
							            *((Uint8 *)(bits)) = (Uint8)sdl_fore;
					            break;
						        case 2: {
						            Uint16 oldcol=*((Uint16 *)(bits));
									if (
										(oldcol & charcolmap->format->Rmask) != (sdl_key & charcolmap->format->Rmask)
										|| (oldcol & charcolmap->format->Gmask) != (sdl_key & charcolmap->format->Gmask)
										|| (oldcol & charcolmap->format->Bmask) != (sdl_key & charcolmap->format->Bmask)
									) {
							            *((Uint16 *)(bits)) = (Uint16)sdl_fore;
							        }
								}
					            break;
						        case 3: { /* Format/endian independent */
						            Uint8 r, g, b;
						            Uint32 oldcol=*((Uint32 *)(bits));
									if (
										(oldcol & charcolmap->format->Rmask) != (sdl_key & charcolmap->format->Rmask)
										|| (oldcol & charcolmap->format->Gmask) != (sdl_key & charcolmap->format->Gmask)
										|| (oldcol & charcolmap->format->Bmask) != (sdl_key & charcolmap->format->Bmask)
									) {
		            					r = (sdl_fore>>charcolmap->format->Rshift)&0xFF;
							            g = (sdl_fore>>charcolmap->format->Gshift)&0xFF;
							            b = (sdl_fore>>charcolmap->format->Bshift)&0xFF;
							            *((bits)+charcolmap->format->Rshift/8) = r;
							            *((bits)+charcolmap->format->Gshift/8) = g;
							            *((bits)+charcolmap->format->Bshift/8) = b;
							        }
					            }
					            break;
						        case 4:
									if ( *((Uint32 *)(bits)) != sdl_key )
							            *((Uint32 *)(bits)) = (Uint32)sdl_fore;
					            break;
						    }
						}
					}
		    		if ( SDL_MUSTLOCK(charcolmap) ) {
				        SDL_UnlockSurface(charcolmap);
				    }
				}
			}
			dstRect.x=x*fontWidth;
			dstRect.y=y*fontHeight;
			dstRect.w=fontWidth;
			dstRect.h=fontHeight;
			if ( bitmap == screen && fullscreen_on ) {
				dstRect.x+=fullscreen_offsetx;
				dstRect.y+=fullscreen_offsety;
			}
			nbBlit++;
			SDL_FillRect(bitmap,&dstRect,sdl_back);
			if ( c->c != ' ') {
				if (fontInRow) {
					srcRect.x = (c->c%fontNbCharHoriz)*fontWidth;
					srcRect.y = (c->c/fontNbCharHoriz)*fontHeight;
				} else {
					srcRect.x = (c->c/fontNbCharVertic)*fontWidth;
					srcRect.y = (c->c%fontNbCharVertic)*fontHeight;
				}
				srcRect.w=fontWidth;
				srcRect.h=fontHeight;
				SDL_BlitSurface(charcolmap,&srcRect,bitmap,&dstRect);
			}
		}
	}
	if ( SDL_MUSTLOCK( bitmap ) ) SDL_UnlockSurface( bitmap );

}

void TCOD_sys_set_keyboard_repeat(int initial_delay, int interval) {
	SDL_EnableKeyRepeat(initial_delay,interval);
}

bool TCOD_sys_init(int w,int h, char_t *buf, bool fullscreen) {
	consoleWidth=w;
	consoleHeight=h;
	if (SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO) < 0 ) TCOD_fatal_nopar("SDL : cannot initialize");
	atexit(SDL_Quit);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
	if ( fullscreen ) {
		find_resolution();
		screen=SDL_SetVideoMode(actual_fullscreen_width,actual_fullscreen_height,32,SDL_FULLSCREEN);
		if ( screen == NULL ) TCOD_fatal_nopar("SDL : cannot set fullscreen video mode");
		SDL_ShowCursor(0);
		actual_fullscreen_width=screen->w;
		actual_fullscreen_height=screen->h;
		fullscreen_offsetx=(actual_fullscreen_width-consoleWidth*fontWidth)/2;
		fullscreen_offsety=(actual_fullscreen_height-consoleHeight*fontHeight)/2;
		SDL_FillRect(screen,0,0);
	} else {
		screen=SDL_SetVideoMode(w*fontWidth,h*fontHeight,32,0);
		if ( screen == NULL ) TCOD_fatal_nopar("SDL : cannot create window");
	}
	charmap=SDL_LoadBMP(font_file);
	if (charmap == NULL ) TCOD_fatal("SDL : cannot load %s",font_file);
	if ( charmap->w < fontNbCharHoriz * fontWidth 
		|| charmap->h < fontNbCharVertic * fontHeight ) TCOD_fatal("bitmap %s too small for %dx%d, %dx%d characters\n",
		font_file,fontWidth,fontHeight,fontNbCharHoriz,fontNbCharVertic);
	charcolmap=SDL_CreateRGBSurface(SDL_SWSURFACE,charmap->w,charmap->h,charmap->format->BitsPerPixel,
		charmap->format->Rmask,charmap->format->Gmask,charmap->format->Bmask,charmap->format->Amask);
	if ( SDL_MUSTLOCK( charcolmap ) ) SDL_LockSurface( charcolmap );
	SDL_BlitSurface(charmap,NULL,charcolmap,NULL);
	if ( SDL_MUSTLOCK( charcolmap ) ) SDL_UnlockSurface( charcolmap );
	SDL_EnableUNICODE(1);
	charcols = (TCOD_color_t *)calloc(sizeof(TCOD_color_t),fontNbCharHoriz*fontNbCharVertic);
	consoleBuffer=buf;
	fullscreen_on=fullscreen;
	memset(key_status,0,sizeof(bool)*(TCODK_CHAR+1));
	return true;
}

void TCOD_sys_save_bitmap(void *bitmap, const char *filename) {
	SDL_SaveBMP((SDL_Surface *)bitmap,filename);
}

void TCOD_sys_save_screenshot(const char *filename) {
	char buf[128];
	if ( filename == NULL ) {
		// generate filename
		int idx=0;
		do {
		    FILE *f=NULL;
			sprintf(buf,"./screenshot%03d.bmp",idx);
			f=fopen(buf,"rb");
			if ( ! f ) filename=buf;
			else {
				idx++;
				fclose(f);
			}
		} while(!filename);
	}
	TCOD_sys_save_bitmap((void *)screen,filename);
}

void TCOD_sys_set_fullscreen(bool fullscreen) {
	fullscreen_on=fullscreen;
	/*
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_InitSubSystem(SDL_INIT_VIDEO);
	charmap=SDL_LoadBMP(font_file);
	if (charmap == NULL ) TCOD_fatal("SDL : cannot load %s",font_file);
	memset(charcols,128,256*sizeof(TCOD_color_t));
	*/
	if ( fullscreen ) {
		find_resolution();
		screen=SDL_SetVideoMode(actual_fullscreen_width,actual_fullscreen_height,32,SDL_FULLSCREEN);
		if ( screen == NULL ) TCOD_fatal_nopar("SDL : cannot set fullscreen video mode");
		SDL_ShowCursor(0);
		actual_fullscreen_width=screen->w;
		actual_fullscreen_height=screen->h;
		fullscreen_offsetx=(actual_fullscreen_width-consoleWidth*fontWidth)/2;
		fullscreen_offsety=(actual_fullscreen_height-consoleHeight*fontHeight)/2;
		/*
		printf ("actual resolution : %dx%d\n",actual_fullscreen_width,actual_fullscreen_height);
		printf ("offset : %dx%d\n",fullscreen_offsetx,fullscreen_offsety);
		printf ("flags : %x bpp : %d bitspp : %d\n",screen->flags, screen->format->BytesPerPixel, screen->format->BitsPerPixel);
		*/
	} else {
		screen=SDL_SetVideoMode(consoleWidth*fontWidth,consoleHeight*fontHeight,32,0);
		if ( screen == NULL ) TCOD_fatal_nopar("SDL : cannot create window");
		SDL_ShowCursor(1);
	}
	/* SDL_WM_SetCaption(window_title,NULL); */
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}


void TCOD_sys_set_window_title(const char *title) {
	strcpy(window_title,title);
	SDL_WM_SetCaption(title,NULL);
}

void TCOD_sys_flush(bool render) {
	static uint32 old_time,new_time=0, elapsed=0;
	int32 frame_time,time_to_wait;
	if ( render ) {
		TCOD_sys_console_to_bitmap(screen,TCOD_console_get_width(NULL),TCOD_console_get_height(NULL),consoleBuffer);
		SDL_Flip(screen);
	}
	old_time=new_time;
	new_time=TCOD_sys_elapsed_milli();
	if ( new_time / 1000 != elapsed ) {
		/* update fps every second */
		fps=cur_fps;
		cur_fps=0;
		elapsed=new_time/1000;
	}
	/* if too fast, wait */
	frame_time=(new_time - old_time);
	last_frame_length = frame_time * 0.001f;
	cur_fps++;
	time_to_wait=min_frame_length-frame_time;
	if (old_time > 0 && time_to_wait > 0) {
		TCOD_sys_sleep_milli(time_to_wait);
		new_time = TCOD_sys_elapsed_milli();
		frame_time=(new_time - old_time);
	} 
	last_frame_length = frame_time * 0.001f;
}

static void TCOD_sys_convert_event(SDL_Event *ev, TCOD_key_t *ret) {
	SDL_KeyboardEvent *kev=&ev->key;
	ret->c=(char)kev->keysym.unicode;
	if ( ( kev->keysym.mod & (KMOD_LCTRL|KMOD_RCTRL) ) != 0 ) {
		// when pressing CTRL-A, we don't get unicode for 'a', but unicode for CTRL-A = 1. Fix it
		if ( kev->keysym.sym >= SDLK_a && kev->keysym.sym <= SDLK_z ) {
			ret->c = 'a'+(kev->keysym.sym - SDLK_a);
		}
	}
	if ( ev->type == SDL_KEYDOWN ) vk_to_c[kev->keysym.sym] = ret->c;
	else if (ev->type == SDL_KEYUP ) ret->c = vk_to_c[kev->keysym.sym];
	switch(kev->keysym.sym) {
		case SDLK_ESCAPE : ret->vk=TCODK_ESCAPE;break;
		case SDLK_SPACE : ret->vk=TCODK_SPACE; break;
		case SDLK_BACKSPACE : ret->vk=TCODK_BACKSPACE;break;
		case SDLK_TAB : ret->vk=TCODK_TAB;break;
		case SDLK_RETURN : ret->vk=TCODK_ENTER;break;
		case SDLK_PAUSE : ret->vk=TCODK_PAUSE;break;
		case SDLK_PAGEUP : ret->vk=TCODK_PAGEUP;break;
		case SDLK_PAGEDOWN : ret->vk=TCODK_PAGEDOWN;break;
		case SDLK_HOME : ret->vk=TCODK_HOME;break;
		case SDLK_END : ret->vk=TCODK_END;break;
		case SDLK_LALT : case SDLK_RALT : ret->vk=TCODK_ALT;break;
		case SDLK_LCTRL : case SDLK_RCTRL : ret->vk=TCODK_CONTROL;break;
		case SDLK_LSHIFT : case SDLK_RSHIFT : ret->vk=TCODK_SHIFT;break;
		case SDLK_PRINT : ret->vk=TCODK_PRINTSCREEN;break;
		case SDLK_LEFT : ret->vk=TCODK_LEFT;break;
		case SDLK_UP : ret->vk=TCODK_UP;break;
		case SDLK_RIGHT : ret->vk=TCODK_RIGHT;break;
		case SDLK_DOWN : ret->vk=TCODK_DOWN;break;
		case SDLK_F1 : ret->vk=TCODK_F1;break;
		case SDLK_F2 : ret->vk=TCODK_F2;break;
		case SDLK_F3 : ret->vk=TCODK_F3;break;
		case SDLK_F4 : ret->vk=TCODK_F4;break;
		case SDLK_F5 : ret->vk=TCODK_F5;break;
		case SDLK_F6 : ret->vk=TCODK_F6;break;
		case SDLK_F7 : ret->vk=TCODK_F7;break;
		case SDLK_F8 : ret->vk=TCODK_F8;break;
		case SDLK_F9 : ret->vk=TCODK_F9;break;
		case SDLK_F10 : ret->vk=TCODK_F10;break;
		case SDLK_F11 : ret->vk=TCODK_F11;break;
		case SDLK_F12 : ret->vk=TCODK_F12;break;
		case SDLK_0 : ret->vk=TCODK_0;break;
		case SDLK_1 : ret->vk=TCODK_1;break;
		case SDLK_2 : ret->vk=TCODK_2;break;
		case SDLK_3 : ret->vk=TCODK_3;break;
		case SDLK_4 : ret->vk=TCODK_4;break;
		case SDLK_5 : ret->vk=TCODK_5;break;
		case SDLK_6 : ret->vk=TCODK_6;break;
		case SDLK_7 : ret->vk=TCODK_7;break;
		case SDLK_8 : ret->vk=TCODK_8;break;
		case SDLK_9 : ret->vk=TCODK_9;break;
		case SDLK_KP0 : ret->vk=TCODK_KP0;break;
		case SDLK_KP1 : ret->vk=TCODK_KP1;break;
		case SDLK_KP2 : ret->vk=TCODK_KP2;break;
		case SDLK_KP3 : ret->vk=TCODK_KP3;break;
		case SDLK_KP4 : ret->vk=TCODK_KP4;break;
		case SDLK_KP5 : ret->vk=TCODK_KP5;break;
		case SDLK_KP6 : ret->vk=TCODK_KP6;break;
		case SDLK_KP7 : ret->vk=TCODK_KP7;break;
		case SDLK_KP8 : ret->vk=TCODK_KP8;break;
		case SDLK_KP9 : ret->vk=TCODK_KP9;break;
		case SDLK_KP_DIVIDE : ret->vk=TCODK_KPDIV;break;
		case SDLK_KP_MULTIPLY : ret->vk=TCODK_KPMUL;break;
		case SDLK_KP_PLUS : ret->vk=TCODK_KPADD;break;
		case SDLK_KP_MINUS : ret->vk=TCODK_KPSUB;break;
		case SDLK_KP_ENTER : ret->vk=TCODK_KPENTER;break;
		case SDLK_KP_PERIOD : ret->vk=TCODK_KPDEC;break;
		default : ret->vk=TCODK_CHAR; break;
	}

}

static TCOD_key_t TCOD_sys_SDLtoTCOD(SDL_Event *ev, int flags) {
	static TCOD_key_t ret;
	ret.vk=TCODK_NONE;
	ret.c=0;
	ret.pressed=0;
	switch (ev->type) {
		case SDL_QUIT :
			TCOD_console_set_window_closed();
		break;
		case SDL_VIDEOEXPOSE :
			TCOD_sys_console_to_bitmap(screen,TCOD_console_get_width(NULL),TCOD_console_get_height(NULL),consoleBuffer);
		break;
		case SDL_KEYUP : {
			SDL_KeyboardEvent *kev=&ev->key;
			TCOD_key_t tmpkey;
			switch(kev->keysym.sym) {
				case SDLK_LALT : ret.lalt=0; break;
				case SDLK_RALT : ret.ralt=0; break;
				case SDLK_LCTRL : ret.lctrl=0; break;
				case SDLK_RCTRL : ret.rctrl=0; break;
				case SDLK_LSHIFT : ret.shift=0; break;
				case SDLK_RSHIFT : ret.shift=0; break;
				default:break;
			}
			TCOD_sys_convert_event(ev,&tmpkey);
			key_status[tmpkey.vk]=false;
			if ( flags & TCOD_KEY_RELEASED ) {
				ret.vk=tmpkey.vk;
				ret.c=tmpkey.c;
				ret.pressed=0;
			}
		}
		break;
		case SDL_KEYDOWN : {
			SDL_KeyboardEvent *kev=&ev->key;
			TCOD_key_t tmpkey;
			switch(kev->keysym.sym) {
				case SDLK_LALT : ret.lalt=1; break;
				case SDLK_RALT : ret.ralt=1; break;
				case SDLK_LCTRL : ret.lctrl=1; break;
				case SDLK_RCTRL : ret.rctrl=1; break;
				case SDLK_LSHIFT : ret.shift=1; break;
				case SDLK_RSHIFT : ret.shift=1; break;
				default : break;
			}
			TCOD_sys_convert_event(ev,&tmpkey);
			key_status[tmpkey.vk]=true;
			if ( flags & TCOD_KEY_PRESSED ) {
				ret.vk=tmpkey.vk;
				ret.c=tmpkey.c;
				ret.pressed=1;
			}
		}
		break;
	}
	return ret;
}

bool TCOD_sys_is_key_pressed(TCOD_keycode_t key) {
	return key_status[key];
}

TCOD_key_t TCOD_sys_check_for_keypress(int flags) {
	static TCOD_key_t noret={TCODK_NONE,0};
	SDL_Event ev;
	TCOD_key_t retkey={TCODK_NONE,0};
	bool event=false;

	SDL_PumpEvents();
	while ( SDL_PollEvent(&ev) ) {
		TCOD_key_t tmpretkey=TCOD_sys_SDLtoTCOD(&ev, flags);
		event=true;
		if ( tmpretkey.vk != TCODK_NONE ) retkey=tmpretkey;
	}
	if ( event ) return retkey;
	return noret;
}

TCOD_key_t TCOD_sys_wait_for_keypress(bool flush) {
	SDL_Event ev;
	TCOD_key_t ret={TCODK_NONE,0};
	SDL_PumpEvents();
	if ( flush ) {
		while ( SDL_PollEvent(&ev) ) {
			TCOD_sys_SDLtoTCOD(&ev,0);			
		}
	}
	do {
		SDL_WaitEvent(&ev);
		ret = TCOD_sys_SDLtoTCOD(&ev,TCOD_KEY_PRESSED);
	} while ( ret.vk == TCODK_NONE && ev.type != SDL_QUIT );
	return ret;
}


void TCOD_sys_sleep_milli(uint32 milliseconds) {
	SDL_Delay(milliseconds);
}

void TCOD_sys_term() {
	SDL_Quit();
	// printf("blits : %d miss : %d (%g%%)\n",nbBlit,nbMiss,((float)nbMiss*100/nbBlit));
}

void *TCOD_sys_load_image(const char *filename) {
	SDL_Surface* surf=SDL_LoadBMP(filename);
	//if ( surf->format->BytesPerPixel < 3 ) TCOD_fatal("Error on file %s : only 24/32bits images supported",filename);
	return (void *)surf;
}

void TCOD_sys_get_image_size(const void *image, int *w, int *h) {
	SDL_Surface *surf=(SDL_Surface *)image;
    *w = surf->w;
    *h = surf->h;
}

TCOD_color_t TCOD_sys_get_image_pixel(const void *image,int x, int y) {
	TCOD_color_t ret;
	SDL_Surface *surf=(SDL_Surface *)image;
	Uint8 bpp;
	Uint8 *bits;
	if ( x < 0 || y < 0 || x >= surf->w || y >= surf->h ) return TCOD_black;
	bpp = surf->format->BytesPerPixel;
	bits = ((Uint8 *)surf->pixels)+y*surf->pitch+x*bpp;
	switch (bpp) {
		case 4 : 
			*((TCOD_color_t *)&ret) = *(TCOD_color_t *)bits; 
		break;
		case 1 :
		{
			if (surf->format->palette) {
				SDL_Color col = surf->format->palette->colors[(*bits)];
				ret.r=col.r;
				ret.g=col.g;
				ret.b=col.b;
			} else return TCOD_black;
		} 
		break;
		default : 
			ret.r =  *((bits)+charmap->format->Rshift/8);
			ret.g =  *((bits)+charmap->format->Gshift/8);
			ret.b =  *((bits)+charmap->format->Bshift/8);
		break;
	}

	return ret;
}

uint32 TCOD_sys_elapsed_milli() {
	return (uint32)SDL_GetTicks();
}

float TCOD_sys_elapsed_seconds() {
	static float div=1.0f/1000.0f;
	return SDL_GetTicks()*div;
}

void TCOD_sys_force_fullscreen_resolution(int width, int height) {
	fullscreen_width=width;
	fullscreen_height=height;
}

void * TCOD_sys_create_bitmap(int width, int height, TCOD_color_t *buf) {
	int x,y;
	SDL_Surface *bitmap=SDL_CreateRGBSurface(SDL_SWSURFACE,width,height,charmap->format->BitsPerPixel,
		charmap->format->Rmask,charmap->format->Gmask,charmap->format->Bmask,charmap->format->Amask);
	for (x=0; x < width; x++) {
		for (y=0; y < height; y++) {
			SDL_Rect rect;
			Uint32 col=SDL_MapRGB(charmap->format,buf[x+y*width].r,buf[x+y*width].g,buf[x+y*width].b);
			rect.x=x;
			rect.y=y;
			rect.w=1;
			rect.h=1;
			SDL_FillRect(bitmap,&rect,col);
		}
	}
	return (void *)bitmap;
}

void TCOD_sys_set_fps(int val) {
	if( val == 0 ) min_frame_length=0;
	else min_frame_length=1000/val;
}

int TCOD_sys_get_fps() {
	return fps;
}

float TCOD_sys_get_last_frame_length() {
	return last_frame_length;
}

void TCOD_sys_get_char_size(int *w, int *h) {
  *w = fontWidth;
  *h = fontHeight;  
}

void TCOD_sys_get_current_resolution(int *w, int *h) {
	const SDL_VideoInfo *info=SDL_GetVideoInfo();
	*w=info->current_w;
	*h=info->current_h;	
}
