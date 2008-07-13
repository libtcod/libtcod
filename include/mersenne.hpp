#ifndef _TCOD_RANDOM_HPP
#define _TCOD_RANDOM_HPP

class TCODLIB_API TCODRandom {
	public :
		static TCODRandom *getInstance();
		TCODRandom();
		TCODRandom(uint32 seed);
		int getInt(int min, int max);
		float getFloat(float min, float max);
		static int getIntFromByteArray(int min, int max, const char *data,int len);
		virtual ~TCODRandom();

	protected :
		friend class TCODNoise;
		TCOD_random_t data;
};

#endif
