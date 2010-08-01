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
#include <stdlib.h>
#include "libtcod.hpp"
#include "libtcod_int.h"

static TCODRandom *instance=(TCODRandom *)NULL;

TCODRandom *TCODRandom::getInstance(void) {
	if (! instance ) {
		instance=new TCODRandom(TCOD_RNG_CMWC,true);
	}
	return instance;
}

TCODRandom::TCODRandom(TCOD_random_algo_t algo, bool allocate) {
	if ( allocate ) data = TCOD_random_new(algo);
}

TCODRandom::TCODRandom(uint32 seed, TCOD_random_algo_t algo) {
	data=TCOD_random_new_from_seed(algo, seed);
}

int TCODRandom::getInt(int min, int max) {
	return TCOD_random_get_int(data,min,max);
}

float TCODRandom::getFloat(float min, float max) {
	return TCOD_random_get_float(data,min,max);
}

TCODRandom::~TCODRandom() {
	TCOD_random_delete(data);
}

TCODRandom *TCODRandom::save() const {
	TCODRandom *ret=new TCODRandom(((mersenne_data_t *)data)->algo,false);
	ret->data=TCOD_random_save(data);
	return ret;
}

void TCODRandom::restore(const TCODRandom *backup) {
	TCOD_random_restore(data,backup->data);
}

double TCODRandom::getGaussianDouble (double mean, double stdDeviation) {
	return TCOD_random_get_gaussian_double(data, mean, stdDeviation);
}

float TCODRandom::getGaussianFloat (float mean, float stdDeviation) {
	return (float)TCOD_random_get_gaussian_double(data, (double)mean, (double)stdDeviation);
}

int TCODRandom::getGaussianInt (int mean, int stdDeviation) {
	double num = TCOD_random_get_gaussian_double(data,(double)mean,(double)stdDeviation);
	return (num >= 0.0 ? (int)(num + 0.5) : (int)(num - 0.5));
}

double TCODRandom::getGaussianRangeDouble (double min, double max) {
	return TCOD_random_get_gaussian_double_range(data,min,max);
}

float TCODRandom::getGaussianRangeFloat (float min, float max) {
	return (float)TCOD_random_get_gaussian_double_range(data,(double)min,(double)max);
}

int TCODRandom::getGaussianRangeInt (int min, int max) {
	return TCOD_random_get_gaussian_int_range(data,min,max);
}

double TCODRandom::getGaussianRangeDoubleCustom (double min, double max, double mean) {
	return TCOD_random_get_gaussian_double_range_custom(data,min,max,mean);
}

float TCODRandom::getGaussianRangeFloatCustom (float min, float max, float mean) {
	return (float)TCOD_random_get_gaussian_double_range_custom(data,(double)min,(double)max,(double)mean);
}

int TCODRandom::getGaussianRangeIntCustom (int min, int max, int mean) {
	return TCOD_random_get_gaussian_int_range_custom(data,min,max,mean);
}

