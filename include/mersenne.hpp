/*
* libtcod 1.5.0
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

#ifndef _TCOD_RANDOM_HPP
#define _TCOD_RANDOM_HPP

class TCODLIB_API TCODRandom {
	public :
		static TCODRandom * getInstance(void);
		TCODRandom(TCOD_random_algo_t algo = TCOD_RNG_CMWC, bool allocate = true);
		TCODRandom(uint32 seed, TCOD_random_algo_t algo = TCOD_RNG_CMWC);
		int getInt(int min, int max);
		float getFloat(float min, float max);
		TCODRandom * save() const;
		void restore(const TCODRandom *backup);
		float getGaussian(float min, float max);

		virtual ~TCODRandom();

	protected :
		friend class TCODLIB_API TCODNoise;
		friend class TCODLIB_API TCODHeightMap;
		friend class TCODLIB_API TCODNamegen;
		TCOD_random_t data;
};

//class TCODLIB_API TCODCmwc {
//    public:
//        static TCODCmwc * getInstance();
//        TCODCmwc(void);
//        TCODCmwc(unsigned long seed);
//        int getInt(int min, int max);
//        float getFloat(float min, float max);
//    private:
//        TCOD_cmwc_t data;
//};

#endif
