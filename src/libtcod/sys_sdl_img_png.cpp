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

#if !defined (__HAIKU__) && !defined (__ANDROID__)
#include <stdlib.h>
#include <stdio.h>
#endif
#include <SDL.h>

#include "../vendor/lodepng.h"

#include "libtcod_int.h"
bool TCOD_sys_check_png(const char *filename) {
	static uint8_t magic_number[]={137, 80, 78, 71, 13, 10, 26, 10};
	return TCOD_sys_check_magic_number(filename,sizeof(magic_number),magic_number);
}

SDL_Surface *TCOD_sys_read_png(const char *filename) {
	unsigned error;
	unsigned char* image;
	unsigned width, height, y, bpp;
	unsigned char* png;
	size_t pngsize;
	LodePNGState state;
	SDL_Surface *bitmap;
	unsigned char *source;
	unsigned int rowsize;

	lodepng_state_init(&state);
	/*optionally customize the state*/
	if (!TCOD_sys_read_file(filename,&png,&pngsize)) return NULL;

	lodepng_inspect(&width,&height,&state, png, pngsize);
	bpp=lodepng_get_bpp(&state.info_png.color);

	if ( bpp == 24 ) {
		/* don't convert to 32 bits because libtcod's 24bits renderer is faster */
		state.info_raw.colortype=LCT_RGB;
	} else if (  bpp != 24 && bpp != 32 ) {
		/* paletted png. convert to 24 bits */
		state.info_raw.colortype=LCT_RGB;
		state.info_raw.bitdepth=8;
		bpp=24;
	}
	error = lodepng_decode(&image, &width, &height, &state, png, pngsize);
	free(png);
	if(error) {
		printf("error %u: %s\n", error, lodepng_error_text(error));
		lodepng_state_cleanup(&state);
		return NULL;
	}

	/* create the SDL surface */
	bitmap = TCOD_sys_get_surface(width, height, bpp==32);
	source=image;
	rowsize=width*bpp/8;
	for (y=0; y<  height; y++ ) {
		uint8_t* row_pointer = static_cast<uint8_t*>(bitmap->pixels)
        + y * bitmap->pitch;
		memcpy(row_pointer,source,rowsize);
		source+=rowsize;
	}

	lodepng_state_cleanup(&state);
	free(image);
	return bitmap;
}

void TCOD_sys_write_png(SDL_Surface *surf, const char *filename) {
	unsigned char *image, *dest=static_cast<unsigned char *>(malloc(surf->h * surf->w * 3 * sizeof(char)));
	int x,y;
	unsigned char *buf;
	size_t size;
	int error;
	/* SDL uses 32bits format without alpha layer for screen. convert it to 24 bits */
	image=dest;
	for (y=0; y<  surf->h; y++ ) {
		for (x=0; x < surf->w; x++ ) {
			uint8_t* pixel = static_cast<uint8_t*>(surf->pixels)
          + y * surf->pitch + x * surf->format->BytesPerPixel;
			*dest++=*((pixel)+surf->format->Rshift/8);
			*dest++=*((pixel)+surf->format->Gshift/8);
			*dest++=*((pixel)+surf->format->Bshift/8);
		}
	}
	error=lodepng_encode_memory(&buf,&size,image,surf->w,surf->h,LCT_RGB,8);
	free(image);
	if ( ! error ) {
		TCOD_sys_write_file(filename, buf, static_cast<uint32_t>(size));
		free(buf);
	} else {
		printf("error %u: %s\n", error, lodepng_error_text(error));
	}
}
#endif /* TCOD_BARE */
