/*
* libtcod 1.5.1
* Copyright (c) 2008,2009,2010,2012 Jice & Mingos
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

/*
 * Warning ! This is not part of libtcod. It's an outdated SFML renderer attempt. To be upgraded... one day... maybe...
 */


#include <SFML/Graphics.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "libtcod.h"
#include "libtcod_int.h"

static sfRenderWindow *renderWindow=NULL;
static sfInput *input=NULL;
static bool cursorShown=true;
static sfClock *clock=NULL;
static sfImage* charmap=NULL;
static sfSprite* charmapSprite=NULL;

static int consoleWidth=0;
static int consoleHeight=0;
static char_t *consoleBuffer=NULL;
static char_t *prevConsoleBuffer=NULL;

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

static bool key_status[TCODK_CHAR+1];

static int nbBlit=0;

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
	int i,bestw,besth,wantedw,wantedh;
	wantedw=fullscreen_width>consoleWidth*fontWidth?fullscreen_width:consoleWidth*fontWidth;
	wantedh=fullscreen_height>consoleHeight*fontHeight?fullscreen_height:consoleHeight*fontHeight;
	actual_fullscreen_width=wantedw;
	actual_fullscreen_height=wantedh;

	bestw=99999;
	besth=99999;
	for(i=0;i < sfVideoMode_GetModesCount();i++) {
		sfVideoMode mode=sfVideoMode_GetMode(i);
		if (mode.Width >= wantedw && mode.Width <= bestw
			&& mode.Height >= wantedh && mode.Height <= besth ) {
			bestw=mode.Width;
			besth=mode.Height;
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
	sfImage *bitmap;
	w = TCOD_console_get_width(console) * fontWidth;
	h = TCOD_console_get_height(console) * fontHeight;
	bitmap=sfImage_CreateFromColor(w,h,sfBlack);
	return (void *)bitmap;
}

void TCOD_sys_console_to_bitmap(void *vbitmap, int console_width, int console_height, char_t *console_buffer, char_t *prev_console_buffer) {
	int x,y;
	sfImage *bitmap=(sfImage *)vbitmap;
	TCOD_color_t fading_color = TCOD_console_get_fading_color();
	static int oldFade=-1;
	uint8 fade = TCOD_console_get_fade();
	static sfShape *rectShape=NULL;

	if ( ! rectShape ) {
		rectShape=sfShape_CreateRectangle(0.5f, 0.5f, 0.5f + fontWidth, 0.5f + fontHeight, sfBlack, 0.0f, sfBlack);
		sfShape_EnableOutline(rectShape, sfFalse);
	}
	for (y=0;y<console_height;y++) {
		for (x=0; x<console_width; x++) {
			char_t *c=&console_buffer[x+y*console_width];
			bool changed=true;
			if ( changed ) {
				sfColor b={c->back.r,c->back.g,c->back.b,255};
				// background
				b.r = ((int)b.r) * fade / 255 + ((int)fading_color.r) * (255-fade)/255;
				b.g = ((int)b.g) * fade / 255  + ((int)fading_color.g) * (255-fade)/255;
				b.b = ((int)b.b) * fade / 255 + ((int)fading_color.b) * (255-fade)/255;
				int destx=x*fontWidth;
				int desty=y*fontHeight;
				if ( (void *)bitmap == (void *)renderWindow && fullscreen_on ) {
					destx+=fullscreen_offsetx;
					desty+=fullscreen_offsety;
				}
				sfShape_SetPosition(rectShape,destx+0.5f,desty+0.5f);
				sfShape_SetColor(rectShape,sfWhite);
				sfRenderWindow_DrawShape (renderWindow, rectShape);
				nbBlit++;
				int srcx,srcy;
				if ( c->c != ' ') {
					// foreground
					int ascii=(int)c->c;
					sfColor f={c->fore.r,c->fore.g,c->fore.b,255};
					f.r = ((int)f.r) * fade / 255 + ((int)fading_color.r) * (255-fade)/255;
					f.g = ((int)f.g) * fade / 255 + ((int)fading_color.g) * (255-fade)/255;
					f.b = ((int)f.b) * fade / 255 + ((int)fading_color.b) * (255-fade)/255;
					if (fontInRow) {
						srcx = (ascii%fontNbCharHoriz)*fontWidth;
						srcy = (ascii/fontNbCharHoriz)*fontHeight;
					} else {
						srcx = (ascii/fontNbCharVertic)*fontWidth;
						srcy = (ascii%fontNbCharVertic)*fontHeight;
					}
					sfIntRect rect={srcx,srcy,srcx+fontWidth-1,srcy+fontHeight-1};
					sfSprite_SetSubRect(charmapSprite,&rect);
					sfSprite_SetX(charmapSprite,destx);
					sfSprite_SetY(charmapSprite,desty);
					sfSprite_SetColor(charmapSprite,f);
					sfRenderWindow_DrawSprite(renderWindow,charmapSprite);
				}
			}
		}
	}

}

void TCOD_sys_set_keyboard_repeat(int initial_delay, int interval) {
	if ( initial_delay ) sfRenderWindow_EnableKeyRepeat(renderWindow,sfTrue);
	else sfRenderWindow_EnableKeyRepeat(renderWindow,sfFalse);
	// delays not supported by SFML ?
}

bool TCOD_sys_init(int w,int h, char_t *buf, char_t *oldbuf, bool fullscreen) {
	consoleWidth=w;
	consoleHeight=h;
	
	if ( fullscreen ) {
		find_resolution();
		sfWindowSettings Settings = {0, 0, 0};
		sfVideoMode Mode = {actual_fullscreen_width, actual_fullscreen_height, 32};
		renderWindow=sfRenderWindow_Create(Mode,window_title,sfFullscreen,Settings);
		TCOD_mouse_show_cursor(false);
		actual_fullscreen_width=sfRenderWindow_GetWidth(renderWindow);
		actual_fullscreen_height=sfRenderWindow_GetHeight(renderWindow);
		fullscreen_offsetx=(actual_fullscreen_width-consoleWidth*fontWidth)/2;
		fullscreen_offsety=(actual_fullscreen_height-consoleHeight*fontHeight)/2;
	} else {
		sfWindowSettings Settings = {0, 0, 0};
		sfVideoMode Mode = {w*fontWidth, h*fontHeight, 32};
		renderWindow=sfRenderWindow_Create(Mode,window_title,sfClose,Settings);
		TCOD_mouse_show_cursor(true);
	}
	charmap=sfImage_CreateFromFile(font_file);
	if (charmap == NULL ) TCOD_fatal("SFML : cannot load %s",font_file);
	if ( sfImage_GetWidth(charmap) < fontNbCharHoriz * fontWidth 
		|| sfImage_GetHeight(charmap) < fontNbCharVertic * fontHeight ) TCOD_fatal("bitmap %s too small for %dx%d, %dx%d characters\n",
		font_file,fontWidth,fontHeight,fontNbCharHoriz,fontNbCharVertic);
	sfColor colorKey={fontKeyCol.r,fontKeyCol.g,fontKeyCol.b,255};
	sfImage_CreateMaskFromColor(charmap, colorKey, 0);
	sfImage_SetSmooth(charmap, sfFalse);
	sfImage_Bind(charmap);
	charmapSprite=sfSprite_Create();
	sfSprite_SetImage(charmapSprite,charmap);
	sfSprite_SetBlendMode(charmapSprite,sfBlendNone);
	consoleBuffer=buf;
	prevConsoleBuffer=oldbuf;
	fullscreen_on=fullscreen;
	memset(key_status,0,sizeof(bool)*(TCODK_CHAR+1));
	return true;
}

void TCOD_sys_save_bitmap(void *bitmap, const char *filename) {
	sfImage_SaveToFile((sfImage *)bitmap,filename);
}

void TCOD_sys_save_screenshot(const char *filename) {
	sfImage *img = sfRenderWindow_Capture(renderWindow);
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
	sfImage_SaveToFile(img,filename);
}

void TCOD_sys_set_fullscreen(bool fullscreen) {
	fullscreen_on=fullscreen;
	if ( fullscreen ) {
		find_resolution();
		sfWindowSettings Settings = {24, 8, 0};
		sfVideoMode Mode = {actual_fullscreen_width, actual_fullscreen_height, 32};
		renderWindow=sfRenderWindow_Create(Mode,window_title,sfFullscreen,Settings);
		TCOD_mouse_show_cursor(false);
		actual_fullscreen_width=sfRenderWindow_GetWidth(renderWindow);
		actual_fullscreen_height=sfRenderWindow_GetHeight(renderWindow);
		fullscreen_offsetx=(actual_fullscreen_width-consoleWidth*fontWidth)/2;
		fullscreen_offsety=(actual_fullscreen_height-consoleHeight*fontHeight)/2;
		/*
		printf ("actual resolution : %dx%d\n",actual_fullscreen_width,actual_fullscreen_height);
		printf ("offset : %dx%d\n",fullscreen_offsetx,fullscreen_offsety);
		printf ("flags : %x bpp : %d bitspp : %d\n",screen->flags, screen->format->BytesPerPixel, screen->format->BitsPerPixel);
		*/
	} else {
		sfWindowSettings Settings = {24, 8, 0};
		sfVideoMode Mode = {consoleWidth*fontWidth, consoleHeight*fontHeight, 32};
		renderWindow=sfRenderWindow_Create(Mode,window_title,sfClose|sfTitlebar,Settings);
		TCOD_mouse_show_cursor(true);
	}
}


void TCOD_sys_set_window_title(const char *title) {
	strcpy(window_title,title);
	// Not supported by SFML ??
}

void TCOD_sys_flush(bool render) {
	static uint32 old_time,new_time=0, elapsed=0;
	int32 frame_time,time_to_wait;
	if ( render ) {
		TCOD_sys_console_to_bitmap(renderWindow,TCOD_console_get_width(NULL),TCOD_console_get_height(NULL),consoleBuffer,prevConsoleBuffer);
		sfRenderWindow_Display(renderWindow);
	}
	old_time=new_time;
	new_time=TCOD_sys_elapsed_milli();
	if ( new_time / 1000 != elapsed ) {
		/* update fps every second */
		fps=cur_fps;
		cur_fps=0;
		elapsed=new_time/1000;
printf ("nbBlit : %d\n",(int)(nbBlit/fps));nbBlit=0;
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

static void TCOD_sys_convert_event(sfEvent *ev, TCOD_key_t *ret) {
	if ( ev->Type == sfEvtKeyPressed || ev->Type == sfEvtKeyReleased ) {
		switch(ev->Key.Code) {
			case sfKeyEscape : ret->vk=TCODK_ESCAPE;break;
			case sfKeySpace : ret->vk=TCODK_SPACE; break;
			case sfKeyBack : ret->vk=TCODK_BACKSPACE;break;
			case sfKeyTab : ret->vk=TCODK_TAB;break;
			case sfKeyReturn : ret->vk=TCODK_ENTER;break;
			case sfKeyPause : ret->vk=TCODK_PAUSE;break;
			case sfKeyPageUp : ret->vk=TCODK_PAGEUP;break;
			case sfKeyPageDown : ret->vk=TCODK_PAGEDOWN;break;
			case sfKeyHome : ret->vk=TCODK_HOME;break;
			case sfKeyEnd : ret->vk=TCODK_END;break;
			case sfKeyDelete : ret->vk=TCODK_DELETE;break;
			case sfKeyInsert : ret->vk=TCODK_INSERT; break;
			case sfKeyLAlt : case sfKeyRAlt : ret->vk=TCODK_ALT;break;
			case sfKeyLControl : case sfKeyRControl : ret->vk=TCODK_CONTROL;break;
			case sfKeyLShift : case sfKeyRShift : ret->vk=TCODK_SHIFT;break;
	//		case SDLK_PRINT : ret->vk=TCODK_PRINTSCREEN;break;
			case sfKeyLeft : ret->vk=TCODK_LEFT;break;
			case sfKeyUp : ret->vk=TCODK_UP;break;
			case sfKeyRight : ret->vk=TCODK_RIGHT;break;
			case sfKeyDown : ret->vk=TCODK_DOWN;break;
			case sfKeyF1 : ret->vk=TCODK_F1;break;
			case sfKeyF2 : ret->vk=TCODK_F2;break;
			case sfKeyF3 : ret->vk=TCODK_F3;break;
			case sfKeyF4 : ret->vk=TCODK_F4;break;
			case sfKeyF5 : ret->vk=TCODK_F5;break;
			case sfKeyF6 : ret->vk=TCODK_F6;break;
			case sfKeyF7 : ret->vk=TCODK_F7;break;
			case sfKeyF8 : ret->vk=TCODK_F8;break;
			case sfKeyF9 : ret->vk=TCODK_F9;break;
			case sfKeyF10 : ret->vk=TCODK_F10;break;
			case sfKeyF11 : ret->vk=TCODK_F11;break;
			case sfKeyF12 : ret->vk=TCODK_F12;break;
			case sfKeyNum0 : ret->vk=TCODK_KP0;break;
			case sfKeyNum1 : ret->vk=TCODK_KP1;break;
			case sfKeyNum2 : ret->vk=TCODK_KP2;break;
			case sfKeyNum3 : ret->vk=TCODK_KP3;break;
			case sfKeyNum4 : ret->vk=TCODK_KP4;break;
			case sfKeyNum5 : ret->vk=TCODK_KP5;break;
			case sfKeyNum6 : ret->vk=TCODK_KP6;break;
			case sfKeyNum7 : ret->vk=TCODK_KP7;break;
			case sfKeyNum8 : ret->vk=TCODK_KP8;break;
			case sfKeyNum9 : ret->vk=TCODK_KP9;break;
			case sfKeyDivide : ret->vk=TCODK_KPDIV;break;
			case sfKeyMultiply : ret->vk=TCODK_KPMUL;break;
			case sfKeyAdd : ret->vk=TCODK_KPADD;break;
			case sfKeySubtract : ret->vk=TCODK_KPSUB;break;
	//		case SDLK_KP_ENTER : ret->vk=TCODK_KPENTER;break;
	//		case SDLK_KP_PERIOD : ret->vk=TCODK_KPDEC;break;
			default : ret->vk=TCODK_NONE; break;
		}
	} else {
		ret->c = ev->Text.Unicode;
		ret->vk = TCODK_CHAR;
		if ( ret->c >= '0' && ret->c <= '9' ) ret->vk = TCODK_0+(ret->c-'0');
	}
}

static TCOD_key_t TCOD_sys_SFMLtoTCOD(sfEvent *ev, int flags) {
	static TCOD_key_t ret;
	ret.vk=TCODK_NONE;
	ret.c=0;
	ret.pressed=0;
	switch (ev->Type) {
		case sfEvtClosed :
			TCOD_ctx.is_window_closed=true;
		break;
		case sfEvtKeyReleased : {
			TCOD_key_t tmpkey;
			switch(ev->Key.Code) {
				case sfKeyLAlt : ret.lalt=0; break;
				case sfKeyRAlt : ret.ralt=0; break;
				case sfKeyLControl : ret.lctrl=0; break;
				case sfKeyRControl : ret.rctrl=0; break;
				case sfKeyLShift : ret.shift=0; break;
				case sfKeyRShift : ret.shift=0; break;
				default : break;
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
		case sfEvtKeyPressed : {
			TCOD_key_t tmpkey;
			switch(ev->Key.Code) {
				case sfKeyLAlt : ret.lalt=1; break;
				case sfKeyRAlt : ret.ralt=1; break;
				case sfKeyLControl : ret.lctrl=1; break;
				case sfKeyRControl : ret.rctrl=1; break;
				case sfKeyLShift : ret.shift=1; break;
				case sfKeyRShift : ret.shift=1; break;
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
		case sfEvtTextEntered : {
			
		} 
		break;
		default : break;
	}
	return ret;
}

bool TCOD_sys_is_key_pressed(TCOD_keycode_t key) {
	return key_status[key];
}

TCOD_key_t TCOD_sys_check_for_keypress(int flags) {
	static TCOD_key_t noret={TCODK_NONE,0};
	sfEvent ev;
	TCOD_key_t retkey={TCODK_NONE,0};
	bool event=false;

	while ( sfRenderWindow_GetEvent(renderWindow, &ev) ) {
		TCOD_key_t tmpretkey=TCOD_sys_SFMLtoTCOD(&ev, flags);
		event=true;
		if ( tmpretkey.vk != TCODK_NONE ) retkey=tmpretkey;
	}
	if ( event ) return retkey;
	return noret;
}

TCOD_key_t TCOD_sys_wait_for_keypress(bool flush) {
	sfEvent event;
	TCOD_key_t ret={TCODK_NONE,0};
	if ( flush ) {
		while ( sfRenderWindow_GetEvent(renderWindow, &event) ) {
			if (event.Type == sfEvtClosed) {
				sfRenderWindow_Close(renderWindow);
				return ret;
			}
			TCOD_sys_SFMLtoTCOD(&event,0);			
		}
	}
	do {
		while ( !sfRenderWindow_GetEvent(renderWindow, &event) ) { TCOD_sys_sleep_milli(10); }
		ret = TCOD_sys_SFMLtoTCOD(&event,TCOD_KEY_PRESSED);
	} while ( ret.vk == TCODK_NONE && event.Type != sfEvtClosed );
	if (event.Type == sfEvtClosed) sfRenderWindow_Close(renderWindow);
	return ret;
}


void TCOD_sys_sleep_milli(uint32 milliseconds) {
	sfSleep((float)(milliseconds)/1000.0f);
}

void TCOD_sys_term() {
	sfRenderWindow_Destroy(renderWindow);
}

void *TCOD_sys_load_image(const char *filename) {
	sfImage* img=sfImage_CreateFromFile(filename);
	return (void *)img;
}

void TCOD_sys_get_image_size(const void *image, int *w, int *h) {
	sfImage *img=(sfImage *)image;
    *w = (int)sfImage_GetWidth(img);
    *h = (int)sfImage_GetHeight(img);
}

TCOD_color_t TCOD_sys_get_image_pixel(const void *image,int x, int y) {
	TCOD_color_t ret;
	sfImage *img=(sfImage *)image;
	sfColor col=sfImage_GetPixel(img,(unsigned int)x,(unsigned int)y);
	ret.r=col.r;
	ret.g=col.g;
	ret.b=col.b;
	return ret;
}

uint32 TCOD_sys_elapsed_milli() {
	if (!clock) clock=sfClock_Create();
	return (uint32)(sfClock_GetTime(clock)*1000);
}

float TCOD_sys_elapsed_seconds() {
	if (!clock) clock=sfClock_Create();
	return sfClock_GetTime(clock);
}

void TCOD_sys_force_fullscreen_resolution(int width, int height) {
	fullscreen_width=width;
	fullscreen_height=height;
}

void * TCOD_sys_create_bitmap(int width, int height, TCOD_color_t *buf) {
	unsigned int x,y;
	sfImage *bitmap=sfImage_CreateFromColor(width,height,sfBlack);
	for (x=0; x < width; x++) {
		int offset=x;
		for (y=0; y < height; y++) {
			sfColor col;
			col.r=buf[offset].r;
			col.g=buf[offset].g;
			col.b=buf[offset].b;
			col.a=255;
			sfImage_SetPixel(bitmap,x,y,col);
			offset+=width;
		}
	}
	return (void *)bitmap;
}

void TCOD_sys_delete_bitmap(void *bitmap) {
	sfImage_Destroy((sfImage *)bitmap);
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
	sfVideoMode mode = sfVideoMode_GetDesktopMode(); 
	*w=mode.Width;
	*h=mode.Height;	
}

/* mouse stuff */
TCOD_mouse_t TCOD_mouse_get_status() {
  static int oldMouseX=-1,oldMouseY=-1;
  TCOD_mouse_t ms;
  int charWidth, charHeight;
  static bool lbut=false;
  static bool rbut=false;
  static bool mbut=false;
  
  if (!input) {
	if ( !renderWindow ) return ms;
	input=sfRenderWindow_GetInput(renderWindow);
  }

  ms.lbutton = sfInput_IsMouseButtonDown(input,sfButtonLeft);
  ms.lbutton_pressed=false;
  if (ms.lbutton) lbut=true;
  else if( lbut ) {
  	lbut=false;
  	ms.lbutton_pressed=true;
  }
  ms.rbutton = sfInput_IsMouseButtonDown(input,sfButtonRight);
  ms.rbutton_pressed=false;
  if (ms.rbutton) rbut=true;
  else if( rbut ) {
  	rbut=false;
  	ms.rbutton_pressed=true;
  }
  ms.mbutton = sfInput_IsMouseButtonDown(input,sfButtonMiddle);
  ms.mbutton_pressed=false;
  if (ms.mbutton) mbut=true;
  else if( mbut ) {
  	mbut=false;
  	ms.mbutton_pressed=true;
  }

  ms.x = (int) sfInput_GetMouseX(input);
  ms.y = (int) sfInput_GetMouseY(input);
  
  if ( oldMouseX != -1 ) {
  	ms.dx=ms.x - oldMouseX;
  	ms.dy=ms.y - oldMouseY;
  } else {
  	ms.dx=0;
  	ms.dy=0;
  }
  oldMouseX=ms.x;
  oldMouseY=ms.y;
  
  TCOD_sys_get_char_size(&charWidth,&charHeight);
  ms.cx = ms.x / charWidth;
  ms.cy = ms.y / charHeight;
  ms.dcx = ms.dx / charWidth;
  ms.dcy = ms.dy / charHeight;
  return ms;
}

void TCOD_mouse_show_cursor(bool visible) {
  cursorShown=visible;
  sfRenderWindow_ShowMouseCursor (renderWindow, visible ? sfTrue : sfFalse);
}

bool TCOD_mouse_is_cursor_visible() {
  return cursorShown;
}

void TCOD_mouse_move(int x, int y) {
  sfRenderWindow_SetCursorPosition (renderWindow, (unsigned int) x, (unsigned int) y);
}

