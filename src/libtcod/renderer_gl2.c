/* BSD 3-Clause License
 *
 * Copyright © 2008-2021, Jice and the libtcod contributors.
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
#include "renderer_gl2.h"
#ifndef NO_SDL
#include <SDL.h>
#include <stdint.h>
#include <string.h>

#include "../vendor/glad.h"
#include "console.h"
#include "context_viewport.h"
#include "renderer_gl.h"
#include "renderer_gl_internal.h"
static const char* VERTEX_SOURCE =
    "\
uniform mat4 mvp_matrix; // Model-view-projection matrix\n\
attribute vec2 a_vertex; // Simple vertex quad.\n\
uniform vec2 v_console_shape; // The true size of the textures.\n\
uniform vec2 v_console_size; // The relative size of the console textures.\n\
varying vec2 v_coord; // Simple quad coordinate.\n\
void main(void)\n\
{\n\
  gl_Position = mvp_matrix * vec4(a_vertex, 0.0, 1.0);\n\
  v_coord = vec2(a_vertex.x, 1.0 - a_vertex.y); // Flip Y\n\
  // Expand to console coordinates.\n\
  v_coord *= v_console_shape * v_console_size;\n\
}\n\
";
static const char* FRAGMENT_SOURCE =
    "\
uniform vec2 v_tiles_shape; // Tileset columns/rows.\n\
uniform vec2 v_tiles_size; // Tileset texture size.\n\
uniform sampler2D t_tileset;\n\
\n\
uniform vec2 v_console_shape; // The true size of the textures.\n\
uniform sampler2D t_console_tile; // Packed tileset coordinates.\n\
uniform sampler2D t_console_fg;\n\
uniform sampler2D t_console_bg;\n\
\n\
varying vec2 v_coord; // Console coordinates.\n\
\n\
void main(void)\n\
{\n\
  vec2 tile_size = v_tiles_size / v_tiles_shape;\n\
  // The sample coordinate for per-tile console variables.\n\
  vec2 console_pos = floor(v_coord);\n\
  console_pos += vec2(0.5, 0.5); // Offset to the center (for sampling.)\n\
  console_pos /= v_console_shape; // Scale to fit in t_console_X textures.\n\
\n\
  // Coordinates within a tile.\n\
  vec2 tile_interp = fract(v_coord);\n\
\n\
  vec4 tile_encoded = vec4(texture2D(t_console_tile, console_pos));\n\
\n\
  // Unpack tileset index.\n\
  vec2 tile_address = vec2(\n\
      tile_encoded.x * float(0xff) + tile_encoded.y * float(0xff00),\n\
      tile_encoded.z * float(0xff) + tile_encoded.w * float(0xff00)\n\
  );\n\
\n\
  // Clamp the edges of tile_interp to prevent alias bleeding.\n\
  tile_interp = clamp(tile_interp, 0.5 / tile_size, 1.0 - 0.5 / tile_size);\n\
\n\
  // Apply tile_interp and scale.\n\
  tile_address = (tile_address + tile_interp) / v_tiles_shape;\n\
\n\
  vec4 tile_color = texture2D(t_tileset, tile_address);\n\
\n\
  vec4 bg = texture2D(t_console_bg, console_pos);\n\
  vec4 fg = texture2D(t_console_fg, console_pos);\n\
  fg.rgb *= tile_color.rgb;\n\
\n\
  gl_FragColor = mix(bg, fg, tile_color.a);\n\
}\n\
";
/**
 *  Compile the shader program and return error status.
 */
static TCOD_Error gl2_build_shader(uint32_t* program_out) {
  uint32_t program = glCreateProgram();
  uint32_t v_shader = glCreateShader(GL_VERTEX_SHADER);
  uint32_t f_shader = glCreateShader(GL_FRAGMENT_SHADER);
  // Move shader ownership to the program.
  glAttachShader(program, v_shader);
  glDeleteShader(v_shader);
  glAttachShader(program, f_shader);
  glDeleteShader(f_shader);
  // Compile shaders.
  glShaderSource(v_shader, 1, &VERTEX_SOURCE, NULL);
  glShaderSource(f_shader, 1, &FRAGMENT_SOURCE, NULL);
  glCompileShader(v_shader);
  glCompileShader(f_shader);
  // Check and return shader errors.
  int status;
  char log[4096];
  int log_length;
  glGetShaderInfoLog(v_shader, sizeof(log), &log_length, log);
  glGetShaderiv(v_shader, GL_COMPILE_STATUS, &status);
  if (!status) {
    TCOD_set_errorvf("Failed to compile vertex shader:\n%s", log);
    glDeleteProgram(program);
    return TCOD_E_ERROR;
  }
  if (log_length) {
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Vertex shader compile log:\n%s", log);
  }
  glGetShaderInfoLog(f_shader, sizeof(log), &log_length, log);
  glGetShaderiv(f_shader, GL_COMPILE_STATUS, &status);
  if (!status) {
    TCOD_set_errorvf("Failed to compile fragment shader:\n%s", log);
    glDeleteProgram(program);
    return TCOD_E_ERROR;
  }
  if (log_length) {
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Fragment shader compile log:\n%s", log);
  }
  // Link shader program.
  glLinkProgram(program);
  // Check shader program for errors.
  glGetProgramInfoLog(program, sizeof(log), &log_length, log);
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (!status) {
    TCOD_set_errorvf("Failed to link shader program:\n%s", log);
    glDeleteProgram(program);
    return TCOD_E_ERROR;
  }
  if (log_length) {
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Shader program link log:\n%s", log);
  }
  *program_out = program;
  return TCOD_E_OK;
}
/**
 *  Grow texture sizes if they're too small.
 */
TCOD_NODISCARD
static TCOD_Error resize_textures(struct TCOD_RendererGL2* renderer, const TCOD_Console* console) {
  if (console->w > renderer->console_width || console->h > renderer->console_height) {
    if (renderer->console_width == 0) {
      renderer->console_width = 2;
    }
    if (renderer->console_height == 0) {
      renderer->console_height = 2;
    }
    while (renderer->console_width < console->w) {
      renderer->console_width *= 2;
    }
    while (renderer->console_height < console->w) {
      renderer->console_height *= 2;
    }
    for (int i = 0; i < 3; ++i) {
      glBindTexture(GL_TEXTURE_2D, renderer->console_textures[i]);
      glTexImage2D(
          GL_TEXTURE_2D,
          0,
          GL_RGBA,
          renderer->console_width,
          renderer->console_height,
          0,
          GL_RGBA,
          GL_UNSIGNED_BYTE,
          NULL);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    int gl_error = glGetError();
    if (gl_error) {
      return TCOD_set_errorvf(
          "Unexpected OpenGL error %i while allocating %ix%i textures.",
          gl_error,
          renderer->console_width,
          renderer->console_height);
    }
  }
  return TCOD_E_OK;
  ;
}
/**
 *  Get the texture coordinates for a codepoint.
 */
static void get_tex_coord(const struct TCOD_TilesetAtlasOpenGL* __restrict atlas, int ch, uint8_t* __restrict out) {
  const struct TCOD_Tileset* tileset = atlas->tileset;
  int tile_id = 0;
  if (ch < tileset->character_map_length) {
    tile_id = tileset->character_map[ch];
  }
  int x = tile_id % atlas->texture_columns;
  int y = tile_id / atlas->texture_columns;
  out[0] = x & 0xff;
  out[1] = (x >> 8) & 0xff;
  out[2] = y & 0xff;
  out[3] = (y >> 8) & 0xff;
}
/**
    Return a OpenGL texture filter setting based on SDL's
    SDL_HINT_RENDER_SCALE_QUALITY hint.
 */
TCOD_NODISCARD
static int gl_filter_from_sdl_hint() {
  const char* value = SDL_GetHint(SDL_HINT_RENDER_SCALE_QUALITY);
  if (!value) {
    return GL_LINEAR;  // Default option.
  }
  if (strcmp(value, "0") == 0 || strcmp(value, "nearest") == 0) {
    return GL_NEAREST;
  }
  return GL_LINEAR;
}
TCOD_NODISCARD
static TCOD_Error render(
    struct TCOD_RendererGL2* __restrict renderer,
    const TCOD_Console* __restrict console,
    const struct TCOD_ViewportOptions* __restrict viewport) {
  uint8_t* ch_buffer = malloc(sizeof(*ch_buffer) * console->elements * 4);
  TCOD_ColorRGBA* fg_buffer = malloc(sizeof(*fg_buffer) * console->elements);
  TCOD_ColorRGBA* bg_buffer = malloc(sizeof(*bg_buffer) * console->elements);
  if (!ch_buffer || !fg_buffer || !bg_buffer) {
    free(ch_buffer);
    free(fg_buffer);
    free(bg_buffer);
    TCOD_set_errorv("Out of memory.");
    return TCOD_E_OUT_OF_MEMORY;
  }
  for (int i = 0; i < console->elements; ++i) {
    get_tex_coord(renderer->common.atlas, console->tiles[i].ch, ch_buffer + i * 4);
    fg_buffer[i] = console->tiles[i].fg;
    bg_buffer[i] = console->tiles[i].bg;
  }
  glBindTexture(GL_TEXTURE_2D, renderer->console_textures[0]);  // ch
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, console->w, console->h, GL_RGBA, GL_UNSIGNED_BYTE, ch_buffer);
  glBindTexture(GL_TEXTURE_2D, renderer->console_textures[1]);  // fg
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, console->w, console->h, GL_RGBA, GL_UNSIGNED_BYTE, fg_buffer);
  glBindTexture(GL_TEXTURE_2D, renderer->console_textures[2]);  // bg
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, console->w, console->h, GL_RGBA, GL_UNSIGNED_BYTE, bg_buffer);
  free(ch_buffer);
  free(fg_buffer);
  free(bg_buffer);

  glUseProgram(renderer->program);
  int mvp_matrix = glGetUniformLocation(renderer->program, "mvp_matrix");
  int a_vertex = glGetAttribLocation(renderer->program, "a_vertex");
  int v_console_shape = glGetUniformLocation(renderer->program, "v_console_shape");
  int v_console_size = glGetUniformLocation(renderer->program, "v_console_size");
  int v_tiles_shape = glGetUniformLocation(renderer->program, "v_tiles_shape");
  int v_tiles_size = glGetUniformLocation(renderer->program, "v_tiles_size");
  int t_tileset = glGetUniformLocation(renderer->program, "t_tileset");
  int t_console_bg = glGetUniformLocation(renderer->program, "t_console_bg");
  int t_console_fg = glGetUniformLocation(renderer->program, "t_console_fg");
  int t_console_tile = glGetUniformLocation(renderer->program, "t_console_tile");

  float matrix[4 * 4];
  gl_get_viewport_scale(renderer->common.atlas, console, viewport, matrix, &renderer->common);
  glUniformMatrix4fv(mvp_matrix, 1, GL_FALSE, matrix);

  // Upload data of texture shapes.
  const struct TCOD_TilesetAtlasOpenGL* atlas = renderer->common.atlas;

  float tiles_shape[2] = {
      (float)atlas->texture_size / atlas->tileset->tile_width,
      (float)atlas->texture_size / atlas->tileset->tile_height,
  };
  glUniform2fv(v_tiles_shape, 1, tiles_shape);
  float tiles_size[2] = {
      (float)atlas->texture_columns * atlas->tileset->tile_width,
      (float)atlas->texture_rows * atlas->tileset->tile_height,
  };
  glUniform2fv(v_tiles_size, 1, tiles_size);

  float console_shape[2] = {(float)renderer->console_width, (float)renderer->console_height};
  glUniform2fv(v_console_shape, 1, console_shape);
  float console_size[2] = {
      (float)console->w / console_shape[0],
      (float)console->h / console_shape[1],
  };
  glUniform2fv(v_console_size, 1, console_size);

  // Bind textures to program.
  glActiveTexture(GL_TEXTURE0 + 3);  // Tileset atlas.
  glBindTexture(GL_TEXTURE_2D, atlas->texture);
  // Texels are clamped by the shader, GL_LINEAR can be used without bleeding.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_from_sdl_hint());
  glUniform1i(t_tileset, 3);
  glActiveTexture(GL_TEXTURE0 + 0);  // Tile position.
  glBindTexture(GL_TEXTURE_2D, renderer->console_textures[0]);
  glUniform1i(t_console_tile, 0);
  glActiveTexture(GL_TEXTURE0 + 1);  // Foreground color.
  glBindTexture(GL_TEXTURE_2D, renderer->console_textures[1]);
  glUniform1i(t_console_fg, 1);
  glActiveTexture(GL_TEXTURE0 + 2);  // Background color.
  glBindTexture(GL_TEXTURE_2D, renderer->console_textures[2]);
  glUniform1i(t_console_bg, 2);

  // Bind vertex array.
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vertex_buffer);
  glEnableVertexAttribArray(a_vertex);
  glVertexAttribPointer(a_vertex, 2, GL_BYTE, GL_FALSE, 0, 0);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glActiveTexture(GL_TEXTURE0 + 3);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0 + 2);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0 + 1);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
  int gl_error;
  if ((gl_error = glGetError()) != 0) {
    return TCOD_set_errorvf("Unexpected OpenGL error %i.", gl_error);
  }
  return TCOD_E_OK;
}
/**
 *  Render the console onto the screen.
 */
static TCOD_Error gl2_accumulate(
    struct TCOD_Context* __restrict context,
    const TCOD_Console* __restrict console,
    const struct TCOD_ViewportOptions* __restrict viewport) {
  struct TCOD_RendererGL2* renderer = context->contextdata_;
  int gl_error = glGetError();
  if (gl_error) {
    return TCOD_set_errorvf("Unhandled OpenGL error %i.", gl_error);
  }
  TCOD_Error err;
  if ((err = resize_textures(renderer, console)) < 0) {
    return err;
  }
  if ((err = render(renderer, console, viewport)) < 0) {
    return err;
  }

  glFlush();
  if ((gl_error = glGetError()) != 0) {
    return TCOD_set_errorvf("Unexpected OpenGL error %i.", gl_error);
  }
  return TCOD_E_OK;
}
/**
 *  Clear, render, and swap the screen.
 */
static TCOD_Error gl2_present(
    struct TCOD_Context* __restrict context,
    const TCOD_Console* __restrict console,
    const struct TCOD_ViewportOptions* __restrict viewport) {
  if (!viewport) {
    viewport = &TCOD_VIEWPORT_DEFAULT_;
  }
  struct TCOD_RendererGL2* renderer = context->contextdata_;
  int window_width;
  int window_height;
  SDL_GL_GetDrawableSize(renderer->common.window, &window_width, &window_height);
  glViewport(0, 0, window_width, window_height);
  glClearColor(
      (float)viewport->clear_color.r / 255.0f,
      (float)viewport->clear_color.g / 255.0f,
      (float)viewport->clear_color.b / 255.0f,
      (float)viewport->clear_color.a / 255.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  TCOD_Error err = gl2_accumulate(context, console, viewport);
  SDL_GL_SwapWindow(renderer->common.window);
  return err;
}
void gl2_destructor(struct TCOD_Context* __restrict context) {
  struct TCOD_RendererGL2* renderer = context->contextdata_;
  if (!renderer) {
    return;
  }
  glDeleteBuffers(1, &renderer->vertex_buffer);
  glDeleteTextures(3, renderer->console_textures);
  glDeleteProgram(renderer->program);
  TCOD_renderer_gl_common_uninit(&renderer->common);
  free(renderer);
}
TCODLIB_API TCOD_NODISCARD struct TCOD_Context* TCOD_renderer_new_gl2(
    int x,
    int y,
    int pixel_width,
    int pixel_height,
    const char* title,
    int window_flags,
    int vsync,
    struct TCOD_Tileset* tileset) {
  struct TCOD_Context* context = TCOD_context_new_();
  if (!context) {
    return NULL;
  }
  context->type = TCOD_RENDERER_OPENGL2;
  struct TCOD_RendererGL2* renderer = calloc(sizeof(*renderer), 1);
  if (!renderer) {
    TCOD_context_delete(context);
    return NULL;
  }
  context->c_destructor_ = gl2_destructor;
  context->contextdata_ = renderer;
  TCOD_Error err = TCOD_renderer_gl_common_init(
      x, y, pixel_width, pixel_height, title, window_flags, vsync, tileset, 2, 0, SDL_GL_CONTEXT_PROFILE_CORE, context);
  if (err < 0) {
    TCOD_context_delete(context);
    return NULL;
  }
  if (gl2_build_shader(&renderer->program) < 0) {
    TCOD_context_delete(context);
    return NULL;
  }

  glGenTextures(3, renderer->console_textures);
  for (int i = 0; i < 3; ++i) {
    glBindTexture(GL_TEXTURE_2D, renderer->console_textures[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenBuffers(1, &renderer->vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vertex_buffer);
  const uint8_t VERTEX_BUFFER_DATA[8] = {0, 0, 0, 1, 1, 0, 1, 1};
  glBufferData(GL_ARRAY_BUFFER, 8, VERTEX_BUFFER_DATA, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  context->c_accumulate_ = gl2_accumulate;
  context->c_present_ = gl2_present;
  return context;
}
#endif  // NO_SDL
