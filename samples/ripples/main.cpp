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

#include <SDL.h>
#include <libtcod.h>

#include <algorithm>
#include <array>
#include <memory>

#include "util_ripples.hpp"

TCODNoise noise2d(2);
TCODNoise noise3d(3);
std::unique_ptr<RippleManager> rippleManager;
std::unique_ptr<TCODImage> ground, ground_with_ripples;

std::array<TCODColor, 256> mapGradient;
static constexpr auto MAX_COLOR_KEY = 8;

// TCOD's land color map
static constexpr float sandHeight = 0.3f;
static constexpr float grassHeight = 0.5f;
static constexpr float snowHeight = 0.9f;

static constexpr std::array<int, MAX_COLOR_KEY> keyIndex{
    0,
    static_cast<int>(sandHeight * 255),
    static_cast<int>(sandHeight * 255) + 4,
    static_cast<int>(grassHeight * 255),
    static_cast<int>(grassHeight * 255) + 10,
    static_cast<int>(snowHeight * 255),
    static_cast<int>(snowHeight * 255) + 10,
    255,
};
static constexpr std::array<TCODColor, MAX_COLOR_KEY> keyColor{
    TCODColor(10, 10, 90),  // deep water
    TCODColor(30, 30, 170),  // water-sand transition
    TCODColor(114, 150, 71),  // sand
    TCODColor(80, 120, 10),  // sand-grass transition
    TCODColor(17, 109, 7),  // grass
    TCODColor(120, 220, 120),  // grass-snow transistion
    TCODColor(208, 208, 239),  // snow
    TCODColor(255, 255, 255),
};

void update(float elapsed, TCOD_key_t, TCOD_mouse_t mouse) {
  if (mouse.lbutton) rippleManager->startRipple(mouse.cx * 2, mouse.cy * 2);
  rippleManager->updateRipples(elapsed);
}

void render() {
  // copy ground into ground_with_ripples. damn libtcod should have that...
  for (int x = 0; x < CON_W * 2; ++x) {
    for (int y = 0; y < CON_H * 2; ++y) {
      ground_with_ripples->putPixel(x, y, ground->getPixel(x, y));
    }
  }
  rippleManager->renderRipples(ground.get(), ground_with_ripples.get());
  ground_with_ripples->blit2x(TCODConsole::root, 0, 0);
  TCODConsole::root->setDefaultForeground({255, 255, 255});
  TCODConsole::root->printf(3, 49, "Click in water to trigger ripples");
}

int main(int argc, char* argv[]) {
  // initialize the game window
  TCODConsole::initRoot(CON_W, CON_H, "Water ripples", false, TCOD_RENDERER_OPENGL2);
  TCODSystem::setFps(25);

  bool endCredits = false;

  // create a 2d noise
  TCODHeightMap hm(CON_W * 2, CON_H * 2);
  hm.addFbm(&noise2d, 3.0f, 3.0f, 0, 0, 7.0f, 1.0f, 0.5f);
  hm.normalize();
  // apply a color map to create a ground image
  TCODColor::genMap(&mapGradient[0], MAX_COLOR_KEY, keyColor.data(), keyIndex.data());
  ground = std::make_unique<TCODImage>(CON_W * 2, CON_H * 2);
  ground_with_ripples = std::make_unique<TCODImage>(CON_W * 2, CON_H * 2);
  // create a TCODMap defining water zones. Walkable = water
  TCODMap waterMap(CON_W * 2, CON_H * 2);

  for (int x = 0; x < CON_W * 2; ++x) {
    for (int y = 0; y < CON_H * 2; ++y) {
      const float h = hm.getValue(x, y);
      const bool isWater = h < sandHeight;
      waterMap.setProperties(x, y, isWater, isWater);
      const int ih = std::max(0, std::min(static_cast<int>(h * 256), 255));  // Clamp to 0...255.
      ground->putPixel(x, y, mapGradient[ih]);
    }
  }
  rippleManager = std::make_unique<RippleManager>(&waterMap);

  while (!TCODConsole::isWindowClosed()) {
    TCOD_key_t k;
    TCOD_mouse_t mouse;

    TCODSystem::checkForEvent(TCOD_EVENT_KEY | TCOD_EVENT_MOUSE, &k, &mouse);

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
