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
#include "gl2_renderer.h"

#include <array>
#include <string>
#include <utility>

#include "gl2_ext_.h"
#include "../utility/matrix.h"
#include "../console.h"
#include "../libtcod_int.h"

#define SHADER_STRINGIFY(...) #__VA_ARGS__

namespace tcod {
namespace sdl2 {
/**
 *  Return a number rounded up to a power of 2.
 */
int round_to_pow2(int i)
{
  if (i <= 0) { i = 1; }
  i |= i >> 1;
  i |= i >> 2;
  i |= i >> 4;
  i |= i >> 8;
  return ++i;
}

class TwoTranglesRenderer {
 public:
  explicit TwoTranglesRenderer(OpenGLTilesetAlias alias)
  : alias_(alias),
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
  /**
   *  Upload color data to the currently bound texture.
   */
  template<typename F>
  void upload_colors(const TCOD_Console& console, const F& color_at)
  {
    std::vector<TCOD_ColorRGBA> tile_colors;
    tile_colors.reserve(console.size());
    for (int y = 0; y < console.h; ++y) {
      for (int x = 0; x < console.w; ++x) {
        tile_colors.emplace_back(color_at(y, x));
      }
    }
    glTexSubImage2D(
        GL_TEXTURE_2D, 0, 0, 0, console.w, console.h, GL_RGBA,
        GL_UNSIGNED_BYTE, tile_colors.data());
  }
  /**
   *  Upload glyph positions to the currently bound texture.
   */
  void upload_characters(const TCOD_Console& console)
  {
    std::vector<uint8_t> tile_indexes;
    tile_indexes.reserve(console.size() * 4);
    for (int i = 0; i < console.size(); ++i) {
      auto tile = alias_.get_tile_position(console.tiles[i].ch);
      tile_indexes.emplace_back(tile.at(0) & 0xff);
      tile_indexes.emplace_back(tile.at(0) >> 8);
      tile_indexes.emplace_back(tile.at(1) & 0xff);
      tile_indexes.emplace_back(tile.at(1) >> 8);
    }
    glTexSubImage2D(
        GL_TEXTURE_2D, 0, 0, 0, console.w, console.h, GL_RGBA,
        GL_UNSIGNED_BYTE, tile_indexes.data());
  }
  void render(const TCOD_Console& console)
  {
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    glDisable(GL_SAMPLE_COVERAGE);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_STENCIL_TEST);
    //glClear(GL_COLOR_BUFFER_BIT); gl_check();
    program_.use(); gl_check();

    glUniform2fv(program_.get_uniform("v_tiles_shape"), 1,
                 alias_.get_alias_shape().data());
    glUniform2fv(program_.get_uniform("v_tiles_size"), 1,
                 alias_.get_alias_size().data());

    if (console.h > cached_size_[0] || console.w > cached_size_[1]) {
      cached_size_ = {console.h, console.w};
      int tex_width = round_to_pow2(console.w);
      int tex_height = round_to_pow2(console.h);

      glUniform2f(program_.get_uniform("v_console_shape"),
                  tex_width, tex_height);
      glUniform2f(program_.get_uniform("v_console_size"),
                  static_cast<float>(console.w) / tex_width,
                  static_cast<float>(console.h) / tex_height);
      bg_tex_.bind();
      glTexImage2D(
          GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA,
          GL_UNSIGNED_BYTE, nullptr);
      fg_tex_.bind();
      glTexImage2D(
          GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA,
          GL_UNSIGNED_BYTE, nullptr);
      ch_tex_.bind();
      glTexImage2D(
          GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA,
          GL_UNSIGNED_BYTE, nullptr);
    }

    glPixelStorei(GL_PACK_ALIGNMENT, 4);

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, alias_.get_alias_texture(console));
    glUniform1i(program_.get_uniform("t_tileset"), 0);

    glActiveTexture(GL_TEXTURE0 + 1);
    bg_tex_.bind();
    glUniform1i(program_.get_uniform("t_console_bg"), 1);
    upload_colors(console, [&](int y, int x){ return console.at(y, x).bg; });

    glActiveTexture(GL_TEXTURE0 + 2);
    fg_tex_.bind();
    glUniform1i(program_.get_uniform("t_console_fg"), 2);
    upload_colors(console, [&](int y, int x){ return console.at(y, x).fg; });

    glActiveTexture(GL_TEXTURE0 + 3);
    ch_tex_.bind();
    glUniform1i(program_.get_uniform("t_console_tile"), 3);
    upload_characters(console);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glFlush();
    gl_check();
  }
 private:
  OpenGLTilesetAlias alias_;
  GLProgram program_;
  GLBuffer vertex_buffer_;
  GLTexture ch_tex_;
  GLTexture fg_tex_;
  GLTexture bg_tex_;
  std::array<ptrdiff_t, 2> cached_size_{-1, -1};
};
class GridRenderer {
 public:
  GridRenderer(OpenGLTilesetAlias alias)
  : alias_(alias),
    program_(
#include "console_grid.glslv"
        ,
#include "console_grid.glslf"
    )
  {
    program_.use(); gl_check();
    const std::array<float, 4*4> matrix{
      2, 0, 0, 0,
      0, 2, 0, 0,
      0, 0, 1, 0,
      -1, -1, 0, 1,
    };
    glUniformMatrix4fv(program_.get_uniform("mvp_matrix"),
                       1, GL_FALSE, matrix.data()); gl_check();
  }
  void setup_static(const TCOD_Console& console)
  {
    std::vector<float> vertices;
    std::vector<uint16_t> elements;
    vertices.reserve(console.w * console.h * 4 * 2);
    elements.reserve(console.w * console.h * 6);
    auto add_vertex = [&](float x, float y) {
      vertices.emplace_back(x / console.w);
      vertices.emplace_back(1.0f - y / console.h);
    };
    for (float y = 0; y < console.h; ++y) {
      for (float x = 0; x < console.w; ++x) {
        for (int i : {0, 1, 2, 2, 1, 3}) {
          elements.emplace_back((y * console.w + x) * 4 + i);
        }
        add_vertex(x, y);
        add_vertex(x, y + 1);
        add_vertex(x + 1, y);
        add_vertex(x + 1, y + 1);
      }
    }
    vertex_buffer_ = GLBuffer(GL_ARRAY_BUFFER, vertices, GL_STATIC_DRAW);
    vertex_buffer_.bind();
    const int a_vertex = program_.get_attribute("a_vertex");
    glEnableVertexAttribArray(a_vertex); gl_check();
    glVertexAttribPointer(a_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0); gl_check();
    element_buffer_ = GLBuffer(GL_ELEMENT_ARRAY_BUFFER, elements,
                               GL_STATIC_DRAW);
  }
  void setup_attributes(const TCOD_Console& console)
  {
    attribute_buffer_ = GLBuffer(
        GL_ARRAY_BUFFER,
        std::vector<TileAttribute>(console.w * console.h * 4),
        GL_STREAM_DRAW); gl_check();
    attribute_buffer_.bind();
    const int a_tile = program_.get_attribute("a_tile");
    glEnableVertexAttribArray(a_tile); gl_check();
    glVertexAttribPointer(
        a_tile,
        2,
        GL_UNSIGNED_SHORT,
        GL_FALSE,
        sizeof(TileAttribute),
        reinterpret_cast<void*>(offsetof(TileAttribute, tile))); gl_check();

    const int a_fg = program_.get_attribute("a_fg");
    glEnableVertexAttribArray(a_fg); gl_check();
    glVertexAttribPointer(
        a_fg,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(TileAttribute),
        reinterpret_cast<void*>(offsetof(TileAttribute, fg))); gl_check();

    const int a_bg = program_.get_attribute("a_bg");
    glEnableVertexAttribArray(a_bg); gl_check();
    glVertexAttribPointer(
        a_bg,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(TileAttribute),
        reinterpret_cast<void*>(offsetof(TileAttribute, bg))); gl_check();
  }
  void update_attributes(const TCOD_Console& console)
  {
    // Setup tileset texture.
    glActiveTexture(GL_TEXTURE0);
    // get_alias_texture must be called before get_tile_position.
    glBindTexture(GL_TEXTURE_2D, alias_.get_alias_texture(console));
    glUniform1i(program_.get_uniform("t_tileset"), 0);

    const std::array<float, 3 * 3> tileset_matrix{alias_.get_alias_matrix()};
    glUniformMatrix3fv(program_.get_uniform("tileset_matrix"),
                       1, GL_FALSE, tileset_matrix.data()); gl_check();

    // Upload console attributes.
    std::vector<TileAttribute> attributes;
    attributes.reserve(console.w * console.h * 4);
    for (int y = 0; y < console.h; ++y) {
      for (int x = 0; x < console.w; ++x) {
        int i = y * console.w + x;
        TilePos tile_pos(alias_.get_tile_position(console.tiles[i].ch));
        const TCOD_ColorRGBA& fg = console.tiles[i].fg;
        const TCOD_ColorRGBA& bg = console.tiles[i].bg;
        attributes.emplace_back(TilePos(tile_pos.x, tile_pos.y), fg, bg);
        attributes.emplace_back(TilePos(tile_pos.x, tile_pos.y + 1), fg, bg);
        attributes.emplace_back(TilePos(tile_pos.x + 1, tile_pos.y), fg, bg);
        attributes.emplace_back(TilePos(tile_pos.x + 1, tile_pos.y + 1), fg, bg);
      }
    }
    attribute_buffer_.update(attributes);
  }
  void setup(const TCOD_Console& console)
  {
    if (cached_size_[0] == console.w && cached_size_[0] == console.h) {
      return;
    }
    cached_size_ = {console.w, console.h};
    setup_static(console);
    setup_attributes(console);
  }
  void render(const TCOD_Console& console)
  {
    program_.use();
    setup(console);
    update_attributes(console);

    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    glDisable(GL_SAMPLE_COVERAGE);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_STENCIL_TEST);

    element_buffer_.bind();
    glDrawElements(
        GL_TRIANGLES,
        cached_size_[0] * cached_size_[1] * 6,
        GL_UNSIGNED_SHORT,
        0); gl_check();
    glFlush(); gl_check();
  }
 private:
  struct TilePos {
    TilePos() = default;
    TilePos(uint16_t x_, uint16_t y_)
    : x(x_), y(y_)
    {}
    TilePos(const std::array<int, 2>& pos)
    : TilePos(pos[0], pos[1])
    {}
    uint16_t x, y;
  };
  struct TileAttribute {
    TileAttribute() = default;
    TileAttribute(
        const TilePos& tile_, const TCOD_ColorRGBA& fg_, const TCOD_ColorRGBA& bg_)
    : tile(tile_), fg(fg_), bg(bg_)
    {}
    TilePos tile;
    TCOD_ColorRGBA fg;
    TCOD_ColorRGBA bg;
  };
  OpenGLTilesetAlias alias_;
  GLProgram program_;
  GLBuffer vertex_buffer_;
  GLBuffer element_buffer_;
  GLBuffer attribute_buffer_;
  std::array<int, 2> cached_size_{-1, -1};
};
typedef TwoTranglesRenderer ActiveOpenGL2Renderer;
class OpenGL2Renderer::impl : public ActiveOpenGL2Renderer {
 public:
  explicit impl(OpenGLTilesetAlias alias)
  : ActiveOpenGL2Renderer(alias)
  {}
  auto read_pixels() const -> Image
  {
    int rect[4];
    glGetIntegerv(GL_VIEWPORT, rect); gl_check();
    Image image(rect[2], rect[3]);
    glReadPixels(0, 0, image.width(), image.height(),
                 GL_RGBA, GL_UNSIGNED_BYTE, image.data()); gl_check();
    for (int y = 0; y < image.height() / 2; ++y) {
      for (int x = 0; x < image.width(); ++x) {
        std::swap(image.atf(x, y), image.atf(x, image.height() - 1 - y));
      }
    }
    return image;
  }
};

OpenGL2Renderer::OpenGL2Renderer(OpenGLTilesetAlias alias)
: impl_(std::make_unique<impl>(alias))
{}
OpenGL2Renderer::OpenGL2Renderer(OpenGL2Renderer&&) noexcept = default;
OpenGL2Renderer& OpenGL2Renderer::operator=(OpenGL2Renderer&&) noexcept = default;
OpenGL2Renderer::~OpenGL2Renderer() noexcept = default;
void OpenGL2Renderer::render(const TCOD_Console* console)
{
  impl_->render(tcod::console::ensure_(console));
}
auto OpenGL2Renderer::read_pixels() const -> Image
{
  return impl_->read_pixels();
}
} // namespace sdl2
} // namespace tcod
