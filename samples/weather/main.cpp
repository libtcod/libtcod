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

#include "main.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cstdio>
#include <string>

static Weather weather;
static float dayTime = 6 * 3600.0f;  // starts at 6.00am
static TCODColor lightningColor(220, 220, 255);
static TCODImage ground;

void update(float elapsed, TCOD_key_t k, TCOD_mouse_t) {
  if (k.c == '+') {
    const float d = weather.getIndicatorDelta();
    weather.setIndicatorDelta(d + elapsed * 0.1f);
  } else if (k.c == '-') {
    const float d = weather.getIndicatorDelta();
    weather.setIndicatorDelta(d - elapsed * 0.1f);
  } else if (k.vk == TCODK_ENTER || k.vk == TCODK_KPENTER) {
    elapsed *= 20.0f;
  }

  dayTime += elapsed * 60 * 3;  // 1 real sec = 3 min
  if (dayTime >= 24 * 3600) dayTime -= 24 * 3600;
  weather.update(elapsed);
  weather.calculateAmbient(dayTime);
}

std::string getDaytime() {
  const int hour = (int)(dayTime / 3600);
  const int minute = (int)((dayTime - hour * 3600) / 60);
  return tcod::stringf("%02d:%02d", hour, minute);
}

void render() {
  static TCODImage img(CON_W * 2, CON_H * 2);
  for (int x = 0; x < CON_W * 2; x++) {
    for (int y = 0; y < CON_H * 2; y++) {
      // we don't use color operation to avoid 0-255 clamping at every step
      // sort of poor man's HDR...
      int r = 0;
      int g = 0;
      int b = 0;

      // default ground color
      TCODColor groundCol = ground.getPixel(x, y);

      // take cloud shadow into account
      const float cloudCoef = weather.getCloud(x, y);
      r += (int)(cloudCoef * weather.getAmbientLightColor().r);
      g += (int)(cloudCoef * weather.getAmbientLightColor().g);
      b += (int)(cloudCoef * weather.getAmbientLightColor().b);

      // take lightning into account
      const float lightning = weather.getLightning(x, y);
      if (lightning > 0.0f) {
        const int lr = (int)(2 * lightning * lightningColor.r);
        const int lg = (int)(2 * lightning * lightningColor.g);
        const int lb = (int)(2 * lightning * lightningColor.b);
        r += lr;
        g += lg;
        b += lb;
      }
      r = std::min(255, r);
      g = std::min(255, g);
      b = std::min(255, b);
      r = groundCol.r * r / 200;
      g = groundCol.g * g / 200;
      b = groundCol.b * b / 200;
      img.putPixel(x, y, TCODColor(r, g, b));
    }
  }
  img.blit2x(TCODConsole::root, 0, 0);
  // rain drops
  for (int y = 0; y < CON_H; ++y) {
    for (int x = 0; x < CON_W; ++x) {
      if (weather.hasRainDrop()) {
        const float lightning = weather.getLightning(x * 2, y * 2);
        const float cloudCoef = weather.getCloud(x * 2, y * 2);
        TCODColor col = TCODColor{0, 0, 191} * cloudCoef;
        col = col * weather.getAmbientLightColor();
        if (lightning > 0.0f) col = col + 2 * lightning * lightningColor;
        TCODConsole::root->setChar(x, y, '/');
        TCODConsole::root->setCharForeground(x, y, col);
      }
    }
  }
  TCODConsole::root->setDefaultForeground({255, 255, 255});
  TCODConsole::root->printf(
      5,
      CON_H - 12,
      "TCOD's weather system :\n"
      "- wind with varying speed and direction\n"
      "- rain\n"
      "- lightnings\n"
      "- day/night cycle\n"
      "Day time : %s\n"
      "Weather : %s\n\n"
      "Weather evolves automatically\nbut you can alter it by holding + or - : %.1f\n"
      "Accelerate time with ENTER",
      getDaytime().c_str(),
      weather.getWeather(),
      weather.getIndicatorDelta());
}

// Generate and return a new ground texture/
auto generate_ground_texture(int width, int height) -> TCODImage {
  auto new_ground = TCODImage(width, height);
  // generate some good locking ground
  static constexpr std::array colors = {
      tcod::ColorRGB(40, 117, 0),  // grass
      tcod::ColorRGB(69, 125, 0),  // sparse grass
      tcod::ColorRGB(110, 125, 0),  // withered grass
      tcod::ColorRGB(150, 143, 92),  // dried grass
      tcod::ColorRGB(133, 115, 71),  // bare ground
      tcod::ColorRGB(111, 100, 73)  // dirt
  };
  static constexpr std::array keys = {0, 51, 102, 153, 204, 255};
  const auto gradientMap = TCODColor::genMap<256>(colors, keys);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const float f[2] = {x * 3.0f / (width / 2.0f), y * 3.0f / (height / 2.0f)};
      const float h = noise2d.getFbm(f, 6.0f, TCOD_NOISE_SIMPLEX);
      const int ih = static_cast<int>(std::clamp(h * 256.0f, 0.0f, 255.0f));
      float coef = 1.0f;
      // darken the lower part (text background)
      if (y > height - 27) coef = 0.5f;
      TCODColor col = coef * TCODColor{gradientMap.at(ih)};
      // add some noise
      col = col * TCODRandom::getInstance()->getFloat(0.95f, 1.05f);
      new_ground.putPixel(x, y, col);
    }
  }
  return new_ground;
}

int main(int argc, char* argv[]) {
  // initialize the game window
  TCODConsole::initRoot(CON_W, CON_H, "Weather system", false, TCOD_RENDERER_OPENGL2);
  TCODMouse::showCursor(true);
  TCODSystem::setFps(25);

  weather = Weather{CON_W * 2, CON_H * 2};
  ground = generate_ground_texture(CON_W * 2, CON_H * 2);
  // for this demo, we want the weather to evolve quite rapidly
  weather.setChangeFactor(3.0f);

  bool endCredits = false;

  while (!TCODConsole::isWindowClosed()) {
    //	read keyboard
    TCOD_key_t k = TCODConsole::checkForKeypress(TCOD_KEY_PRESSED | TCOD_KEY_RELEASED);
    TCOD_mouse_t mouse = TCODMouse::getStatus();
    if (k.vk == TCODK_PRINTSCREEN) {
      // screenshot
      if (!k.pressed) TCODSystem::saveScreenshot(NULL);
      k.vk = TCODK_NONE;
    } else if (k.lalt && (k.vk == TCODK_ENTER || k.vk == TCODK_KPENTER)) {
      // switch fullscreen
      if (!k.pressed) TCODConsole::setFullscreen(!TCODConsole::isFullscreen());
      k.vk = TCODK_NONE;
    }
    // update the game
    update(TCODSystem::getLastFrameLength(), k, mouse);

    // render the game screen
    render();
    // render libtcod credits
    if (!endCredits) endCredits = TCODConsole::renderCredits(4, 4, true);
    // flush updates to screen
    TCODConsole::root->flush();
  }
  return 0;
}
