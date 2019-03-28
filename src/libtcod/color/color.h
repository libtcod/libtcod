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
#ifndef LIBTCOD_COLOR_COLOR_H_
#define LIBTCOD_COLOR_COLOR_H_
#include <stdint.h>
/**
 *  A three channel color struct.
 */
struct TCOD_ColorRGB {
#ifdef __cplusplus
  bool operator==(const TCOD_ColorRGB& rhs) const noexcept
  {
    return r == rhs.r && g == rhs.g && b == rhs.b;
  }
  bool operator!=(const TCOD_ColorRGB& rhs) const noexcept
  {
    return !(*this == rhs);
  }
#endif // __cplusplus
  uint8_t r;
  uint8_t g;
  uint8_t b;
};
typedef struct TCOD_ColorRGB TCOD_color_t;
/**
 *  A four channel color struct.
 */
struct TCOD_ColorRGBA {
#ifdef __cplusplus
  bool operator==(const TCOD_ColorRGBA& rhs) const noexcept
  {
    return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
  }
  bool operator!=(const TCOD_ColorRGBA& rhs) const noexcept
  {
    return !(*this == rhs);
  }
#endif // __cplusplus
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};
#ifdef __cplusplus
namespace tcod {
  struct ColorRGB: TCOD_ColorRGB {
    ColorRGB() = default;
    ColorRGB(uint8_t red, uint8_t green, uint8_t blue)
    : TCOD_ColorRGB{red, green, blue}
    {}
    ColorRGB(const struct TCOD_ColorRGB& rhs)
    : TCOD_ColorRGB{rhs}
    {}
    explicit ColorRGB(const struct TCOD_ColorRGBA& rhs)
    : TCOD_ColorRGB{rhs.r, rhs.g, rhs.b}
    {}
  };
  struct ColorRGBA: TCOD_ColorRGBA {
    ColorRGBA() = default;
    ColorRGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha=0xff)
    : TCOD_ColorRGBA{red, green, blue, alpha}
    {}
    explicit ColorRGBA(const struct TCOD_ColorRGB& color,
                            uint8_t alpha=0xff)
    : TCOD_ColorRGBA{color.r, color.g, color.b, alpha}
    {}
    ColorRGBA(const struct TCOD_ColorRGBA& rhs)
    : TCOD_ColorRGBA{rhs}
    {}
  };
} // namespace tcod
#endif /* __cplusplus */
#endif /* LIBTCOD_COLOR_COLOR_H_ */
