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
#include "console_etc.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "console_drawing.h"
#include "console_rexpaint.h"
#include "context.h"
#include "error.h"
#include "globals.h"
#include "libtcod_int.h"
#include "mersenne.h"
#include "noise.h"
#include "tileset.h"
#include "utility.h"
#include "version.h"

#if defined(TCOD_VISUAL_STUDIO)
static const char* version_string = "libtcod " TCOD_STRVERSION;
#else
static const char* version_string __attribute__((unused)) = "libtcod " TCOD_STRVERSION;
#endif

TCOD_internal_context_t TCOD_ctx = {
    /* number of characters in the bitmap font */
    16,
    16,
    /* font type and layout */
    false,
    false,
    false,
    0,
    /* character size in font */
    0,
    0,
    "terminal.png",
    "",
    NULL,
    NULL,
    NULL,
    0,
    false,
    0,
    0,
    0,
    0,
    0,
    0,
    NULL,  // sdl_cbk
    /* fading data */
    {0, 0, 0},
    255,
    /*key state*/
    {0},
    /* window closed ? */
    false,
    /* mouse focus ? */
    false,
    /* application active ? */
    true,
    NULL,
    NULL,
};
TCOD_Error TCOD_console_flush_ex(TCOD_Console* console, struct TCOD_ViewportOptions* viewport) {
  console = TCOD_console_validate_(console);
  if (!console) {
    TCOD_set_errorv("Console must not be NULL or root console must exist.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!TCOD_ctx.engine) {
    return TCOD_set_errorv("Rendering context is not yet initialized.");
  }
  TCOD_Error err = TCOD_E_OK;
  if (TCOD_ctx.fade == 255) {
    err = TCOD_context_present(TCOD_ctx.engine, console, viewport);
  } else {
    // Apply the global fading color before presenting.
    TCOD_Console* root_copy = TCOD_console_new(TCOD_ctx.root->w, TCOD_ctx.root->h);
    if (!root_copy) {
      return TCOD_E_ERROR;
    }
    const TCOD_ColorRGBA fade_color = {
        TCOD_ctx.fading_color.r,
        TCOD_ctx.fading_color.g,
        TCOD_ctx.fading_color.b,
        255 - TCOD_ctx.fade,
    };
    for (int i = 0; i < root_copy->elements; ++i) {
      root_copy->tiles[i] = TCOD_ctx.root->tiles[i];
      TCOD_color_alpha_blend(&root_copy->tiles[i].fg, &fade_color);
      TCOD_color_alpha_blend(&root_copy->tiles[i].bg, &fade_color);
    }
    err = TCOD_context_present(TCOD_ctx.engine, root_copy, viewport);
    TCOD_console_delete(root_copy);
  }
#ifndef NO_SDL
  sync_time_();
#endif  // NO_SDL
  return err;
}
TCOD_Error TCOD_console_flush(void) { return TCOD_console_flush_ex(NULL, NULL); }
/**
 *  Manually mark a region of a console as dirty.
 */
void TCOD_console_set_dirty(int dx, int dy, int dw, int dh) {
  (void)dx;
  (void)dy;
  (void)dw;
  (void)dh;  // Ignore.
}
/**
 *  \brief Set a font image to be loaded during initialization.
 *
 *  \param fontFile The path to a font image.
 *  \param flags A TCOD_font_flags_t bit-field describing the font image
 *               contents.
 *  \param nb_char_horiz The number of columns in the font image.
 *  \param nb_char_vertic The number of rows in the font image.
 *
 *  `fontFile` will be case-sensitive depending on the platform.
 *
 *  Returns 0 on success, or -1 on an error, you can check the error with
 *  TCOD_sys_get_error()
 *  \rst
 *  .. versionchanged:: 1.12
 *      Now returns -1 on error instead of crashing.
 *  \endrst
 */
TCOD_Error TCOD_console_set_custom_font(const char* fontFile, int flags, int nb_char_horiz, int nb_char_vertic) {
#ifndef TCOD_NO_PNG
  strncpy(TCOD_ctx.font_file, fontFile, sizeof(TCOD_ctx.font_file) - 1);
  /* if layout not defined, assume ASCII_INCOL */
  if (!(flags & (TCOD_FONT_LAYOUT_ASCII_INCOL | TCOD_FONT_LAYOUT_ASCII_INROW | TCOD_FONT_LAYOUT_TCOD))) {
    flags |= TCOD_FONT_LAYOUT_ASCII_INCOL;
  }
  TCOD_ctx.font_in_row = ((flags & TCOD_FONT_LAYOUT_ASCII_INROW) != 0);
  TCOD_ctx.font_greyscale = ((flags & TCOD_FONT_TYPE_GREYSCALE) != 0);
  TCOD_ctx.font_tcod_layout = ((flags & TCOD_FONT_LAYOUT_TCOD) != 0);
  TCOD_ctx.font_flags = flags;
  if (nb_char_horiz > 0 && nb_char_vertic > 0) {
    TCOD_ctx.fontNbCharHoriz = nb_char_horiz;
    TCOD_ctx.fontNbCharVertic = nb_char_vertic;
  } else {
    if (flags & TCOD_FONT_LAYOUT_TCOD) {
      TCOD_ctx.fontNbCharHoriz = nb_char_horiz = 32;
      TCOD_ctx.fontNbCharVertic = nb_char_vertic = 8;
    } else {
      TCOD_ctx.fontNbCharHoriz = nb_char_horiz = 16;
      TCOD_ctx.fontNbCharVertic = nb_char_vertic = 16;
    }
  }
  if (TCOD_ctx.font_tcod_layout) {
    TCOD_ctx.font_in_row = true;
  }

  TCOD_Tileset* tileset = TCOD_tileset_load(fontFile, nb_char_horiz, nb_char_vertic, 0, NULL);
  if (!tileset) {
    return TCOD_E_ERROR;
  }
  TCOD_set_default_tileset(tileset);
  TCOD_sys_decode_font_();
  return TCOD_E_OK;
#else
  return TCOD_set_errorv("Can not call TCOD_console_set_custom_font without PNG support.");
#endif  // TCOD_NO_PNG
}
/**
 *  \brief Remap a character code to a tile.
 *
 *  \param asciiCode Character code to modify.
 *  \param fontCharX X tile-coordinate, starting from the left at zero.
 *  \param fontCharY Y tile-coordinate, starting from the top at zero.
 *
 *  X,Y parameters are the coordinate of the tile, not pixel-coordinates.
 */
void TCOD_console_map_ascii_code_to_font(int asciiCode, int fontCharX, int fontCharY) {
  TCOD_sys_map_ascii_to_font(asciiCode, fontCharX, fontCharY);
}
/**
 *  \brief Remap a series of character codes to a row of tiles.
 *
 *  \param asciiCode The starting character code.
 *  \param nbCodes Number of character codes to assign.
 *  \param fontCharX First X tile-coordinate, starting from the left at zero.
 *  \param fontCharY First Y tile-coordinate, starting from the top at zero.
 *
 *  This function always assigns tiles in row-major order, even if the
 *  TCOD_FONT_LAYOUT_ASCII_INCOL flag was set.
 */
void TCOD_console_map_ascii_codes_to_font(int asciiCode, int nbCodes, int fontCharX, int fontCharY) {
  for (int c = asciiCode; c < asciiCode + nbCodes; ++c) {
    TCOD_sys_map_ascii_to_font(c, fontCharX, fontCharY);
    ++fontCharX;
    if (fontCharX == TCOD_ctx.fontNbCharHoriz) {
      fontCharX = 0;
      ++fontCharY;
    }
  }
}
/**
 *  \brief Remap a string of character codes to a row of tiles.
 *
 *  \param s A null-terminated string.
 *  \param fontCharX First X tile-coordinate, starting from the left at zero.
 *  \param fontCharY First Y tile-coordinate, starting from the top at zero.
 *
 *  This function always assigns tiles in row-major order, even if the
 *  TCOD_FONT_LAYOUT_ASCII_INCOL flag was set.
 */
void TCOD_console_map_string_to_font(const char* s, int fontCharX, int fontCharY) {
  TCOD_IFNOT(s) { return; }
  /* cannot change mapping before initRoot is called */
  TCOD_IFNOT(TCOD_ctx.root) { return; }
  while (*s) {
    TCOD_console_map_ascii_code_to_font(*s, fontCharX, fontCharY);
    ++fontCharX;
    if (fontCharX == TCOD_ctx.fontNbCharHoriz) {
      fontCharX = 0;
      ++fontCharY;
    }
    ++s;
  }
}
void TCOD_console_map_string_to_font_utf(const wchar_t* s, int fontCharX, int fontCharY) {
  TCOD_IFNOT(s != NULL) return;
  while (*s) {
    TCOD_sys_map_ascii_to_font(*s, fontCharX, fontCharY);
    fontCharX++;
    if (fontCharX == TCOD_ctx.fontNbCharHoriz) {
      fontCharX = 0;
      fontCharY++;
    }
    s++;
  }
}
bool TCOD_console_is_key_pressed(TCOD_keycode_t key) {
#ifndef NO_SDL
  return TCOD_sys_is_key_pressed(key);
#else
  return false;
#endif  // NO_SDL
}
void TCOD_console_set_key_color(TCOD_Console* con, TCOD_color_t col) {
  con = TCOD_console_validate_(con);
  if (!con) {
    return;
  }
  con->has_key_color = 1;
  con->key_color = col;
}
#ifndef NO_SDL
TCOD_key_t TCOD_console_wait_for_keypress(bool flush) { return TCOD_sys_wait_for_keypress(flush); }
TCOD_key_t TCOD_console_check_for_keypress(int flags) { return TCOD_sys_check_for_keypress(flags); }
void TCOD_console_credits(void) {
  bool end = false;
  int x = TCOD_console_get_width(NULL) / 2 - 6;
  int y = TCOD_console_get_height(NULL) / 2;
  int fade = 260;
  TCOD_sys_save_fps();
  TCOD_sys_set_fps(25);
  while (!end) {
    TCOD_key_t k;
    end = TCOD_console_credits_render(x, y, false);
    TCOD_sys_check_for_event(TCOD_EVENT_KEY_PRESS, &k, NULL);
    if (fade == 260 && k.vk != TCODK_NONE) {
      fade -= 10;
    }
    if (TCOD_console_flush() < 0) {
      break;
    }
    if (fade < 260) {
      fade -= 10;
      TCOD_console_set_fade(fade, TCOD_black);
      if (fade == 0) end = true;
    }
  }
  TCOD_console_set_fade(255, TCOD_black);
  TCOD_sys_restore_fps();
}

static bool credits_initialized = false;

void TCOD_console_credits_reset(void) { credits_initialized = false; }

/*****************************************************************************
    @brief Return a bitmask based on which quadrants are in the foreground, or -1 on non-quadrant codepoints.

    The returned bitmask is arranged as the following flag values:
        1 2
        4 8
 */
static int8_t quadrants_from_codepoint(int codepoint) {
  switch (codepoint) {
    case 0x20:  // Space.
      return 0;
    case 0x2580:  // Upper half block.
      return 1 | 2;
    case 0x2584:  // Lower half block.
      return 4 | 8;
    case 0x2588:  // Full block.
      return 1 | 2 | 4 | 8;
    case 0x258C:  // Left half block.
      return 1 | 4;
    case 0x2590:  // Right half block.
      return 2 | 8;
    case 0x2596:  // Lower left quadrant.
      return 4;
    case 0x2597:  // Quadrant lower right
      return 8;
    case 0x2598:  // Quadrant upper left
      return 1;
    case 0x2599:  // ▙
      return 1 | 4 | 8;
    case 0x259A:  // ▚
      return 1 | 8;
    case 0x259B:  // ▛
      return 1 | 2 | 4;
    case 0x259C:  // ▜
      return 1 | 2 | 8;
    case 0x259D:  // ▝
      return 2;
    case 0x259E:  // ▞
      return 2 | 4;
    case 0x259F:  // ▟
      return 2 | 4 | 8;
    default:
      return -1;
  }
}

bool TCOD_console_credits_render_ex(TCOD_Console* console, int x, int y, bool alpha, float delta_time) {
  console = TCOD_console_validate_(console);
  if (!console) return false;
  static char powered_by[128];
  static float char_heat[128];
  static int char_x[128];
  static int char_y[128];
  static bool static_vars_initialized = false;
  static int text_length, text_length_upper;
  static int console_w = -1, console_h = -1;
  static float text_progress;
  static TCOD_ColorRGB colormap_heat[64];
  static TCOD_ColorRGB colormap_light[64];
  static TCOD_Noise* noise;
  static const TCOD_ColorRGB color_keys_heat[4] = {
      {255, 255, 204},
      {255, 204, 0},
      {255, 102, 0},
      {102, 153, 255},
  };
  static const TCOD_ColorRGB color_keys_light[4] = {
      {255, 255, 204},
      {128, 128, 77},
      {51, 51, 31},
      {0, 0, 0},
  };
  static const int color_key_positions[4] = {0, 21, 42, 63};
  static TCOD_image_t img = NULL;
  static int left, right, top, bottom;
  /* mini particle system */
#define MAX_PARTICLES 64
  static float particle_heat[MAX_PARTICLES];
  static float particle_x[MAX_PARTICLES], particle_y[MAX_PARTICLES];
  static float particle_x_velocity[MAX_PARTICLES], particle_y_velocity[MAX_PARTICLES];
  static int particle_count = 0;  // Current number of active particles.
  static int first_particle = 0;  // First particle index, particle data wraps around the array.
  static float particle_delay = 0.1f;  // Seconds until the next particle spawns.

  if (!static_vars_initialized) {
    /* initialize all static data, colormaps, ... */
    TCOD_color_gen_map(colormap_heat, 4, color_keys_heat, color_key_positions);
    TCOD_color_gen_map(colormap_light, 4, color_keys_light, color_key_positions);
    text_length = snprintf(powered_by, sizeof(powered_by), "Powered by\n%s", version_string);
    noise = TCOD_noise_new(1, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, NULL);
    text_length_upper = (int)strlen("Powered by\n");
    left = TCOD_MAX(x - 4, 0);
    top = TCOD_MAX(y - 4, 0);
    const TCOD_ColorRGB color = TCOD_console_get_default_background(console);
    TCOD_console_set_default_background(console, TCOD_black);
    TCOD_console_set_default_background(console, color);
    static_vars_initialized = true;
  }
  if (!credits_initialized) {
    /* reset the credits vars ... */
    text_progress = -4.0f;
    int cursor_x = x;
    int cursor_y = y;
    for (int i = 0; i < text_length; ++i) {
      char_heat[i] = -1;
      char_x[i] = cursor_x;
      char_y[i] = cursor_y;
      ++cursor_x;
      if (powered_by[i] == '\n') {
        cursor_x = x;
        ++cursor_y;
      }
    }
    particle_count = first_particle = 0;
    credits_initialized = true;
  }
  if (TCOD_console_get_width(console) != console_w || TCOD_console_get_height(console) != console_h) {
    /* console size has changed */
    console_w = TCOD_console_get_width(console);
    console_h = TCOD_console_get_height(console);
    right = TCOD_MIN(x + text_length, console_w - 1);
    bottom = TCOD_MIN(y + 6, console_h - 1);
    const int width = right - left + 1;
    const int height = bottom - top + 1;
    if (img) TCOD_image_delete(img);
    img = TCOD_image_new(width * 2, height * 2);
  }
  const TCOD_ColorRGB fg_backup = TCOD_console_get_default_foreground(console); /* backup fg color */
  float sparkle_x;
  float sparkle_y;
  if (text_progress < text_length_upper) {
    sparkle_x = x + text_progress;
    sparkle_y = (float)y;
  } else {
    sparkle_x = x - text_length_upper + text_progress;
    sparkle_y = (float)(y + 1);
  }
  const float noise_x = text_progress * 6;
  float sparkle_radius = 3.0f + 2 * TCOD_noise_get(noise, &noise_x);
  if (text_progress >= text_length - 1) {
    sparkle_radius -= (text_progress - text_length + 1) * 4.0f;
  } else if (text_progress < 0.0f) {
    sparkle_radius += text_progress * 4.0f;
  } else if (powered_by[(int)(text_progress + 0.5f)] == ' ' || powered_by[(int)(text_progress + 0.5f)] == '\n') {
    sparkle_radius /= 2;
  }
  const float sparkle_radius2 = sparkle_radius * sparkle_radius * 4;

  /* draw the light */
  for (int xc = left * 2, xi = 0; xc < (right + 1) * 2; ++xc, ++xi) {
    for (int yc = top * 2, yi = 0; yc < (bottom + 1) * 2; ++yc, ++yi) {
      const float dist = ((xc - 2 * sparkle_x) * (xc - 2 * sparkle_x) + (yc - 2 * sparkle_y) * (yc - 2 * sparkle_y));
      TCOD_ColorRGB pixel_color = TCOD_black;
      if (sparkle_radius >= 0.0f && dist < sparkle_radius2) {
        int color_idx =
            63 - (int)(63 * (sparkle_radius2 - dist) / sparkle_radius2) + TCOD_random_get_int(NULL, -10, 10);
        color_idx = TCOD_CLAMP(0, 63, color_idx);
        pixel_color = colormap_light[color_idx];
      }
      if (alpha) {
        TCOD_ColorRGB bg_color = TCOD_console_get_char_background(console, xc / 2, yc / 2);
        const int8_t quadrant_mask = quadrants_from_codepoint(TCOD_console_get_char(console, xc / 2, yc / 2));
        if (quadrant_mask > 0) {
          /* merge two subcell chars...
             get the flag for the existing cell on root console */
          const bool x_flag = (xc & 1);
          const bool y_flag = (yc & 1);
          /* get the flag for the current subcell */
          const int8_t current_quadrant_bit = (1 + 3 * y_flag) * (x_flag + 1);
          if ((current_quadrant_bit & quadrant_mask) != 0) {
            /* the color for this subcell on root console is foreground, not background */
            bg_color = TCOD_console_get_char_foreground(console, xc / 2, yc / 2);
          }
        }
        pixel_color.r = TCOD_MIN(255, bg_color.r + pixel_color.r);
        pixel_color.g = TCOD_MIN(255, bg_color.g + pixel_color.g);
        pixel_color.b = TCOD_MIN(255, bg_color.b + pixel_color.b);
      }
      TCOD_image_put_pixel(img, xi, yi, pixel_color);
    }
  }

  /* draw and update the particles */
  int particles_left_to_update = particle_count;
  int particle_i = first_particle;
  while (particles_left_to_update > 0) {
    const int color_index = TCOD_MIN(63, (int)(64 * (1.0f - particle_heat[particle_i])));
    const TCOD_ColorRGB particle_color = colormap_heat[color_index];
    if (particle_y[particle_i] < (bottom - top + 1) * 2) {
      const int particle_x_int = (int)particle_x[particle_i];
      const int particle_y_int = (int)particle_y[particle_i];
      const float particle_x_fractional = particle_x[particle_i] - particle_x_int;
      const float particle_y_fractional = particle_y[particle_i] - particle_y_int;
      TCOD_ColorRGB col2 = TCOD_image_get_pixel(img, particle_x_int, particle_y_int);
      col2 = TCOD_color_lerp(particle_color, col2, 0.5f * (particle_x_fractional + particle_y_fractional));
      TCOD_image_put_pixel(img, particle_x_int, particle_y_int, col2);
      col2 = TCOD_image_get_pixel(img, particle_x_int + 1, particle_y_int);
      col2 = TCOD_color_lerp(col2, particle_color, particle_x_fractional);
      TCOD_image_put_pixel(img, particle_x_int + 1, particle_y_int, col2);
      col2 = TCOD_image_get_pixel(img, particle_x_int, particle_y_int + 1);
      col2 = TCOD_color_lerp(col2, particle_color, particle_y_fractional);
      TCOD_image_put_pixel(img, particle_x_int, particle_y_int + 1, col2);
    } else {
      particle_y_velocity[particle_i] = -particle_y_velocity[particle_i] * 0.5f;  // Particle bounces on floor.
    }
    particle_x_velocity[particle_i] *= (1.0f - delta_time);
    particle_y_velocity[particle_i] += (1.0f - particle_heat[particle_i]) * delta_time * 300.0f;
    particle_x[particle_i] += particle_x_velocity[particle_i] * delta_time;
    particle_y[particle_i] += particle_y_velocity[particle_i] * delta_time;
    particle_heat[particle_i] -= delta_time * 0.3f;
    if (particle_heat[particle_i] < 0.0f) {  // Particle has died out.
      first_particle = (first_particle + 1) % MAX_PARTICLES;
      --particle_count;
    }
    particle_i = (particle_i + 1) % MAX_PARTICLES;
    --particles_left_to_update;
  }
  particle_delay -= delta_time;
  if (particle_delay < 0.0f && particle_count < MAX_PARTICLES && sparkle_radius > 2.0f) {
    // Fire a new particle.
    const int new_particle = (first_particle + particle_count++) % MAX_PARTICLES;
    particle_x[new_particle] = 2 * (sparkle_x - left);
    particle_y[new_particle] = 2 * (sparkle_y - top) + 2;
    particle_x_velocity[new_particle] = TCOD_random_get_float(NULL, -5.0f, 5.0f);
    particle_y_velocity[new_particle] = TCOD_random_get_float(NULL, -0.5f, -15.0f);
    particle_heat[new_particle] = 1.0f;
    particle_delay += 0.1f;
  }
  TCOD_image_blit_2x(img, console, left, top, 0, 0, -1, -1);
  /* draw the text */
  for (int i = 0; i < text_length; ++i) {
    if (char_heat[i] >= 0.0f && powered_by[i] != '\n') {
      int color_idx = (int)(64 * char_heat[i]);
      TCOD_color_t col;
      color_idx = TCOD_MIN(63, color_idx);
      col = colormap_heat[color_idx];
      if (text_progress >= text_length) {
        float coef = (text_progress - text_length) / text_length;
        if (alpha) {
          TCOD_color_t fore = TCOD_console_get_char_background(console, char_x[i], char_y[i]);
          int r = (int)(coef * fore.r + (1.0f - coef) * col.r);
          int g = (int)(coef * fore.g + (1.0f - coef) * col.g);
          int b = (int)(coef * fore.b + (1.0f - coef) * col.b);
          col.r = (uint8_t)TCOD_MAX(0, TCOD_MIN(r, 255));
          col.g = (uint8_t)TCOD_MAX(0, TCOD_MIN(g, 255));
          col.b = (uint8_t)TCOD_MAX(0, TCOD_MIN(b, 255));
          TCOD_console_set_char_foreground(console, char_x[i], char_y[i], col);
        } else {
          col = TCOD_color_lerp(col, TCOD_black, coef);
        }
      }
      TCOD_console_set_char(console, char_x[i], char_y[i], powered_by[i]);
      TCOD_console_set_char_foreground(console, char_x[i], char_y[i], col);
    }
  }
  /* update letters heat */
  text_progress += delta_time * 4;
  for (int i = 0; i < (int)(text_progress + 0.5f); ++i) {
    char_heat[i] = (text_progress - i) / (text_length / 2);
  }
  /* restore fg color */
  TCOD_console_set_default_foreground(console, fg_backup);
  if (text_progress <= 2 * text_length) return false;
  credits_initialized = false;
  return true;
}
bool TCOD_console_credits_render(int x, int y, bool alpha) {
  return TCOD_console_credits_render_ex(NULL, x, y, alpha, TCOD_sys_get_last_frame_length());
}
void TCOD_console_set_keyboard_repeat(int initial_delay, int interval) {
  (void)initial_delay;
  (void)interval;
}
void TCOD_console_disable_keyboard_repeat(void) {}
#endif  // NO_SDL

static void TCOD_console_read_asc(TCOD_console_t con, FILE* f, int width, int height, float version) {
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con) { return; }
  while (fgetc(f) != '#') {
  }
  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      TCOD_color_t fore, back;
      int c = fgetc(f);
      fore.r = (uint8_t)fgetc(f);
      fore.g = (uint8_t)fgetc(f);
      fore.b = (uint8_t)fgetc(f);
      back.r = (uint8_t)fgetc(f);
      back.g = (uint8_t)fgetc(f);
      back.b = (uint8_t)fgetc(f);
      /* skip solid/walkable info */
      if (version >= 0.3f) {
        fgetc(f);
        fgetc(f);
      }
      TCOD_console_put_char_ex(con, x, y, c, fore, back);
    }
  }
  fclose(f);
}

static int string_ends_with(const char* str, const char* suffix) {
  size_t str_len = strlen(str);
  size_t suffix_len = strlen(suffix);
  return (str_len >= suffix_len) && (0 == strcmp(str + (str_len - suffix_len), suffix));
}

TCOD_console_t TCOD_console_from_file(const char* filename) {
  float version;
  int width, height;
  TCOD_console_t con;
  FILE* f;
  TCOD_IFNOT(filename != NULL) { return NULL; }
#ifndef TCOD_NO_ZLIB
  if (string_ends_with(filename, ".xp")) {
    return TCOD_console_from_xp(filename);
  }
#endif  // TCOD_NO_ZLIB
  f = fopen(filename, "rb");
  TCOD_IFNOT(f != NULL) { return NULL; }
  if (fscanf(f, "ASCII-Paint v%g", &version) != 1) {
    fclose(f);
    return NULL;
  }
  if (fscanf(f, "%i %i", &width, &height) != 2) {
    fclose(f);
    return NULL;
  }
  TCOD_IFNOT(width > 0 && height > 0) {
    fclose(f);
    return NULL;
  }
  con = TCOD_console_new(width, height);
  if (string_ends_with(filename, ".asc")) {
    TCOD_console_read_asc(con, f, width, height, version);
  }
  return con;
}

bool TCOD_console_load_asc(TCOD_Console* con, const char* filename) {
  float version;
  int width, height;
  FILE* f;
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con != NULL) return false;
  TCOD_IFNOT(filename != NULL) { return false; }
  f = fopen(filename, "rb");
  TCOD_IFNOT(f != NULL) { return false; }
  if (fscanf(f, "ASCII-Paint v%g", &version) != 1) {
    fclose(f);
    return false;
  }
  if (fscanf(f, "%i %i", &width, &height) != 2) {
    fclose(f);
    return false;
  }
  TCOD_IFNOT(width > 0 && height > 0) {
    fclose(f);
    return false;
  }
  TCOD_console_resize_(con, width, height);
  TCOD_console_read_asc(con, f, width, height, version);
  return true;
}

bool TCOD_console_save_asc(TCOD_Console* con, const char* filename) {
  static float version = 0.3f;
  FILE* f;
  int x, y;
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con != NULL) return false;
  TCOD_IFNOT(filename != NULL) { return false; }
  TCOD_IFNOT(con->w > 0 && con->h > 0) return false;
  f = fopen(filename, "wb");
  TCOD_IFNOT(f != NULL) return false;
  fprintf(f, "ASCII-Paint v%g\n", (double)version);
  fprintf(f, "%i %i\n", con->w, con->h);
  fputc('#', f);
  for (x = 0; x < con->w; x++) {
    for (y = 0; y < con->h; y++) {
      TCOD_color_t fore, back;
      int c = TCOD_console_get_char(con, x, y);
      fore = TCOD_console_get_char_foreground(con, x, y);
      back = TCOD_console_get_char_background(con, x, y);
      fputc(c, f);
      fputc(fore.r, f);
      fputc(fore.g, f);
      fputc(fore.b, f);
      fputc(back.r, f);
      fputc(back.g, f);
      fputc(back.b, f);
      fputc(0, f); /* solid */
      fputc(1, f); /* walkable */
    }
  }
  fclose(f);
  return true;
}

static bool hasDetectedBigEndianness = false;
static bool isBigEndian;
void detectBigEndianness(void) {
  if (!hasDetectedBigEndianness) {
    uint32_t Value32;
    uint8_t* VPtr = (uint8_t*)&Value32;
    VPtr[0] = VPtr[1] = VPtr[2] = 0;
    VPtr[3] = 1;
    if (Value32 == 1)
      isBigEndian = true;
    else
      isBigEndian = false;
    hasDetectedBigEndianness = true;
  }
}
#ifndef bswap16
static uint16_t bswap16(uint16_t s) {
  uint8_t* ps = (uint8_t*)&s;
  uint16_t res;
  uint8_t* pres = (uint8_t*)&res;
  pres[0] = ps[1];
  pres[1] = ps[0];
  return res;
}
#endif
#ifndef bswap32
static uint32_t bswap32(uint32_t s) {
  uint8_t* ps = (uint8_t*)&s;
  uint32_t res;
  uint8_t* pres = (uint8_t*)&res;
  pres[0] = ps[3];
  pres[1] = ps[2];
  pres[2] = ps[1];
  pres[3] = ps[0];
  return res;
}
#endif
uint16_t l16(uint16_t s) {
  if (isBigEndian)
    return bswap16(s);
  else
    return s;
}

uint32_t l32(uint32_t s) {
  if (isBigEndian)
    return bswap32(s);
  else
    return s;
}

/* fix the endianness */
void fix16(uint16_t* u) { *u = l16(*u); }

void fix32(uint32_t* u) { *u = l32(*u); }

/************ RIFF helpers  */

uint32_t fourCC(const char* c) { return (*(const uint32_t*)c); }

/* checks if u equals str */
bool fourCCequals(uint32_t u, const char* str) { return fourCC(str) == u; }

void fromFourCC(uint32_t u, char* s) {
  const char* c = (const char*)&u;
  s[0] = c[0];
  s[1] = c[1];
  s[2] = c[2];
  s[3] = c[3];
  s[4] = 0;
}

void put8(uint8_t d, FILE* fp) { fwrite(&d, 1, 1, fp); }

void put16(uint16_t d, FILE* fp) { fwrite(&d, 2, 1, fp); }

void put32(uint32_t d, FILE* fp) { fwrite(&d, 4, 1, fp); }

void putFourCC(const char* c, FILE* fp) { put32(fourCC(c), fp); }

void putData(void* what, int length, FILE* fp) { fwrite(what, length, 1, fp); }

bool get8(uint8_t* u, FILE* fp) { return 1 == fread(u, sizeof(uint8_t), 1, fp); }

bool get16(uint16_t* u, FILE* fp) { return 1 == fread(u, sizeof(uint16_t), 1, fp); }

bool get32(uint32_t* u, FILE* fp) { return 1 == fread(u, sizeof(uint32_t), 1, fp); }

bool getData(void* u, size_t sz, FILE* fp) { return 1 == fread(u, sz, 1, fp); }

/********* APF RIFF structures */

typedef struct {
  uint32_t show_grid;
  uint32_t grid_width;
  uint32_t grid_height;
} SettingsDataV1;

#define FILTER_TYPE_UNCOMPRESSED 0
#define FORMAT_TYPE_CRGBRGB 0

typedef struct {
  uint32_t width;
  uint32_t height;
  uint32_t filter;
  uint32_t format;
} ImageDetailsV1;

/* Layers */

typedef struct {
  uint32_t name;
  uint32_t mode;
  uint32_t index;
  uint32_t dataSize;
} LayerV1;

typedef struct {
  uint32_t name;
  uint32_t mode;
  uint32_t fgalpha;
  uint32_t bgalpha;
  uint32_t visible;
  uint32_t index;
  uint32_t dataSize;
} LayerV2;

/* fix the endianness */
void fixSettings(SettingsDataV1* s) {
  fix32(&s->show_grid);
  fix32(&s->grid_width);
  fix32(&s->grid_height);
}

void fixImage(ImageDetailsV1* v) {
  fix32(&v->width);
  fix32(&v->height);
  fix32(&v->filter);
  fix32(&v->format);
}

void fixLayer_v1(LayerV1* l) {
  fix32(&l->mode);
  fix32(&l->index);
  fix32(&l->dataSize);
}

void fixLayer_v2(LayerV2* l) {
  fix32(&l->mode);
  fix32(&l->fgalpha);
  fix32(&l->bgalpha);
  fix32(&l->visible);
  fix32(&l->index);
  fix32(&l->dataSize);
}

/*********** ApfFile */

bool TCOD_console_save_apf(TCOD_console_t con, const char* filename) {
  con = TCOD_console_validate_(con);
  FILE* fp;
  TCOD_IFNOT(con != NULL) return false;
  detectBigEndianness();

  fp = fopen(filename, "wb");
  if (fp == NULL) {
    return false;
  } else {
    int x, y;
    uint32_t riffSize = 0;
    uint32_t imgDetailsSize;
    SettingsDataV1 settingsData;
    ImageDetailsV1 imgData;
    fpos_t posRiffSize;
    uint32_t settingsSz;
    uint32_t layerImageSize;
    uint32_t layerChunkSize;
    /*  riff header*/
    putFourCC("RIFF", fp);
    fgetpos(fp, &posRiffSize);
    put32(0, fp);

    /* APF_ header */
    putFourCC("apf ", fp);
    riffSize += 4;

    /* settings */
    settingsData.show_grid = 0;
    settingsData.grid_width = 8;
    settingsData.grid_height = 8;
    settingsSz = sizeof(uint32_t) + sizeof settingsData;
    putFourCC("sett", fp);
    put32(l32(settingsSz), fp);
    put32(l32(1), fp);
    putData(&settingsData, sizeof settingsData, fp);
    if (settingsSz & 1) {
      put8(0, fp);
      riffSize++;
    }
    riffSize += 4 + 4 + settingsSz;

    /* image details */
    imgData.width = con->w;
    imgData.height = con->h;
    imgData.filter = 0;
    imgData.format = 0;
    imgDetailsSize = sizeof(uint32_t) + sizeof imgData;
    putFourCC("imgd", fp);
    put32(l32(imgDetailsSize), fp);
    put32(l32(1), fp);
    putData(&imgData, sizeof imgData, fp);
    if (imgDetailsSize & 1) {
      put8(0, fp);
      riffSize++;
    }
    riffSize += 4 + 4 + imgDetailsSize;

    /* now write the layers as a RIFF list
       the first layer is the lowest layer
       Assume imgData filter = uncompressed, and imgData format = CRGB */
    layerImageSize = imgData.width * imgData.height * 7;
    layerChunkSize = sizeof(uint32_t) /* version */
                     + sizeof(LayerV2) /* header */
                     + layerImageSize; /* data */

    putFourCC("layr", fp); /* layer */
    put32(l32(layerChunkSize), fp);
    /* VERSION -> */
    put32(l32(2), fp);
    /* Data */
    putFourCC("LAY0", fp);
    put32(l32(0), fp);
    put32(l32(255), fp);
    put32(l32(255), fp);
    put32(l32(1), fp);
    put32(l32(0), fp);
    put32(l32(layerImageSize), fp);

    /* now write out the data */

    for (x = 0; x < con->w; x++) {
      for (y = 0; y < con->h; y++) {
        TCOD_color_t fore, back;
        int c = TCOD_console_get_char(con, x, y);
        fore = TCOD_console_get_char_foreground(con, x, y);
        back = TCOD_console_get_char_background(con, x, y);
        put8(c, fp);
        put8(fore.r, fp);
        put8(fore.g, fp);
        put8(fore.b, fp);
        put8(back.r, fp);
        put8(back.g, fp);
        put8(back.b, fp);
      }
    }

    if (layerChunkSize & 1) {
      put8(0, fp); /* padding bit */
      riffSize++;
    }

    riffSize += 2 * sizeof(uint32_t) + layerChunkSize;

    fsetpos(fp, &posRiffSize);
    put32(l32(riffSize), fp);
  }

  fclose(fp);
  return true;
}

typedef struct LayerData {
  LayerV1 header_v1;
  LayerV2 header_v2;
  uint8_t* data; /* dynamically allocated */
} LayerData;

typedef struct Data {
  ImageDetailsV1 details;
  SettingsDataV1 settings;
  LayerData layer;
} Data;

bool TCOD_console_load_apf(TCOD_Console* con, const char* filename) {
  uint32_t sett = fourCC("sett");
  uint32_t imgd = fourCC("imgd");
  /*
  uint32_t LIST = fourCC("LIST");
  uint32_t LAYR = fourCC("LAYR");
  */
  uint32_t layr = fourCC("layr");
  FILE* fp;
  Data data;
  con = TCOD_console_validate_(con);
  TCOD_IFNOT(con != NULL) return false;

  detectBigEndianness();
  data.details.width = 1;
  data.details.height = 1;
  data.details.filter = 0;
  data.details.format = 0;

  data.settings.show_grid = true;
  data.settings.grid_width = 10;
  data.settings.grid_height = 10;

#define ERR(x)                                     \
  {                                                \
    printf("Error: %s\n. Aborting operation.", x); \
    return false;                                  \
  }
#define ERR_NEWER(x)                                                                                                  \
  {                                                                                                                   \
    printf(                                                                                                           \
        "Error: It looks like this file was made with a newer version of Ascii-Paint\n. In particular the %s field. " \
        "Aborting operation.",                                                                                        \
        x);                                                                                                           \
    return false;                                                                                                     \
  }

  fp = fopen(filename, "rb");
  if (fp == NULL) {
    printf("The file %s could not be loaded.\n", filename);
    return false;
  } else {
    /* read the header */
    uint32_t riff;
    uint32_t riffSize;
    int index = 0;
    int x, y;
    uint8_t* imgData;
    bool keepGoing = true;
    if (!get32(&riff, fp) || !fourCCequals(riff, "RIFF")) {
      ERR("File doesn't have a RIFF header");
    }
    if (!get32(&riffSize, fp)) ERR("No RIFF size field!");
    fix32(&riffSize);

    while (keepGoing && fp) { /* for each subfield, try to find the APF_ field */
      uint32_t apf;
      if (!get32(&apf, fp)) break;
      if (fourCCequals(apf, "apf ") || fourCCequals(apf, "APF ")) {
        /* Process APF segment */
        while (keepGoing && fp) {
          uint32_t seg;
          if (!get32(&seg, fp)) {
            keepGoing = false;
            break;
          } else {
            if (seg == sett) {
              /* size */
              uint32_t sz;
              uint32_t ver;
              SettingsDataV1 settingsData;

              get32(&sz, fp);
              fix32(&sz);
              /* version */
              get32(&ver, fp);
              fix32(&ver);
              if (ver != 1) ERR_NEWER("settings");
              /* ver must be 1 */
              if (!getData(&settingsData, sizeof settingsData, fp)) ERR("Can't read settings.");
              data.settings = settingsData;
              fixSettings(&data.settings);

            } else if (seg == imgd) {
              /* sz */
              uint32_t sz;
              uint32_t ver;
              ImageDetailsV1 dets;

              get32(&sz, fp);
              fix32(&sz);
              /* version */
              get32(&ver, fp);
              fix32(&ver);
              if (ver != 1) ERR_NEWER("image details");
              /* ver must be 1 */
              if (!getData(&dets, sizeof dets, fp)) ERR("Can't read image details.");
              data.details = dets;
              fixImage(&data.details);

              /* get canvas ready */
              TCOD_IFNOT(data.details.width > 0 && data.details.height > 0) {
                fclose(fp);
                return false;
              }
              TCOD_console_resize_(con, data.details.width, data.details.height);
            } else if (seg == layr) {
              uint32_t sz;
              uint32_t ver;

              get32(&sz, fp);
              fix32(&sz);
              /* version */
              get32(&ver, fp);
              fix32(&ver);
              if (ver > 2) ERR_NEWER("layer spec");

              if (ver == 1) {
                if (!getData(&data.layer.header_v1, sizeof(LayerV1), fp)) ERR("Can't read layer header.");
                fixLayer_v1(&data.layer.header_v1);

                /* Read in the data chunk*/
                data.layer.data = (uint8_t*)(malloc(sizeof(uint8_t) * data.layer.header_v1.dataSize));
                getData(data.layer.data, data.layer.header_v1.dataSize, fp);
              } else if (ver == 2) {
                if (!getData(&data.layer.header_v2, sizeof(LayerV2), fp)) ERR("Can't read layer header.");
                fixLayer_v2(&data.layer.header_v2);

                /* Read in the data chunk */
                data.layer.data = (uint8_t*)(malloc(sizeof(uint8_t) * data.layer.header_v2.dataSize));
                getData(data.layer.data, data.layer.header_v2.dataSize, fp);
              }
            } else {
              /* skip unknown segment */
              uint32_t sz;
              get32(&sz, fp);
              fix32(&sz);
              fseek(fp, sz, SEEK_CUR);
            }
          }
        }

        /* we're done! */
        keepGoing = false;
      } else {
        /* skip this segment */
        uint32_t sz;
        get32(&sz, fp);
        fseek(fp, sz, SEEK_CUR);
      }
    }

    imgData = data.layer.data;
    for (x = 0; x < con->w; x++) {
      for (y = 0; y < con->h; y++) {
        TCOD_color_t fore, back;
        int c = imgData[index++];
        fore.r = imgData[index++];
        fore.g = imgData[index++];
        fore.b = imgData[index++];
        back.r = imgData[index++];
        back.g = imgData[index++];
        back.b = imgData[index++];
        TCOD_console_put_char_ex(con, x, y, c, fore, back);
      }
    }

    free(data.layer.data);
  }
  fclose(fp);

  return true;
}
/*

bool ApfFile::Load(std::string filename){
        detectBigEndianness();

        uint32_t sett = fourCC("sett");
        uint32_t imgd = fourCC("imgd");
        uint32_t LIST = fourCC("LIST");
        uint32_t LAYR = fourCC("LAYR");
        uint32_t layr = fourCC("layr");

        Data data; // File data

        data.details.width = 1;
        data.details.height = 1;
        data.details.filter = FILTER_TYPE_UNCOMPRESSED;
        data.details.format = FORMAT_TYPE_CRGBRGB;

        data.settings.show_grid = true;
        data.settings.grid_width = 10;
        data.settings.grid_height = 10;

        data.currentLayer = NULL;

        #define ERR(x) {printf("Error: %s\n. Aborting operation.",x); return false;}
        #define ERR_NEWER(x) {printf("Error: It looks like this file was made with a newer version of Ascii-Paint\n. In
particular the %s field. Aborting operation.",x); return false;}

        FILE* fp = fopen(filename.c_str(), "rb");
        if(fp == NULL) {
                printf("The file %s could not be loaded.\n", filename.c_str());
                return false;
        }
        else {
                // read the header
                uint32_t riff;
                if (not get32(&riff,fp)
                        or
                        not fourCCequals(riff,"RIFF")){
                        ERR("File doesn't have a RIFF header");
                }
                // else
                uint32_t riffSize;
                if (!get32(&riffSize,fp)) ERR("No RIFF size field!");
                fix(&riffSize);

                bool keepGoing = true;
                while(keepGoing and fp){ // for each subfield, try to find the APF_ field
                        uint32_t apf;
                        if (not get32(&apf,fp)) break;
                        if (fourCCequals(apf,"apf ") or fourCCequals(apf,"APF ")){
                                // Process APF segment
                                while(keepGoing and fp){
                                        uint32_t seg;
                                        if (not get32(&seg,fp)){
                                                keepGoing = false;
                                                break;
                                        }
                                        else {
                                                if (seg==sett){
                                                        // size
                                                        uint32_t sz;
                                                        get32(&sz,fp);
                                                        fix(&sz);
                                                        // version
                                                        uint32_t ver;
                                                        get32(&ver,fp);
                                                        fix(&ver);
                                                        if (ver!=1) ERR_NEWER("settings");
                                                        // ver must be 1
                                                        SettingsDataV1 settingsData;
                                                        if (not getData((void*)&settingsData,sizeof settingsData,fp))
ERR("Can't read settings."); data.settings = settingsData; fix(&data.settings);

                                                        // Change app settings
                                                        app->setGridDimensions(data.settings.grid_width,data.settings.grid_height);
                                                        app->setShowGrid(data.settings.show_grid==1);
                                                }
                                                else if (seg==imgd){
                                                        // sz
                                                        uint32_t sz;
                                                        get32(&sz,fp);
                                                        fix(&sz);
                                                        // version
                                                        uint32_t ver;
                                                        get32(&ver,fp);
                                                        fix(&ver);
                                                        if (ver!=1) ERR_NEWER("image details");
                                                        // ver must be 1
                                                        ImageDetailsV1 dets;
                                                        if (not getData((void*)&dets, sizeof dets, fp)) ERR("Can't read
image details."); data.details = dets; fix(&data.details);

                                                        // get canvas ready
                                                        app->canvasWidth = data.details.width;
                                                        app->canvasHeight = data.details.height;
                                                        app->initCanvas();

                                                        // delete new layer
                                                        app->deleteLayer(app->getCurrentLayer()->name);

                                                }
                                                else if (seg==layr){
                                                        // printf("Found a layer\n");

                                                        // sz
                                                        uint32_t sz;
                                                        get32(&sz,fp);
                                                        fix(&sz);
                                                        // version
                                                        uint32_t ver;
                                                        get32(&ver,fp);
                                                        fix(&ver);
                                                        if (ver>2) ERR_NEWER("layer spec");

                                                        if (ver==1){
                                                                LayerV1 layerHeader;
                                                                if (not getData((void*)&layerHeader, sizeof layerHeader,
fp)) ERR("Can't read layer header."); fix(&layerHeader);

                                                                // creat new layer data
                                                                LayerData* ld = new LayerData;
                                                                ld->header = layerHeader; // already fixed
                                                                ld->data = new uint8[ld->header.dataSize];

                                                                // Read in the data chunk
                                                                getData((void*) ld->data, ld->header.dataSize, fp);

                                                                // push layer onto the list
                                                                data.currentLayer = ld;
                                                                data.layers.push(ld);
                                                        }
                                                        else if (ver==2){
                                                                LayerV2 layerHeader;
                                                                if (not getData((void*)&layerHeader, sizeof layerHeader,
fp)) ERR("Can't read layer header."); fix(&layerHeader);

                                                                // creat new layer data
                                                                LayerData* ld = new LayerData;
                                                                ld->header = layerHeader; // already fixed
                                                                ld->data = new uint8[ld->header.dataSize];

                                                                // Read in the data chunk
                                                                getData((void*) ld->data, ld->header.dataSize, fp);

                                                                // push layer onto the list
                                                                data.currentLayer = ld;
                                                                data.layers.push(ld);
                                                        }
                                                }
                                                else {
                                                        // skip unknown segment
                                                        uint32_t sz;
                                                        get32(&sz,fp);
                                                        fix(&sz);
                                                        fseek(fp,sz,SEEK_CUR);
                                                }
                                        }
                                }

                                // we're done!
                                keepGoing = false;
                        }
                        else {
                                // skip this segment
                                uint32_t sz;
                                get32(&sz,fp);
                                fseek(fp,sz,SEEK_CUR);
                        }
                }

                // finally, copy the layers into the current document
                for(int i=0;i<data.layers.size();i++){
                        // for now, just load the first layer
                        LayerData* ld = data.layers.get(i);

                        // canvas width/height have already been set...
                        Layer* l = app->addNewLayer();

                        // Parse layer header
                        l->name = fromFourCC(ld->header.name);
                        l->fgalpha = ld->header.fgalpha;
                        l->bgalpha = ld->header.bgalpha;
                        l->visible = (ld->header.visible==1);
                        // l->compositingMode =

                        // Copy data into currently selected canvas
                        uint8_t* imgData = ld->data;
                        CanvasImage *img = new CanvasImage;
                        // Write the brush data for every brush in the image
                        int index = 0;
                        for(int x = 0; x < app->canvasWidth; x++) {
                                for(int y = 0; y < app->canvasHeight; y++) {
                                        Brush b;
                                        b.symbol = (unsigned char)(imgData[index++]);
                                        b.fore.r = (uint8_t)(imgData[index++]);
                                        b.fore.g = (uint8_t)(imgData[index++]);
                                        b.fore.b = (uint8_t)(imgData[index++]);
                                        b.back.r = (uint8_t)(imgData[index++]);
                                        b.back.g = (uint8_t)(imgData[index++]);
                                        b.back.b = (uint8_t)(imgData[index++]);
                                        b.solid = true; // deprecated
                                        b.walkable = true; // deprecated
                                        img->push_back(b);
                                }
                        }

                        app->setCanvasImage(*img);
                        delete img;
                }

                // then free all the temporary layer data
                for(int i=0;i<data.layers.size();i++){
                        delete[]data.layers.get(i)->data;
                        delete data.layers.get(i);
                }

                // and update the layer widget
                app->gui->layerWidget->regenerateLayerList();
        }
        fclose(fp);

        return true;
}
*/
