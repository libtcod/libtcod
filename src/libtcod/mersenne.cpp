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
#include "mersenne.hpp"

#include <stdlib.h>

#include "libtcod_int.h"

static TCODRandom *instance=(TCODRandom *)NULL;

TCODRandom *TCODRandom::getInstance(void) {
	if (! instance ) {
		instance=new TCODRandom(TCOD_RNG_CMWC,true);
	}
	return instance;
}

TCODRandom::TCODRandom(TCOD_random_algo_t algo, bool allocate) {
	if ( allocate ) data = TCOD_random_new(algo);
}

TCODRandom::TCODRandom(uint32_t seed, TCOD_random_algo_t algo) {
	data=TCOD_random_new_from_seed(algo, seed);
}

TCODRandom::~TCODRandom() {
	TCOD_random_delete(data);
}

TCODRandom *TCODRandom::save() const {
  TCODRandom *ret =
      new TCODRandom(data->algo, false);
	ret->data=TCOD_random_save(data);
	return ret;
}

void TCODRandom::restore(const TCODRandom *backup) {
	TCOD_random_restore(data,backup->data);
}
