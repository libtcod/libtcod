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
#ifndef LIBTCOD_SDL2_SDL2_DISPLAY_H_
#define LIBTCOD_SDL2_SDL2_DISPLAY_H_

#ifdef __cplusplus
#include <array>
#include <memory>
#include <string>
#endif // __cplusplus

#include "sdl2_renderer.h"
#include "../engine/display.h"

struct SDL_Window;
struct SDL_Renderer;
#ifdef __cplusplus
namespace tcod {
namespace sdl2 {
using tcod::image::Image;
/**
 *  Incomplete interface for subclasses expecting an SDL2 window.
 */
class WindowedDisplay: public engine::Display {
 public:
  explicit WindowedDisplay(std::shared_ptr<struct SDL_Window> window);
  WindowedDisplay(const std::array<int, 2>& window_size, int window_flags,
                  const std::string& title);
  virtual void set_title(const std::string& title) override;
  virtual std::string get_title() override;
  virtual void set_fullscreen(bool fullscreen) override;
  virtual int get_fullscreen() override;
  /**
   *  Return the tile coordinate of the last console given to present.
   */
  virtual auto pixel_to_tile(const std::array<double, 2>& xy)
      -> std::array<double, 2> override
  {
    return {
        std::get<0>(xy) * std::get<0>(pixel_to_tile_scale),
        std::get<1>(xy) * std::get<1>(pixel_to_tile_scale),
    };
  }
  virtual auto get_sdl_window() -> struct SDL_Window* override
  {
    return window_.get();
  }
  virtual auto get_sdl_renderer() -> struct SDL_Renderer* override
  {
    return nullptr;
  }
 protected:
  /**
   *  Update the scale using a console/tileset and the current window size.
   */
  void update_pixel_to_tile_scale(const TCOD_Console* console);
 private:
  std::shared_ptr<SDL_Window> window_;
  /**
   *  Scale used to convert from pixel coordinate to tile coordinate.
   */
  std::array<double, 2> pixel_to_tile_scale = {1.0, 1.0};
};
/**
 *  Display interface using a basic SDL2 renderer.
 */
class SDL2Display: public WindowedDisplay {
 public:
  SDL2Display(
      std::shared_ptr<Tileset> tileset,
      const std::array<int, 2>& window_size,
      int window_flags,
      const std::string& title,
      int renderer_flags);
  virtual void set_tileset(std::shared_ptr<Tileset> tileset) override;
  virtual void accumulate(const TCOD_Console*) override;
  virtual void accumulate(const TCOD_Console*, const struct SDL_Rect*) override;
  virtual void present(const TCOD_Console*) override;
  virtual auto read_pixels() const -> Image override;
  virtual auto get_sdl_renderer() -> struct SDL_Renderer* override
  {
    return renderer_.get();
  }
 private:
  std::shared_ptr<SDL_Renderer> renderer_;
  SDL2Renderer tcod_renderer_;
};
} // namespace sdl2
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_SDL2_SDL2_DISPLAY_H_
