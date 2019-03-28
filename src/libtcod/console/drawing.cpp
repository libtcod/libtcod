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
#include "drawing.h"

#include "../libtcod_int.h"
#include "../console.h"
namespace tcod {
namespace console {
/**
 *  Clamp the given values to fit within a console.
 */
static void clamp_rect_(int cx, int cy, int cw, int ch,
                        int& x, int& y, int& w, int& h)
{
  if (x + w > cw) { w = cw - x; }
  if (y + h > ch) { h = ch - y; }
  if (x < cx) {
    w -= cx - x;
    x = cx;
  }
  if (y < cy) {
    h -= cy - y;
    y = cy;
  }
}
void put(
    TCOD_Console* console,
    int x,
    int y,
    int ch,
    const TCOD_color_t* fg,
    const TCOD_color_t* bg,
    TCOD_bkgnd_flag_t flag)
{
  console = TCOD_console_validate_(console);
  if (!console) { return; }
  if (!TCOD_console_is_index_valid_(console, x, y)) { return; }
  int console_index = y * console->w + x;
  if (ch > 0) { console->tiles[console_index].ch = ch; }
  if (fg) { TCOD_console_set_char_foreground(console, x, y, *fg); }
  if (bg) { TCOD_console_set_char_background(console, x, y, *bg, flag); }
}
void draw_rect(
    TCOD_Console* console,
    int x,
    int y,
    int width,
    int height,
    int ch,
    const TCOD_color_t* fg,
    const TCOD_color_t* bg,
    TCOD_bkgnd_flag_t flag)
{
  console = TCOD_console_validate_(console);
  if (!console) { return; }
  clamp_rect_(0, 0, console->w, console->h, x, y, width, height);
  TCOD_ASSERT(x + width <= console->w && y + height <= console->h);
  for (int console_y = y; console_y < y + height; ++console_y) {
    for (int console_x = x; console_x < x + width; ++console_x) {
      put(console, console_x, console_y, ch, fg, bg, flag);
    }
  }
}
} // namespace console
} // namespace tcod
void TCOD_console_rect(TCOD_Console* console, int x, int y, int rw, int rh,
                       bool clear, TCOD_bkgnd_flag_t flag)
{
  console = TCOD_console_validate_(console);
  if (!console) { return; }
  tcod::console::draw_rect(console, x, y, rw, rh,
                           clear ? 0x20 : 0, nullptr, &console->back, flag);
}
void TCOD_console_hline(TCOD_Console* console,int x, int y, int l,
                        TCOD_bkgnd_flag_t flag)
{
  console = TCOD_console_validate_(console);
  if (!console) { return; }
  tcod::console::draw_rect(console, x, y, l, 1,
                           0x2500, &console->fore, &console->back, flag); // ─
}
void TCOD_console_vline(TCOD_Console* console,int x, int y, int l,
                        TCOD_bkgnd_flag_t flag)
{
  console = TCOD_console_validate_(console);
  if (!console) { return; }
  tcod::console::draw_rect(console, x, y, 1, l,
                           0x2502, &console->fore, &console->back, flag); // │
}
