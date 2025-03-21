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
#pragma once
#ifndef LIBTCOD_GLOBALS_H_
#define LIBTCOD_GLOBALS_H_

#include "config.h"
#include "tileset.h"
/**
    Return the default tileset, may be NULL.

    A non-NULL return value is a new reference to the global tileset.
    When you are done you will need to call `TCOD_tileset_delete` on this
    pointer.

    This function is provisional, the API may change in the future.
    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCODLIB_CAPI TCOD_Tileset* TCOD_get_default_tileset(void);
/**
    Set the default tileset and update the default display to use it.

    This will keep alive a reference to the given tileset.  If you no longer
    need the pointer then you should call `TCOD_tileset_delete` on it after
    this function.

    This function is provisional, the API may change in the future.
    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCODLIB_CAPI void TCOD_set_default_tileset(TCOD_Tileset* tileset);
#endif  // LIBTCOD_GLOBALS_H_
