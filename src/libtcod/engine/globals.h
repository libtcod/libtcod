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
#ifndef LIBTCOD_ENGINE_GLOBALS_H_
#define LIBTCOD_ENGINE_GLOBALS_H_

#ifdef __cplusplus
#include <memory>
#endif // __cplusplus

#include "../portability.h"
#include "backend.h"
#include "display.h"
#include "../tileset/tileset.h"
#include "../tileset/tilesheet.h"

#ifdef __cplusplus
namespace tcod {
namespace engine {
// All functions here are provisional unless given an added version.
using tileset::Tileset;
using tileset::Tilesheet;
/**
 *  Replace the active backend with a new instance, can be nullptr.
 */
TCODLIB_API void set_backend(std::shared_ptr<Backend> backend);
/**
 *  Return a shared pointer to the active backend.  Might be nullptr.
 */
TCODLIB_API std::shared_ptr<Backend> get_backend();
/**
 *  Replace the active display with a new instance, can be nullptr.
 */
TCODLIB_API void set_display(std::shared_ptr<Display> display);
/**
 *  Return a shared pointer to the active display.  Might be nullptr.
 */
TCODLIB_API std::shared_ptr<Display> get_display();
/**
 *  Replace the active tileset with a new one, can be nullptr.
 *
 *  If there is an active display then its tileset will be set to this one.
 */
TCODLIB_API void set_tileset(std::shared_ptr<Tileset> tileset);
/**
 *  Return the current active tileset.  This might be nullptr.
 */
TCODLIB_API auto get_tileset() -> std::shared_ptr<Tileset>;
TCODLIB_API void set_tilesheet(std::shared_ptr<Tilesheet> sheet);
TCODLIB_API auto get_tilesheet() -> std::shared_ptr<Tilesheet>;
} // namespace sdl2
} // namespace tcod
#endif // __cplusplus
/**
 *  Return the default tileset, may be NULL.
 *
 *  This function is provisional, the API may change in the future.
 */
TCODLIB_CAPI TCOD_Tileset* TCOD_get_default_tileset(void);
/**
 *  Set the default tileset and update the default display to use it.
 *
 *  This function is provisional, the API may change in the future.
 */
TCODLIB_CAPI void TCOD_set_default_tileset(TCOD_Tileset* tileset);
#endif // LIBTCOD_ENGINE_GLOBALS_H_


