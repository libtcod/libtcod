/* libtcod
 * Copyright © 2008-2018 Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The name of copyright holder nor the names of its contributors may not
 *       be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef LIBTCOD_ENGINE_DISPLAY_H_
#define LIBTCOD_ENGINE_DISPLAY_H_

#ifdef __cplusplus
#include <string>
#endif // __cplusplus

#include "../tileset/tileset.h"
#include "../console_types.h"

#ifdef __cplusplus
namespace tcod {
namespace engine {
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
   *  When it is suppered this can be used to switch between a window and
   *  borderless fullscreen window.
   */
  virtual void set_fullscreen(bool fullscreen) = 0;
  /**
   *  Return true if the display is in fullscreen mode.
   *
   *  Returns a negative number if the display does not support fullscreen.
   */
  virtual int get_fullscreen() = 0;
  virtual void present(const TCOD_Console*) = 0;
};
/**
 *  Incomplete interface for subclasses which don't need an SDL2 window.
 */
class TerminalDisplay: public Display {
 public:
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
};
} // namespace sdl2
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
 */
TCODLIB_CAPI void TCOD_console_init_root(int w, int h, const char* title,
                                         bool fullscreen,
                                         TCOD_renderer_t renderer);
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
#endif // LIBTCOD_ENGINE_DISPLAY_H_


