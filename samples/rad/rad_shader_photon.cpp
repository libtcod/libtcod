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

#include <algorithm>

#include "rad_shader.hpp"

PhotonShader::PhotonShader(float reflectivity, float selfIllumination, int nbPass)
    : maxRadius_(0), reflectivity_(reflectivity), selfIllumination_(selfIllumination), nbPass_(nbPass) {}

int PhotonShader::addLight(int x, int y, int radius, const TCODColor& col) {
  if (radius > maxRadius_) maxRadius_ = radius;
  return Shader::addLight(x, y, radius, col);
}

void PhotonShader::init(TCODMap* map) {
  Shader::init(map);
  int size = map->getWidth() * map->getHeight();
  int maxDiameter = 2 * maxRadius_ + 1;
  // initialize data
  ff_ = new float[size * maxDiameter * maxDiameter];
  ffSum_ = new float[size];
  data_ = new CellData[size];
  memset(ff_, 0, sizeof(float) * size * maxDiameter * maxDiameter);
  memset(data_, 0, sizeof(CellData) * size);
  // compute form factors
  for (int y = 0; y < map->getHeight(); y++) {
    for (int x = 0; x < map->getWidth(); x++) {
      computeFormFactor(x, y);
    }
  }
}

// compute form factor of cell x,y relative to all its surrounding cells
void PhotonShader::computeFormFactor(int x, int y) {
  const int o_minx = x - maxRadius_;
  const int o_miny = y - maxRadius_;
  const int o_maxx = x + maxRadius_;
  const int o_maxy = y + maxRadius_;
  const int minx = std::max(o_minx, 0);
  const int miny = std::max(o_miny, 0);
  const int maxx = std::min(o_maxx, map_->getWidth() - 1);
  const int maxy = std::min(o_maxy, map_->getHeight() - 1);
  const int maxDiameter = 2 * maxRadius_ + 1;
  float* cellFormFactor = ff_ + (x + y * map_->getWidth()) * maxDiameter * maxDiameter;
  map_->computeFov(x, y, maxRadius_);
  const int squareRad = (maxRadius_ * maxRadius_);
  // float invRad=1.0/squareRad;
  double curFfSum = 0;
  const float offset = 1.0f / (1.0f + (float)(maxRadius_ * maxRadius_) / 20);
  const float factor = 1.0f / (1.0f - offset);
  for (int cx = minx, cdx = minx - o_minx; cx <= maxx; cx++, cdx++) {
    for (int cy = miny, cdy = miny - o_miny; cy <= maxy; cy++, cdy++) {
      if (map_->isInFov(cx, cy)) {
        const int dist = (maxRadius_ - cdx) * (maxRadius_ - cdx) + (maxRadius_ - cdy) * (maxRadius_ - cdy);
        if (dist <= squareRad) {
          // float value = (1.0f-dist*invRad) ;
          // float value =1.0f/(1.0f+(float)(dist)/20);
          float value = (1.0f / (1.0f + (float)(dist) / 20) - offset) * factor;
          curFfSum += value;
          cellFormFactor[cdx + cdy * maxDiameter] = value;
        }
      }
    }
  }
  // scale so that the sum of all form factors for cell x,y is 1.0
  ffSum_[x + y * map_->getWidth()] = (float)curFfSum;
  if (curFfSum > 1E-8) {
    curFfSum = 1.0 / curFfSum;
    for (int cx = minx, cdx = minx - o_minx; cx <= maxx; cx++, cdx++) {
      for (int cy = miny, cdy = miny - o_miny; cy <= maxy; cy++, cdy++) {
        cellFormFactor[cdx + cdy * maxDiameter] *= (float)curFfSum;
      }
    }
  }
}

void PhotonShader::computeLightContribution(
    int lx, int ly, int l_radius, const FColor& l_col, float /* reflectivity */) {
  const int o_minx = lx - l_radius;
  const int o_miny = ly - l_radius;
  const int o_maxx = lx + l_radius;
  const int o_maxy = ly + l_radius;
  const int minx = std::max(o_minx, 0);
  const int miny = std::max(o_miny, 0);
  const int maxx = std::min(o_maxx, map_->getWidth() - 1);
  const int maxy = std::min(o_maxy, map_->getHeight() - 1);
  const int maxDiameter = 2 * maxRadius_ + 1;
  const float* cellFormFactor = ff_ + (lx + ly * map_->getWidth()) * maxDiameter * maxDiameter;
  // compute the light's contribution
  static constexpr auto MIN_FACTOR = 1.0f / 255.0f;
  const int width = map_->getWidth();
  for (int y = miny, cdy = miny - o_miny; y <= maxy; y++, cdy++) {
    CellData* cellData = &data_[minx + y * width];
    const float* cellFormRow = &cellFormFactor[minx - o_minx + cdy * maxDiameter];
    for (int x = minx; x <= maxx; x++, cellData++, cellFormRow++) {
      const float cell_ff = *cellFormRow;
      if (cell_ff > MIN_FACTOR) {
        cellData->incoming.r += l_col.r * cell_ff;
        cellData->incoming.g += l_col.g * cell_ff;
        cellData->incoming.b += l_col.b * cell_ff;
      }
    }
  }
}

void PhotonShader::propagate() {
  CellData* cellData = data_;

  const int size = map_->getWidth() * map_->getHeight();
  lightsCoord_.clear();
  for (int c = 0; c < size; c++, cellData++) {
    cellData->emission.r = cellData->incoming.r * reflectivity_;
    cellData->emission.g = cellData->incoming.g * reflectivity_;
    cellData->emission.b = cellData->incoming.b * reflectivity_;
    cellData->outgoing.r += cellData->incoming.r * selfIllumination_;
    cellData->outgoing.g += cellData->incoming.g * selfIllumination_;
    cellData->outgoing.b += cellData->incoming.b * selfIllumination_;
    cellData->incoming.r = 0;
    cellData->incoming.g = 0;
    cellData->incoming.b = 0;
    if (cellData->emission.r > 0 || cellData->emission.g > 0 || cellData->emission.b > 0)
      lightsCoord_.emplace_back(c % map_->getWidth(), c / map_->getWidth());
  }
}

void PhotonShader::compute() {
  // turn off all lights
  const int size = map_->getWidth() * map_->getHeight();
  memset(data_, 0, sizeof(CellData) * size);
  // first pass. lights only
  for (const Light& l : lights_) {
    const int off = l.x + l.y * map_->getWidth();
    CellData* cellData = &data_[off];
    const float sum = ffSum_[off];
    cellData->emission.r = l.col.r * sum;
    cellData->emission.g = l.col.r * sum;
    cellData->emission.b = l.col.r * sum;
    computeLightContribution(l.x, l.y, l.radius, cellData->emission, 0.5f);
  }
  // other passes. all lit cells act as lights
  for (int pass = 1; pass < nbPass_; ++pass) {
    propagate();
    const CellData* cellData = data_;
    // for (int y=0; y < map->getHeight(); y++ ) {
    //	for (int x=0; x < map->getWidth(); x++, cellData++ ) {
    for (const Coord& c : lightsCoord_) {
      cellData = &data_[c.x + c.y * map_->getWidth()];
      computeLightContribution(c.x, c.y, maxRadius_, cellData->emission, reflectivity_);
    }
  }

  const CellData* cellData = data_;
  TCODColor* col = lightmap_;
  propagate();
  for (int c = 0; c < size; c++, cellData++, col++) {
    col->r = static_cast<uint8_t>(std::min(255.0f, cellData->outgoing.r));
    col->g = static_cast<uint8_t>(std::min(255.0f, cellData->outgoing.g));
    col->b = static_cast<uint8_t>(std::min(255.0f, cellData->outgoing.b));
  }
}
