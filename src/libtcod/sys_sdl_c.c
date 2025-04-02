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
#ifndef NO_SDL
#include <SDL3/SDL.h>
#endif  // NO_SDL
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "console.h"
#include "console_etc.h"
#include "console_init.h"
#include "error.h"
#include "globals.h"
#include "libtcod_int.h"
#include "logging.h"
#include "mouse.h"
#include "parser.h"
#include "sys.h"
#include "tileset.h"

#ifndef NO_SDL

/* to enable bitmap locking. Is there any use ?? makes the OSX port renderer to fail */
/*#define USE_SDL_LOCKS */

/* image support stuff */
bool TCOD_sys_check_bmp(const char* filename);
SDL_Surface* TCOD_sys_read_bmp(const char* filename);
TCOD_Error TCOD_sys_write_bmp(SDL_Surface* surf, const char* filename);
#ifndef TCOD_NO_PNG
bool TCOD_sys_check_png(const char* filename);
SDL_Surface* TCOD_sys_read_png(const char* filename);
TCOD_Error TCOD_sys_write_png(SDL_Surface* surf, const char* filename);
#endif  // TCOD_NO_PNG

typedef struct {
  const char* extension;
  bool (*check_type)(const char* filename);
  SDL_Surface* (*read)(const char* filename);
  TCOD_Error (*write)(SDL_Surface* surf, const char* filename);
} image_support_t;

static image_support_t image_type[] = {
    {"BMP", TCOD_sys_check_bmp, TCOD_sys_read_bmp, TCOD_sys_write_bmp},
#ifndef TCOD_NO_PNG
    {"PNG", TCOD_sys_check_png, TCOD_sys_read_png, TCOD_sys_write_png},
#endif  // TCOD_NO_PNG
    {NULL, NULL, NULL, NULL},
};

scale_data_t scale_data = {0};
float scale_factor = 1.0f;
#define MAX_SCALE_FACTOR 5.0f

/* mouse stuff */
#ifdef TCOD_TOUCH_INPUT
static bool mouse_touch = true;
#endif

/* minimum length for a frame (when fps are limited) */
static int min_frame_length = 0;
static int min_frame_length_backup = 0;
/* number of frames in the last second */
static int fps = 0;
/* current number of frames */
static int cur_fps = 0;
/* length of the last rendering loop, (in seconds) */
static float last_frame_length = 0.0f;

int oldFade = -1;

/* convert SDL vk to a char (depends on the keyboard layout) */
typedef struct {
  SDL_Keycode sdl_key;
  char tcod_key;
} vk_to_c_entry;
#define NUM_VK_TO_C_ENTRIES 10
static vk_to_c_entry vk_to_c[NUM_VK_TO_C_ENTRIES];
void TCOD_sys_register_SDL_renderer(SDL_renderer_t renderer) { TCOD_ctx.sdl_cbk = renderer; }

void TCOD_sys_update_char(int asciiCode, int font_x, int font_y, const TCOD_Image* img, int x, int y) {
  static const TCOD_color_t pink = {255, 0, 255};
  int img_width;
  int img_height;
  if (!img) {
    return;
  }
  TCOD_image_get_size(img, &img_width, &img_height);
  if (!TCOD_ctx.tileset) {
    return;
  }
  int tile_id = font_y * TCOD_ctx.tileset->virtual_columns + font_x;
  if (TCOD_tileset_reserve(TCOD_ctx.tileset, tile_id + 1) < 0) {
    return;
  }
  struct TCOD_ColorRGBA* tile_out = TCOD_ctx.tileset->pixels + tile_id * TCOD_ctx.tileset->tile_length;
  for (int px = 0; px < TCOD_ctx.tileset->tile_width; ++px) {
    for (int py = 0; py < TCOD_ctx.tileset->tile_height; ++py) {
      TCOD_color_t col = TCOD_image_get_pixel(img, x + px, y + py);
      struct TCOD_ColorRGBA* out = tile_out + py * TCOD_ctx.tileset->tile_width + px;
      if (TCOD_color_equals(col, pink)) {
        *out = (TCOD_ColorRGBA){255, 255, 255, 0};
        continue;
      }
      *out = (TCOD_ColorRGBA){col.r, col.g, col.b, 255};
    }
  }
  TCOD_tileset_assign_tile(TCOD_ctx.tileset, tile_id, asciiCode);
  TCOD_tileset_notify_tile_changed(TCOD_ctx.tileset, tile_id);
}

TCOD_Error TCOD_sys_load_player_config(void) {
  const char* font;
  int fullscreenWidth, fullscreenHeight;
  if (!TCOD_sys_file_exists("./libtcod.cfg")) {
    return TCOD_E_OK;
  }
  /* define file structure */
  TCOD_parser_t parser = TCOD_parser_new();
  TCOD_parser_struct_t libtcod = TCOD_parser_new_struct(parser, "libtcod");
  TCOD_struct_add_property(libtcod, "renderer", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(libtcod, "font", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(libtcod, "fontInRow", TCOD_TYPE_BOOL, false);
  TCOD_struct_add_property(libtcod, "fontGreyscale", TCOD_TYPE_BOOL, false);
  TCOD_struct_add_property(libtcod, "fontTcodLayout", TCOD_TYPE_BOOL, false);
  TCOD_struct_add_property(libtcod, "fontNbCharHoriz", TCOD_TYPE_INT, false);
  TCOD_struct_add_property(libtcod, "fontNbCharVertic", TCOD_TYPE_INT, false);
  TCOD_struct_add_property(libtcod, "fullscreen", TCOD_TYPE_BOOL, false);
  TCOD_struct_add_property(libtcod, "fullscreenWidth", TCOD_TYPE_INT, false);
  TCOD_struct_add_property(libtcod, "fullscreenHeight", TCOD_TYPE_INT, false);
  TCOD_struct_add_property(libtcod, "fullscreenScaling", TCOD_TYPE_BOOL, false);

  /* parse file */
  TCOD_parser_run(parser, "./libtcod.cfg", NULL);

  // Set user preferences.
  // Custom fullscreen resolution.
  TCOD_ctx.fullscreen = TCOD_parser_get_bool_property(parser, "libtcod.fullscreen");
  fullscreenWidth = TCOD_parser_get_int_property(parser, "libtcod.fullscreenWidth");
  fullscreenHeight = TCOD_parser_get_int_property(parser, "libtcod.fullscreenHeight");
  if (fullscreenWidth > 0) {
    TCOD_ctx.fullscreen_width = fullscreenWidth;
  }
  if (fullscreenHeight > 0) {
    TCOD_ctx.fullscreen_height = fullscreenHeight;
  }
  // Custom Font.
  font = TCOD_parser_get_string_property(parser, "libtcod.font");
  if (font != NULL) {
    /* custom font */
    if (TCOD_sys_file_exists(font)) {
      int fontNbCharHoriz, fontNbCharVertic;
      strncpy(TCOD_ctx.font_file, font, sizeof(TCOD_ctx.font_file) - 1);
      TCOD_ctx.font_in_row = TCOD_parser_get_bool_property(parser, "libtcod.fontInRow");
      TCOD_ctx.font_greyscale = TCOD_parser_get_bool_property(parser, "libtcod.fontGreyscale");
      TCOD_ctx.font_tcod_layout = TCOD_parser_get_bool_property(parser, "libtcod.fontTcodLayout");
      fontNbCharHoriz = TCOD_parser_get_int_property(parser, "libtcod.fontNbCharHoriz");
      fontNbCharVertic = TCOD_parser_get_int_property(parser, "libtcod.fontNbCharVertic");
      if (fontNbCharHoriz > 0) {
        TCOD_ctx.fontNbCharHoriz = fontNbCharHoriz;
      }
      if (fontNbCharVertic > 0) {
        TCOD_ctx.fontNbCharVertic = fontNbCharVertic;
      }
      TCOD_Error err = TCOD_console_set_custom_font(
          font,
          (TCOD_ctx.font_in_row ? TCOD_FONT_LAYOUT_ASCII_INROW : TCOD_FONT_LAYOUT_ASCII_INCOL) |
              (TCOD_ctx.font_greyscale ? TCOD_FONT_TYPE_GREYSCALE : 0) |
              (TCOD_ctx.font_tcod_layout ? TCOD_FONT_LAYOUT_TCOD : 0),
          fontNbCharHoriz,
          fontNbCharVertic);
      if (err < 0) {
        return err;
      }
    } else {
      printf("Warning : font file '%s' does not exist\n", font);
    }
  }
  TCOD_log_warning("The use of libtcod.cfg is deprecated.");
  TCOD_set_error("The use of libtcod.cfg is deprecated.");
  return TCOD_E_WARN;
}

TCOD_renderer_t TCOD_sys_get_renderer(void) {
  if (!TCOD_ctx.engine) {
    return TCOD_RENDERER_SDL2;
  }
  return (TCOD_renderer_t)TCOD_context_get_renderer_type(TCOD_ctx.engine);
}

int TCOD_sys_set_renderer(TCOD_renderer_t renderer) {
  if (TCOD_ctx.engine && renderer == TCOD_sys_get_renderer()) {
    return 0;
  }
  return TCOD_console_init_root(
      TCOD_ctx.root->w, TCOD_ctx.root->h, TCOD_ctx.window_title, TCOD_console_is_fullscreen(), renderer);
}
TCOD_DEPRECATED_NOMESSAGE
static char* TCOD_strcasestr(const char* haystack, const char* needle) {
  const char *start_n = NULL, *np = NULL;
  for (const char* p = haystack; *p; p++) {
    if (np) {
      if (toupper(*p) == toupper(*np)) {
        if (!*++np) {
          return (char*)start_n;
        }  // Const cast.
      } else {
        np = 0;
      }
    } else if (toupper(*p) == toupper(*needle)) {
      np = needle + 1;
      start_n = p;
    }
  }
  return 0;
}

TCOD_Error TCOD_sys_save_bitmap(SDL_Surface* bitmap, const char* filename) {
  const image_support_t* img = image_type;
  while (img->extension != NULL && TCOD_strcasestr(filename, img->extension) == NULL) {
    ++img;
  }
  if (img->extension == NULL || img->write == NULL) {
    img = image_type;  // default to bmp
  }
  return img->write(bitmap, filename);
}

void TCOD_sys_save_screenshot(const char* filename) { TCOD_context_save_screenshot(TCOD_ctx.engine, filename); }

void TCOD_sys_set_fullscreen(bool fullscreen) {
  TCOD_ctx.fullscreen = fullscreen;
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  if (!window) {
    return;
  }
  SDL_SetWindowFullscreen(window, fullscreen);
}

void TCOD_sys_set_scale_factor(float value) {
  scale_factor = value;
  if (scale_factor + 1e-3f < scale_data.min_scale_factor) {
    scale_factor = scale_data.min_scale_factor;
  } else if (scale_factor - 1e-3f > MAX_SCALE_FACTOR) {
    scale_factor = MAX_SCALE_FACTOR;
  }
}

void TCOD_sys_set_window_title(const char* title) {
  strncpy(TCOD_ctx.window_title, title, sizeof(TCOD_ctx.window_title) - 1);
  TCOD_ctx.window_title[sizeof(TCOD_ctx.window_title) - 1] = '\0';
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  if (window) {
    SDL_SetWindowTitle(window, title);
  }
}
/**
 *  Keep track of time and wait if the frame-rate is faster than the set FPS.
 */
void sync_time_(void) {
  static uint32_t old_time = 0;
  static uint32_t new_time = 0;
  static uint32_t elapsed = 0;
  int32_t frame_time;
  int32_t time_to_wait;

  old_time = new_time;
  new_time = TCOD_sys_elapsed_milli();
  /* If TCOD has been terminated and restarted. */
  if (old_time > new_time) old_time = elapsed = 0;
  if (new_time / 1000 != elapsed) {
    /* update fps every second */
    fps = cur_fps;
    cur_fps = 0;
    elapsed = new_time / 1000;
  }
  /* if too fast, wait */
  frame_time = (new_time - old_time);
  last_frame_length = frame_time * 0.001f;
  cur_fps++;
  time_to_wait = min_frame_length - frame_time;
  if (old_time > 0 && time_to_wait > 0) {
    TCOD_sys_sleep_milli(time_to_wait);
    new_time = TCOD_sys_elapsed_milli();
    frame_time = (new_time - old_time);
  }
  last_frame_length = frame_time * 0.001f;
}

static char TCOD_sys_get_vk(SDL_Keycode sdl_key) {
  int i;
  for (i = 0; i < NUM_VK_TO_C_ENTRIES; i++) {
    if (vk_to_c[i].sdl_key == sdl_key) {
      vk_to_c[i].sdl_key = 0;
      return vk_to_c[i].tcod_key;
    }
  }
  return 0;
}

static void TCOD_sys_set_vk(SDL_Keycode sdl_key, char tcod_key) {
  int i;
  for (i = 0; i < NUM_VK_TO_C_ENTRIES; i++) {
    if (vk_to_c[i].sdl_key == 0) {
      vk_to_c[i].sdl_key = sdl_key;
      vk_to_c[i].tcod_key = tcod_key;
      break;
    }
  }
}

static void TCOD_sys_convert_event(const SDL_Event* ev, TCOD_key_t* ret) {
  const SDL_KeyboardEvent* kev = &ev->key;
  /* SDL2 does not map keycodes and modifiers to characters, this is on the developer.
          Presumably in order to avoid problems with different keyboard layouts, they
          are expected to write their own key mapping editing code for the user.  */
  if (SDLK_SCANCODE_MASK == (kev->key & SDLK_SCANCODE_MASK)) {
    ret->c = 0;
  } else if (kev->key >= 256) {
    TCOD_set_errorvf("Old event API does not support key: %s", SDL_GetKeyName(kev->key));
    ret->c = 0;
  } else {
    ret->c = (char)kev->key;
  }
  if ((kev->mod & (SDL_KMOD_LCTRL | SDL_KMOD_RCTRL)) != 0) {
    /* when pressing CTRL-A, we don't get unicode for 'a', but unicode for CTRL-A = 1. Fix it */
    if (kev->key >= SDLK_A && kev->key <= SDLK_Z) {
      ret->c = (char)('a' + (kev->key - SDLK_A));
    }
  }
  if (ev->type == SDL_EVENT_KEY_DOWN)
    TCOD_sys_set_vk(kev->key, ret->c);
  else if (ev->type == SDL_EVENT_KEY_UP)
    ret->c = TCOD_sys_get_vk(kev->key);
  switch (kev->key) {
    case SDLK_ESCAPE:
      ret->vk = TCODK_ESCAPE;
      break;
    case SDLK_SPACE:
      ret->vk = TCODK_SPACE;
      break;
    case SDLK_BACKSPACE:
      ret->vk = TCODK_BACKSPACE;
      break;
    case SDLK_TAB:
      ret->vk = TCODK_TAB;
      break;
    case SDLK_RETURN:
      ret->vk = TCODK_ENTER;
      break;
    case SDLK_PAUSE:
      ret->vk = TCODK_PAUSE;
      break;
    case SDLK_PAGEUP:
      ret->vk = TCODK_PAGEUP;
      break;
    case SDLK_PAGEDOWN:
      ret->vk = TCODK_PAGEDOWN;
      break;
    case SDLK_HOME:
      ret->vk = TCODK_HOME;
      break;
    case SDLK_END:
      ret->vk = TCODK_END;
      break;
    case SDLK_DELETE:
      ret->vk = TCODK_DELETE;
      break;
    case SDLK_INSERT:
      ret->vk = TCODK_INSERT;
      break;
    case SDLK_LALT:
    case SDLK_RALT:
      ret->vk = TCODK_ALT;
      break;
    case SDLK_LCTRL:
    case SDLK_RCTRL:
      ret->vk = TCODK_CONTROL;
      break;
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
      ret->vk = TCODK_SHIFT;
      break;
    case SDLK_PRINTSCREEN:
      ret->vk = TCODK_PRINTSCREEN;
      break;
    case SDLK_LEFT:
      ret->vk = TCODK_LEFT;
      break;
    case SDLK_UP:
      ret->vk = TCODK_UP;
      break;
    case SDLK_RIGHT:
      ret->vk = TCODK_RIGHT;
      break;
    case SDLK_DOWN:
      ret->vk = TCODK_DOWN;
      break;
    case SDLK_F1:
      ret->vk = TCODK_F1;
      break;
    case SDLK_F2:
      ret->vk = TCODK_F2;
      break;
    case SDLK_F3:
      ret->vk = TCODK_F3;
      break;
    case SDLK_F4:
      ret->vk = TCODK_F4;
      break;
    case SDLK_F5:
      ret->vk = TCODK_F5;
      break;
    case SDLK_F6:
      ret->vk = TCODK_F6;
      break;
    case SDLK_F7:
      ret->vk = TCODK_F7;
      break;
    case SDLK_F8:
      ret->vk = TCODK_F8;
      break;
    case SDLK_F9:
      ret->vk = TCODK_F9;
      break;
    case SDLK_F10:
      ret->vk = TCODK_F10;
      break;
    case SDLK_F11:
      ret->vk = TCODK_F11;
      break;
    case SDLK_F12:
      ret->vk = TCODK_F12;
      break;
    case SDLK_0:
      ret->vk = TCODK_0;
      break;
    case SDLK_1:
      ret->vk = TCODK_1;
      break;
    case SDLK_2:
      ret->vk = TCODK_2;
      break;
    case SDLK_3:
      ret->vk = TCODK_3;
      break;
    case SDLK_4:
      ret->vk = TCODK_4;
      break;
    case SDLK_5:
      ret->vk = TCODK_5;
      break;
    case SDLK_6:
      ret->vk = TCODK_6;
      break;
    case SDLK_7:
      ret->vk = TCODK_7;
      break;
    case SDLK_8:
      ret->vk = TCODK_8;
      break;
    case SDLK_9:
      ret->vk = TCODK_9;
      break;
    case SDLK_RGUI:
      ret->vk = TCODK_RWIN;
      break;
    case SDLK_LGUI:
      ret->vk = TCODK_LWIN;
      break;
    case SDLK_NUMLOCKCLEAR:
      ret->vk = TCODK_NUMLOCK;
      break;
    case SDLK_KP_0:
      ret->vk = TCODK_KP0;
      break;
    case SDLK_KP_1:
      ret->vk = TCODK_KP1;
      break;
    case SDLK_KP_2:
      ret->vk = TCODK_KP2;
      break;
    case SDLK_KP_3:
      ret->vk = TCODK_KP3;
      break;
    case SDLK_KP_4:
      ret->vk = TCODK_KP4;
      break;
    case SDLK_KP_5:
      ret->vk = TCODK_KP5;
      break;
    case SDLK_KP_6:
      ret->vk = TCODK_KP6;
      break;
    case SDLK_KP_7:
      ret->vk = TCODK_KP7;
      break;
    case SDLK_KP_8:
      ret->vk = TCODK_KP8;
      break;
    case SDLK_KP_9:
      ret->vk = TCODK_KP9;
      break;
    case SDLK_KP_DIVIDE:
      ret->vk = TCODK_KPDIV;
      break;
    case SDLK_KP_MULTIPLY:
      ret->vk = TCODK_KPMUL;
      break;
    case SDLK_KP_PLUS:
      ret->vk = TCODK_KPADD;
      break;
    case SDLK_KP_MINUS:
      ret->vk = TCODK_KPSUB;
      break;
    case SDLK_KP_ENTER:
      ret->vk = TCODK_KPENTER;
      break;
    case SDLK_KP_PERIOD:
      ret->vk = TCODK_KPDEC;
      break;
    default:
      ret->vk = TCODK_CHAR;
      break;
  }
}
/**
 *  Parse an SDL key-up or key-down event and return the global key state.
 */
static TCOD_key_t TCOD_sys_SDLtoTCOD(const SDL_Event* ev, int flags) {
  TCOD_key_t* ret = &TCOD_ctx.key_state;
  ret->vk = TCODK_NONE;
  ret->c = 0;
  ret->pressed = 0;
  switch (ev->type) {
    case SDL_EVENT_KEY_UP: {
      TCOD_key_t tmp_key;
      switch (ev->key.key) {
        case SDLK_LALT:
          ret->lalt = 0;
          break;
        case SDLK_RALT:
          ret->ralt = 0;
          break;
        case SDLK_LCTRL:
          ret->lctrl = 0;
          break;
        case SDLK_RCTRL:
          ret->rctrl = 0;
          break;
        case SDLK_LSHIFT:
          ret->shift = 0;
          break;
        case SDLK_RSHIFT:
          ret->shift = 0;
          break;
        case SDLK_LGUI:
          ret->lmeta = 0;
          break;
        case SDLK_RGUI:
          ret->rmeta = 0;
          break;
        default:
          break;
      }
      TCOD_sys_convert_event(ev, &tmp_key);
      if (flags & TCOD_KEY_RELEASED) {
        ret->vk = tmp_key.vk;
        ret->c = tmp_key.c;
        ret->pressed = 0;
      }
      break;
    }
    case SDL_EVENT_KEY_DOWN: {
      TCOD_key_t tmp_key;
      switch (ev->key.key) {
        case SDLK_LALT:
          ret->lalt = 1;
          break;
        case SDLK_RALT:
          ret->ralt = 1;
          break;
        case SDLK_LCTRL:
          ret->lctrl = 1;
          break;
        case SDLK_RCTRL:
          ret->rctrl = 1;
          break;
        case SDLK_LSHIFT:
          ret->shift = 1;
          break;
        case SDLK_RSHIFT:
          ret->shift = 1;
          break;
        case SDLK_LGUI:
          ret->lmeta = 1;
          break;
        case SDLK_RGUI:
          ret->rmeta = 1;
          break;
        default:
          break;
      }
      TCOD_sys_convert_event(ev, &tmp_key);
      if (flags & TCOD_KEY_PRESSED) {
        ret->vk = tmp_key.vk;
        ret->c = tmp_key.c;
        ret->pressed = 1;
      }
      break;
    }
  }
  return *ret;
}

bool TCOD_sys_is_key_pressed(TCOD_keycode_t key) {
  const bool* state = SDL_GetKeyboardState(NULL);
  switch (key) {
    case TCODK_ESCAPE:
      return state[SDL_SCANCODE_ESCAPE] != 0;
    case TCODK_SPACE:
      return state[SDL_SCANCODE_SPACE] != 0;
    case TCODK_BACKSPACE:
      return state[SDL_SCANCODE_BACKSPACE] != 0;
    case TCODK_TAB:
      return state[SDL_SCANCODE_TAB] != 0;
    case TCODK_ENTER:
      return state[SDL_SCANCODE_RETURN] != 0;
    case TCODK_PAUSE:
      return state[SDL_SCANCODE_PAUSE] != 0;
    case TCODK_PAGEUP:
      return state[SDL_SCANCODE_PAGEUP] != 0;
    case TCODK_PAGEDOWN:
      return state[SDL_SCANCODE_PAGEDOWN] != 0;
    case TCODK_HOME:
      return state[SDL_SCANCODE_HOME] != 0;
    case TCODK_END:
      return state[SDL_SCANCODE_END] != 0;
    case TCODK_DELETE:
      return state[SDL_SCANCODE_DELETE] != 0;
    case TCODK_INSERT:
      return state[SDL_SCANCODE_INSERT] != 0;
    case TCODK_ALT:
      return (state[SDL_SCANCODE_LALT] | state[SDL_SCANCODE_RALT]) != 0;
    case TCODK_CONTROL:
      return (state[SDL_SCANCODE_LCTRL] | state[SDL_SCANCODE_RCTRL]) != 0;
    case TCODK_SHIFT:
      return (state[SDL_SCANCODE_LSHIFT] | state[SDL_SCANCODE_RSHIFT]) != 0;
    case TCODK_PRINTSCREEN:
      return state[SDL_SCANCODE_PRINTSCREEN] != 0;
    case TCODK_LEFT:
      return state[SDL_SCANCODE_LEFT] != 0;
    case TCODK_UP:
      return state[SDL_SCANCODE_UP] != 0;
    case TCODK_RIGHT:
      return state[SDL_SCANCODE_RIGHT] != 0;
    case TCODK_DOWN:
      return state[SDL_SCANCODE_DOWN] != 0;
    case TCODK_F1:
      return state[SDL_SCANCODE_F1] != 0;
    case TCODK_F2:
      return state[SDL_SCANCODE_F2] != 0;
    case TCODK_F3:
      return state[SDL_SCANCODE_F3] != 0;
    case TCODK_F4:
      return state[SDL_SCANCODE_F4] != 0;
    case TCODK_F5:
      return state[SDL_SCANCODE_F5] != 0;
    case TCODK_F6:
      return state[SDL_SCANCODE_F6] != 0;
    case TCODK_F7:
      return state[SDL_SCANCODE_F7] != 0;
    case TCODK_F8:
      return state[SDL_SCANCODE_F8] != 0;
    case TCODK_F9:
      return state[SDL_SCANCODE_F9] != 0;
    case TCODK_F10:
      return state[SDL_SCANCODE_F10] != 0;
    case TCODK_F11:
      return state[SDL_SCANCODE_F11] != 0;
    case TCODK_F12:
      return state[SDL_SCANCODE_F12] != 0;
    case TCODK_0:
      return state[SDL_SCANCODE_0] != 0;
    case TCODK_1:
      return state[SDL_SCANCODE_1] != 0;
    case TCODK_2:
      return state[SDL_SCANCODE_2] != 0;
    case TCODK_3:
      return state[SDL_SCANCODE_3] != 0;
    case TCODK_4:
      return state[SDL_SCANCODE_4] != 0;
    case TCODK_5:
      return state[SDL_SCANCODE_5] != 0;
    case TCODK_6:
      return state[SDL_SCANCODE_6] != 0;
    case TCODK_7:
      return state[SDL_SCANCODE_7] != 0;
    case TCODK_8:
      return state[SDL_SCANCODE_8] != 0;
    case TCODK_9:
      return state[SDL_SCANCODE_9] != 0;
    case TCODK_RWIN:
      return state[SDL_SCANCODE_RGUI] != 0;
    case TCODK_LWIN:
      return state[SDL_SCANCODE_LGUI] != 0;
    case TCODK_NUMLOCK:
      return state[SDL_SCANCODE_NUMLOCKCLEAR] != 0;
    case TCODK_KP0:
      return state[SDL_SCANCODE_KP_0] != 0;
    case TCODK_KP1:
      return state[SDL_SCANCODE_KP_1] != 0;
    case TCODK_KP2:
      return state[SDL_SCANCODE_KP_2] != 0;
    case TCODK_KP3:
      return state[SDL_SCANCODE_KP_3] != 0;
    case TCODK_KP4:
      return state[SDL_SCANCODE_KP_4] != 0;
    case TCODK_KP5:
      return state[SDL_SCANCODE_KP_5] != 0;
    case TCODK_KP6:
      return state[SDL_SCANCODE_KP_6] != 0;
    case TCODK_KP7:
      return state[SDL_SCANCODE_KP_7] != 0;
    case TCODK_KP8:
      return state[SDL_SCANCODE_KP_8] != 0;
    case TCODK_KP9:
      return state[SDL_SCANCODE_KP_9] != 0;
    case TCODK_KPDIV:
      return state[SDL_SCANCODE_KP_DIVIDE] != 0;
    case TCODK_KPMUL:
      return state[SDL_SCANCODE_KP_MULTIPLY] != 0;
    case TCODK_KPADD:
      return state[SDL_SCANCODE_KP_PLUS] != 0;
    case TCODK_KPSUB:
      return state[SDL_SCANCODE_KP_MINUS] != 0;
    case TCODK_KPENTER:
      return state[SDL_SCANCODE_KP_ENTER] != 0;
    case TCODK_KPDEC:
      return state[SDL_SCANCODE_KP_PERIOD] != 0;
    default:
      return 0;
  }
}

void TCOD_sys_unproject_screen_coords(int sx, int sy, int* ssx, int* ssy) {
  if (scale_data.dst_display_width != 0) {
    *ssx =
        (scale_data.src_x0 +
         ((sx - scale_data.dst_offset_x) * scale_data.src_copy_width) / scale_data.dst_display_width);
    *ssy =
        (scale_data.src_y0 +
         ((sy - scale_data.dst_offset_y) * scale_data.src_copy_width) / scale_data.dst_display_width);
  } else {
    *ssx = sx - TCOD_ctx.fullscreen_offset_x;
    *ssy = sy - TCOD_ctx.fullscreen_offset_y;
  }
}

void TCOD_sys_convert_console_to_screen_coords(int cx, int cy, int* sx, int* sy) {
  *sx = scale_data.dst_offset_x +
        ((cx * TCOD_ctx.font_width - scale_data.src_x0) * scale_data.dst_display_width) / scale_data.src_copy_width;
  *sy = scale_data.dst_offset_y +
        ((cy * TCOD_ctx.font_height - scale_data.src_y0) * scale_data.dst_display_height) / scale_data.src_copy_height;
}

void TCOD_sys_convert_screen_to_console_coords(int sx, int sy, int* cx, int* cy) {
  int ssx, ssy;
  TCOD_sys_unproject_screen_coords(sx, sy, &ssx, &ssy);
  *cx = ssx / TCOD_ctx.font_width;
  *cy = ssy / TCOD_ctx.font_height;
}
/** The global libtcod mouse state. */
static TCOD_mouse_t tcod_mouse = {0, 0, 0, 0, 0, 0, 0, 0, false, false, false, false, false, false, false, false};
/**
 *  Convert a pixel coordinate to a tile coordinate using global state.
 */
void TCOD_sys_pixel_to_tile(double* x, double* y) {
  if (!x || !y) {
    return;
  }
  if (TCOD_ctx.engine && TCOD_ctx.engine->c_pixel_to_tile_) {
    TCOD_ctx.engine->c_pixel_to_tile_(TCOD_ctx.engine, x, y);
  } else {
    *x = (*x - TCOD_ctx.fullscreen_offset_x) / TCOD_ctx.font_width;
    *y = (*y - TCOD_ctx.fullscreen_offset_y) / TCOD_ctx.font_height;
  }
}
/**
 *  Parse the coordinates and motion of an SDL event into a libtcod mouse
 *  struct.
 */
static void sdl_parse_mouse_(const SDL_Event* ev, TCOD_mouse_t* mouse) {
  if (!ev || !mouse) {
    return;
  }
  switch (ev->type) {
    case SDL_EVENT_MOUSE_MOTION:
      mouse->x = (int)ev->motion.x;
      mouse->y = (int)ev->motion.y;
      mouse->dx = (int)ev->motion.xrel;
      mouse->dy = (int)ev->motion.yrel;
      break;
    case SDL_EVENT_MOUSE_WHEEL:
      // Leave x,y attributes as is to preserve the original libtcod behaviour.
      mouse->wheel_up = ev->wheel.y > 0;
      mouse->wheel_down = ev->wheel.y < 0;
      mouse->dx = mouse->dy = 0;
      break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
      mouse->x = (int)ev->button.x;
      mouse->y = (int)ev->button.y;
      mouse->dx = mouse->dy = 0;
      break;
    default:
      return;
  }
  if (TCOD_ctx.engine && TCOD_ctx.engine->c_pixel_to_tile_) {
    double x = mouse->x;
    double y = mouse->y;
    TCOD_ctx.engine->c_pixel_to_tile_(TCOD_ctx.engine, &x, &y);
    int cell_x = (int)x;
    int cell_y = (int)y;
    x = mouse->x - mouse->dx;
    y = mouse->y - mouse->dy;
    TCOD_ctx.engine->c_pixel_to_tile_(TCOD_ctx.engine, &x, &y);
    int prev_cell_x = (int)x;
    int prev_cell_y = (int)y;
    mouse->cx = cell_x;
    mouse->cy = cell_y;
    mouse->dcx = cell_x - prev_cell_x;
    mouse->dcy = cell_y - prev_cell_y;
  } else {
    mouse->cx = 0;
    mouse->cy = 0;
    mouse->dcx = 0;
    mouse->dcy = 0;
  }
}
/**
 *  Parse an SDL_Event into `mouse` and return the relevant TCOD_event_t.
 *
 *  Returns 0 if the event wasn't mouse related.
 */
TCOD_event_t TCOD_sys_handle_mouse_event(const SDL_Event* ev, TCOD_mouse_t* mouse) {
  if (!ev) {
    return TCOD_EVENT_NONE;
  }
  if (!mouse) {
    mouse = &tcod_mouse;
  }
  sdl_parse_mouse_(ev, mouse);
  switch (ev->type) {
    case SDL_EVENT_MOUSE_MOTION:
      return TCOD_EVENT_MOUSE_MOVE;
    case SDL_EVENT_MOUSE_WHEEL:
      return TCOD_EVENT_MOUSE_PRESS;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      switch (ev->button.button) {
        case SDL_BUTTON_LEFT:
          mouse->lbutton = true;
          break;
        case SDL_BUTTON_MIDDLE:
          mouse->mbutton = true;
          break;
        case SDL_BUTTON_RIGHT:
          mouse->rbutton = true;
          break;
        default:
          break;
      }
      return TCOD_EVENT_MOUSE_PRESS;
    case SDL_EVENT_MOUSE_BUTTON_UP:
      switch (ev->button.button) {
        case SDL_BUTTON_LEFT:
          if (mouse->lbutton) {
            mouse->lbutton_pressed = true;
          }
          mouse->lbutton = false;
          break;
        case SDL_BUTTON_MIDDLE:
          if (mouse->mbutton) {
            mouse->mbutton_pressed = true;
          }
          mouse->mbutton = false;
          break;
        case SDL_BUTTON_RIGHT:
          if (mouse->rbutton) {
            mouse->rbutton_pressed = true;
          }
          mouse->rbutton = false;
          break;
      }
      return TCOD_EVENT_MOUSE_RELEASE;
    default:
      return TCOD_EVENT_NONE;
  }
}
/**
 *  Parse an SDL_Event into `key` and return the relevant TCOD_event_t.
 *
 *  Returns 0 if the event wasn't keyboard related.
 */
TCOD_event_t TCOD_sys_handle_key_event(const SDL_Event* ev, TCOD_key_t* key) {
  if (!ev) {
    return TCOD_EVENT_NONE;
  }
  if (!key) {
    key = &TCOD_ctx.key_state;
  }
  switch (ev->type) {
    case SDL_EVENT_KEY_DOWN:
      *key = TCOD_sys_SDLtoTCOD(ev, TCOD_KEY_PRESSED);
      return TCOD_EVENT_KEY_PRESS;
    case SDL_EVENT_KEY_UP:
      *key = TCOD_sys_SDLtoTCOD(ev, TCOD_KEY_RELEASED);
      return TCOD_EVENT_KEY_RELEASE;
    case SDL_EVENT_TEXT_INPUT: {
      *key = TCOD_ctx.key_state;
      key->vk = TCODK_TEXT;
      key->c = 0;
      key->pressed = 1;
      strncpy(key->text, ev->text.text, TCOD_KEY_TEXT_SIZE - 1);
      return TCOD_EVENT_KEY_PRESS;
    } break;
    default:
      return TCOD_EVENT_NONE;
  }
}
static TCOD_event_t TCOD_sys_handle_event(SDL_Event* ev, TCOD_event_t eventMask, TCOD_key_t* key, TCOD_mouse_t* mouse) {
  int retMask = TCOD_EVENT_NONE;
  /* printf("TCOD_sys_handle_event type=%04x\n", ev->type); */
  retMask |= TCOD_sys_handle_mouse_event(ev, mouse) & eventMask;
  retMask |= TCOD_sys_handle_key_event(ev, key) & eventMask;
  switch (ev->type) {
    case SDL_EVENT_QUIT:
      TCOD_ctx.is_window_closed = true;
      break;
#ifdef TCOD_ANDROID
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
      /* printf("SDL2 WINDOWEVENT (SDL_WINDOWEVENT_SIZE_CHANGED): 0x%04x w=%d h=%d\n", ev->window.event,
       * ev->window.data1, ev->window.data2); */
      /* If the app is started while the device is locked, the screen will be in portrait mode.  We need to rescale
       * when it changes. */
      if (scale_data.surface_width != ev->window.data1 || scale_data.surface_height != ev->window.data1)
        scale_data.force_recalc = 1;
      break;
    }
#endif
    case SDL_EVENT_WINDOW_MOUSE_ENTER: /**< Window has gained mouse focus */
      TCOD_ctx.app_has_mouse_focus = true;
      break;
    case SDL_EVENT_WINDOW_MOUSE_LEAVE: /**< Window has lost mouse focus */
      TCOD_ctx.app_has_mouse_focus = false;
      break;
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
      TCOD_ctx.app_is_active = true;
      break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
      TCOD_ctx.app_is_active = false;
      break;
    case SDL_EVENT_WINDOW_EXPOSED: /**< Window has been returned to and needs a refresh. */
      break;
    default:
      break;
  }
  return (TCOD_event_t)retMask;
}
/**
 *  Internal function containing code shared by TCOD_sys_wait_for_event
 *  and TCOD_sys_check_for_event
 */
static TCOD_event_t TCOD_sys_check_for_event_(SDL_Event* ev, int eventMask, TCOD_key_t* key, TCOD_mouse_t* mouse) {
  TCOD_event_t retMask = TCOD_EVENT_NONE;
  tcod_mouse.lbutton_pressed = 0;
  tcod_mouse.rbutton_pressed = 0;
  tcod_mouse.mbutton_pressed = 0;
  tcod_mouse.wheel_up = 0;
  tcod_mouse.wheel_down = 0;
  tcod_mouse.dx = 0;
  tcod_mouse.dy = 0;
  if (key) {
    key->vk = TCODK_NONE;
    key->c = 0;
    key->text[0] = '\0';
  }
  while (SDL_PollEvent(ev)) {
    retMask = TCOD_sys_handle_event(ev, (TCOD_event_t)eventMask, key, &tcod_mouse);
    if (retMask & TCOD_EVENT_KEY) {
      break; /* only one key event per frame */
    }
  }
  if (mouse) {
    *mouse = tcod_mouse;
  }
  if (key && !(eventMask & retMask & TCOD_EVENT_KEY)) {
    key->vk = TCODK_NONE;
    key->c = 0;
    key->text[0] = '\0';
  }
  return retMask;
}
/**
 *  Wait for a specific type of event.
 *
 *  \param eventMask A bit-mask of TCOD_event_t flags.
 *  \param key Optional pointer to a TCOD_key_t struct.
 *  \param mouse Optional pointer to a TCOD_mouse_t struct.
 *  \param flush This should always be false.
 *  \return A TCOD_event_t flag showing which event was actually processed.
 *
 *  This function also returns when the SDL window is being closed.
 */
TCOD_event_t TCOD_sys_wait_for_event(int eventMask, TCOD_key_t* key, TCOD_mouse_t* mouse, bool flush) {
  SDL_Event ev;
  TCOD_event_t retMask = TCOD_EVENT_NONE;
  if (eventMask == 0) {
    return TCOD_EVENT_NONE;
  }
  if (flush) {
    while (SDL_PollEvent(NULL)) {
      TCOD_sys_check_for_event_(&ev, 0, NULL, NULL);
    }
  }
  do {
    SDL_WaitEvent(NULL);
    retMask = TCOD_sys_check_for_event_(&ev, eventMask, key, mouse);
  } while (ev.type != SDL_EVENT_QUIT && (retMask & eventMask) == 0);
  return retMask;
}
/**
 *  Check for a specific type of event.
 *
 *  \param eventMask A bit-mask of TCOD_event_t flags.
 *  \param key Optional pointer to a TCOD_key_t struct.
 *  \param mouse Optional pointer to a TCOD_mouse_t struct.
 *  \return A TCOD_event_t flag showing which event was actually processed.
 */
TCOD_event_t TCOD_sys_check_for_event(int eventMask, TCOD_key_t* key, TCOD_mouse_t* mouse) {
  SDL_Event ev;
  return TCOD_sys_check_for_event_(&ev, eventMask, key, mouse);
}
/**
 *  Return a copy of the current mouse state.
 */
TCOD_mouse_t TCOD_mouse_get_status(void) { return tcod_mouse; }

/* classic keyboard functions (based on generic events) */
/**
 *  Return immediately with a recently pressed key.
 *
 *  \param flags A TCOD_event_t bit-field, for example: `TCOD_EVENT_KEY_PRESS`
 *  \return A TCOD_key_t struct with a recently pressed key.
 *          If no event exists then the `vk` attribute will be `TCODK_NONE`
 */
TCOD_key_t TCOD_sys_check_for_keypress(int flags) {
  TCOD_key_t key;
  TCOD_event_t ev = TCOD_sys_check_for_event(flags & TCOD_EVENT_KEY, &key, NULL);
  if ((ev & TCOD_EVENT_KEY) == 0) {
    return (TCOD_key_t){0};
  }
  return key;
}
/**
 *  Wait for a key press event, then return it.
 *
 *  \param flush If 1 then the event queue will be cleared before waiting for
 *               the next event.  This should always be 0.
 *  \return A TCOD_key_t struct with the most recent key data.
 */
TCOD_key_t TCOD_sys_wait_for_keypress(bool flush) {
  TCOD_key_t key;
  TCOD_event_t ev = TCOD_sys_wait_for_event(TCOD_EVENT_KEY_PRESS, &key, NULL, flush);
  if ((ev & TCOD_EVENT_KEY_PRESS) == 0) {
    return (TCOD_key_t){0};
  }
  return key;
}

void TCOD_sys_sleep_milli(uint32_t milliseconds) { SDL_Delay(milliseconds); }

SDL_Surface* TCOD_sys_load_image(const char* filename) {
  if (!filename) {
    TCOD_set_errorv("File name can not be NULL.");
    return NULL;
  }
  const image_support_t* img = image_type;
  while (img->extension != NULL && !img->check_type(filename)) {
    img++;
  }
  if (img->extension == NULL || img->read == NULL) {
    TCOD_set_errorvf("File is missing or corrupt: %s", filename);
    return NULL;  // Unknown format.
  }
  return img->read(filename);
}

void TCOD_sys_get_image_size(const SDL_Surface* image, int* w, int* h) {
  *w = image->w;
  *h = image->h;
}
TCOD_color_t TCOD_sys_get_image_pixel(const SDL_Surface* image, int x, int y) {
  if (x < 0 || y < 0 || x >= image->w || y >= image->h) {
    return TCOD_black;
  }
  const SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(image->format);
  uint8_t bpp = format->bytes_per_pixel;
  uint8_t* bits = image->pixels;
  bits += y * image->pitch + x * bpp;
  switch (bpp) {
    case 1: {
      const SDL_Palette* palette = SDL_GetSurfacePalette((SDL_Surface*)image);
      if (palette) {
        SDL_Color col = palette->colors[(*bits)];
        return (TCOD_ColorRGB){col.r, col.g, col.b};
      } else {
        return TCOD_black;
      }
    } break;
    default:
      return (TCOD_ColorRGB){
          *((bits) + format->Rshift / 8),
          *((bits) + format->Gshift / 8),
          *((bits) + format->Bshift / 8),
      };
  }
}

int TCOD_sys_get_image_alpha(const SDL_Surface* surf, int x, int y) {
  if (x < 0 || y < 0 || x >= surf->w || y >= surf->h) {
    return 255;
  }
  const SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(surf->format);
  uint8_t bpp = format->bytes_per_pixel;
  if (bpp != 4) {
    return 255;
  }
  uint8_t* bits = surf->pixels;
  bits += y * surf->pitch + x * bpp;
  return bits[format->Ashift / 8];
}
uint32_t TCOD_sys_elapsed_milli(void) { return (uint32_t)SDL_GetTicks(); }
float TCOD_sys_elapsed_seconds(void) {
  static float div = 1.0f / 1000.0f;
  return SDL_GetTicks() * div;
}

void TCOD_sys_force_fullscreen_resolution(int width, int height) {
  TCOD_ctx.fullscreen_width = width;
  TCOD_ctx.fullscreen_height = height;
}

void TCOD_sys_set_fps(int val) {
  if (val == 0)
    min_frame_length = 0;
  else
    min_frame_length = 1000 / val;
}

void TCOD_sys_save_fps(void) { min_frame_length_backup = min_frame_length; }

void TCOD_sys_restore_fps(void) { min_frame_length = min_frame_length_backup; }

int TCOD_sys_get_fps(void) { return fps; }

float TCOD_sys_get_last_frame_length(void) { return last_frame_length; }

void TCOD_sys_get_char_size(int* w, int* h) {
  if (TCOD_ctx.tileset) {
    *w = TCOD_ctx.tileset->tile_width;
    *h = TCOD_ctx.tileset->tile_height;
  } else {
    *w = TCOD_ctx.font_width;
    *h = TCOD_ctx.font_height;
  }
}

TCOD_Error TCOD_sys_get_current_resolution(int* w, int* h) {
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  SDL_DisplayID monitor_index = 0;
  if (window) {
    monitor_index = SDL_GetDisplayForWindow(window);
    if (monitor_index == 0) {
      return TCOD_set_errorvf("SDL error: %s", SDL_GetError());
    }
  }
  // Temporarily load the video subsystem if it isn't already active.
  if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
    return TCOD_set_errorvf("SDL error: %s", SDL_GetError());
  }
  SDL_Rect display_rect;
  if (!SDL_GetDisplayBounds(monitor_index, &display_rect)) {
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return TCOD_set_errorvf("SDL error: %s", SDL_GetError());
  }
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
  if (w) *w = display_rect.w;
  if (h) *h = display_rect.h;
  return TCOD_E_OK;
}

/* image stuff */
bool TCOD_sys_check_magic_number(const char* filename, size_t size, uint8_t* data) {
  uint8_t tmp[128];
  SDL_IOStream* rwops = SDL_IOFromFile(filename, "rb");
  if (!rwops) return false;
  if (SDL_ReadIO(rwops, tmp, size) != size) {
    SDL_CloseIO(rwops);
    return false;
  }
  SDL_CloseIO(rwops);
  for (size_t i = 0; i < size; i++) {
    if (tmp[i] != data[i]) {
      return false;
    }
  }
  return true;
}

SDL_Window* TCOD_sys_get_SDL_window(void) { return TCOD_sys_get_sdl_window(); }

SDL_Renderer* TCOD_sys_get_SDL_renderer(void) { return TCOD_sys_get_sdl_renderer(); }

/* mouse stuff */
void TCOD_mouse_show_cursor(bool visible) {
  if (visible) {
    SDL_ShowCursor();
  } else {
    SDL_HideCursor();
  }
}

bool TCOD_mouse_is_cursor_visible(void) { return SDL_CursorVisible(); }

void TCOD_mouse_move(int x, int y) {
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  if (!window) {
    return;
  }
  SDL_WarpMouseInWindow(window, (float)x, (float)y);
}

void TCOD_mouse_includes_touch(bool enable) { (void)enable; }

/*clipboard stuff */
bool TCOD_sys_clipboard_set(const char* text) { return SDL_SetClipboardText(text) == 0; }

char* TCOD_sys_clipboard_get() {
  /*
  We hold onto the last clipboard text pointer SDL gave us.
  For C API callers it can be considered a borrowed reference.
  For Python ctypes API callers, the contents are copied into the Python string that is constructed from it.
  */
  static char* last_clipboard_text = NULL;
  static char BLANK[] = "";
  if (last_clipboard_text) {
    SDL_free(last_clipboard_text);
  }
  last_clipboard_text = SDL_GetClipboardText();
  if (last_clipboard_text == NULL) {
    last_clipboard_text = BLANK;
  }
  return last_clipboard_text;
}

bool TCOD_sys_read_file(const char* filename, unsigned char** buf, size_t* size) {
  /* get file size */
  SDL_IOStream* rwops = SDL_IOFromFile(filename, "rb");
  if (!rwops) {
    return false;
  }
  SDL_SeekIO(rwops, 0, SDL_IO_SEEK_END);
  size_t filesize = SDL_TellIO(rwops);
  SDL_SeekIO(rwops, 0, SDL_IO_SEEK_SET);
  /* allocate buffer */
  *buf = (unsigned char*)malloc(filesize);
  /* read from file */
  if (SDL_ReadIO(rwops, *buf, filesize) != filesize) {
    SDL_CloseIO(rwops);
    free(*buf);
    return false;
  }
  SDL_CloseIO(rwops);
  *size = filesize;
  return true;
}

bool TCOD_sys_file_exists(const char* filename, ...) {
  char f[1024];
  va_list ap;
  va_start(ap, filename);
  vsnprintf(f, sizeof(f), filename, ap);
  va_end(ap);
  return SDL_GetPathInfo(f, NULL);
}

bool TCOD_sys_write_file(const char* filename, unsigned char* buf, uint32_t size) {
  SDL_IOStream* rwops = SDL_IOFromFile(filename, "wb");
  if (!rwops) {
    return false;
  }
  SDL_WriteIO(rwops, buf, size);
  SDL_CloseIO(rwops);
  return true;
}
#endif  // NO_SDL
