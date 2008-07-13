#ifndef _TCOD_PERLIN_H
#define _TCOD_PERLIN_H

typedef void *TCOD_noise_t;

#define TCOD_NOISE_MAX_OCTAVES			128	
#define TCOD_NOISE_MAX_DIMENSIONS		4
#define TCOD_NOISE_DEFAULT_HURST        0.5f
#define TCOD_NOISE_DEFAULT_LACUNARITY   2.0f

TCODLIB_API TCOD_noise_t TCOD_noise_new(int dimensions, float hurst, float lacunarity, TCOD_random_t random);
// basic perlin noise
TCODLIB_API float TCOD_noise_get( TCOD_noise_t noise, float *f );
// fractional brownian motion
TCODLIB_API float TCOD_noise_fbm( TCOD_noise_t noise, float *f, float octaves );
// turbulence
TCODLIB_API float TCOD_noise_turbulence( TCOD_noise_t noise, float *f, float octaves );
TCODLIB_API void TCOD_noise_delete(TCOD_noise_t noise);

#endif
