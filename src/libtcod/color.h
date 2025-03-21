/* BSD 3-Clause License
 *
 * Copyright © 2008-2025, Jice and the libtcod contributors.
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
#pragma once
#ifndef TCOD_COLOR_H_
#define TCOD_COLOR_H_

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
    if (ch != '{') throw std::runtime_error("Expected missing '{'.");
    int channel;
    in >> channel;
    rgb.r = static_cast<uint8_t>(channel);
    in >> ch;
    if (ch != ',') throw std::runtime_error("Expected missing ',' delimiter.");
    in >> channel;
    rgb.g = static_cast<uint8_t>(channel);
    in >> ch;
    if (ch != ',') throw std::runtime_error("Expected missing ',' delimiter.");
    in >> channel;
    rgb.b = static_cast<uint8_t>(channel);
    in >> ch;
    if (ch != '}') throw std::runtime_error("Expected missing '}'.");
    return in;
  }
  template <class Archive>
  void serialize(Archive& archive) {
    archive(r, g, b);
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
    if (ch != '{') throw std::runtime_error("Expected missing '{'.");
    int channel;
    in >> channel;
    rgba.r = static_cast<uint8_t>(channel);
    in >> ch;
    if (ch != ',') throw std::runtime_error("Expected missing ',' delimiter.");
    in >> channel;
    rgba.g = static_cast<uint8_t>(channel);
    in >> ch;
    if (ch != ',') throw std::runtime_error("Expected missing ',' delimiter.");
    in >> channel;
    rgba.b = static_cast<uint8_t>(channel);
    in >> ch;
    if (ch == '}') {
      rgba.a = 255;
      return in;
    }
    if (ch != ',') throw std::runtime_error("Expected missing ',' delimiter.");
    in >> channel;
    rgba.a = static_cast<uint8_t>(channel);
    in >> ch;
    if (ch != '}') throw std::runtime_error("Expected missing '}'.");
    return in;
  }
  template <class Archive>
  void serialize(Archive& archive) {
    archive(r, g, b, a);
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

/// @cond INTERNAL
/***************************************************************************
    @brief Color names.

    \rst
    .. deprecated:: 1.23
    \endrst
 */
enum {
  TCOD_COLOR_RED TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_FLAME TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_ORANGE TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_AMBER TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_YELLOW TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_LIME TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_CHARTREUSE TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_GREEN TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_SEA TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_TURQUOISE TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_CYAN TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_SKY TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_AZURE TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_BLUE TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_HAN TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_VIOLET TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_PURPLE TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_FUCHSIA TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_MAGENTA TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_PINK TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_CRIMSON TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_NB
};

#if defined(_MSC_VER) && !defined(__clang__)
#pragma deprecated(TCOD_COLOR_RED)
#pragma deprecated(TCOD_COLOR_FLAME)
#pragma deprecated(TCOD_COLOR_ORANGE)
#pragma deprecated(TCOD_COLOR_AMBER)
#pragma deprecated(TCOD_COLOR_YELLOW)
#pragma deprecated(TCOD_COLOR_LIME)
#pragma deprecated(TCOD_COLOR_CHARTREUSE)
#pragma deprecated(TCOD_COLOR_GREEN)
#pragma deprecated(TCOD_COLOR_SEA)
#pragma deprecated(TCOD_COLOR_TURQUOISE)
#pragma deprecated(TCOD_COLOR_CYAN)
#pragma deprecated(TCOD_COLOR_SKY)
#pragma deprecated(TCOD_COLOR_AZURE)
#pragma deprecated(TCOD_COLOR_BLUE)
#pragma deprecated(TCOD_COLOR_HAN)
#pragma deprecated(TCOD_COLOR_VIOLET)
#pragma deprecated(TCOD_COLOR_PURPLE)
#pragma deprecated(TCOD_COLOR_FUCHSIA)
#pragma deprecated(TCOD_COLOR_MAGENTA)
#pragma deprecated(TCOD_COLOR_PINK)
#pragma deprecated(TCOD_COLOR_CRIMSON)
#endif  // _MSC_VER

/***************************************************************************
    @brief Color levels

    \rst
    .. deprecated:: 1.23
    \endrst
 */
enum {
  TCOD_COLOR_DESATURATED TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_LIGHTEST TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_LIGHTER TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_LIGHT TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_NORMAL TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_DARK TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_DARKER TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_DARKEST TCOD_DEPRECATED_ENUM,
  TCOD_COLOR_LEVELS
};

#if defined(_MSC_VER) && !defined(__clang__)
#pragma deprecated(TCOD_COLOR_DESATURATED)
#pragma deprecated(TCOD_COLOR_LIGHTEST)
#pragma deprecated(TCOD_COLOR_LIGHTER)
#pragma deprecated(TCOD_COLOR_LIGHT)
#pragma deprecated(TCOD_COLOR_NORMAL)
#pragma deprecated(TCOD_COLOR_DARK)
#pragma deprecated(TCOD_COLOR_DARKER)
#pragma deprecated(TCOD_COLOR_DARKEST)
#endif  // _MSC_VER

/* color array */
extern TCODLIB_API const TCOD_color_t TCOD_colors[TCOD_COLOR_NB][TCOD_COLOR_LEVELS];

/* grey levels */
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 0, 0}") extern TCODLIB_API const TCOD_color_t TCOD_black;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){31, 31, 31}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_grey;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 63, 63}") extern TCODLIB_API const TCOD_color_t TCOD_darker_grey;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){95, 95, 95}") extern TCODLIB_API const TCOD_color_t TCOD_dark_grey;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 127, 127}") extern TCODLIB_API const TCOD_color_t TCOD_grey;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){159, 159, 159}") extern TCODLIB_API const TCOD_color_t TCOD_light_grey;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 191, 191}") extern TCODLIB_API const TCOD_color_t TCOD_lighter_grey;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){223, 223, 223}") extern TCODLIB_API const TCOD_color_t TCOD_lightest_grey;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){31, 31, 31}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_gray;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 63, 63}") extern TCODLIB_API const TCOD_color_t TCOD_darker_gray;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){95, 95, 95}") extern TCODLIB_API const TCOD_color_t TCOD_dark_gray;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 127, 127}") extern TCODLIB_API const TCOD_color_t TCOD_gray;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){159, 159, 159}") extern TCODLIB_API const TCOD_color_t TCOD_light_gray;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 191, 191}") extern TCODLIB_API const TCOD_color_t TCOD_lighter_gray;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){223, 223, 223}") extern TCODLIB_API const TCOD_color_t TCOD_lightest_gray;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 255, 255}") extern TCODLIB_API const TCOD_color_t TCOD_white;

/* sepia */
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){31, 24, 15}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_sepia;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 50, 31}") extern TCODLIB_API const TCOD_color_t TCOD_darker_sepia;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){94, 75, 47}") extern TCODLIB_API const TCOD_color_t TCOD_dark_sepia;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 101, 63}") extern TCODLIB_API const TCOD_color_t TCOD_sepia;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){158, 134, 100}") extern TCODLIB_API const TCOD_color_t TCOD_light_sepia;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 171, 143}") extern TCODLIB_API const TCOD_color_t TCOD_lighter_sepia;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){222, 211, 195}")
extern TCODLIB_API const TCOD_color_t TCOD_lightest_sepia;

/* standard colors */
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 0, 0}") extern TCODLIB_API const TCOD_color_t TCOD_red;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 63, 0}") extern TCODLIB_API const TCOD_color_t TCOD_flame;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 127, 0}") extern TCODLIB_API const TCOD_color_t TCOD_orange;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 191, 0}") extern TCODLIB_API const TCOD_color_t TCOD_amber;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 255, 0}") extern TCODLIB_API const TCOD_color_t TCOD_yellow;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 255, 0}") extern TCODLIB_API const TCOD_color_t TCOD_lime;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 255, 0}") extern TCODLIB_API const TCOD_color_t TCOD_chartreuse;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 255, 0}") extern TCODLIB_API const TCOD_color_t TCOD_green;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 255, 127}") extern TCODLIB_API const TCOD_color_t TCOD_sea;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 255, 191}") extern TCODLIB_API const TCOD_color_t TCOD_turquoise;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 255, 255}") extern TCODLIB_API const TCOD_color_t TCOD_cyan;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 191, 255}") extern TCODLIB_API const TCOD_color_t TCOD_sky;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 127, 255}") extern TCODLIB_API const TCOD_color_t TCOD_azure;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 0, 255}") extern TCODLIB_API const TCOD_color_t TCOD_blue;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 0, 255}") extern TCODLIB_API const TCOD_color_t TCOD_han;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 0, 255}") extern TCODLIB_API const TCOD_color_t TCOD_violet;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 0, 255}") extern TCODLIB_API const TCOD_color_t TCOD_purple;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 0, 255}") extern TCODLIB_API const TCOD_color_t TCOD_fuchsia;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 0, 191}") extern TCODLIB_API const TCOD_color_t TCOD_magenta;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 0, 127}") extern TCODLIB_API const TCOD_color_t TCOD_pink;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 0, 63}") extern TCODLIB_API const TCOD_color_t TCOD_crimson;

/* dark colors */
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 0, 0}") extern TCODLIB_API const TCOD_color_t TCOD_dark_red;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 47, 0}") extern TCODLIB_API const TCOD_color_t TCOD_dark_flame;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 95, 0}") extern TCODLIB_API const TCOD_color_t TCOD_dark_orange;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 143, 0}") extern TCODLIB_API const TCOD_color_t TCOD_dark_amber;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 191, 0}") extern TCODLIB_API const TCOD_color_t TCOD_dark_yellow;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){143, 191, 0}") extern TCODLIB_API const TCOD_color_t TCOD_dark_lime;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){95, 191, 0}") extern TCODLIB_API const TCOD_color_t TCOD_dark_chartreuse;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 191, 0}") extern TCODLIB_API const TCOD_color_t TCOD_dark_green;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 191, 95}") extern TCODLIB_API const TCOD_color_t TCOD_dark_sea;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 191, 143}") extern TCODLIB_API const TCOD_color_t TCOD_dark_turquoise;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 191, 191}") extern TCODLIB_API const TCOD_color_t TCOD_dark_cyan;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 143, 191}") extern TCODLIB_API const TCOD_color_t TCOD_dark_sky;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 95, 191}") extern TCODLIB_API const TCOD_color_t TCOD_dark_azure;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 0, 191}") extern TCODLIB_API const TCOD_color_t TCOD_dark_blue;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){47, 0, 191}") extern TCODLIB_API const TCOD_color_t TCOD_dark_han;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){95, 0, 191}") extern TCODLIB_API const TCOD_color_t TCOD_dark_violet;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){143, 0, 191}") extern TCODLIB_API const TCOD_color_t TCOD_dark_purple;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 0, 191}") extern TCODLIB_API const TCOD_color_t TCOD_dark_fuchsia;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 0, 143}") extern TCODLIB_API const TCOD_color_t TCOD_dark_magenta;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 0, 95}") extern TCODLIB_API const TCOD_color_t TCOD_dark_pink;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 0, 47}") extern TCODLIB_API const TCOD_color_t TCOD_dark_crimson;

/* darker colors */
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 0, 0}") extern TCODLIB_API const TCOD_color_t TCOD_darker_red;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 31, 0}") extern TCODLIB_API const TCOD_color_t TCOD_darker_flame;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 63, 0}") extern TCODLIB_API const TCOD_color_t TCOD_darker_orange;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 95, 0}") extern TCODLIB_API const TCOD_color_t TCOD_darker_amber;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 127, 0}") extern TCODLIB_API const TCOD_color_t TCOD_darker_yellow;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){95, 127, 0}") extern TCODLIB_API const TCOD_color_t TCOD_darker_lime;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 127, 0}")
extern TCODLIB_API const TCOD_color_t TCOD_darker_chartreuse;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 127, 0}") extern TCODLIB_API const TCOD_color_t TCOD_darker_green;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 127, 63}") extern TCODLIB_API const TCOD_color_t TCOD_darker_sea;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 127, 95}") extern TCODLIB_API const TCOD_color_t TCOD_darker_turquoise;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 127, 127}") extern TCODLIB_API const TCOD_color_t TCOD_darker_cyan;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 95, 127}") extern TCODLIB_API const TCOD_color_t TCOD_darker_sky;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 63, 127}") extern TCODLIB_API const TCOD_color_t TCOD_darker_azure;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 0, 127}") extern TCODLIB_API const TCOD_color_t TCOD_darker_blue;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){31, 0, 127}") extern TCODLIB_API const TCOD_color_t TCOD_darker_han;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 0, 127}") extern TCODLIB_API const TCOD_color_t TCOD_darker_violet;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){95, 0, 127}") extern TCODLIB_API const TCOD_color_t TCOD_darker_purple;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 0, 127}") extern TCODLIB_API const TCOD_color_t TCOD_darker_fuchsia;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 0, 95}") extern TCODLIB_API const TCOD_color_t TCOD_darker_magenta;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 0, 63}") extern TCODLIB_API const TCOD_color_t TCOD_darker_pink;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 0, 31}") extern TCODLIB_API const TCOD_color_t TCOD_darker_crimson;

/* darkest colors */
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 0, 0}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_red;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 15, 0}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_flame;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 31, 0}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_orange;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 47, 0}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_amber;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 63, 0}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_yellow;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){47, 63, 0}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_lime;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){31, 63, 0}")
extern TCODLIB_API const TCOD_color_t TCOD_darkest_chartreuse;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 63, 0}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_green;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 63, 31}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_sea;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 63, 47}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_turquoise;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 63, 63}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_cyan;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 47, 63}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_sky;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 31, 63}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_azure;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){0, 0, 63}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_blue;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){15, 0, 63}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_han;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){31, 0, 63}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_violet;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){47, 0, 63}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_purple;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 0, 63}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_fuchsia;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 0, 47}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_magenta;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 0, 31}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_pink;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 0, 15}") extern TCODLIB_API const TCOD_color_t TCOD_darkest_crimson;

/* light colors */
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 63, 63}") extern TCODLIB_API const TCOD_color_t TCOD_light_red;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 111, 63}") extern TCODLIB_API const TCOD_color_t TCOD_light_flame;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 159, 63}") extern TCODLIB_API const TCOD_color_t TCOD_light_orange;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 207, 63}") extern TCODLIB_API const TCOD_color_t TCOD_light_amber;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 255, 63}") extern TCODLIB_API const TCOD_color_t TCOD_light_yellow;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){207, 255, 63}") extern TCODLIB_API const TCOD_color_t TCOD_light_lime;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){159, 255, 63}")
extern TCODLIB_API const TCOD_color_t TCOD_light_chartreuse;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 255, 63}") extern TCODLIB_API const TCOD_color_t TCOD_light_green;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 255, 159}") extern TCODLIB_API const TCOD_color_t TCOD_light_sea;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 255, 207}")
extern TCODLIB_API const TCOD_color_t TCOD_light_turquoise;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 255, 255}") extern TCODLIB_API const TCOD_color_t TCOD_light_cyan;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 207, 255}") extern TCODLIB_API const TCOD_color_t TCOD_light_sky;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 159, 255}") extern TCODLIB_API const TCOD_color_t TCOD_light_azure;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 63, 255}") extern TCODLIB_API const TCOD_color_t TCOD_light_blue;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){111, 63, 255}") extern TCODLIB_API const TCOD_color_t TCOD_light_han;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){159, 63, 255}") extern TCODLIB_API const TCOD_color_t TCOD_light_violet;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){207, 63, 255}") extern TCODLIB_API const TCOD_color_t TCOD_light_purple;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 63, 255}") extern TCODLIB_API const TCOD_color_t TCOD_light_fuchsia;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 63, 207}") extern TCODLIB_API const TCOD_color_t TCOD_light_magenta;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 63, 159}") extern TCODLIB_API const TCOD_color_t TCOD_light_pink;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 63, 111}") extern TCODLIB_API const TCOD_color_t TCOD_light_crimson;

/* lighter colors */
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 127, 127}") extern TCODLIB_API const TCOD_color_t TCOD_lighter_red;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 159, 127}") extern TCODLIB_API const TCOD_color_t TCOD_lighter_flame;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 191, 127}")
extern TCODLIB_API const TCOD_color_t TCOD_lighter_orange;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 223, 127}") extern TCODLIB_API const TCOD_color_t TCOD_lighter_amber;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 255, 127}")
extern TCODLIB_API const TCOD_color_t TCOD_lighter_yellow;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){223, 255, 127}") extern TCODLIB_API const TCOD_color_t TCOD_lighter_lime;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 255, 127}")
extern TCODLIB_API const TCOD_color_t TCOD_lighter_chartreuse;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 255, 127}") extern TCODLIB_API const TCOD_color_t TCOD_lighter_green;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 255, 191}") extern TCODLIB_API const TCOD_color_t TCOD_lighter_sea;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 255, 223}")
extern TCODLIB_API const TCOD_color_t TCOD_lighter_turquoise;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 255, 255}") extern TCODLIB_API const TCOD_color_t TCOD_lighter_cyan;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 223, 255}") extern TCODLIB_API const TCOD_color_t TCOD_lighter_sky;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 191, 255}") extern TCODLIB_API const TCOD_color_t TCOD_lighter_azure;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 127, 255}") extern TCODLIB_API const TCOD_color_t TCOD_lighter_blue;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){159, 127, 255}") extern TCODLIB_API const TCOD_color_t TCOD_lighter_han;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 127, 255}")
extern TCODLIB_API const TCOD_color_t TCOD_lighter_violet;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){223, 127, 255}")
extern TCODLIB_API const TCOD_color_t TCOD_lighter_purple;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 127, 255}")
extern TCODLIB_API const TCOD_color_t TCOD_lighter_fuchsia;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 127, 223}")
extern TCODLIB_API const TCOD_color_t TCOD_lighter_magenta;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 127, 191}") extern TCODLIB_API const TCOD_color_t TCOD_lighter_pink;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 127, 159}")
extern TCODLIB_API const TCOD_color_t TCOD_lighter_crimson;

/* lightest colors */
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 191, 191}") extern TCODLIB_API const TCOD_color_t TCOD_lightest_red;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 207, 191}")
extern TCODLIB_API const TCOD_color_t TCOD_lightest_flame;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 223, 191}")
extern TCODLIB_API const TCOD_color_t TCOD_lightest_orange;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 239, 191}")
extern TCODLIB_API const TCOD_color_t TCOD_lightest_amber;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 255, 191}")
extern TCODLIB_API const TCOD_color_t TCOD_lightest_yellow;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){239, 255, 191}") extern TCODLIB_API const TCOD_color_t TCOD_lightest_lime;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){223, 255, 191}")
extern TCODLIB_API const TCOD_color_t TCOD_lightest_chartreuse;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 255, 191}")
extern TCODLIB_API const TCOD_color_t TCOD_lightest_green;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 255, 223}") extern TCODLIB_API const TCOD_color_t TCOD_lightest_sea;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 255, 239}")
extern TCODLIB_API const TCOD_color_t TCOD_lightest_turquoise;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 255, 255}") extern TCODLIB_API const TCOD_color_t TCOD_lightest_cyan;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 239, 255}") extern TCODLIB_API const TCOD_color_t TCOD_lightest_sky;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 223, 255}")
extern TCODLIB_API const TCOD_color_t TCOD_lightest_azure;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 191, 255}") extern TCODLIB_API const TCOD_color_t TCOD_lightest_blue;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){207, 191, 255}") extern TCODLIB_API const TCOD_color_t TCOD_lightest_han;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){223, 191, 255}")
extern TCODLIB_API const TCOD_color_t TCOD_lightest_violet;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){239, 191, 255}")
extern TCODLIB_API const TCOD_color_t TCOD_lightest_purple;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 191, 255}")
extern TCODLIB_API const TCOD_color_t TCOD_lightest_fuchsia;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 191, 239}")
extern TCODLIB_API const TCOD_color_t TCOD_lightest_magenta;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 191, 223}") extern TCODLIB_API const TCOD_color_t TCOD_lightest_pink;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 191, 207}")
extern TCODLIB_API const TCOD_color_t TCOD_lightest_crimson;

/* desaturated */
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 63, 63}") extern TCODLIB_API const TCOD_color_t TCOD_desaturated_red;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 79, 63}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_flame;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 95, 63}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_orange;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 111, 63}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_amber;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 127, 63}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_yellow;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){111, 127, 63}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_lime;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){95, 127, 63}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_chartreuse;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 127, 63}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_green;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 127, 95}") extern TCODLIB_API const TCOD_color_t TCOD_desaturated_sea;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 127, 111}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_turquoise;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 127, 127}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_cyan;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 111, 127}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_sky;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 95, 127}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_azure;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){63, 63, 127}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_blue;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){79, 63, 127}") extern TCODLIB_API const TCOD_color_t TCOD_desaturated_han;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){95, 63, 127}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_violet;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){111, 63, 127}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_purple;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 63, 127}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_fuchsia;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 63, 111}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_magenta;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 63, 95}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_pink;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){127, 63, 79}")
extern TCODLIB_API const TCOD_color_t TCOD_desaturated_crimson;

/* metallic */
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){191, 151, 96}") extern TCODLIB_API const TCOD_color_t TCOD_brass;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){197, 136, 124}") extern TCODLIB_API const TCOD_color_t TCOD_copper;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){229, 191, 0}") extern TCODLIB_API const TCOD_color_t TCOD_gold;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){203, 203, 203}") extern TCODLIB_API const TCOD_color_t TCOD_silver;

/* miscellaneous */
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){172, 255, 175}") extern TCODLIB_API const TCOD_color_t TCOD_celadon;
TCOD_DEPRECATED("Replace with (TCOD_ColorRGB){255, 159, 127}") extern TCODLIB_API const TCOD_color_t TCOD_peach;

/// @endcond
#ifdef __cplusplus
}  // extern "C"
#endif
#endif  // TCOD_COLOR_H_
