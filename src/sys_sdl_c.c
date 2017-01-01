/*
* libtcod 1.6.2
* Copyright (c) 2008,2009,2010,2012,2013,2016 Jice & Mingos & rmtew
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "libtcod.h"
#include "libtcod_int.h"

static TCOD_SDL_driver_t *sdl=NULL;

#if defined(__ANDROID__)
#define TCOD_TOUCH_INPUT
#define MAX_TOUCH_FINGERS 5

typedef struct {
  int nupdates; /* how many updates have happened since the first finger was pressed. */
  Uint32 ticks0; /* current number of ticks at start of touch event sequence. */
  SDL_FingerID finger_id; /* the last finger which was pressed. */
  int coords[MAX_TOUCH_FINGERS][2]; /* absolute position of each finger. */
  int coords_delta[MAX_TOUCH_FINGERS][2]; /* absolute position of each finger. */
  int consolecoords[MAX_TOUCH_FINGERS][2]; /* cell coordinates in the root console for each finger. */
  int consolecoords_delta[MAX_TOUCH_FINGERS][2]; /* cell coordinates in the root console for each finger. */
  int nfingers; /* number of unique fingers employed at any time during this. */
  int nfingerspressed; /* number of those fingers currently still pressed. */
  SDL_FingerID finger_ids[MAX_TOUCH_FINGERS];
  char fingerspressed[MAX_TOUCH_FINGERS];
} TCOD_touch_t;
#endif

/* to enable bitmap locking. Is there any use ?? makes the OSX port renderer to fail */
/*#define USE_SDL_LOCKS */

/* image support stuff */
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

scale_data_t scale_data={0};
SDL_Window* window=NULL;
SDL_Renderer* renderer=NULL;
float scale_factor=1.0f;
SDL_Surface* charmap=NULL;
static TCOD_render_state_t *renderState = NULL;
static bool has_startup=false;
#define MAX_SCALE_FACTOR 5.0f

/* font transparent color */
static TCOD_color_t fontKeyCol={0,0,0};

static Uint32 sdl_key=0, rgb_mask=0, nrgb_mask=0;

/* mouse stuff */
static bool mousebl=false;
static bool mousebm=false;
static bool mousebr=false;
static bool mouse_force_bl=false;
static bool mouse_force_bm=false;
static bool mouse_force_br=false;
#ifdef TCOD_TOUCH_INPUT
static bool mouse_touch=true;
#endif

/* minimum length for a frame (when fps are limited) */
static int min_frame_length=0;
static int min_frame_length_backup=0;
/* number of frames in the last second */
static int fps=0;
/* current number of frames */
static int cur_fps=0;
/* length of the last rendering loop */
static float last_frame_length=0.0f;

static TCOD_color_t *charcols=NULL;
static bool *first_draw=NULL;
static bool key_status[TCODK_CHAR+1];
int oldFade=-1;

/* convert SDL vk to a char (depends on the keyboard layout) */
typedef struct {
	SDL_Keycode	sdl_key;
	int tcod_key;
} vk_to_c_entry;
#define NUM_VK_TO_C_ENTRIES 10
static vk_to_c_entry vk_to_c[NUM_VK_TO_C_ENTRIES];

/* convert ASCII code to TCOD layout position */
static int init_ascii_to_tcod[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0, 76, 77,  0,  0,  0,  0,  0, /* ASCII 0 to 15 */
 71, 70, 72,  0,  0,  0,  0,  0, 64, 65, 67, 66,  0, 73, 68, 69, /* ASCII 16 to 31 */
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, /* ASCII 32 to 47 */
 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, /* ASCII 48 to 63 */
 32, 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110, /* ASCII 64 to 79 */
111,112,113,114,115,116,117,118,119,120,121, 33, 34, 35, 36, 37, /* ASCII 80 to 95 */
 38,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142, /* ASCII 96 to 111 */
143,144,145,146,147,148,149,150,151,152,153, 39, 40, 41, 42,  0, /* ASCII 112 to 127 */
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* ASCII 128 to 143 */
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* ASCII 144 to 159 */
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* ASCII 160 to 175 */
 43, 44, 45, 46, 49,  0,  0,  0,  0, 81, 78, 87, 88,  0,  0, 55, /* ASCII 176 to 191 */
 53, 50, 52, 51, 47, 48,  0,  0, 85, 86, 82, 84, 83, 79, 80,  0, /* ASCII 192 to 207 */
  0,  0,  0,  0,  0,  0,  0,  0,  0, 56, 54,  0,  0,  0,  0,  0, /* ASCII 208 to 223 */
 74, 75, 57, 58, 59, 60, 61, 62, 63,  0,  0,  0,  0,  0,  0,  0, /* ASCII 224 to 239 */
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* ASCII 240 to 255 */
};

bool *ascii_updated=NULL;
bool any_ascii_updated=false;

static void alloc_ascii_tables(void) {
	if ( TCOD_ctx.ascii_to_tcod ) free(TCOD_ctx.ascii_to_tcod);
	if ( ascii_updated ) free(ascii_updated);
	if ( charcols ) {
		free(charcols);
		free(first_draw);
	}

	TCOD_ctx.ascii_to_tcod = (int *)calloc(sizeof(int),TCOD_ctx.max_font_chars);
	ascii_updated = (bool *)calloc(sizeof(bool),TCOD_ctx.max_font_chars);
	charcols = (TCOD_color_t *)calloc(sizeof(TCOD_color_t),TCOD_ctx.max_font_chars);
	first_draw =(bool *)calloc(sizeof(bool),TCOD_ctx.max_font_chars);
	memcpy(TCOD_ctx.ascii_to_tcod,init_ascii_to_tcod,sizeof(int)*256);
}

static void check_ascii_to_tcod(void) {
	if ( TCOD_ctx.fontNbCharHoriz * TCOD_ctx.fontNbCharVertic != TCOD_ctx.max_font_chars ) {
		TCOD_ctx.max_font_chars=TCOD_ctx.fontNbCharHoriz * TCOD_ctx.fontNbCharVertic;
		alloc_ascii_tables();
	}
}

void TCOD_sys_register_SDL_renderer(SDL_renderer_t renderer) {
	TCOD_ctx.sdl_cbk=renderer;
}

void TCOD_sys_map_ascii_to_font(int asciiCode, int fontCharX, int fontCharY) {
	if ( asciiCode > 0 && asciiCode < TCOD_ctx.max_font_chars )
		TCOD_ctx.ascii_to_tcod[asciiCode] = fontCharX + fontCharY * TCOD_ctx.fontNbCharHoriz;
}

void TCOD_sys_load_font(void) {
	int i;
	bool hasTransparent=false;
	int x,y;

	if ( charmap ) SDL_FreeSurface(charmap);
	charmap=TCOD_sys_load_image(TCOD_ctx.font_file);
	if (charmap == NULL ) TCOD_fatal("SDL : cannot load %s",TCOD_ctx.font_file);
	if ( (float)(charmap->w / TCOD_ctx.fontNbCharHoriz) != charmap->w / TCOD_ctx.fontNbCharHoriz
		|| (float)(charmap->h / TCOD_ctx.fontNbCharVertic) != charmap->h / TCOD_ctx.fontNbCharVertic ) TCOD_fatal(" %s size is not a multiple of font layout (%dx%d)\n",
		TCOD_ctx.font_file,TCOD_ctx.fontNbCharHoriz,TCOD_ctx.fontNbCharVertic);
	TCOD_ctx.font_width=charmap->w/TCOD_ctx.fontNbCharHoriz;
	TCOD_ctx.font_height=charmap->h/TCOD_ctx.fontNbCharVertic;
	/* allocated bool array for colored flags */
	if ( TCOD_ctx.colored ) free(TCOD_ctx.colored);
	TCOD_ctx.colored=(bool *)calloc(sizeof(bool), TCOD_ctx.fontNbCharHoriz*TCOD_ctx.fontNbCharVertic);
	check_ascii_to_tcod();
	/* figure out what kind of font we have */
	/* check if the alpha layer is actually used */
	if ( charmap->format->BytesPerPixel == 4 ) {
		printf ("32bits font... checking for alpha layer... ");
		for (x=0; !hasTransparent && x < charmap->w; x ++ ) {
			for (y=0;!hasTransparent && y < charmap->h; y++ ) {
				Uint8 *pixel=(Uint8 *)(charmap->pixels) + y * charmap->pitch + x * charmap->format->BytesPerPixel;
				Uint8 alpha=*((pixel)+charmap->format->Ashift/8);
				if ( alpha < 255 ) {
					hasTransparent=true;
				}
			}
		}
		printf (hasTransparent ? "present\n" : "not present\n");
	} else if ( charmap->format->BytesPerPixel != 3 ) {
		/* convert to 24 bits */
		SDL_Surface *temp;
		printf ("font bpp < 24. converting to 24bits\n");
		temp=(SDL_Surface *)TCOD_sys_get_surface(charmap->w,charmap->h,false);
		SDL_BlitSurface(charmap,NULL,temp,NULL);
		SDL_FreeSurface(charmap);
		charmap=temp;
	} else {
		printf ("24 bits font.\n");
	}
	if (! hasTransparent ) {
		/* alpha layer not used */
		int keyx,keyy;
        Uint8 *pixel;
		/* the key color is found on the character corresponding to space ' ' */
		if ( TCOD_ctx.font_tcod_layout ) {
			keyx = TCOD_ctx.font_width/2;
			keyy = TCOD_ctx.font_height/2;
		} else if (TCOD_ctx.font_in_row) {
			keyx = ((int)(' ') % TCOD_ctx.fontNbCharHoriz ) * TCOD_ctx.font_width + TCOD_ctx.font_width/2;
			keyy = ((int)(' ') / TCOD_ctx.fontNbCharHoriz ) * TCOD_ctx.font_height + TCOD_ctx.font_height/2;
		} else {
			keyx = ((int)(' ') / TCOD_ctx.fontNbCharVertic ) * TCOD_ctx.font_width + TCOD_ctx.font_width/2;
			keyy = ((int)(' ') % TCOD_ctx.fontNbCharVertic ) * TCOD_ctx.font_height + TCOD_ctx.font_height/2;
		}
		pixel=(Uint8 *)(charmap->pixels) + keyy * charmap->pitch + keyx * charmap->format->BytesPerPixel;
		fontKeyCol.r=*((pixel)+charmap->format->Rshift/8);
		fontKeyCol.g=*((pixel)+charmap->format->Gshift/8);
		fontKeyCol.b=*((pixel)+charmap->format->Bshift/8);
		printf ("key color : %d %d %d\n",fontKeyCol.r,fontKeyCol.g,fontKeyCol.b);
		if ( ! TCOD_ctx.font_greyscale && charmap->format->BytesPerPixel == 4 ) {
			/* 32 bits font but alpha layer not used. convert to 24 bits (faster) */
			SDL_Surface *temp;
			printf ("32bits font with no alpha => converting to faster 24 bits\n");
			temp=(SDL_Surface *)TCOD_sys_get_surface(charmap->w,charmap->h,false);
			SDL_BlitSurface(charmap,NULL,temp,NULL);
			SDL_FreeSurface(charmap);
			charmap=temp;
		}
	}
	/* detect colored tiles */
	for (i=0; i < TCOD_ctx.fontNbCharHoriz*TCOD_ctx.fontNbCharVertic; i++ ) {
		int px,py,cx,cy;
		bool end=false;
		cx=(i%TCOD_ctx.fontNbCharHoriz);
		cy=(i/TCOD_ctx.fontNbCharHoriz);
		for( px=0; !end && px < TCOD_ctx.font_width; px++ ) {
			for (py=0; !end && py < TCOD_ctx.font_height; py++ ) {
					Uint8 *pixel=(Uint8 *)(charmap->pixels) + (cy*TCOD_ctx.font_height+py) * charmap->pitch 
						+ (cx*TCOD_ctx.font_width+px) * charmap->format->BytesPerPixel;
					Uint8 r=*((pixel)+charmap->format->Rshift/8);
					Uint8 g=*((pixel)+charmap->format->Gshift/8);
					Uint8 b=*((pixel)+charmap->format->Bshift/8);
					if ( charmap->format->BytesPerPixel == 3 ) {
						/* ignore key color */
						if ( r == fontKeyCol.r  && g == fontKeyCol.g && b == fontKeyCol.b ) continue;
					}
					/* colored tile if a pixel is not desaturated */
					if ( r != g || g !=b || b != r ) {
						TCOD_ctx.colored[i]=true;
						printf ("character for ascii code %d is colored\n",i);
						end=true;
					}
			}
		}
	}	
	/* convert 24/32 bits greyscale to 32bits font with alpha layer */
	if ( ! hasTransparent && TCOD_ctx.font_greyscale ) {
		bool invert=( fontKeyCol.r > 128 ); /* black on white font ? */
		/* convert the surface to 32 bits if needed */
		if ( charmap->format->BytesPerPixel != 4 ) {
			SDL_Surface *temp;
			printf("24bits greyscale font. converting to 32bits\n");
			temp=(SDL_Surface *)TCOD_sys_get_surface(charmap->w,charmap->h,true);
			SDL_BlitSurface(charmap,NULL,temp,NULL);
			SDL_FreeSurface(charmap);
			charmap=temp;
		}
		for (i=0; i < TCOD_ctx.fontNbCharHoriz*TCOD_ctx.fontNbCharVertic; i++ ) {
			int cx,cy;
			cx=(i%TCOD_ctx.fontNbCharHoriz);
			cy=(i/TCOD_ctx.fontNbCharHoriz);			
			/* fill the surface with white (except colored tiles), use alpha layer for characters */
			for (x=cx*TCOD_ctx.font_width; x < (cx+1)*TCOD_ctx.font_width; x ++ ) {
				for (y=cy*TCOD_ctx.font_height;y < (cy+1)*TCOD_ctx.font_height; y++ ) {
					if ( ! TCOD_ctx.colored[i]) {
						Uint8 *pixel=(Uint8 *)(charmap->pixels) + y * charmap->pitch + x * charmap->format->BytesPerPixel;
						Uint8 r=*((pixel)+charmap->format->Rshift/8);
						*((pixel)+charmap->format->Ashift/8) = (invert ? 255-r : r);
						*((pixel)+charmap->format->Rshift/8)=255;
						*((pixel)+charmap->format->Gshift/8)=255;
						*((pixel)+charmap->format->Bshift/8)=255;
					} else {
						Uint8 *pixel=(Uint8 *)(charmap->pixels) + y * charmap->pitch + x * charmap->format->BytesPerPixel;
						Uint8 r=*((pixel)+charmap->format->Rshift/8);
						Uint8 g=*((pixel)+charmap->format->Gshift/8);
						Uint8 b=*((pixel)+charmap->format->Bshift/8);
						if ( r == fontKeyCol.r && g == fontKeyCol.g && b == fontKeyCol.b ) {
							*((pixel)+charmap->format->Ashift/8) = 0;
						} else {
							*((pixel)+charmap->format->Ashift/8) = 255;							
						}
					}
				}
			}
		}
	}
	sdl_key=SDL_MapRGB(charmap->format,fontKeyCol.r,fontKeyCol.g,fontKeyCol.b);
	rgb_mask=charmap->format->Rmask|charmap->format->Gmask|charmap->format->Bmask;
	nrgb_mask = ~ rgb_mask;
	sdl_key &= rgb_mask; /* remove the alpha part */
	if ( charmap->format->BytesPerPixel == 3 ) SDL_SetColorKey(charmap,SDL_TRUE|SDL_RLEACCEL,sdl_key);
	for (i=0; i < TCOD_ctx.fontNbCharHoriz*TCOD_ctx.fontNbCharVertic; i++ ) {
		charcols[i]=fontKeyCol;
		first_draw[i]=true;
	}
	check_ascii_to_tcod();
	if (!TCOD_ctx.font_tcod_layout) {
		/* apply standard ascii mapping */
		if ( TCOD_ctx.font_in_row ) {
			/* for font in row */
			for (i=0; i < TCOD_ctx.max_font_chars; i++ ) TCOD_ctx.ascii_to_tcod[i]=i;
		} else {
			/* for font in column */
			for (i=0; i < TCOD_ctx.max_font_chars; i++ ) {
				int fy = i % TCOD_ctx.fontNbCharVertic;
				int fx = i / TCOD_ctx.fontNbCharVertic;
				TCOD_ctx.ascii_to_tcod[i]=fx + fy * TCOD_ctx.fontNbCharHoriz;
			}
		}
	}
}

void TCOD_sys_set_custom_font(const char *fontFile,int nb_ch, int nb_cv, int flags) {
	strcpy(TCOD_ctx.font_file,fontFile);
	/* if layout not defined, assume ASCII_INCOL */
	if (flags == 0 || flags == TCOD_FONT_TYPE_GREYSCALE) flags |= TCOD_FONT_LAYOUT_ASCII_INCOL;
	TCOD_ctx.font_in_row=((flags & TCOD_FONT_LAYOUT_ASCII_INROW) != 0);
	TCOD_ctx.font_greyscale = ((flags & TCOD_FONT_TYPE_GREYSCALE) != 0 );
	TCOD_ctx.font_tcod_layout = ((flags & TCOD_FONT_LAYOUT_TCOD) != 0 );
	if ( nb_ch> 0 ) {
		TCOD_ctx.fontNbCharHoriz=nb_ch;
		TCOD_ctx.fontNbCharVertic=nb_cv;
	} else {
		if ( ( flags & TCOD_FONT_LAYOUT_ASCII_INROW ) || ( flags & TCOD_FONT_LAYOUT_ASCII_INCOL )  ) {
			TCOD_ctx.fontNbCharHoriz=16;
			TCOD_ctx.fontNbCharVertic=16;
		} else {
			TCOD_ctx.fontNbCharHoriz=32;
			TCOD_ctx.fontNbCharVertic=8;
		}
	}
	if ( TCOD_ctx.font_tcod_layout ) TCOD_ctx.font_in_row=true;
	check_ascii_to_tcod();
	/*
	screw up things on linux64. apparently, useless
	TCOD_sys_load_font();
	*/
}

void find_resolution(void) {
	TCOD_ctx.actual_fullscreen_width=TCOD_ctx.fullscreen_width>TCOD_ctx.root->w*TCOD_ctx.font_width?TCOD_ctx.fullscreen_width:TCOD_ctx.root->w*TCOD_ctx.font_width;
	TCOD_ctx.actual_fullscreen_height=TCOD_ctx.fullscreen_height>TCOD_ctx.root->h*TCOD_ctx.font_height?TCOD_ctx.fullscreen_height:TCOD_ctx.root->h*TCOD_ctx.font_height;
	sdl->get_closest_mode(&TCOD_ctx.actual_fullscreen_width,&TCOD_ctx.actual_fullscreen_height);
}

void *TCOD_sys_create_bitmap_for_console(TCOD_console_t console) {
	int w,h;
	w = TCOD_console_get_width(console) * TCOD_ctx.font_width;
	h = TCOD_console_get_height(console) * TCOD_ctx.font_height;
	return TCOD_sys_get_surface(w,h,false);
}

static void TCOD_sys_render(void *vbitmap, int console_width, int console_height, TCOD_render_state_t *render_state) {
	sdl->render(vbitmap, console_width, console_height, render_state);
}

void TCOD_sys_console_to_bitmap(void *vbitmap, int console_width, int console_height, TCOD_render_state_t *render_state) {
	static SDL_Surface *charmap_backup=NULL;
	SDL_Surface *bitmap = (SDL_Surface *)vbitmap;
	int x,y;
	Uint32 sdl_back=0, sdl_fore=0;
	TCOD_color_t fading_color = TCOD_console_get_fading_color();
	int fade = (int)TCOD_console_get_fade();
	bool track_changes = (oldFade == fade && render_state->oldbuf);
	Uint8 bpp = charmap->format->BytesPerPixel;
	char_t *c=&render_state->buf[0];
	char_t *oc;
	TCOD_color_t *ofg, *obg, *nfg, *nbg;
	int hdelta;
	if (render_state->clear_screen) {
		track_changes = false;
		render_state->clear_screen = false;
	}
	if ( bpp == 4 ) {
		hdelta=(charmap->pitch - TCOD_ctx.font_width*bpp)/4;
	} else {
		hdelta=(charmap->pitch - TCOD_ctx.font_width*bpp);
	}
	nfg = TCOD_image_get_colors(render_state->fg_colors);
	nbg = TCOD_image_get_colors(render_state->bg_colors);
	if (track_changes) {
		oc = &render_state->oldbuf[0];
		ofg = TCOD_image_get_colors(render_state->fg_colors_prev);
		obg = TCOD_image_get_colors(render_state->bg_colors_prev);
	}
	if ( charmap_backup == NULL ) {
		charmap_backup=(SDL_Surface *)TCOD_sys_get_surface(charmap->w,charmap->h,true);
		SDL_BlitSurface(charmap,NULL,charmap_backup,NULL);
	}
#ifdef USE_SDL_LOCKS
	if ( SDL_MUSTLOCK( bitmap ) && SDL_LockSurface( bitmap ) < 0 ) return;
#endif
	for (y=0;y<console_height;y++) {
		for (x=0; x<console_width; x++) {
			SDL_Rect srcRect,dstRect;
			bool changed=true;
			if ( c->cf == -1 ) c->cf = TCOD_ctx.ascii_to_tcod[c->c];
			if ( track_changes ) {
				changed=false;
				if (c->dirty || ascii_updated[c->c] ||
					nbg->r != obg->r || nbg->g != obg->g || nbg->b != obg->b ||
					nfg->r != ofg->r || nfg->g != ofg->g || nfg->b != ofg->b ||
					c->c != oc->c || c->cf != oc->cf) {
					changed=true;
				}
			}
			c->dirty=0;
			if ( changed ) {
				TCOD_color_t b=*nbg;
				dstRect.x=x*TCOD_ctx.font_width;
				dstRect.y=y*TCOD_ctx.font_height;
				dstRect.w=TCOD_ctx.font_width;
				dstRect.h=TCOD_ctx.font_height;
				/* draw background */
				if ( fade != 255 ) {
					b.r = ((int)b.r) * fade / 255 + ((int)fading_color.r) * (255-fade)/255;
					b.g = ((int)b.g) * fade / 255  + ((int)fading_color.g) * (255-fade)/255;
					b.b = ((int)b.b) * fade / 255 + ((int)fading_color.b) * (255-fade)/255;
				}
				sdl_back=SDL_MapRGB(bitmap->format,b.r,b.g,b.b);
				SDL_FillRect(bitmap,&dstRect,sdl_back);
				if ( c->c != ' ' ) {
					/* draw foreground */
					int ascii=c->cf;
					TCOD_color_t *curtext = &charcols[ascii];
					bool first = first_draw[ascii];
					TCOD_color_t f=*nfg;

					if ( fade != 255 ) {
						f.r = ((int)f.r) * fade / 255 + ((int)fading_color.r) * (255-fade)/255;
						f.g = ((int)f.g) * fade / 255 + ((int)fading_color.g) * (255-fade)/255;
						f.b = ((int)f.b) * fade / 255 + ((int)fading_color.b) * (255-fade)/255;
					}
					/* only draw character if foreground color != background color */
					if ( ascii_updated[c->c] || f.r != b.r || f.g != b.g || f.b != b.b ) {
						if ( charmap->format->Amask == 0
							&& f.r == fontKeyCol.r && f.g == fontKeyCol.g && f.b == fontKeyCol.b ) {
							/* cannot draw with the key color... */
							if ( f.r < 255 ) f.r++; else f.r--;
						}
						srcRect.x = (ascii%TCOD_ctx.fontNbCharHoriz)*TCOD_ctx.font_width;
						srcRect.y = (ascii/TCOD_ctx.fontNbCharHoriz)*TCOD_ctx.font_height;
						srcRect.w=TCOD_ctx.font_width;
						srcRect.h=TCOD_ctx.font_height;

						if ( charmap && (first || curtext->r != f.r || curtext->g != f.g || curtext->b!=f.b) ) {
							/* change the character color in the font */
					    	first_draw[ascii]=false;
							sdl_fore=SDL_MapRGB(charmap->format,f.r,f.g,f.b) & rgb_mask;
							*curtext=f;
#ifdef USE_SDL_LOCKS
							if ( SDL_MUSTLOCK(charmap) ) {
								if ( SDL_LockSurface(charmap) < 0 ) return;
							}
#endif
							if ( bpp == 4 ) {
								/* 32 bits font : fill the whole character with foreground color */
								Uint32 *pix = (Uint32 *)(((Uint8 *)charmap->pixels)+srcRect.x*bpp + srcRect.y*charmap->pitch);
								int h=TCOD_ctx.font_height;
								if ( ! TCOD_ctx.colored[ascii] ) {
									while ( h-- ) {
										int w=TCOD_ctx.font_width;
										while ( w-- ) {
											(*pix) &= nrgb_mask;
											(*pix) |= sdl_fore;
											pix++;
										}
										pix += hdelta;
									}
								} else {
									/* colored character : multiply color with foreground color */
									Uint32 *pixorig = (Uint32 *)(((Uint8 *)charmap_backup->pixels)+srcRect.x*bpp + srcRect.y*charmap_backup->pitch);
									int hdelta_backup=(charmap_backup->pitch - TCOD_ctx.font_width*4)/4;
									while (h> 0) {
										int w=TCOD_ctx.font_width;
										while ( w > 0 ) {
											int r=(int)(*((Uint8 *)(pixorig)+charmap_backup->format->Rshift/8));
											int g=(int)(*((Uint8 *)(pixorig)+charmap_backup->format->Gshift/8));
											int b=(int)(*((Uint8 *)(pixorig)+charmap_backup->format->Bshift/8));
											(*pix) &= nrgb_mask; /* erase the color */
											r = r * f.r / 255;
											g = g * f.g / 255;
											b = b * f.b / 255;
											/* set the new color */
											(*pix) |= (r<<charmap->format->Rshift)|(g<<charmap->format->Gshift)|(b<<charmap->format->Bshift);
											w--;
											pix++;
											pixorig++;
										}
										h--;
										pix += hdelta;
										pixorig += hdelta_backup;								
									}
								}
							} else	{
								/* 24 bits font : fill only non key color pixels */
								Uint32 *pix = (Uint32 *)(((Uint8 *)charmap->pixels)+srcRect.x*bpp + srcRect.y*charmap->pitch);
								int h=TCOD_ctx.font_height;
								if ( ! TCOD_ctx.colored[ascii] ) {
									while ( h-- ) {
										int w=TCOD_ctx.font_width;
										while ( w-- ) {
											if (((*pix) & rgb_mask) != sdl_key ) {
												(*pix) &= nrgb_mask;
												(*pix) |= sdl_fore;
											}
											pix = (Uint32 *) (((Uint8 *)pix)+3);
										}
										pix = (Uint32 *) (((Uint8 *)pix)+hdelta);
									}
								} else {
									/* colored character : multiply color with foreground color */
									Uint32 *pixorig = (Uint32 *)(((Uint8 *)charmap_backup->pixels)+srcRect.x*4 + srcRect.y*charmap_backup->pitch);
									/* charmap_backup is always 32 bits */
									int hdelta_backup=(charmap_backup->pitch - TCOD_ctx.font_width*4)/4;
									while (h> 0) {
										int w=TCOD_ctx.font_width;
										while ( w > 0 ) {
											if (((*pixorig) & rgb_mask) != sdl_key ) {
												int r=(int)(*((Uint8 *)(pixorig)+charmap_backup->format->Rshift/8));
												int g=(int)(*((Uint8 *)(pixorig)+charmap_backup->format->Gshift/8));
												int b=(int)(*((Uint8 *)(pixorig)+charmap_backup->format->Bshift/8));
												(*pix) &= nrgb_mask; /* erase the color */
												r = r * f.r / 255;
												g = g * f.g / 255;
												b = b * f.b / 255;
												/* set the new color */
												(*pix) |= (r<<charmap->format->Rshift)|(g<<charmap->format->Gshift)|(b<<charmap->format->Bshift); 
											}
											w--;
											pix = (Uint32 *) (((Uint8 *)pix)+3);
											pixorig ++;
										}
										h--;
										pix = (Uint32 *) (((Uint8 *)pix)+hdelta);
										pixorig += hdelta_backup;
									}
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
			c++;
			nfg++;
			nbg++;
			if (track_changes) {
				oc++;
				ofg++;
				obg++;
			}
		}
	}
#ifdef USE_SDL_LOCKS
	if ( SDL_MUSTLOCK( bitmap ) ) SDL_UnlockSurface( bitmap );
#endif
}

void *TCOD_sys_get_surface(int width, int height, bool alpha) {
	return sdl->create_surface(width,height,alpha);
}

void TCOD_sys_update_char(int asciiCode, int fontx, int fonty, TCOD_image_t img, int x, int y) {
	int px,py;
	int iw,ih;
	static TCOD_color_t pink={255,0,255};
	TCOD_sys_map_ascii_to_font(asciiCode,fontx,fonty);
	TCOD_image_get_size(img,&iw,&ih);
	for (px=0; px < TCOD_ctx.font_width; px ++ ) {
		for (py=0;py < TCOD_ctx.font_height; py++ ) {
			TCOD_color_t col=TCOD_white;
			Uint8 *pixel;
			Uint8 bpp;
			if ( (unsigned)(x+px) < (unsigned)iw && (unsigned)(y+py) < (unsigned)ih ) {
				col=TCOD_image_get_pixel(img,x+px,y+py);
			}
			pixel=(Uint8 *)(charmap->pixels) + (fonty*TCOD_ctx.font_height+py) * charmap->pitch + (fontx*TCOD_ctx.font_width+px) * charmap->format->BytesPerPixel;
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
	/* TODO : improve this. */
	charcols[asciiCode]=pink;
	ascii_updated[asciiCode]=true;
	any_ascii_updated=true;
}

#ifdef TCOD_MACOSX
void CustomSDLMain();
#endif

void TCOD_sys_startup(void) {
	if (has_startup) return;
	sdl = SDL_implementation_factory();
#ifdef TCOD_MACOSX
	CustomSDLMain();
#endif
#ifndef NDEBUG
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
#endif
	TCOD_IFNOT(SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO) >= 0 ) return;
#ifndef	TCOD_WINDOWS
	/* not needed and might crash on windows */
	atexit(SDL_Quit);
#endif
	memset(&TCOD_ctx.key_state,0,sizeof(TCOD_key_t));
	TCOD_ctx.max_font_chars=256;
	alloc_ascii_tables();
#ifndef NO_OPENGL
	TCOD_opengl_init_attributes();
#endif

	has_startup=true;
}

static void TCOD_sys_load_player_config(void) {
	const char *renderer;	
	const char *font;	
	int fullscreenWidth,fullscreenHeight;

	/* define file structure */
	TCOD_parser_t parser=TCOD_parser_new();
	TCOD_parser_struct_t libtcod = TCOD_parser_new_struct(parser, "libtcod");
	TCOD_struct_add_property(libtcod, "renderer", TCOD_TYPE_STRING, false);
	TCOD_struct_add_property(libtcod, "font", TCOD_TYPE_STRING, false);
	TCOD_struct_add_property(libtcod, "fontInRow", TCOD_TYPE_BOOL, false);
	TCOD_struct_add_property(libtcod, "fontGreyscale", TCOD_TYPE_BOOL, false);
	TCOD_struct_add_property(libtcod, "fontTcodLayout", TCOD_TYPE_BOOL, false);
	TCOD_struct_add_property(libtcod, "fontNbCharHoriz", TCOD_TYPE_INT, false);
	TCOD_struct_add_property(libtcod, "fontNbCharVertic", TCOD_TYPE_INT, false);
	TCOD_struct_add_property(libtcod, "fullscreen", TCOD_TYPE_BOOL, false);
	TCOD_struct_add_property(libtcod, "fullscreenWidth", TCOD_TYPE_INT, false);
	TCOD_struct_add_property(libtcod, "fullscreenHeight", TCOD_TYPE_INT, false);
	TCOD_struct_add_property(libtcod, "fullscreenScaling", TCOD_TYPE_BOOL, false);

	/* parse file */
	TCOD_parser_run(parser,"./libtcod.cfg",NULL);

	/* set user preferences */
	renderer=TCOD_parser_get_string_property(parser, "libtcod.renderer");
	if ( renderer != NULL ) {
		/* custom renderer */
		if ( TCOD_strcasecmp(renderer,"GLSL") == 0 ) TCOD_ctx.renderer=TCOD_RENDERER_GLSL;
		else if ( TCOD_strcasecmp(renderer,"OPENGL") == 0 ) TCOD_ctx.renderer=TCOD_RENDERER_OPENGL;
		else if ( TCOD_strcasecmp(renderer,"SDL") == 0 ) TCOD_ctx.renderer=TCOD_RENDERER_SDL;
		else printf ("Warning : unknown renderer '%s' in libtcod.cfg\n", renderer);
	}
	font=TCOD_parser_get_string_property(parser, "libtcod.font");
	if ( font != NULL ) {
		/* custom font */
		if ( TCOD_sys_file_exists(font)) {
			int fontNbCharHoriz,fontNbCharVertic;
			strcpy(TCOD_ctx.font_file,font);
			TCOD_ctx.font_in_row=TCOD_parser_get_bool_property(parser,"libtcod.fontInRow");
			TCOD_ctx.font_greyscale=TCOD_parser_get_bool_property(parser,"libtcod.fontGreyscale");
			TCOD_ctx.font_tcod_layout=TCOD_parser_get_bool_property(parser,"libtcod.fontTcodLayout");
			fontNbCharHoriz=TCOD_parser_get_int_property(parser,"libtcod.fontNbCharHoriz");
			fontNbCharVertic=TCOD_parser_get_int_property(parser,"libtcod.fontNbCharVertic");
			if ( fontNbCharHoriz > 0 ) TCOD_ctx.fontNbCharHoriz=fontNbCharHoriz;
			if ( fontNbCharVertic > 0 ) TCOD_ctx.fontNbCharVertic=fontNbCharVertic;
			if ( charmap ) {
				SDL_FreeSurface(charmap);
				charmap=NULL;
			}
		} else {
			printf ("Warning : font file '%s' does not exist\n",font);
		}
	}
	/* custom fullscreen resolution */
	TCOD_ctx.fullscreen=TCOD_parser_get_bool_property(parser,"libtcod.fullscreen");
	fullscreenWidth=TCOD_parser_get_int_property(parser,"libtcod.fullscreenWidth");
	fullscreenHeight=TCOD_parser_get_int_property(parser,"libtcod.fullscreenHeight");
	if ( fullscreenWidth > 0 ) TCOD_ctx.fullscreen_width=fullscreenWidth;
	if ( fullscreenHeight > 0 ) TCOD_ctx.fullscreen_height=fullscreenHeight;
}


TCOD_renderer_t TCOD_sys_get_renderer(void) {
	return TCOD_ctx.renderer;
}

void TCOD_sys_set_renderer(TCOD_renderer_t renderer) {
	if ( renderer == TCOD_ctx.renderer ) return;
	TCOD_ctx.renderer=renderer;
	if ( window ) {
		TCOD_sys_term();
	}
	TCOD_sys_init(TCOD_ctx.root->w,TCOD_ctx.root->h,&TCOD_ctx.root->state,TCOD_ctx.fullscreen);
	TCOD_console_set_window_title(TCOD_ctx.window_title);
	TCOD_console_set_dirty(0,0,TCOD_ctx.root->w,TCOD_ctx.root->h);
}

void TCOD_sys_init_screen_offset(void) {
	TCOD_ctx.fullscreen_offsetx=(TCOD_ctx.actual_fullscreen_width-TCOD_ctx.root->w*TCOD_ctx.font_width)/2;
	TCOD_ctx.fullscreen_offsety=(TCOD_ctx.actual_fullscreen_height-TCOD_ctx.root->h*TCOD_ctx.font_height)/2;
}

bool TCOD_sys_init(int w,int h, TCOD_render_state_t *render_state, bool fullscreen) {
	static TCOD_renderer_t last_renderer=TCOD_RENDERER_SDL;
	static char last_font[512]="";
	if ( ! has_startup ) TCOD_sys_startup();
	/* check if there is a user (player) config file */
	if ( TCOD_sys_file_exists("./libtcod.cfg")) {
		/* yes, read it */
		TCOD_sys_load_player_config();
		if (TCOD_ctx.fullscreen) fullscreen=true;
	}
	if (last_renderer != TCOD_ctx.renderer || ! charmap || strcmp(last_font,TCOD_ctx.font_file) != 0) {
		/* reload the font when switching renderer to restore original character colors */
		TCOD_sys_load_font();
	}
	sdl->create_window(w,h,fullscreen);
	renderState = render_state;
	memset(key_status,0,sizeof(bool)*(TCODK_CHAR+1));
	memset(ascii_updated,0,sizeof(bool)*TCOD_ctx.max_font_chars);
	return true;
}

void TCOD_sys_save_bitmap(void *bitmap, const char *filename) {
	image_support_t *img=image_type;
	while ( img->extension != NULL && strcasestr(filename,img->extension) == NULL ) img++;
	if ( img->extension == NULL || img->write == NULL ) img=image_type; /* default to bmp */
	img->write((SDL_Surface *)bitmap,filename);
}

void TCOD_sys_save_screenshot(const char *filename) {
	char buf[128];
	if ( filename == NULL ) {
		/* generate filename */
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
	sdl->save_screenshot(filename);
}

void TCOD_sys_set_fullscreen(bool fullscreen) {
	TCOD_ctx.fullscreen=fullscreen;
	sdl->set_fullscreen(fullscreen);
}

void TCOD_sys_set_scale_factor(float value) {
	float old_scale_factor = scale_factor;
	scale_factor = value;
	if (scale_factor + 1e-3 < scale_data.min_scale_factor)
		scale_factor = scale_data.min_scale_factor;
	else if (scale_factor - 1e-3 > MAX_SCALE_FACTOR)
		scale_factor = MAX_SCALE_FACTOR;
	printf("scale_factor: %0.3f -> %0.3f (wanted: %0.3f)", old_scale_factor, scale_factor, value);
}

void TCOD_sys_set_window_title(const char *title) {
	strcpy(TCOD_ctx.window_title,title);
	sdl->set_window_title(title);
}

void TCOD_sys_flush(bool render) {
	static uint32 old_time,new_time=0, elapsed=0;
	int32 frame_time,time_to_wait;
	if ( render ) {
		TCOD_sys_render(NULL, TCOD_console_get_width(NULL), TCOD_console_get_height(NULL), renderState);
	}
	old_time=new_time;
	new_time=TCOD_sys_elapsed_milli();
	/* If TCOD has been terminated and restarted. */
	if (old_time > new_time)
		old_time = elapsed = 0;
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

static char TCOD_sys_get_vk(SDL_Keycode sdl_key) {
	int i;
	for (i = 0; i < NUM_VK_TO_C_ENTRIES; i++) {
		if (vk_to_c[i].sdl_key == sdl_key) {
			vk_to_c[i].sdl_key = 0;
			return vk_to_c[i].tcod_key;
		}
	}
	return 0;
}

static void TCOD_sys_set_vk(SDL_Keycode sdl_key, char tcod_key) {
	int i;
	for (i = 0; i < NUM_VK_TO_C_ENTRIES; i++) {
		if (vk_to_c[i].sdl_key == 0) {
			vk_to_c[i].sdl_key = sdl_key;
			vk_to_c[i].tcod_key = tcod_key;
			break;
		}
	}
}

static void TCOD_sys_convert_event(SDL_Event *ev, TCOD_key_t *ret) {
	SDL_KeyboardEvent *kev=&ev->key;
	/* SDL2 does not map keycodes and modifiers to characters, this is on the developer.
		Presumably in order to avoid problems with different keyboard layouts, they
		are expected to write their own key mapping editing code for the user.  */
	if (SDLK_SCANCODE_MASK == (kev->keysym.sym & SDLK_SCANCODE_MASK))
		ret->c = 0;
	else
		ret->c = kev->keysym.sym;
	if ( ( kev->keysym.mod & (KMOD_LCTRL|KMOD_RCTRL) ) != 0 ) {
		/* when pressing CTRL-A, we don't get unicode for 'a', but unicode for CTRL-A = 1. Fix it */
		if ( kev->keysym.sym >= SDLK_a && kev->keysym.sym <= SDLK_z ) {
			ret->c = 'a'+(kev->keysym.sym - SDLK_a);
		}
	}
	if ( ev->type == SDL_KEYDOWN ) TCOD_sys_set_vk(kev->keysym.sym, ret->c);
	else if (ev->type == SDL_KEYUP ) ret->c = TCOD_sys_get_vk(kev->keysym.sym);
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
		case SDLK_PRINTSCREEN : ret->vk=TCODK_PRINTSCREEN;break;
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
		case SDLK_RGUI : ret->vk=TCODK_RWIN;break;
		case SDLK_LGUI : ret->vk=TCODK_LWIN;break;
		case SDLK_NUMLOCKCLEAR : ret->vk=TCODK_NUMLOCK;break;
		case SDLK_KP_0 : ret->vk=TCODK_KP0;break;
		case SDLK_KP_1 : ret->vk=TCODK_KP1;break;
		case SDLK_KP_2 : ret->vk=TCODK_KP2;break;
		case SDLK_KP_3 : ret->vk=TCODK_KP3;break;
		case SDLK_KP_4 : ret->vk=TCODK_KP4;break;
		case SDLK_KP_5 : ret->vk=TCODK_KP5;break;
		case SDLK_KP_6 : ret->vk=TCODK_KP6;break;
		case SDLK_KP_7 : ret->vk=TCODK_KP7;break;
		case SDLK_KP_8 : ret->vk=TCODK_KP8;break;
		case SDLK_KP_9 : ret->vk=TCODK_KP9;break;
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
	TCOD_key_t *ret = &TCOD_ctx.key_state;
	ret->vk=TCODK_NONE;
	ret->c=0;
	ret->pressed=0;
	switch (ev->type) {
		/* handled in TCOD_sys_handle_event */
		/*
		case SDL_QUIT :
		case SDL_VIDEOEXPOSE :
		case SDL_MOUSEBUTTONDOWN : {
		case SDL_MOUSEBUTTONUP : {
		break;
		*/
		case SDL_KEYUP : {
			SDL_KeyboardEvent *kev=&ev->key;
			TCOD_key_t tmpkey;
			switch(kev->keysym.sym) {
				case SDLK_LALT : ret->lalt=0; break;
				case SDLK_RALT : ret->ralt=0; break;
				case SDLK_LCTRL : ret->lctrl=0; break;
				case SDLK_RCTRL : ret->rctrl=0; break;
				case SDLK_LSHIFT : ret->shift=0; break;
				case SDLK_RSHIFT : ret->shift=0; break;
				case SDLK_LGUI : ret->lmeta=0; break;
				case SDLK_RGUI : ret->rmeta=0; break;
				default:break;
			}
			TCOD_sys_convert_event(ev,&tmpkey);
			key_status[tmpkey.vk]=false;
			if ( flags & TCOD_KEY_RELEASED ) {
				ret->vk=tmpkey.vk;
				ret->c=tmpkey.c;
				ret->pressed=0;
			}
		}
		break;
		case SDL_KEYDOWN : {
			SDL_KeyboardEvent *kev=&ev->key;
			TCOD_key_t tmpkey;
			switch(kev->keysym.sym) {
				case SDLK_LALT : ret->lalt=1; break;
				case SDLK_RALT : ret->ralt=1; break;
				case SDLK_LCTRL : ret->lctrl=1; break;
				case SDLK_RCTRL : ret->rctrl=1; break;
				case SDLK_LSHIFT : ret->shift=1; break;
				case SDLK_RSHIFT : ret->shift=1; break;
				case SDLK_LGUI : ret->lmeta=1; break;
				case SDLK_RGUI : ret->rmeta=1; break;
				default : break;
			}
			TCOD_sys_convert_event(ev,&tmpkey);
			key_status[tmpkey.vk]=true;
			if ( flags & TCOD_KEY_PRESSED ) {
				ret->vk=tmpkey.vk;
				ret->c=tmpkey.c;
				ret->pressed=1;
			}
		}
		break;
	}
	return *ret;
}

bool TCOD_sys_is_key_pressed(TCOD_keycode_t key) {
	return key_status[key];
}

#ifdef TCOD_TOUCH_INPUT
static TCOD_touch_t tcod_touch={0};

static int TCOD_sys_get_touch_finger_index(SDL_FingerID fingerId) {
	int i;
	for (i = 0; i < tcod_touch.nfingers; i++)
		if (tcod_touch.finger_ids[i] == fingerId)
			return i;
	if (i == tcod_touch.nfingers && i+1 <= MAX_TOUCH_FINGERS) {
		tcod_touch.nfingers += 1;
		tcod_touch.finger_ids[i] = fingerId;
		return i;
	}
	return -1;
}
#endif

void TCOD_sys_unproject_screen_coords(int sx, int sy, int *ssx, int *ssy) {
	if (scale_data.dst_display_width != 0 ) {
		*ssx = (scale_data.src_x0 + ((sx - scale_data.dst_offset_x) * scale_data.src_copy_width) / scale_data.dst_display_width);
		*ssy = (scale_data.src_y0 + ((sy - scale_data.dst_offset_y) * scale_data.src_copy_width) / scale_data.dst_display_width);
	} else {
		*ssx=sx - TCOD_ctx.fullscreen_offsetx;
		*ssy=sy - TCOD_ctx.fullscreen_offsety;
	}
}

void TCOD_sys_convert_console_to_screen_coords(int cx, int cy, int *sx, int *sy) {
	*sx = scale_data.dst_offset_x + ((cx * TCOD_ctx.font_width - scale_data.src_x0) * scale_data.dst_display_width) / scale_data.src_copy_width;
	*sy = scale_data.dst_offset_y + ((cy * TCOD_ctx.font_height - scale_data.src_y0) * scale_data.dst_display_height) / scale_data.src_copy_height;
}

void TCOD_sys_convert_screen_to_console_coords(int sx, int sy, int *cx, int *cy) {
	int ssx, ssy;
	TCOD_sys_unproject_screen_coords(sx, sy, &ssx, &ssy);
	*cx = ssx / TCOD_ctx.font_width;
	*cy = ssy / TCOD_ctx.font_height;
}

static TCOD_mouse_t tcod_mouse={0,0,0,0,0,0,0,0,false,false,false,false,false,false,false,false};
static TCOD_event_t TCOD_sys_handle_event(SDL_Event *ev,TCOD_event_t eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse) {
	TCOD_event_t retMask=0;
	key->text[0] = '\0';
	/* printf("TCOD_sys_handle_event type=%04x\n", ev->type); */
	switch(ev->type) {
		case SDL_KEYDOWN : {		 
			TCOD_key_t tmpKey=TCOD_sys_SDLtoTCOD(ev,TCOD_KEY_PRESSED);
			if ( (TCOD_EVENT_KEY_PRESS & eventMask) != 0) {
				retMask|=TCOD_EVENT_KEY_PRESS; 
				if ( key ) *key = tmpKey; 
				return retMask;					
			}
		}
		break;
		case SDL_KEYUP : { 
			TCOD_key_t tmpKey=TCOD_sys_SDLtoTCOD(ev,TCOD_KEY_RELEASED);
			if ( (TCOD_EVENT_KEY_RELEASE & eventMask) != 0) {
				retMask|=TCOD_EVENT_KEY_RELEASE; 
				if ( key ) *key = tmpKey;
				return retMask;					
			}
		}
		break;
		case SDL_TEXTINPUT: {
			SDL_TextInputEvent *iev=&ev->text;
			TCOD_key_t ret;
			ret.vk = TCODK_TEXT;
			ret.pressed = 1;
			strncpy(ret.text, iev->text, TCOD_KEY_TEXT_SIZE);
			*key = ret;
			return retMask | TCOD_EVENT_KEY_PRESS; 
		}
		break;
#ifdef TCOD_TOUCH_INPUT
		/*
		 * Need to distinguish between:
		 * - Tap: Can be optionally delegated to a mouse press.
		 * - Touch and drag: Should affect scaling screen position.
		 *
  		 */
		case SDL_FINGERDOWN :
		case SDL_FINGERUP :
		case SDL_FINGERMOTION :
		{
			SDL_Touch *touch=SDL_GetTouch(ev->tfinger.touchId);
			int idx, mouse_touch_valid;
			float xf, yf, screen_x, screen_y;
			Uint32 ticks_taken = 0;

			/* Reset the global variable. */
			if (tcod_touch.nfingerspressed == 0) {
				tcod_touch.nupdates = 0;
				tcod_touch.nfingers = 0;
				tcod_touch.ticks0 = SDL_GetTicks();
			} else
				ticks_taken = SDL_GetTicks() - tcod_touch.ticks0;

			idx = TCOD_sys_get_touch_finger_index(ev->tfinger.fingerId);
			if (idx == -1) {
				TCOD_LOG(("ERROR: failed to allocate extra finger"));
				break;
			}

			/* Count the number of events contributing to an ongoing tracked touch (zeroed above in finger press). */
			tcod_touch.finger_id = ev->tfinger.fingerId;
			tcod_touch.nupdates += 1;

			/* We only emulate mouse events when the first finger is the only one pressed. */
			if (SDL_FINGERDOWN == ev->type) {
				tcod_touch.nfingerspressed += 1;
				tcod_touch.fingerspressed[idx] = 1;
				mouse_touch_valid = mouse_touch && tcod_touch.nfingerspressed == 1 && tcod_touch.fingerspressed[0];
			} else if (SDL_FINGERUP == ev->type) {
				mouse_touch_valid = mouse_touch && tcod_touch.nfingerspressed == 1 && tcod_touch.fingerspressed[0];
				tcod_touch.nfingerspressed -= 1;
				tcod_touch.fingerspressed[idx] = 0;
			} else
				mouse_touch_valid = mouse_touch && tcod_touch.nfingerspressed == 1 && tcod_touch.fingerspressed[0];

			/* Coordinates are raw full screen positions. */
			screen_x = (ev->tfinger.x * scale_data.surface_width) / touch->xres;
			screen_y = (ev->tfinger.y * scale_data.surface_height) / touch->yres;
			xf = (float)(screen_x - scale_data.dst_offset_x) / scale_data.dst_display_width;
			yf = (float)(screen_y - scale_data.dst_offset_y) / scale_data.dst_display_height;
			tcod_touch.coords[idx][0] = scale_data.src_x0 + scale_data.src_copy_width * xf;
			tcod_touch.coords[idx][1] = scale_data.src_y0 + scale_data.src_copy_height * yf;
			tcod_touch.coords_delta[idx][0] = (ev->tfinger.dx * scale_data.src_proportionate_width) / touch->xres;
			tcod_touch.coords_delta[idx][1] = (ev->tfinger.dy * scale_data.src_proportionate_height) / touch->yres;

			/* Console coordinates need to be mapped back from screen coordinates through scaling. */
			tcod_touch.consolecoords[idx][0] = tcod_touch.coords[idx][0] / TCOD_ctx.font_width;
			tcod_touch.consolecoords[idx][1] = tcod_touch.coords[idx][1] / TCOD_ctx.font_height;
			tcod_touch.consolecoords_delta[idx][0] = tcod_touch.coords_delta[idx][0] / TCOD_ctx.font_width;
			tcod_touch.consolecoords_delta[idx][1] = tcod_touch.coords_delta[idx][1] / TCOD_ctx.font_height;

			if (SDL_FINGERDOWN == ev->type) {
				// printf("SDL_FINGERDOWN [%d] ticks=%d", tcod_touch.nupdates, ticks_taken);
				if ((TCOD_EVENT_FINGER_PRESS & eventMask) != 0)
					retMask |= TCOD_EVENT_FINGER_PRESS;

				if (mouse_touch_valid && (TCOD_EVENT_MOUSE_PRESS & eventMask) != 0) {
					mouse->lbutton=mousebl=true;
					retMask |= TCOD_EVENT_MOUSE_PRESS;
				}
			} else if (SDL_FINGERUP == ev->type) {
				// printf("SDL_FINGERUP [%d] ticks=%d", tcod_touch.nupdates, ticks_taken);
				if ((TCOD_EVENT_FINGER_RELEASE & eventMask) != 0)
					retMask |= TCOD_EVENT_FINGER_RELEASE;

				if (mouse_touch_valid && (TCOD_EVENT_MOUSE_RELEASE & eventMask) != 0) {
					if (mousebl)
						mouse->lbutton_pressed = mouse_force_bl=true;
					mouse->lbutton = mousebl=false;
					retMask |= TCOD_EVENT_MOUSE_RELEASE;
				}
			} else if (SDL_FINGERMOTION == ev->type) {
				float scale_adjust = 1.0f, xc_shift = 0.0f, yc_shift = 0.0f;

				// printf("SDL_FINGERMOTION [%d] ticks=%d", tcod_touch.nupdates, ticks_taken);
				if ((TCOD_EVENT_FINGER_MOVE & eventMask) != 0)
					retMask |= TCOD_EVENT_FINGER_MOVE;

				if (mouse_touch_valid && (TCOD_EVENT_MOUSE_MOVE & eventMask) != 0)
					retMask |= TCOD_EVENT_MOUSE_MOVE;

				if (tcod_touch.nfingerspressed == 1) {
					/* One finger drag AKA drag to move.
					 * Ignore the first few move events that happen unhelpfully immediately after finger down. */
					if (tcod_touch.fingerspressed[0] && (tcod_touch.coords_delta[0][0] || tcod_touch.coords_delta[0][1]) && ticks_taken > 10) {
						xc_shift = (float)tcod_touch.coords_delta[idx][0] / scale_data.surface_width;
						yc_shift = (float)tcod_touch.coords_delta[idx][1] / scale_data.surface_height;
					}
				} else if (tcod_touch.nfingerspressed == 2) {
					/* Two finger pinch AKA pinch to zoom
					 * Both fingers should stay exactly where they are on the full surface
					 * in order to manipulate the drag and zoom effect.  */
					if (tcod_touch.fingerspressed[0] && tcod_touch.fingerspressed[1]) {
						/*
						 * New algorithm
						 */
						int f0x0 = tcod_touch.coords[0][0]-tcod_touch.coords_delta[0][0], f0y0 = tcod_touch.coords[0][1]-tcod_touch.coords_delta[0][1];
						int f1x0 = tcod_touch.coords[1][0]-tcod_touch.coords_delta[1][0], f1y0 = tcod_touch.coords[1][1]-tcod_touch.coords_delta[1][1];
						int f0x1 = tcod_touch.coords[0][0], f0y1 = tcod_touch.coords[0][1];
						int f1x1 = tcod_touch.coords[1][0], f1y1 = tcod_touch.coords[1][1];
						float p0x = (f1x0 + f0x0)/2.0f, p0y = (f1y0 + f0y0)/2.0f;
						float p1x = (f1x1 + f0x1)/2.0f, p1y = (f1y1 + f0y1)/2.0f;
						float len_previous = sqrtf((float)(pow(f0x0-f1x0,2) + pow(f0y0-f1y0,2)));
						float len_current = sqrt((float)(pow(f0x1-f1x1, 2) + pow(f0y1-f1y1,2)));
						scale_adjust = len_current/len_previous;
						xc_shift = ((p1x - p0x) / scale_data.surface_width);
						yc_shift = ((p1y - p0y) / scale_data.surface_height);
					}
				}

				/* Bound the translations within the console area. */
				if (fabs(xc_shift) > 1e-3f) {
					scale_xc -= xc_shift; /* Actual display shift is the inverted finger movement. */
					if (scale_xc + 1e-3f < 0.0f)
						scale_xc = 0.0f;
					if (scale_xc - 1e-3f > 1.0f)
						scale_xc = 1.0f;
				}
				if (fabs(yc_shift) > 1e-3f) {
					scale_yc -= yc_shift; /* Actual display shift is the inverted finger movement. */
					if (scale_yc + 1e-3f < 0.0f)
						scale_yc = 0.0f;
					if (scale_yc - 1e-3f > 1.0f)
						scale_yc = 1.0f;
				}
				if (fabs(scale_adjust - 1.0f) > 1e-3f)
					TCOD_sys_set_scale_factor(scale_factor * scale_adjust);
			}

			/* We need to distinguish between handleable touch events, and short distinct mouse events. */
			if (ticks_taken > 400 || tcod_touch.nfingers > 1) {
				// printf("DEF NOT MOUSE CODE[%d]", tcod_touch.nupdates);
				mouse->cx = 0;
				mouse->cy = 0;
				mouse->dcx = 0;
				mouse->dcy = 0;
			} else if (mouse_touch_valid && (retMask & (TCOD_EVENT_MOUSE_PRESS|TCOD_EVENT_MOUSE_RELEASE|TCOD_EVENT_MOUSE_MOVE)) != 0) {
				mouse->x = tcod_touch.coords[idx][0];
				mouse->y = tcod_touch.coords[idx][1];
				mouse->dx += tcod_touch.coords_delta[idx][0];
				mouse->dy += tcod_touch.coords_delta[idx][1];
				mouse->cx = tcod_touch.consolecoords[idx][0];
				mouse->cy = tcod_touch.consolecoords[idx][1];
				mouse->dcx = tcod_touch.consolecoords_delta[idx][0];
				mouse->dcy = tcod_touch.consolecoords_delta[idx][1];
				/* printf("CX,CY: %d,%d", mouse->cx,mouse->cy); */
			}

			break;
		}
#endif
		case SDL_MOUSEMOTION : 
			if ( (TCOD_EVENT_MOUSE_MOVE & eventMask) != 0) {
				SDL_MouseMotionEvent *mev=&ev->motion;
				TCOD_sys_unproject_screen_coords(mev->x, mev->y, &mouse->x, &mouse->y);
				if (scale_data.surface_width != 0) {
					mouse->dx += (mev->xrel * scale_data.src_proportionate_width) / scale_data.surface_width;
					mouse->dy += (mev->yrel * scale_data.src_proportionate_height) / scale_data.surface_height;
				}
				mouse->cx = mouse->x / TCOD_ctx.font_width;
				mouse->cy = mouse->y / TCOD_ctx.font_height;
				mouse->dcx = mouse->dx / TCOD_ctx.font_width;
				mouse->dcy = mouse->dy / TCOD_ctx.font_height;

				return retMask | TCOD_EVENT_MOUSE_MOVE;
			}
		break; 
		case SDL_MOUSEWHEEL :
			if (ev->wheel.y < 0)
				mouse->wheel_down=true;
			else
				mouse->wheel_up=true;
			return retMask | TCOD_EVENT_MOUSE_PRESS;
		break;
		case SDL_MOUSEBUTTONDOWN : 
			if ( (TCOD_EVENT_MOUSE_PRESS & eventMask) != 0) {
				SDL_MouseButtonEvent *mev=&ev->button;
				retMask|=TCOD_EVENT_MOUSE_PRESS;
				switch (mev->button) {
					case SDL_BUTTON_LEFT : mouse->lbutton=mousebl=true; break;
					case SDL_BUTTON_MIDDLE : mouse->mbutton=mousebm=true; break;
					case SDL_BUTTON_RIGHT : mouse->rbutton=mousebr=true; break;
				}
				/* update mouse position */
				if ( (TCOD_EVENT_MOUSE_MOVE & eventMask) == 0) {
					mouse->x=mev->x;
					mouse->y=mev->y;
					mouse->cx = (mouse->x - TCOD_ctx.fullscreen_offsetx) / TCOD_ctx.font_width;
					mouse->cy = (mouse->y - TCOD_ctx.fullscreen_offsety) / TCOD_ctx.font_height;
				}
				return retMask;
			}
		break; 
		case SDL_MOUSEBUTTONUP : 
			if ( (TCOD_EVENT_MOUSE_RELEASE & eventMask) != 0) {
				SDL_MouseButtonEvent *mev=&ev->button;
				retMask|=TCOD_EVENT_MOUSE_RELEASE;
				switch (mev->button) {
					case SDL_BUTTON_LEFT : if (mousebl) mouse->lbutton_pressed = mouse_force_bl=true; mouse->lbutton = mousebl=false; break;
					case SDL_BUTTON_MIDDLE : if (mousebm) mouse->mbutton_pressed = mouse_force_bm=true; mouse->mbutton = mousebm=false; break;
					case SDL_BUTTON_RIGHT : if (mousebr) mouse->rbutton_pressed = mouse_force_br=true; mouse->rbutton = mousebr=false; break;
				}
				/* update mouse position */
				if ( (TCOD_EVENT_MOUSE_MOVE & eventMask) == 0) {
					mouse->x=mev->x;
					mouse->y=mev->y;
					mouse->cx = (mouse->x - TCOD_ctx.fullscreen_offsetx) / TCOD_ctx.font_width;
					mouse->cy = (mouse->y - TCOD_ctx.fullscreen_offsety) / TCOD_ctx.font_height;
				}				
				return retMask;
			}
		break;
		case SDL_QUIT :
			TCOD_ctx.is_window_closed=true;
		break;
		case SDL_WINDOWEVENT :
			/* At this point, there are some corner cases that need dealing with.  So log this. */
			/* printf("SDL2 WINDOWEVENT: 0x%04x\n", ev->window.event); */
			switch (ev->window.event) {
#ifdef TCOD_ANDROID
			case SDL_WINDOWEVENT_SIZE_CHANGED:
			{
				/* printf("SDL2 WINDOWEVENT (SDL_WINDOWEVENT_SIZE_CHANGED): 0x%04x w=%d h=%d\n", ev->window.event, ev->window.data1, ev->window.data2); */
				/* If the app is started while the device is locked, the screen will be in portrait mode.  We need to rescale when it changes. */
				if (scale_data.surface_width != ev->window.data1 || scale_data.surface_height != ev->window.data1)
					scale_data.force_recalc = 1;
				break;
			}
#endif
			case SDL_WINDOWEVENT_ENTER:          /**< Window has gained mouse focus */
				TCOD_ctx.app_has_mouse_focus=true; break;
			case SDL_WINDOWEVENT_LEAVE:          /**< Window has lost mouse focus */
				TCOD_ctx.app_has_mouse_focus=false; break;
			case SDL_WINDOWEVENT_MAXIMIZED:      /**< Window has been maximized */
				TCOD_ctx.app_is_active=true; break;
			case SDL_WINDOWEVENT_MINIMIZED:      /**< Window has been minimized */
				TCOD_ctx.app_is_active=false; break;
			case SDL_WINDOWEVENT_EXPOSED:        /**< Window has been returned to and needs a refresh. */
				TCOD_sys_render(NULL,TCOD_console_get_width(NULL),TCOD_console_get_height(NULL), renderState);
				break;
#ifdef NDEBUG_HMM
			default:
				TCOD_LOG(("SDL2 WINDOWEVENT (unknown): 0x%04x\n", ev->window.event));
				break;
#endif
			}
 		break;
		default : break; 
	}
	return retMask;
}

TCOD_event_t TCOD_sys_wait_for_event(int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse, bool flush) {
	SDL_Event ev;
	TCOD_event_t retMask=0;
	if ( eventMask == 0 ) return 0;
	if ( flush ) {
		while ( SDL_PollEvent(&ev) ) {
			TCOD_sys_SDLtoTCOD(&ev,0);
		}
	}
	tcod_mouse.lbutton_pressed =false;
	tcod_mouse.rbutton_pressed =false;
	tcod_mouse.mbutton_pressed =false;
	tcod_mouse.wheel_up=false;
	tcod_mouse.wheel_down=false;
	tcod_mouse.dx=0;
	tcod_mouse.dy=0;
	if ( key ) {
		key->vk=TCODK_NONE;
		key->c=0;
	}	
	do {
		SDL_WaitEvent(&ev);
		retMask=TCOD_sys_handle_event(&ev,eventMask,key,&tcod_mouse);
	} while ( ev.type != SDL_QUIT && (retMask & eventMask) == 0 );
	if (mouse) { *mouse=tcod_mouse; }
	return retMask;
}

TCOD_event_t TCOD_sys_check_for_event(int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse) {
	SDL_Event ev;
	TCOD_event_t retMask=0;
	if ( eventMask == 0 ) return 0;
	tcod_mouse.lbutton_pressed =false;
	tcod_mouse.rbutton_pressed =false;
	tcod_mouse.mbutton_pressed =false;
	tcod_mouse.wheel_up=false;
	tcod_mouse.wheel_down=false;
	tcod_mouse.dx=0;
	tcod_mouse.dy=0;
	if ( key ) {
		key->vk=TCODK_NONE;
		key->c=0;
	}
	while ( SDL_PollEvent(&ev) ) {
		retMask=TCOD_sys_handle_event(&ev,eventMask,key,&tcod_mouse);
		if ((retMask & TCOD_EVENT_KEY) != 0)
			/* only one key event per frame */ 
			break; 
	}
	if (mouse) { *mouse=tcod_mouse; }
	return retMask;
}

TCOD_mouse_t TCOD_mouse_get_status(void) {
	return tcod_mouse;
}


/* classic keyboard functions (based on generic events) */

TCOD_key_t TCOD_sys_check_for_keypress(int flags) {
	static TCOD_key_t noret={TCODK_NONE,0};

	TCOD_key_t key;
	TCOD_event_t ev = TCOD_sys_check_for_event(flags & TCOD_EVENT_KEY, &key, NULL);

	if ((ev & TCOD_EVENT_KEY) == 0) return noret;

	return key;
}

TCOD_key_t TCOD_sys_wait_for_keypress(bool flush) {
	static TCOD_key_t noret={TCODK_NONE,0};

	TCOD_key_t key;
	TCOD_event_t ev = TCOD_sys_wait_for_event(TCOD_EVENT_KEY_PRESS, &key, NULL, flush);

	if ((ev & TCOD_EVENT_KEY_PRESS) == 0) return noret;

	return key;
}


void TCOD_sys_sleep_milli(uint32 milliseconds) {
	SDL_Delay(milliseconds);
}

void TCOD_sys_term(void) {
	sdl->term();
	SDL_Quit();
	memset(&scale_data,0,sizeof(scale_data_t));
	has_startup=false;
}

void *TCOD_sys_load_image(const char *filename) {
	image_support_t *img=image_type;
	while ( img->extension != NULL && !img->check_type(filename) ) img++;
	if ( img->extension == NULL || img->read == NULL ) return NULL; /* unknown format */
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

uint32 TCOD_sys_elapsed_milli(void) {
	return (uint32)SDL_GetTicks();
}

float TCOD_sys_elapsed_seconds(void) {
	static float div=1.0f/1000.0f;
	return SDL_GetTicks()*div;
}

void TCOD_sys_force_fullscreen_resolution(int width, int height) {
	TCOD_ctx.fullscreen_width=width;
	TCOD_ctx.fullscreen_height=height;
}

void * TCOD_sys_create_bitmap(int width, int height, TCOD_color_t *buf) {
	int x,y;
	SDL_PixelFormat fmt;
	SDL_Surface *bitmap;
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
	bitmap=SDL_CreateRGBSurface(SDL_SWSURFACE,width,height,fmt.BitsPerPixel,fmt.Rmask,fmt.Gmask,fmt.Bmask,fmt.Amask);
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

void TCOD_sys_save_fps(void) {
	min_frame_length_backup=min_frame_length;
}

void TCOD_sys_restore_fps(void) {
	min_frame_length=min_frame_length_backup;
}

int TCOD_sys_get_fps(void) {
	return fps;
}

float TCOD_sys_get_last_frame_length(void) {
	return last_frame_length;
}

void TCOD_sys_get_char_size(int *w, int *h) {
  *w = TCOD_ctx.font_width;
  *h = TCOD_ctx.font_height;
}

void TCOD_sys_get_current_resolution(int *w, int *h) {
	/* be sure that SDL is initialized */
	TCOD_sys_startup();
	sdl->get_current_resolution(w,h);
}

/* image stuff */
bool TCOD_sys_check_magic_number(const char *filename, int size, uint8 *data) {
	uint8 tmp[128];
	int i;
	SDL_RWops *rwops =  SDL_RWFromFile(filename, "rb");
	if (! rwops) return false;
	if ( (i = rwops->read(rwops,tmp,size,1)) != 1 ) {
		rwops->close(rwops);
		return false;
	}
	rwops->close(rwops);
	for (i=0; i< size; i++) if (tmp[i]!=data[i]) return false;
	return true;
}

void *TCOD_sys_get_SDL_window(void) {
	return (void *)window;
}

void *TCOD_sys_get_SDL_renderer(void) {
	return (void *)renderer;
}

/* mouse stuff */
void TCOD_mouse_show_cursor(bool visible) {
  SDL_ShowCursor(visible ? 1 : 0);
}

bool TCOD_mouse_is_cursor_visible(void) {
  return SDL_ShowCursor(-1) ? true : false;
}

void TCOD_mouse_move(int x, int y) {
	sdl->set_mouse_position(x,y);
}

void TCOD_mouse_includes_touch(bool enable) {
#ifdef TCOD_TOUCH_INPUT
	mouse_touch = enable;
#endif
}

bool TCOD_sys_read_file(const char *filename, unsigned char **buf, size_t *size) {
	return sdl->file_read(filename,buf,size);
}

bool TCOD_sys_file_exists(const char * filename, ...) {
	char f[1024];
	va_list ap;
	va_start(ap,filename);
	vsprintf(f,filename,ap);
	va_end(ap);	
	return sdl->file_exists(f);
}

bool TCOD_sys_write_file(const char *filename, unsigned char *buf, uint32 size) {
	return sdl->file_write(filename,buf,size);
}
