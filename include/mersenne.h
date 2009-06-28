/*
* libtcod 1.4.2
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

#ifndef _TCOD_RANDOM_H
#define _TCOD_RANDOM_H

typedef void *TCOD_random_t;

TCODLIB_API TCOD_random_t TCOD_random_get_instance();
TCODLIB_API TCOD_random_t TCOD_random_new();
TCODLIB_API TCOD_random_t TCOD_random_save(TCOD_random_t mersenne);
TCODLIB_API void TCOD_random_restore(TCOD_random_t mersenne, TCOD_random_t backup);
TCODLIB_API TCOD_random_t TCOD_random_new_from_seed(uint32 seed);
TCODLIB_API int TCOD_random_get_int(TCOD_random_t mersenne, int min, int max);
TCODLIB_API float TCOD_random_get_float(TCOD_random_t mersenne, float min, float max);
TCODLIB_API int TCOD_random_get_int_from_byte_array(int min, int max, const char *data,int len);
TCODLIB_API void TCOD_random_delete(TCOD_random_t mersenne);

#endif
