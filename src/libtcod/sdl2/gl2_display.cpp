/* libtcod
 * Copyright © 2008-2019 Jice and the libtcod contributors.
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
#include "gl2_display.h"

#include "gl2_ext_.h"
#include <SDL.h>

namespace tcod {
namespace sdl2 {
static std::shared_ptr<void> new_gl_context(OpenGL2Display& self)
{
  std::shared_ptr<void> new_context(
      SDL_GL_CreateContext(self.get_window()),
      [](SDL_GLContext context){ SDL_GL_DeleteContext(context); });
  if (!new_context) { throw std::runtime_error(SDL_GetError()); }
  if(!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    throw std::runtime_error("Failed to invoke the GLAD loader.");
  }
  SDL_GL_SetSwapInterval(0);
  return new_context;
}
OpenGL2Display::OpenGL2Display(std::shared_ptr<Tileset> tileset,
                         std::pair<int, int> window_size, int window_flags,
                         const std::string& title)
: WindowedDisplay(window_size, window_flags | SDL_WINDOW_OPENGL, title),
  glcontext_(new_gl_context(*this)),
  tcod_renderer_(tileset)
{}
void OpenGL2Display::set_tileset(std::shared_ptr<Tileset> tileset)
{
  if (!tileset) {
    throw std::invalid_argument("tileset must not be nullptr.");
  }
  tcod_renderer_ = OpenGL2Renderer(tileset);
}
void OpenGL2Display::present(const TCOD_Console* console)
{
  int window_size[2];
  SDL_GL_GetDrawableSize(get_window(), &window_size[0], &window_size[1]);
  glViewport(0,0,window_size[0],window_size[1]);

  tcod_renderer_.render(console);
  SDL_GL_SwapWindow(get_window());

  update_pixel_to_tile_scale(console);
}
auto OpenGL2Display::read_pixels() const -> Image
{
  return tcod_renderer_.read_pixels();
}
} // namespace sdl2
} // namespace tcod
