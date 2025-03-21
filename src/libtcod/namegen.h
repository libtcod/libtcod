/* BSD 3-Clause License
 *
 * Copyright © 2008-2025, Jice and the libtcod contributors.
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
/*
 * Mingos' NameGen
 * This file was written by Dominik "Mingos" Marczuk.
 */
#pragma once
#ifndef TCOD_NAMEGEN_H_
#define TCOD_NAMEGEN_H_

#include "list.h"
#include "mersenne.h"
#include "portability.h"

#ifdef __cplusplus
extern "C" {
#endif
/* the generator typedef */
struct TCOD_NameGen;
typedef struct TCOD_NameGen* TCOD_namegen_t;

/* parse a file with syllable sets */
TCODLIB_API void TCOD_namegen_parse(const char* filename, TCOD_Random* random);
/* generate a name */
TCODLIB_API char* TCOD_namegen_generate(const char* name, bool allocate);
/* generate a name using a custom generation rule */
TCODLIB_API char* TCOD_namegen_generate_custom(const char* name, const char* rule, bool allocate);
/* retrieve the list of all available syllable set names */
TCODLIB_API TCOD_list_t TCOD_namegen_get_sets(void);
/* delete a generator */
TCODLIB_API void TCOD_namegen_destroy(void);
#ifdef __cplusplus
}
#endif
#endif  // TCOD_NAMEGEN_H_
