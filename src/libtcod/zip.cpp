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
#include "zip.hpp"

TCODZip::TCODZip() {
	data=TCOD_zip_new();
}

TCODZip::~TCODZip() {
	TCOD_zip_delete(data);
}

void TCODZip::putChar(char val) {
	TCOD_zip_put_char(data,val);
}

void TCODZip::putInt(int val) {
	TCOD_zip_put_int(data,val);
}

void TCODZip::putFloat(float val) {
	TCOD_zip_put_float(data,val);
}

void TCODZip::putString(const char *val) {
	TCOD_zip_put_string(data,val);
}

void TCODZip::putData(int nbBytes, const void *pdata) {
	TCOD_zip_put_data(data,nbBytes,pdata);
}

void TCODZip::putColor(const TCODColor *val) {
	TCOD_color_t col;
	col.r=val->r;
	col.g=val->g;
	col.b=val->b;
	TCOD_zip_put_color(data,col);
}

#ifdef TCOD_IMAGE_SUPPORT
void TCODZip::putImage(const TCODImage *val) {
	TCOD_zip_put_image(data,val->data);
}
#endif

#ifdef TCOD_CONSOLE_SUPPORT
void TCODZip::putConsole(const TCODConsole *val) {
	TCOD_zip_put_console(data,val->data);
}
#endif

int TCODZip::saveToFile(const char *filename) {
	return TCOD_zip_save_to_file(data,filename);
}

int TCODZip::loadFromFile(const char *filename) {
	return TCOD_zip_load_from_file(data,filename);
}

char TCODZip::getChar() {
	return TCOD_zip_get_char(data);
}

int TCODZip::getInt() {
	return TCOD_zip_get_int(data);
}

float TCODZip::getFloat() {
	return TCOD_zip_get_float(data);
}

const char *TCODZip::getString() {
	return TCOD_zip_get_string(data);
}

int TCODZip::getData(int nbBytes, void *pdata) {
	return TCOD_zip_get_data(data,nbBytes,pdata);
}

TCODColor TCODZip::getColor() {
	return TCODColor(TCOD_zip_get_color(data));
}

#ifdef TCOD_IMAGE_SUPPORT
TCODImage *TCODZip::getImage() {
	return new TCODImage(TCOD_zip_get_image(data));
}
#endif

#ifdef TCOD_CONSOLE_SUPPORT
TCODConsole *TCODZip::getConsole() {
	return new TCODConsole(TCOD_zip_get_console(data));
}
#endif

uint32_t TCODZip::getCurrentBytes() const {
	return TCOD_zip_get_current_bytes(data);
}

uint32_t TCODZip::getRemainingBytes() const {
	return TCOD_zip_get_remaining_bytes(data);
}

void TCODZip::skipBytes(uint32_t nbBytes) {
	TCOD_zip_skip_bytes(data,nbBytes);
}
