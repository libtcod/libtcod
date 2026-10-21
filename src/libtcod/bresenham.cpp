/* BSD 3-Clause License
 *
 * Copyright © 2008-2026, Jice and the libtcod contributors.
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
#include "bresenham.hpp"

#include <algorithm>
#include <climits>
#include <cstdint>
#include <cstdlib>

#include "error.h"

int TCOD_bresenham(int xFrom, int yFrom, int xTo, int yTo, int n, int* out) {
  if (n < 0) {
    TCOD_set_errorv("Bresenham output capacity must not be negative.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  const auto delta_x = std::abs(static_cast<int64_t>(xTo) - xFrom);
  const auto delta_y = std::abs(static_cast<int64_t>(yTo) - yFrom);
  const auto length = std::max(delta_x, delta_y) + 1;
  if (length > INT_MAX) {
    TCOD_set_errorv("Bresenham line length exceeds INT_MAX.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (!out || n == 0) return static_cast<int>(length);
  auto line = tcod::BresenhamLine({xFrom, yFrom}, {xTo, yTo});
  const int count = std::min(n, static_cast<int>(length));
  for (int i = 0; i < count; ++i) {
    const auto point = line[i];
    *out++ = point[0];
    *out++ = point[1];
  }
  return static_cast<int>(length);
}

// ********** bresenham line drawing **********
void TCODLine::init(int xFrom, int yFrom, int xTo, int yTo) { TCOD_line_init(xFrom, yFrom, xTo, yTo); }

bool TCODLine::step(int* xCur, int* yCur) { return TCOD_line_step(xCur, yCur) != 0; }

static TCODLineListener* line_listener = NULL;

// C to C++ bridge
extern "C" bool internalListener(int x, int y) { return line_listener->putPoint(x, y) ? 1 : 0; }

bool TCODLine::line(int xFrom, int yFrom, int xTo, int yTo, TCODLineListener* listener) {
  line_listener = listener;
  return TCOD_line(xFrom, yFrom, xTo, yTo, internalListener) != 0;
}
