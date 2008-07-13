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

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "libtcod.h"

#ifdef BORLANDC
#define sqrtf (float)sqrt
#endif

typedef struct {
	int ndim;
	unsigned char map[256]; // Randomized map of indexes into buffer
	float buffer[256][TCOD_NOISE_MAX_DIMENSIONS]; 	// Random 256 x ndim buffer
	// fractal stuff
	float H;
	float lacunarity;
	float exponent[TCOD_NOISE_MAX_OCTAVES];
	TCOD_random_t rand;
} perlin_data_t;

static float lattice( perlin_data_t *data, int ix, float fx, int iy, float fy, int iz, float fz, int iw, float fw)
{
#ifdef BORLANDC
    int n[4];
    float f[4];
#else
	int n[4] = {ix, iy, iz, iw};
	float f[4] = {fx, fy, fz, fw};
#endif
	int nIndex = 0;
	int i;
	float value = 0;
#ifdef BORLANDC
    n[0]=ix;n[1]=iy;n[2]=iz;n[3]=iw;
    f[0]=fx;f[1]=fy;f[2]=fz;f[3]=fw;
#endif
	for(i=0; i<data->ndim; i++)
		nIndex = data->map[(nIndex + n[i]) & 0xFF];
	for(i=0; i<data->ndim; i++)
		value += data->buffer[nIndex][i] * f[i];
	return value;
}

#define DEFAULT_SEED 0x15687436
#define DELTA				1e-6f
#define SWAP(a, b, t)		t = a; a = b; b = t

#define FLOOR(a) ((int)a - (a < 0 && a != (int)a))
#define CUBIC(a)	( a * a * (3 - 2*a) )

static void normalize(perlin_data_t *data, float *f)
{
	float magnitude = 0;
	int i;
	for(i=0; i<data->ndim; i++)
		magnitude += f[i]*f[i];
	magnitude = 1 / sqrtf(magnitude);
	for(i=0; i<data->ndim; i++)
		f[i] *= magnitude;
}


TCOD_noise_t TCOD_noise_new(int ndim, float hurst, float lacunarity, TCOD_random_t random)
{
	perlin_data_t *data=(perlin_data_t *)calloc(sizeof(perlin_data_t),1);
	int i, j;
	unsigned char tmp;
	float f = 1;
	data->rand = random ? random : TCOD_random_get_instance();
	data->ndim = ndim;
	for(i=0; i<256; i++)
	{
		data->map[i] = (unsigned char)i;
		for(j=0; j<data->ndim; j++)
			data->buffer[i][j] = TCOD_random_get_float(data->rand,-0.5, 0.5);
		normalize(data,data->buffer[i]);
	}

	while(--i)
	{
		j = TCOD_random_get_int(data->rand,0, 255);
		SWAP(data->map[i], data->map[j], tmp);
	}

	data->H = hurst;
	data->lacunarity = lacunarity;
	for(i=0; i<TCOD_NOISE_MAX_OCTAVES; i++)
	{
		//exponent[i] = powf(f, -H);
		data->exponent[i] = 1.0f / f;
		f *= lacunarity;
	}
	return (TCOD_noise_t)data;
}

float TCOD_noise_get( TCOD_noise_t noise, float *f )
{
	perlin_data_t *data=(perlin_data_t *)noise;
	int n[TCOD_NOISE_MAX_DIMENSIONS];			// Indexes to pass to lattice function
	int i;
	float r[TCOD_NOISE_MAX_DIMENSIONS];		// Remainders to pass to lattice function
	float w[TCOD_NOISE_MAX_DIMENSIONS];		// Cubic values to pass to interpolation function
	float value;

	for(i=0; i<data->ndim; i++)
	{
		n[i] = FLOOR(f[i]);
		r[i] = f[i] - n[i];
		w[i] = CUBIC(r[i]);
	}

	switch(data->ndim)
	{
		case 1:
			value = LERP(lattice(data,n[0], r[0],0,0,0,0,0,0),
						  lattice(data,n[0]+1, r[0]-1,0,0,0,0,0,0),
						  w[0]);
			break;
		case 2:
			value = LERP(LERP(lattice(data,n[0], r[0], n[1], r[1],0,0,0,0),
							   lattice(data,n[0]+1, r[0]-1, n[1], r[1],0,0,0,0),
							   w[0]),
						  LERP(lattice(data,n[0], r[0], n[1]+1, r[1]-1,0,0,0,0),
							   lattice(data,n[0]+1, r[0]-1, n[1]+1, r[1]-1,0,0,0,0),
							   w[0]),
						  w[1]);
			break;
		case 3:
			value = LERP(LERP(LERP(lattice(data,n[0], r[0], n[1], r[1], n[2], r[2],0,0),
									lattice(data,n[0]+1, r[0]-1, n[1], r[1], n[2], r[2],0,0),
									w[0]),
							   LERP(lattice(data,n[0], r[0], n[1]+1, r[1]-1, n[2], r[2],0,0),
									lattice(data,n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2], r[2],0,0),
									w[0]),
							   w[1]),
						  LERP(LERP(lattice(data,n[0], r[0], n[1], r[1], n[2]+1, r[2]-1,0,0),
									lattice(data,n[0]+1, r[0]-1, n[1], r[1], n[2]+1, r[2]-1,0,0),
									w[0]),
							   LERP(lattice(data,n[0], r[0], n[1]+1, r[1]-1, n[2]+1, r[2]-1,0,0),
									lattice(data,n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2]+1, r[2]-1,0,0),
									w[0]),
							   w[1]),
						  w[2]);
			break;
		case 4:
		default:
			value = LERP(LERP(LERP(LERP(lattice(data,n[0], r[0], n[1], r[1], n[2], r[2], n[3], r[3]),
										 lattice(data,n[0]+1, r[0]-1, n[1], r[1], n[2], r[2], n[3], r[3]),
										 w[0]),
									LERP(lattice(data,n[0], r[0], n[1]+1, r[1]-1, n[2], r[2], n[3], r[3]),
										 lattice(data,n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2], r[2], n[3], r[3]),
										 w[0]),
									w[1]),
									LERP(LERP(lattice(data,n[0], r[0], n[1], r[1], n[2]+1, r[2]-1, n[3], r[3]),
										 lattice(data,n[0]+1, r[0]-1, n[1], r[1], n[2]+1, r[2]-1, n[3], r[3]),
										 w[0]),
									LERP(lattice(data,n[0], r[0], n[1]+1, r[1]-1, n[2]+1, r[2]-1,0,0),
										 lattice(data,n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2]+1, r[2]-1, n[3], r[3]),
										 w[0]),
									w[1]),
							   w[2]),
						  LERP(LERP(LERP(lattice(data,n[0], r[0], n[1], r[1], n[2], r[2], n[3]+1, r[3]-1),
										 lattice(data,n[0]+1, r[0]-1, n[1], r[1], n[2], r[2], n[3]+1, r[3]-1),
										 w[0]),
									LERP(lattice(data,n[0], r[0], n[1]+1, r[1]-1, n[2], r[2], n[3]+1, r[3]-1),
										 lattice(data,n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2], r[2], n[3]+1, r[3]-1),
										 w[0]),
									w[1]),
									LERP(LERP(lattice(data,n[0], r[0], n[1], r[1], n[2]+1, r[2]-1, n[3]+1, r[3]-1),
										 lattice(data,n[0]+1, r[0]-1, n[1], r[1], n[2]+1, r[2]-1, n[3]+1, r[3]-1),
										 w[0]),
									LERP(lattice(data,n[0], r[0], n[1]+1, r[1]-1, n[2]+1, r[2]-1,0,0),
										 lattice(data,n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2]+1, r[2]-1, n[3]+1, r[3]-1),
										 w[0]),
									w[1]),
							   w[2]),
						  w[3]);
			break;
	}
	return CLAMP(-0.99999f, 0.99999f, value);
}

float TCOD_noise_fbm( TCOD_noise_t noise,  float *f, float octaves )
{
	float tf[TCOD_NOISE_MAX_DIMENSIONS];
	perlin_data_t *data=(perlin_data_t *)noise;
	// Initialize locals
	double value = 0;
	int i,j;
	memcpy(tf,f,sizeof(float)*data->ndim);

	// Inner loop of spectral construction, where the fractal is built
	for(i=0; i<(int)octaves; i++)
	{
		value += (double)(TCOD_noise_get(noise,tf)) * data->exponent[i];
		for (j=0; j < data->ndim; j++) tf[j] *= data->lacunarity;
	}

	// Take care of remainder in octaves
	octaves -= (int)octaves;
	if(octaves > DELTA)
		value += (double)(octaves * TCOD_noise_get(noise,tf)) * data->exponent[i];
	return CLAMP(-0.99999f, 0.99999f, (float)value);
}

float TCOD_noise_turbulence( TCOD_noise_t noise, float *f, float octaves )
{
	float tf[TCOD_NOISE_MAX_DIMENSIONS];
	perlin_data_t *data=(perlin_data_t *)noise;
	// Initialize locals
	double value = 0;
	int i,j;
	memcpy(tf,f,sizeof(float)*data->ndim);

	// Inner loop of spectral construction, where the fractal is built
	for(i=0; i<(int)octaves; i++)
	{
		value += (double)(ABS(TCOD_noise_get(noise,tf))) * data->exponent[i];
		for (j=0; j < data->ndim; j++) tf[j] *= data->lacunarity;
	}

	// Take care of remainder in octaves
	octaves -= (int)octaves;
	if(octaves > DELTA)
		value += (double)(octaves * ABS(TCOD_noise_get(noise,tf))) * data->exponent[i];
	return CLAMP(-0.99999f, 0.99999f, (float)value);
}

void TCOD_noise_delete(TCOD_noise_t noise) {
	free((perlin_data_t *)noise);
}
