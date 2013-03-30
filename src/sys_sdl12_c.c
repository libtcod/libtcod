/*
* libtcod 1.6.0
* Copyright (c) 2008,2009,2010,2012,2013 Jice & Mingos
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
#include "libtcod.h"
#include "libtcod_int.h"

static void get_closest_mode(int *w, int *h) {
	int i;
	int bestw,besth;
	SDL_Rect **modes = modes=SDL_ListModes(NULL, SDL_FULLSCREEN);

	bestw=99999;
	besth=99999;
	if(modes != (SDL_Rect **)0 && modes != (SDL_Rect **)-1){
		for(i=0;modes[i];++i) {
			if (modes[i]->w >= *w && modes[i]->w <= bestw
				&& modes[i]->h >= *h && modes[i]->h <= besth
				&& SDL_VideoModeOK(modes[i]->w, modes[i]->h, 32, SDL_FULLSCREEN)) {
				bestw=modes[i]->w;
				besth=modes[i]->h;
			}
		}
	}
	if ( bestw != 99999) {
		*w=bestw;
		*h=besth;
	}
}

/* render the console on a surface/texture
   SDL12 : vbitmap = SDL_Surface
   SDL2 : vbitmap = SDL_Texture */
static void render(void *vbitmap, int console_width, int console_height, char_t *console_buffer, char_t *prev_console_buffer) {
	char_t *prev_console_buffer_ptr = prev_console_buffer;
	if ( TCOD_ctx.renderer == TCOD_RENDERER_SDL ) {
		TCOD_sys_console_to_bitmap(vbitmap, console_width, console_height, console_buffer, prev_console_buffer_ptr);
		if ( TCOD_ctx.sdl_cbk ) {
			TCOD_ctx.sdl_cbk((void *)screen);
		}
		SDL_Flip(screen);
	}
#ifndef NO_OPENGL
	else {
		TCOD_opengl_render(oldFade, ascii_updated, console_buffer, prev_console_buffer);
		TCOD_opengl_swap();
	}  
#endif
	oldFade=(int)TCOD_console_get_fade();
	if ( any_ascii_updated ) {
		memset(ascii_updated,0,sizeof(bool)*TCOD_ctx.max_font_chars);
		any_ascii_updated=false;
	}
}

static SDL_Surface *create_surface(int width, int height, bool with_alpha) {
	Uint32 rmask,gmask,bmask,amask;
	SDL_Surface *bitmap;
	int flags=SDL_SWSURFACE;

	if ( with_alpha ) {
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
		with_alpha ? 32:24,
		rmask,gmask,bmask,amask);
	if ( with_alpha ) {
		SDL_SetAlpha(bitmap, SDL_SRCALPHA, 255);
	}
	return (void *)bitmap;
}

static void create_window(int w, int h, bool fullscreen) {
	if ( fullscreen  ) {
		find_resolution();
#ifndef NO_OPENGL	
		if (TCOD_ctx.renderer != TCOD_RENDERER_SDL ) {
			TCOD_opengl_init_attributes();
			screen=SDL_SetVideoMode(TCOD_ctx.actual_fullscreen_width,TCOD_ctx.actual_fullscreen_height,32,SDL_FULLSCREEN|SDL_OPENGL);
			if ( screen && TCOD_opengl_init_state(w, h, charmap) && TCOD_opengl_init_shaders() ) {
				TCOD_LOG(("Using %s renderer...\n",TCOD_ctx.renderer == TCOD_RENDERER_GLSL ? "GLSL" : "OPENGL"));
			} else {
				TCOD_LOG(("Fallback to SDL renderer...\n"));
				TCOD_ctx.renderer = TCOD_RENDERER_SDL;
			}
		} 
#endif		
		if (TCOD_ctx.renderer == TCOD_RENDERER_SDL ) {
			screen=SDL_SetVideoMode(TCOD_ctx.actual_fullscreen_width,TCOD_ctx.actual_fullscreen_height,32,SDL_FULLSCREEN);
			if ( screen == NULL ) TCOD_fatal_nopar("SDL : cannot set fullscreen video mode");
		}
		SDL_ShowCursor(0);
		TCOD_ctx.actual_fullscreen_width=screen->w;
		TCOD_ctx.actual_fullscreen_height=screen->h;
		TCOD_sys_init_screen_offset();
		SDL_FillRect(screen,0,0);
	} else {
#ifndef NO_OPENGL	
		if (TCOD_ctx.renderer != TCOD_RENDERER_SDL ) {
			TCOD_opengl_init_attributes();
			screen=SDL_SetVideoMode(w*TCOD_ctx.font_width,h*TCOD_ctx.font_height,32,SDL_OPENGL);
			if ( screen && TCOD_opengl_init_state(w, h, charmap) && TCOD_opengl_init_shaders() ) {
				TCOD_LOG(("Using %s renderer...\n",TCOD_ctx.renderer == TCOD_RENDERER_GLSL ? "GLSL" : "OPENGL"));
			} else {
				TCOD_LOG(("Fallback to SDL renderer...\n"));
				TCOD_ctx.renderer = TCOD_RENDERER_SDL;
			}
		} 
#endif		
		if (TCOD_ctx.renderer == TCOD_RENDERER_SDL ) {
			screen=SDL_SetVideoMode(w*TCOD_ctx.font_width,h*TCOD_ctx.font_height,32,0);
			TCOD_LOG(("Using SDL renderer...\n"));
		}
		if ( screen == NULL ) TCOD_fatal_nopar("SDL : cannot create window");
	}
	SDL_EnableUNICODE(1);
	TCOD_ctx.fullscreen=fullscreen;
}

static void set_fullscreen(bool fullscreen) {
	bool mouseOn=SDL_ShowCursor(-1);
	if ( fullscreen ) {
		SDL_Surface *newscreen;
		find_resolution();
		newscreen=SDL_SetVideoMode(TCOD_ctx.actual_fullscreen_width,TCOD_ctx.actual_fullscreen_height,32,SDL_FULLSCREEN);
		TCOD_IFNOT ( newscreen != NULL ) return;
		screen=newscreen;
		SDL_ShowCursor(mouseOn ? 1:0);
		TCOD_ctx.actual_fullscreen_width=screen->w;
		TCOD_ctx.actual_fullscreen_height=screen->h;
		TCOD_sys_init_screen_offset();
	} else {
		SDL_Surface *newscreen=SDL_SetVideoMode(TCOD_ctx.root->w*TCOD_ctx.font_width,TCOD_ctx.root->h*TCOD_ctx.font_height,32,0);
		TCOD_IFNOT( newscreen != NULL ) return;
		screen=newscreen;
		SDL_ShowCursor(mouseOn ? 1:0);
		TCOD_ctx.fullscreen_offsetx=0;
		TCOD_ctx.fullscreen_offsety=0;
	}
	TCOD_ctx.fullscreen=fullscreen;
	oldFade=-1; /* to redraw the whole screen */
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

static void set_window_title(const char *title) {
	SDL_WM_SetCaption(title,NULL);
}

static void save_screenshot(const char *filename) {
	if ( TCOD_ctx.renderer == TCOD_RENDERER_SDL ) {
		TCOD_sys_save_bitmap((void *)screen,filename);
#ifndef NO_OPENGL		
	} else {
		SDL_Surface *screenshot=(SDL_Surface *)TCOD_opengl_get_screen();
		TCOD_sys_save_bitmap((void *)screenshot,filename);
		SDL_FreeSurface(screenshot);
#endif		
	}
}
/* get desktop resolution */
static void get_current_resolution(int *w, int *h) {
	const SDL_VideoInfo *info=SDL_GetVideoInfo();
	*w=info->current_w;
	*h=info->current_h;
}

static void set_mouse_position(int x, int y) {
  SDL_WarpMouse((Uint16)x,(Uint16)y);
}

static bool file_read(const char *filename, unsigned char **buf, size_t *size) {
	uint32 filesize;
	/* get file size */
	FILE * fops=fopen(filename,"rb");
	if (!fops) return false;
	fseek(fops,0,SEEK_END);
	filesize=ftell(fops);
	fseek(fops,0,SEEK_SET);
	/* allocate buffer */
	*buf = (unsigned char *)malloc(sizeof(unsigned char)*filesize);
	/* read from file */
	if (fread(*buf,sizeof(unsigned char),filesize,fops) != filesize ) {
		fclose(fops);
		free(*buf);
		return false;
	}
	fclose(fops);
	*size=filesize;
	return true;
}

static bool file_exists(const char * filename) {
	FILE * fops;
	fops=fopen(filename,"rb");
	if (fops) {
		fclose(fops);
		return true;
	}
	return false;
}

static bool file_write(const char *filename, unsigned char *buf, uint32 size) {
	FILE * fops=fopen(filename,"wb");
	if (!fops) return false;
	fwrite(buf,sizeof(unsigned char),size,fops);
	fclose(fops);
	return true;
}

static void term() {
	screen=NULL;
}

TCOD_SDL_driver_t *SDL_implementation_factory() {
	TCOD_SDL_driver_t *ret=(TCOD_SDL_driver_t *)calloc(1,sizeof(TCOD_SDL_driver_t));
	ret->get_closest_mode=get_closest_mode;
	ret->render=render;
	ret->create_surface=create_surface;
	ret->create_window=create_window;
	ret->set_fullscreen=set_fullscreen;
	ret->set_window_title=set_window_title;
	ret->save_screenshot=save_screenshot;
	ret->get_current_resolution=get_current_resolution;
	ret->set_mouse_position=set_mouse_position;
	ret->file_read=file_read;
	ret->file_exists=file_exists;
	ret->file_write=file_write;
	ret->term=term;
	return ret;
}

