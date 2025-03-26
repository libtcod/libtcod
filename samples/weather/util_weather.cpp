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

#include <math.h>

#include "main.hpp"

static constexpr auto OCTAVES = 7.0f;
static constexpr auto SCALE = 2.0f;
static constexpr auto MAX_WIND_SPEED = 4.0f;
static constexpr auto LIGHTNING_LEVEL = 0.4f;
static constexpr auto LIGHTNING_RADIUS = 500;
static constexpr auto LIGHTNING_LIFE = 2.0f;
static constexpr auto LIGHTNING_INTENSITY_SPEED = 20.0f;
static constexpr auto LIGHTNING_MIN_PROB = 1.0f / 7.0f;
static constexpr auto LIGHTNING_MAX_PROB = 1.0f;
static constexpr auto RAIN_MIN_PROB = 1.0f / 4000.0f;
static constexpr auto RAIN_MED_PROB = 1.0f / 400.0f;
static constexpr auto RAIN_MAX_PROB = 1.0f / 10.0f;

Weather::Weather(int width, int height) {
  map_ = TCODHeightMap(width + 2, height + 2);
  // TODO : would be better with a 3d noise and slowly varying z
  // but you can notice the difference only when time is accelerated
  map_.addFbm(&noise2d, SCALE, SCALE, 0.0f, 0.0f, OCTAVES, 0.5f, 0.5f);
  update(0.0f);
}

void Weather::move(int dx, int dy) {
  this->dx_ += dx;
  this->dy_ += dy;
}

const char* Weather::getWeather() const noexcept {
  if (indicator_ > 0.9f)
    return "The sky is completely clear.";
  else if (indicator_ > 0.7f)
    return "The sky is clear.";
  else if (indicator_ > 0.6f)
    return "It's cloudy.";
  else if (indicator_ > 0.5f)
    return "You feel a breeze.";
  else if (indicator_ > 0.4f)
    return "It's drizzling.";
  else if (indicator_ > 0.3f)
    return "It's raining.";
  else if (indicator_ > 0.2f)
    return "You get caught in a storm.";
  else
    return "The storm is raging";
}

void Weather::update(float delta_time) {
  elapsed_time_ += delta_time;
  float perlin_x = changeFactor_ * elapsed_time_ / 100.0f;
  indicator_ = (1.0f + noise1d.get(&perlin_x, TCOD_NOISE_SIMPLEX)) * 0.5f + indicator_bias_;
  indicator_ = std::clamp(indicator_, 0.0f, 1.0f);
  const float wind_speed = 1.0f - indicator_;
  perlin_x *= 2.0f;
  const float windDir = (2.0f * 3.1415926f * 0.5f) * (1.0f + noise1d.get(&perlin_x, TCOD_NOISE_SIMPLEX));
  dx_ += MAX_WIND_SPEED * wind_speed * cosf(windDir) * delta_time;
  dy_ += 0.5f * MAX_WIND_SPEED * wind_speed * sinf(windDir) * delta_time;
  if (indicator_ < LIGHTNING_LEVEL) {
    const float storm = (LIGHTNING_LEVEL - indicator_) / LIGHTNING_LEVEL;  // storm power 0-1
    const float lightning_probability =
        LIGHTNING_MIN_PROB + ((LIGHTNING_MAX_PROB - LIGHTNING_MIN_PROB) * storm);  // nb of lightning per second
    if (TCODRandom::getInstance()->getFloat(0, 1) < lightning_probability * delta_time) {
      // spawn lightning
      auto& new_lightning = lightnings_.emplace_back();
      new_lightning.pos_x = TCODRandom::getInstance()->getInt(0, map_.w);
      new_lightning.pos_y = TCODRandom::getInstance()->getInt(0, map_.h);
      new_lightning.life = TCODRandom::getInstance()->getFloat(0.1f, LIGHTNING_LIFE);
      new_lightning.radius_squared = TCODRandom::getInstance()->getInt(LIGHTNING_RADIUS, LIGHTNING_RADIUS * 2);
      new_lightning.noise_x = TCODRandom::getInstance()->getFloat(0.0f, 1000.0f);
      new_lightning.intensity = 0.0f;
    }
  }

  int bx = 0, by = 0;
  while (dx_ >= 1.0f) {
    dx_ -= 1.0f;
    noise_x_ += 1.0f;
    bx++;
  }
  while (dx_ <= -1.0f) {
    dx_ += 1.0f;
    noise_x_ -= 1.0f;
    bx--;
  }
  while (dy_ >= 1.0f) {
    dy_ -= 1.0f;
    noise_y_ += 1.0f;
    by++;
  }
  while (dy_ <= -1.0f) {
    dy_ += 1.0f;
    noise_y_ -= 1.0f;
    by--;
  }
  // update lightnings
  for (int i = static_cast<int>(lightnings_.size()) - 1; i >= 0; --i) {
    lightning_t& lightning = lightnings_.at(i);
    lightning.life -= delta_time;
    lightning.noise_x += delta_time * LIGHTNING_INTENSITY_SPEED;
    if (lightning.life <= 0) {
      lightnings_.erase(lightnings_.begin() + i);
      continue;
    } else {
      lightning.intensity = 0.5f * noise1d.get(&lightning.noise_x, TCOD_NOISE_SIMPLEX) + 1.0f;
      lightning.pos_x -= bx;
      lightning.pos_y -= by;
    }
  }

  if (bx || by) {
    // recompute the whole map
    // TODO : should recompute only missing rows/columns
    // the world generator demo has that, but only for
    // horizontal move. Here clouds move in any direction
    map_.clear();
    map_.addFbm(&noise2d, SCALE, SCALE, noise_x_, noise_y_, OCTAVES, 0.5f, 0.5f);
  }
}

float Weather::getCloud(int x, int y) {
  // cloud layer
  // 1.0 : no cloud
  // 0 : dark cloud. This way you can easily render ground with color * cloud coef
  float cdx = dx_;
  float cdy = dy_;
  if (dx_ >= 0)
    x++;
  else
    cdx = dx_ + 1.0f;
  if (dy_ >= 0)
    y++;
  else
    cdy = dy_ + 1.0f;
  float val = map_.getInterpolatedValue(x + cdx, y + cdy);  // between 0 and 1
  val += 2 * indicator_ - 0.5f;
  val = std::clamp(val, 0.2f, 1.0f);
  return val;
}

float Weather::getLightning(int x, int y) {
  if (indicator_ >= 0.3f) return 0.0f;
  if (dx_ >= 0) x++;
  if (dy_ >= 0) y++;
  float light = 0.0f;  // accumulated light/intensity
  float cloud = map_.getValue(x, y);
  cloud = 1.0f - cloud;  // inverted cloud. 0=sky, 1=dark cloud
  cloud -= 0.6f;  // no lightning under 0.6f. cloud is now 0 - 0.4
  if (cloud <= 0.0f) return 0.0f;
  cloud = cloud / 0.4f;  // now back to 0-1 range (but only for really cloudy zones)
  for (const lightning_t& lightning : lightnings_) {
    const int dx = lightning.pos_x - x;
    const int dy = lightning.pos_y - y;
    const int dist_squared = dx * dx + dy * dy;
    if (dist_squared < lightning.radius_squared) {
      light += lightning.intensity * (float)(lightning.radius_squared - dist_squared) / lightning.radius_squared;
    }
  }
  return std::clamp(cloud * light, 0.0f, 1.0f);
}

bool Weather::hasRainDrop() {
  if (indicator_ >= 0.5f) return false;
  float rain_chance;
  if (indicator_ >= 0.3f) {
    rain_chance = RAIN_MIN_PROB + (RAIN_MED_PROB - RAIN_MIN_PROB) * (0.5f - indicator_) * 5;
  } else {
    rain_chance = RAIN_MED_PROB + (RAIN_MAX_PROB - RAIN_MED_PROB) * (0.3f - indicator_) * 3.33f;
  }
  return TCODRandom::getInstance()->getFloat(0, 1) < rain_chance;
}

void Weather::calculateAmbient(float timeInSeconds) {
  // calculate ambient light
  static constexpr TCODColor night(0, 0, 128);
  static constexpr TCODColor dawn(196, 0, 0);
  static constexpr TCODColor dawn2(220, 200, 64);
  static constexpr TCODColor day(255, 255, 195);
  float coef = 0.0f;
  const float hour = timeInSeconds / 3600.0f;
  // TODO : should use a color gradient map for that..
  if (hour > 21.0f || hour < 6.0f) {
    ambientColor_ = night;  // night
    coef = 0.0;
  } else if (hour < 7.0f) {
    // between 6am and 7am
    coef = (hour - 6.0f);
    ambientColor_ = TCODColor::lerp(night, dawn, coef);
    coef /= 3.0f;
  } else if (hour < 8.0f) {
    // between 7am and 8am
    coef = (hour - 7.0f);
    ambientColor_ = TCODColor::lerp(dawn, dawn2, coef);
    coef = 0.33333f + coef / 3.0f;
  } else if (hour < 9.0f) {
    // between 8am and 9am
    coef = (hour - 8.0f);
    ambientColor_ = TCODColor::lerp(dawn2, day, coef);
    coef = 0.66666f + coef / 3.0f;
  } else if (hour < 18.0f) {
    // between 9am and 6pm
    ambientColor_ = day;
    coef = 1.0f;
  } else if (hour < 19.0f) {
    // between 6pm and 7pm
    coef = (hour - 18.0f);
    ambientColor_ = TCODColor::lerp(day, dawn2, coef);
    coef = 0.66666f + (1.0f - coef) / 3.0f;
  } else if (hour < 20.0f) {
    // between 7pm and 8pm
    coef = (hour - 19.0f);
    ambientColor_ = TCODColor::lerp(dawn2, dawn, coef);
    coef = 0.33333f + (1.0f - coef) / 3.0f;
  } else if (hour < 21.0f) {
    // between 8pm and 9pm
    coef = (hour - 20.0f);
    ambientColor_ = TCODColor::lerp(dawn, night, coef);
    coef = (1.0f - coef) / 3.0f;
  }
}
