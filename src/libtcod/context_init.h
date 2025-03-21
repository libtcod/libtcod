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
#ifndef LIBTCOD_CONTEXT_INIT_H_
#define LIBTCOD_CONTEXT_INIT_H_
#include <stdbool.h>

#include "config.h"
#include "context.h"
#include "error.h"
#ifndef NO_SDL
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
/**
    Create a new context with the given parameters.

    `params` is a non-NULL pointer to a TCOD_ContextParams struct.
    See its documentation for info on the parameters.

    `out` is the output for the `TCOD_Context`, must not be NULL.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_NODISCARD TCOD_Error TCOD_context_new(const TCOD_ContextParams* params, TCOD_Context** out);
#ifdef __cplusplus
}  // extern "C"
namespace tcod {
/**
    @brief Initialize and return a new libtcod context.  Also returns an error code for non-critical issues.

    @param params Options to configure the new context with.
    @param out_code Will be set to an error code on non-critical issues.
    @return ContextPtr A pointer to the new context.

    For critical issues an exception is thrown as usual.
    Non-critical issues are things such as being unable to create a desired renderer and using to a fallback instead.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
[[deprecated("This function has been replaced by `auto context = tcod::Context(params);`")]] TCOD_NODISCARD inline auto
new_context(const TCOD_ContextParams& params, TCOD_Error& out_code) -> ContextPtr {
  struct TCOD_Context* context = nullptr;
  out_code = check_throw_error(TCOD_context_new(&params, &context));
  return ContextPtr{context};
}
/**
    @brief Initialize and return a new libtcod context.

    @param params Options to configure the new context with.
    @return ContextPtr A pointer to the new context.
    \rst
    .. versionadded:: 1.19
    \endrst
 */
[[deprecated("This function has been replaced by `auto context = tcod::Context(params);`")]] TCOD_NODISCARD inline auto
new_context(const TCOD_ContextParams& params) -> ContextPtr {
  struct TCOD_Context* context = nullptr;
  check_throw_error(TCOD_context_new(&params, &context));
  return ContextPtr{context};
}
}  // namespace tcod
#endif  // __cplusplus
#endif  // NO_SDL
#endif  // LIBTCOD_CONTEXT_INIT_H_
