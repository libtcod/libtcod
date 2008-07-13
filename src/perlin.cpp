/*
* libtcod 1.3.2
* Copyright (c) 2007,2008 J.C.Wilk
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of J.C.Wilk may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY J.C.WILK ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL J.C.WILK BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdlib.h>
#include "libtcod.hpp"
TCODNoise::TCODNoise(int dimensions) {
	data = TCOD_noise_new(dimensions, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, TCODRandom::getInstance()->data);
}

TCODNoise::TCODNoise(int dimensions, TCODRandom *random) {
	data = TCOD_noise_new(dimensions, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, random->data);
}

TCODNoise::TCODNoise(int dimensions, float hurst, float lacunarity) {
	data = TCOD_noise_new(dimensions, hurst, lacunarity, TCODRandom::getInstance()->data);
}

TCODNoise::TCODNoise(int dimensions, float hurst, float lacunarity, TCODRandom *random) {
	data = TCOD_noise_new(dimensions, hurst, lacunarity, random->data);
}

float TCODNoise::getNoise(float *f) const {
	return TCOD_noise_get(data,f);
}

float TCODNoise::getFbm(float *f, float octaves) const {
	return TCOD_noise_fbm(data,f,octaves);
}

float TCODNoise::getTurbulence(float *f, float octaves) const {
	return TCOD_noise_turbulence(data,f,octaves);
}

TCODNoise::~TCODNoise() {
	TCOD_noise_delete(data);
}
