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

#if defined (__HAIKU__) || defined (__ANDROID__)
#include <SDL.h>
#elif defined(TCOD_SDL2)
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#endif
#include <png.h>
#include "libtcod.h"
#include "libtcod_int.h"

#define png_infopp_NULL (png_infopp)NULL
#if defined( TCOD_MACOSX ) || defined( TCOD_HAIKU ) 
#define int_p_NULL (int*)NULL
#endif

/* Redirected png IO through SDL's rwops mechanism. */
static void png_read_data(png_structp png_ptr, png_bytep data, png_size_t length) {
	SDL_RWops *rwops = png_get_io_ptr(png_ptr);
	rwops->read(rwops,data,length,1);
}
static void png_write_data(png_structp png_ptr, png_bytep data, png_size_t length) {
	SDL_RWops *rwops = png_get_io_ptr(png_ptr);
	rwops->write(rwops,data,length,1);
}
static void png_flush_data(png_structp png_ptr) {}


bool TCOD_sys_check_png(const char *filename) {
	static uint8 magic_number[]={137, 80, 78, 71, 13, 10, 26, 10};
	return TCOD_sys_check_magic_number(filename,sizeof(magic_number),magic_number);
}

SDL_Surface *TCOD_sys_read_png(const char *filename) {
	png_uint_32 png_width,png_height,y;
	int png_bit_depth,png_color_type,png_interlace_type;
	png_structp png_ptr;
	png_infop info_ptr;
#if SDL_VERSION_ATLEAST(2,0,0)
	SDL_RWops *rwops;
#else
	FILE *fp;
#endif
	SDL_Surface *bitmap;
	png_bytep *row_pointers;

#if SDL_VERSION_ATLEAST(2,0,0)
	if ((rwops = SDL_RWFromFile(filename, "rb")) == NULL)
#else
	if ((fp = fopen(filename, "rb")) == NULL)
#endif
		return NULL;
	/* Create and initialize the png_struct with the desired error handler
	* functions.  If you want to use the default stderr and longjump method,
	* you can supply NULL for the last three parameters.  We also supply the
	* the compiler header file version, so that we know if the application
	* was compiled with a compatible version of the library.  REQUIRED
	*/
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL, NULL, NULL);

	if (png_ptr == NULL)
	{
#if SDL_VERSION_ATLEAST(2,0,0)
		rwops->close(rwops);
#else
		fclose(fp);
#endif
		return NULL;
	}

	/* Allocate/initialize the memory for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
#if SDL_VERSION_ATLEAST(2,0,0)
		rwops->close(rwops);
#else
		fclose(fp);
#endif
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}

	/* Set error handling if you are using the setjmp/longjmp method (this is
	* the normal method of doing things with libpng).  REQUIRED unless you
	* set up your own error handlers in the png_create_read_struct() earlier.
	*/

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		/* Free all of the memory associated with the png_ptr and info_ptr */
		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
#if SDL_VERSION_ATLEAST(2,0,0)
		rwops->close(rwops);
#else
		fclose(fp);
#endif
		/* If we get here, we had a problem reading the file */
		return NULL;
	}

#if SDL_VERSION_ATLEAST(2,0,0)
	png_set_read_fn(png_ptr, (voidp)rwops, (png_rw_ptr)png_read_data);
#else
	png_init_io(png_ptr, fp);
#endif

	/*
	* If you have enough memory to read in the entire image at once,
	* and you need to specify only transforms that can be controlled
	* with one of the PNG_TRANSFORM_* bits (this presently excludes
	* dithering, filling, setting background, and doing gamma
	* adjustment), then you can read the entire image (including
	* pixels) into the info structure with this call:
	*/
	/*png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, png_voidp_NULL); */

	/* get info about the image */
	png_read_info(png_ptr,info_ptr);
	png_get_IHDR(png_ptr,info_ptr,&png_width,&png_height,&png_bit_depth,&png_color_type,
		&png_interlace_type,NULL,NULL);

	/* convert the image to a format suitable with libtcod */
	png_set_strip_16(png_ptr); /* 16 bits channels => 8 bits channels */
	png_set_packing(png_ptr); /* 1,2,4 bits depth => 24/32 bits depth */
	if ( png_color_type == PNG_COLOR_TYPE_GRAY ) png_set_expand(png_ptr); /* grayscale => color */
	if ( png_color_type == PNG_COLOR_TYPE_GRAY_ALPHA ) png_set_gray_to_rgb(png_ptr);

	/* update the image information */
	png_read_update_info(png_ptr,info_ptr);
	png_get_IHDR(png_ptr,info_ptr,&png_width,&png_height,&png_bit_depth,&png_color_type,
		&png_interlace_type,NULL,NULL);

	/* create the SDL surface */
	bitmap=TCOD_sys_get_surface(png_width,png_height,(int)png_get_channels(png_ptr, info_ptr)== 4);

	/* get row data */
	row_pointers=(png_bytep *)malloc(sizeof(png_bytep)*png_height);
	for (y=0; y<  png_height; y++ ) {
		row_pointers[y]=(png_bytep)(Uint8 *)(bitmap->pixels) + y * bitmap->pitch;
	}

	/* read png data directly into the SDL surface */
	png_read_image(png_ptr,row_pointers);

	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
	free(row_pointers);

	/* close the file */
#if SDL_VERSION_ATLEAST(2,0,0)
	rwops->close(rwops);
#else
	fclose(fp);
#endif
	return bitmap;
}

void TCOD_sys_write_png(const SDL_Surface *surf, const char *filename) {
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep *row_pointers;
	int y,x;
#if SDL_VERSION_ATLEAST(2,0,0)
	SDL_RWops *rwops = SDL_RWFromFile(filename, "wb");
	if (!rwops) return;
#else
	FILE *fp=fopen(filename,"wb");
	if (!fp) return;
#endif

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL, NULL, NULL);

	if (png_ptr == NULL)
	{
#if SDL_VERSION_ATLEAST(2,0,0)
		rwops->close(rwops);
#else
		fclose(fp);
#endif
		return;
	}

	/* Allocate/initialize the memory for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
#if SDL_VERSION_ATLEAST(2,0,0)
		rwops->close(rwops);
#else
		fclose(fp);
#endif
		png_destroy_write_struct(&png_ptr, png_infopp_NULL);
		return;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		/* Free all of the memory associated with the png_ptr and info_ptr */
		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
#if SDL_VERSION_ATLEAST(2,0,0)
		rwops->close(rwops);
#else
		fclose(fp);
#endif
		/* If we get here, we had a problem reading the file */
		return;
	}

#if SDL_VERSION_ATLEAST(2,0,0)
	png_set_write_fn(png_ptr, (png_rw_ptr)rwops, (png_rw_ptr)png_write_data, (png_flush_ptr)png_flush_data);
#else
	png_init_io(png_ptr, fp);
#endif

	png_set_IHDR(png_ptr,info_ptr,surf->w, surf->h,
		8,PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	/* get row data */
	row_pointers=(png_bytep *)malloc(sizeof(png_bytep)*surf->h);

	for (y=0; y<  surf->h; y++ ) {
/*		TODO : we should be able to use directly the surface data... */
/*		row_pointers[y]=(png_bytep)(Uint8 *)(surf->pixels) + y * surf->pitch; */
		row_pointers[y]=(png_bytep)malloc(sizeof(png_byte)*surf->w*3);
		for (x=0; x < surf->w; x++ ) {
			Uint8 *pixel=(Uint8 *)(surf->pixels) + y * surf->pitch + x * surf->format->BytesPerPixel;
			row_pointers[y][x*3]=*((pixel)+surf->format->Rshift/8);
			row_pointers[y][x*3+1]=*((pixel)+surf->format->Gshift/8);
			row_pointers[y][x*3+2]=*((pixel)+surf->format->Bshift/8);
		}
	}

	png_set_rows(png_ptr,info_ptr,row_pointers);

	png_write_png(png_ptr,info_ptr,PNG_TRANSFORM_IDENTITY,NULL);

#if SDL_VERSION_ATLEAST(2,0,0)
	rwops->close(rwops);
#else
	fclose(fp);
#endif
	/* clean up, and free any memory allocated - REQUIRED */
	png_destroy_write_struct(&png_ptr, &info_ptr);

	for (y=0; y<  surf->h; y++ ) {
		free(row_pointers[y]);
	}
	free(row_pointers);
}

