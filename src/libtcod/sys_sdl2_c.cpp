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
#include "sys.h"

#include <stdio.h>
#include <string.h>

#include <SDL.h>

#include "console.h"
#include "libtcod_int.h"
#include "utility.h"
#include "color/canvas.h"

static SDL_Surface* scale_screen=NULL;
static bool clear_screen=false;
static struct TCOD_Console *root_console_cache; /* cache for previous values */

/* This just forces a complete redraw, bypassing the usual rendering of changes. */
void TCOD_sys_set_clear_screen(void) {
	clear_screen=true;
}

static void get_closest_mode(int *w, int *h) {
	SDL_DisplayMode wantedmode, closestmode;
	wantedmode.w = *w;
	wantedmode.h = *h;
	wantedmode.format = 0;  /* don't care for rest. */
	wantedmode.refresh_rate = 0;
	wantedmode.driverdata = 0;
	if (SDL_GetClosestDisplayMode(window?SDL_GetWindowDisplayIndex(window):0, &wantedmode, &closestmode) == &closestmode) {
		*w=closestmode.w;
		*h=closestmode.h;
	}
}

/*
 * Separate out the actual rendering, so that render to texture can be done.
 */
static void actual_rendering(void) {
	SDL_Rect srcRect, dstRect;
	SDL_Texture *texture;

	if (scale_data.min_scale_factor - 1e-3f > scale_factor) {
		/* Prepare for the unscaled and centered copy of the entire console. */
		srcRect.x=0; srcRect.y=0; srcRect.w=scale_screen->w; srcRect.h=scale_screen->h;
		if (TCOD_ctx.fullscreen) {
			dstRect.x=TCOD_ctx.fullscreen_offsetx; dstRect.y=TCOD_ctx.fullscreen_offsety;
		} else {
			dstRect.x=0; dstRect.y=0;
		}
		dstRect.w=scale_screen->w; dstRect.h=scale_screen->h;
	} else {
		/* Prepare for the scaled copy of the displayed console area. */
		srcRect.x=scale_data.src_x0; srcRect.y=scale_data.src_y0; srcRect.w=scale_data.src_copy_width; srcRect.h=scale_data.src_copy_height;
		dstRect.x=scale_data.dst_offset_x; dstRect.y=scale_data.dst_offset_y;
		dstRect.w=scale_data.dst_display_width; dstRect.h=scale_data.dst_display_height;
	}
	if ( TCOD_ctx.sdl_cbk ) {
		TCOD_ctx.sdl_cbk(scale_screen);
	}
	texture = SDL_CreateTextureFromSurface(renderer, scale_screen);
	SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
	SDL_DestroyTexture(texture);
}

/* Return an up-to-date cache for the root console, create or resize the cache
   if needed */
static struct TCOD_Console *ensure_cache(struct TCOD_Console* root) {
	if (!root_console_cache ||
			root_console_cache->w != root->w ||
			root_console_cache->h != root->h) {
		if (root_console_cache) { TCOD_console_delete(root_console_cache); }
		root_console_cache = TCOD_console_new(root->w, root->h);
	}
	return root_console_cache;
}

/* In order to avoid rendering race conditions and the ensuing segmentation
 * faults, this should only be called when it would normally be and not
 * specifically to force screen refreshes.  To this end, and to avoid
 * threading complications it takes care of special cases internally.  */
static void render(
    TCOD_SDL_driver_t *sdl,
    void*, // vbitmap
    struct TCOD_Console *console)
{
	if ( TCOD_ctx.renderer == TCOD_RENDERER_SDL ) {
		int console_width_p = console->w * TCOD_ctx.font_width;
		int console_height_p = console->h * TCOD_ctx.font_height;

		/* Make a bitmap of exact rendering size and correct format. */
		if (scale_screen == NULL) {
			int bpp;
			uint32_t rmask, gmask, bmask, amask;
			if (SDL_PixelFormatEnumToMasks(SDL_GetWindowPixelFormat(window), &bpp, &rmask, &gmask, &bmask, &amask) == SDL_FALSE) {
				TCOD_fatal("SDL : failed to create scaling surface : indeterminate window pixel format");
				return;
			}
			scale_screen=SDL_CreateRGBSurface(SDL_SWSURFACE,console_width_p,console_height_p,bpp,rmask,gmask,bmask,amask);
			if (scale_screen == NULL) {
				TCOD_fatal("SDL : failed to create scaling surface");
				return;
			}
		} else if (clear_screen) {
			clear_screen=false;
			SDL_FillRect(scale_screen,0,0);
			/* Implicitly do complete console redraw, not just tracked changes. */
			TCOD_console_set_dirty(0, 0, console->w, console->h);
		}

		TCOD_sys_console_to_bitmap(scale_screen, console, ensure_cache(console));

		/* Scale the rendered bitmap to the screen, preserving aspect ratio, and blit it.
		 * This data is also used for console coordinate resolution.. */
		if (scale_data.last_scale_factor != scale_factor || scale_data.last_scale_xc != sdl->scale_xc || scale_data.last_scale_yc != sdl->scale_yc ||
				scale_data.last_fullscreen != TCOD_ctx.fullscreen || scale_data.force_recalc) {
			/* Preserve old value of input variables, to enable recalculation if they change. */
			scale_data.last_scale_factor = scale_factor;
			scale_data.last_scale_xc = sdl->scale_xc;
			scale_data.last_scale_yc = sdl->scale_yc;
			scale_data.last_fullscreen = TCOD_ctx.fullscreen;
			scale_data.force_recalc = 0;

			if (scale_data.last_fullscreen) {
				scale_data.surface_width = TCOD_ctx.actual_fullscreen_width;
				scale_data.surface_height = TCOD_ctx.actual_fullscreen_height;
			} else {
				scale_data.surface_width = console_width_p;
				scale_data.surface_height = console_height_p;
			}
			scale_data.min_scale_factor = std::max<float>(
          static_cast<float>(console_width_p) / scale_data.surface_width,
          static_cast<float>(console_height_p) / scale_data.surface_height);
			if (scale_data.min_scale_factor > 1.0f)
				scale_data.min_scale_factor = 1.0f;
			/*printf("min_scale_factor %0.3f = MAX(%d/%d, %d/%d)", scale_data.min_scale_factor, console_width_p, scale_data.surface_width, console_height_p, scale_data.surface_height);*/

			scale_data.dst_height_width_ratio =
          static_cast<float>(scale_data.surface_height)
          / scale_data.surface_width;
			scale_data.src_proportionate_width =
          static_cast<int>(console_width_p / scale_factor);
			scale_data.src_proportionate_height = static_cast<int>(
          (console_width_p * scale_data.dst_height_width_ratio)
          / scale_factor);

			/* Work out how much of the console to copy. */
			scale_data.src_x0 = static_cast<int>(
          (sdl->scale_xc * console_width_p)
          - (0.5f * scale_data.src_proportionate_width));
			if (scale_data.src_x0 + scale_data.src_proportionate_width > console_width_p)
				scale_data.src_x0 = console_width_p - scale_data.src_proportionate_width;
			if (scale_data.src_x0 < 0)
				scale_data.src_x0 = 0;
			scale_data.src_copy_width = scale_data.src_proportionate_width;
			if (scale_data.src_x0 + scale_data.src_copy_width > console_width_p)
				scale_data.src_copy_width = console_width_p - scale_data.src_x0;

			scale_data.src_y0 = static_cast<int>(
          (sdl->scale_yc * console_height_p)
          - (0.5f * scale_data.src_proportionate_height));
			if (scale_data.src_y0 + scale_data.src_proportionate_height > console_height_p)
				scale_data.src_y0 = console_height_p - scale_data.src_proportionate_height;
			if (scale_data.src_y0 < 0)
				scale_data.src_y0 = 0;
			scale_data.src_copy_height = scale_data.src_proportionate_height;
			if (scale_data.src_y0 + scale_data.src_copy_height > console_height_p)
				scale_data.src_copy_height = console_height_p - scale_data.src_y0;

			scale_data.dst_display_width = (scale_data.src_copy_width * scale_data.surface_width) / scale_data.src_proportionate_width;
			scale_data.dst_display_height = (scale_data.src_copy_height * scale_data.surface_height) / scale_data.src_proportionate_height;
			scale_data.dst_offset_x = (scale_data.surface_width - scale_data.dst_display_width)/2;
			scale_data.dst_offset_y = (scale_data.surface_height - scale_data.dst_display_height)/2;
		}

		SDL_RenderClear(renderer);
		actual_rendering();
		SDL_RenderPresent(renderer);
	}
#ifndef NO_OPENGL
	else {
		TCOD_opengl_render(oldFade, NULL, console, ensure_cache(console));
		TCOD_opengl_swap();
	}
#endif
  oldFade = TCOD_console_get_fade();
}

/* Return the current root console cache if it exists, or NULL. */
static struct TCOD_Console *get_root_console_cache(void){
	return root_console_cache;
}

static SDL_Surface *create_surface(int width, int height, bool with_alpha) {
	uint32_t rmask,gmask,bmask,amask;
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
	bitmap=SDL_CreateRGBSurface(flags,width,height,
		with_alpha ? 32:24,
		rmask,gmask,bmask,amask);
	if ( with_alpha ) {
		SDL_SetSurfaceAlphaMod(bitmap, 255);
	}
	return bitmap;
}

static void create_window(int w, int h, bool fullscreen) {
	uint32_t winflags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
#if defined(TCOD_ANDROID)
	/* Android should always be fullscreen. */
	TCOD_ctx.fullscreen = fullscreen = true;
#endif
	if ( fullscreen  ) {
		find_resolution();
#ifndef NO_OPENGL
		if (TCOD_ctx.renderer != TCOD_RENDERER_SDL ) {
			TCOD_opengl_init_attributes();
			winflags |= SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL;
#	if defined(TCOD_ANDROID) && defined(FUTURE_SUPPORT)
			winflags |= SDL_WINDOW_RESIZABLE;
#	endif
			window = SDL_CreateWindow(TCOD_ctx.window_title,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,TCOD_ctx.actual_fullscreen_width,TCOD_ctx.actual_fullscreen_height,winflags);
			if ( window && TCOD_opengl_init_state(w, h, charmap) && TCOD_opengl_init_shaders() ) {
				TCOD_LOG(("Using %s renderer...\n",TCOD_ctx.renderer == TCOD_RENDERER_GLSL ? "GLSL" : "OPENGL"));
			} else {
				TCOD_LOG(("Fallback to SDL renderer...\n"));
				TCOD_ctx.renderer = TCOD_RENDERER_SDL;
			}
		}
#endif
		if (TCOD_ctx.renderer == TCOD_RENDERER_SDL ) {
			winflags |= SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS;
#	if defined(TCOD_ANDROID) && defined(FUTURE_SUPPORT)
			winflags |= SDL_WINDOW_RESIZABLE;
#	endif
			window = SDL_CreateWindow(TCOD_ctx.window_title,SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, TCOD_ctx.actual_fullscreen_width,TCOD_ctx.actual_fullscreen_height,winflags);
			if ( window == NULL ) TCOD_fatal("SDL : cannot set fullscreen video mode: %s", SDL_GetError());
		}
		SDL_ShowCursor(0);
		SDL_GetWindowSize(window,&TCOD_ctx.actual_fullscreen_width,&TCOD_ctx.actual_fullscreen_height);
		TCOD_sys_init_screen_offset();
	} else {
#ifndef NO_OPENGL
		if (TCOD_ctx.renderer != TCOD_RENDERER_SDL ) {
			TCOD_opengl_init_attributes();
			winflags |= SDL_WINDOW_OPENGL;
			window = SDL_CreateWindow(TCOD_ctx.window_title,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,w*TCOD_ctx.font_width,h*TCOD_ctx.font_height,winflags);
			if ( window && TCOD_opengl_init_state(w, h, charmap) && TCOD_opengl_init_shaders() ) {
				TCOD_LOG(("Using %s renderer...\n",TCOD_ctx.renderer == TCOD_RENDERER_GLSL ? "GLSL" : "OPENGL"));
			} else {
				TCOD_LOG(("Fallback to SDL renderer...\n"));
				TCOD_ctx.renderer = TCOD_RENDERER_SDL;
			}
		}
#endif
		if (TCOD_ctx.renderer == TCOD_RENDERER_SDL ) {
			window = SDL_CreateWindow(TCOD_ctx.window_title,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,w*TCOD_ctx.font_width,h*TCOD_ctx.font_height,winflags);
			TCOD_LOG(("Using SDL renderer...\n"));
		}
		if ( window == NULL ) TCOD_fatal("Cannot create SDL window: %s", SDL_GetError());
	}
	if (TCOD_ctx.renderer == TCOD_RENDERER_SDL ) {
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if ( renderer == NULL ) TCOD_fatal("Cannot create SDL renderer: %s", SDL_GetError());
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	}
}

static void destroy_window(void) {
#ifndef NO_OPENGL
	if (TCOD_ctx.renderer == TCOD_RENDERER_OPENGL || TCOD_ctx.renderer == TCOD_RENDERER_GLSL) {
		TCOD_opengl_uninit_state();
	}
#endif
	if (scale_screen) {
		SDL_FreeSurface(scale_screen);
		scale_screen = NULL;
	}
	if (renderer) {
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
	}
	if (window) {
		SDL_DestroyWindow(window);
		window = NULL;
	}
  if (root_console_cache) {
    TCOD_console_delete(root_console_cache);
    root_console_cache = NULL;
  }
}

static void set_fullscreen(bool fullscreen) {
	bool mouseOn=SDL_ShowCursor(-1);
	if ( fullscreen ) {
		find_resolution();
		SDL_SetWindowFullscreen(window, fullscreen);
		SDL_ShowCursor(mouseOn ? 1:0);
		SDL_GetWindowSize(window,&TCOD_ctx.actual_fullscreen_width,&TCOD_ctx.actual_fullscreen_height);
		TCOD_sys_init_screen_offset();
	} else {
		SDL_SetWindowFullscreen(window, fullscreen);
		SDL_ShowCursor(mouseOn ? 1:0);
		TCOD_ctx.fullscreen_offsetx=0;
		TCOD_ctx.fullscreen_offsety=0;
	}
	TCOD_ctx.fullscreen=fullscreen;
	oldFade=-1; /* to redraw the whole screen */
}

static void set_window_title(const char *title) {
	SDL_SetWindowTitle(window, title);
}

static void save_screenshot(const char* filename)
{
  switch (TCOD_ctx.renderer) {
    case TCOD_RENDERER_SDL: {
      int width, height;
      SDL_GetRendererOutputSize(renderer, &width, &height);
      tcod::image::Image pixels(width, height);
      SDL_RenderReadPixels(
          renderer,
          nullptr,
          SDL_PIXELFORMAT_RGBA32,
          static_cast<void*>(pixels.data()),
          width * 4);
      tcod::image::save(pixels, filename);
      break;
    }
#ifndef NO_OPENGL
    case TCOD_RENDERER_OPENGL:
      SDL_Surface* screenshot = TCOD_opengl_get_screen();
      TCOD_sys_save_bitmap(screenshot, filename);
      SDL_FreeSurface(screenshot);
      break;
#endif
  }
}
/* get desktop resolution */
static void get_current_resolution(int *w, int *h) {
	int displayidx;
	SDL_Rect rect = { 0, 0, 0, 0 };
	if (window) {
		TCOD_IFNOT(window) return;
		displayidx = SDL_GetWindowDisplayIndex(window);
		TCOD_IFNOT(displayidx >= 0) return;
	} else {
		/* No window if no console, but user can want to know res before opening one. */
		TCOD_IFNOT(SDL_GetNumVideoDisplays() > 0) return;
		displayidx = 0;
	}
	TCOD_IFNOT(SDL_GetDisplayBounds(displayidx, &rect) == 0) return;
	*w=rect.w;
	*h=rect.h;
}

static void set_mouse_position(int x, int y) {
  SDL_WarpMouseInWindow(
      window, static_cast<uint16_t>(x), static_cast<uint16_t>(y));
}

static const char *get_clipboard_text(void) {
#ifdef TCOD_LINUX
	/*
		X11 clipboard is inaccessible without an open window.
		https://bugzilla.libsdl.org/show_bug.cgi?id=3222
	*/
	if (!window)
		return "";
#endif

	/*
	We hold onto the last clipboard text pointer SDL gave us.
	For C API callers it can be considered a borrowed reference.
	For Python ctypes API callers, the contents are copied into the Python string that is constructed from it.
	*/
	if (last_clipboard_text) {
		SDL_free(last_clipboard_text);
		last_clipboard_text = NULL;
	}
	last_clipboard_text = SDL_GetClipboardText();
	return last_clipboard_text;
}

static bool set_clipboard_text(const char *text) {
#ifdef TCOD_LINUX
	/*
	X11 clipboard is inaccessible without an open window.
	https://bugzilla.libsdl.org/show_bug.cgi?id=3222
	*/
	if (!window)
		return false;
#endif

	return SDL_SetClipboardText(text) == 0;
}

/* android compatible file access functions */
static bool file_read(const char *filename, unsigned char **buf, size_t *size) {
	size_t filesize;
	/* get file size */
	SDL_RWops *rwops= SDL_RWFromFile(filename,"rb");
	if (!rwops) return false;
	SDL_RWseek(rwops,0,RW_SEEK_END);
	filesize=SDL_RWtell(rwops);
	SDL_RWseek(rwops,0,RW_SEEK_SET);
	/* allocate buffer */
	*buf = static_cast<unsigned char*>(malloc(sizeof(unsigned char) * filesize));
	/* read from file */
	if (SDL_RWread(rwops, *buf, sizeof(unsigned char), filesize) != filesize) {
		SDL_RWclose(rwops);
		free(*buf);
		return false;
	}
	SDL_RWclose(rwops);
	*size=filesize;
	return true;
}

static bool file_exists(const char * filename) {
	SDL_RWops *rwops;
	rwops = SDL_RWFromFile(filename,"rb");
	if (rwops) {
		SDL_RWclose(rwops);
		return true;
	}
	return false;
}

static bool file_write(const char *filename, unsigned char *buf, uint32_t size) {
	SDL_RWops *rwops= SDL_RWFromFile(filename,"wb");
	if (!rwops) return false;
	SDL_RWwrite(rwops,buf,sizeof(unsigned char),size);
	SDL_RWclose(rwops);
	return true;
}

static void shutdown_(void) {
	if (last_clipboard_text) {
		SDL_free(last_clipboard_text);
		last_clipboard_text = NULL;
	}
	if (root_console_cache) {
		TCOD_console_delete(root_console_cache);
		root_console_cache = NULL;
		}
}

TCOD_SDL_driver_t *SDL_implementation_factory(void) {
	TCOD_SDL_driver_t *ret =
      static_cast<TCOD_SDL_driver_t *>(calloc(1, sizeof(TCOD_SDL_driver_t)));
    ret->scale_xc = 0.5f;
    ret->scale_yc = 0.5f;

    ret->get_closest_mode = get_closest_mode;
	ret->render = render;
	ret->create_surface = create_surface;
	ret->create_window = create_window;
	ret->destroy_window = destroy_window;
	ret->set_fullscreen = set_fullscreen;
	ret->set_window_title = set_window_title;
	ret->save_screenshot = save_screenshot;
	ret->get_current_resolution = get_current_resolution;
	ret->set_mouse_position = set_mouse_position;
	ret->get_clipboard_text = get_clipboard_text;
	ret->set_clipboard_text = set_clipboard_text;
	ret->file_read = file_read;
	ret->file_exists = file_exists;
	ret->file_write = file_write;
	ret->shutdown = shutdown_;
	ret->get_root_console_cache = get_root_console_cache;
	return ret;
}

#endif /* TCOD_BARE */
