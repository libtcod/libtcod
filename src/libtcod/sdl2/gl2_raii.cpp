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
#include "gl2_raii.h"

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "gl2_ext_.h"

namespace tcod {
namespace sdl2 {
#ifdef GL_ES_VERSION_2_0 // OpenGL ES 2.0
static const char* GLSL_VERSION_DIRECTIVE = "#version 100\n";
#else // OpenGL 2.0
static const char* GLSL_VERSION_DIRECTIVE = "#version 110\n";
#endif
GLShader::GLShader(int shader_type, const std::string& source)
: shader_{glCreateShader(shader_type)}
{
  gl_check();
  if (!shader_) {
    throw std::runtime_error("Error creating OpenGL shader.");
  }
  std::array<const char*, 2> sources{
      GLSL_VERSION_DIRECTIVE,
      source.c_str(),
  };
  glShaderSource(
      shader_,
      static_cast<GLsizei>(sources.size()),
      sources.data(),
      nullptr);
  gl_check();
  glCompileShader(shader_); gl_check();
#ifndef NDEBUG
  verify_shader();
#endif
}
GLShader::~GLShader() noexcept
{
  if (shader_) { glDeleteShader(shader_); gl_check(); }
}
std::string GLShader::get_log() const
{
  std::vector<char> info_log(get_variable(GL_INFO_LOG_LENGTH));
  if (info_log.size() == 0) { return ""; }
  glGetShaderInfoLog(
      shader_,
      static_cast<GLsizei>(info_log.size()),
      nullptr,
      info_log.data());
  gl_check();
  return std::string(info_log.data());
}
int GLShader::get_variable(int name) const
{
  int variable;
  glGetShaderiv(shader_, name, &variable); gl_check();
  return variable;
}
void GLShader::verify_shader()
{
  if (get_variable(GL_COMPILE_STATUS)) { return; }
  throw std::runtime_error(std::string("Shader failed to compile:\n")
                           + get_log());
}
GLProgram::GLProgram()
: program_{glCreateProgram()}
{
  gl_check();
  if (!program_) {
    throw std::runtime_error("Error creating OpenGL program.");
  }
}
GLProgram::GLProgram(const std::string& vshader, const std::string& fshader)
: GLProgram()
{
  attach(GLShader(GL_VERTEX_SHADER, vshader));
  attach(GLShader(GL_FRAGMENT_SHADER, fshader));
  link();
}
GLProgram::~GLProgram() noexcept
{
  if (program_) { glDeleteProgram(program_); gl_check(); }
}
void GLProgram::attach(const GLShader& shader)
{
  glAttachShader(program_, shader.get()); gl_check();
}
std::string GLProgram::get_log() const
{
  std::vector<char> info_log(get_variable(GL_INFO_LOG_LENGTH));
  if (info_log.size() == 0) { return ""; }
  glGetProgramInfoLog(
      program_,
      static_cast<GLsizei>(info_log.size()),
      nullptr,
      info_log.data());
  gl_check();
  return std::string(info_log.data());
}
void GLProgram::link()
{
  glLinkProgram(program_); gl_check();
  if (!get_variable(GL_LINK_STATUS)) {
    throw std::runtime_error(
        std::string("Shader program failed to link:\n") + get_log());
  }
}
void GLProgram::validate()
{
  glValidateProgram(program_); gl_check();
  if (!get_variable(GL_VALIDATE_STATUS)) {
    throw std::runtime_error(
        std::string("Shader program failed to validate:\n") + get_log());
  }
}
void GLProgram::use() const
{
  glUseProgram(program_); gl_check();
}
int GLProgram::get_attribute(const std::string& name) const
{
  int attribute = glGetAttribLocation(program_, name.c_str());
  gl_check();
  return attribute;
}
int GLProgram::get_variable(int name) const
{
  int variable;
  glGetProgramiv(program_, name, &variable); gl_check();
  return variable;
}
int GLProgram::get_uniform(const std::string& name) const
{
  int id = glGetUniformLocation(program_, name.c_str());
  gl_check();
  if (id == -1) {
    throw std::runtime_error("Could not find uniform: " + name);
  }
  return id;
}

GLBuffer::GLBuffer()
: buffer_{0}
{
  glGenBuffers(1, &buffer_); gl_check();
  if (!buffer_) {
    throw std::runtime_error("Error creating OpenGL program.");
  }
}
GLBuffer::~GLBuffer() noexcept
{
  if (buffer_) { glDeleteBuffers(1, &buffer_); gl_check(); }
}
void GLBuffer::allocate(int target, size_t size, const void* data, int usage)
{
  target_ = target;
  bind();
  glBufferData(target_, size, data, usage); gl_check();
}
void GLBuffer::update(size_t size, const void* data)
{
  bind();
  glBufferSubData(target_, 0, size, data); gl_check();
}
void GLBuffer::bind()
{
  glBindBuffer(target_, buffer_); gl_check();
}
GLTexture::GLTexture()
{
  glGenTextures(1, &texture_);
}
GLTexture::~GLTexture() noexcept
{
  if (texture_) { glDeleteTextures(1, &texture_); }
}
void GLTexture::bind()
{
  glBindTexture(GL_TEXTURE_2D, texture_); gl_check();
}
// Framebuffers
/*
GLFramebuffer::GLFramebuffer()
{
  glGenFramebuffers(1, &framebuffer_);
}
GLFramebuffer::~GLFramebuffer() noexcept
{
  if (framebuffer_) { glDeleteFramebuffers(1, &framebuffer_); }
}
void GLFramebuffer::bind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_); gl_check();
}
*/
} // namespace sdl2
} // namespace tcod
