/* libtcod
 * Copyright © 2008-2018 Jice and the libtcod contributers.
 * All rights reserved.
 *
 * libtcod 'The Doryen library' is a cross-platform C/C++ library for roguelike
 * developers.
 * Its source code is available from:
 * https://github.com/libtcod/libtcod
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

OpenGL2Renderer::OpenGL2Renderer(OpenGLTilesetAlias alias)
: TilesetObserver(alias.get_tileset()),
  alias_{alias},
  program_(
#include "console_2tris.glslv"
      ,
#include "console_2tris.glslf"
  )
{
}
void OpenGL2Renderer::render(const TCOD_Console* console)
{
  glClear(GL_COLOR_BUFFER_BIT); gl_check();
  program_.use(); gl_check();
  GLBuffer v_buffer(
      GL_ARRAY_BUFFER,
      std::array<int8_t, 8>{0, 0, 0, 1, 1, 0, 1, 1},
      GL_STATIC_DRAW
  );
  const int vertex_id = glGetAttribLocation(program_.get(), "a_vertex");
  glVertexAttribPointer(vertex_id, 2, GL_BYTE, GL_FALSE, 0, 0); gl_check();
  glEnableVertexAttribArray(vertex_id); gl_check();

  glEnableVertexAttribArray(vertex_id); gl_check();

  const std::array<float, 4*4> matrix{
    2, 0, 0, 0,
    0, 2, 0, 0,
    0, 0, 1, 0,
    -1, -1, 0, 1,
  };
  glUniformMatrix4fv(program_.get_uniform("mvp_matrix"),
                     1, GL_FALSE, matrix.data()); gl_check();

  glUniform2i(program_.get_uniform("v_console_shape"), console->w, console->h);
  glUniform2iv(program_.get_uniform("v_tiles_shape"), 1,
               alias_.get_alias_shape().data());

  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, alias_.get_alias_texture());
  glUniform1i(program_.get_uniform("t_tileset"), 0);

  GLTexture bg_tex{};
  glActiveTexture(GL_TEXTURE0 + 1);
  glBindTexture(GL_TEXTURE_2D, bg_tex.get());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, console->w, console->h, 0, GL_RGB,
               GL_UNSIGNED_BYTE, console->bg_array);
  glUniform1i(program_.get_uniform("t_console_bg"), 1);

  GLTexture fg_tex;
  glActiveTexture(GL_TEXTURE0 + 2);
  glBindTexture(GL_TEXTURE_2D, fg_tex.get());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, console->w, console->h, 0, GL_RGB,
               GL_UNSIGNED_BYTE, console->fg_array);
  glUniform1i(program_.get_uniform("t_console_fg"), 2);

  GLTexture ch_tex;
  glActiveTexture(GL_TEXTURE0 + 3);
  glBindTexture(GL_TEXTURE_2D, ch_tex.get());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  std::vector<uint8_t> tile_indexes;
  for (int i = 0; i < console->w * console->h; ++i){
    auto tile = alias_.get_tile_position(console->ch_array[i]);
    tile_indexes.emplace_back(tile.at(0) & 0xff);
    tile_indexes.emplace_back(tile.at(0) >> 8);
    tile_indexes.emplace_back(tile.at(1) & 0xff);
    tile_indexes.emplace_back(tile.at(1) >> 8);
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, console->w, console->h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, tile_indexes.data());
  glUniform1i(program_.get_uniform("t_console_tile"), 3);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  gl_check();
}
} // namespace sdl2
} // namespace tcod
