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

#ifndef _TCOD_PERLIN_HPP
#define _TCOD_PERLIN_HPP

#include "noise_defaults.h"

/**
 @PageName noise
 @PageCategory Base toolkits
 @PageTitle Noise generator
 @PageDesc This toolkit provides several functions to generate Perlin noise and other derived noises. It can handle noise functions from 1 to 4 dimensions.
 @FuncDesc
	Usage example:
	1D noise : the variation of a torch intensity
	2D fbm : heightfield generation or clouds
	3D fbm : animated smoke
	If you don't know what is Perlin noise and derived functions, or what is the influence of the different fractal parameters, check the Perlin noise sample included with the library.
	<table width="800px" class="none">
		<tr><td align="center">Simplex noise, fbm, turbulence</td>
		<td align="center"><img border="1" src="simplex.png"></td>
		<td align="center"><img border="1" src="fbm_simplex.png"></td>
		<td align="center"><img border="1" src="turbulence_simplex.png"></td></tr>
		<tr><td align="center">Perlin noise, fbm, turbulence</td>
		<td align="center"><img border="1" src="perlin.png"></td>
		<td align="center"><img border="1" src="fbm_perlin.png"></td>
		<td align="center"><img border="1" src="turbulence_perlin.png"></td></tr>
		<tr><td align="center">Wavelet noise, fbm, turbulence</td>
		<td align="center"><img border="1" src="wavelet.png"></td>
		<td align="center"><img border="1" src="fbm_wavelet.png"></td>
		<td align="center"><img border="1" src="turbulence_wavelet.png"></td></tr>
	</table>
	<h6>Noise functions relative times</h6>

	For example, in 4D, Perlin noise is 17 times slower than simplex noise.
	<table border="1">
		<tr><td></td><td>1D</td><td>2D</td><td>3D</td><td>4D</td></tr>
		<tr><td>simplex</td><td>1</td><td>1</td><td>1</td><td>1</td></tr>
		<tr><td>Perlin</td><td>1.3</td><td>4</td><td>5</td><td>17</td></tr>
		<tr><td>wavelet</td><td>53</td><td>32</td><td>14</td><td>X</td></tr>
	</table>
 */
class TCODLIB_API TCODNoise {
	public :
		/**
		@PageName noise_init
		@PageFather noise
		@PageTitle Creating a noise generator
		@FuncDesc Those functions initialize a noise generator from a number of dimensions (from 1 to 4), some fractal parameters and a random number generator.
			The C++ version provides several constructors. When the hurst and lacunarity parameters are omitted, default values (TCOD_NOISE_DEFAULT_HURST = 0.5f and TCOD_NOISE_DEFAULT_LACUNARITY = 2.0f) are used.
		@Cpp
			TCODNoise::TCODNoise(int dimensions, TCOD_noise_type_t type = TCOD_NOISE_DEFAULT)
			TCODNoise::TCODNoise(int dimensions, TCODRandom *random, TCOD_noise_type_t type = TCOD_NOISE_DEFAULT)
			TCODNoise::TCODNoise(int dimensions, float hurst, float lacunarity, TCOD_noise_type_t type = TCOD_NOISE_DEFAULT)
			TCODNoise::TCODNoise(int dimensions, float hurst, float lacunarity, TCODRandom *random, TCOD_noise_type_t type = TCOD_NOISE_DEFAULT)
		@C TCOD_noise_t TCOD_noise_new(int dimensions, float hurst, float lacunarity, TCOD_random_t random)
		@Py noise_new(dimensions, hurst=TCOD_NOISE_DEFAULT_HURST, lacunarity=TCOD_NOISE_DEFAULT_LACUNARITY, random=0)
		@C#
			TCODNoise::TCODNoise(int dimensions)
			TCODNoise::TCODNoise(int dimensions, TCODRandom random)
			TCODNoise::TCODNoise(int dimensions, float hurst, float lacunarity)
			TCODNoise::TCODNoise(int dimensions, float hurst, float lacunarity, TCODRandom random)
		@Param dimensions	From 1 to 4.
		@Param hurst	For fractional brownian motion and turbulence, the fractal Hurst exponent. You can use the default value TCOD_NOISE_DEFAULT_HURST = 0.5f.
		@Param lacunarity	For fractional brownian motion and turbulence, the fractal lacunarity. You can use the default value TCOD_NOISE_DEFAULT_LACUNARITY = 2.0f.
		@Param random	A random number generator obtained with the Mersenne twister toolkit or NULL to use the default random number generator.
		@CppEx
			// 1 dimension generator
			TCODNoise * noise1d = new TCODNoise(1);
			// 2D noise with a predefined random number generator
			TCODRandom *myRandom = new TCODRandom();
			TCODNoise *noise2d = new TCODNoise(2,myRandom);
			// a 3D noise generator with a specific fractal parameters
			TCODNoise *noise3d = new TCODNoise(3,0.7f,1.4f);
		@CEx
			// 1 dimension generator
			TCOD_noise_t noise1d = TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			// 2D noise with a predefined random number generator
			TCOD_random_t my_random = TCOD_random_new();
			TCOD_noise_t noise2d = TCOD_noise_new(2,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,my_random);
			// a 3D noise generator with a specific fractal parameters
			TCOD_noise_t noise3d = TCOD_noise_new(3,0.7f, 1.4f,NULL);
		@PyEx
			# 1 dimension generator
			noise1d = libtcod.noise_new(1)
			# 2D noise with a predefined random number generator
			my_random = libtcod.random_new();
			noise2d = libtcod.noise_new(2,libtcod.NOISE_DEFAULT_HURST, libtcod.NOISE_DEFAULT_LACUNARITY,my_random)
			# a 3D noise generator with a specific fractal parameters
			noise3d = libtcod.noise_new(3, 0.7, 1.4)
		*/
		TCODNoise(int dimensions, TCOD_noise_type_t type = TCOD_NOISE_DEFAULT);
		TCODNoise(int dimensions, TCODRandom *random, TCOD_noise_type_t type = TCOD_NOISE_DEFAULT);
		TCODNoise(int dimensions, float hurst, float lacunarity, TCOD_noise_type_t type = TCOD_NOISE_DEFAULT);
		TCODNoise(int dimensions, float hurst, float lacunarity, TCODRandom *random, TCOD_noise_type_t type = TCOD_NOISE_DEFAULT);

		/**
		@PageName noise_init
		@FuncDesc To release ressources used by a generator, use those functions :
		@Cpp TCODNoise::~TCODNoise()
		@C void TCOD_noise_delete(TCOD_noise_t noise)
		@Py noise_delete(noise)
		@C# void TCODNoise::Dispose()
		@Param noise	In the C and python versions, the generator handler, returned by the initialization function.
		@CppEx
			// create a generator
			TCODNoise *noise = new TCODNoise(2);
			// use it
			...
			// destroy it
			delete noise;
		@CEx
			// create a generator
			TCOD_noise_t noise = TCOD_noise_new(2,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAUT_LACUNARITY, NULL);
			// use it
			...
			// destroy it
			TCOD_noise_delete(noise);
		@PyEx
			# create a generator
			noise = libtcod.noise_new(2,litbcod.NOISE_DEFAULT_HURST, litbcod.NOISE_DEFAUT_LACUNARITY, 0)
			# use it
			...
			# destroy it
			litbcod.noise_delete(noise)
		*/
		virtual ~TCODNoise();

   		/**
   		@PageName noise_simplex
		@PageFather noise
		@PageTitle Getting simplex noise
		@FuncTitle Getting flat simplex noise
		@FuncDesc This function returns the simplex noise function value between -1.0 and 1.0 at given coordinates.
			The simplex noise is much faster than Perlin, especially in 4 dimensions. It has a better contrast too.
		@Cpp float TCODNoise::getSimplex(float *f) const
		@C float TCOD_noise_simplex(TCOD_noise_t noise, float *f)
		@Py noise_simplex(noise, f)
		@C# float TCODNoise::getSimplexNoise(float[] f)
		@Param noise	In the C version, the generator handler, returned by the initialization function.
		@Param f	An array of coordinates, depending on the generator dimensions (between 1 and 4). The same array of coordinates will always return the same value.
		@CppEx
			// 1d noise
			TCODNoise * noise1d = new TCODNoise(1);
			float p=0.5f;
			float value = noise1d->getSimplex(&p);
			// 2d noise
			TCODNoise * noise2d = new TCODNoise(2);
			float p[2]={0.5f,0.7f};
			float value = noise2d->getSimplex(p);
		@CEx
			// 1d noise
			TCOD_noise_t noise1d = TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p=0.5f;
			float value = TCOD_noise_simplex(noise1d,&p);
			// 2d noise
			TCOD_noise_t noise2d = TCOD_noise_new(2,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p[2]={0.5f,0.7f};
			float value = TCOD_noise_simplex(noise2d,p);
		@PyEx
			# 1d noise
			noise1d = libtcod.noise_new(1)
			value = libtcod.noise_simplex(noise1d,[0.5])
			# 2d noise
			noise2d = libtcod.noise_new(2)
			value = libtcod.noise_simplex(noise2d,[0.5,0.7])
   		*/
		float getSimplex(float *f) const;

		/**
		@PageName noise_simplex
		@FuncTitle Getting simplex fbm noise
		@FuncDesc This function returns the fbm function value between -1.0 and 1.0 at given coordinates, using fractal hurst and lacunarity defined when the generator has been created.
		@Cpp float TCODNoise::getFbmSimplex(float *f, float octaves) const
		@C float TCOD_noise_fbm_simplex(TCOD_noise_t noise, float *f, float octaves)
		@Py noise_fbm_simplex(noise, f, octaves)
		@C# float TCODNoise::getSimplexBrownianMotion(float[] f, float octaves)
		@Param noise	In the C version, the generator handler, returned by the initialization function.
		@Param f	An array of coordinates, depending on the generator dimensions (between 1 and 4). The same array of coordinates will always return the same value.
		@Param octaves	Number of iterations. Must be < TCOD_NOISE_MAX_OCTAVES = 128
		@CppEx
			// 1d fbm
			TCODNoise * noise1d = new TCODNoise(1);
			float p=0.5f;
			float value = noise1d->getFbmSimplex(&p,32.0f);
			// 2d fbm
			TCODNoise * noise2d = new TCODNoise(2);
			float p[2]={0.5f,0.7f};
			float value = noise2d->getFbmSimplex(p,32.0f);
		@CEx
			// 1d fbm
			TCOD_noise_t noise1d = TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p=0.5f;
			float value = TCOD_noise_fbm_simplex(noise1d,&p,32.0f);
			// 2d fbm
			TCOD_noise_t noise2d = TCOD_noise_new(2,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p[2]={0.5f,0.7f};
			float value = TCOD_noise_fbm_simplex(noise2d,p,32.0f);
		@PyEx
			# 1d noise
			noise1d = libtcod.noise_new(1)
			value = libtcod.noise_fbm_simplex(noise1d,[0.5],32.0)
			# 2d noise
			noise2d = libtcod.noise_new(2)
			value = libtcod.noise_fbm_simplex(noise2d,[0.5,0.7],32.0)
		*/
		float getFbmSimplex(float *f, float octaves) const;

		/**
		@PageName noise_simplex
		@FuncTitle Getting simplex turbulence
		@FuncDesc This function returns the turbulence function value between -1.0 and 1.0 at given coordinates, using fractal hurst and lacunarity defined when the generator has been created.
		@Cpp float TCODNoise::getTurbulenceSimplex(float *f, float octaves) const
		@C float TCOD_noise_turbulence_simplex(TCOD_noise_t noise, float *f, float octaves)
		@Py noise_turbulence_simplex(noise, f, octaves)
		@C# float TCODNoise::getSimplexTurbulence(float[] f, float octaves)
		@Param noise	In the C version, the generator handler, returned by the initialization function.
		@Param f	An array of coordinates, depending on the generator dimensions (between 1 and 4). The same array of coordinates will always return the same value.
		@Param octaves	Number of iterations. Must be < TCOD_NOISE_MAX_OCTAVES = 128
		@CppEx
			// 1d fbm
			TCODNoise * noise1d = new TCODNoise(1);
			float p=0.5f;
			float value = noise1d->getTurbulenceSimplex(&p,32.0f);
			// 2d fbm
			TCODNoise * noise2d = new TCODNoise(2);
			float p[2]={0.5f,0.7f};
			float value = noise2d->getTurbulenceSimplex(p,32.0f);
		@CEx
			// 1d fbm
			TCOD_noise_t noise1d = TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p=0.5f;
			float value = TCOD_noise_turbulence_simplex(noise1d,&p,32.0f);
			// 2d fbm
			TCOD_noise_t noise2d = TCOD_noise_new(2,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p[2]={0.5f,0.7f};
			float value = TCOD_noise_turbulence_simplex(noise2d,p,32.0f);
		@PyEx
			# 1d noise
			noise1d = libtcod.noise_new(1)
			value = libtcod.noise_turbulence_simplex(noise1d,[0.5],32.0)
			# 2d noise
			noise2d = libtcod.noise_new(2)
			value = libtcod.noise_turbulence_simplex(noise2d,[0.5,0.7],32.0)
		*/
		float getTurbulenceSimplex(float *f, float octaves) const;

		/**
		@PageName noise_perlin
		@PageFather noise
		@PageTitle Getting Perlin noise
		@FuncTitle Getting flat Perlin noise
		@FuncDesc This function returns the perlin noise function value between -1.0 and 1.0 at given coordinates.
		@Cpp float TCODNoise::getPerlin(float *f) const
		@C float TCOD_noise_perlin(TCOD_noise_t noise, float *f)
		@Py noise_perlin(noise, f)
		@C# float TCODNoise::getPerlinNoise(float[] f)
		@Param noise	In the C version, the generator handler, returned by the initialization function.
		@Param f	An array of coordinates, depending on the generator dimensions (between 1 and 4). The same array of coordinates will always return the same value.
		@CppEx
			// 1d noise
			TCODNoise * noise1d = new TCODNoise(1);
			float p=0.5f;
			float value = noise1d->getPerlin(&p);
			// 2d noise
			TCODNoise * noise2d = new TCODNoise(2);
			float p[2]={0.5f,0.7f};
			float value = noise2d->getPerlin(p);
		@CEx
			// 1d noise
			TCOD_noise_t noise1d = TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p=0.5f;
			float value = TCOD_noise_perlin(noise1d,&p);
			// 2d noise
			TCOD_noise_t noise2d = TCOD_noise_new(2,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p[2]={0.5f,0.7f};
			float value = TCOD_noise_perlin(noise2d,p);
		@PyEx
			# 1d noise
			noise1d = libtcod.noise_new(1)
			value = libtcod.noise_perlin(noise1d,[0.5])
			# 2d noise
			noise2d = libtcod.noise_new(2)
			value = libtcod.noise_perlin(noise2d,[0.5,0.7])
		*/
		float getPerlin(float *f) const;

		/**
		@PageName noise_perlin
		@FuncTitle Getting Perlin fbm
		@FuncDesc This function returns the fbm function value between -1.0 and 1.0 at given coordinates, using fractal hurst and lacunarity defined when the generator has been created.
		@Cpp float TCODNoise::getFbmPerlin(float *f, float octaves) const
		@C float TCOD_noise_fbm_perlin(TCOD_noise_t noise, float *f, float octaves)
		@Py noise_fbm_perlin(noise, f, octaves)
		@C# float TCODNoise::getPerlinBrownianMotion(float[] f, float octaves)
		@Param noise	In the C version, the generator handler, returned by the initialization function.
		@Param f	An array of coordinates, depending on the generator dimensions (between 1 and 4). The same array of coordinates will always return the same value.
		@Param octaves	Number of iterations. Must be < TCOD_NOISE_MAX_OCTAVES = 128
		@CppEx
			// 1d fbm
			TCODNoise * noise1d = new TCODNoise(1);
			float p=0.5f;
			float value = noise1d->getFbmPerlin(&p,32.0f);
			// 2d fbm
			TCODNoise * noise2d = new TCODNoise(2);
			float p[2]={0.5f,0.7f};
			float value = noise2d->getFbmPerlin(p,32.0f);
		@CEx
			// 1d fbm
			TCOD_noise_t noise1d = TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p=0.5f;
			float value = TCOD_noise_fbm_perlin(noise1d,&p,32.0f);
			// 2d fbm
			TCOD_noise_t noise2d = TCOD_noise_new(2,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p[2]={0.5f,0.7f};
			float value = TCOD_noise_fbm_perlin(noise2d,p,32.0f);
		@PyEx
			# 1d noise
			noise1d = libtcod.noise_new(1)
			value = libtcod.noise_fbm_perlin(noise1d,[0.5],32.0)
			# 2d noise
			noise2d = libtcod.noise_new(2)
			value = libtcod.noise_fbm_perlin(noise2d,[0.5,0.7],32.0)
		*/
		float getFbmPerlin(float *f, float octaves) const;

		/**
		@PageName noise_perlin
		@FuncTitle Getting Perlin turbulence
		@FuncDesc This function returns the turbulence function value between -1.0 and 1.0 at given coordinates, using fractal hurst and lacunarity defined when the generator has been created.
		@Cpp float TCODNoise::getTurbulencePerlin(float *f, float octaves) const
		@C float TCOD_noise_turbulence_perlin(TCOD_noise_t noise, float *f, float octaves)
		@Py noise_turbulence_perlin(noise, f, octaves)
		@C# float TCODNoise::getPerlinTurbulence(float[] f, float octaves)
		@Param noise	In the C version, the generator handler, returned by the initialization function.
		@Param f	An array of coordinates, depending on the generator dimensions (between 1 and 4). The same array of coordinates will always return the same value.
		@Param octaves	Number of iterations. Must be < TCOD_NOISE_MAX_OCTAVES = 128
		@CppEx
			// 1d fbm
			TCODNoise * noise1d = new TCODNoise(1);
			float p=0.5f;
			float value = noise1d->getTurbulencePerlin(&p,32.0f);
			// 2d fbm
			TCODNoise * noise2d = new TCODNoise(2);
			float p[2]={0.5f,0.7f};
			float value = noise2d->getTurbulencePerlin(p,32.0f);
		@CEx
			// 1d fbm
			TCOD_noise_t noise1d = TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p=0.5f;
			float value = TCOD_noise_turbulence_perlin(noise1d,&p,32.0f);
			// 2d fbm
			TCOD_noise_t noise2d = TCOD_noise_new(2,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p[2]={0.5f,0.7f};
			float value = TCOD_noise_turbulence_perlin(noise2d,p,32.0f);
		@PyEx
			# 1d noise
			noise1d = libtcod.noise_new(1)
			value = libtcod.noise_turbulence_perlin(noise1d,[0.5],32.0)
			# 2d noise
			noise2d = libtcod.noise_new(2)
			value = libtcod.noise_turbulence_perlin(noise2d,[0.5,0.7],32.0)
		*/
		float getTurbulencePerlin(float *f, float octaves) const;

		/**
		@PageName noise_wavelet
		@PageFather noise
		@PageTitle Getting wavelet noise
		@FuncTitle Getting flat wavelet noise
		@FuncDesc This function returns the wavelet noise function value between -1.0 and 1.0 at given coordinates.
			The wavelet noise is much slower than Perlin, and can only be computed in 1,2 or 3 dimensions. But it doesn't blur at high scales.
		@Cpp float TCODNoise::getWavelet(float *f) const
		@C float TCOD_noise_wavelet(TCOD_noise_t noise, float *f)
		@Py noise_wavelet(noise, f)
		@C# float TCODNoise::getWaveletNoise(float[] f)
		@Param noise	In the C version, the generator handler, returned by the initialization function.
		@Param f	An array of coordinates, depending on the generator dimensions (between 1 and 4). The same array of coordinates will always return the same value.
		@CppEx
			// 1d noise
			TCODNoise * noise1d = new TCODNoise(1);
			float p=0.5f;
			float value = noise1d->getWavelet(&p);
			// 2d noise
			TCODNoise * noise2d = new TCODNoise(2);
			float p[2]={0.5f,0.7f};
			float value = noise2d->getWavelet(p);
		@CEx
			// 1d noise
			TCOD_noise_t noise1d = TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p=0.5f;
			float value = TCOD_noise_wavelet(noise1d,&p);
			// 2d noise
			TCOD_noise_t noise2d = TCOD_noise_new(2,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p[2]={0.5f,0.7f};
			float value = TCOD_noise_wavelet(noise2d,p);
		@PyEx
			# 1d noise
			noise1d = libtcod.noise_new(1)
			value = libtcod.noise_wavelet(noise1d,[0.5])
			# 2d noise
			noise2d = libtcod.noise_new(2)
			value = libtcod.noise_wavelet(noise2d,[0.5,0.7])
		*/
		float getWavelet(float *f) const;

		/**
		@PageName noise_wavelet
		@FuncTitle Getting wavelet fbm
		@FuncDesc This function returns the fbm function value between -1.0 and 1.0 at given coordinates, using fractal hurst and lacunarity defined when the generator has been created.
		@Cpp float TCODNoise::getFbmWavelet(float *f, float octaves) const
		@C float TCOD_noise_fbm_wavelet(TCOD_noise_t noise, float *f, float octaves)
		@Py noise_fbm_wavelet(noise, f, octaves)
		@C# float TCODNoise::getWaveletBrownianMotion(float[] f, float octaves)
		@Param noise	In the C version, the generator handler, returned by the initialization function.
		@Param f	An array of coordinates, depending on the generator dimensions (between 1 and 4). The same array of coordinates will always return the same value.
		@Param octaves	Number of iterations. Must be < TCOD_NOISE_MAX_OCTAVES = 128
		@CppEx
			// 1d fbm
			TCODNoise * noise1d = new TCODNoise(1);
			float p=0.5f;
			float value = noise1d->getFbmWavelet(&p,32.0f);
			// 2d fbm
			TCODNoise * noise2d = new TCODNoise(2);
			float p[2]={0.5f,0.7f};
			float value = noise2d->getFbmWavelet(p,32.0f);
		@CEx
			// 1d fbm
			TCOD_noise_t noise1d = TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p=0.5f;
			float value = TCOD_noise_fbm_wavelet(noise1d,&p,32.0f);
			// 2d fbm
			TCOD_noise_t noise2d = TCOD_noise_new(2,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p[2]={0.5f,0.7f};
			float value = TCOD_noise_fbm_wavelet(noise2d,p,32.0f);
		@PyEx
			# 1d noise
			noise1d = libtcod.noise_new(1)
			value = libtcod.noise_fbm_wavelet(noise1d,[0.5],32.0)
			# 2d noise
			noise2d = libtcod.noise_new(2)
			value = libtcod.noise_fbm_wavelet(noise2d,[0.5,0.7],32.0)
		*/
		float getFbmWavelet(float *f, float octaves) const;

		/**
		@PageName noise_wavelet
		@FuncTitle Getting wavelet turbulence
		@FuncDesc This function returns the turbulence function value between -1.0 and 1.0 at given coordinates, using fractal hurst and lacunarity defined when the generator has been created.
		@Cpp float TCODNoise::getTurbulenceWavelet(float *f, float octaves) const
		@C float TCOD_noise_turbulence_wavelet(TCOD_noise_t noise, float *f, float octaves)
		@Py noise_turbulence_wavelet(noise, f, octaves)
		@C# float TCODNoise::getWaveletTurbulence(float[] f, float octaves)
		@Param noise	In the C version, the generator handler, returned by the initialization function.
		@Param f	An array of coordinates, depending on the generator dimensions (between 1 and 4). The same array of coordinates will always return the same value.
		@Param octaves	Number of iterations. Must be < TCOD_NOISE_MAX_OCTAVES = 128
		@CppEx
			// 1d fbm
			TCODNoise * noise1d = new TCODNoise(1);
			float p=0.5f;
			float value = noise1d->getTurbulenceWavelet(&p,32.0f);
			// 2d fbm
			TCODNoise * noise2d = new TCODNoise(2);
			float p[2]={0.5f,0.7f};
			float value = noise2d->getTurbulenceWavelet(p,32.0f);
		@CEx
			// 1d fbm
			TCOD_noise_t noise1d = TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p=0.5f;
			float value = TCOD_noise_turbulence_wavelet(noise1d,&p,32.0f);
			// 2d fbm
			TCOD_noise_t noise2d = TCOD_noise_new(2,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p[2]={0.5f,0.7f};
			float value = TCOD_noise_turbulence_wavelet(noise2d,p,32.0f);
		@PyEx
			# 1d noise
			noise1d = libtcod.noise_new(1)
			value = libtcod.noise_turbulence_wavelet(noise1d,[0.5],32.0)
			# 2d noise
			noise2d = libtcod.noise_new(2)
			value = libtcod.noise_turbulence_wavelet(noise2d,[0.5,0.7],32.0)
		*/
		float getTurbulenceWavelet(float *f, float octaves) const;

		void setType (TCOD_noise_type_t type);
		float get(float *f, TCOD_noise_type_t type = TCOD_NOISE_DEFAULT);
		float getFbm(float *f, float octaves, TCOD_noise_type_t type = TCOD_NOISE_DEFAULT);
		float getTurbulence(float *f, float octaves, TCOD_noise_type_t type = TCOD_NOISE_DEFAULT);

	protected :
		friend class TCODLIB_API TCODHeightMap;
		TCOD_noise_t data;
};

#endif
