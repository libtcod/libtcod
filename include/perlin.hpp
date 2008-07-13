#ifndef _TCOD_PERLIN_HPP
#define _TCOD_PERLIN_HPP

class TCODLIB_API TCODNoise {
	public :
		TCODNoise(int dimensions);
		TCODNoise(int dimensions, TCODRandom *random);
		TCODNoise(int dimensions, float hurst, float lacunarity);
		TCODNoise(int dimensions, float hurst, float lacunarity, TCODRandom *random);
		float getNoise(float *f) const;
		float getFbm(float *f, float octaves) const;
		float getTurbulence(float *f, float octaves) const;
		virtual ~TCODNoise();

	protected :
		TCOD_noise_t data;
};

#endif
