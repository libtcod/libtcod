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
#ifndef LIBTCOD_SDL2_GL2_DISPLAY_H_
#define LIBTCOD_SDL2_GL2_DISPLAY_H_

#include "gl2_renderer.h"
#include "sdl2_display.h"

namespace tcod {
namespace sdl2 {
class OpenGL2Display: public WindowedDisplay {
 public:
  explicit OpenGL2Display(
      std::shared_ptr<Tileset> tileset,
      std::array<int, 2> window_size,
      int window_flags,
      const std::string& title);
  virtual void set_tileset(std::shared_ptr<Tileset> tileset) override;
  virtual void accumulate(const TCOD_Console*) override;
  virtual void accumulate(const TCOD_Console*, const struct SDL_Rect*) override;
  virtual void present(const TCOD_Console*) override;
  virtual auto read_pixels() const -> Image override;
 private:
  std::shared_ptr<void> glcontext_;
  OpenGL2Renderer tcod_renderer_;
};
} // namespace sdl2
} // namespace tcod
#endif //LIBTCOD_SDL2_GL2_DISPLAY_H_
