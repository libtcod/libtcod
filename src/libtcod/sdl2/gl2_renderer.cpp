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
#include "gl2_renderer.h"

#include <array>
#include <string>
#include <utility>

#include "gl2_ext_.h"
#include "../console.h"

namespace tcod {
namespace sdl2 {

class OpenGL2Renderer::impl : public TilesetObserver {
 public:
  explicit impl(OpenGLTilesetAlias alias)
  : TilesetObserver(alias.get_tileset()),
    alias_(alias),
    program_(
#include "console_2tris.glslv"
        ,
#include "console_2tris.glslf"
    ),
    vertex_buffer_(
        GL_ARRAY_BUFFER,
        std::array<int8_t, 8>{0, 0, 0, 1, 1, 0, 1, 1},
        GL_STATIC_DRAW
    )
  {
    program_.use(); gl_check();

    vertex_buffer_.bind();
    const int a_vertex = program_.get_attribute("a_vertex");
    glVertexAttribPointer(a_vertex, 2, GL_BYTE, GL_FALSE, 0, 0); gl_check();
    glEnableVertexAttribArray(a_vertex); gl_check();

    const std::array<float, 4*4> matrix{
      2, 0, 0, 0,
      0, 2, 0, 0,
      0, 0, 1, 0,
      -1, -1, 0, 1,
    };
    glUniformMatrix4fv(program_.get_uniform("mvp_matrix"),
                       1, GL_FALSE, matrix.data()); gl_check();

    bg_tex_.bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    fg_tex_.bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    ch_tex_.bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  void render(const TCOD_Console* console)
  {
    //glClear(GL_COLOR_BUFFER_BIT); gl_check();
    program_.use(); gl_check();

    glUniform2fv(program_.get_uniform("v_tiles_shape"), 1,
                 alias_.get_alias_shape().data());
    glUniform2fv(program_.get_uniform("v_tiles_size"), 1,
                 alias_.get_alias_size().data());

    if (cached_size.first != console->w || cached_size.second != console->h) {
      cached_size = {console->w, console->h};
      glUniform2i(program_.get_uniform("v_console_shape"),
                  console->w, console->h);
      bg_tex_.bind();
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, console->w, console->h, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, nullptr);
      fg_tex_.bind();
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, console->w, console->h, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, nullptr);
      ch_tex_.bind();
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, console->w, console->h, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, nullptr);
    }

    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, alias_.get_alias_texture(*console));
    glUniform1i(program_.get_uniform("t_tileset"), 0);

    glActiveTexture(GL_TEXTURE0 + 1);
    bg_tex_.bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, console->w, console->h, GL_RGB,
                    GL_UNSIGNED_BYTE, console->bg_array);
    glUniform1i(program_.get_uniform("t_console_bg"), 1);

    glActiveTexture(GL_TEXTURE0 + 2);
    fg_tex_.bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, console->w, console->h, GL_RGB,
                    GL_UNSIGNED_BYTE, console->fg_array);
    glUniform1i(program_.get_uniform("t_console_fg"), 2);

    glActiveTexture(GL_TEXTURE0 + 3);
    ch_tex_.bind();
    std::vector<uint8_t> tile_indexes(console->w * console->h * 4);
    int tile_i = 0;
    for (int i = 0; i < console->w * console->h; ++i) {
      auto tile = alias_.get_tile_position(console->ch_array[i]);
      tile_indexes[tile_i++] = tile.at(0) & 0xff;
      tile_indexes[tile_i++] = tile.at(0) >> 8;
      tile_indexes[tile_i++] = tile.at(1) & 0xff;
      tile_indexes[tile_i++] = tile.at(1) >> 8;
    }
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, console->w, console->h, GL_RGBA,
                    GL_UNSIGNED_BYTE, tile_indexes.data());
    glUniform1i(program_.get_uniform("t_console_tile"), 3);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    gl_check();
  }
  auto read_pixels() const -> Image
  {
    return {};
  }
  void on_tileset_changed(
      const std::vector<std::pair<int, Tile&>> &changes) override
  {}
 private:
  OpenGLTilesetAlias alias_;
  GLProgram program_;
  GLBuffer vertex_buffer_;
  GLTexture ch_tex_;
  GLTexture fg_tex_;
  GLTexture bg_tex_;
  std::pair<int, int> cached_size{-1, -1};
};

OpenGL2Renderer::OpenGL2Renderer(OpenGLTilesetAlias alias)
: impl_(std::make_unique<impl>(alias))
{}
OpenGL2Renderer::OpenGL2Renderer(OpenGL2Renderer&&) noexcept = default;
OpenGL2Renderer& OpenGL2Renderer::operator=(OpenGL2Renderer&&) noexcept = default;
OpenGL2Renderer::~OpenGL2Renderer() noexcept = default;
void OpenGL2Renderer::render(const TCOD_Console* console)
{
  impl_->render(console);
}
auto OpenGL2Renderer::read_pixels() const -> Image
{
  return impl_->read_pixels();
}
} // namespace sdl2
} // namespace tcod
