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
#include "context_init.h"
#ifndef NO_SDL
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "console.h"
#include "console_etc.h"
#include "globals.h"
#include "libtcod_int.h"
#include "logging.h"
#include "renderer_sdl2.h"
#include "renderer_xterm.h"
#include "tileset_fallback.h"

static TCOD_Error ensure_tileset(TCOD_Tileset** tileset) {
  if (*tileset) {
    return TCOD_E_OK;
  }
  if (!TCOD_ctx.tileset) {
    TCOD_console_set_custom_font("terminal.png", TCOD_FONT_LAYOUT_ASCII_INCOL, 0, 0);
  }
  if (!TCOD_ctx.tileset) {
    TCOD_set_default_tileset(TCOD_tileset_load_fallback_font_(0, TCOD_FALLBACK_FONT_SIZE));
  }
  if (!TCOD_ctx.tileset) {
    TCOD_set_errorv("No font loaded and couldn't load a fallback font!");
    return TCOD_E_ERROR;
  }
  *tileset = TCOD_ctx.tileset;
  return TCOD_E_OK;
}
/**
    Return the renderer from a string object.  Returns -1 on failure.
 */
static int get_renderer_from_str(const char* string) {
  if (!string) {
    return -1;
  } else if (strcmp(string, "sdl") == 0) {
    return TCOD_RENDERER_SDL;
  } else if (strcmp(string, "opengl") == 0) {
    return TCOD_RENDERER_OPENGL;
  } else if (strcmp(string, "glsl") == 0) {
    return TCOD_RENDERER_GLSL;
  } else if (strcmp(string, "sdl2") == 0) {
    return TCOD_RENDERER_SDL2;
  } else if (strcmp(string, "opengl2") == 0) {
    return TCOD_RENDERER_OPENGL2;
  } else if (strcmp(string, "xterm") == 0) {
    return TCOD_RENDERER_XTERM;
  } else {
    return -1;
  }
}
/**
 *  Set `renderer` from the TCOD_RENDERER environment variable if it exists.
 */
static void get_env_renderer(int* renderer_type) {
  const char* value = getenv("TCOD_RENDERER");
  if (get_renderer_from_str(value) >= 0) {
    *renderer_type = get_renderer_from_str(value);
  }
}
/**
 *  Set `vsync` from the TCOD_VSYNC environment variable if it exists.
 */
static void get_env_vsync(int* vsync) {
  const char* value = getenv("TCOD_VSYNC");
  if (!value) {
    return;
  }
  if (strcmp(value, "0") == 0) {
    *vsync = 0;
  } else if (strcmp(value, "1") == 0) {
    *vsync = 1;
  }
}
/**
    Default command line message behavior.  Print to stdout and terminate.
 */
static void default_cli_output(void* userdata, const char* output) {
  (void)userdata;  // Unused.
  printf("%s", output);
  exit(0);
}
/**
    Sends formatted output to `cli_output`.

    If `cli_output` is NULL then `default_cli_output` is used.

    Always returns `TCOD_E_REQUIRES_ATTENTION`.
 */
TCODLIB_FORMAT(2, 3)
static TCOD_Error send_to_cli_out(const TCOD_ContextParams* params, const char* fmt, ...) {
  void (*cli_output)(void* userdata, const char* output) = params->cli_output;
  if (!cli_output) {
    cli_output = default_cli_output;
  }
  va_list vlist;
  va_start(vlist, fmt);
  char buffer[4096] = "";
  vsnprintf(buffer, sizeof(buffer), fmt, vlist);
  va_end(vlist);
  cli_output(params->cli_userdata, buffer);  // This is normally expected to terminate the program.
  TCOD_set_error("The provided command line arguments require attention.");
  return TCOD_E_REQUIRES_ATTENTION;
}
/**
    The message displayed when "-help" is given.
 */
static const char TCOD_help_msg[] =
    "Command line options:\n\
-help : Show this help message.\n\
-windowed : Open in a resizable window.\n\
-fullscreen : Open a borderless fullscreen window.\n\
-resolution <width>x<height> : Sets the desired pixel resolution.\n\
-width <pixels> : Set the desired pixel width.\n\
-height <pixels> : Set the desired pixel height.\n\
-renderer <sdl|sdl2|opengl|opengl2|xterm> : Change the active libtcod renderer.\n\
-vsync : Enable Vsync when possible.\n\
-no-vsync : Disable Vsync.\n\
";
/**
    Return true if an arg variable is "-name" or "--name".
 */
#define TCOD_CHECK_ARGUMENT(arg, name) (strcmp((arg), ("-" name)) == 0 || strcmp((arg), ("--" name)) == 0)
/***************************************************************************
    @brief Parse context parameters and output to a fully defined struct.
 */
static TCOD_Error parse_context_parameters(const TCOD_ContextParams* in, TCOD_ContextParams* out) {
  const int tcod_version = in->tcod_version ? in->tcod_version : TCOD_COMPILEDVERSION;
  *out = (TCOD_ContextParams){
      .tcod_version = tcod_version,
      .window_x = in->window_x,
      .window_y = in->window_y,
      .pixel_width = in->pixel_width,
      .pixel_height = in->pixel_height,
      .columns = in->columns,
      .rows = in->rows,
      .renderer_type = in->renderer_type,
      .tileset = in->tileset,
      .vsync = in->vsync,
      .sdl_window_flags = in->sdl_window_flags,
      .window_title = in->window_title,
      .argc = in->argc,
      .argv = in->argv,
      .cli_output = in->cli_output,
      .cli_userdata = in->cli_userdata,
      .window_xy_defined = in->window_xy_defined,
      .console = (tcod_version >= TCOD_VERSIONNUM(1, 19, 0) ? in->console : NULL),
  };
  if (!out->window_xy_defined) {
    if (!out->window_x) out->window_x = (int)SDL_WINDOWPOS_UNDEFINED;
    if (!out->window_y) out->window_y = (int)SDL_WINDOWPOS_UNDEFINED;
  }
  get_env_renderer(&out->renderer_type);
  get_env_vsync(&out->vsync);

  // Parse CLI arguments.
  for (int i = 0; i < out->argc; ++i) {
    if (strcmp(out->argv[i], "-?") == 0 || strcmp(out->argv[i], "-h") == 0 ||
        TCOD_CHECK_ARGUMENT(out->argv[i], "help")) {
      return send_to_cli_out(out, "%s", TCOD_help_msg);
    } else if (TCOD_CHECK_ARGUMENT(out->argv[i], "windowed")) {
      out->sdl_window_flags &= ~(SDL_WINDOW_FULLSCREEN);
      out->sdl_window_flags |= SDL_WINDOW_RESIZABLE;
    } else if (TCOD_CHECK_ARGUMENT(out->argv[i], "exclusive-fullscreen")) {
      out->sdl_window_flags &= ~(SDL_WINDOW_FULLSCREEN);
      out->sdl_window_flags |= SDL_WINDOW_FULLSCREEN;
    } else if (TCOD_CHECK_ARGUMENT(out->argv[i], "fullscreen")) {
      out->sdl_window_flags &= ~(SDL_WINDOW_FULLSCREEN);
      out->sdl_window_flags |= SDL_WINDOW_FULLSCREEN;
    } else if (TCOD_CHECK_ARGUMENT(out->argv[i], "vsync")) {
      out->vsync = 1;
    } else if (TCOD_CHECK_ARGUMENT(out->argv[i], "no-vsync")) {
      out->vsync = 0;
    } else if (TCOD_CHECK_ARGUMENT(out->argv[i], "width")) {
      if (++i < out->argc) {
        out->pixel_width = atoi(out->argv[i]);
      } else {
        return send_to_cli_out(out, "Width must be given a number.");
      }
    } else if (TCOD_CHECK_ARGUMENT(out->argv[i], "height")) {
      if (++i < out->argc) {
        out->pixel_height = atoi(out->argv[i]);
      } else {
        TCOD_set_error("Height must be given a number.");
        return send_to_cli_out(out, "Height must be given a number.");
      }
    } else if (TCOD_CHECK_ARGUMENT(out->argv[i], "renderer")) {
      if (++i < out->argc && get_renderer_from_str(out->argv[i]) >= 0) {
        out->renderer_type = get_renderer_from_str(out->argv[i]);
      } else {
        TCOD_set_error("Renderer should be one of [sdl|sdl2|opengl|opengl2|xterm]");
        return send_to_cli_out(out, "Renderer should be one of [sdl|sdl2|opengl|opengl2|xterm]");
      }
    } else if (TCOD_CHECK_ARGUMENT(out->argv[i], "resolution")) {
      if (++i < out->argc && sscanf(out->argv[i], "%dx%d", &out->pixel_width, &out->pixel_height) == 2) {
      } else {
        return send_to_cli_out(out, "Resolution argument should be in the format: <width>x<height>");
      }
    }
  }
  TCOD_Error err = ensure_tileset(&out->tileset);
  if (err < 0) return err;
  if (out->pixel_width < 0 || out->pixel_height < 0) {
    TCOD_set_errorvf("Width and height must be non-negative. Not %i,%i", out->pixel_width, out->pixel_height);
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (out->columns <= 0) out->columns = 80;
  if (out->rows <= 0) out->rows = 24;
  if (out->console) {
    out->columns = out->console->w;
    out->rows = out->console->h;
  }
  if (out->pixel_width <= 0) out->pixel_width = out->columns * out->tileset->tile_width;
  if (out->pixel_height <= 0) out->pixel_height = out->rows * out->tileset->tile_height;
  if (!out->window_title && out->argv) out->window_title = out->argv[0];  // Default the window title to argv[0].
  return TCOD_E_OK;
}

/**
    Create a new context while filling incomplete values as needed and
    unpacking values from the envrionment and the command line.
 */
TCOD_Error TCOD_context_new(const TCOD_ContextParams* params_in, TCOD_Context** out) {
  if (!params_in) {
    TCOD_set_errorv("Params must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!out) {
    TCOD_set_errorv("Output must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  TCOD_ContextParams params;
  TCOD_Error err = parse_context_parameters(params_in, &params);
  if (err < 0) return err;

  // Initialize the renderer.
  err = TCOD_E_OK;
  switch (params.renderer_type) {
    case TCOD_RENDERER_SDL:
    case TCOD_RENDERER_OPENGL2:
    case TCOD_RENDERER_OPENGL:
      TCOD_log_warning("All libtcod renderers other than TCOD_RENDERER_SDL2 will be ignored.");
      //@fallthrough@
    default:
    case TCOD_RENDERER_GLSL:  // Enum zero, so assume it means "don't care".
    case TCOD_RENDERER_SDL2:
      *out = TCOD_renderer_init_sdl2(
          params.window_x,
          params.window_y,
          params.pixel_width,
          params.pixel_height,
          params.window_title,
          params.sdl_window_flags,
          params.vsync,
          params.tileset);
      if (!*out) {
        return TCOD_E_ERROR;
      }
      return err;
    case TCOD_RENDERER_XTERM:
      *out = TCOD_renderer_init_xterm(
          params.window_xy_defined ? params.window_x : -1,
          params.window_xy_defined ? params.window_y : -1,
          params.pixel_width,
          params.pixel_height,
          params.columns,
          params.rows,
          params.window_title);
      if (!*out) return TCOD_E_ERROR;
      return err;
  }
}
#endif  // NO_SDL
