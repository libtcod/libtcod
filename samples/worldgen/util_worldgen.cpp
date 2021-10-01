/*
 * Copyright (c) 2009 Jice
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

// world generator
// this was mostly generated with libtcod 1.4.2 heightmap tool !

#include <SDL.h>

#include <array>
#include <cmath>
#include <cstdio>
#include <vector>

#include "main.hpp"

// Return the current time in seconds ata decent resolution.
static float get_time() { return static_cast<float>(SDL_GetPerformanceCounter()) / SDL_GetPerformanceFrequency(); }

static constexpr auto WHITE = TCODColor{255, 255, 255};

// temperature / precipitation Biome diagram (Whittaker diagram)
static constexpr EBiome biomeDiagram[5][5] = {
    // arctic/alpine climate (below -5°C)
    {
        TUNDRA,
        TUNDRA,
        TUNDRA,
        TUNDRA,
        TUNDRA,
    },
    // cold climate (-5 / 5 °C)
    {
        COLD_DESERT,
        GRASSLAND,
        BOREAL_FOREST,
        BOREAL_FOREST,
        BOREAL_FOREST,
    },
    // temperate climate (5 / 15 °C)
    {
        COLD_DESERT,
        GRASSLAND,
        TEMPERATE_FOREST,
        TEMPERATE_FOREST,
        TROPICAL_MONTANE_FOREST,
    },
    // warm climate (15 - 20°C)
    {
        HOT_DESERT,
        SAVANNA,
        TROPICAL_DRY_FOREST,
        TROPICAL_EVERGREEN_FOREST,
        TROPICAL_EVERGREEN_FOREST,
    },
    // tropical climate (above 20 °C)
    {
        HOT_DESERT,
        THORN_FOREST,
        TROPICAL_DRY_FOREST,
        TROPICAL_EVERGREEN_FOREST,
        TROPICAL_EVERGREEN_FOREST,
    },
};

static constexpr float sandHeight = 0.12f;
static constexpr float grassHeight = 0.16f;  // 0.315f;
static constexpr float rockHeight = 0.655f;
static constexpr float snowHeight = 0.905f;  // 0.785f;
// TCOD's land color map
static constexpr int MAX_COLOR_KEY = 10;
static constexpr int COLOR_KEY_MAX_SEA = static_cast<int>(sandHeight * 255) - 1;
static constexpr int COLOR_KEY_MIN_LAND = static_cast<int>(sandHeight * 255);
static constexpr int keyIndex[MAX_COLOR_KEY] = {
    0,
    COLOR_KEY_MAX_SEA,
    COLOR_KEY_MIN_LAND,
    static_cast<int>(grassHeight * 255),
    static_cast<int>(grassHeight * 255) + 10,
    static_cast<int>(rockHeight * 255),
    static_cast<int>(rockHeight * 255) + 10,
    static_cast<int>(snowHeight * 255),
    static_cast<int>(snowHeight * 255) + 10,
    255};
static constexpr TCODColor keyColor[MAX_COLOR_KEY] = {
    TCODColor(0, 0, 50),  // deep water
    TCODColor(20, 20, 200),  // water-sand transition
    TCODColor(134, 180, 101),  // sand
    TCODColor(80, 120, 10),  // sand-grass transition
    TCODColor(17, 109, 7),  // grass
    TCODColor(30, 85, 12),  // grass-rock transistion
    TCODColor(64, 70, 20),  // rock
    TCODColor(120, 140, 40),  // rock-snow transistion
    TCODColor(208, 208, 239),  // snow
    TCODColor(255, 255, 255)};

// altitude color map
static constexpr int MAX_ALT_KEY = 8;
static constexpr std::array<int, MAX_ALT_KEY> altIndexes{
    0, 15, (int)(sandHeight * 255), (int)(sandHeight * 255) + 1, 80, 130, 195, 255};
static constexpr std::array<float, MAX_ALT_KEY> altitudes{
    -2000, -1000, -100, 0, 500, 1000, 2500, 4000  // in meters
};
static constexpr std::array<TCODColor, MAX_ALT_KEY> altColors{
    TCODColor(24, 165, 255),  // -2000
    TCODColor(132, 214, 255),  // -1000
    TCODColor(247, 255, 255),  // -100
    TCODColor(49, 149, 44),  // 0
    TCODColor(249, 209, 151),  // 500
    TCODColor(165, 148, 24),  // 1000
    TCODColor(153, 110, 6),  // 2500
    TCODColor(172, 141, 138),  // 4000
};

// precipitation color map
static constexpr int MAX_PREC_KEY = 19;
static constexpr std::array<int, MAX_PREC_KEY> precIndexes{
    4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 50, 60, 70, 80, 100, 120, 140, 160, 255};
static constexpr std::array<float, MAX_PREC_KEY> precipitations{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 13, 15, 18, 20, 25, 30, 35, 40  // cm / m² / year
};
static constexpr std::array<TCODColor, MAX_PREC_KEY> precColors{
    TCODColor(128, 0, 0),  // < 4
    TCODColor(173, 55, 0),  // 4-8
    TCODColor(227, 102, 0),  // 8-12
    TCODColor(255, 149, 0),  // 12-16
    TCODColor(255, 200, 0),  // 16-20
    TCODColor(255, 251, 0),  // 20-24
    TCODColor(191, 255, 0),  // 24-28
    TCODColor(106, 251, 0),  // 28-32
    TCODColor(25, 255, 48),  // 32-36
    TCODColor(48, 255, 141),  // 36-40
    TCODColor(28, 255, 232),  // 40-50
    TCODColor(54, 181, 255),  // 50-60
    TCODColor(41, 71, 191),  // 60-70
    TCODColor(38, 0, 255),  // 70-80
    TCODColor(140, 0, 255),  // 80-100
    TCODColor(221, 0, 255),  // 100-120
    TCODColor(255, 87, 255),  // 120-140
    TCODColor(255, 173, 255),  // 140-160
    TCODColor(255, 206, 255),  // > 160
};

// temperature color map
static constexpr int MAX_TEMP_KEY = 7;
static constexpr int tempIndexes[MAX_TEMP_KEY] = {0, 42, 84, 126, 168, 210, 255};
static constexpr int temperatures[MAX_TEMP_KEY] = {-30, -20, -10, 0, 10, 20, 30};
static constexpr TCODColor tempKeyColor[MAX_TEMP_KEY] = {
    TCODColor(180, 8, 130),  // -30 °C
    TCODColor(32, 1, 139),  // -20 °C
    TCODColor(0, 65, 252),  // -10 °C
    TCODColor(37, 255, 236),  // 0 °C
    TCODColor(255, 255, 1),  // 10 °C
    TCODColor(255, 29, 4),  // 20 °C
    TCODColor(80, 3, 0),  // 30 °C
};

int WorldGenerator::getWidth() const { return HM_WIDTH; }

int WorldGenerator::getHeight() const { return HM_HEIGHT; }

float WorldGenerator::getAltitude(int x, int y) const { return hm->getValue(x, y); }

float WorldGenerator::getRealAltitude(float x, float y) const {
  int ih = (int)(256 * getInterpolatedAltitude(x, y));
  int idx;
  ih = CLAMP(0, 255, ih);
  for (idx = 0; idx < MAX_ALT_KEY - 1; ++idx) {
    if (altIndexes.at(idx + 1) > ih) break;
  }
  return altitudes.at(idx) + (altitudes.at(idx + 1) - altitudes.at(idx)) * (ih - altIndexes.at(idx)) /
                                 (altIndexes.at(idx + 1) - altIndexes.at(idx));
}

float WorldGenerator::getPrecipitations(float x, float y) const {
  int i_prec = (int)(256 * precipitation->getValue((int)x, (int)y));
  int idx;
  i_prec = CLAMP(0, 255, i_prec);
  for (idx = 0; idx < MAX_PREC_KEY - 1; ++idx) {
    if (precIndexes.at(idx + 1) > i_prec) break;
  }
  return precipitations.at(idx) + (precipitations.at(idx + 1) - precipitations.at(idx)) *
                                      (i_prec - precIndexes.at(idx)) / (precIndexes.at(idx + 1) - precIndexes.at(idx));
}

float WorldGenerator::getTemperature(float x, float y) const { return temperature->getValue((int)x, (int)y); }

EBiome WorldGenerator::getBiome(float x, float y) const { return biomeMap[(int)x + (int)y * HM_WIDTH]; }

float WorldGenerator::getInterpolatedAltitude(float x, float y) const { return hm->getInterpolatedValue(x, y); }

void WorldGenerator::getInterpolatedNormal(float x, float y, float n[3]) const {
  return hm2->getNormal(x, y, n, sandHeight);
}

float WorldGenerator::getSandHeight() const { return sandHeight; }

bool WorldGenerator::isOnSea(float x, float y) const { return getInterpolatedAltitude(x, y) <= sandHeight; }

void WorldGenerator::addHill(int nbHill, float baseRadius, float radiusVar, float height) {
  for (int i = 0; i < nbHill; ++i) {
    const float hillMinRadius = baseRadius * (1.0f - radiusVar);
    const float hillMaxRadius = baseRadius * (1.0f + radiusVar);
    const float radius = wgRng->getFloat(hillMinRadius, hillMaxRadius);
    const int xh = wgRng->getInt(0, HM_WIDTH - 1);
    const int yh = wgRng->getInt(0, HM_HEIGHT - 1);
    hm->addHill(static_cast<float>(xh), static_cast<float>(yh), radius, height);
  }
}

void WorldGenerator::setLandMass(float landMass, float waterLevel) {
  // fix land mass. We want a proportion of landMass above sea level
#ifndef NDEBUG
  float t0 = get_time();
#endif
  int heightcount[256] = {0};
  for (int y = 0; y < HM_HEIGHT; ++y) {
    for (int x = 0; x < HM_WIDTH; ++x) {
      const float h = hm->getValue(x, y);
      int ih = (int)(h * 255);
      ih = CLAMP(0, 255, ih);
      heightcount[ih]++;
    }
  }
  int i = 0, totalcount = 0;
  while (totalcount < HM_WIDTH * HM_HEIGHT * (1.0f - landMass)) {
    totalcount += heightcount[i];
    i++;
  }
  const float newWaterLevel = i / 255.0f;
  const float landCoef = (1.0f - waterLevel) / (1.0f - newWaterLevel);
  const float waterCoef = waterLevel / newWaterLevel;
  // water level should be raised/lowered to newWaterLevel
  for (int y = 0; y < HM_HEIGHT; ++y) {
    for (int x = 0; x < HM_WIDTH; ++x) {
      float h = hm->getValue(x, y);
      if (h > newWaterLevel) {
        h = waterLevel + (h - newWaterLevel) * landCoef;
      } else {
        h = h * waterCoef;
      }
      hm->setValue(x, y, h);
    }
  }
#ifndef NDEBUG
  float t1 = get_time();
  DBG(("  Landmass... %g\n", t1 - t0));
#endif
}

// function building the heightmap
void WorldGenerator::buildBaseMap() {
  float t0 = get_time();
  addHill(600, 16.0 * HM_WIDTH / 200, 0.7f, 0.3f);
  hm->normalize();
  float t1 = get_time();
  DBG(("  Hills... %g\n", t1 - t0));
  t0 = t1;

  hm->addFbm(noise, 2.20f * HM_WIDTH / 400, 2.20f * HM_WIDTH / 400, 0, 0, 10.0f, 1.0f, 2.05f);
  hm->normalize();
  hm2->copy(hm);
  t1 = get_time();
  DBG(("  Fbm... %g\n", t1 - t0));
  t0 = t1;

  setLandMass(0.6f, sandHeight);

  // fix land/mountain ratio using x^3 curve above sea level
  for (int y = 0; y < HM_HEIGHT; ++y) {
    for (int x = 0; x < HM_WIDTH; ++x) {
      float h = hm->getValue(x, y);
      if (h >= sandHeight) {
        float coef = (h - sandHeight) / (1.0f - sandHeight);
        h = sandHeight + coef * coef * coef * (1.0f - sandHeight);
        hm->setValue(x, y, h);
      }
    }
  }
  t1 = get_time();
  DBG(("  Flatten plains... %g\n", t1 - t0));
  t0 = t1;

  // we use a custom erosion algo
  // hm->rainErosion(15000*HM_WIDTH/400,0.03,0.01,wgRng);
  // t1=get_time();
  // DBG(("  Erosion... %g\n", t1-t0 ));
  // t0=t1;
  // compute clouds
  float f[2];
  for (int x = 0; x < HM_WIDTH; x++) {
    f[0] = 6.0f * ((float)(x) / HM_WIDTH);
    for (int y = 0; y < HM_HEIGHT; y++) {
      f[1] = 6.0f * ((float)(y) / HM_HEIGHT);
      // clouds[x][y] = 0.5f * (1.0f + 0.8f * noise->getFbmSimplex(f,4.0f));
      clouds[x][y] = 0.5f * (1.0f + 0.8f * noise->getFbm(f, 4.0f, TCOD_NOISE_SIMPLEX));
    }
  }
  t1 = get_time();
  DBG(("  Init clouds... %g\n", t1 - t0));
  t0 = t1;
}

// function blurring the heightmap
void WorldGenerator::smoothMap() {
  // 3x3 kernel for smoothing operations
  static const int smoothKernelSize = 9;
  static const int smoothKernelDx[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
  static const int smoothKernelDy[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
  static const float smoothKernelWeight[9] = {2, 8, 2, 8, 20, 8, 2, 8, 2};

#ifndef NDEBUG
  const float t0 = get_time();
#endif
  hm->kernelTransform(smoothKernelSize, smoothKernelDx, smoothKernelDy, smoothKernelWeight, -1000, 1000);
  hm2->kernelTransform(smoothKernelSize, smoothKernelDx, smoothKernelDy, smoothKernelWeight, -1000, 1000);
  hm->normalize();
#ifndef NDEBUG
  const float t1 = get_time();
  DBG(("  Blur... %g\n", t1 - t0));
#endif
}

static constexpr int8_t DIR_X[9] = {0, -1, 0, 1, -1, 1, -1, 0, 1};
static constexpr int8_t DIR_Y[9] = {0, -1, -1, -1, 0, 0, 1, 1, 1};
static constexpr float dir_coef[9] = {
    1.0f, 1.0f / 1.414f, 1.0f, 1.0f / 1.414f, 1.0f, 1.0f, 1.0f / 1.414f, 1.0f, 1.0f / 1.414f};
static constexpr uint8_t opposite_dir[9] = {0, 8, 7, 6, 5, 4, 3, 2, 1};

// erosion parameters
static constexpr auto EROSION_FACTOR = 0.01f;
static constexpr auto SEDIMENTATION_FACTOR = 0.01f;
static constexpr auto MAX_EROSION_ALT = 0.9f;
static constexpr auto MUDSLIDE_COEF = 0.4f;

void WorldGenerator::erodeMap() {
  TCODHeightMap newMap(HM_WIDTH, HM_HEIGHT);
  for (int i = 5; i != 0; --i) {
    // compute flow and slope maps
    map_data_t* md = mapData;
    for (int y = 0; y < HM_HEIGHT; ++y) {
      for (int x = 0; x < HM_WIDTH; ++x) {
        const float h = hm->getValue(x, y);
        float h_min = h, h_max = h;
        uint8_t minDir = 0;
        uint8_t maxDir = 0;
        for (uint8_t j = 1; j < 9; ++j) {
          const int ix = x + DIR_X[j];
          const int iy = y + DIR_Y[j];
          if (IN_RECTANGLE(ix, iy, HM_WIDTH, HM_HEIGHT)) {
            float h2 = hm->getValue(ix, iy);
            if (h2 < h_min) {
              h_min = h2;
              minDir = j;
            } else if (h2 > h_max) {
              h_max = h2;
              maxDir = j;
            }
          }
        }
        md->flowDir = minDir;
        md->upDir = maxDir;
        float slope = h_min - h;  // this is negative
        slope *= dir_coef[minDir];
        md->slope = slope;
        md++;
      }
    }

    md = mapData;
    for (int y = 0; y < HM_HEIGHT; ++y) {
      for (int x = 0; x < HM_WIDTH; ++x) {
        float sediment = 0.0f;
        bool end = false;
        int ix = x, iy = y;
        uint8_t oldFlow = md->flowDir;
        const map_data_t* md2 = md;
        while (!end) {
          float h = hm->getValue(ix, iy);
          if (h < sandHeight - 0.01f) break;
          if (md2->flowDir == opposite_dir[oldFlow]) {
            h += SEDIMENTATION_FACTOR * sediment;
            hm->setValue(ix, iy, h);
            end = true;
          } else {
            // remember, slope is negative
            h += precipitation->getValue(ix, iy) * EROSION_FACTOR * md2->slope;
            h = MAX(h, sandHeight);
            sediment -= md2->slope;
            hm->setValue(ix, iy, h);
            oldFlow = md2->flowDir;
            ix += DIR_X[oldFlow];
            iy += DIR_Y[oldFlow];
            md2 = &mapData[ix + iy * HM_WIDTH];
          }
        }
        md++;
      }
    }
    DBG(("  Erosion pass %d\n", i));

    // mudslides (smoothing)
    const float sandCoef = 1.0f / (1.0f - sandHeight);
    for (int y = 0; y < HM_HEIGHT; ++y) {
      for (int x = 0; x < HM_WIDTH; ++x) {
        const float h = hm->getValue(x, y);
        if (h < sandHeight - 0.01f || h >= MAX_EROSION_ALT) {
          newMap.setValue(x, y, h);
          continue;
        }
        float sumDelta1 = 0.0f, sumDelta2 = 0.0f;
        int nb1 = 1, nb2 = 1;
        for (int j = 1; j < 9; j++) {
          const int ix = x + DIR_X[j];
          const int iy = y + DIR_Y[j];
          if (IN_RECTANGLE(ix, iy, HM_WIDTH, HM_HEIGHT)) {
            const float ih = hm->getValue(ix, iy);
            if (ih < h) {
              if (j == 1 || j == 3 || j == 6 || j == 8) {
                // diagonal neighbour
                sumDelta1 += (ih - h) * 0.4f;
                nb1++;
              } else {
                // adjacent neighbour
                sumDelta2 += (ih - h) * 1.6f;
                nb2++;
              }
            }
          }
        }
        // average height difference with lower neighbors
        float dh = sumDelta1 / nb1 + sumDelta2 / nb2;
        dh *= MUDSLIDE_COEF;
        const float h_coef = (h - sandHeight) * sandCoef;
        dh *= (1.0f - h_coef * h_coef * h_coef);  // less smoothing at high altitudes

        newMap.setValue(x, y, h + dh);
      }
    }
    hm->copy(&newMap);
  }
}

// interpolated cloud thickness
float WorldGenerator::getCloudThickness(float x, float y) const {
  x += cloudDx;
  const int ix = static_cast<int>(x);
  const int iy = static_cast<int>(y);
  const int ix1 = std::min(HM_WIDTH - 1, ix + 1);
  const int iy1 = std::min(HM_HEIGHT - 1, iy + 1);
  const float fdx = x - ix;
  const float fdy = y - iy;
  const float v1 = clouds[ix][iy];
  const float v2 = clouds[ix1][iy];
  const float v3 = clouds[ix][iy1];
  const float v4 = clouds[ix1][iy1];
  const float vx1 = ((1.0f - fdx) * v1 + fdx * v2);
  const float vx2 = ((1.0f - fdx) * v3 + fdx * v4);
  const float v = ((1.0f - fdy) * vx1 + fdy * vx2);
  return v;
}

TCODColor WorldGenerator::getMapColor(float h) {
  int colorIdx;
  if (h < sandHeight)
    colorIdx = (int)(h / sandHeight * COLOR_KEY_MAX_SEA);
  else
    colorIdx = COLOR_KEY_MIN_LAND + (int)((h - sandHeight) / (1.0f - sandHeight) * (255 - COLOR_KEY_MIN_LAND));
  colorIdx = CLAMP(0, 255, colorIdx);
  return mapGradient[colorIdx];
}

void WorldGenerator::computeSunLight(const float lightDir[3]) {
  for (int y = 0; y < HM_HEIGHT; y++) {
    for (int x = 0; x < HM_WIDTH; x++) {
      worldint[x + y * HM_WIDTH] = getMapIntensity(x + 0.5f, y + 0.5f, lightDir);
    }
  }
}

float WorldGenerator::getMapIntensity(float worldX, float worldY, const float lightDir[3]) {
  // sun color & direction
  static constexpr TCODColor sunCol(255, 255, 160);
  float normal[3];
  const float wx = CLAMP(0.0f, HM_WIDTH - 1, worldX);
  const float wy = CLAMP(0.0f, HM_HEIGHT - 1, worldY);
  // apply sun light
  getInterpolatedNormal(wx, wy, normal);
  normal[2] *= 3.0f;
  float intensity = 0.75f - (normal[0] * lightDir[0] + normal[1] * lightDir[1] + normal[2] * lightDir[2]) * 0.75f;
  intensity = CLAMP(0.75f, 1.5f, intensity);
  return intensity;
}

TCODColor WorldGenerator::getInterpolatedColor(float worldX, float worldY) {
  return getInterpolatedColor(worldmap, worldX, worldY);
}

TCODColor WorldGenerator::getInterpolatedColor(TCODImage* img, float x, float y) {
  int w, h;
  img->getSize(&w, &h);
  const float wx = CLAMP(0.0f, w - 1, x);
  const float wy = CLAMP(0.0f, h - 1, y);
  const int iwx = (int)wx;
  const int iwy = (int)wy;
  const float dx = wx - iwx;
  const float dy = wy - iwy;

  const TCODColor colNW = img->getPixel(iwx, iwy);
  const TCODColor colNE = (iwx < w - 1 ? img->getPixel(iwx + 1, iwy) : colNW);
  const TCODColor colSW = (iwy < h - 1 ? img->getPixel(iwx, iwy + 1) : colNW);
  const TCODColor colSE = (iwx < w - 1 && iwy < h - 1 ? img->getPixel(iwx + 1, iwy + 1) : colNW);
  const TCODColor colN = TCODColor::lerp(colNW, colNE, dx);
  const TCODColor colS = TCODColor::lerp(colSW, colSE, dx);
  const TCODColor col = TCODColor::lerp(colN, colS, dy);
  return col;
}

float WorldGenerator::getInterpolatedIntensity(float worldX, float worldY) {
  return getInterpolatedFloat(worldint, worldX, worldY, HM_WIDTH, HM_HEIGHT);
}

void WorldGenerator::updateClouds(float elapsedTime) {
  cloudTotalDx += elapsedTime * 5;
  cloudDx += elapsedTime * 5;
  if (cloudDx >= 1.0f) {
    const int colsToTranslate = static_cast<int>(cloudDx);
    cloudDx -= colsToTranslate;
    // translate the cloud map
    for (int y = 0; y < HM_HEIGHT; ++y) {
      for (int x = colsToTranslate; x < HM_WIDTH; ++x) {
        clouds[x - colsToTranslate][y] = clouds[x][y];
      }
    }
    // compute a new column
    const float cdx = std::floor(cloudTotalDx);
    for (int y = 0; y < HM_HEIGHT; ++y) {
      for (int x = HM_WIDTH - colsToTranslate; x < HM_WIDTH; ++x) {
        const float f[2] = {
            6.0f * ((float)(x + cdx) / HM_WIDTH),
            6.0f * ((float)(y) / HM_HEIGHT),
        };
        // clouds[x][y] = 0.5f * (1.0f + 0.8f * noise->getFbmSimplex(f,4.0f));
        clouds[x][y] = 0.5f * (1.0f + 0.8f * noise->getFbm(f, 4.0f, TCOD_NOISE_SIMPLEX));
      }
    }
  }
}

class RiverPathCbk : public ITCODPathCallback {
 public:
  float getWalkCost(int xFrom, int yFrom, int xTo, int yTo, void* userData) const {
    WorldGenerator* world = (WorldGenerator*)userData;
    const float h1 = world->hm->getValue(xFrom, yFrom);
    const float h2 = world->hm->getValue(xTo, yTo);
    if (h2 < sandHeight) return 0.0f;
    //        float f[2] = {xFrom*10.0f/HM_WIDTH,yFrom*10.0f/HM_HEIGHT};
    //        return (1.0f+h2-h1)*10+5*(1.0f+noise2d.getSimplex(f));
    return (0.01f + h2 - h1) * 100;
  }
};

/*
void WorldGenerator::generateRivers() {
    static int riverId=0;
    static RiverPathCbk cbk;
//    static TCODPath *path=NULL;
    static TCODDijkstra *path=NULL;
        // the source
        int sx,sy;
        // the destination
        int dx=-1,dy=-1;
        // get a random point near the coast
        sx = wgRng->getInt(0,HM_WIDTH-1);
        sy = wgRng->getInt(0,HM_HEIGHT-1);
        float h = hm->getValue(sx,sy);
        while ( h <  sandHeight - 0.02 || h >= sandHeight ) {
                sx++;
                if ( sx == HM_WIDTH ) {
                        sx=0;
                        sy++;
                        if ( sy == HM_HEIGHT ) sy=0;
                }
                h = hm->getValue(sx,sy);
        }
        riverId++;
        // get a closes mountain point
        float minDist=1E10f;
        int minx = sx - HM_WIDTH/4;
        int maxx = sx + HM_WIDTH/4;
        int miny = sy - HM_HEIGHT/4;
        int maxy = sy + HM_HEIGHT/4;
        minx = MAX(0,minx);
        maxx = MIN(HM_WIDTH-1,maxx);
        miny = MAX(0,miny);
        maxy = MIN(HM_HEIGHT-1,maxy);
        h = MIN(snowHeight,h + wgRng->getFloat(0.1f,0.5f));
        for (int y=miny; y < maxy; y++) {
        for (int x=minx; x < maxx; x++) {
            float dh=hm->getValue(x,y);
            if ( dh >= h ) {
                dx=x;
                dy=y;
                break;
            }
        }
        }

        if (! path) {
//	    path = new TCODPath(HM_WIDTH,HM_HEIGHT,&cbk,this);
            path = new TCODDijkstra(HM_WIDTH,HM_HEIGHT,&cbk,this);
        }
        path->compute(dx,dy);
//	if ( dx >= 0 && path->compute(dx,dy,sx,sy) ) {
        if ( dx >= 0 ) { path->setPath(sx,sy) ;
        DBG( ("river : %d %d -> %d %d\n",sx,sy,dx,dy));
        int x,y;
            while (path->walk(&x,&y)) {
            map_data_t *md=&mapData[x+y*HM_WIDTH];
            if ( md->riverId != 0 ) break;
            md->riverId = riverId;
            }
        }

}
*/

void WorldGenerator::generateRivers() {
  static uint8_t riverId = 0;
  // the source
  int sx = wgRng->getInt(0, HM_WIDTH - 1);
  int sy = wgRng->getInt(HM_HEIGHT / 5, 4 * HM_HEIGHT / 5);
  // get a random point near the coast
  float h = hm->getValue(sx, sy);
  while (h < sandHeight - 0.02 || h >= sandHeight) {
    ++sx;
    if (sx == HM_WIDTH) {
      sx = 0;
      ++sy;
      if (sy == HM_HEIGHT) sy = 0;
    }
    h = hm->getValue(sx, sy);
  }
  std::vector<int> tree;
  std::vector<int> randPt;
  tree.push_back(sx + sy * HM_WIDTH);
  ++riverId;
  // the destination
  const int dx = sx;
  const int dy = sy;
  for (int i = 0; i < wgRng->getInt(50, 200); ++i) {
    const int rx = wgRng->getInt(sx - 200, sx + 200);
    const int ry = wgRng->getInt(sy - 200, sy + 200);
    //	    if ( IN_RECTANGLE(rx,ry,HM_WIDTH,HM_HEIGHT) ) {
    //	        float h=hm->getValue(rx,ry);
    //	        if ( h >= sandHeight ) {
    randPt.push_back(rx + ry * HM_WIDTH);
    //	        }
    //	    }
  }
  for (size_t i = 0; i < randPt.size(); ++i) {
    const int rx = randPt.at(i) % HM_WIDTH;
    const int ry = randPt.at(i) / HM_WIDTH;

    float minDist = 1E10;
    int best_x = -1, best_y = -1;
    for (size_t j = 0; j < tree.size(); ++j) {
      const int tx = tree.at(j) % HM_WIDTH;
      const int ty = tree.at(j) / HM_WIDTH;
      const float dist = static_cast<float>((tx - rx) * (tx - rx) + (ty - ry) * (ty - ry));
      if (dist < minDist) {
        minDist = dist;
        best_x = tx;
        best_y = ty;
      }
    }
    TCODLine::init(best_x, best_y, rx, ry);
    int len = 3;
    int cx = best_x;
    int cy = best_y;
    map_data_t* md = &mapData[cx + cy * HM_WIDTH];
    if (md->riverId == riverId) md->riverId = 0;
    do {
      md = &mapData[cx + cy * HM_WIDTH];
      if (md->riverId > 0) return;
      if (hm->getValue(cx, cy) >= sandHeight) {
        md->riverId = riverId;
        precipitation->setValue(cx, cy, 1.0f);
      }
      if (cx == 0 || cx == HM_WIDTH - 1 || cy == 0 || cy == HM_HEIGHT - 1)
        len = 0;
      else if (TCODLine::step(&cx, &cy))
        len = 0;
      len--;
    } while (len > 0);
    const int newNode = cx + cy * HM_WIDTH;
    if (newNode != best_x + best_y * HM_WIDTH) {
      tree.push_back(newNode);
    }
  }
}

/*
void WorldGenerator::generateRivers() {
    static int riverId=0;
        // the source
        int sx,sy;
        // the destination
        int dx,dy;
        // get a random point near the coast
        sx = wgRng->getInt(0,HM_WIDTH-1);
        sy = wgRng->getInt(HM_HEIGHT/5,4*HM_HEIGHT/5);
        float h = hm->getValue(sx,sy);
        map_data_t *md=&mapData[sx+sy*HM_WIDTH];
        while ( md->riverId == 0 && (h <  sandHeight - 0.02 || h >= sandHeight) ) {
                sx++;
                if ( sx == HM_WIDTH ) {
                        sx=0;
                        sy++;
                        if ( sy == HM_HEIGHT ) sy=0;
                }
                h = hm->getValue(sx,sy);
                md=&mapData[sx+sy*HM_WIDTH];
        }
        riverId++;
        dx = sx;
        dy = sy;
        DBG( ("source : %d %d\n",sx,sy));
        // travel down to the see
        // get the highest point around current position
        bool deadEnd=false;
        int len=0;
        river_t *river=new river_t();
        rivers.push(river);
        int maxlen=HM_WIDTH,last_dx=1,last_dy=1;
        do {
        int coord = sx + sy*HM_WIDTH;
            map_data_t *md=&mapData[coord];
            if ( md->riverId != 0 ) {
                river_t *joined = rivers.get(md->riverId-1);
                int i=0;
                while (joined->coords.get(i) != coord ) i++;
                while ( i < joined->coords.size() ) {
                    int newStrength=joined->strength.get(i)+1;
                    joined->strength.set(newStrength,i);
                    i++;
                }
                break;
            }
        md->riverId = riverId;
        md->riverLength = len++;
        river->coords.push(coord);
        river->strength.push(1);
                if ( md->upDir != 0 ) {
                    last_dx=dir_x[md->upDir];
                        sx += last_dx;
                        last_dy=dir_y[md->upDir];
                        sy += last_dy;
                        deadEnd=false;
                } else if ( deadEnd ) {
                    break;
                } else {
                        sx += last_dx;
                        sy += last_dy;
                        if ( ! IN_RECTANGLE(sx,sy,HM_WIDTH,HM_HEIGHT ) ) break;
                        deadEnd=true;
                }
                h=hm->getValue(sx,sy);
                maxlen--;
        } while ( maxlen > 0 && h <= snowHeight);

}
*/

EClimate WorldGenerator::getClimateFromTemp(float temp) {
  if (temp <= -5) return ARCTIC_ALPINE;
  if (temp <= 5) return COLD;
  if (temp <= 15) return TEMPERATE;
  if (temp <= 20) return WARM;
  return TROPICAL;
}

float WorldGenerator::getInterpolatedFloat(const float* arr, float x, float y, int width, int height) {
  const float wx = CLAMP(0.0f, width - 1, x);
  const float wy = CLAMP(0.0f, height - 1, y);
  const int iwx = static_cast<int>(wx);
  const int iwy = static_cast<int>(wy);
  const float dx = wx - iwx;
  const float dy = wy - iwy;

  const float iNW = arr[iwx + iwy * width];
  const float iNE = (iwx < width - 1 ? arr[iwx + 1 + iwy * width] : iNW);
  const float iSW = (iwy < height - 1 ? arr[iwx + (iwy + 1) * width] : iNW);
  const float iSE = (iwx < width - 1 && iwy < height - 1 ? arr[iwx + 1 + (iwy + 1) * width] : iNW);
  const float iN = (1.0f - dx) * iNW + dx * iNE;
  const float iS = (1.0f - dx) * iSW + dx * iSE;
  return (1.0f - dy) * iN + dy * iS;
}

int WorldGenerator::getRiverStrength(int riverId) {
  // river_t *river = rivers.get(riverId-1);
  // return river->strength.get(river->strength.size()-1);
  return 2;
}

void WorldGenerator::computePrecipitations() {
  static constexpr float waterAdd = 0.03f;
  static constexpr float slopeCoef = 2.0f;
  static constexpr float basePrecip = 0.01f;  // precipitation coef when slope == 0
  float t0 = get_time();
  // north/south winds
  for (int dir_y = -1; dir_y <= 1; dir_y += 2) {
    for (int x = 0; x < HM_WIDTH; ++x) {
      const float noise_x = (float)(x)*5 / HM_WIDTH;
      // float waterAmount=(1.0f+noise1d.getFbmSimplex(&noise_x,3.0f));
      float waterAmount = (1.0f + noise1d.getFbm(&noise_x, 3.0f, TCOD_NOISE_SIMPLEX));
      const int start_y = (dir_y == -1 ? HM_HEIGHT - 1 : 0);
      const int end_y = (dir_y == -1 ? -1 : HM_HEIGHT);
      for (int y = start_y; y != end_y; y += dir_y) {
        const float h = hm->getValue(x, y);
        if (h < sandHeight) {
          waterAmount += waterAdd;
        } else if (waterAmount > 0.0f) {
          float slope;
          if ((unsigned)(y + dir_y) < (unsigned)HM_HEIGHT)
            slope = hm->getValue(x, y + dir_y) - h;
          else
            slope = h - hm->getValue(x, y - dir_y);
          if (slope >= 0.0f) {
            const float precip = waterAmount * (basePrecip + slope * slopeCoef);
            precipitation->setValue(x, y, precipitation->getValue(x, y) + precip);
            waterAmount -= precip;
            waterAmount = MAX(0.0f, waterAmount);
          }
        }
      }
    }
  }
  float t1 = get_time();
  DBG(("  North/south winds... %g\n", t1 - t0));
  t0 = t1;

  // east/west winds
  for (int dir_x = -1; dir_x <= 1; dir_x += 2) {
    for (int y = 0; y < HM_HEIGHT; ++y) {
      const float noise_y = (float)(y)*5 / HM_HEIGHT;
      // float waterAmount=(1.0f+noise1d.getFbmSimplex(&noise_y,3.0f));
      float waterAmount = (1.0f + noise1d.getFbm(&noise_y, 3.0f, TCOD_NOISE_SIMPLEX));
      const int start_x = (dir_x == -1 ? HM_WIDTH - 1 : 0);
      const int end_x = (dir_x == -1 ? -1 : HM_WIDTH);
      for (int x = start_x; x != end_x; x += dir_x) {
        const float h = hm->getValue(x, y);
        if (h < sandHeight) {
          waterAmount += waterAdd;
        } else if (waterAmount > 0.0f) {
          float slope;
          if ((unsigned)(x + dir_x) < (unsigned)HM_WIDTH)
            slope = hm->getValue(x + dir_x, y) - h;
          else
            slope = h - hm->getValue(x - dir_x, y);
          if (slope >= 0.0f) {
            const float precip = waterAmount * (basePrecip + slope * slopeCoef);
            precipitation->setValue(x, y, precipitation->getValue(x, y) + precip);
            waterAmount -= precip;
            waterAmount = MAX(0.0f, waterAmount);
          }
        }
      }
    }
  }
  t1 = get_time();
  DBG(("  East/west winds... %g\n", t1 - t0));
  t0 = t1;

  float min, max;
  precipitation->getMinMax(&min, &max);

  // latitude impact
  for (int y = HM_HEIGHT / 4; y < 3 * HM_HEIGHT / 4; ++y) {
    // latitude (0 : equator, -1/1 : pole)
    const float lat = (float)(y - HM_HEIGHT / 4) * 2 / HM_HEIGHT;
    const float coef = sinf(2 * 3.1415926f * lat);
    for (int x = 0; x < HM_WIDTH; x++) {
      const float f[2] = {(float)(x) / HM_WIDTH, (float)(y) / HM_HEIGHT};
      // float x_coef = coef + 0.5f*noise2d.getFbmSimplex(f,3.0f);
      const float x_coef = coef + 0.5f * noise2d.getFbm(f, 3.0f, TCOD_NOISE_SIMPLEX);
      float precip = precipitation->getValue(x, y);
      precip += (max - min) * x_coef * 0.1f;
      precipitation->setValue(x, y, precip);
    }
  }
  t1 = get_time();
  DBG(("  latitude... %g\n", t1 - t0));
  t0 = t1;

  // very fast blur by scaling down and up
  static constexpr int factor = 8;
  static constexpr int smallWidth = (HM_WIDTH + factor - 1) / factor;
  static constexpr int smallHeight = (HM_HEIGHT + factor - 1) / factor;
  auto lowResMap = std::array<float, smallWidth * smallHeight>{};
  for (int x = 0; x < HM_WIDTH; x++) {
    for (int y = 0; y < HM_HEIGHT; y++) {
      const float v = precipitation->getValue(x, y);
      const int ix = x / factor;
      const int iy = y / factor;
      lowResMap[ix + iy * smallWidth] += v;
    }
  }
  static constexpr float coef = 1.0f / factor;
  for (int y = 0; y < HM_HEIGHT; ++y) {
    for (int x = 0; x < HM_WIDTH; ++x) {
      const float v = getInterpolatedFloat(lowResMap.data(), x * coef, y * coef, smallWidth, smallHeight);
      precipitation->setValue(x, y, v);
    }
  }
}

void WorldGenerator::smoothPrecipitations() {
  float t0 = get_time();

  // better quality polishing blur using a 5x5 kernel
  // faster than TCODHeightmap kernelTransform function
  TCODHeightMap temp_hm(HM_WIDTH, HM_HEIGHT);
  temp_hm.copy(precipitation);
  for (int i = 4; i != 0; --i) {
    for (int x = 0; x < HM_WIDTH; ++x) {
      int minx = x - 2;
      int maxx = x + 2;
      const int miny = 0;
      const int maxy = 2;
      float sum = 0.0f;
      int count = 0;
      minx = std::max(0, minx);
      maxx = std::min(HM_WIDTH - 1, maxx);
      // compute the kernel sum at x,0
      for (int iy = miny; iy <= maxy; ++iy) {
        for (int ix = minx; ix <= maxx; ++ix) {
          sum += precipitation->getValue(ix, iy);
          ++count;
        }
      }
      temp_hm.setValue(x, 0, sum / count);
      for (int y = 1; y < HM_HEIGHT; ++y) {
        if (y - 2 >= 0) {
          // remove the top-line sum
          for (int ix = minx; ix <= maxx; ++ix) {
            sum -= precipitation->getValue(ix, y - 2);
            --count;
          }
        }
        if (y + 2 < HM_HEIGHT) {
          // add the bottom-line sum
          for (int ix = minx; ix <= maxx; ++ix) {
            sum += precipitation->getValue(ix, y + 2);
            ++count;
          }
        }
        temp_hm.setValue(x, y, sum / count);
      }
    }
  }
  precipitation->copy(&temp_hm);

  float t1 = get_time();
  DBG(("  Blur... %g\n", t1 - t0));
  t0 = t1;

  precipitation->normalize();
  t1 = get_time();
  DBG(("  Normalization... %g\n", t1 - t0));
  t0 = t1;
}

void WorldGenerator::computeTemperaturesAndBiomes() {
  // temperature shift with altitude : -25°C at 6000 m
  // mean temp at sea level : 25°C at lat 0  5°C at lat 45 -25°C at lat 90 (sinusoide)
  static constexpr float sandCoef = 1.0f / (1.0f - sandHeight);
  static constexpr float waterCoef = 1.0f / sandHeight;
  for (int y = 0; y < HM_HEIGHT; ++y) {
    const float lat = (float)(y - HM_HEIGHT / 2) * 2 / HM_HEIGHT;
    float latTemp = 0.5f * (1.0f + powf(sinf(3.1415926f * (lat + 0.5f)), 5));  // between 0 and 1
    if (latTemp > 0.0f) latTemp = sqrt(latTemp);
    latTemp = -30 + latTemp * 60;
    for (int x = 0; x < HM_WIDTH; ++x) {
      const float h0 = hm->getValue(x, y);
      float h = h0 - sandHeight;
      if (h < 0.0f)
        h *= waterCoef;
      else
        h *= sandCoef;
      const float altShift = -35 * h;
      const float temp = latTemp + altShift;
      temperature->setValue(x, y, temp);
      const float humid = precipitation->getValue(x, y);
      // compute biome
      EClimate climate = getClimateFromTemp(temp);
      int iHumid = (int)(humid * 5);
      iHumid = std::min(4, iHumid);
      const EBiome& biome = biomeDiagram[climate][iHumid];
      biomeMap[x + y * HM_WIDTH] = biome;
    }
  }
  float min, max;
  temperature->getMinMax(&min, &max);
  DBG(("Temperatures min/max: %g / %g\n", min, max));
}

TCODColor WorldGenerator::getBiomeColor(EBiome biome, int x, int y) {
  static constexpr TCODColor biomeColors[] = {
      // TUNDRA,
      TCODColor(200, 240, 255),
      // COLD_DESERT,
      TCODColor(180, 210, 210),
      // GRASSLAND,
      TCODColor(0, 255, 127),
      // BOREAL_FOREST,
      TCODColor(14, 93, 43),
      // TEMPERATE_FOREST,
      TCODColor(44, 177, 83),
      // TROPICAL_MONTANE_FOREST,
      TCODColor(185, 232, 164),
      // HOT_DESERT,
      TCODColor(235, 255, 210),
      // SAVANNA,
      TCODColor(255, 205, 20),
      // TROPICAL_DRY_FOREST,
      TCODColor(60, 130, 40),
      // TROPICAL_EVERGREEN_FOREST,
      TCODColor(0, 255, 0),
      // THORN_FOREST,
      TCODColor(192, 192, 112),
  };
  int count = 1;
  int r = biomeColors[biome].r;
  int g = biomeColors[biome].g;
  int b = biomeColors[biome].b;
  for (int i = 0; i < 4; ++i) {
    const int ix = x + wgRng->getInt(-10, 10);
    const int iy = y + wgRng->getInt(-10, 10);
    if (IN_RECTANGLE(ix, iy, HM_WIDTH, HM_HEIGHT)) {
      const TCODColor& c = biomeColors[biomeMap[ix + iy * HM_WIDTH]];
      r += c.r + wgRng->getInt(-10, 10);
      g += c.g + wgRng->getInt(-10, 10);
      b += c.b + wgRng->getInt(-10, 10);
      ++count;
    }
  }
  r /= count;
  g /= count;
  b /= count;
  r = CLAMP(0, 255, r);
  g = CLAMP(0, 255, g);
  b = CLAMP(0, 255, b);
  return TCODColor(r, g, b);
}

void WorldGenerator::computeColors() {
  // alter map color using temperature & precipitation maps
  const map_data_t* md = mapData;
  for (int y = 0; y < HM_HEIGHT; ++y) {
    for (int x = 0; x < HM_WIDTH; ++x) {
      const float h = hm->getValue(x, y);
      float temp = temperature->getValue(x, y);
      const EBiome& biome = biomeMap[x + y * HM_WIDTH];
      TCODColor c;
      if (h < sandHeight)
        c = getMapColor(h);
      else {
        c = getMapColor(h);
        c = TCODColor::lerp(c, getBiomeColor(biome, x, y), 0.5f);
      }

      // snow near poles
      temp += 10 * (clouds[HM_WIDTH - 1 - x][HM_HEIGHT - 1 - y]);  // cheap 2D noise ;)
      if (temp < -10.0f && h < sandHeight)
        worldmap->putPixel(x, y, TCODColor::lerp(WHITE, c, 0.3f));
      else if (temp < -8.0f && h < sandHeight)
        worldmap->putPixel(x, y, TCODColor::lerp(WHITE, c, 0.3f + 0.7f * (10.0f + temp) / 2.0f));
      else if (temp < -2.0f && h >= sandHeight)
        worldmap->putPixel(x, y, WHITE);
      else if (temp < 2.0f && h >= sandHeight) {
        // TCODColor snow = mapGradient[(int)(snowHeight*255) + (int)((255 - (int)(snowHeight*255)) *
        // (0.6f-temp)/0.4f)];
        c = TCODColor::lerp(WHITE, c, (temp + 2) / 4.0f);
        worldmap->putPixel(x, y, c);
      } else {
        worldmap->putPixel(x, y, c);
      }
      ++md;
    }
  }
  // draw rivers
  /*
  for (river_t **it=rivers.begin(); it != rivers.end(); it++) {
      for (int i=0; i < (*it)->coords.size(); i++ ) {
          int coord = (*it)->coords.get(i);
          int strength = (*it)->strength.get(i);
          int x = coord % HM_WIDTH;
          int y = coord / HM_WIDTH;
          TCODColor c= worldmap->getPixel(x,y);
          c = TCODColor::lerp(c,TCODColor::blue,(float)(strength)/5.0f);
          worldmap->putPixel(x,y,c);
      }
  }
  */
  md = mapData;
  for (int y = 0; y < HM_HEIGHT; ++y) {
    for (int x = 0; x < HM_WIDTH; ++x) {
      if (md->riverId > 0) {
        TCODColor c = worldmap->getPixel(x, y);
        c = TCODColor::lerp(c, {0, 0, 255}, 0.3f);
        worldmap->putPixel(x, y, c);
      }
      ++md;
    }
  }
  // blur
  static constexpr int dx[] = {0, -1, 0, 1, 0};
  static constexpr int dy[] = {0, 0, -1, 0, 1};
  static constexpr int coef[] = {1, 2, 2, 2, 2};
  for (int y = 0; y < HM_HEIGHT; ++y) {
    for (int x = 0; x < HM_WIDTH; ++x) {
      int r = 0, g = 0, b = 0, count = 0;
      for (int i = 0; i < 5; ++i) {
        const int ix = x + dx[i];
        const int iy = y + dy[i];
        if (IN_RECTANGLE(ix, iy, HM_WIDTH, HM_HEIGHT)) {
          TCODColor c = worldmap->getPixel(ix, iy);
          r += coef[i] * c.r;
          g += coef[i] * c.g;
          b += coef[i] * c.b;
          count += coef[i];
        }
      }
      r /= count;
      g /= count;
      b /= count;
      worldmap->putPixel(x, y, TCODColor(r, g, b));
    }
  }
  drawCoasts(worldmap);
}

void WorldGenerator::generate(TCODRandom* wRng) {
  float t00, t0 = get_time();
  t00 = t0;
  cloudDx = cloudTotalDx = 0.0f;
  TCODColor::genMap(mapGradient, MAX_COLOR_KEY, keyColor, keyIndex);
  if (wRng == NULL) wRng = TCODRandom::getInstance();
  wgRng = wRng;
  noise = new TCODNoise(2, wgRng);
  hm = new TCODHeightMap(HM_WIDTH, HM_HEIGHT);
  hm2 = new TCODHeightMap(HM_WIDTH, HM_HEIGHT);
  worldmap = new TCODImage(HM_WIDTH, HM_HEIGHT);
  worldint = new float[HM_WIDTH * HM_HEIGHT];
  temperature = new TCODHeightMap(HM_WIDTH, HM_HEIGHT);
  precipitation = new TCODHeightMap(HM_WIDTH, HM_HEIGHT);
  biomeMap = new EBiome[HM_WIDTH * HM_HEIGHT];
  mapData = new map_data_t[HM_WIDTH * HM_HEIGHT];
  memset(mapData, 0, sizeof(map_data_t) * HM_WIDTH * HM_HEIGHT);
  float t1 = get_time();
  DBG(("Initialization... %g\n", t1 - t0));
  t0 = t1;

  buildBaseMap();
  t1 = get_time();
  DBG(("Heightmap construction... %g\n", t1 - t0));
  t0 = t1;

  computePrecipitations();
  t1 = get_time();
  DBG(("Precipitation map... %g\n", t1 - t0));
  t0 = t1;

  erodeMap();
  t1 = get_time();
  DBG(("Erosion... %g\n", t1 - t0));
  t0 = t1;

  smoothMap();
  t1 = get_time();
  DBG(("Smooth... %g\n", t1 - t0));
  t0 = t1;

  setLandMass(0.6f, sandHeight);

  for (int i = 0; i < HM_WIDTH * HM_HEIGHT / 3000; ++i) {
    //	for (int i=0; i < 1; i++) {
    generateRivers();
  }
  t1 = get_time();
  DBG(("Rivers... %g\n", t1 - t0));
  t0 = t1;

  smoothPrecipitations();
  t1 = get_time();
  DBG(("Smooth precipitations... %g\n", t1 - t0));
  t0 = t1;

  computeTemperaturesAndBiomes();
  t1 = get_time();
  DBG(("Temperature map... %g\n", t1 - t0));
  t0 = t1;

  computeColors();
  t1 = get_time();
  DBG(("Color map... %g\n", t1 - t0));
  t0 = t1;

  t1 = get_time();
  DBG(("TOTAL TIME... %g\n", t1 - t00));
}

void WorldGenerator::drawCoasts(TCODImage* img) {
  // detect coasts
  for (int y = 0; y < HM_HEIGHT - 1; ++y) {
    for (int x = 0; x < HM_WIDTH - 1; ++x) {
      float h = hm->getValue(x, y);
      float h2 = hm->getValue(x + 1, y);
      if ((h < sandHeight && h2 >= sandHeight) || (h2 < sandHeight && h >= sandHeight))
        img->putPixel(x, y, {0, 0, 0});
      else {
        h = hm->getValue(x, y);
        h2 = hm->getValue(x, y + 1);
        if ((h < sandHeight && h2 >= sandHeight) || (h2 < sandHeight && h >= sandHeight))
          img->putPixel(x, y, {0, 0, 0});
      }
    }
  }
}

void WorldGenerator::saveBiomeMap(const char* filename) {
  static TCODImage* legend = NULL;
  static int legendHeight, legendWidth;
  static constexpr TCODColor biomeColors[] = {
      // TUNDRA,
      TCODColor(88, 234, 250),
      // COLD_DESERT,
      TCODColor(129, 174, 170),
      // GRASSLAND,
      TCODColor(0, 255, 127),
      // BOREAL_FOREST,
      TCODColor(14, 93, 43),
      // TEMPERATE_FOREST,
      TCODColor(44, 177, 83),
      // TROPICAL_MONTANE_FOREST,
      TCODColor(185, 232, 164),
      // HOT_DESERT,
      TCODColor(229, 247, 184),
      // SAVANNA,
      TCODColor(255, 127, 0),
      // TROPICAL_DRY_FOREST,
      TCODColor(191, 191, 0),
      // TROPICAL_EVERGREEN_FOREST,
      TCODColor(0, 255, 0),
      // THORN_FOREST,
      TCODColor(192, 192, 112),
  };
  if (legend == NULL) {
    legend = new TCODImage("data/img/legend_biome.png");
    legend->getSize(&legendWidth, &legendHeight);
  }
  if (filename == NULL) filename = "world_biome.png";
  TCODImage img(MAX(HM_WIDTH, legendWidth), HM_HEIGHT + legendHeight);
  // draw biome map
  for (int y = 0; y < HM_HEIGHT; ++y) {
    for (int x = 0; x < HM_WIDTH; ++x) {
      const float h = hm->getValue(x, y);
      if (h < sandHeight)
        img.putPixel(x, y, TCODColor(100, 100, 255));
      else
        img.putPixel(x, y, biomeColors[biomeMap[x + y * HM_WIDTH]]);
    }
  }
  drawCoasts(&img);
  // blit legend
  int legend_x = MAX(HM_WIDTH, legendWidth) / 2 - legendWidth / 2;
  for (int y = 0; y < legendHeight; ++y) {
    for (int x = 0; x < legendWidth; ++x) {
      img.putPixel(legend_x + x, HM_HEIGHT + y, legend->getPixel(x, y));
    }
  }
  // fill legend colors
  for (int i = 0; i < 6; ++i) {
    for (int x = 17; x < 47; ++x)
      for (int y = 4 + i * 14; y < 14 + i * 14; ++y) {
        img.putPixel(legend_x + x, HM_HEIGHT + y, biomeColors[i]);
      }
  }
  for (int i = 6; i < NB_BIOMES; ++i) {
    for (int x = 221; x < 251; ++x)
      for (int y = 4 + (i - 6) * 14; y < 14 + (i - 6) * 14; ++y)
        img.putPixel(legend_x + x, HM_HEIGHT + y, biomeColors[i]);
  }
  img.save(filename);
}

void WorldGenerator::saveTemperatureMap(const char* filename) {
  static TCODColor tempGradient[256];

  static TCODImage* legend = NULL;
  static int legendHeight, legendWidth;
  if (legend == NULL) {
    legend = new TCODImage("data/img/legend_temperature.png");
    legend->getSize(&legendWidth, &legendHeight);
    TCODColor::genMap(tempGradient, MAX_TEMP_KEY, tempKeyColor, tempIndexes);
  }

  if (filename == NULL) filename = "world_temperature.png";
  TCODImage img(MAX(HM_WIDTH, legendWidth), HM_HEIGHT + legendHeight);
  float minTemp, maxTemp;
  temperature->getMinMax(&minTemp, &maxTemp);
  // render temperature map
  for (int y = 0; y < HM_HEIGHT; ++y) {
    for (int x = 0; x < HM_WIDTH; ++x) {
      float h = hm->getValue(x, y);
      if (h < sandHeight)
        img.putPixel(x, y, TCODColor(100, 100, 255));
      else {
        float temp = temperature->getValue(x, y);
        temp = (temp - minTemp) / (maxTemp - minTemp);
        int colorIdx = (int)(temp * 255);
        colorIdx = CLAMP(0, 255, colorIdx);
        img.putPixel(x, y, tempGradient[colorIdx]);
      }
    }
  }
  drawCoasts(&img);

  // blit legend
  const int legend_x = std::max(HM_WIDTH, legendWidth) / 2 - legendWidth / 2;
  for (int y = 0; y < legendHeight; ++y) {
    for (int x = 0; x < legendWidth; ++x) {
      img.putPixel(legend_x + x, HM_HEIGHT + y, legend->getPixel(x, y));
    }
  }
  img.save(filename);
}

void WorldGenerator::savePrecipitationMap(const char* filename) {
  static TCODImage* legend = NULL;
  static int legendHeight, legendWidth;
  if (legend == NULL) {
    legend = new TCODImage("data/img/legend_precipitation.png");
    legend->getSize(&legendWidth, &legendHeight);
  }

  if (filename == NULL) filename = "world_precipitation.png";
  TCODImage img(MAX(HM_WIDTH, legendWidth), HM_HEIGHT + legendHeight);
  // render precipitation map
  for (int y = 0; y < HM_HEIGHT; ++y) {
    for (int x = 0; x < HM_WIDTH; ++x) {
      const float h = hm->getValue(x, y);
      if (h < sandHeight)
        img.putPixel(x, y, TCODColor(100, 100, 255));
      else {
        const float prec = precipitation->getValue(x, y);
        const int i_prec = (int)(prec * 180);
        int colorIdx = 0;
        while (colorIdx < MAX_PREC_KEY && i_prec > precIndexes.at(colorIdx)) ++colorIdx;
        colorIdx = CLAMP(0, MAX_PREC_KEY, colorIdx);
        img.putPixel(x, y, precColors.at(colorIdx));
      }
    }
  }
  drawCoasts(&img);

  // blit legend
  int legend_x = MAX(HM_WIDTH, legendWidth) / 2 - legendWidth / 2;
  for (int y = 0; y < legendHeight; ++y) {
    for (int x = 0; x < legendWidth; ++x) {
      img.putPixel(legend_x + x, HM_HEIGHT + y, legend->getPixel(x, y));
    }
  }
  img.save(filename);
}

void WorldGenerator::saveAltitudeMap(const char* filename) {
  static std::array<TCODColor, 256> altGradient{};

  static TCODImage* legend = NULL;
  static int legendHeight, legendWidth;
  if (legend == NULL) {
    legend = new TCODImage("data/img/legend_altitude.png");
    legend->getSize(&legendWidth, &legendHeight);
    TCODColor::genMap(&altGradient[0], MAX_ALT_KEY, &altColors[0], &altIndexes[0]);
  }

  if (filename == NULL) filename = "world_altitude.png";
  TCODImage img(HM_WIDTH + legendWidth, MAX(HM_HEIGHT, legendHeight));
  // render altitude map
  for (int y = 0; y < HM_HEIGHT; ++y) {
    for (int x = 0; x < HM_WIDTH; ++x) {
      const float h = hm->getValue(x, y);
      int i_alt = (int)(h * 256);
      i_alt = CLAMP(0, 255, i_alt);
      img.putPixel(x, y, altGradient.at(i_alt));
    }
  }

  // blit legend
  int legend_y = MAX(HM_HEIGHT, legendHeight) / 2 - legendHeight / 2;
  for (int y = 0; y < legendHeight; ++y) {
    for (int x = 0; x < legendWidth; ++x) {
      img.putPixel(HM_WIDTH + x, legend_y + y, legend->getPixel(x, y));
    }
  }
  img.save(filename);
}
