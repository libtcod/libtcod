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
#ifndef LIBTCOD_COLOR_COLOR_H_
#define LIBTCOD_COLOR_COLOR_H_
#include <stdint.h>

#include "../color.h"
#ifdef __cplusplus
namespace tcod {
struct ColorRGBA {
  ColorRGBA() = default;
  ColorRGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha=0xff)
  : r(red), g(green), b(blue), a(alpha)
  {}
  explicit ColorRGBA(const TCOD_color_t& color)
  : ColorRGBA(color.r, color.g, color.b)
  {}
  bool operator==(const ColorRGBA& rhs) const noexcept
  {
    return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
  }
  bool operator!=(const ColorRGBA& rhs) const noexcept
  {
    return !(*this == rhs);
  }
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};
} // namespace tcod
#endif /* __cplusplus */
#endif /* LIBTCOD_COLOR_COLOR_H_ */
