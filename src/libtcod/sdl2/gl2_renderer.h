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
#ifndef LIBTCOD_SDL2_GL2_RENDERER_H_
#define LIBTCOD_SDL2_GL2_RENDERER_H_
#ifdef __cplusplus
#endif /* __cplusplus */
#include <memory>

#include "gl_alias.h"
#include "gl2_raii.h"
#include "../color/color.h"
#include "../console_types.h"
#include "../tileset/observer.h"
#include "../tileset/tileset.h"
#include "../utility/vector2.h"
#ifdef __cplusplus
namespace tcod {
namespace sdl2 {
using tcod::image::Image;
using tcod::tileset::Tileset;
class OpenGL2Renderer {
 public:
  explicit OpenGL2Renderer(OpenGLTilesetAlias alias);
  explicit OpenGL2Renderer(std::shared_ptr<Tileset> tileset)
  : OpenGL2Renderer(OpenGLTilesetAlias(tileset))
  {}
  OpenGL2Renderer(const OpenGL2Renderer&) = delete;
  OpenGL2Renderer& operator=(const OpenGL2Renderer&) = delete;
  OpenGL2Renderer(OpenGL2Renderer&&) noexcept;
  OpenGL2Renderer& operator=(OpenGL2Renderer&&) noexcept;
  ~OpenGL2Renderer() noexcept;

  void render(const TCOD_Console* console);
  auto read_pixels() const -> Image;
 private:
  class impl;
  std::unique_ptr<impl> impl_;
};
} // namespace sdl2
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_SDL2_GL2_RENDERER_H_
