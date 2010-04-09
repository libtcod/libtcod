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

#ifndef _TCOD_RANDOM_HPP
#define _TCOD_RANDOM_HPP

#include "mersenne_types.h"

/**
 @PageName random
 @PageCategory Base toolkits
 @PageTitle Pseudorandom number generator
 @PageDesc This toolkit is an implementation of two fast and high quality pseudorandom number generators:
* a Mersenne twister generator,
* a Complementary-Multiply-With-Carry generator.
CMWC is faster than MT (see table below) and has a much better period (1039460 vs. 106001). It is the default algo since libtcod 1.5.0.

Relative performances in two independent tests (lower is better) :
<table class="param"> 
    <tr> 
      <th>Algorithm</th> 
      <th>Numbers generated</th> 
      <th>Perf (1)</th> 
      <th>Perf (2)</th> 
    </tr> 
    <tr class="hilite"> 
      <td>MT</td> 
      <td>integer</td> 
      <td>62</td> 
      <td>50</td> 
    </tr> 
    <tr> 
      <td>MT</td> 
      <td>float</td> 
      <td>54</td> 
      <td>45</td> 
    </tr> 
    <tr class="hilite"> 
      <td>CMWC</td> 
      <td>integer</td> 
      <td>21</td> 
      <td>34</td> 
    </tr> 
    <tr> 
      <td>CMWC</td> 
      <td>float</td> 
      <td>32</td> 
      <td>27</td> 
    </tr> 
</table> 

<h6>For python users:</h6>
Python already has great builtin random generators. But some parts of the Doryen library (noise, heightmap, ...) uses RNG as parameters. If you intend to use those functions, you must provide a RNG created with the library.
 */

class TCODLIB_API TCODRandom {
	public :
		/**
		@PageName random_init
		@PageFather random
		@PageTitle Creating a generator
		@FuncTitle Default generator
		@FuncDesc The simplest way to get random number is to use the default generator. The first time you get this generator, it is initialized by calling TCOD_random_new. Then, on successive calls, this function returns the same generator (singleton pattern).
		@Cpp static TCODRandom * TCODRandom::getInstance (void)
		@C TCOD_random_t TCOD_random_get_instance (void)
		@Py random_get_instance ()
		@Param algo	The PRNG algorithm the generator should be using. Possible values are:
			* TCOD_RNG_MT for Mersenne Twister,
			* TCOD_RNG_CMWC for Complementary Multiply-With-Carry.		
		*/		
		static TCODRandom * getInstance(void);
		
		/**
		@PageName random_init
		@FuncTitle Generators with random seeds
		@FuncDesc You can also create as many generators as you want with a random seed (the number of seconds since Jan 1 1970 at the time the constructor is called). Warning ! If you call this function several times in the same second, it will return the same generator.
		@Cpp TCODRandom::TCODRandom (TCOD_random_algo_t algo = TCOD_RNG_CMWC)
		@C TCOD_random_t TCOD_random_new (TCOD_random_algo_t algo)
		@Py random_new (algo = RNG_CMWC)
		@Param algo	The PRNG algorithm the generator should be using.		
		*/		
		TCODRandom(TCOD_random_algo_t algo = TCOD_RNG_CMWC, bool allocate = true);
		
		/**
		@PageName random_init
		@FuncTitle Generators with user defined seeds
		@FuncDesc Finally, you can create generators with a specific seed. Those allow you to get a reproducible set of random numbers. You can for example save a dungeon in a file by saving only the seed used for its generation (provided you have a determinist generation algorithm)
		@Cpp TCODRandom::TCODRandom (uint32 seed, TCOD_random_algo_t algo = TCOD_RNG_CMWC);
		@C TCOD_random_t TCOD_random_new_from_seed (TCOD_random_algo_t algo, uint32 seed);
		@Py random_new_from_seed(seed, algo=RNG_CMWC)
		@Param seed	The 32 bits seed used to initialize the generator. Two generators created with the same seed will generate the same set of pseudorandom numbers.
		@Param algo	The PRNG algorithm the generator should be using.
		@CppEx // default generator
			TCODRandom * default = TCODRandom::getInstance();
			// another random generator
			TCODRandom * myRandom = new TCODRandom();
			// a random generator with a specific seed
			TCODRandom * myDeterministRandom = new TCODRandom(0xdeadbeef);
        @CEx // default generator 
			TCOD_random_t default = TCOD_random_get_instance();
			// another random generator 
			TCOD_random_t my_random = TCOD_random_new(TCOD_RNG_CMWC);
			// a random generator with a specific seed
			TCOD_random_t my_determinist_random = TCOD_random_new_from_seed(TCOD_RNG_CMWC,0xdeadbeef);
		@PyEx # default generator
			default = libtcod.random_get_instance()
			# another random generator
			my_random = libtcod.random_new()
			# a random generator with a specific seed
			my_determinist_random = libtcod.random_new_from_seed(0xdeadbeef)
		*/		
		TCODRandom(uint32 seed, TCOD_random_algo_t algo = TCOD_RNG_CMWC);

		/**
		@PageName random_init
		@FuncTitle Destroying a RNG
		@FuncDesc To release ressources used by a generator, use those functions :
			NB : do not delete the default random generator !
		@Cpp TCODRandom::~TCODRandom()
		@C void TCOD_random_delete(TCOD_random_t mersenne)
		@Py random_delete(mersenne)
		@Param mersenne	In the C version, the generator handler, returned by the initialization functions.
		@CppEx // create a generator
	      TCODRandom *rnd = new TCODRandom();
	      // use it 
	      ...
	      // destroy it
	      delete rnd;
		@CEx // create a generator 
	      TCOD_random_t rnd = TCOD_random_new();
	      // use it  
	      ...
	      // destroy it 
	      TCOD_random_delete(rnd);
		@PyEx # create a generator 
	      rnd = libtcod.random_new()
	      # use it 
	      ...
	      # destroy it 
	      libtcod.random_delete(rnd)		
		*/				
		virtual ~TCODRandom();
		
		/**
		@PageName random_use
		@PageFather random
		@PageTitle Using a generator
		@FuncTitle Getting an integer
		@FuncDesc Once you obtained a generator (using one of those methods), you can get random numbers using following functions.
		@Cpp int TCODRandom::getInt(int min, int max)
		@C int TCOD_random_get_int(TCOD_random_t mersenne, int min, int max)
		@Py random_get_int(mersenne, mi, ma)
		@Param mersenne	In the C version, the generator handler, returned by the initialization functions. If NULL, the default generator is used..
		@Param min, max	Range of values returned. Each time you call this function, you get a number between (including) min and max		
		*/		
		int getInt(int min, int max);
		
		/**
		@PageName random_use
		@FuncTitle Getting a float
		@FuncDesc To get a random floating point number :
		@Cpp float TCODRandom::getFloat(float min, float max)
		@C float TCOD_random_get_float(TCOD_random_t mersenne, float min, float max)
		@Py random_get_float(mersenne, mi, ma)
		@Param mersenne	In the C version, the generator handler, returned by the initialization functions. If NULL, the default generator is used.
		@Param min, max	Range of values returned. Each time you call this function, you get a number between (including) min and max
		@CppEx // default generator
	      TCODRandom * default = TCODRandom::getInstance();
	      int aRandomIntBetween0And1000 = default->getInt(0,1000);
	      // another random generator
	      TCODRandom *myRandom = new TCODRandom();
	      float aRandomFloatBetween0And1000 = myRandom->getFloat(0.0f,1000.0f);
		@CEx // default generator
	      int a_random_int_between_0_and_1000 = TCOD_random_get_float(NULL,0,1000);
	      // another random generator
	      TCOD_random_t my_random = TCOD_random_new();
	      float a_random_float_between_0_and_1000 = TCOD_random_get_float(my_random,0.0f,1000.0f);
		@PyEx # default generator 
	      a_random_int_between_0_and_1000 = libtcod.random_get_float(0,0,1000)
	      # another random generator 
	      my_random = libtcod.random_new()
	      a_random_float_between_0_and_1000 = libtcod.random_get_float(my_random,0.0,1000.0)		
		*/		
		float getFloat(float min, float max);

   		/**
		@PageName random_use
		@FuncTitle Getting numbers with a Gaussian distribution
		@FuncDesc To get a random number, either integer or floating point, with an approximated Gaussian distribution:
			Due to the Gaussian distribution, most values are near (min+max)/2
			The integer version of the function will work best at larger deltas (max-min).		
		@Cpp float TCODRandom::getGaussianFloat(float min, float max)
	      int TCODRandom::getGaussianInt(int min, int max)
		@C float TCOD_random_get_gaussian_float(TCOD_random_t mersenne, float min, float max)
	      int TCOD_random_get_gaussian_int(TCOD_random_t mersenne, int min, int max)
		@Py random_get_gaussian_float(mersenne, mi, ma)
	      random_get_gaussian_int(mersenne, mi, ma)
		@Param mersenne	In the C version, the generator handler, returned by the initialization functions. If NULL, the default generator is used.
		@Param min, max	Range of values returned. Each time you call this function, you get a number between (including) min and max.
		*/		
		float getGaussianFloat(float min, float max);
		int getGaussianInt(int min, int max);

		/**
		@PageName random_use
		@FuncTitle Saving a RNG state
		@FuncDesc You can save the state of a generator with :
		@Cpp TCODRandom *TCODRandom::save() const
		@C TCOD_random_t TCOD_random_save(TCOD_random_t mersenne)
		@Py random_save(mersenne)
		@Param mersenne	In the C version, the generator handler, returned by the initialization functions. If NULL, the default generator is used.
		*/		
		TCODRandom * save() const;

		/**
		@PageName random_use
		@FuncTitle Restoring a saved state
		@FuncDesc And restore it later. This makes it possible to get the same serie of number several times with a single generator.
		@Cpp void TCODRandom::restore(const TCODRandom *backup)
		@C void TCOD_random_restore(TCOD_random_t mersenne, TCOD_random_t backup)
		@Py random_restore(mersenne, backup)
		@Param mersenne	In the C version, the generator handler, returned by the initialization functions. If NULL, the default generator is used.
		@CppEx // default generator
	      TCODRandom * default = TCODRandom::getInstance();
	      // save the state
	      TCODRandom *backup=default->save();
	      // get a random number (or several)
	      int number1 = default->getInt(0,1000);
	      // restore the state
	      default->restore(backup);
	      // get a random number
	      int number2 = default->getInt(0,1000);
	      // => number1 == number2
		@CEx // save default generator state 
	      TCOD_random_t backup=TCOD_random_save(NULL);
	      // get a random number 
	      int number1 = TCOD_random_get_float(NULL,0,1000);
	      // restore the state 
	      TCOD_random_restore(NULL,backup);
	      // get a random number 
	      int number2 = TCOD_random_get_float(NULL,0,1000);
	      // number1 == number2 
		@PyEx # save default generator state 
	      backup=libtcod.random_save(0)
	      # get a random number 
	      number1 = libtcod.random_get_float(0,0,1000)
	      # restore the state 
	      libtcod.random_restore(0,backup)
	      # get a random number 
	      number2 = libtcod.random_get_float(0,0,1000)
	      # number1 == number2 		 		
		*/		
		void restore(const TCODRandom *backup);

	protected :
		friend class TCODLIB_API TCODNoise;
		friend class TCODLIB_API TCODHeightMap;
		friend class TCODLIB_API TCODNamegen;
		friend class TCODNameGenerator;	// Used for SWIG interface, does NOT need TCODLIB_API
		TCOD_random_t data;
};

#endif
