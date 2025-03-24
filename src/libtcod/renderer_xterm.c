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
#include "renderer_xterm.h"
#ifndef NO_SDL
#include <SDL3/SDL.h>
#include <ctype.h>
#include <limits.h>
#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
#include <windows.h>
#elif !defined(__MINGW32__)
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#endif

#include "console_types.h"
#include "error.h"
#include "logging.h"

#define DOUBLE_CLICK_TIME 500

#if defined(_WIN32)
static DWORD g_old_mode_stdin = 0;  // Windows console stdin mode before initialization.
static DWORD g_old_mode_stdout = 0;  // Windows console stdout mode before initialization.
#elif !defined(__MINGW32__)
static struct termios g_old_termios;
#endif

struct TerminalSizeOut {
  int columns;
  int rows;
  uint64_t timestamp;
};

static struct {
  SDL_Mutex* lock;
  struct TerminalSizeOut* out;
} g_terminal_size_state = {
    .lock = NULL,
    .out = NULL,
};
static struct {
  uint8_t button_down;
  uint64_t last_mouse_down_timestamp;
  uint8_t num_clicks;
  int last_mouse_motion_x;
  int last_mouse_motion_y;
} g_mouse_state = {
    .button_down = 0,
    .last_mouse_down_timestamp = 0,
    .num_clicks = 1,
    .last_mouse_motion_x = -1,
    .last_mouse_motion_y = -1,
};

struct TCOD_RendererXterm {
  TCOD_Console* cache;
  SDL_Thread* input_thread;
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
/// Poll and return the terminal size.  Returns TCOD_E_ERROR if this times out.
static TCOD_Error xterm_get_terminal_size(struct TerminalSizeOut* out) {
  out->timestamp = 0;
  out->columns = 0;
  out->rows = 0;
  fprintf(stdout, "\x1b[%i;%iH", SHRT_MAX, SHRT_MAX);  // Move cursor to lower-right corner.
  fflush(stdout);
  SDL_LockMutex(g_terminal_size_state.lock);
  g_terminal_size_state.out = out;
  SDL_UnlockMutex(g_terminal_size_state.lock);
  const uint64_t start_time = SDL_GetTicks();
  fprintf(stdout, "\x1b[6n");  // Poll the cursor position.
  fflush(stdout);
  while (SDL_GetTicks() < start_time + 100) {
    SDL_LockMutex(g_terminal_size_state.lock);
    if (out->timestamp >= start_time) {
      g_terminal_size_state.out = NULL;
      SDL_UnlockMutex(g_terminal_size_state.lock);
      return TCOD_E_OK;
    }
    SDL_UnlockMutex(g_terminal_size_state.lock);
    SDL_Delay(1);
  }
  SDL_LockMutex(g_terminal_size_state.lock);
  g_terminal_size_state.out = NULL;
  SDL_UnlockMutex(g_terminal_size_state.lock);
  return TCOD_E_ERROR;
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
  struct TerminalSizeOut term_size;
  xterm_get_terminal_size(&term_size);  // This polls the terminal and might be too slow.

  fprintf(stdout, "\x1b[?25l");  // Cursor un-hiding on Windows after window is resized.
  for (int y = 0; y < console->h && y < term_size.rows; ++y) {
    fprintf(stdout, "\x1b[%d;0H", y);  // Move cursor to start of next line.
    int skip_tiles = 0;  // Skip unchanged tiles.
    for (int x = 0; x < console->w && x < term_size.columns; ++x) {
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
/// Undo the terminal setup performed on initialization.
static void xterm_cleanup(void) {
  fprintf(
      stdout,
      "\x1b[2J"  // Clear the screen.
      "\x1b[?1049l"  // Disable alternative screen buffer.
      "\x1b[?25h"  // Show cursor.
      "\x1b[?1003l"  // Disable all motion mouse tracking.
      "\x1b[?1004h"  // Don't send focus in/out events.
      "\033c"  // Reset to initial state.
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
/// Send keyboard and text input events to SDL.
static void send_sdl_key_press(SDL_Keycode ch, bool shift) {
  SDL_Keycode sym = ch;
  SDL_Keymod mod = SDL_KMOD_NONE;
  if (shift) {
    sym = tolower(sym);
    mod = SDL_KMOD_SHIFT;
  }
  SDL_Event down_event = {
      .key = {
          .type = SDL_EVENT_KEY_DOWN,
          .timestamp = SDL_GetTicks(),
          .windowID = 0,
          .which = 0,
          .scancode = SDL_GetScancodeFromKey(sym, &mod),
          .key = sym,
          .mod = mod,
          .down = true,
          .repeat = false}};
  SDL_PushEvent(&down_event);
  SDL_Event up_event = down_event;
  up_event.type = SDL_EVENT_KEY_UP;
  up_event.key.down = false;
  SDL_PushEvent(&up_event);
}

static int read_terminated_int(char* after) {
  *after = '\0';
  char buf[16] = "";
  for (size_t i = 0; i < sizeof(buf) - 1; i++) {
    const char ch = (char)getchar();
    if (!isdigit(ch)) {
      *after = ch;
      buf[i] = '\0';
      return atoi(buf);
    }
    buf[i] = ch;
  }
  return atoi(buf);
}
/// Send mouse inputs to SDL.
static void xterm_handle_mouse_click(int cb, int x, int y) {
  const int cb_button = cb & 3;
  const uint64_t timestamp = SDL_GetTicks();
  uint32_t type = SDL_EVENT_MOUSE_BUTTON_DOWN;
  bool down = true;
  uint8_t button = 0;
  switch (cb_button) {
    case 0:
      button = SDL_BUTTON_LEFT;
      break;
    case 1:
      button = SDL_BUTTON_MIDDLE;
      break;
    case 2:
      button = SDL_BUTTON_RIGHT;
      break;
    case 3:
      type = SDL_EVENT_MOUSE_BUTTON_UP;
      down = false;
      button = g_mouse_state.button_down;
      break;
    default:
      TCOD_log_debug_f("unknown mouse button %i\n", cb_button);
  }
  if (type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    // We don't get button info on mouse up, so only do one click at once.
    if (g_mouse_state.button_down > 0) return;
    g_mouse_state.button_down = button;
    if (timestamp < g_mouse_state.last_mouse_down_timestamp + DOUBLE_CLICK_TIME) g_mouse_state.num_clicks += 1;
    g_mouse_state.last_mouse_down_timestamp = timestamp;
  } else {
    if (g_mouse_state.button_down <= 0) return;
    g_mouse_state.button_down = 0;
  }
  SDL_Event button_event = {
      .button = {
          .type = type,
          .timestamp = timestamp,
          .windowID = 0,
          .which = 0,
          .button = button,
          .down = down,
          .clicks = g_mouse_state.num_clicks,
          .x = (float)x,
          .y = (float)y,
      }};
  SDL_PushEvent(&button_event);
  if (type != SDL_EVENT_MOUSE_BUTTON_DOWN) g_mouse_state.num_clicks = 1;
}
/// Send mouse wheel events to SDL.
static void xterm_handle_mouse_wheel(int cb) {
  const int cb_button = cb & 3;
  float dy = 0;
  switch (cb_button) {
    case 0:
      dy = 1;
      break;
    case 1:
      dy = -1;
      break;
    default:
      TCOD_log_debug_f("unknown mouse wheel button %i\n", cb_button);
  }
  SDL_Event wheel_event = {
      .wheel = {
          .type = SDL_EVENT_MOUSE_WHEEL,
          .timestamp = SDL_GetTicks(),
          .windowID = 0,
          .which = 0,
          .x = 0,
          .y = dy,
          .direction = SDL_MOUSEWHEEL_NORMAL,
      }};
  SDL_PushEvent(&wheel_event);
}
/// Send mouse motion info to SDL.
static void xterm_handle_mouse_motion(int x, int y) {
  int xrel = 0, yrel = 0;
  if (g_mouse_state.last_mouse_motion_x >= 0 && g_mouse_state.last_mouse_motion_y >= 0) {
    xrel = x - g_mouse_state.last_mouse_motion_x;
    yrel = y - g_mouse_state.last_mouse_motion_y;
  }
  g_mouse_state.last_mouse_motion_x = x;
  g_mouse_state.last_mouse_motion_y = y;
  SDL_Event motion_event = {
      .motion = {
          .type = SDL_EVENT_MOUSE_MOTION,
          .timestamp = SDL_GetTicks(),
          .windowID = 0,
          .which = 0,
          .x = (float)x,
          .y = (float)y,
          .xrel = (float)xrel,
          .yrel = (float)yrel,
      }};
  SDL_PushEvent(&motion_event);
}
/// Parse X10 compatibility mode mouse escape sequences.
static void xterm_handle_mouse_escape() {
  const char cb = (char)getchar();
  const int x = getchar() - 33;
  const int y = getchar() - 33;
  if (cb & 32) {
    if (cb & 64)
      xterm_handle_mouse_wheel(cb);
    else
      xterm_handle_mouse_click(cb, x, y);
  } else {
    xterm_handle_mouse_motion(x, y);
  }
}

static bool xterm_handle_input_escape_code(char* start, char* end, int* arg0, int* arg1) {
  *start = '\0';
  *arg0 = -1;
  *arg1 = -1;
  *start = (char)getchar();
  if (*start != '[' && *start != 'O') return false;
  *arg0 = read_terminated_int(end);
  if (*end == ';') *arg1 = read_terminated_int(end);
  return true;
}
/// Send a window event to SDL.
static void xterm_handle_focus_change(SDL_EventType event) {
  SDL_Event focus_event = {
      .window = {
          .type = event,
          .timestamp = SDL_GetTicks(),
          .windowID = 0,
      }};
  SDL_PushEvent(&focus_event);
}
/// Dispatch an ANSI escape sequence, excluding the first escape byte.
static void xterm_handle_input_escape() {
  char start, end;
  int arg0, arg1;
  if (!xterm_handle_input_escape_code(&start, &end, &arg0, &arg1)) return;
  bool unknown = false;
  switch (start) {
    case '[':  // CSI
      switch (end) {
        case 'M':
          xterm_handle_mouse_escape();
          break;
        case 'I':
          xterm_handle_focus_change(SDL_EVENT_WINDOW_FOCUS_GAINED);
          break;
        case 'O':
          xterm_handle_focus_change(SDL_EVENT_WINDOW_FOCUS_LOST);
          break;
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
          SDL_LockMutex(g_terminal_size_state.lock);
          if (g_terminal_size_state.out != NULL) {
            g_terminal_size_state.out->rows = arg0;
            g_terminal_size_state.out->columns = arg1;
            g_terminal_size_state.out->timestamp = SDL_GetTicks();
          } else {
            send_sdl_key_press(SDLK_F3, false);
          }
          SDL_UnlockMutex(g_terminal_size_state.lock);
          break;
        case 'S':
          send_sdl_key_press(SDLK_F4, false);
          break;
        case '~':
          switch (arg0) {
            case 1:
              send_sdl_key_press(SDLK_HOME, false);
              break;
            case 4:
              send_sdl_key_press(SDLK_END, false);
              break;
            case 2:
              send_sdl_key_press(SDLK_INSERT, false);
              break;
            case 3:
              send_sdl_key_press(SDLK_DELETE, false);
              break;
            case 5:
              send_sdl_key_press(SDLK_PAGEUP, false);
              break;
            case 6:
              send_sdl_key_press(SDLK_PAGEDOWN, false);
              break;
            case 7:  // For urxvt
              send_sdl_key_press(SDLK_HOME, false);
              break;
            case 8:  // For urxvt
              send_sdl_key_press(SDLK_END, false);
              break;
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
              break;
            default:
              unknown = true;
          }
          break;
        default:
          unknown = true;
      }
      break;
    case 'O':  // SS3
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
        default:
          unknown = true;
      }
      break;
    default:
      unknown = true;
  }
  if (unknown) TCOD_log_debug_f("unknown input escape code '%c' '%c' %i %i\n", start, end, arg0, arg1);
}
/// ANSI input event loop.
static int xterm_handle_input(void* nulldata) {
  (void)nulldata;  // Unused
  while (true) {
    const char ch = (char)getchar();
    if (ch == '\x1b') {
      xterm_handle_input_escape();
      continue;
    }
    send_sdl_key_press(ch, isupper(ch));
  }
  return 0;
}

static TCOD_Error xterm_recommended_console_size(
    struct TCOD_Context* __restrict self, float magnification, int* __restrict columns, int* __restrict rows) {
  (void)self;  // Unused.
  (void)magnification;
  struct TerminalSizeOut size_out;
  TCOD_Error err = xterm_get_terminal_size(&size_out);
  if (err < 0) return err;
  *columns = size_out.columns;
  *rows = size_out.rows;
  return TCOD_E_OK;
}

#ifndef _WIN32
static void xterm_on_window_change_signal(int signum) {
  (void)signum;  // Unused
  int columns, rows;
  xterm_recommended_console_size(NULL, 1.0, &columns, &rows);
  SDL_Event resize_event = {
      .window = {
          .type = SDL_EVENT_WINDOW_RESIZED,
          .timestamp = SDL_GetTicks(),
          .windowID = 0,
          .data1 = columns,
          .data2 = rows,
      }};
  SDL_PushEvent(&resize_event);
}
#endif

#ifndef _WIN32
static void xterm_on_hangup_signal(int signum) {
  (void)signum;  // Unused
  SDL_Event quit_event = {
      .quit = {
          .type = SDL_EVENT_QUIT,
          .timestamp = SDL_GetTicks(),
      }};
  SDL_PushEvent(&quit_event);
}
#endif

TCOD_Context* TCOD_renderer_init_xterm(
    int window_x, int window_y, int pixel_width, int pixel_height, int columns, int rows, const char* window_title) {
#ifdef __MINGW32__
  TCOD_set_errorv("Renderer not supported.");
  return NULL;
#endif
  TCOD_Context* context = TCOD_context_new_();
  if (!context) return NULL;
  context->type = TCOD_RENDERER_XTERM;
  struct TCOD_RendererXterm* data = context->contextdata_ = calloc(1, sizeof(*data));
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
      "\x1b[?1003h"  // Enable all motion mouse tracking.
      "\x1b[?1004h"  // Send focus in/out events.
  );
  if (window_x > 0 && window_y > 0) fprintf(stdout, "\x1b[3;%i;%it", window_x, window_y);
  if (columns > 0 && rows > 0) {
    fprintf(stdout, "\x1b[8;%i;%it", rows, columns);
  } else if (pixel_width > 0 && pixel_height > 0) {
    fprintf(stdout, "\x1b[4;%i;%it", pixel_height, pixel_width);
  }
  if (window_title) fprintf(stdout, "\x1b]0;%s\x07", window_title);
  fflush(stdout);
  g_terminal_size_state.lock = SDL_CreateMutex();
  SDL_Init(SDL_INIT_VIDEO);  // Need SDL init to get keysyms
  data->input_thread = SDL_CreateThread(&xterm_handle_input, "input thread", NULL);
  return context;
}
#endif  // NO_SDL
