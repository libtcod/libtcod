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
TCODNoise::TCODNoise(int dimensions, TCOD_noise_type_t type) {
	data = TCOD_noise_new(dimensions, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, TCODRandom::getInstance()->data);
	TCOD_noise_set_type(data,type);
}

TCODNoise::TCODNoise(int dimensions, TCODRandom *random, TCOD_noise_type_t type) {
	data = TCOD_noise_new(dimensions, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, random->data);
	TCOD_noise_set_type(data,type);
}

TCODNoise::TCODNoise(int dimensions, float hurst, float lacunarity, TCOD_noise_type_t type) {
	data = TCOD_noise_new(dimensions, hurst, lacunarity, TCODRandom::getInstance()->data);
	TCOD_noise_set_type(data,type);
}

TCODNoise::TCODNoise(int dimensions, float hurst, float lacunarity, TCODRandom *random, TCOD_noise_type_t type) {
	data = TCOD_noise_new(dimensions, hurst, lacunarity, random->data);
	TCOD_noise_set_type(data,type);
}

float TCODNoise::getPerlin(float *f) const {
	return TCOD_noise_perlin(data,f);
}

float TCODNoise::getFbmPerlin(float *f, float octaves) const {
	return TCOD_noise_fbm_perlin(data,f,octaves);
}

float TCODNoise::getTurbulencePerlin(float *f, float octaves) const {
	return TCOD_noise_turbulence_perlin(data,f,octaves);
}

float TCODNoise::getSimplex(float *f) const {
	return TCOD_noise_simplex(data,f);
}

float TCODNoise::getFbmSimplex(float *f, float octaves) const {
	return TCOD_noise_fbm_simplex(data,f,octaves);
}

float TCODNoise::getTurbulenceSimplex(float *f, float octaves) const {
	return TCOD_noise_turbulence_simplex(data,f,octaves);
}

float TCODNoise::getWavelet(float *f) const {
	return TCOD_noise_wavelet(data,f);
}

float TCODNoise::getFbmWavelet(float *f, float octaves) const {
	return TCOD_noise_fbm_wavelet(data,f,octaves);
}

float TCODNoise::getTurbulenceWavelet(float *f, float octaves) const {
	return TCOD_noise_turbulence_wavelet(data,f,octaves);
}

void TCODNoise::setType(TCOD_noise_type_t type) {
	TCOD_noise_set_type(data,type);
}

float TCODNoise::get (float *f) {
	return TCOD_noise_get(data,f);
}

float TCODNoise::getFbm (float *f, float octaves) {
	return TCOD_noise_get_fbm(data,f,octaves);
}

float TCODNoise::getTurbulence (float *f, float octaves) {
	return TCOD_noise_get_turbulence(data,f,octaves);
}

TCODNoise::~TCODNoise() {
	TCOD_noise_delete(data);
}
