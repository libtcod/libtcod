/*
 * Copyright (c) 2010 Jice
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The name of Jice may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Jice ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Jice BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <SDL3/SDL.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>

// windows specific inclusion of alloca
// all other platforms have alloca in stdlib.h
#ifndef alloca
#include <malloc.h>
#endif

#include <libtcod.hpp>

#include "main.hpp"
#include "util_ripples.hpp"

// dummy height indicating that a cell is not water
static constexpr auto NO_WATER = -1000.0f;
// how much wave energy is lost per second
static constexpr auto DAMPING_COEF = 1.3f;
// wave height below which the water is considered still
static constexpr auto ACTIVE_THRESHOLD = 0.02f;
// height on the triggering ripple
static constexpr auto RIPPLE_TRIGGER = 3.0f;
// how many times ripples are updated per second
static constexpr auto RIPPLE_FPS = 10;

RippleManager::RippleManager(const TCODMap& waterMap) : width{waterMap.getWidth()}, height{waterMap.getHeight()} {
  std::vector<bool> visited(width * height);
  // first time : compute water zones
  zone.cumulatedElapsed = 0.0f;
  zone.isActive = false;
  zone.data = std::vector<float>(width * height);
  for (int dy = 0; dy < height; ++dy) {
    for (int dx = 0; dx < width; ++dx) {
      const float value = waterMap.isWalkable(dx, dy) ? 0.0f : NO_WATER;
      // set water height to 0.0, not water cells to -1000
      zone.data[dx + dy * width] = value;
    }
  }
  zone.oldData = zone.data;
}

void RippleManager::startRipple(int x, int y) {
  // look for the water zone
  if (!(0 <= x && x < width && 0 <= y && y < height)) return;
  const int offset = x + y * width;
  if (zone.data[offset] != NO_WATER) {
    zone.data[offset] = -RIPPLE_TRIGGER;
    zone.isActive = true;
  }
}

bool RippleManager::updateRipples(float elapsed) {
  zone.cumulatedElapsed += elapsed;
  if (zone.isActive && zone.cumulatedElapsed > 1.0f / RIPPLE_FPS) {
    zone.isActive = false;
    // update the ripples
    zone.cumulatedElapsed = 0.0f;
    // swap grids
    std::swap(zone.data, zone.oldData);
    for (int zy2 = 1; zy2 < height - 1; ++zy2) {
      for (int zx2 = 1; zx2 < width - 1; ++zx2) {
        const int offset = zx2 + zy2 * width;
        if (zone.data[offset] != NO_WATER) {
          float sum = 0.0f;
          int count = 0;
          // wave smoothing + spreading
          if (zone.oldData[offset - 1] != NO_WATER) {
            sum += zone.oldData[offset - 1];
            ++count;
          }
          if (zone.oldData[offset + 1] != NO_WATER) {
            sum += zone.oldData[offset + 1];
            ++count;
          }
          if (zone.oldData[offset - width] != NO_WATER) {
            sum += zone.oldData[offset - width];
            ++count;
          }
          if (zone.oldData[offset + width] != NO_WATER) {
            sum += zone.oldData[offset + width];
            ++count;
          }
          sum = sum * 2 / count;
          zone.data[offset] = sum - zone.data[offset];
          // damping
          zone.data[offset] *= 1.0f - DAMPING_COEF / RIPPLE_FPS;
          if (std::abs(zone.data[offset]) > ACTIVE_THRESHOLD) {
            zone.isActive = true;
          }
        }
      }
    }
  }
  return zone.isActive;
}

void RippleManager::renderRipples(const TCODImage& ground, TCODImage& groundWithRipples) const {
  const float elCoef = SDL_GetTicks() / 1000.0f * 2.0f;
  for (int y = 1; y < height - 1; ++y) {
    for (int x = 1; x < width - 1; ++x) {
      if (getData(x, y) != NO_WATER) {
        float xOffset = (getData(x - 1, y) - getData(x + 1, y));
        const float yOffset = (getData(x, y - 1) - getData(x, y + 1));
        const float f[3] = {static_cast<float>(x), static_cast<float>(y), elCoef};
        xOffset += noise3d.get(f, TCOD_NOISE_SIMPLEX) * 0.3f;
        if (std::abs(xOffset) < 250 && std::abs(yOffset) < 250) {
          TCODColor col = ground.getPixel(x + static_cast<int>(xOffset), y + static_cast<int>(yOffset));
          col = col + TCODColor{255, 255, 255} * xOffset * 0.1f;
          groundWithRipples.putPixel(x, y, col);
        }
      }
    }
  }
}
