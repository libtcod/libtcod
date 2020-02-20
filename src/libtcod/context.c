/* BSD 3-Clause License
 *
 * Copyright © 2008-2020, Jice and the libtcod contributors.
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
#include "context.h"

#include "stdlib.h"

struct TCOD_Context* TCOD_context_new_(void)
{
  struct TCOD_Context* renderer = calloc(sizeof(*renderer), 1);
  return renderer;
}

void TCOD_context_delete(struct TCOD_Context* renderer)
{
  if (!renderer) { return; }
  if (renderer->destructor_) { renderer->destructor_(renderer); }
  free(renderer);
}

TCOD_PUBLIC TCOD_Error TCOD_context_present(
    struct TCOD_Context* context,
    const struct TCOD_Console* console,
    const struct TCOD_ViewportOptions* viewport)
{
  if (!context) {
    TCOD_set_errorv("Context must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!console) {
    TCOD_set_errorv("Console must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  return context->present_(context, console, viewport);
}
