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
#ifndef _TCOD_COLOR_H
#define _TCOD_COLOR_H

#include <stdbool.h>
#include <stdint.h>
#ifdef __cplusplus
#include <istream>
#include <ostream>
#include <stdexcept>
#endif  // __cplusplus

#include "config.h"
/**
    A 3-channel RGB color struct.
 */
struct TCOD_ColorRGB {
#ifdef __cplusplus
  constexpr bool operator==(const TCOD_ColorRGB& rhs) const noexcept { return r == rhs.r && g == rhs.g && b == rhs.b; }
  constexpr bool operator!=(const TCOD_ColorRGB& rhs) const noexcept { return !(*this == rhs); }
  friend std::ostream& operator<<(std::ostream& out, const TCOD_ColorRGB& rgb) {
    return out << '{' << static_cast<int>(rgb.r) << ", " << static_cast<int>(rgb.g) << ", " << static_cast<int>(rgb.b)
               << '}';
  }
  friend std::istream& operator>>(std::istream& in, TCOD_ColorRGB& rgb) {
    in >> std::ws;
    char ch;
    in >> ch;
    if (ch != '{') std::runtime_error("Expected missing '{'.");
    int channel;
    in >> channel;
    rgb.r = static_cast<uint8_t>(channel);
    in >> ch;
    if (ch != ',') std::runtime_error("Expected missing ',' delimiter.");
    in >> channel;
    rgb.g = static_cast<uint8_t>(channel);
    in >> ch;
    if (ch != ',') std::runtime_error("Expected missing ',' delimiter.");
    in >> channel;
    rgb.b = static_cast<uint8_t>(channel);
    in >> ch;
    if (ch != '}') std::runtime_error("Expected missing '}'.");
    return in;
  }
#endif  // __cplusplus
  uint8_t r;
  uint8_t g;
  uint8_t b;
};
typedef struct TCOD_ColorRGB TCOD_color_t;
typedef struct TCOD_ColorRGB TCOD_ColorRGB;
/**
    A 4-channel RGBA color struct.
 */
struct TCOD_ColorRGBA {
#ifdef __cplusplus
  constexpr bool operator==(const TCOD_ColorRGBA& rhs) const noexcept {
    return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
  }
  constexpr bool operator!=(const TCOD_ColorRGBA& rhs) const noexcept { return !(*this == rhs); }
  constexpr TCOD_ColorRGBA& operator=(const TCOD_ColorRGB& rhs) {
    return (*this) = TCOD_ColorRGBA{rhs.r, rhs.g, rhs.b, 255};
  }
  friend std::ostream& operator<<(std::ostream& out, const TCOD_ColorRGBA& rgba) {
    return out << '{' << static_cast<int>(rgba.r) << ", " << static_cast<int>(rgba.g) << ", "
               << static_cast<int>(rgba.b) << ", " << static_cast<int>(rgba.a) << '}';
  }
  friend std::istream& operator>>(std::istream& in, TCOD_ColorRGBA& rgba) {
    in >> std::ws;
    char ch;
    in >> ch;
    if (ch != '{') std::runtime_error("Expected missing '{'.");
    int channel;
    in >> channel;
    rgba.r = static_cast<uint8_t>(channel);
    in >> ch;
    if (ch != ',') std::runtime_error("Expected missing ',' delimiter.");
    in >> channel;
    rgba.g = static_cast<uint8_t>(channel);
    in >> ch;
    if (ch != ',') std::runtime_error("Expected missing ',' delimiter.");
    in >> channel;
    rgba.b = static_cast<uint8_t>(channel);
    in >> ch;
    if (ch == '}') {
      rgba.a = 255;
      return in;
    }
    if (ch != ',') std::runtime_error("Expected missing ',' delimiter.");
    in >> channel;
    rgba.a = static_cast<uint8_t>(channel);
    in >> ch;
    if (ch != '}') std::runtime_error("Expected missing '}'.");
    return in;
  }
#endif  // __cplusplus
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};
typedef struct TCOD_ColorRGBA TCOD_ColorRGBA;
#ifdef __cplusplus
extern "C" {
#endif
/* constructors */
TCODLIB_API TCOD_color_t TCOD_color_RGB(uint8_t r, uint8_t g, uint8_t b);
TCODLIB_API TCOD_color_t TCOD_color_HSV(float hue, float saturation, float value);
/* basic operations */
TCODLIB_API bool TCOD_color_equals(TCOD_color_t c1, TCOD_color_t c2);
TCODLIB_API TCOD_color_t TCOD_color_add(TCOD_color_t c1, TCOD_color_t c2);
TCODLIB_API TCOD_color_t TCOD_color_subtract(TCOD_color_t c1, TCOD_color_t c2);
TCODLIB_API TCOD_color_t TCOD_color_multiply(TCOD_color_t c1, TCOD_color_t c2);
TCODLIB_API TCOD_color_t TCOD_color_multiply_scalar(TCOD_color_t c1, float value);
TCODLIB_API TCOD_color_t TCOD_color_lerp(TCOD_color_t c1, TCOD_color_t c2, float coef);
/**
 *  Blend `src` into `dst` as an alpha blending operation.
 *  \rst
 *  .. versionadded:: 1.16
 *  \endrst
 */
void TCOD_color_alpha_blend(TCOD_ColorRGBA* dst, const TCOD_ColorRGBA* src);

/* HSV transformations */
TCODLIB_API void TCOD_color_set_HSV(TCOD_color_t* color, float hue, float saturation, float value);
TCODLIB_API void TCOD_color_get_HSV(TCOD_color_t color, float* hue, float* saturation, float* value);
TCODLIB_API float TCOD_color_get_hue(TCOD_color_t color);
TCODLIB_API void TCOD_color_set_hue(TCOD_color_t* color, float hue);
TCODLIB_API float TCOD_color_get_saturation(TCOD_color_t color);
TCODLIB_API void TCOD_color_set_saturation(TCOD_color_t* color, float saturation);
TCODLIB_API float TCOD_color_get_value(TCOD_color_t color);
TCODLIB_API void TCOD_color_set_value(TCOD_color_t* color, float value);
TCODLIB_API void TCOD_color_shift_hue(TCOD_color_t* color, float shift);
TCODLIB_API void TCOD_color_scale_HSV(TCOD_color_t* color, float saturation_coef, float value_coef);
/* color map */
TCODLIB_API void TCOD_color_gen_map(TCOD_color_t* map, int nb_key, const TCOD_color_t* key_color, const int* key_index);

/* color names */
enum {
  TCOD_COLOR_RED,
  TCOD_COLOR_FLAME,
  TCOD_COLOR_ORANGE,
  TCOD_COLOR_AMBER,
  TCOD_COLOR_YELLOW,
  TCOD_COLOR_LIME,
  TCOD_COLOR_CHARTREUSE,
  TCOD_COLOR_GREEN,
  TCOD_COLOR_SEA,
  TCOD_COLOR_TURQUOISE,
  TCOD_COLOR_CYAN,
  TCOD_COLOR_SKY,
  TCOD_COLOR_AZURE,
  TCOD_COLOR_BLUE,
  TCOD_COLOR_HAN,
  TCOD_COLOR_VIOLET,
  TCOD_COLOR_PURPLE,
  TCOD_COLOR_FUCHSIA,
  TCOD_COLOR_MAGENTA,
  TCOD_COLOR_PINK,
  TCOD_COLOR_CRIMSON,
  TCOD_COLOR_NB
};

/* color levels */
enum {
  TCOD_COLOR_DESATURATED,
  TCOD_COLOR_LIGHTEST,
  TCOD_COLOR_LIGHTER,
  TCOD_COLOR_LIGHT,
  TCOD_COLOR_NORMAL,
  TCOD_COLOR_DARK,
  TCOD_COLOR_DARKER,
  TCOD_COLOR_DARKEST,
  TCOD_COLOR_LEVELS
};

/* color array */
extern TCODLIB_API const TCOD_color_t TCOD_colors[TCOD_COLOR_NB][TCOD_COLOR_LEVELS];

/* grey levels */
extern TCODLIB_API const TCOD_color_t TCOD_black;  // 0, 0, 0
extern TCODLIB_API const TCOD_color_t TCOD_darkest_grey;  // 31, 31, 31
extern TCODLIB_API const TCOD_color_t TCOD_darker_grey;  // 63, 63, 63
extern TCODLIB_API const TCOD_color_t TCOD_dark_grey;  // 95, 95, 95
extern TCODLIB_API const TCOD_color_t TCOD_grey;  // 127, 127, 127
extern TCODLIB_API const TCOD_color_t TCOD_light_grey;  // 159, 159, 159
extern TCODLIB_API const TCOD_color_t TCOD_lighter_grey;  // 191, 191, 191
extern TCODLIB_API const TCOD_color_t TCOD_lightest_grey;  // 223, 223, 223
extern TCODLIB_API const TCOD_color_t TCOD_darkest_gray;  // 31, 31, 31
extern TCODLIB_API const TCOD_color_t TCOD_darker_gray;  // 63, 63, 63
extern TCODLIB_API const TCOD_color_t TCOD_dark_gray;  // 95, 95, 95
extern TCODLIB_API const TCOD_color_t TCOD_gray;  // 127, 127, 127
extern TCODLIB_API const TCOD_color_t TCOD_light_gray;  // 159, 159, 159
extern TCODLIB_API const TCOD_color_t TCOD_lighter_gray;  // 191, 191, 191
extern TCODLIB_API const TCOD_color_t TCOD_lightest_gray;  // 223, 223, 223
extern TCODLIB_API const TCOD_color_t TCOD_white;  // 255, 255, 255

/* sepia */
extern TCODLIB_API const TCOD_color_t TCOD_darkest_sepia;  // 31, 24, 15
extern TCODLIB_API const TCOD_color_t TCOD_darker_sepia;  // 63, 50, 31
extern TCODLIB_API const TCOD_color_t TCOD_dark_sepia;  // 94, 75, 47
extern TCODLIB_API const TCOD_color_t TCOD_sepia;  // 127, 101, 63
extern TCODLIB_API const TCOD_color_t TCOD_light_sepia;  // 158, 134, 100
extern TCODLIB_API const TCOD_color_t TCOD_lighter_sepia;  // 191, 171, 143
extern TCODLIB_API const TCOD_color_t TCOD_lightest_sepia;  // 222, 211, 195

/* standard colors */
extern TCODLIB_API const TCOD_color_t TCOD_red;  // 255, 0, 0
extern TCODLIB_API const TCOD_color_t TCOD_flame;  // 255, 63, 0
extern TCODLIB_API const TCOD_color_t TCOD_orange;  // 255, 127, 0
extern TCODLIB_API const TCOD_color_t TCOD_amber;  // 255, 191, 0
extern TCODLIB_API const TCOD_color_t TCOD_yellow;  // 255, 255, 0
extern TCODLIB_API const TCOD_color_t TCOD_lime;  // 191, 255, 0
extern TCODLIB_API const TCOD_color_t TCOD_chartreuse;  // 127, 255, 0
extern TCODLIB_API const TCOD_color_t TCOD_green;  // 0, 255, 0
extern TCODLIB_API const TCOD_color_t TCOD_sea;  // 0, 255, 127
extern TCODLIB_API const TCOD_color_t TCOD_turquoise;  // 0, 255, 191
extern TCODLIB_API const TCOD_color_t TCOD_cyan;  // 0, 255, 255
extern TCODLIB_API const TCOD_color_t TCOD_sky;  // 0, 191, 255
extern TCODLIB_API const TCOD_color_t TCOD_azure;  // 0, 127, 255
extern TCODLIB_API const TCOD_color_t TCOD_blue;  // 0, 0, 255
extern TCODLIB_API const TCOD_color_t TCOD_han;  // 63, 0, 255
extern TCODLIB_API const TCOD_color_t TCOD_violet;  // 127, 0, 255
extern TCODLIB_API const TCOD_color_t TCOD_purple;  // 191, 0, 255
extern TCODLIB_API const TCOD_color_t TCOD_fuchsia;  // 255, 0, 255
extern TCODLIB_API const TCOD_color_t TCOD_magenta;  // 255, 0, 191
extern TCODLIB_API const TCOD_color_t TCOD_pink;  // 255, 0, 127
extern TCODLIB_API const TCOD_color_t TCOD_crimson;  // 255, 0, 63

/* dark colors */
extern TCODLIB_API const TCOD_color_t TCOD_dark_red;  // 191, 0, 0
extern TCODLIB_API const TCOD_color_t TCOD_dark_flame;  // 191, 47, 0
extern TCODLIB_API const TCOD_color_t TCOD_dark_orange;  // 191, 95, 0
extern TCODLIB_API const TCOD_color_t TCOD_dark_amber;  // 191, 143, 0
extern TCODLIB_API const TCOD_color_t TCOD_dark_yellow;  // 191, 191, 0
extern TCODLIB_API const TCOD_color_t TCOD_dark_lime;  // 143, 191, 0
extern TCODLIB_API const TCOD_color_t TCOD_dark_chartreuse;  // 95, 191, 0
extern TCODLIB_API const TCOD_color_t TCOD_dark_green;  // 0, 191, 0
extern TCODLIB_API const TCOD_color_t TCOD_dark_sea;  // 0, 191, 95
extern TCODLIB_API const TCOD_color_t TCOD_dark_turquoise;  // 0, 191, 143
extern TCODLIB_API const TCOD_color_t TCOD_dark_cyan;  // 0, 191, 191
extern TCODLIB_API const TCOD_color_t TCOD_dark_sky;  // 0, 143, 191
extern TCODLIB_API const TCOD_color_t TCOD_dark_azure;  // 0, 95, 191
extern TCODLIB_API const TCOD_color_t TCOD_dark_blue;  // 0, 0, 191
extern TCODLIB_API const TCOD_color_t TCOD_dark_han;  // 47, 0, 191
extern TCODLIB_API const TCOD_color_t TCOD_dark_violet;  // 95, 0, 191
extern TCODLIB_API const TCOD_color_t TCOD_dark_purple;  // 143, 0, 191
extern TCODLIB_API const TCOD_color_t TCOD_dark_fuchsia;  // 191, 0, 191
extern TCODLIB_API const TCOD_color_t TCOD_dark_magenta;  // 191, 0, 143
extern TCODLIB_API const TCOD_color_t TCOD_dark_pink;  // 191, 0, 95
extern TCODLIB_API const TCOD_color_t TCOD_dark_crimson;  // 191, 0, 47

/* darker colors */
extern TCODLIB_API const TCOD_color_t TCOD_darker_red;  // 127, 0, 0
extern TCODLIB_API const TCOD_color_t TCOD_darker_flame;  // 127, 31, 0
extern TCODLIB_API const TCOD_color_t TCOD_darker_orange;  // 127, 63, 0
extern TCODLIB_API const TCOD_color_t TCOD_darker_amber;  // 127, 95, 0
extern TCODLIB_API const TCOD_color_t TCOD_darker_yellow;  // 127, 127, 0
extern TCODLIB_API const TCOD_color_t TCOD_darker_lime;  // 95, 127, 0
extern TCODLIB_API const TCOD_color_t TCOD_darker_chartreuse;  // 63, 127, 0
extern TCODLIB_API const TCOD_color_t TCOD_darker_green;  // 0, 127, 0
extern TCODLIB_API const TCOD_color_t TCOD_darker_sea;  // 0, 127, 63
extern TCODLIB_API const TCOD_color_t TCOD_darker_turquoise;  // 0, 127, 95
extern TCODLIB_API const TCOD_color_t TCOD_darker_cyan;  // 0, 127, 127
extern TCODLIB_API const TCOD_color_t TCOD_darker_sky;  // 0, 95, 127
extern TCODLIB_API const TCOD_color_t TCOD_darker_azure;  // 0, 63, 127
extern TCODLIB_API const TCOD_color_t TCOD_darker_blue;  // 0, 0, 127
extern TCODLIB_API const TCOD_color_t TCOD_darker_han;  // 31, 0, 127
extern TCODLIB_API const TCOD_color_t TCOD_darker_violet;  // 63, 0, 127
extern TCODLIB_API const TCOD_color_t TCOD_darker_purple;  // 95, 0, 127
extern TCODLIB_API const TCOD_color_t TCOD_darker_fuchsia;  // 127, 0, 127
extern TCODLIB_API const TCOD_color_t TCOD_darker_magenta;  // 127, 0, 95
extern TCODLIB_API const TCOD_color_t TCOD_darker_pink;  // 127, 0, 63
extern TCODLIB_API const TCOD_color_t TCOD_darker_crimson;  // 127, 0, 31

/* darkest colors */
extern TCODLIB_API const TCOD_color_t TCOD_darkest_red;  // 63, 0, 0
extern TCODLIB_API const TCOD_color_t TCOD_darkest_flame;  // 63, 15, 0
extern TCODLIB_API const TCOD_color_t TCOD_darkest_orange;  // 63, 31, 0
extern TCODLIB_API const TCOD_color_t TCOD_darkest_amber;  // 63, 47, 0
extern TCODLIB_API const TCOD_color_t TCOD_darkest_yellow;  // 63, 63, 0
extern TCODLIB_API const TCOD_color_t TCOD_darkest_lime;  // 47, 63, 0
extern TCODLIB_API const TCOD_color_t TCOD_darkest_chartreuse;  // 31, 63, 0
extern TCODLIB_API const TCOD_color_t TCOD_darkest_green;  // 0, 63, 0
extern TCODLIB_API const TCOD_color_t TCOD_darkest_sea;  // 0, 63, 31
extern TCODLIB_API const TCOD_color_t TCOD_darkest_turquoise;  // 0, 63, 47
extern TCODLIB_API const TCOD_color_t TCOD_darkest_cyan;  // 0, 63, 63
extern TCODLIB_API const TCOD_color_t TCOD_darkest_sky;  // 0, 47, 63
extern TCODLIB_API const TCOD_color_t TCOD_darkest_azure;  // 0, 31, 63
extern TCODLIB_API const TCOD_color_t TCOD_darkest_blue;  // 0, 0, 63
extern TCODLIB_API const TCOD_color_t TCOD_darkest_han;  // 15, 0, 63
extern TCODLIB_API const TCOD_color_t TCOD_darkest_violet;  // 31, 0, 63
extern TCODLIB_API const TCOD_color_t TCOD_darkest_purple;  // 47, 0, 63
extern TCODLIB_API const TCOD_color_t TCOD_darkest_fuchsia;  // 63, 0, 63
extern TCODLIB_API const TCOD_color_t TCOD_darkest_magenta;  // 63, 0, 47
extern TCODLIB_API const TCOD_color_t TCOD_darkest_pink;  // 63, 0, 31
extern TCODLIB_API const TCOD_color_t TCOD_darkest_crimson;  // 63, 0, 15

/* light colors */
extern TCODLIB_API const TCOD_color_t TCOD_light_red;  // 255, 63, 63
extern TCODLIB_API const TCOD_color_t TCOD_light_flame;  // 255, 111, 63
extern TCODLIB_API const TCOD_color_t TCOD_light_orange;  // 255, 159, 63
extern TCODLIB_API const TCOD_color_t TCOD_light_amber;  // 255, 207, 63
extern TCODLIB_API const TCOD_color_t TCOD_light_yellow;  // 255, 255, 63
extern TCODLIB_API const TCOD_color_t TCOD_light_lime;  // 207, 255, 63
extern TCODLIB_API const TCOD_color_t TCOD_light_chartreuse;  // 159, 255, 63
extern TCODLIB_API const TCOD_color_t TCOD_light_green;  // 63, 255, 63
extern TCODLIB_API const TCOD_color_t TCOD_light_sea;  // 63, 255, 159
extern TCODLIB_API const TCOD_color_t TCOD_light_turquoise;  // 63, 255, 207
extern TCODLIB_API const TCOD_color_t TCOD_light_cyan;  // 63, 255, 255
extern TCODLIB_API const TCOD_color_t TCOD_light_sky;  // 63, 207, 255
extern TCODLIB_API const TCOD_color_t TCOD_light_azure;  // 63, 159, 255
extern TCODLIB_API const TCOD_color_t TCOD_light_blue;  // 63, 63, 255
extern TCODLIB_API const TCOD_color_t TCOD_light_han;  // 111, 63, 255
extern TCODLIB_API const TCOD_color_t TCOD_light_violet;  // 159, 63, 255
extern TCODLIB_API const TCOD_color_t TCOD_light_purple;  // 207, 63, 255
extern TCODLIB_API const TCOD_color_t TCOD_light_fuchsia;  // 255, 63, 255
extern TCODLIB_API const TCOD_color_t TCOD_light_magenta;  // 255, 63, 207
extern TCODLIB_API const TCOD_color_t TCOD_light_pink;  // 255, 63, 159
extern TCODLIB_API const TCOD_color_t TCOD_light_crimson;  // 255, 63, 111

/* lighter colors */
extern TCODLIB_API const TCOD_color_t TCOD_lighter_red;  // 255, 127, 127
extern TCODLIB_API const TCOD_color_t TCOD_lighter_flame;  // 255, 159, 127
extern TCODLIB_API const TCOD_color_t TCOD_lighter_orange;  // 255, 191, 127
extern TCODLIB_API const TCOD_color_t TCOD_lighter_amber;  // 255, 223, 127
extern TCODLIB_API const TCOD_color_t TCOD_lighter_yellow;  // 255, 255, 127
extern TCODLIB_API const TCOD_color_t TCOD_lighter_lime;  // 223, 255, 127
extern TCODLIB_API const TCOD_color_t TCOD_lighter_chartreuse;  // 191, 255, 127
extern TCODLIB_API const TCOD_color_t TCOD_lighter_green;  // 127, 255, 127
extern TCODLIB_API const TCOD_color_t TCOD_lighter_sea;  // 127, 255, 191
extern TCODLIB_API const TCOD_color_t TCOD_lighter_turquoise;  // 127, 255, 223
extern TCODLIB_API const TCOD_color_t TCOD_lighter_cyan;  // 127, 255, 255
extern TCODLIB_API const TCOD_color_t TCOD_lighter_sky;  // 127, 223, 255
extern TCODLIB_API const TCOD_color_t TCOD_lighter_azure;  // 127, 191, 255
extern TCODLIB_API const TCOD_color_t TCOD_lighter_blue;  // 127, 127, 255
extern TCODLIB_API const TCOD_color_t TCOD_lighter_han;  // 159, 127, 255
extern TCODLIB_API const TCOD_color_t TCOD_lighter_violet;  // 191, 127, 255
extern TCODLIB_API const TCOD_color_t TCOD_lighter_purple;  // 223, 127, 255
extern TCODLIB_API const TCOD_color_t TCOD_lighter_fuchsia;  // 255, 127, 255
extern TCODLIB_API const TCOD_color_t TCOD_lighter_magenta;  // 255, 127, 223
extern TCODLIB_API const TCOD_color_t TCOD_lighter_pink;  // 255, 127, 191
extern TCODLIB_API const TCOD_color_t TCOD_lighter_crimson;  // 255, 127, 159

/* lightest colors */
extern TCODLIB_API const TCOD_color_t TCOD_lightest_red;  // 255, 191, 191
extern TCODLIB_API const TCOD_color_t TCOD_lightest_flame;  // 255, 207, 191
extern TCODLIB_API const TCOD_color_t TCOD_lightest_orange;  // 255, 223, 191
extern TCODLIB_API const TCOD_color_t TCOD_lightest_amber;  // 255, 239, 191
extern TCODLIB_API const TCOD_color_t TCOD_lightest_yellow;  // 255, 255, 191
extern TCODLIB_API const TCOD_color_t TCOD_lightest_lime;  // 239, 255, 191
extern TCODLIB_API const TCOD_color_t TCOD_lightest_chartreuse;  // 223, 255, 191
extern TCODLIB_API const TCOD_color_t TCOD_lightest_green;  // 191, 255, 191
extern TCODLIB_API const TCOD_color_t TCOD_lightest_sea;  // 191, 255, 223
extern TCODLIB_API const TCOD_color_t TCOD_lightest_turquoise;  // 191, 255, 239
extern TCODLIB_API const TCOD_color_t TCOD_lightest_cyan;  // 191, 255, 255
extern TCODLIB_API const TCOD_color_t TCOD_lightest_sky;  // 191, 239, 255
extern TCODLIB_API const TCOD_color_t TCOD_lightest_azure;  // 191, 223, 255
extern TCODLIB_API const TCOD_color_t TCOD_lightest_blue;  // 191, 191, 255
extern TCODLIB_API const TCOD_color_t TCOD_lightest_han;  // 207, 191, 255
extern TCODLIB_API const TCOD_color_t TCOD_lightest_violet;  // 223, 191, 255
extern TCODLIB_API const TCOD_color_t TCOD_lightest_purple;  // 239, 191, 255
extern TCODLIB_API const TCOD_color_t TCOD_lightest_fuchsia;  // 255, 191, 255
extern TCODLIB_API const TCOD_color_t TCOD_lightest_magenta;  // 255, 191, 239
extern TCODLIB_API const TCOD_color_t TCOD_lightest_pink;  // 255, 191, 223
extern TCODLIB_API const TCOD_color_t TCOD_lightest_crimson;  // 255, 191, 207

/* desaturated */
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_red;  // 127, 63, 63
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_flame;  // 127, 79, 63
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_orange;  // 127, 95, 63
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_amber;  // 127, 111, 63
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_yellow;  // 127, 127, 63
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_lime;  // 111, 127, 63
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_chartreuse;  // 95, 127, 63
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_green;  // 63, 127, 63
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_sea;  // 63, 127, 95
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_turquoise;  // 63, 127, 111
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_cyan;  // 63, 127, 127
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_sky;  // 63, 111, 127
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_azure;  // 63, 95, 127
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_blue;  // 63, 63, 127
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_han;  // 79, 63, 127
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_violet;  // 95, 63, 127
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_purple;  // 111, 63, 127
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_fuchsia;  // 127, 63, 127
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_magenta;  // 127, 63, 111
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_pink;  // 127, 63, 95
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_crimson;  // 127, 63, 79

/* metallic */
extern TCODLIB_API const TCOD_color_t TCOD_brass;  // 191, 151, 96
extern TCODLIB_API const TCOD_color_t TCOD_copper;  // 197, 136, 124
extern TCODLIB_API const TCOD_color_t TCOD_gold;  // 229, 191, 0
extern TCODLIB_API const TCOD_color_t TCOD_silver;  // 203, 203, 203

/* miscellaneous */
extern TCODLIB_API const TCOD_color_t TCOD_celadon;  // 172, 255, 175
extern TCODLIB_API const TCOD_color_t TCOD_peach;  // 255, 159, 127

#ifdef __cplusplus
}  // extern "C"
namespace tcod {
/***************************************************************************
    @brief A C++ RGB color, used to handle conversions between color types.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
struct ColorRGB : public TCOD_ColorRGB {
 public:
  /***************************************************************************
      @brief Default construct a black ColorRGB object.  RGB values are zero.
   */
  constexpr ColorRGB() noexcept : ColorRGB{0, 0, 0} {}
  /***************************************************************************
      @brief Construct a ColorRGB object with the provided color.
   */
  constexpr ColorRGB(uint8_t red, uint8_t green, uint8_t blue) noexcept : TCOD_ColorRGB{red, green, blue} {}
  /***************************************************************************
      @brief Construct a ColorRGB object from an TCOD_ColorRGB struct.
   */
  explicit constexpr ColorRGB(const TCOD_ColorRGB& rhs) noexcept : TCOD_ColorRGB{rhs} {}
  /***************************************************************************
      @brief Construct a ColorRGB object from an RGBA color, truncating the alpha.
   */
  explicit constexpr ColorRGB(const TCOD_ColorRGBA& rhs) noexcept : ColorRGB{rhs.r, rhs.g, rhs.b} {}
  /***************************************************************************
      @brief Allow implicit casts to RGBA colors, where alpha=255 is implied.
   */
  [[nodiscard]] constexpr operator const TCOD_ColorRGBA() const noexcept { return TCOD_ColorRGBA{r, g, b, 255}; }
  /***************************************************************************
      @brief Allow explicit casts to a TCOD_ColorRGB pointer.
   */
  [[nodiscard]] constexpr explicit operator TCOD_ColorRGB*() noexcept { return this; }
  /***************************************************************************
      @brief Allow explicit casts to a const TCOD_ColorRGB pointer.
   */
  [[nodiscard]] constexpr explicit operator const TCOD_ColorRGB*() const noexcept { return this; }
};
/***************************************************************************
    @brief A C++ RGBA color, used to handle conversions between color types.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
struct ColorRGBA : public TCOD_ColorRGBA {
 public:
  /***************************************************************************
      @brief Default construct a black ColorRGBA object.  RGB values are zero, alpha is 255.
   */
  constexpr ColorRGBA() noexcept : ColorRGBA{0, 0, 0, 255} {}
  /***************************************************************************
      @brief Construct a ColorRGBA object with the provided color and alpha.
   */
  constexpr ColorRGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255) noexcept
      : TCOD_ColorRGBA{red, green, blue, alpha} {}
  /***************************************************************************
      @brief Construct a ColorRGBA object by adding an alpha channel to an RGB object.
   */
  explicit constexpr ColorRGBA(const TCOD_ColorRGB& rhs, uint8_t alpha = 255) noexcept
      : TCOD_ColorRGBA{rhs.r, rhs.b, rhs.b, alpha} {}
  /***************************************************************************
      @brief Construct a ColorRGBA object from an TCOD_ColorRGBA struct.
   */
  explicit constexpr ColorRGBA(const TCOD_ColorRGBA& rhs) noexcept : TCOD_ColorRGBA{rhs} {}
  /***************************************************************************
      @brief Allow explicit conversions to a TCOD_ColorRGB struct.
   */
  [[nodiscard]] constexpr explicit operator TCOD_ColorRGB() const noexcept { return TCOD_ColorRGB{r, g, b}; };
  /***************************************************************************
      @brief Allow explicit conversions to a TCOD_ColorRGBA pointer.
   */
  [[nodiscard]] constexpr explicit operator TCOD_ColorRGBA*() noexcept { return this; }
  /***************************************************************************
      @brief Allow explicit conversions to a const TCOD_ColorRGBA pointer.
   */
  [[nodiscard]] constexpr explicit operator const TCOD_ColorRGBA*() const noexcept { return this; }
};
}  // namespace tcod
#endif
#endif
