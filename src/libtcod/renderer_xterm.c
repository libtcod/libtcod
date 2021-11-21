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
#include "renderer_xterm.h"

#include <SDL.h>
#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#if defined(_WIN32)
#include <windows.h>
#elif !defined(__MINGW32__)
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#endif

#include "error.h"

#if defined(_WIN32)
static DWORD g_old_mode_stdin = 0;
static DWORD g_old_mode_stdout = 0;
#elif !defined(__MINGW32__)
static struct termios g_old_termios;
#endif

static bool g_waiting_for_get_size = false;
static int g_got_rows = 0;
static int g_got_columns = 0;
static Uint32 g_got_size_timestamp = 0;

struct TCOD_RendererXterm {
  TCOD_Console* cache;
  SDL_Thread *input_thread;
};

static char* ucs4_to_utf8(int ucs4, char out[5]) {
  static const unsigned char B10000000 = 128;
  static const unsigned char B11000000 = 192;
  static const unsigned char B11100000 = 224;
  static const unsigned char B11110000 = 240;
  static const int B000_000000_000000_111111 = 63;
  static const int B000_000000_011111_000000 = 1984;
  static const int B000_000000_111111_000000 = 4032;
  static const int B000_001111_000000_000000 = 61440;
  static const int B000_111111_000000_000000 = 258048;
  static const int B111_000000_000000_000000 = 1835008;
  if (ucs4 < 0) {
  } else if (ucs4 <= 0x7F) {
    out[0] = (char)ucs4;
    out[1] = '\0';
    return out;
  } else if (ucs4 <= 0x07FF) {
    out[0] = B11000000 | (unsigned char)((ucs4 & B000_000000_011111_000000) >> 6);
    out[1] = B10000000 | (unsigned char)((ucs4 & B000_000000_000000_111111) >> 0);
    out[2] = '\0';
    return out;
  } else if (ucs4 <= 0xFFFF) {
    out[0] = B11100000 | (unsigned char)((ucs4 & B000_001111_000000_000000) >> 12);
    out[1] = B10000000 | (unsigned char)((ucs4 & B000_000000_111111_000000) >> 6);
    out[2] = B10000000 | (unsigned char)((ucs4 & B000_000000_000000_111111) >> 0);
    out[3] = '\0';
    return out;
  } else if (ucs4 <= 0x10FFFF) {
    out[0] = B11110000 | (unsigned char)((ucs4 & B111_000000_000000_000000) >> 18);
    out[1] = B10000000 | (unsigned char)((ucs4 & B000_111111_000000_000000) >> 12);
    out[2] = B10000000 | (unsigned char)((ucs4 & B000_000000_111111_000000) >> 6);
    out[3] = B10000000 | (unsigned char)((ucs4 & B000_000000_000000_111111) >> 0);
    out[4] = '\0';
    return out;
  }
  out[0] = '?';
  out[1] = '\0';
  return out;
}

static TCOD_Error xterm_present(
    struct TCOD_Context* __restrict self,
    const struct TCOD_Console* __restrict console,
    const struct TCOD_ViewportOptions* __restrict viewport) {
  (void)viewport;
  struct TCOD_RendererXterm* context = self->contextdata_;
  if (context->cache && (context->cache->w != console->w || context->cache->h != console->h)) {
    TCOD_console_delete(context->cache);
    context->cache = NULL;
  }
  if (!context->cache) {
    context->cache = TCOD_console_new(console->w, console->h);
    for (int i = 0; i < context->cache->elements; ++i) context->cache->tiles[i].ch = -1;
  }
  fprintf(stdout, "\x1b[?25l");  // Cursor un-hiding on Windows after window is resized.
  for (int y = 0; y < console->h; ++y) {
    fprintf(stdout, "\x1b[%d;0H", y);  // Move cursor to start of next line.
    int skip_tiles = 0;  // Skip unchanged tiles.
    for (int x = 0; x < console->w; ++x) {
      TCOD_ConsoleTile* prev_tile = &context->cache->tiles[console->w * y + x];
      const TCOD_ConsoleTile* tile = &console->tiles[console->w * y + x];
      if (tile->ch == prev_tile->ch && tile->fg.r == prev_tile->fg.r && tile->fg.g == prev_tile->fg.g &&
          tile->fg.b == prev_tile->fg.b && tile->bg.r == prev_tile->bg.r && tile->bg.g == prev_tile->bg.g &&
          tile->bg.b == prev_tile->bg.b) {
        ++skip_tiles;
        continue;
      }
      if (skip_tiles) {
        fprintf(stdout, "\x1b[%dC", skip_tiles);  // Move cursor forward.
        skip_tiles = 0;
      }
      char utf8[5];
      fprintf(  // Print a character with colors.
          stdout,
          "\x1b[38;2;%u;%u;%u;48;2;%u;%u;%um%s",
          tile->fg.r,
          tile->fg.g,
          tile->fg.b,
          tile->bg.r,
          tile->bg.g,
          tile->bg.b,
          ucs4_to_utf8(tile->ch & 0x10FFFF, utf8));
      *prev_tile = *tile;
    }
  }
  return TCOD_E_OK;
}

static void xterm_cleanup(void) {
  fprintf(
      stdout,
      "\x1b[2J"  // Clear the screen.
      "\x1b[?1049l"  // Disable alternative screen buffer.
      "\x1b[?25h"  // Show cursor.
      "\x1b" "c"  // Reset to initial state.
  );
#if defined(_WIN32)
  SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), g_old_mode_stdin);
  SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), g_old_mode_stdout);
#elif !defined(__MINGW32__)
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_old_termios);
#endif
}

static void xterm_destructor(struct TCOD_Context* __restrict self) {
  struct TCOD_RendererXterm* context = self->contextdata_;
  if (context) free(context);
  xterm_cleanup();
}

static void send_sdl_key_press(SDL_Keycode ch, bool shift) {
    bool is_ascii = ch <= (SDL_Keycode)INT_MAX && isascii(ch);
    SDL_Keycode sym = ch;
    Uint16 mod = KMOD_NONE;
    isascii(ch);
    if (shift) {
      sym = tolower(sym);
      mod = KMOD_SHIFT;
    }
    SDL_Event down_event = {
      .key = {
        .type = SDL_KEYDOWN,
        .timestamp = SDL_GetTicks(),
        .windowID = 0,
        .state = SDL_PRESSED,
        .repeat = 0,
        .keysym = {
          .sym = sym,
          .scancode = SDL_GetScancodeFromKey(sym),
          .mod = mod
        }
      }
    };
    SDL_PushEvent(&down_event);
    if (is_ascii && isprint(ch)) {
      SDL_Event text_event = {
        .text = {
          .type = SDL_TEXTINPUT,
          .timestamp = SDL_GetTicks(),
          .windowID = 0,
          .text[0] = ch,
          .text[1] = '\0'
        }
      };
      SDL_PushEvent(&text_event);
    }
    SDL_Event up_event = down_event;
    up_event.type = SDL_KEYUP;
    up_event.key.state = SDL_RELEASED;
    SDL_PushEvent(&up_event);
}

static int read_terminated_int(char *after) {
  *after = '\0';
  char buf[16] = "";
  for (size_t i = 0; i < sizeof(buf) - 1; i++) {
    int ch = getchar();
    if (!isdigit(ch)) {
      *after = ch;
      buf[i] = '\0';
      return atoi(buf);
    }
    buf[i] = ch;
  }
  return atoi(buf);
}

static bool xterm_handle_input_escape_code(
    char *start,
    char *end,
    int *arg0,
    int *arg1) {
  *start = '\0';
  *arg0 = -1;
  *arg1 = -1;
  *start = getchar();
  if (*start != '[' && *start != 'O')
    return false;
  *arg0 = read_terminated_int(end);
  if (*end == ';')
    *arg1 = read_terminated_int(end);
  return true;
}

static void xterm_handle_input_escape() {
  char start, end;
  int arg0, arg1;
  if (!xterm_handle_input_escape_code(&start, &end, &arg0, &arg1))
    return;
  switch (start) {
    case '[': // CSI
      switch (end) {
        case 'A':
          send_sdl_key_press(SDLK_UP, false);
          break;
        case 'B':
          send_sdl_key_press(SDLK_DOWN, false);
          break;
        case 'C':
          send_sdl_key_press(SDLK_RIGHT, false);
          break;
        case 'D':
          send_sdl_key_press(SDLK_LEFT, false);
          break;
        case 'H':
          send_sdl_key_press(SDLK_HOME, false);
          break;
        case 'F':
          send_sdl_key_press(SDLK_END, false);
          break;
        case 'P':
          send_sdl_key_press(SDLK_F1, false);
          break;
        case 'Q':
          send_sdl_key_press(SDLK_F2, false);
          break;
        case 'R':
          if (g_waiting_for_get_size) {
            g_got_rows = arg0;
            g_got_columns = arg1;
            g_got_size_timestamp = SDL_GetTicks();
          } else {
            send_sdl_key_press(SDLK_F3, false);
          }
          break;
        case 'S':
          send_sdl_key_press(SDLK_F4, false);
          break;
        case '~':
          switch (arg0) {
            case 11:
              send_sdl_key_press(SDLK_F1, false);
              break;
            case 12:
              send_sdl_key_press(SDLK_F2, false);
              break;
            case 13:
              send_sdl_key_press(SDLK_F3, false);
              break;
            case 14:
              send_sdl_key_press(SDLK_F4, false);
              break;
            case 15:
              send_sdl_key_press(SDLK_F5, false);
              break;
            case 17:
              send_sdl_key_press(SDLK_F6, false);
              break;
            case 18:
              send_sdl_key_press(SDLK_F7, false);
              break;
            case 19:
              send_sdl_key_press(SDLK_F8, false);
              break;
            case 20:
              send_sdl_key_press(SDLK_F9, false);
              break;
            case 21:
              send_sdl_key_press(SDLK_F10, false);
              break;
            case 23:
              send_sdl_key_press(SDLK_F11, false);
              break;
            case 24:
              send_sdl_key_press(SDLK_F12, false);
          }
          break;
      }
      break;
    case 'O': // SS3
      switch (end) {
        case 'P':
          send_sdl_key_press(SDLK_F1, false);
          break;
        case 'Q':
          send_sdl_key_press(SDLK_F2, false);
          break;
        case 'R':
          send_sdl_key_press(SDLK_F3, false);
          break;
        case 'S':
          send_sdl_key_press(SDLK_F4, false);
          break;
      }
      break;
  }
}

static int xterm_handle_input(void *arg) {
  while (true) {
    int ch = getchar();
    if (ch == '\x1b') {
      xterm_handle_input_escape();
      continue;
    }
    send_sdl_key_press(ch, isupper(ch));
  }
  return 0;
}

static TCOD_Error xterm_recommended_console_size(
    struct TCOD_Context* __restrict self,
    float magnification,
    int* __restrict columns,
    int* __restrict rows) {
  fprintf(stdout, "\x1b[%i;%iH", INT_MAX, INT_MAX);
  fflush(stdout);
  g_waiting_for_get_size = true;
  Uint32 start_time = SDL_GetTicks();
  fprintf(stdout, "\x1b[6n");
  fflush(stdout);
  while (!SDL_TICKS_PASSED(SDL_GetTicks(), start_time + 100)) {
    if (SDL_TICKS_PASSED(g_got_size_timestamp, start_time)) {
      *columns = g_got_columns;
      *rows = g_got_rows;
      g_waiting_for_get_size = false;
      return TCOD_E_OK;
    }
    SDL_Delay(1);
  }
  g_waiting_for_get_size = false;
  return TCOD_E_ERROR;
}

#ifndef _WIN32
static void xterm_on_window_change_signal(int signum) {
  int columns, rows;
  xterm_recommended_console_size(NULL, 1.0, &columns, &rows);
  SDL_Event resize_event = {
    .window = {
      .type = SDL_WINDOWEVENT,
      .event = SDL_WINDOWEVENT_RESIZED,
      .timestamp = SDL_GetTicks(),
      .windowID = 0,
      .data1 = columns,
      .data2 = rows,
    }
  };
  SDL_PushEvent(&resize_event);
}
#endif

#ifndef _WIN32
static void xterm_on_hangup_signal(int signum) {
  SDL_Event quit_event = {
    .quit = {
      .type = SDL_QUIT,
      .timestamp = SDL_GetTicks(),
    }
  };
  SDL_PushEvent(&quit_event);
}
#endif

TCOD_Context* TCOD_renderer_init_xterm(
    int columns,
    int rows,
    const char* window_title) {
#ifdef __MINGW32__
  TCOD_set_errorv("Renderer not supported.");
  return NULL;
#endif
  TCOD_Context* context = TCOD_context_new_();
  if (!context) return NULL;
  struct TCOD_RendererXterm* data = context->contextdata_ = calloc(sizeof(*data), 1);
  if (!data) {
    TCOD_context_delete(context);
    TCOD_set_errorv("Could not allocate memory.");
    return NULL;
  }
  context->c_present_ = &xterm_present;
  context->c_destructor_ = &xterm_destructor;
  context->c_recommended_console_size_ = xterm_recommended_console_size;
  atexit(&xterm_cleanup);
  setlocale(LC_ALL, ".UTF-8");  // Enable UTF-8.
#if defined(_WIN32) && !defined(__MINGW32__)
  HANDLE handle_stdin = GetStdHandle(STD_INPUT_HANDLE);
  HANDLE handle_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleMode(handle_stdin, &g_old_mode_stdin);
  GetConsoleMode(handle_stdout, &g_old_mode_stdout);
  SetConsoleMode(handle_stdin, ENABLE_VIRTUAL_TERMINAL_INPUT);
  SetConsoleMode(
      handle_stdout, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN);
#elif !defined(__MINGW32__)
  tcgetattr(STDIN_FILENO, &g_old_termios);
  struct termios new_termios = g_old_termios;
  new_termios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  new_termios.c_oflag &= ~(OPOST);
  new_termios.c_cflag &= ~(CSIZE | PARENB);
  new_termios.c_cflag |= CS8;
  new_termios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  new_termios.c_cc[VMIN] = 1;
  new_termios.c_cc[VTIME] = 1;
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios) < 0) {
    TCOD_set_errorv("Could not set raw terminal mode.");
    return NULL;
  }
  signal(SIGWINCH, xterm_on_window_change_signal);
  signal(SIGHUP, xterm_on_hangup_signal);
#endif
  fprintf(
      stdout,
      "\x1b[?1049h"  // Enable alternative screen buffer.
      "\x1b[2J"  // Clear the screen.
      "\x1b[?25l"  // Hide cursor.
  );
  if (columns > 0 && rows > 0) fprintf(stdout, "\x1b[8;%i;%it", rows, columns);
  if (window_title) fprintf(stdout, "\x1b]0;%s\x07", window_title);
  SDL_Init(SDL_INIT_VIDEO); // Need SDL init to get keysyms
  data->input_thread = SDL_CreateThread(&xterm_handle_input, "input thread", NULL);
  return context;
}
