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
#ifndef TCOD_BARE
#include "mouse.h"
#include "sys.h"

#include <array>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <SDL.h>
#include "console.h"
#include "libtcod_int.h"
#include "parser.h"
#include "color/canvas.h"
#include "engine/error.h"
#include "engine/globals.h"

static TCOD_SDL_driver_t *sdl=NULL;


/* library initialization function */
#ifdef TCOD_WINDOWS
#define NOMINMAX 1
#include <windows.h>
BOOL APIENTRY DllMain(HINSTANCE, DWORD reason, LPVOID) {
	switch (reason) {
		case DLL_PROCESS_ATTACH : sdl = SDL_implementation_factory(); break;
		default : break;
	}
	return TRUE;
}
#else
	void __attribute__ ((constructor)) DllMain(void) {
		sdl = SDL_implementation_factory();
	}
#endif

#if defined(__ANDROID__)
#define TCOD_TOUCH_INPUT
#define MAX_TOUCH_FINGERS 5

typedef struct {
  int nupdates; /* how many updates have happened since the first finger was pressed. */
  uint32_t ticks0; /* current number of ticks at start of touch event sequence. */
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
void TCOD_sys_write_bmp(SDL_Surface *surf, const char *filename);
bool TCOD_sys_check_png(const char *filename);
SDL_Surface *TCOD_sys_read_png(const char *filename);
void TCOD_sys_write_png(SDL_Surface *surf, const char *filename);

typedef struct {
	const char *extension;
	bool (*check_type)(const char *filename);
	SDL_Surface *(*read)(const char *filename);
	void (*write)(SDL_Surface *surf, const char *filename);
} image_support_t;

static constexpr image_support_t image_type[] = {
	{ "BMP", TCOD_sys_check_bmp, TCOD_sys_read_bmp, TCOD_sys_write_bmp },
	{ "PNG", TCOD_sys_check_png, TCOD_sys_read_png, TCOD_sys_write_png },
	{ NULL, NULL, NULL, NULL },
};

scale_data_t scale_data = {};
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
float scale_factor = 1.0f;
SDL_Surface* charmap = nullptr;
char *last_clipboard_text = nullptr;
static bool has_startup = false;
#define MAX_SCALE_FACTOR 5.0f

/* font transparent color */
static TCOD_color_t fontKeyCol={0,0,0};

static uint32_t sdl_key=0, rgb_mask=0, nrgb_mask=0;

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
/**
 *  Codec for TCOD_FONT_LAYOUT_TCOD.
 *
 *  Converts from EASCII code-point -> raw tile position.
 */
constexpr std::array<int, 256> tcod_codec_{
  0,  0,  0,  0,  0,  0,  0,  0,  0, 76, 77,  0,  0,  0,  0,  0, /* 0 to 15 */
 71, 70, 72,  0,  0,  0,  0,  0, 64, 65, 67, 66,  0, 73, 68, 69, /* 16 to 31 */
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, /* 32 to 47 */
 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, /* 48 to 63 */
 32, 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110, /* 64 to 79 */
111,112,113,114,115,116,117,118,119,120,121, 33, 34, 35, 36, 37, /* 80 to 95 */
 38,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142, /* 96 to 111 */
143,144,145,146,147,148,149,150,151,152,153, 39, 40, 41, 42,  0, /* 112 to 127 */
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 128 to 143 */
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 144 to 159 */
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 160 to 175 */
 43, 44, 45, 46, 49,  0,  0,  0,  0, 81, 78, 87, 88,  0,  0, 55, /* 176 to 191 */
 53, 50, 52, 51, 47, 48,  0,  0, 85, 86, 82, 84, 83, 79, 80,  0, /* 192 to 207 */
  0,  0,  0,  0,  0,  0,  0,  0,  0, 56, 54,  0,  0,  0,  0,  0, /* 208 to 223 */
 74, 75, 57, 58, 59, 60, 61, 62, 63,  0,  0,  0,  0,  0,  0,  0, /* 224 to 239 */
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 240 to 255 */
};
constexpr std::array<int, 256> cp437_codec_{
0x0000, 0x263A, 0x263B, 0x2665, 0x2666, 0x2663, 0x2660, 0x2022,
0x25D8, 0x25CB, 0x25D9, 0x2642, 0x2640, 0x266A, 0x266B, 0x263C,
0x25BA, 0x25C4, 0x2195, 0x203C, 0x00B6, 0x00A7, 0x25AC, 0x21A8,
0x2191, 0x2193, 0x2192, 0x2190, 0x221F, 0x2194, 0x25B2, 0x25BC,
0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7,
0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5,
0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9,
0x00FF, 0x00D6, 0x00DC, 0x00A2, 0x00A3, 0x00A5, 0x20A7, 0x0192,
0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA,
0x00BF, 0x2310, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB,
0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,
0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F,
0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,
0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B,
0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
0x03B1, 0x00DF, 0x0393, 0x03C0, 0x03A3, 0x03C3, 0x00B5, 0x03C4,
0x03A6, 0x0398, 0x03A9, 0x03B4, 0x221E, 0x03C6, 0x03B5, 0x2229,
0x2261, 0x00B1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00F7, 0x2248,
0x00B0, 0x2219, 0x00B7, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0,
};
static void alloc_ascii_tables(void) {
	if ( TCOD_ctx.ascii_to_tcod ) free(TCOD_ctx.ascii_to_tcod);
	if ( charcols ) {
		free(charcols);
		free(first_draw);
	}

	TCOD_ctx.ascii_to_tcod =
      static_cast<int*>(calloc(sizeof(int),TCOD_ctx.max_font_chars));
	charcols = static_cast<TCOD_color_t*>(calloc(sizeof(TCOD_color_t),
                                               TCOD_ctx.max_font_chars));
	first_draw =
      static_cast<bool*>(calloc(sizeof(bool), TCOD_ctx.max_font_chars));
}
/** Reallocate the TCOD_ctx.ascii_to_tcod array, usually to make it bigger.
 */
static int realloc_ascii_tables(int new_size) {
  int *new_table = static_cast<int*>(realloc(TCOD_ctx.ascii_to_tcod,
                                             sizeof(int) * new_size));
  int i;
  if (!new_table) {
    return -1; /* failed to realloc table (old table pointer is still good) */
  }
  /* any new array indexes are undefined and need to be filled with zeros */
  for (i = TCOD_ctx.max_font_chars; i < new_size; ++i) {
    new_table[i] = 0;
  }
  TCOD_ctx.ascii_to_tcod = new_table;
  TCOD_ctx.max_font_chars = new_size;
  return 0;
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
/** See TCOD_console_map_ascii_code_to_font */
void TCOD_sys_map_ascii_to_font(int asciiCode, int fontCharX, int fontCharY) {
  if (asciiCode <= 0) { return; } /* can't reassign 0 or negatives */
  // Assign to new-style Tileset.
  auto tileset = tcod::engine::get_tileset();
  auto tilesheet = tcod::engine::get_tilesheet();
  if (tileset && tilesheet) {
    try {
      int tile_id = fontCharX + fontCharY * tilesheet->get_columns();
      tileset->set_tile(asciiCode, tilesheet->get_tile(tile_id));
    } catch (const std::runtime_error&) { // Ignore errors and continue.
    } catch (const std::logic_error&) {
    }
  }
  // Assign to legacy character table.
  if (asciiCode >= TCOD_ctx.max_font_chars) {
    /* reduce total allocations by resizing in increments of 256 */
    if (realloc_ascii_tables((asciiCode | 0xff) + 1)) {
      return; /* Failed to realloc table (old table pointer is still good) */
    }
  }
  TCOD_ctx.ascii_to_tcod[asciiCode] =
      fontCharX + fontCharY * TCOD_ctx.fontNbCharHoriz;
}
/**
 *  Set a code-point to point to an already existing code-point.
 */
static void TCOD_sys_map_clone_(int new_codepoint, int old_codepoint) {
  if (old_codepoint < 0) { return; }
  if (old_codepoint >= TCOD_ctx.max_font_chars) { return; }
  TCOD_sys_map_ascii_to_font(
      new_codepoint, TCOD_ctx.ascii_to_tcod[old_codepoint], 0);
}

static int TCOD_sys_load_font(void) {
	int i;
	bool hasTransparent=false;
	int x,y;

  if (charmap) { SDL_FreeSurface(charmap); }
  charmap = TCOD_sys_load_image(TCOD_ctx.font_file);
  if (charmap == NULL) {
    return tcod::set_errorf("SDL : cannot load %s", TCOD_ctx.font_file);
  }
	if ((static_cast<float>(charmap->w / TCOD_ctx.fontNbCharHoriz)
       != charmap->w / TCOD_ctx.fontNbCharHoriz)
      || (static_cast<float>(charmap->h / TCOD_ctx.fontNbCharVertic)
          != charmap->h / TCOD_ctx.fontNbCharVertic)) {
    return tcod::set_errorf(
        " %s size is not a multiple of font layout (%dx%d)\n",
        TCOD_ctx.font_file,
        TCOD_ctx.fontNbCharHoriz,
        TCOD_ctx.fontNbCharVertic);
  }
	TCOD_ctx.font_width=charmap->w/TCOD_ctx.fontNbCharHoriz;
	TCOD_ctx.font_height=charmap->h/TCOD_ctx.fontNbCharVertic;
	/* allocated bool array for colored flags */
	if ( TCOD_ctx.colored ) free(TCOD_ctx.colored);
	TCOD_ctx.colored = static_cast<bool*>(
      calloc(sizeof(bool),
             TCOD_ctx.fontNbCharHoriz * TCOD_ctx.fontNbCharVertic));
	check_ascii_to_tcod();
	/* figure out what kind of font we have */
	/* check if the alpha layer is actually used */
	if ( charmap->format->BytesPerPixel == 4 ) {
		TCOD_LOG(("32bits font... checking for alpha layer... "));
		for (x=0; !hasTransparent && x < charmap->w; x ++ ) {
			for (y=0;!hasTransparent && y < charmap->h; y++ ) {
				uint8_t* pixel = static_cast<uint8_t*>(charmap->pixels)
            + y * charmap->pitch + x * charmap->format->BytesPerPixel;
				uint8_t alpha=*((pixel)+charmap->format->Ashift/8);
				if ( alpha < 255 ) {
					hasTransparent=true;
				}
			}
		}
		TCOD_LOG((hasTransparent ? "present\n" : "not present\n"));
	} else if ( charmap->format->BytesPerPixel != 3 ) {
		/* convert to 24 bits */
		SDL_Surface *temp;
		TCOD_LOG(("font bpp < 24. converting to 24bits\n"));
		temp = TCOD_sys_get_surface(charmap->w, charmap->h, false);
		SDL_BlitSurface(charmap,NULL,temp,NULL);
		SDL_FreeSurface(charmap);
		charmap=temp;
	} else {
		TCOD_LOG(("24 bits font.\n"));
	}
	if (! hasTransparent ) {
		/* alpha layer not used */
		int keyx,keyy;
        uint8_t*pixel;
		/* the key color is found on the character corresponding to space ' ' */
		if ( TCOD_ctx.font_tcod_layout ) {
			keyx = TCOD_ctx.font_width/2;
			keyy = TCOD_ctx.font_height/2;
		} else if (TCOD_ctx.font_in_row) {
			keyx = (static_cast<int>(' ') % TCOD_ctx.fontNbCharHoriz)
          * TCOD_ctx.font_width + TCOD_ctx.font_width / 2;
			keyy = (static_cast<int>(' ') / TCOD_ctx.fontNbCharHoriz)
          * TCOD_ctx.font_height + TCOD_ctx.font_height / 2;
		} else {
			keyx = (static_cast<int>(' ') / TCOD_ctx.fontNbCharVertic)
          * TCOD_ctx.font_width + TCOD_ctx.font_width / 2;
			keyy = (static_cast<int>(' ') % TCOD_ctx.fontNbCharVertic)
          * TCOD_ctx.font_height + TCOD_ctx.font_height / 2;
		}
		pixel = static_cast<uint8_t*>(charmap->pixels)
        + keyy * charmap->pitch + keyx * charmap->format->BytesPerPixel;
		fontKeyCol.r=*((pixel)+charmap->format->Rshift/8);
		fontKeyCol.g=*((pixel)+charmap->format->Gshift/8);
		fontKeyCol.b=*((pixel)+charmap->format->Bshift/8);
		TCOD_LOG(("key color : %d %d %d\n",fontKeyCol.r,fontKeyCol.g,fontKeyCol.b));
		if ( ! TCOD_ctx.font_greyscale && charmap->format->BytesPerPixel == 4 ) {
			/* 32 bits font but alpha layer not used. convert to 24 bits (faster) */
			SDL_Surface *temp;
			TCOD_LOG(("32bits font with no alpha => converting to faster 24 bits\n"));
			temp = TCOD_sys_get_surface(charmap->w, charmap->h, false);
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
					uint8_t*pixel=static_cast<uint8_t*>(charmap->pixels)
              + (cy * TCOD_ctx.font_height + py) * charmap->pitch
						  + (cx * TCOD_ctx.font_width + px)
                * charmap->format->BytesPerPixel;
					uint8_t r=*((pixel)+charmap->format->Rshift/8);
					uint8_t g=*((pixel)+charmap->format->Gshift/8);
					uint8_t b=*((pixel)+charmap->format->Bshift/8);
					if ( charmap->format->BytesPerPixel == 3 ) {
						/* ignore key color */
						if ( r == fontKeyCol.r  && g == fontKeyCol.g && b == fontKeyCol.b ) continue;
					}
					/* colored tile if a pixel is not desaturated */
					if ( r != g || g !=b || b != r ) {
						TCOD_ctx.colored[i]=true;
						TCOD_LOG(("character for ascii code %d is colored\n",i));
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
			TCOD_LOG(("24bits greyscale font. converting to 32bits\n"));
			temp = TCOD_sys_get_surface(charmap->w, charmap->h, true);
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
						uint8_t* pixel = static_cast<uint8_t*>(charmap->pixels)
                + y * charmap->pitch + x * charmap->format->BytesPerPixel;
						uint8_t r=*((pixel)+charmap->format->Rshift/8);
						*((pixel)+charmap->format->Ashift/8) = (invert ? 255-r : r);
						*((pixel)+charmap->format->Rshift/8)=255;
						*((pixel)+charmap->format->Gshift/8)=255;
						*((pixel)+charmap->format->Bshift/8)=255;
					} else {
						uint8_t* pixel = static_cast<uint8_t*>(charmap->pixels)
                + y * charmap->pitch + x * charmap->format->BytesPerPixel;
						uint8_t r=*((pixel)+charmap->format->Rshift/8);
						uint8_t g=*((pixel)+charmap->format->Gshift/8);
						uint8_t b=*((pixel)+charmap->format->Bshift/8);
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
  TCOD_sys_decode_font_();
  return 0;
}
/**
 *  Decode the font layout depending on the current flags.
 */
void TCOD_sys_decode_font_(void)
{
  if (TCOD_ctx.font_flags & TCOD_FONT_LAYOUT_CP437) {
    for (int i = 0; i < static_cast<int>(cp437_codec_.size()); ++i) {
      TCOD_sys_map_ascii_to_font(cp437_codec_.at(i), i, 0);
    }
  } else if (TCOD_ctx.font_tcod_layout) {
    for (int i = 0; i < static_cast<int>(tcod_codec_.size()); ++i) {
      TCOD_sys_map_ascii_to_font(i, tcod_codec_.at(i), 0);
    }
    TCOD_sys_map_clone_(0x2500, TCOD_CHAR_HLINE);
    TCOD_sys_map_clone_(0x2502, TCOD_CHAR_VLINE);
    TCOD_sys_map_clone_(0x250C, TCOD_CHAR_NW);
    TCOD_sys_map_clone_(0x2510, TCOD_CHAR_NE);
    TCOD_sys_map_clone_(0x2514, TCOD_CHAR_SW);
    TCOD_sys_map_clone_(0x2518, TCOD_CHAR_SE);
  } else {
    if (TCOD_ctx.font_in_row) {
      /* for font in row */
      for (int i = 0; i < TCOD_ctx.max_font_chars; ++i) {
        TCOD_sys_map_ascii_to_font(i, i, 0);
      }
    } else {
      /* for font in column */
      for (int i = 0; i < TCOD_ctx.max_font_chars; ++i) {
        int fy = i % TCOD_ctx.fontNbCharVertic;
        int fx = i / TCOD_ctx.fontNbCharVertic;
        TCOD_sys_map_ascii_to_font(i, fx, fy);
      }
    }
    TCOD_sys_map_clone_(0x2500, 0xC4);
    TCOD_sys_map_clone_(0x2502, 0xB3);
    TCOD_sys_map_clone_(0x250C, 0xDA);
    TCOD_sys_map_clone_(0x2510, 0xBF);
    TCOD_sys_map_clone_(0x2514, 0xC0);
    TCOD_sys_map_clone_(0x2518, 0xD9);
  }
}
/**
 *  This function used to handle more stuff, now it only initializes the
 *  legacy font system.
 *
 *  See TCOD_console_set_custom_font.
 */
void TCOD_sys_set_custom_font(const char*, int, int, int) {
  check_ascii_to_tcod();
}

void find_resolution(void) {
	TCOD_ctx.actual_fullscreen_width=TCOD_ctx.fullscreen_width>TCOD_ctx.root->w*TCOD_ctx.font_width?TCOD_ctx.fullscreen_width:TCOD_ctx.root->w*TCOD_ctx.font_width;
	TCOD_ctx.actual_fullscreen_height=TCOD_ctx.fullscreen_height>TCOD_ctx.root->h*TCOD_ctx.font_height?TCOD_ctx.fullscreen_height:TCOD_ctx.root->h*TCOD_ctx.font_height;
	sdl->get_closest_mode(&TCOD_ctx.actual_fullscreen_width,&TCOD_ctx.actual_fullscreen_height);
}

SDL_Surface* TCOD_sys_create_bitmap_for_console(TCOD_Console* console) {
  int w = TCOD_console_get_width(console) * TCOD_ctx.font_width;
  int h = TCOD_console_get_height(console) * TCOD_ctx.font_height;
  return TCOD_sys_get_surface(w, h, false);
}

static void TCOD_sys_render(void *vbitmap, struct TCOD_Console* console) {
  if (!tcod::engine::get_display()) {
    sdl->render(sdl, vbitmap, console);
  }
}

static TCOD_color_t blend_fade_color_(
    const TCOD_color_t& color,
    const TCOD_color_t& fade_color,
    uint8_t fade_value)
{
  if (fade_value != 255) { return color; }
  return {
      static_cast<uint8_t>(
          static_cast<int>(color.r) * fade_value / 255
          + static_cast<int>(fade_color.r) * (255 - fade_value) / 255),
      static_cast<uint8_t>(
          static_cast<int>(color.g) * fade_value / 255
          + static_cast<int>(fade_color.g) * (255 - fade_value) / 255),
      static_cast<uint8_t>(
          static_cast<int>(color.b) * fade_value / 255
          + static_cast<int>(fade_color.b) * (255 - fade_value) / 255),
  };
}

void TCOD_sys_console_to_bitmap(
    SDL_Surface* vbitmap,
    struct TCOD_Console* console,
    struct TCOD_Console* cache)
{
  static SDL_Surface* charmap_backup = NULL;
  SDL_Surface *bitmap = vbitmap;
  uint32_t sdl_back = 0;
  uint32_t sdl_fore = 0;
  TCOD_color_t fading_color = TCOD_console_get_fading_color();
  int fade = TCOD_console_get_fade();
  /* can only track changes on the root console */
  bool track_changes = (cache && oldFade == fade);
  uint8_t bpp = charmap->format->BytesPerPixel;
  int hdelta;
  if (bpp == 4) {
    hdelta = (charmap->pitch - TCOD_ctx.font_width * bpp) / 4;
  } else {
    hdelta = (charmap->pitch - TCOD_ctx.font_width * bpp);
  }
  if (charmap_backup == NULL) {
    charmap_backup = TCOD_sys_get_surface(charmap->w, charmap->h, true);
    SDL_BlitSurface(charmap, NULL, charmap_backup, NULL);
  }
  for (int y = 0; y < console->h; ++y) {
    for (int x = 0; x < console->w; ++x) {
      using tcod::console::Tile;
      int console_i = y * console->w + x;
      const Tile& current = console->tiles[console_i];
      const int& c = current.ch;
      const TCOD_ColorRGBA& nfg = current.fg;
      const TCOD_ColorRGBA& nbg = current.bg;
      SDL_Rect srcRect, dstRect;
      if (track_changes) {
        Tile& old = cache->tiles[console_i];
        if (current == old) {
          continue;
        }
        old = current;
      }
      TCOD_ColorRGB b = tcod::ColorRGB(nbg);
      dstRect.x = x * TCOD_ctx.font_width;
      dstRect.y = y * TCOD_ctx.font_height;
      dstRect.w = TCOD_ctx.font_width;
      dstRect.h = TCOD_ctx.font_height;
      /* draw background */
      b = blend_fade_color_(b, fading_color, fade);
      sdl_back = SDL_MapRGB(bitmap->format, b.r, b.g, b.b);
      SDL_FillRect(bitmap, &dstRect, sdl_back);
      if (c != ' ') {
        /* draw foreground */
        int ascii = TCOD_get_tileid_for_charcode_(c);
        TCOD_ColorRGB *curtext = &charcols[ascii];
        bool first = first_draw[ascii];
        TCOD_ColorRGB f = tcod::ColorRGB(nfg);
        f = blend_fade_color_(f, fading_color, fade);
        /* only draw character if foreground color != background color */
        if (f != b) {
          if (charmap->format->Amask == 0 && f == fontKeyCol) {
            /* cannot draw with the key color... */
            if (f.r < 255) { f.r++; } else { f.r--; }
          }
          srcRect.x = (ascii % TCOD_ctx.fontNbCharHoriz)
                      * TCOD_ctx.font_width;
          srcRect.y = (ascii / TCOD_ctx.fontNbCharHoriz)
                      * TCOD_ctx.font_height;
          srcRect.w = TCOD_ctx.font_width;
          srcRect.h = TCOD_ctx.font_height;

          if (charmap && (first || *curtext != f)) {
            /* change the character color in the font */
            first_draw[ascii] = false;
            sdl_fore = SDL_MapRGB(charmap->format, f.r, f.g, f.b) & rgb_mask;
            *curtext = f;
            if (bpp == 4) {
              /* 32 bits font : fill the whole character with foreground color */
              uint32_t *pix = reinterpret_cast<uint32_t *>(
                  reinterpret_cast<uint8_t*>(charmap->pixels) + srcRect.x * bpp
                  + srcRect.y * charmap->pitch);
              int h = TCOD_ctx.font_height;
              if (!TCOD_ctx.colored[ascii]) {
                while (h--) {
                  int w = TCOD_ctx.font_width;
                  while (w--) {
                    (*pix) &= nrgb_mask;
                    (*pix) |= sdl_fore;
                    ++pix;
                  }
                  pix += hdelta;
                }
              } else {
                /* colored character : multiply color with foreground color */
                uint32_t *pixorig = reinterpret_cast<uint32_t *>(
                    reinterpret_cast<uint8_t*>(charmap_backup->pixels)
                    + srcRect.x * bpp + srcRect.y * charmap_backup->pitch);
                int hdelta_backup = (charmap_backup->pitch
                                      - TCOD_ctx.font_width * 4) / 4;
                while (h > 0) {
                  int w = TCOD_ctx.font_width;
                  while (w > 0) {
                    int r_ = reinterpret_cast<uint8_t*>(pixorig)[
                        charmap_backup->format->Rshift / 8];
                    int g_ = reinterpret_cast<uint8_t*>(pixorig)[
                        charmap_backup->format->Gshift / 8];
                    int b_ = reinterpret_cast<uint8_t*>(pixorig)[
                        charmap_backup->format->Bshift / 8];
                    (*pix) &= nrgb_mask; /* erase the color */
                    r_ = r_ * f.r / 255;
                    g_ = g_ * f.g / 255;
                    b_ = b_ * f.b / 255;
                    /* set the new color */
                    (*pix) |= (r_ << charmap->format->Rshift)
                              | (g_ <<charmap->format->Gshift)
                              | (b_ <<charmap->format->Bshift);
                    w--;
                    pix++;
                    pixorig++;
                  }
                  h--;
                  pix += hdelta;
                  pixorig += hdelta_backup;
                }
              }
            } else {
              /* 24 bits font : fill only non key color pixels */
              uint32_t *pix = reinterpret_cast<uint32_t*>(
                  reinterpret_cast<uint8_t*>(charmap->pixels)
                  + srcRect.x*bpp + srcRect.y * charmap->pitch);
              int h = TCOD_ctx.font_height;
              if (!TCOD_ctx.colored[ascii]) {
                while (h--) {
                  int w = TCOD_ctx.font_width;
                  while (w--) {
                    if (((*pix) & rgb_mask) != sdl_key ) {
                      (*pix) &= nrgb_mask;
                      (*pix) |= sdl_fore;
                    }
                    pix = reinterpret_cast<uint32_t*>(
                        reinterpret_cast<uint8_t*>(pix) + 3);
                  }
                  pix = reinterpret_cast<uint32_t*>(
                      reinterpret_cast<uint8_t*>(pix) + hdelta);
                }
              } else {
                /* colored character : multiply color with foreground color */
                uint32_t *pixorig = reinterpret_cast<uint32_t*>(
                    reinterpret_cast<uint8_t*>(charmap_backup->pixels)
                    + srcRect.x * 4 + srcRect.y * charmap_backup->pitch);
                /* charmap_backup is always 32 bits */
                int hdelta_backup = (charmap_backup->pitch
                                      - TCOD_ctx.font_width * 4) / 4;
                while (h > 0) {
                  int w = TCOD_ctx.font_width;
                  while (w > 0) {
                    if (((*pixorig) & rgb_mask) != sdl_key) {
                      int r_ = reinterpret_cast<uint8_t*>(pixorig)[
                          charmap_backup->format->Rshift / 8];
                      int g_ = reinterpret_cast<uint8_t*>(pixorig)[
                          charmap_backup->format->Gshift / 8];
                      int b_ = reinterpret_cast<uint8_t*>(pixorig)[
                          charmap_backup->format->Bshift / 8];
                      (*pix) &= nrgb_mask; /* erase the color */
                      r_ = r_ * f.r / 255;
                      g_ = g_ * f.g / 255;
                      b_ = b_ * f.b / 255;
                      /* set the new color */
                      (*pix) |= (r_ << charmap->format->Rshift)
                                | (g_ << charmap->format->Gshift)
                                | (b_ << charmap->format->Bshift);
                    }
                    w--;
                    pix = reinterpret_cast<uint32_t*>(
                        reinterpret_cast<uint8_t*>(pix) + 3);
                    pixorig++;
                  }
                  h--;
                  pix = reinterpret_cast<uint32_t*>(
                      reinterpret_cast<uint8_t*>(pix) + hdelta);
                  pixorig += hdelta_backup;
                }
              }
            }
          }
          SDL_BlitSurface(charmap, &srcRect, bitmap, &dstRect);
        }
      }
    }
  }
#ifdef USE_SDL_LOCKS
  if (SDL_MUSTLOCK(bitmap)) { SDL_UnlockSurface(bitmap); }
#endif
}

SDL_Surface *TCOD_sys_get_surface(int width, int height, bool alpha) {
	return sdl->create_surface(width,height,alpha);
}

void TCOD_sys_update_char(int asciiCode, int fontx, int fonty,
                          TCOD_Image* img, int x, int y)
{
  static const TCOD_color_t pink = {255, 0, 255};
  TCOD_sys_map_ascii_to_font(asciiCode, fontx, fonty);
  int iw, ih;
  TCOD_image_get_size(img, &iw, &ih);
  for (int px = 0; px < TCOD_ctx.font_width; ++px) {
    for (int py = 0; py < TCOD_ctx.font_height; ++py) {
      TCOD_color_t col = TCOD_image_get_pixel(img, x + px, y + py);
      uint8_t* pixel = static_cast<uint8_t*>(charmap->pixels)
                       + (fonty * TCOD_ctx.font_height + py)
                       * charmap->pitch
                       + (fontx * TCOD_ctx.font_width + px)
                       * charmap->format->BytesPerPixel;
      uint8_t bpp = charmap->format->BytesPerPixel;
      if (bpp == 4) {
        *(pixel + charmap->format->Ashift / 8) = col.r;
        *(pixel + charmap->format->Rshift / 8) = 255;
        *(pixel + charmap->format->Gshift / 8) = 255;
        *(pixel + charmap->format->Bshift / 8) = 255;
      } else {
        *(pixel + charmap->format->Rshift / 8) = col.r;
        *(pixel + charmap->format->Gshift / 8) = col.g;
        *(pixel + charmap->format->Bshift / 8) = col.b;
      }
    }
  }
  charcols[asciiCode] = pink;
  TCOD_sys_set_dirty_character_code(asciiCode);
}
/**
 *  Older startup function, mostly called from TCOD_sys_init.
 */
void TCOD_sys_startup(void) {
	if (has_startup) return;
#ifndef NDEBUG
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
#endif
	TCOD_IFNOT(SDL_Init(SDL_INIT_VIDEO) >= 0 ) return;
	memset(&TCOD_ctx.key_state,0,sizeof(TCOD_key_t));
	TCOD_ctx.max_font_chars=256;
	alloc_ascii_tables();
#ifndef NO_OPENGL
	TCOD_opengl_init_attributes();
#endif

	has_startup=true;
}
/**
 *  All shutdown routines call this function.
 *
 *  Mostly used internally. TCOD_quit should be called to shutdown the library.
 */
void TCOD_sys_shutdown(void)
{
  if (TCOD_ctx.root) {
    TCOD_console_delete(TCOD_ctx.root);
  }
  if (tcod::engine::get_display()){
    tcod::engine::set_display(nullptr);
    return;
  }
  if (has_startup) {
    sdl->destroy_window();
    sdl->shutdown();
    memset(&scale_data, 0, sizeof(scale_data_t));
    has_startup = false;
  }
  SDL_Quit();
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

int TCOD_sys_set_renderer(TCOD_renderer_t renderer) {
  if (renderer == TCOD_ctx.renderer) { return 0; }
  return TCOD_console_init_root(
      TCOD_ctx.root->w,
      TCOD_ctx.root->h,
      TCOD_ctx.window_title,
      TCOD_console_is_fullscreen(),
      renderer
  );
}

void TCOD_sys_init_screen_offset(void) {
	TCOD_ctx.fullscreen_offsetx=(TCOD_ctx.actual_fullscreen_width-TCOD_ctx.root->w*TCOD_ctx.font_width)/2;
	TCOD_ctx.fullscreen_offsety=(TCOD_ctx.actual_fullscreen_height-TCOD_ctx.root->h*TCOD_ctx.font_height)/2;
}

bool TCOD_sys_init(struct TCOD_Console *console, bool fullscreen) {
	static TCOD_renderer_t last_renderer=TCOD_RENDERER_SDL;
	static char last_font[512]="";
	TCOD_sys_startup();
	/* check if there is a user (player) config file */
	if ( TCOD_sys_file_exists("./libtcod.cfg")) {
		/* yes, read it */
		TCOD_sys_load_player_config();
		if (TCOD_ctx.fullscreen) fullscreen=true;
	}
	if (last_renderer != TCOD_ctx.renderer || ! charmap || strcmp(last_font,TCOD_ctx.font_file) != 0) {
		/* reload the font when switching renderer to restore original character colors */
		if (TCOD_sys_load_font() < 0) { return false; }
	}
	sdl->create_window(console->w, console->h, fullscreen);
	memset(key_status,0,sizeof(bool)*(TCODK_CHAR+1));

	return true;
}
TCOD_DEPRECATED_NOMESSAGE
static char* TCOD_strcasestr(const char *haystack, const char *needle)
{
  const char *startn = NULL, *np = NULL;
  for (const char* p = haystack; *p; p++) {
    if (np) {
      if (toupper(*p) == toupper(*np)) {
        if (!*++np) { return const_cast<char*>(startn); }
      } else {
        np = 0;
      }
    } else if (toupper(*p) == toupper(*needle)) {
      np = needle + 1;
      startn = p;
    }
  }
  return 0;
}

void TCOD_sys_save_bitmap(SDL_Surface* bitmap, const char *filename) {
  const image_support_t *img = image_type;
  while (img->extension != NULL
         && TCOD_strcasestr(filename,img->extension) == NULL ) {
    ++img;
  }
  if (img->extension == NULL || img->write == NULL) {
    img = image_type; // default to bmp
  }
  img->write(bitmap, filename);
}

void TCOD_sys_save_screenshot(const char *filename) {
  char buf[128];
  int idx = 0;
  while (!filename) {
    /* generate filename */
    FILE *access_file = NULL;
    sprintf(buf, "./screenshot%03d.png", idx);
    access_file = fopen(buf, "rb");
    if (!access_file) {
      filename = buf;
    } else {
      idx++;
      fclose(access_file);
    }
  }
  auto display = tcod::engine::get_display();
  if (display) {
    tcod::image::save(display->read_pixels(), filename);
  } else {
    sdl->save_screenshot(filename);
  }
}

void TCOD_sys_set_fullscreen(bool fullscreen) {
	TCOD_ctx.fullscreen=fullscreen;
	sdl->set_fullscreen(fullscreen);
}

void TCOD_sys_set_scale_factor(float value)
{
  scale_factor = value;
  if (scale_factor + 1e-3f < scale_data.min_scale_factor) {
    scale_factor = scale_data.min_scale_factor;
  } else if (scale_factor - 1e-3f > MAX_SCALE_FACTOR) {
    scale_factor = MAX_SCALE_FACTOR;
  }
}

void TCOD_sys_set_window_title(const char *title) {
  strncpy(TCOD_ctx.window_title, title, sizeof(TCOD_ctx.window_title) - 1);
  TCOD_ctx.window_title[sizeof(TCOD_ctx.window_title) - 1] = '\0';
  sdl->set_window_title(title);
}
/**
 *  Keep track of time and wait if the frame-rate is faster than the set FPS.
 */
static void sync_time(void) {
  static uint32_t old_time = 0;
  static uint32_t new_time = 0;
  static uint32_t elapsed = 0;
  int32_t frame_time;
  int32_t time_to_wait;

  old_time = new_time;
  new_time = TCOD_sys_elapsed_milli();
  /* If TCOD has been terminated and restarted. */
  if (old_time > new_time)
    old_time = elapsed = 0;
  if ( new_time / 1000 != elapsed ) {
    /* update fps every second */
    fps = cur_fps;
    cur_fps = 0;
    elapsed = new_time / 1000;
  }
  /* if too fast, wait */
  frame_time = (new_time - old_time);
  last_frame_length = frame_time * 0.001f;
  cur_fps++;
  time_to_wait = min_frame_length - frame_time;
  if (old_time > 0 && time_to_wait > 0) {
    TCOD_sys_sleep_milli(time_to_wait);
    new_time = TCOD_sys_elapsed_milli();
    frame_time=(new_time - old_time);
  }
  last_frame_length = frame_time * 0.001f;
}
/**
 *  Flush the screen, if `render` is true then the root console will be
 *  presented (this is mostly ignored.)
 */
void TCOD_sys_flush(bool render) {
  if (!TCOD_ctx.root) { return; }
  std::shared_ptr<tcod::engine::Display> display = tcod::engine::get_display();
  if (display) {
    display->present(TCOD_ctx.root);
  } else {
    if (render) { TCOD_sys_render(NULL, TCOD_ctx.root); }
  }
  sync_time();
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

static void TCOD_sys_convert_event(const SDL_Event *ev, TCOD_key_t *ret) {
	const SDL_KeyboardEvent *kev=&ev->key;
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
/**
 *  Parse an SDL key-up or key-down event and return the global key state.
 */
static TCOD_key_t TCOD_sys_SDLtoTCOD(const SDL_Event *ev, int flags) {
  TCOD_key_t *ret = &TCOD_ctx.key_state;
  ret->vk = TCODK_NONE;
  ret->c = 0;
  ret->pressed = 0;
  switch (ev->type) {
    case SDL_KEYUP: {
      TCOD_key_t tmpkey;
      switch(ev->key.keysym.sym) {
        case SDLK_LALT: ret->lalt = 0; break;
        case SDLK_RALT: ret->ralt = 0; break;
        case SDLK_LCTRL: ret->lctrl = 0; break;
        case SDLK_RCTRL: ret->rctrl = 0; break;
        case SDLK_LSHIFT: ret->shift = 0; break;
        case SDLK_RSHIFT: ret->shift = 0; break;
        case SDLK_LGUI: ret->lmeta = 0; break;
        case SDLK_RGUI: ret->rmeta = 0; break;
        default: break;
      }
      TCOD_sys_convert_event(ev, &tmpkey);
      key_status[tmpkey.vk] = false;
      if (flags & TCOD_KEY_RELEASED) {
        ret->vk = tmpkey.vk;
        ret->c = tmpkey.c;
        ret->pressed = 0;
      }
      break;
    }
    case SDL_KEYDOWN: {
      TCOD_key_t tmpkey;
      switch(ev->key.keysym.sym) {
        case SDLK_LALT: ret->lalt = 1; break;
        case SDLK_RALT: ret->ralt = 1; break;
        case SDLK_LCTRL: ret->lctrl = 1; break;
        case SDLK_RCTRL: ret->rctrl = 1; break;
        case SDLK_LSHIFT: ret->shift = 1; break;
        case SDLK_RSHIFT: ret->shift = 1; break;
        case SDLK_LGUI: ret->lmeta = 1; break;
        case SDLK_RGUI: ret->rmeta = 1; break;
        default : break;
      }
      TCOD_sys_convert_event(ev, &tmpkey);
      key_status[tmpkey.vk] = true;
      if (flags & TCOD_KEY_PRESSED) {
        ret->vk = tmpkey.vk;
        ret->c = tmpkey.c;
        ret->pressed = 1;
      }
    break;
    }
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
/** The global libtcod mouse state. */
static TCOD_mouse_t tcod_mouse={0,0,0,0,0,0,0,0,false,false,false,false,false,false,false,false};
/**
 *  Convert a pixel coordinate to a tile coordinate using global state.
 */
void TCOD_sys_pixel_to_tile(double* x, double* y)
{
  if (!x || !y) { return; }
  auto display = tcod::engine::get_display();
  if (display) {
    std::array<double, 2> xy = display->pixel_to_tile({*x, *y});
    *x = std::get<0>(xy);
    *y = std::get<1>(xy);
  } else {
    *x = (*x - TCOD_ctx.fullscreen_offsetx) / TCOD_ctx.font_width;
    *y = (*y - TCOD_ctx.fullscreen_offsety) / TCOD_ctx.font_height;
  }
}
/**
 *  Parse the coordinates and motion of an SDL event into a libtcod mouse
 *  struct.
 */
static void sdl_parse_mouse_(const SDL_Event& ev, TCOD_mouse_t& mouse)
{
  switch(ev.type) {
    case SDL_MOUSEMOTION:
      mouse.x = ev.motion.x;
      mouse.y = ev.motion.y;
      mouse.dx = ev.motion.xrel;
      mouse.dy = ev.motion.yrel;
      break;
    case SDL_MOUSEWHEEL:
      // Leave x,y attributes as is to preserve the original libtcod behaviour.
      mouse.wheel_up = ev.wheel.y > 0;
      mouse.wheel_down = ev.wheel.y < 0;
      mouse.dx = mouse.dy = 0;
      break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      mouse.x = ev.button.x;
      mouse.y = ev.button.y;
      mouse.dx = mouse.dy = 0;
      break;
    default: return;
  }
  auto display = tcod::engine::get_display();
  if (display) {
    auto to_tile = [&](double x, double y) -> std::array<int, 2> {
      std::array<double, 2> result(display->pixel_to_tile({x, y}));
      return {static_cast<int>(result.at(0)), static_cast<int>(result.at(1))};
    };
    std::array<int, 2> cell_xy(to_tile(mouse.x, mouse.y));
    std::array<int, 2> prev_cell_xy(
        to_tile(mouse.x - mouse.dx, mouse.y - mouse.dy)
    );
    mouse.cx = cell_xy.at(0);
    mouse.cy = cell_xy.at(1);
    mouse.dcx = cell_xy.at(0) - prev_cell_xy.at(0);
    mouse.dcy = cell_xy.at(1) - prev_cell_xy.at(1);
  } else {
    mouse.x -= TCOD_ctx.fullscreen_offsetx;
    mouse.y -= TCOD_ctx.fullscreen_offsety;
    mouse.cx = mouse.x / TCOD_ctx.font_width;
    mouse.cy = mouse.y / TCOD_ctx.font_height;
    int prev_cx = (mouse.x - mouse.dx) / TCOD_ctx.font_width;
    int prev_cy = (mouse.y - mouse.dy) / TCOD_ctx.font_height;
    mouse.dcx = mouse.cx - prev_cx;
    mouse.dcy = mouse.cy - prev_cy;
  }
}
/**
 *  Parse an SDL_Event into `mouse` and return the relevant TCOD_event_t.
 *
 *  Returns 0 if the event wasn't mouse related.
 */
TCOD_event_t TCOD_sys_handle_mouse_event(
    const SDL_Event* ev, TCOD_mouse_t* mouse)
{
  if (!ev || !mouse) { return TCOD_EVENT_NONE; }
  sdl_parse_mouse_(*ev, *mouse);
  switch(ev->type) {
    case SDL_MOUSEMOTION:
      return TCOD_EVENT_MOUSE_MOVE;
    case SDL_MOUSEWHEEL:
      return TCOD_EVENT_MOUSE_PRESS;
    case SDL_MOUSEBUTTONDOWN:
      switch (ev->button.button) {
        case SDL_BUTTON_LEFT: mouse->lbutton = mousebl = true; break;
        case SDL_BUTTON_MIDDLE: mouse->mbutton = mousebm = true; break;
        case SDL_BUTTON_RIGHT: mouse->rbutton = mousebr = true; break;
        default: break;
      }
      return TCOD_EVENT_MOUSE_PRESS;
    case SDL_MOUSEBUTTONUP:
      switch (ev->button.button) {
        case SDL_BUTTON_LEFT:
        if (mousebl) { mouse->lbutton_pressed = mouse_force_bl = true; }
        mouse->lbutton = mousebl = false;
        break;
        case SDL_BUTTON_MIDDLE:
        if (mousebm) { mouse->mbutton_pressed = mouse_force_bm = true; }
        mouse->mbutton = mousebm = false;
        break;
        case SDL_BUTTON_RIGHT:
        if (mousebr) { mouse->rbutton_pressed = mouse_force_br = true; }
        mouse->rbutton = mousebr = false;
        break;
      }
      return TCOD_EVENT_MOUSE_RELEASE;
    default:
      return TCOD_EVENT_NONE;
  }
}
/**
 *  Parse an SDL_Event into `key` and return the relevant TCOD_event_t.
 *
 *  Returns 0 if the event wasn't keyboard related.
 */
TCOD_event_t TCOD_sys_handle_key_event(const SDL_Event* ev, TCOD_key_t* key)
{
  if (!ev || !key) { return TCOD_EVENT_NONE; }
  switch(ev->type) {
    case SDL_KEYDOWN:
      *key = TCOD_sys_SDLtoTCOD(ev, TCOD_KEY_PRESSED);
      return TCOD_EVENT_KEY_PRESS;
    case SDL_KEYUP:
      *key = TCOD_sys_SDLtoTCOD(ev, TCOD_KEY_RELEASED);
      return TCOD_EVENT_KEY_RELEASE;
    case SDL_TEXTINPUT: {
      *key = TCOD_ctx.key_state;
      key->vk = TCODK_TEXT;
      key->c = 0;
      key->pressed = 1;
      strncpy(key->text, ev->text.text, TCOD_KEY_TEXT_SIZE);
      return TCOD_EVENT_KEY_PRESS;
    }
    break;
    default:
      return TCOD_EVENT_NONE;
  }
}
static TCOD_event_t TCOD_sys_handle_event(
    SDL_Event *ev, TCOD_event_t eventMask,
    TCOD_key_t *key, TCOD_mouse_t *mouse)
{
	int retMask = TCOD_EVENT_NONE;
	/* printf("TCOD_sys_handle_event type=%04x\n", ev->type); */
	retMask |= TCOD_sys_handle_mouse_event(ev, mouse) & eventMask;
	retMask |= TCOD_sys_handle_key_event(ev, key) & eventMask;
	switch(ev->type) {
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
			int idx, mouse_touch_valid;
			float xf, yf, screen_x, screen_y;
			uint32_t ticks_taken = 0;

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
			screen_x = ev->tfinger.x * scale_data.surface_width;
			screen_y = ev->tfinger.y * scale_data.surface_height;
			xf = (float)(screen_x - scale_data.dst_offset_x) / scale_data.dst_display_width;
			yf = (float)(screen_y - scale_data.dst_offset_y) / scale_data.dst_display_height;
			tcod_touch.coords[idx][0] = scale_data.src_x0 + scale_data.src_copy_width * xf;
			tcod_touch.coords[idx][1] = scale_data.src_y0 + scale_data.src_copy_height * yf;
			tcod_touch.coords_delta[idx][0] = ev->tfinger.dx * scale_data.src_proportionate_width;
			tcod_touch.coords_delta[idx][1] = ev->tfinger.dy * scale_data.src_proportionate_height;

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
					sdl->scale_xc -= xc_shift; /* Actual display shift is the inverted finger movement. */
					if (sdl->scale_xc + 1e-3f < 0.0f)
                        sdl->scale_xc = 0.0f;
					if (sdl->scale_xc - 1e-3f > 1.0f)
                        sdl->scale_xc = 1.0f;
				}
				if (fabs(yc_shift) > 1e-3f) {
                    sdl->scale_yc -= yc_shift; /* Actual display shift is the inverted finger movement. */
					if (sdl->scale_yc + 1e-3f < 0.0f)
                        sdl->scale_yc = 0.0f;
					if (sdl->scale_yc - 1e-3f > 1.0f)
                        sdl->scale_yc = 1.0f;
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
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				TCOD_ctx.app_is_active=true; break;
			case SDL_WINDOWEVENT_FOCUS_LOST:
				TCOD_ctx.app_is_active=false; break;
			case SDL_WINDOWEVENT_EXPOSED:        /**< Window has been returned to and needs a refresh. */
				TCOD_sys_render(NULL, TCOD_ctx.root);
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
	return static_cast<TCOD_event_t>(retMask);
}
/**
 *  Internal function containing code shared by TCOD_sys_wait_for_event
 *  and TCOD_sys_check_for_event
 */
static TCOD_event_t TCOD_sys_check_for_event_(
    SDL_Event *ev, int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse) {
  TCOD_event_t retMask = TCOD_EVENT_NONE;
  if (eventMask == 0) { return TCOD_EVENT_NONE; }
  tcod_mouse.lbutton_pressed = 0;
  tcod_mouse.rbutton_pressed = 0;
  tcod_mouse.mbutton_pressed = 0;
  tcod_mouse.wheel_up = 0;
  tcod_mouse.wheel_down = 0;
  tcod_mouse.dx = 0;
  tcod_mouse.dy = 0;
  if (key) {
    key->vk = TCODK_NONE;
    key->c = 0;
    key->text[0] = '\0';
  }
  while (SDL_PollEvent(ev)) {
    retMask = TCOD_sys_handle_event(ev, static_cast<TCOD_event_t>(eventMask),
                                    key, &tcod_mouse);
    if (retMask & TCOD_EVENT_KEY) {
      break; /* only one key event per frame */
    }
  }
  if (mouse) { *mouse = tcod_mouse; }
  if (key && !(eventMask & retMask & TCOD_EVENT_KEY)) {
    key->vk = TCODK_NONE;
    key->c = 0;
    key->text[0] = '\0';
  }
  return retMask;
}
/**
 *  Wait for a specific type of event.
 *
 *  \param eventMask A bit-mask of TCOD_event_t flags.
 *  \param key Optional pointer to a TCOD_key_t struct.
 *  \param mouse Optional pointer to a TCOD_mouse_t struct.
 *  \param flush This should always be false.
 *  \return A TCOD_event_t flag showing which event was actually processed.
 *
 *  This function also returns when the SDL window is being closed.
 */
TCOD_event_t TCOD_sys_wait_for_event(int eventMask, TCOD_key_t *key,
                                     TCOD_mouse_t *mouse, bool flush) {
  SDL_Event ev;
  TCOD_event_t retMask = TCOD_EVENT_NONE;
  if (eventMask == 0) { return TCOD_EVENT_NONE; }
  if (flush) {
    while (SDL_PollEvent(&ev)) {
      TCOD_sys_check_for_event_(&ev, 0, NULL, NULL);
    }
  }
  do {
    SDL_WaitEvent(NULL);
    retMask = TCOD_sys_check_for_event_(&ev, eventMask, key, mouse);
  } while (ev.type != SDL_QUIT && (retMask & eventMask) == 0);
  return retMask;
}
/**
 *  Check for a specific type of event.
 *
 *  \param eventMask A bit-mask of TCOD_event_t flags.
 *  \param key Optional pointer to a TCOD_key_t struct.
 *  \param mouse Optional pointer to a TCOD_mouse_t struct.
 *  \param flush This should always be false.
 *  \return A TCOD_event_t flag showing which event was actually processed.
 */
TCOD_event_t TCOD_sys_check_for_event(
    int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse) {
  SDL_Event ev;
  return TCOD_sys_check_for_event_(&ev, eventMask, key, mouse);
}
/**
 *  Return a copy of the current mouse state.
 */
TCOD_mouse_t TCOD_mouse_get_status(void) {
  return tcod_mouse;
}

/* classic keyboard functions (based on generic events) */
/**
 *  Return immediately with a recently pressed key.
 *
 *  \param flags A TCOD_event_t bit-field, for example: `TCOD_EVENT_KEY_PRESS`
 *  \return A TCOD_key_t struct with a recently pressed key.
 *          If no event exists then the `vk` attribute will be `TCODK_NONE`
 */
TCOD_key_t TCOD_sys_check_for_keypress(int flags) {
  TCOD_key_t key;
  TCOD_event_t ev = TCOD_sys_check_for_event(flags & TCOD_EVENT_KEY,
                                             &key, nullptr);
  if ((ev & TCOD_EVENT_KEY) == 0) { return {}; }
  return key;
}
/**
 *  Wait for a key press event, then return it.
 *
 *  \param flush If 1 then the event queue will be cleared before waiting for
 *               the next event.  This should always be 0.
 *  \return A TCOD_key_t struct with the most recent key data.
 */
TCOD_key_t TCOD_sys_wait_for_keypress(bool flush) {
  TCOD_key_t key;
  TCOD_event_t ev = TCOD_sys_wait_for_event(TCOD_EVENT_KEY_PRESS,
                                            &key, nullptr, flush);
  if ((ev & TCOD_EVENT_KEY_PRESS) == 0) { return {}; }
  return key;
}

void TCOD_sys_sleep_milli(uint32_t milliseconds) {
	SDL_Delay(milliseconds);
}

SDL_Surface* TCOD_sys_load_image(const char *filename) {
  const image_support_t* img = image_type;
  while (img->extension != NULL && !img->check_type(filename)) { img++; }
  if (img->extension == NULL || img->read == NULL) { return NULL; } /* unknown format */
  return img->read(filename);
}

void TCOD_sys_get_image_size(const SDL_Surface* image, int *w, int *h)
{
  *w = image->w;
  *h = image->h;
}
TCOD_color_t TCOD_sys_get_image_pixel(const SDL_Surface* image, int x, int y)
{
  if (x < 0 || y < 0 || x >= image->w || y >= image->h) { return TCOD_black; }
  uint8_t bpp = image->format->BytesPerPixel;
  uint8_t* bits = static_cast<uint8_t*>(image->pixels);
  bits += y * image->pitch + x * bpp;
  switch (bpp) {
    case 1:
      if (image->format->palette) {
        SDL_Color col = image->format->palette->colors[(*bits)];
        return {col.r, col.g, col.b};
      } else {
        return TCOD_black;
      }
      break;
    default:
      return {
          *((bits)+image->format->Rshift/8),
          *((bits)+image->format->Gshift/8),
          *((bits)+image->format->Bshift/8),
      };
  }
}

int TCOD_sys_get_image_alpha(const SDL_Surface *surf, int x, int y)
{
  if (x < 0 || y < 0 || x >= surf->w || y >= surf->h) { return 255; }
  uint8_t bpp = surf->format->BytesPerPixel;
  if (bpp != 4) { return 255; }
  uint8_t* bits = static_cast<uint8_t*>(surf->pixels);
  bits += y * surf->pitch + x * bpp;
  return bits[surf->format->Ashift / 8];
}
uint32_t TCOD_sys_elapsed_milli(void)
{
  return SDL_GetTicks();
}
float TCOD_sys_elapsed_seconds(void)
{
  static float div=1.0f / 1000.0f;
  return SDL_GetTicks() * div;
}

void TCOD_sys_force_fullscreen_resolution(int width, int height) {
	TCOD_ctx.fullscreen_width=width;
	TCOD_ctx.fullscreen_height=height;
}

SDL_Surface* TCOD_sys_create_bitmap(int width, int height, TCOD_color_t *buf)
{
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
			uint32_t col=SDL_MapRGB(&fmt,buf[x+y*width].r,buf[x+y*width].g,buf[x+y*width].b);
			rect.x=x;
			rect.y=y;
			rect.w=1;
			rect.h=1;
			SDL_FillRect(bitmap,&rect,col);
		}
	}
	return bitmap;
}

void TCOD_sys_delete_bitmap(SDL_Surface* bitmap) {
	SDL_FreeSurface(bitmap);
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
  auto tileset = tcod::engine::get_tileset();
  if (tileset) {
    *w = tileset->get_tile_width();
    *h = tileset->get_tile_height();
  } else {
    *w = TCOD_ctx.font_width;
    *h = TCOD_ctx.font_height;
  }
}

void TCOD_sys_get_current_resolution(int *w, int *h) {
	/* be sure that SDL is initialized */
	TCOD_sys_startup();
	sdl->get_current_resolution(w,h);
}

/* image stuff */
bool TCOD_sys_check_magic_number(const char *filename, size_t size, uint8_t*data) {
	uint8_t tmp[128];
	size_t i;
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

SDL_Window* TCOD_sys_get_SDL_window(void)
{
  return window;
}

SDL_Renderer* TCOD_sys_get_SDL_renderer(void)
{
  return renderer;
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

#ifdef TCOD_TOUCH_INPUT
void TCOD_mouse_includes_touch(bool enable)
{
	mouse_touch = enable;
}
#else
void TCOD_mouse_includes_touch(bool)
{}
#endif

/*clipboard stuff */
bool TCOD_sys_clipboard_set(const char *value) {
	if (!has_startup) { return false; }
	return sdl->set_clipboard_text(value);
}

char *TCOD_sys_clipboard_get() {
	if (!has_startup) { return const_cast<char*>(""); }
	return const_cast<char*>(sdl->get_clipboard_text());
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

bool TCOD_sys_write_file(const char *filename, unsigned char *buf, uint32_t size) {
	return sdl->file_write(filename,buf,size);
}

/* Mark a rectangle of tiles dirty. */
void TCOD_sys_set_dirty(int dx, int dy, int dw, int dh) {
	int x, y;
	struct TCOD_Console *dat = sdl->get_root_console_cache();
	if (!dat) return;
	TCOD_IFNOT(dx < dat->w && dy < dat->h && dx + dw >= 0 && dy + dh >= 0) return;
	TCOD_IFNOT(dx >= 0) {
		dw += dx;
		dx = 0;
	}
	TCOD_IFNOT(dy >= 0) {
		dh += dy;
		dy = 0;
	}
	TCOD_IFNOT(dx + dw <= dat->w) dw = dat->w - dx;
	TCOD_IFNOT(dy + dh <= dat->h) dh = dat->h - dy;

	for (x = dx; x < dx + dw; ++x) {
		for (y = dy; y < dy + dh; ++y) {
			dat->tiles[dat->w * y + x].ch = -1;
		}
	}
}

/* Mark a character dirty by invalidating all occurrences of it in the software
   renderer cache.  Each character tile will be refreshed when the old buffer
   is compared to the current buffer next render.
   It's recommended that this method stays non-public. */
void TCOD_sys_set_dirty_character_code(int ch) {
	int i;
	struct TCOD_Console *dat = sdl->get_root_console_cache();
	if (!dat) return;
	for (i = 0; i < dat->w * dat->h; ++i) {
		if (dat->tiles[i].ch == ch) {
			dat->tiles[i].ch = -1;
		}
	}
}
/**
 *  Return the current tile index for a given character code.
 */
int TCOD_get_tileid_for_charcode_(int charcode) {
  if (charcode >= 0 && charcode < TCOD_ctx.max_font_chars) {
    return TCOD_ctx.ascii_to_tcod[charcode];
  }
  return 0;
}
struct SDL_Window* TCOD_sys_get_sdl_window_(void)
{
  return window;
}
struct SDL_Renderer* TCOD_sys_get_sdl_renderer_(void)
{
  return renderer;
}
#endif /* TCOD_BARE */
