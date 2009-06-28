/*
* libtcod 1.4.1
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

#ifndef _TCOD_PERLIN_HPP
#define _TCOD_PERLIN_HPP

class TCODLIB_API TCODNoise {
	public :
		TCODNoise(int dimensions);
		TCODNoise(int dimensions, TCODRandom *random);
		TCODNoise(int dimensions, float hurst, float lacunarity);
		TCODNoise(int dimensions, float hurst, float lacunarity, TCODRandom *random);
		// perlin noise and derived
		float getPerlin(float *f) const;
		float getFbmPerlin(float *f, float octaves) const;
		float getTurbulencePerlin(float *f, float octaves) const;

		// simplex noise and derived
		float getSimplex(float *f) const;
		float getFbmSimplex(float *f, float octaves) const;
		float getTurbulenceSimplex(float *f, float octaves) const;

		// wavelet noise and derived
		float getWavelet(float *f) const;
		float getFbmWavelet(float *f, float octaves) const;
		float getTurbulenceWavelet(float *f, float octaves) const;

		virtual ~TCODNoise();

	protected :
		friend class TCODLIB_API TCODHeightMap;		
		TCOD_noise_t data;
};

#endif
