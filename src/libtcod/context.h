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
#ifndef LIBTCOD_CONTEXT_H_
#define LIBTCOD_CONTEXT_H_

#ifdef __cplusplus
#include <algorithm>
#include <array>
#include <memory>
#include <stdexcept>
#endif  // __cplusplus
#include "config.h"
#include "console.h"
#include "context_viewport.h"
#include "error.h"
#include "tileset.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Rect;

struct TCOD_Context;  // Defined in this header later.
typedef struct TCOD_Context TCOD_Context;
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
/**
 *  Delete a rendering context.
 *  \rst
 *  .. versionadded:: 1.16
 *  \endrst
 */
TCOD_PUBLIC void TCOD_context_delete(struct TCOD_Context* renderer);
/**
 *  Create an uninitialized rendering context.
 *
 *  Used internally.
 */
TCOD_NODISCARD struct TCOD_Context* TCOD_context_new_(void);
/**
    Present a console to the screen, using a rendering context.

    `console` is the console to present, the console can be any size.

    `viewport` is the optional viewport options to use.
    This will affect the scaling of the console with the current context.
    This can be NULL to use the default options, which are to stretch the
    console to fit the screen.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_context_present(
    struct TCOD_Context* context, const struct TCOD_Console* console, const struct TCOD_ViewportOptions* viewport);
/**
    Convert the screen coordinates to tile coordinates for this context.

    `x` and `y` are the pointers to the screen coordinates, these will be
    converted to tile coordinates after the call to this function.

    The parameters given to the last call to `TCOD_context_present` will
    determine where the tiles are for this call.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_context_screen_pixel_to_tile_d(struct TCOD_Context* context, double* x, double* y);
/**
    Convert the screen coordinates to integer tile coordinates for this context.

    Save as `TCOD_context_screen_pixel_to_tile` but the inputs and results are
    integers.  This is useful if you don't need sub-tile coordinates.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_context_screen_pixel_to_tile_i(struct TCOD_Context* context, int* x, int* y);
/**
    Save the last presented console to a PNG file.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_context_save_screenshot(struct TCOD_Context* context, const char* filename);
/**
    Return a pointer the SDL_Window for this context if it uses one.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC struct SDL_Window* TCOD_context_get_sdl_window(struct TCOD_Context* context);
/**
    Return a pointer the SDL_Renderer for this context if it uses one.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC struct SDL_Renderer* TCOD_context_get_sdl_renderer(struct TCOD_Context* context);
/**
    Change the active tileset for this context.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_context_change_tileset(struct TCOD_Context* self, TCOD_Tileset* tileset);
/**
    Return the `TCOD_renderer_t` renderer type for this context.

    Returns a negative number on error, such as `context` being NULL.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC int TCOD_context_get_renderer_type(struct TCOD_Context* context);
/**
    Set `columns` and `rows` to the recommended console size for this context.

    `magnification` determines the apparent size of the tiles on the output.
    Values of 0.0f or lower will default to 1.0f.

    \rst
    .. versionadded:: 1.16
    \endrst
 */
TCOD_PUBLIC TCOD_Error TCOD_context_recommended_console_size(
    struct TCOD_Context* context, float magnification, int* __restrict columns, int* __restrict rows);
#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
/**
    A rendering context for libtcod.
    \rst
    .. versionadded:: 1.16
    \endrst
 */
struct TCOD_Context {
#ifdef __cplusplus
  /**
      Return the TCOD_renderer_t value of this context which may be diffent
      than the one requested.
   */
  auto get_renderer_type() noexcept -> int { return TCOD_context_get_renderer_type(this); }
  /**
      Present a console to the display with the provided viewport options.
   */
  void present(const TCOD_Console& console, const TCOD_ViewportOptions& viewport) {
    tcod::check_throw_error(TCOD_context_present(this, &console, &viewport));
  }
  /**
      Present a console to the display.
   */
  void present(const TCOD_Console& console) { tcod::check_throw_error(TCOD_context_present(this, &console, nullptr)); }
  /**
      Return a non-owning pointer to the SDL_Window used by this context.

      May return nullptr.
   */
  auto get_sdl_window() noexcept -> struct SDL_Window* { return TCOD_context_get_sdl_window(this); }
  /**
      Return a non-owning pointer to the SDL_Renderer used by this context.

      May return nullptr.
   */
  auto get_sdl_renderer() noexcept -> struct SDL_Renderer* { return TCOD_context_get_sdl_renderer(this); }
  /**
      Convert pixel coordinates to this contexts integer tile coordinates.
   */
  auto pixel_to_tile_coordinates(const std::array<int, 2>& xy) -> std::array<int, 2> {
    std::array<int, 2> out{xy[0], xy[1]};
    tcod::check_throw_error(TCOD_context_screen_pixel_to_tile_i(this, &out[0], &out[1]));
    return out;
  }
  /**
      Convert pixel coordinates to this contexts sub-tile coordinates.
   */
  auto pixel_to_tile_coordinates(const std::array<double, 2>& xy) -> std::array<double, 2> {
    std::array<double, 2> out{xy[0], xy[1]};
    tcod::check_throw_error(TCOD_context_screen_pixel_to_tile_d(this, &out[0], &out[1]));
    return out;
  }
  /**
      Save a screenshot to `filepath`.

      If `filepath` is nullptr then a unique file name will be generated.
   */
  void save_screenshot(const char* filepath) { tcod::check_throw_error(TCOD_context_save_screenshot(this, filepath)); }
  /**
      Save a screenshot to `filepath`.
   */
  void save_screenshot(const std::string& filepath) { return this->save_screenshot(filepath.data()); }
  /**
      Return a new console with a size automatically determined by the context.

      `min_columns` and `min_rows` are the minimum size to use for the new
      console.

      `magnification` determines the apparent size of the tiles that will
      be rendered by a console created with the output values.  A
      `magnification` larger then 1.0f will output smaller console parameters,
      which will show as larger tiles when presented.
      Only values larger than zero are allowed.
   */
  auto new_console(int min_columns = 1, int min_rows = 1, float magnification = 1.0f) -> tcod::ConsolePtr {
    int columns;
    int rows;
    if (magnification <= 0.0f) {
      throw std::invalid_argument(
          std::string("Magnification must be greater than zero. Got ") + std::to_string(magnification));
    }
    tcod::check_throw_error(TCOD_context_recommended_console_size(this, magnification, &columns, &rows));
    return tcod::new_console(std::max(columns, min_columns), std::max(rows, min_rows));
  }
#endif  // __cplusplus
  // All remaining members are private.
  /**
      The TCOD_renderer_t value of this context.
   */
  int type;
  /**
      A pointer to this contexts unique data.
   */
  void* __restrict contextdata_;
  // Context C callback are prefixed with 'c_', always check if see if these are NULL.
  /**
      Called when this context is deleted.
   */
  void (*c_destructor_)(struct TCOD_Context* __restrict self);
  /**
      Called to present a console to a contexts display.

      `console` must not be NULL.

      `viewport` may be NULL.
   */
  TCOD_Error (*c_present_)(
      struct TCOD_Context* __restrict self,
      const struct TCOD_Console* __restrict console,
      const struct TCOD_ViewportOptions* __restrict viewport);
  /**
      Convert pixel coordinates to the contexts tile coordinates.
   */
  void (*c_pixel_to_tile_)(struct TCOD_Context* __restrict self, double* __restrict x, double* __restrict y);
  /**
      Ask this context to save a screenshot.
   */
  TCOD_Error (*c_save_screenshot_)(struct TCOD_Context* __restrict self, const char* __restrict filename);
  /**
      Return this contexts SDL_Window, if any.
   */
  struct SDL_Window* (*c_get_sdl_window_)(struct TCOD_Context* __restrict self);
  /**
      Return this contexts SDL_Renderer, if any.
   */
  struct SDL_Renderer* (*c_get_sdl_renderer_)(struct TCOD_Context* __restrict self);
  /**
      Draw a console without flipping the display.
      This method of drawing is deprecated.
   */
  TCOD_Error (*c_accumulate_)(
      struct TCOD_Context* __restrict self,
      const struct TCOD_Console* __restrict console,
      const struct TCOD_ViewportOptions* __restrict viewport);
  /**
      Change the tileset used by this context.
  */
  TCOD_Error (*c_set_tileset_)(struct TCOD_Context* __restrict self, TCOD_Tileset* __restrict tileset);
  /**
      Output the recommended console size to `columns` and `rows`.

      `magnification` determines the apparent size of tiles,
      but might be ignored.
  */
  TCOD_Error (*c_recommended_console_size_)(
      struct TCOD_Context* __restrict self, float magnification, int* __restrict columns, int* __restrict rows);
};
#ifdef __cplusplus
namespace tcod {
struct ContextDeleter {
  void operator()(TCOD_Context* console) const { TCOD_context_delete(console); }
};
typedef std::unique_ptr<TCOD_Context, ContextDeleter> ContextPtr;
typedef std::shared_ptr<TCOD_Context> ContextSharedPtr;
}  // namespace tcod
#endif  // __cplusplus
#endif  // LIBTCOD_CONTEXT_H_
