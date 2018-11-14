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
#ifndef LIBTCOD_SDL2_SDL2_DISPLAY_H_
#define LIBTCOD_SDL2_SDL2_DISPLAY_H_

#ifdef __cplusplus
#include <memory>
#include <string>
#include <utility>
#endif // __cplusplus

#include "sdl2_renderer.h"
#include "../engine/display.h"

struct SDL_Window;
struct SDL_Renderer;
#ifdef __cplusplus
namespace tcod {
namespace sdl2 {
/**
 *  Incomplete interface for subclasses expecting an SDL2 window.
 */
class WindowedDisplay: public engine::Display {
 public:
  WindowedDisplay(std::pair<int, int> window_size, int window_flags,
                  const std::string& title);
  virtual void set_title(const std::string& title) override;
  virtual std::string get_title() override;
  virtual void set_fullscreen(bool fullscreen) override;
  virtual int get_fullscreen() override;
  SDL_Window* get_window() { return window_.get(); }
 private:
  std::shared_ptr<SDL_Window> window_;
};
/**
 *  Display interface using a basic SDL2 renderer.
 */
class SDL2Display: public WindowedDisplay {
 public:
  SDL2Display(
      std::shared_ptr<Tileset> tileset,
      std::pair<int, int> window_size,
      int window_flags,
      const std::string& title);
  virtual void set_tileset(std::shared_ptr<Tileset> tileset) override;
  virtual void present(const TCOD_Console*) override;
 private:
  std::shared_ptr<SDL_Renderer> renderer_;
  SDL2Renderer tcod_renderer_;
};
} // namespace sdl2
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_SDL2_SDL2_DISPLAY_H_
