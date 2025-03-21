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
#include "context_viewport.h"

#include <stdlib.h>
#include <string.h>

#include "error.h"

const struct TCOD_ViewportOptions TCOD_VIEWPORT_DEFAULT_ = {
    .tcod_version = TCOD_COMPILEDVERSION,
    .keep_aspect = false,
    .integer_scaling = false,
    .clear_color = {0, 0, 0, 255},
    .align_x = 0.5f,
    .align_y = 0.5f,
};

TCOD_ViewportOptions* TCOD_viewport_new(void) {
  TCOD_ViewportOptions* viewport = malloc(sizeof(*viewport));
  if (!viewport) {
    TCOD_set_errorv("Could not allocate memory.");
    return NULL;
  }
  memcpy(viewport, &TCOD_VIEWPORT_DEFAULT_, sizeof(*viewport));
  return viewport;
}
void TCOD_viewport_delete(TCOD_ViewportOptions* viewport) { free(viewport); }
