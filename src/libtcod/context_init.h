/* BSD 3-Clause License
 *
 * Copyright © 2008-2021, Jice and the libtcod contributors.
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
#ifndef LIBTCOD_CONTEXT_INIT_H_
#define LIBTCOD_CONTEXT_INIT_H_
#include <stdbool.h>

#include "config.h"
#include "context.h"
#include "error.h"
#include "tileset.h"
/**
    A struct of paramers used to create a new context with `TCOD_context_new`.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
typedef struct TCOD_ContextParams {
  /**
      Must be `TCOD_COMPILEDVERSION`.
   */
  int tcod_version;
  /**
      `window_x` and `window_y` are the starting position of the window.
      These are SDL parameters so values like `SDL_WINDOWPOS_UNDEFINED` and
      `SDL_WINDOWPOS_CENTERED` are acceptable.

      Values of zero will be converted to `SDL_WINDOWPOS_UNDEFINED` unless
      `window_xy_defined` is true.
   */
  int window_x, window_y;
  /**
      `pixel_width` and `pixel_height` are the desired size of the window in pixels.
      If these are zero then they'll be derived from `columns`, `rows`, and the `tileset`.
   */
  int pixel_width, pixel_height;
  /**
      `columns` and `rows` are the desired size of the terminal window.
      Usually you'll set either these or the pixel resolution.

      If you are setting these values from a TCOD_Console then you should set the console attribute instead.
   */
  int columns, rows;
  /**
      `renderer_type` is one of the `TCOD_renderer_t` values.
   */
  int renderer_type;
  /**
      `tileset` is an optional pointer to a tileset object.
      If this is NULL then a platform specific fallback tileset will be used.
      This fallback is known to be unreliable, but it should work well enough
      for prototyping code.
   */
  TCOD_Tileset* tileset;
  /**
      If `vsync` is true, then vertical sync will be enabled whenever possible.
      A value of true is recommended.
   */
  int vsync;
  /**
      `sdl_window_flags` is a bitfield of SDL_WindowFlags flags.
      For a window, a value of ``SDL_WINDOW_RESIZABLE`` is recommended.
      For fullscreen, a value of
      ``SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP`` is recommended.
      You should avoid the ``SDL_WINDOW_FULLSCREEN`` flag whenever possible.
   */
  int sdl_window_flags;
  /**
      `window_title` will be the title of the opened window.

      If not set then `argv[0]` will be used if available.
   */
  const char* window_title;
  /**
      The number of items in `argv`.
   */
  int argc;
  /**
      `argc` and `argv` are optional CLI parameters.
      You can pass `0` and `NULL` respectfully to ignore them.
      If unsure then you should pass the `argc` and `argv` arguments from your
      `main` function.
   */
  const char* const* argv;
  /**
      If user attention is required for the given CLI parameters then
      `cli_output` will be called with `cli_userdata` and an error or help
      message.
      If `cli_output` is NULL then it will print the message to stdout and
      terminate the program.  If `cli_output` returns normally then
      TCOD_E_REQUIRES_ATTENTION will be returned from `TCOD_context_new`.
   */
  void (*cli_output)(void* userdata, const char* output);
  /**
      This is passed to the `userdata` parameter of `cli_output` if called.
   */
  void* cli_userdata;
  /**
      If this is false then `window_x`/`window_y` parameters of zero are
      assumed to be undefined and will be changed to `SDL_WINDOWPOS_UNDEFINED`.
   */
  bool window_xy_defined;
  /***************************************************************************
      @brief A console to be used as a reference for the desired window size.
      This can set as an alternative to the columns and rows attributes.

      \rst
      .. versionadded:: 1.19
      \endrst
   */
  TCOD_Console* console;
} TCOD_ContextParams;
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
TCOD_NODISCARD inline auto new_context(const TCOD_ContextParams& params, TCOD_Error& out_code) -> ContextPtr {
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
TCOD_NODISCARD inline auto new_context(const TCOD_ContextParams& params) -> ContextPtr {
  TCOD_Error discard_output;
  return new_context(params, discard_output);
}
}  // namespace tcod
#endif  // __cplusplus
#endif  // LIBTCOD_CONTEXT_INIT_H_
