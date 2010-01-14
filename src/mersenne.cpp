/*
* libtcod 1.5.0
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
#include <stdlib.h>
#include "libtcod.hpp"

static TCODRandom *instance=(TCODRandom *)NULL;

TCODRandom *TCODRandom::getInstance() {
	if (! instance ) {
		instance=new TCODRandom();
	}
	return instance;
}

TCODRandom::TCODRandom(bool allocate) {
	if ( allocate ) data = TCOD_random_new();
}

TCODRandom::TCODRandom(uint32 seed) {
	data=TCOD_random_new_from_seed(seed);
}

int TCODRandom::getInt(int min, int max) {
	return TCOD_random_get_int(data,min,max);
}

float TCODRandom::getFloat(float min, float max) {
	return TCOD_random_get_float(data,min,max);
}

int TCODRandom::getIntFromByteArray(int min, int max, const char *data,int len) {
	return TCOD_random_get_int_from_byte_array(min,max,data,len);
}

TCODRandom::~TCODRandom() {
	TCOD_random_delete(data);
}

TCODRandom *TCODRandom::save() const {
	TCODRandom *ret=new TCODRandom(false);
	ret->data=TCOD_random_save(data);
	return ret;
}

void TCODRandom::restore(const TCODRandom *backup) {
	TCOD_random_restore(data,backup->data);
}

