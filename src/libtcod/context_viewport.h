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
#ifndef LIBTCOD_CONTEXT_VIEWPORT_H_
#define LIBTCOD_CONTEXT_VIEWPORT_H_

#include <stdbool.h>

#include "color.h"

/**
    Viewport options for the rendering context.
    \rst
    .. versionadded:: 1.16
    \endrst
 */
struct TCOD_ViewportOptions {
  /**
      Must be set to `TCOD_COMPILEDVERSION`.
   */
  int tcod_version;
  /**
      If true then the aspect ratio will be kept square when the console is
      scaled.  The view will be letter-boxed.

      If false the console will be stretched to fill the screen.

      Set this is true if your tileset is deigned for square pixels.
   */
  bool keep_aspect;
  /**
      If true then console scaling will be fixed to integer increments.

      Has no effect if the console must be scaled down.
   */
  bool integer_scaling;
  /**
      The color to clear the screen with before rendering the console.
   */
  TCOD_ColorRGBA clear_color;
  /**
      Alignment of the console when it is letter-boxed: 0.0f renders the
      console in the upper-left corner, and 1.0f in the lower-right.

      Values of 0.5f will center the console.
      Values outside of the range 0.0f to 1.0f are clamped.
   */
  float align_x;
  float align_y;
};
typedef struct TCOD_ViewportOptions TCOD_ViewportOptions;
/**
    Default viewport options if none are provided.
 */
extern const struct TCOD_ViewportOptions TCOD_VIEWPORT_DEFAULT_;
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
/**
    Allocate a new viewport options struct.

    You should not allocate this struct manually since the size of it may change
    between versions.
    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_NODISCARD TCOD_ViewportOptions* TCOD_viewport_new(void);
/**
    Delete a viewport.
    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC void TCOD_viewport_delete(TCOD_ViewportOptions* viewport);
#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
#endif  // LIBTCOD_CONTEXT_VIEWPORT_H_
