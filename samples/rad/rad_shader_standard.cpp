/*
 * Photon reactor
 * Copyright (c) 2011 Jice
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The names of Jice may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY JICE ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL JICE BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <libtcod.h>

#include "rad_shader.hpp"

void StandardShader::compute() {
  // turn off all lights
  memset(lightmap_, 0, sizeof(TCODColor) * map_->getWidth() * map_->getHeight());
  for (const Light& l : lights_) {
    // compute the potential visible set for this light
    int minx = l.x - l.radius;
    int miny = l.y - l.radius;
    int maxx = l.x + l.radius;
    int maxy = l.y + l.radius;
    miny = std::max(0, miny);
    minx = std::max(0, minx);
    maxx = std::min(maxx, map_->getWidth() - 1);
    maxy = std::min(maxy, map_->getHeight() - 1);
    const float offset = 1.0f / (1.0f + static_cast<float>(l.radius * l.radius) / 20.0f);
    const float factor = 1.0f / (1.0f - offset);
    // compute the light's fov
    TCODMap l_map(maxx - minx + 1, maxy - miny + 1);
    for (int x = minx; x <= maxx; x++) {
      for (int y = miny; y <= maxy; y++) {
        l_map.setProperties(x - minx, y - miny, map_->isWalkable(x, y), map_->isTransparent(x, y));
      }
    }
    l_map.computeFov(l.x - minx, l.y - miny, l.radius);
    // compute the light's contribution
    // double invSquaredRadius=1.0 / (l->radius*l->radius);
    for (int x = minx; x <= maxx; x++) {
      for (int y = miny; y <= maxy; y++) {
        if (l_map.isInFov(x - minx, y - miny)) {
          const int squaredDist = (l.x - x) * (l.x - x) + (l.y - y) * (l.y - y);
          // basic
          // double coef = 1.0-squaredDist*invSquaredRadius;
          // inv_sqr1
          // double coef=(1.0f/(1.0f+(float)(squaredDist)));
          // inv_sqr2
          const float coef = (1.0f / (1.0f + static_cast<float>(squaredDist) / 20.0f) - offset) * factor;
          TCODColor* col = &lightmap_[x + y * map_->getWidth()];
          *col = *col + l.col * coef;
        }
      }
    }
  }
}
