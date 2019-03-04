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
#include "noise.hpp"

#include <stdlib.h>
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

void TCODNoise::setType(TCOD_noise_type_t type) {
	TCOD_noise_set_type(data,type);
}

float TCODNoise::get (float *f, TCOD_noise_type_t type) {
	if (type == TCOD_NOISE_DEFAULT) return TCOD_noise_get(data,f);
	else return TCOD_noise_get_ex(data,f,type);
}

float TCODNoise::getFbm (float *f, float octaves, TCOD_noise_type_t type) {
	if (type == TCOD_NOISE_DEFAULT) return TCOD_noise_get_fbm(data,f,octaves);
	else return TCOD_noise_get_fbm_ex(data,f,octaves,type);
}

float TCODNoise::getTurbulence (float *f, float octaves, TCOD_noise_type_t type) {
	if (type == TCOD_NOISE_DEFAULT) return TCOD_noise_get_turbulence(data,f,octaves);
	else return TCOD_noise_get_turbulence_ex(data,f,octaves,type);
}

TCODNoise::~TCODNoise() {
	TCOD_noise_delete(data);
}
