/*
* libtcod 1.4.2
* Copyright (c) 2008,2009 Jice
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Jice ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Jice BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <SDL/SDL.h>
#include "libtcod.h"
#include "libtcod_int.h"

// to enable bitmap locking. Is there any use ?? makes the OSX port renderer to fail
//#define USE_SDL_LOCKS

// image support stuff
bool TCOD_sys_check_bmp(const char *filename);
SDL_Surface *TCOD_sys_read_bmp(const char *filename);
void TCOD_sys_write_bmp(const SDL_Surface *surf, const char *filename);
bool TCOD_sys_check_png(const char *filename);
SDL_Surface *TCOD_sys_read_png(const char *filename);
void TCOD_sys_write_png(const SDL_Surface *surf, const char *filename);

typedef struct {
	char *extension;
	bool (*check_type)(const char *filename);
	SDL_Surface *(*read)(const char *filename);
	void (*write)(const SDL_Surface *surf, const char *filename);
} image_support_t;

static image_support_t image_type[] = {
	{ "BMP", TCOD_sys_check_bmp, TCOD_sys_read_bmp, TCOD_sys_write_bmp },
	{ "PNG", TCOD_sys_check_png, TCOD_sys_read_png, TCOD_sys_write_png },
	{ NULL, NULL, NULL, NULL },
};

static SDL_Surface* screen;
static SDL_Surface* charmap=NULL;
static int consoleWidth=0;
static int consoleHeight=0;
static char_t *consoleBuffer=NULL;
static char_t *prevConsoleBuffer=NULL;
static bool has_startup=false;

// size of a character in the bitmap font
static int fontWidth=8;
static int fontHeight=8;
// number of characters in the bitmap font
int fontNbCharHoriz=16;
int fontNbCharVertic=16;
// font layout (character 0 to 15 on the first row or the first column)
bool fontInRow=false, fontIsGreyscale=false, fontTcodLayout=false;

// font transparent color
static TCOD_color_t fontKeyCol={0,0,0};

static int fullscreen_width=0;
static int fullscreen_height=0;
static int actual_fullscreen_width=0;
static int actual_fullscreen_height=0;
static int fullscreen_offsetx=0;
static int fullscreen_offsety=0;

static char font_file[512]="terminal.png";
static char window_title[512]="";

static bool fullscreen_on;

static Uint32 sdl_key=0, rgb_mask=0, nrgb_mask=0;

// mouse stuff
static bool mousebl=false;
static bool mousebm=false;
static bool mousebr=false;
static bool mouse_force_bl=false;
static bool mouse_force_bm=false;
static bool mouse_force_br=false;

// minimum length for a frame (when fps are limited)
static int min_frame_length=0;
static int min_frame_length_backup=0;
// number of frames in the last second
static int fps=0;
// current number of frames
static int cur_fps=0;
// length of the last rendering loop
static float last_frame_length=0.0f;

static TCOD_color_t *charcols=NULL;
static bool *first_draw=NULL;
static bool key_status[TCODK_CHAR+1];
static int oldFade=-1;

// convert SDL vk to a char (depends on the keyboard layout)
static char vk_to_c[SDLK_LAST];

// convert ASCII code to TCOD layout position
static int init_ascii_to_tcod[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0, 76, 77,  0,  0,  0,  0,  0, // ASCII 0 to 15
 71, 70, 72,  0,  0,  0,  0,  0, 64, 65, 67, 66,  0, 73, 68, 69, // ASCII 16 to 31
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, // ASCII 32 to 47
 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, // ASCII 48 to 63
 32, 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110, // ASCII 64 to 79
111,112,113,114,115,116,117,118,119,120,121, 33, 34, 35, 36, 37, // ASCII 80 to 95
 38,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142, // ASCII 96 to 111
143,144,145,146,147,148,149,150,151,152,153, 39, 40, 41, 42,  0, // ASCII 112 to 127
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // ASCII 128 to 143
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // ASCII 144 to 159
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // ASCII 160 to 175
 43, 44, 45, 46, 49,  0,  0,  0,  0, 81, 78, 87, 88,  0,  0, 55, // ASCII 176 to 191
 53, 50, 52, 51, 47, 48,  0,  0, 85, 86, 82, 84, 83, 79, 80,  0, // ASCII 192 to 207
  0,  0,  0,  0,  0,  0,  0,  0,  0, 56, 54,  0,  0,  0,  0,  0, // ASCII 208 to 223
 74, 75, 57, 58, 59, 60, 61, 62, 63,  0,  0,  0,  0,  0,  0,  0, // ASCII 224 to 239
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // ASCII 240 to 255
};

int *ascii_to_tcod=NULL;
static int max_font_chars=0;

static bool *ascii_updated=NULL;
static bool any_ascii_updated=false;

static void alloc_ascii_tables() {
	if ( ascii_to_tcod ) free(ascii_to_tcod);
	if ( ascii_updated ) free(ascii_updated);
	if ( charcols ) {
		free(charcols);
		free(first_draw);
	}

	ascii_to_tcod = (int *)calloc(sizeof(int),max_font_chars);
	ascii_updated = (bool *)calloc(sizeof(bool),max_font_chars);
	charcols = (TCOD_color_t *)calloc(sizeof(TCOD_color_t),max_font_chars);
	first_draw =(bool *)calloc(sizeof(bool),max_font_chars);
	memcpy(ascii_to_tcod,init_ascii_to_tcod,sizeof(int)*256);
}

static void check_ascii_to_tcod() {
	if ( fontNbCharHoriz * fontNbCharVertic != max_font_chars ) {
		max_font_chars=fontNbCharHoriz * fontNbCharVertic;
		alloc_ascii_tables();
	}
}

void TCOD_sys_map_ascii_to_font(asciiCode, fontCharX, fontCharY) {
	if ( asciiCode > 0 && asciiCode < max_font_chars )
		ascii_to_tcod[asciiCode] = fontCharX + fontCharY * fontNbCharHoriz;
}

void TCOD_sys_load_font() {
	int i;
	bool hasTransparent=false;
	int x,y;

	charmap=TCOD_sys_load_image(font_file);
	if (charmap == NULL ) TCOD_fatal("SDL : cannot load %s",font_file);
	if ( (float)(charmap->w / fontNbCharHoriz) != charmap->w / fontNbCharHoriz
		|| (float)(charmap->h / fontNbCharVertic) != charmap->h / fontNbCharVertic ) TCOD_fatal(" %s size is not a multiple of font layout (%dx%d)\n",
		font_file,fontNbCharHoriz,fontNbCharVertic);
	fontWidth=charmap->w/fontNbCharHoriz;
	fontHeight=charmap->h/fontNbCharVertic;
	check_ascii_to_tcod();
	// figure out what kind of font we have
	// check if the alpha layer is actually used
	if ( charmap->format->BytesPerPixel == 4 )
	for (x=0; !hasTransparent && x < charmap->w; x ++ ) {
		for (y=0;!hasTransparent && y < charmap->h; y++ ) {
			Uint8 *pixel=(Uint8 *)(charmap->pixels) + y * charmap->pitch + x * charmap->format->BytesPerPixel;
			Uint8 alpha=*((pixel)+charmap->format->Ashift/8);
			if ( alpha < 255 ) {
				hasTransparent=true;
			}
		}
	}
	if (! hasTransparent ) {
		// 24 bit font. check if greyscale
		int x,y,keyx,keyy;
        Uint8 *pixel;
		// the key color is found on the character corresponding to space ' '
		if ( fontTcodLayout ) {
			keyx = fontWidth/2;
			keyy = fontHeight/2;
		} else if (fontInRow) {
			keyx = ((int)(' ') % fontNbCharVertic ) * fontWidth + fontWidth/2;
			keyy = ((int)(' ') / fontNbCharVertic ) * fontHeight + fontHeight/2;
		} else {
			keyx = ((int)(' ') / fontNbCharVertic ) * fontWidth + fontWidth/2;
			keyy = ((int)(' ') % fontNbCharVertic ) * fontHeight + fontHeight/2;
		}
		pixel=(Uint8 *)(charmap->pixels) + keyy * charmap->pitch + keyx * charmap->format->BytesPerPixel;
		fontKeyCol.r=*((pixel)+charmap->format->Rshift/8);
		fontKeyCol.g=*((pixel)+charmap->format->Gshift/8);
		fontKeyCol.b=*((pixel)+charmap->format->Bshift/8);
		// convert greyscale to font with alpha layer
		if ( fontIsGreyscale ) {
			bool invert=( fontKeyCol.r > 128 ); // black on white font ?
			// convert it to 32 bits if needed
			if ( charmap->format->BytesPerPixel != 4 ) {
				SDL_Surface *temp=(SDL_Surface *)TCOD_sys_get_surface(charmap->w,charmap->h,true);
				SDL_BlitSurface(charmap,NULL,temp,NULL);
				SDL_FreeSurface(charmap);
				charmap=temp;
			}
			for (x=0; x < charmap->w; x ++ ) {
				for (y=0;y < charmap->h; y++ ) {
					Uint8 *pixel=(Uint8 *)(charmap->pixels) + y * charmap->pitch + x * charmap->format->BytesPerPixel;
					Uint8 r=*((pixel)+charmap->format->Rshift/8);
					*((pixel)+charmap->format->Ashift/8) = (invert ? 255-r : r);
					*((pixel)+charmap->format->Rshift/8)=255;
					*((pixel)+charmap->format->Gshift/8)=255;
					*((pixel)+charmap->format->Bshift/8)=255;
				}
			}
		} else {
			// alpha layer not used. convert to 24 bits (faster)
			SDL_Surface *temp=(SDL_Surface *)TCOD_sys_get_surface(charmap->w,charmap->h,false);
			SDL_BlitSurface(charmap,NULL,temp,NULL);
			SDL_FreeSurface(charmap);
			charmap=temp;
		}
	}
	/*
	charmap=SDL_CreateRGBSurface(SDL_SWSURFACE,charmap->w,charmap->h,24,0xFF0000, 0xFF00, 0xFF, 0);
	if ( SDL_MUSTLOCK( charmap ) ) SDL_LockSurface( charmap );
	SDL_BlitSurface(charmap,NULL,charmap,NULL);
	*/
	sdl_key=SDL_MapRGB(charmap->format,fontKeyCol.r,fontKeyCol.g,fontKeyCol.b);
	rgb_mask=charmap->format->Rmask|charmap->format->Gmask|charmap->format->Bmask;
	nrgb_mask = ~ rgb_mask;
	sdl_key &= rgb_mask; // remove the alpha part
	if ( charmap->format->BytesPerPixel == 3 ) SDL_SetColorKey(charmap,SDL_SRCCOLORKEY|SDL_RLEACCEL,sdl_key);
	for (i=0; i < fontNbCharHoriz*fontNbCharVertic; i++ ) {
		charcols[i]=fontKeyCol;
		first_draw[i]=true;
	}
	check_ascii_to_tcod();
	if (!fontTcodLayout) {
		// apply standard ascii mapping
		for (i=0; i < max_font_chars; i++ ) ascii_to_tcod[i]=i;
	}
}

void TCOD_sys_set_custom_font(const char *fontFile,int nb_ch, int nb_cv, int flags) {
	strcpy(font_file,fontFile);
	if (flags==0) flags=TCOD_FONT_LAYOUT_ASCII_INCOL;
	fontInRow=((flags & TCOD_FONT_LAYOUT_ASCII_INROW) != 0);
	fontIsGreyscale = ((flags & TCOD_FONT_TYPE_GREYSCALE) != 0 );
	fontTcodLayout = ((flags & TCOD_FONT_LAYOUT_TCOD) != 0 );
	if ( nb_ch> 0 ) {
		fontNbCharHoriz=nb_ch;
		fontNbCharVertic=nb_cv;
	} else {
		if ( ( flags & TCOD_FONT_LAYOUT_ASCII_INROW ) || ( flags & TCOD_FONT_LAYOUT_ASCII_INCOL )  ) {
			fontNbCharHoriz=16;
			fontNbCharVertic=16;
		} else {
			fontNbCharHoriz=32;
			fontNbCharVertic=8;
		}
	}
	if ( fontTcodLayout ) fontInRow=true;
	check_ascii_to_tcod();
	TCOD_sys_load_font();
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
	w = TCOD_console_get_width(console) * fontWidth;
	h = TCOD_console_get_height(console) * fontHeight;
	return TCOD_sys_get_surface(w,h,false);
}

void TCOD_sys_console_to_bitmap(void *vbitmap, int console_width, int console_height, char_t *console_buffer, char_t *prev_console_buffer) {
	int x,y;
	SDL_Surface *bitmap=(SDL_Surface *)vbitmap;
	Uint32 sdl_back=0,sdl_fore=0;
	TCOD_color_t fading_color = TCOD_console_get_fading_color();
	int fade = (int)TCOD_console_get_fade();
	bool track_changes=(oldFade == fade && prev_console_buffer);
#ifdef USE_SDL_LOCKS
	if ( SDL_MUSTLOCK( bitmap ) && SDL_LockSurface( bitmap ) < 0 ) return;
#endif
	for (y=0;y<console_height;y++) {
		for (x=0; x<console_width; x++) {
			SDL_Rect srcRect,dstRect;
			bool changed=true;
			char_t *c=&console_buffer[x+y*console_width];
			if ( c->cf == -1 ) c->cf = ascii_to_tcod[c->c];
			if ( track_changes ) {
				char_t *oc=&prev_console_buffer[x+y*console_width];
				changed=false;
				if ( ascii_updated[ c->c ] || c->back.r != oc->back.r || c->back.g != oc->back.g
					|| c->back.b != oc->back.b || c->fore.r != oc->fore.r
					|| c->fore.g != oc->fore.g || c->fore.b != oc->fore.b
					|| c->c != oc->c || c->cf != oc->cf) {
					changed=true;
				}
			}
			if ( changed ) {
				TCOD_color_t b=c->back;
				dstRect.x=x*fontWidth;
				dstRect.y=y*fontHeight;
				dstRect.w=fontWidth;
				dstRect.h=fontHeight;
				// draw background
				b.r = ((int)b.r) * fade / 255 + ((int)fading_color.r) * (255-fade)/255;
				b.g = ((int)b.g) * fade / 255  + ((int)fading_color.g) * (255-fade)/255;
				b.b = ((int)b.b) * fade / 255 + ((int)fading_color.b) * (255-fade)/255;
				sdl_back=SDL_MapRGB(bitmap->format,b.r,b.g,b.b);
				if ( bitmap == screen && fullscreen_on ) {
					dstRect.x+=fullscreen_offsetx;
					dstRect.y+=fullscreen_offsety;
				}
				SDL_FillRect(bitmap,&dstRect,sdl_back);
				if ( c->c != ' ' ) {
					// draw foreground
					//int ascii=fontTcodLayout ? (int)(ascii_to_tcod[c->c]): c->c;
					int ascii=c->cf;
					TCOD_color_t *curtext = &charcols[ascii];
					bool first = first_draw[ascii];
					TCOD_color_t f=c->fore;

					f.r = ((int)f.r) * fade / 255 + ((int)fading_color.r) * (255-fade)/255;
					f.g = ((int)f.g) * fade / 255 + ((int)fading_color.g) * (255-fade)/255;
					f.b = ((int)f.b) * fade / 255 + ((int)fading_color.b) * (255-fade)/255;
					// only draw character if foreground color != background color
					if ( ascii_updated[c->c] || f.r != b.r || f.g != b.g || f.b != b.b ) {
						if ( charmap && charmap->format->Amask == 0
							&& f.r == fontKeyCol.r && f.g == fontKeyCol.g && f.b == fontKeyCol.b ) {
							// cannot draw with the key color...
							if ( f.r < 255 ) f.r++; else f.r--;
						}
						if (fontInRow) {
							srcRect.x = (ascii%fontNbCharHoriz)*fontWidth;
							srcRect.y = (ascii/fontNbCharHoriz)*fontHeight;
						} else {
							srcRect.x = (ascii/fontNbCharVertic)*fontWidth;
							srcRect.y = (ascii%fontNbCharVertic)*fontHeight;
						}
						srcRect.w=fontWidth;
						srcRect.h=fontHeight;

						if ( charmap && (first || curtext->r != f.r || curtext->g != f.g || curtext->b!=f.b) ) {
							// change the character color in the font
					    	Uint8 bpp = charmap->format->BytesPerPixel;
					    	first_draw[ascii]=false;
							sdl_fore=SDL_MapRGB(charmap->format,f.r,f.g,f.b) & rgb_mask;
							*curtext=f;
#ifdef USE_SDL_LOCKS
							if ( SDL_MUSTLOCK(charmap) ) {
								if ( SDL_LockSurface(charmap) < 0 ) return;
							}
#endif
							if ( bpp == 4 ) {
								// 32 bits font : fill the whole character with color
								Uint32 *pix = (Uint32 *)(((Uint8 *)charmap->pixels)+srcRect.x*bpp + srcRect.y*charmap->pitch);
								int hdelta=(charmap->pitch - fontWidth*bpp)/4;
								int h=fontHeight;
								while (h> 0) {
									int w=fontWidth;
									while ( w > 0 ) {
										(*pix) &= nrgb_mask;
										(*pix) |= sdl_fore;
										w--;
										pix++;
									}
									h--;
									pix += hdelta;
								}
							} else	{
								// 24 bits font : fill only non key color pixels
								Uint32 *pix = (Uint32 *)(((Uint8 *)charmap->pixels)+srcRect.x*bpp + srcRect.y*charmap->pitch);
								int h=fontHeight;
								int hdelta=(charmap->pitch - fontWidth*bpp);
								while (h> 0) {
									int w=fontWidth;
									while ( w > 0 ) {
										if (((*pix) & rgb_mask) != sdl_key ) {
											(*pix) &= nrgb_mask;
											(*pix) |= sdl_fore;
										}
										w--;
										pix = (Uint32 *) (((Uint8 *)pix)+3);
									}
									h--;
									pix = (Uint32 *) (((Uint8 *)pix)+hdelta);
								}
							}
#ifdef USE_SDL_LOCKS
							if ( SDL_MUSTLOCK(charmap) ) {
								SDL_UnlockSurface(charmap);
							}
#endif							
						}
						SDL_BlitSurface(charmap,&srcRect,bitmap,&dstRect);
					}
				}
			}
		}
	}
#ifdef USE_SDL_LOCKS
	if ( SDL_MUSTLOCK( bitmap ) ) SDL_UnlockSurface( bitmap );
#endif	
	oldFade=fade;
	if ( any_ascii_updated ) {
		memset(ascii_updated,0,sizeof(bool)*max_font_chars);
		any_ascii_updated=false;
	}
}

void TCOD_sys_set_keyboard_repeat(int initial_delay, int interval) {
	SDL_EnableKeyRepeat(initial_delay,interval);
}

void *TCOD_sys_get_surface(int width, int height, bool alpha) {
	Uint32 rmask,gmask,bmask,amask;
	SDL_Surface *bitmap;
	int flags=SDL_SWSURFACE;

	if ( alpha ) {
		if ( SDL_BYTEORDER == SDL_LIL_ENDIAN ) {
			rmask=0x000000FF;
			gmask=0x0000FF00;
			bmask=0x00FF0000;
			amask=0xFF000000;
		} else {
			rmask=0xFF000000;
			gmask=0x00FF0000;
			bmask=0x0000FF00;
			amask=0x000000FF;
		}
		flags|=SDL_SRCALPHA;
	} else {
		if ( SDL_BYTEORDER == SDL_LIL_ENDIAN ) {
			rmask=0x0000FF;
			gmask=0x00FF00;
			bmask=0xFF0000;
		} else {
			rmask=0xFF0000;
			gmask=0x00FF00;
			bmask=0x0000FF;
		}
		amask=0;
	}
	bitmap=SDL_AllocSurface(flags,width,height,
		alpha ? 32:24,
		rmask,gmask,bmask,amask);
	if ( alpha ) {
		SDL_SetAlpha(bitmap, SDL_SRCALPHA, 255);
	}
	return (void *)bitmap;
}

void TCOD_sys_update_char(int asciiCode, int fontx, int fonty, TCOD_image_t img, int x, int y) {
	int px,py;
	int iw,ih;
	static TCOD_color_t pink={255,0,255};
	TCOD_sys_map_ascii_to_font(asciiCode,fontx,fonty);
	TCOD_image_get_size(img,&iw,&ih);
	for (px=0; px < fontWidth; px ++ ) {
		for (py=0;py < fontHeight; py++ ) {
			TCOD_color_t col=TCOD_white;
			Uint8 *pixel;
			Uint8 bpp;
			if ( (unsigned)(x+px) < (unsigned)iw && (unsigned)(y+py) < (unsigned)ih ) {
				col=TCOD_image_get_pixel(img,x+px,y+py);
			}
			pixel=(Uint8 *)(charmap->pixels) + (fonty*fontHeight+py) * charmap->pitch + (fontx*fontWidth+px) * charmap->format->BytesPerPixel;
	    	bpp = charmap->format->BytesPerPixel;
			if (bpp == 4 ) {
				*((pixel)+charmap->format->Ashift/8) = col.r;
				*((pixel)+charmap->format->Rshift/8)=255;
				*((pixel)+charmap->format->Gshift/8)=255;
				*((pixel)+charmap->format->Bshift/8)=255;
			} else {
				*((pixel)+charmap->format->Rshift/8)=col.r;
				*((pixel)+charmap->format->Gshift/8)=col.g;
				*((pixel)+charmap->format->Bshift/8)=col.b;
			}
		}
	}
	// TODO : improve this. Won't work if we want a pink portrait...
	charcols[asciiCode]=pink;
	ascii_updated[asciiCode]=true;
	any_ascii_updated=true;
}

void TCOD_sys_startup() {
	if (has_startup) return;
	if (SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO) < 0 ) TCOD_fatal_nopar("SDL : cannot initialize");
	atexit(SDL_Quit);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
	max_font_chars=256;
	alloc_ascii_tables();
	has_startup=true;
}

bool TCOD_sys_init(int w,int h, char_t *buf, char_t *oldbuf, bool fullscreen) {
	if ( ! has_startup ) TCOD_sys_startup();
	consoleWidth=w;
	consoleHeight=h;
	if (! charmap) TCOD_sys_load_font();
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
	//if ( SDL_MUSTLOCK( charmap ) ) SDL_UnlockSurface( charmap );
	SDL_EnableUNICODE(1);
	consoleBuffer=buf;
	prevConsoleBuffer=oldbuf;
	fullscreen_on=fullscreen;
	memset(key_status,0,sizeof(bool)*(TCODK_CHAR+1));
	memset(ascii_updated,0,sizeof(bool)*max_font_chars);
	return true;
}

void TCOD_sys_save_bitmap(void *bitmap, const char *filename) {
	image_support_t *img=image_type;
	while ( img->extension != NULL && strcasestr(filename,img->extension) == NULL ) img++;
	if ( img->extension == NULL || img->write == NULL ) img=image_type; // default to bmp
	img->write((SDL_Surface *)bitmap,filename);
}

void TCOD_sys_save_screenshot(const char *filename) {
	char buf[128];
	if ( filename == NULL ) {
		// generate filename
		int idx=0;
		do {
		    FILE *f=NULL;
			sprintf(buf,"./screenshot%03d.png",idx);
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
	bool mouseOn=SDL_ShowCursor(-1);
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
		SDL_ShowCursor(mouseOn ? 1:0);
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
		SDL_ShowCursor(mouseOn ? 1:0);
		fullscreen_offsetx=0;
		fullscreen_offsety=0;
	}
	/* SDL_WM_SetCaption(window_title,NULL); */
	oldFade=-1; // to redraw the whole screen
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
		TCOD_sys_console_to_bitmap(screen,TCOD_console_get_width(NULL),TCOD_console_get_height(NULL),consoleBuffer, prevConsoleBuffer);
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
		} else if ( kev->keysym.sym >= SDLK_0 && kev->keysym.sym <= SDLK_9 ) {
			ret->c = '0'+(kev->keysym.sym - SDLK_0);
		} else if ( kev->keysym.sym >= SDLK_KP0 && kev->keysym.sym <= SDLK_KP9 ) {
			ret->c = '0'+(kev->keysym.sym - SDLK_KP0);
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
		case SDLK_DELETE : ret->vk=TCODK_DELETE;break;
		case SDLK_INSERT : ret->vk=TCODK_INSERT; break;
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
			TCOD_sys_console_to_bitmap(screen,TCOD_console_get_width(NULL),TCOD_console_get_height(NULL),consoleBuffer,prevConsoleBuffer);
		break;
		case SDL_MOUSEBUTTONDOWN : {
			SDL_MouseButtonEvent *mev=&ev->button;
			switch (mev->button) {
				case 1 : mousebl=true; break;
				case 2 : mousebm=true; break;
				case 3 : mousebr=true; break;
			}
		}
		break;
		case SDL_MOUSEBUTTONUP : {
			SDL_MouseButtonEvent *mev=&ev->button;
			switch (mev->button) {
				case 1 : if (mousebl) mouse_force_bl=true; mousebl=false; break;
				case 2 : if (mousebm) mouse_force_bm=true; mousebm=false; break;
				case 3 : if (mousebr) mouse_force_br=true; mousebr=false; break;
			}
		}
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

	SDL_PumpEvents();
	while ( SDL_PollEvent(&ev) ) {
		TCOD_key_t tmpretkey=TCOD_sys_SDLtoTCOD(&ev, flags);
		if ( tmpretkey.vk != TCODK_NONE ) {
			return tmpretkey;
		}
	}
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
	image_support_t *img=image_type;
	while ( img->extension != NULL && !img->check_type(filename) ) img++;
	if ( img->extension == NULL || img->read == NULL ) return NULL; // unknown format
	return img->read(filename);
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
			ret.r =  *((bits)+surf->format->Rshift/8);
			ret.g =  *((bits)+surf->format->Gshift/8);
			ret.b =  *((bits)+surf->format->Bshift/8);
//			if (ret.r != 255) printf ("%X => %2x%2x%2x %d %d %d\n",*(Uint32 *)bits,ret.r,ret.g,ret.b,
//				surf->format->Rshift/8,surf->format->Gshift/8,surf->format->Bshift/8);
		break;
	}

	return ret;
}

int TCOD_sys_get_image_alpha(const void *image,int x, int y) {
	SDL_Surface *surf=(SDL_Surface *)image;
	Uint8 bpp;
	Uint8 *bits;
	if ( x < 0 || y < 0 || x >= surf->w || y >= surf->h ) return 255;
	bpp = surf->format->BytesPerPixel;
	if ( bpp != 4 ) return 255;
	bits = ((Uint8 *)surf->pixels)+y*surf->pitch+x*bpp;
	return *((bits)+surf->format->Ashift/8);
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

/*
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
*/
void * TCOD_sys_create_bitmap(int width, int height, TCOD_color_t *buf) {
	int x,y;
	SDL_PixelFormat fmt;
	memset(&fmt,0,sizeof(SDL_PixelFormat));
	if ( charmap != NULL ) {
		fmt = *charmap->format;
	} else {
		fmt.BitsPerPixel=24;
		fmt.Amask=0;
		if ( SDL_BYTEORDER == SDL_LIL_ENDIAN ) {
			fmt.Rmask=0x0000FF;
			fmt.Gmask=0x00FF00;
			fmt.Bmask=0xFF0000;
		} else {
			fmt.Rmask=0xFF0000;
			fmt.Gmask=0x00FF00;
			fmt.Bmask=0x0000FF;
		}
	}
	SDL_Surface *bitmap=SDL_CreateRGBSurface(SDL_SWSURFACE,width,height,fmt.BitsPerPixel,fmt.Rmask,fmt.Gmask,fmt.Bmask,fmt.Amask);
	for (x=0; x < width; x++) {
		for (y=0; y < height; y++) {
			SDL_Rect rect;
			Uint32 col=SDL_MapRGB(&fmt,buf[x+y*width].r,buf[x+y*width].g,buf[x+y*width].b);
			rect.x=x;
			rect.y=y;
			rect.w=1;
			rect.h=1;
			SDL_FillRect(bitmap,&rect,col);
		}
	}
	return (void *)bitmap;
}

void TCOD_sys_delete_bitmap(void *bitmap) {
	SDL_FreeSurface((SDL_Surface *)bitmap);
}

void TCOD_sys_set_fps(int val) {
	if( val == 0 ) min_frame_length=0;
	else min_frame_length=1000/val;
}

void TCOD_sys_save_fps() {
	min_frame_length_backup=min_frame_length;
}

void TCOD_sys_restore_fps() {
	min_frame_length=min_frame_length_backup;
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

/* image stuff */
bool TCOD_sys_check_magic_number(const char *filename, int size, uint8 *data) {
	uint8 tmp[128];
	int i;
	FILE *f=fopen(filename,"rb");
	if (! f) return false;
	if ( fread(tmp,1,128,f) < (unsigned)size ) {
		fclose(f);
		return false;
	}
	for (i=0; i< size; i++) if (tmp[i]!=data[i]) return false;
	fclose(f);
	return true;
}

/* mouse stuff */

TCOD_mouse_t TCOD_mouse_get_status() {
	TCOD_mouse_t ms;
	int charWidth, charHeight;
	static bool lbut=false;
	static bool rbut=false;
	static bool mbut=false;
	Uint8 buttons;
	SDL_PumpEvents();

	buttons = SDL_GetMouseState(&ms.x,&ms.y);
	SDL_GetRelativeMouseState(&ms.dx,&ms.dy);
	ms.lbutton = (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) ? 1 : 0;
	ms.lbutton_pressed=false;
	if (ms.lbutton) lbut=true;
	else if( lbut ) {
		lbut=false;
		ms.lbutton_pressed=true;
	}
	if ( mouse_force_bl ) {
		ms.lbutton_pressed=true;
		mouse_force_bl=false;
	}

	ms.rbutton = (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) ? 1 : 0;
	ms.rbutton_pressed=false;
	if (ms.rbutton) rbut=true;
	else if( rbut ) {
		rbut=false;
		ms.rbutton_pressed=true;
	}
	if ( mouse_force_br ) {
		ms.rbutton_pressed=true;
		mouse_force_br=false;
	}

	ms.mbutton = (buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) ? 1 : 0;
	ms.mbutton_pressed=false;
	if (ms.mbutton) mbut=true;
	else if( mbut ) {
		mbut=false;
		ms.mbutton_pressed=true;
	}
	if ( mouse_force_bm ) {
		ms.mbutton_pressed=true;
		mouse_force_bm=false;
	}

	ms.wheel_up = (buttons & SDL_BUTTON(SDL_BUTTON_WHEELUP)) ? 1 : 0;
	ms.wheel_down = (buttons & SDL_BUTTON(SDL_BUTTON_WHEELDOWN)) ? 1 : 0;
	TCOD_sys_get_char_size(&charWidth,&charHeight);
	ms.cx = (ms.x - fullscreen_offsetx) / charWidth;
	ms.cy = (ms.y - fullscreen_offsety) / charHeight;
	ms.dcx = ms.dx / charWidth;
	ms.dcy = ms.dy / charHeight;
	return ms;
}

void TCOD_mouse_show_cursor(bool visible) {
  SDL_ShowCursor(visible ? 1 : 0);
}

bool TCOD_mouse_is_cursor_visible() {
  return SDL_ShowCursor(-1) ? true : false;
}

void TCOD_mouse_move(int x, int y) {
  SDL_WarpMouse((Uint16)x,(Uint16)y);
}

