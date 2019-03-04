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
#ifndef _TCOD_PERLIN_HPP
#define _TCOD_PERLIN_HPP

#include "mersenne.hpp"
#include "noise.h"
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
		@FuncDesc To release resources used by a generator, use those functions :
		@Cpp TCODNoise::~TCODNoise()
		@C void TCOD_noise_delete(TCOD_noise_t noise)
		@Py noise_delete(noise)
		@C# void TCODNoise::Dispose()
		@Param noise	In the C and Python versions, the generator handler, returned by the initialization function.
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
   		@PageName noise_setType
		@PageFather noise
		@PageTitle Choosing a noise type
		@FuncTitle Choosing a noise type
		@FuncDesc Use this function to define the default algorithm used by the noise functions.
			The default algorithm is simplex. It's much faster than Perlin, especially in 4 dimensions. It has a better contrast too.
		@Cpp void TCODNoise::setType(TCOD_noise_type_t type)
		@C void TCOD_noise_set_type(TCOD_noise_t noise, TCOD_noise_type_t type)
		@Py noise_set_type(noise, type)
		@C# void TCODNoise::setType(type)
		@Param noise	In the C version, the generator handler, returned by the initialization function.
		@Param type		The algorithm to use, either TCOD_NOISE_SIMPLEX, TCOD_NOISE_PERLIN or TCOD_NOISE_WAVELET.
		@CppEx
			TCODNoise * noise1d = new TCODNoise(1);
			noise1d->setType(TCOD_NOISE_PERLIN);
		@CEx
			TCOD_noise_t noise1d = TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			TCOD_noise_set_type(noise1d,TCOD_NOISE_PERLIN);
		@PyEx
			noise1d = libtcod.noise_new(1)
			libtcod.noise_set_type(noise1d,libtcod.NOISE_PERLIN)
   		*/
		void setType (TCOD_noise_type_t type);
   		/**
   		@PageName noise_get
		@PageFather noise
		@PageTitle Getting flat noise
		@FuncDesc This function returns the noise function value between -1.0 and 1.0 at given coordinates.
		@Cpp float TCODNoise::get(float *f, TCOD_noise_type_t type = TCOD_NOISE_DEFAULT)
		@C	float TCOD_noise_get(TCOD_noise_t noise, float *f)
float TCOD_noise_get_ex(TCOD_noise_t noise, float *f, TCOD_noise_type_t type)
		@Py noise_get(noise, f, type=NOISE_DEFAULT)
		@C# float TCODNoise::get(float[] f, type=NoiseDefault)
		@Param noise	In the C version, the generator handler, returned by the initialization function.
		@Param f	An array of coordinates, depending on the generator dimensions (between 1 and 4). The same array of coordinates will always return the same value.
		@Param type	The algorithm to use. If not defined, use the default one (set with setType or simplex if not set)
		@CppEx
			// 1d noise
			TCODNoise * noise1d = new TCODNoise(1);
			float p=0.5f;
			// get a 1d simplex value
			float value = noise1d->get(&p);
			// 2d noise
			TCODNoise * noise2d = new TCODNoise(2);
			float p[2]={0.5f,0.7f};
			// get a 2D Perlin value
			float value = noise2d->get(p, TCOD_NOISE_PERLIN);
		@CEx
			// 1d noise
			TCOD_noise_t noise1d = TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p=0.5f;
			// get a 1d simplex value
			float value = TCOD_noise_get(noise1d,&p);
			// 2d noise
			TCOD_noise_t noise2d = TCOD_noise_new(2,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p[2]={0.5f,0.7f};
			// get a 2d perlin value
			float value = TCOD_noise_get_ex(noise2d,p,TCOD_NOISE_PERLIN);
		@PyEx
			# 1d noise
			noise1d = libtcod.noise_new(1)
			# get a 1d simplex value
			value = libtcod.noise_get(noise1d,[0.5])
			# 2d noise
			noise2d = libtcod.noise_new(2)
			# get a 2d perlin value
			value = libtcod.noise_get(noise2d,[0.5,0.7], libtcod.NOISE_PERLIN)
   		*/
		float get(float *f, TCOD_noise_type_t type = TCOD_NOISE_DEFAULT);
		/**
		@PageName noise_get_fbm
		@PageFather noise
		@PageTitle Getting fbm noise
		@FuncDesc This function returns the fbm function value between -1.0 and 1.0 at given coordinates, using fractal hurst and lacunarity defined when the generator has been created.
		@Cpp float TCODNoise::getFbm(float *f, float octaves, TCOD_noise_type_t type = TCOD_NOISE_DEFAULT)
		@C	float TCOD_noise_get_fbm(TCOD_noise_t noise, float *f, float octaves)
float TCOD_noise_get_fbm(TCOD_noise_t noise, float *f, float octaves, TCOD_noise_type_t type)
		@Py noise_get_fbm(noise, f, octaves, type=NOISE_DEFAULT)
		@C# float TCODNoise::getBrownianMotion(float[] f, float octaves, type=NoiseDefault)
		@Param noise	In the C version, the generator handler, returned by the initialization function.
		@Param f	An array of coordinates, depending on the generator dimensions (between 1 and 4). The same array of coordinates will always return the same value.
		@Param octaves	Number of iterations. Must be < TCOD_NOISE_MAX_OCTAVES = 128
		@Param type	The algorithm to use. If not defined, use the default one (set with setType or simplex if not set)
		@CppEx
			// 1d fbm
			TCODNoise * noise1d = new TCODNoise(1);
			float p=0.5f;
			// get a 1d simplex fbm
			float value = noise1d->getFbm(&p,32.0f);
			// 2d fbm
			TCODNoise * noise2d = new TCODNoise(2);
			float p[2]={0.5f,0.7f};
			// get a 2d perlin fbm
			float value = noise2d->getFbm(p,32.0f, TCOD_NOISE_PERLIN);
		@CEx
			// 1d fbm
			TCOD_noise_t noise1d = TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p=0.5f;
			// get a 1d simplex fbm
			float value = TCOD_noise_get_fbm(noise1d,&p,32.0f);
			// 2d fbm
			TCOD_noise_t noise2d = TCOD_noise_new(2,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p[2]={0.5f,0.7f};
			// get a 2d perlin fbm
			float value = TCOD_noise_get_fbm_ex(noise2d,p,32.0f,TCOD_NOISE_PERLIN);
		@PyEx
			# 1d noise
			noise1d = libtcod.noise_new(1)
			# 1d simplex fbm
			value = libtcod.noise_get_fbm(noise1d,[0.5],32.0)
			# 2d noise
			noise2d = libtcod.noise_new(2)
			# 2d perlin fbm
			value = libtcod.noise_get_fbm(noise2d,[0.5,0.7],32.0, libtcod.NOISE_PERLIN)
		*/
		float getFbm(float *f, float octaves, TCOD_noise_type_t type = TCOD_NOISE_DEFAULT);
		/**
		@PageName noise_get_turbulence
		@PageFather noise
		@PageTitle Getting turbulence
		@FuncDesc This function returns the turbulence function value between -1.0 and 1.0 at given coordinates, using fractal hurst and lacunarity defined when the generator has been created.
		@Cpp float TCODNoise::getTurbulence(float *f, float octaves, TCOD_noise_type_t type = TCOD_NOISE_DEFAULT)
		@C	float TCOD_noise_get_turbulence(TCOD_noise_t noise, float *f, float octaves)
float TCOD_noise_get_turbulence_ex(TCOD_noise_t noise, float *f, float octaves, TCOD_noise_type_t)
		@Py noise_get_turbulence(noise, f, octaves, type=NOISE_DEFAULT)
		@C# float TCODNoise::getTurbulence(float[] f, float octaves, type=NoiseDefault)
		@Param noise	In the C version, the generator handler, returned by the initialization function.
		@Param f	An array of coordinates, depending on the generator dimensions (between 1 and 4). The same array of coordinates will always return the same value.
		@Param octaves	Number of iterations. Must be < TCOD_NOISE_MAX_OCTAVES = 128
		@CppEx
			// 1d fbm
			TCODNoise * noise1d = new TCODNoise(1);
			float p=0.5f;
			// a 1d simplex turbulence
			float value = noise1d->getTurbulence(&p,32.0f);
			// 2d fbm
			TCODNoise * noise2d = new TCODNoise(2);
			float p[2]={0.5f,0.7f};
			// a 2d perlin turbulence
			float value = noise2d->getTurbulence(p,32.0f, TCOD_NOISE_PERLIN);
		@CEx
			// 1d fbm
			TCOD_noise_t noise1d = TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p=0.5f;
			// a 1d simplex turbulence
			float value = TCOD_noise_get_turbulence(noise1d,&p,32.0f);
			// 2d fbm
			TCOD_noise_t noise2d = TCOD_noise_new(2,TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
			float p[2]={0.5f,0.7f};
			// a 2d perlin turbulence
			float value = TCOD_noise_get_turbulence_ex(noise2d,p,32.0f, TCOD_NOISE_PERLIN);
		@PyEx
			# 1d noise
			noise1d = libtcod.noise_new(1)
			# 1d simplex turbulence
			value = libtcod.noise_get_turbulence(noise1d,[0.5],32.0)
			# 2d noise
			noise2d = libtcod.noise_new(2)
			# 2d perlin turbulence
			value = libtcod.noise_get_turbulence(noise2d,[0.5,0.7],32.0,libtcod.NOISE_PERLIN)
		*/
		float getTurbulence(float *f, float octaves, TCOD_noise_type_t type = TCOD_NOISE_DEFAULT);

	protected :
		friend class TCODLIB_API TCODHeightMap;
		TCOD_noise_t data;
};

#endif
