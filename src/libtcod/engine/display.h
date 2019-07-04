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
#ifndef LIBTCOD_ENGINE_DISPLAY_H_
#define LIBTCOD_ENGINE_DISPLAY_H_

#ifdef __cplusplus
#include <array>
#include <string>
#endif // __cplusplus

#include "../color/canvas.h"
#include "../tileset/tileset.h"
#include "../console_types.h"

struct SDL_Rect;
struct SDL_Window;
struct SDL_Renderer;
#ifdef __cplusplus
namespace tcod {
namespace engine {
using tcod::image::Image;
using tcod::tileset::Tileset;
class Display {
 public:
  virtual ~Display() = default;
  virtual void set_tileset(std::shared_ptr<Tileset> tileset) = 0;
  virtual void set_title(const std::string& title) = 0;
  virtual std::string get_title() = 0;
  /**
   *  Set the fullscreen status if the display supports it.
   *
   *  If it is supported this can be used to switch between a window and
   *  borderless fullscreen window.
   */
  virtual void set_fullscreen(bool fullscreen) = 0;
  /**
   *  Return true if the display is in fullscreen mode.
   *
   *  Returns a negative number if the display does not support fullscreen.
   */
  virtual int get_fullscreen() = 0;
  /**
   *  Render a console over this display.
   */
  virtual void accumulate(const TCOD_Console*) = 0;
  virtual void accumulate(const TCOD_Console*, const struct SDL_Rect* viewport) = 0;
  /**
   *  Render a console and present the display.
   */
  virtual void present(const TCOD_Console*) = 0;
  virtual auto pixel_to_tile(const std::array<double, 2>& xy)
      -> std::array<double, 2> = 0;

  virtual auto read_pixels() const -> Image = 0;
  /**
   *  Return the pointer to an SDL2 window, if this display uses SDL.
   *
   *  Could return nullptr.
   */
  virtual auto get_sdl_window() -> struct SDL_Window* = 0;
  /**
   *  Return the pointer to an SDL2 renderer, if this display uses SDL's
   *  renderer.
   *
   *  Could return nullptr.
   */
  virtual auto get_sdl_renderer() -> struct SDL_Renderer* = 0;
};
/**
 *  Incomplete interface for subclasses which don't need an SDL2 window.
 */
class TerminalDisplay: public Display {
 public:
  using Display::accumulate;
  virtual void set_tileset(std::shared_ptr<Tileset>) override
  {}
  virtual void set_title(const std::string&) override
  {}
  virtual std::string get_title() override
  {
    return {};
  }
  /**
   *  Terminals do not support fullscreen modes.
   */
  virtual void set_fullscreen(bool) override
  {}
  /**
   *  Return a negative error code.
   */
  virtual int get_fullscreen() override
  {
    return -1;
  }
  virtual auto pixel_to_tile(const std::array<double, 2>& xy)
      -> std::array<double, 2> override
  {
    return xy;
  }
  virtual auto get_sdl_window() -> struct SDL_Window* override
  {
    return nullptr;
  }
  virtual auto get_sdl_renderer() -> struct SDL_Renderer* override
  {
    return nullptr;
  }
  virtual void accumulate(const TCOD_Console* console, const struct SDL_Rect*) override
  {
    accumulate(console);
  }
};
} // namespace sdl2
namespace console {
TCODLIB_API void init_root(int w, int h, const std::string& title,
                           bool fullscreen, TCOD_renderer_t renderer);
TCODLIB_API void init_root(
    int w,
    int h,
    const std::string& title,
    bool fullscreen,
    TCOD_renderer_t renderer,
    bool vsync);
} // namespace console
} // namespace tcod
#endif // __cplusplus
/**
 *  \brief Initialize the libtcod graphical engine.
 *
 *  \param w The width in tiles.
 *  \param h The height in tiles.
 *  \param title The title for the window.
 *  \param fullscreen Fullscreen option.
 *  \param renderer Which renderer to use when rendering the console.
 *
 *  You may want to call TCOD_console_set_custom_font BEFORE calling this
 *  function.  By default this function loads libtcod's `terminal.png` image
 *  from the working directory.
 *
 *  Afterwards TCOD_quit must be called before the program exits.
 *
 *  Returns 0 on success, or -1 on an error, you can check the error with
 *  TCOD_sys_get_error()
 *
 *  `renderer` and vsync settings can be overridden by the `TCOD_RENDERER` or
 *  `TCOD_VSYNC` environment variables.
 *
 *  Valid case-sensitive options for `TCOD_RENDERER` are:
 *  - sdl
 *  - opengl
 *  - glsl
 *  - sdl2
 *  - opengl2
 *
 *  Valid options for `TCOD_VSYNC` are `0` or `1`.
 *
 *  \rst
 *  .. versionchanged:: 1.12
 *      Now returns -1 on error instead of crashing.
 *
 *  .. versionchanged:: 1.13
 *      Added the `TCOD_RENDERER` and `TCOD_VSYNC` overrides.
 *  \endrst
 */

TCODLIB_CAPI TCOD_NODISCARD int TCOD_console_init_root(
    int w, int h, const char* title, bool fullscreen, TCOD_renderer_t renderer);
TCODLIB_CAPI TCOD_NODISCARD int TCOD_console_init_root_(
    int w,
    int h,
    const char* title,
    bool fullscreen,
    TCOD_renderer_t renderer,
    bool vsync);
/**
 *  Shutdown libtcod.  This must be called before your program exits.
 *  \rst
 *  .. versionadded:: 1.8
 *  \endrst
 */
TCODLIB_CAPI void TCOD_quit(void);
/**
 *  Change the title string of the active window.
 *
 *  \param title A utf8 string.
 */
TCODLIB_CAPI void TCOD_console_set_window_title(const char *title);
/**
 *  Set the display to be full-screen or windowed.
 *
 *  \param fullscreen If true the display will go full-screen.
 */
TCODLIB_CAPI void TCOD_console_set_fullscreen(bool fullscreen);
/**
 *  Return true if the display is full-screen.
 */
TCODLIB_CAPI bool TCOD_console_is_fullscreen(void);
/**
 *  Return true if the window has mouse focus.
 */
TCODLIB_CAPI bool TCOD_console_has_mouse_focus(void);
/**
 *  Return true if the window has keyboard focus.
 *
 *  \verbatim embed:rst:leading-asterisk
 *  .. versionchanged: 1.7
 *      This function was previously broken.  It now keeps track of keyboard
 *      focus.
 *  \endverbatim
 */
TCODLIB_CAPI bool TCOD_console_is_active(void);
/**
 *  Return true if the window is closing.
 */
TCODLIB_CAPI bool TCOD_console_is_window_closed(void);
/**
 *  Return an SDL_Window pointer if one is in use, returns NULL otherwise.
 *  \rst
 *  .. versionadded:: 1.11
 *  \endrst
 */
TCODLIB_CAPI struct SDL_Window* TCOD_sys_get_sdl_window(void);
/**
 *  Return an SDL_Renderer pointer if one is in use, returns NULL otherwise.
 *  \rst
 *  .. versionadded:: 1.11
 *  \endrst
 */
TCODLIB_CAPI struct SDL_Renderer* TCOD_sys_get_sdl_renderer(void);
/**
 *  Render a console over the display.
 *  \rst
 *  `console` can be any size, the active render will try to scale it to fit
 *  the screen.
 *
 *  The function will only work for the SDL2/OPENGL2 renderers.
 *
 *  Unlike :any:`TCOD_console_flush` this will not present the display.
 *  You will need to do that manually, likely with the SDL API.
 *
 *  Returns 0 on success, or a negative number on a failure such as the
 *  incorrect renderer being active.
 *
 *  .. versionadded:: 1.11
 *
 *  .. seealso::
 *      :any:`TCOD_sys_get_sdl_window` :any:`TCOD_sys_get_sdl_renderer`
 *  \endrst
 */
TCODLIB_CAPI int TCOD_sys_accumulate_console(const TCOD_Console* console);
TCODLIB_CAPI int TCOD_sys_accumulate_console_(const TCOD_Console* console, const struct SDL_Rect* viewport);
TCODLIB_CAPI TCOD_NODISCARD int TCOD_sys_init_sdl2_renderer_(
    int width,
    int height,
    const char* title,
    int window_flags,
    int renderer_flags);
#endif // LIBTCOD_ENGINE_DISPLAY_H_
