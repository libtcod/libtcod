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
#include "image.hpp"

#ifdef TCOD_IMAGE_SUPPORT

TCODImage::TCODImage(const char *filename) : deleteData(true) {
  data = TCOD_image_load(filename);
}

TCODImage::TCODImage(int width, int height) : deleteData(true) {
  data = TCOD_image_new(width,height);
}

#ifdef TCOD_CONSOLE_SUPPORT
TCODImage::TCODImage(const TCODConsole *con) {
  data = TCOD_image_from_console(con->data);
}
#endif

void TCODImage::clear(const TCODColor col) {
	TCOD_color_t ccol;
	ccol.r=col.r;
	ccol.g=col.g;
	ccol.b=col.b;
	TCOD_image_clear(data,ccol);
}

void TCODImage::getSize(int *w,int *h) const {
	TCOD_image_get_size(data,w,h);
}

TCODImage::~TCODImage() {
	if ( deleteData ) TCOD_image_delete(data);
}

TCODColor TCODImage::getPixel(int x, int y) const {
	return TCOD_image_get_pixel(data,x,y);
}

int TCODImage::getAlpha(int x,int y) const {
	return TCOD_image_get_alpha(data,x,y);
}

TCODColor TCODImage::getMipmapPixel(float x0,float y0, float x1, float y1) {
	return TCOD_image_get_mipmap_pixel(data,x0,y0,x1,y1);
}

void TCODImage::putPixel(int x, int y, const TCODColor col) {
	TCOD_color_t ccol = {col.r,col.g,col.b};
	TCOD_image_put_pixel(data,x,y,ccol);
}

#ifdef TCOD_CONSOLE_SUPPORT
void TCODImage::blit(TCODConsole *console, float x, float y, TCOD_bkgnd_flag_t bkgnd_flag, float scalex, float scaley, float angle) const {
	TCOD_image_blit(data,console->data,x,y,bkgnd_flag,scalex,scaley,angle);
}

void TCODImage::blitRect(TCODConsole *console, int x, int y, int w, int h, TCOD_bkgnd_flag_t bkgnd_flag) const {
	TCOD_image_blit_rect(data,console->data,x,y,w,h,bkgnd_flag);
}
#endif /* TCOD_CONSOLE_SUPPORT */

void TCODImage::save(const char *filename) const {
	TCOD_image_save(data,filename);
}

void TCODImage::setKeyColor(const TCODColor keyColor) {
	TCOD_color_t ccol = {keyColor.r,keyColor.g,keyColor.b};
	TCOD_image_set_key_color(data,ccol);
}

bool TCODImage::isPixelTransparent(int x, int y) const {
	return TCOD_image_is_pixel_transparent(data,x,y) != 0;
}

#ifdef TCOD_CONSOLE_SUPPORT
void TCODImage::refreshConsole(const TCODConsole *console) {
	TCOD_image_refresh_console(data,console->data);
}
#endif /* TCOD_CONSOLE_SUPPORT */

void TCODImage::invert() {
	TCOD_image_invert(data);
}

void TCODImage::hflip() {
	TCOD_image_hflip(data);
}

void TCODImage::rotate90(int numRotations) {
	TCOD_image_rotate90(data,numRotations);
}

void TCODImage::vflip() {
	TCOD_image_vflip(data);
}

void TCODImage::scale(int neww, int newh) {
	TCOD_image_scale(data,neww,newh);
}

#ifdef TCOD_CONSOLE_SUPPORT
void TCODImage::blit2x(TCODConsole *dest, int dx, int dy, int sx, int sy, int w, int h) const {
	TCOD_image_blit_2x(data,dest->data,dx,dy,sx,sy,w,h);
}
#endif /* TCOD_CONSOLE_SUPPORT */

#endif /* TCOD_IMAGE_SUPPORT */
