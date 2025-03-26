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

#include <algorithm>
#include <array>
#include <libtcod.hpp>
#include <libtcod/timer.hpp>
#include <memory>

#include "util_ripples.hpp"

static constexpr auto WHITE = TCOD_ColorRGB{255, 255, 255};

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

void render(TCOD_Console& console) {
  // copy ground into ground_with_ripples. damn libtcod should have that...
  for (int y = 0; y < CONSOLE_HEIGHT * 2; ++y) {
    for (int x = 0; x < CONSOLE_WIDTH * 2; ++x) {
      ground_with_ripples->putPixel(x, y, ground->getPixel(x, y));
    }
  }
  rippleManager->renderRipples(*ground, *ground_with_ripples);
  tcod::draw_quartergraphics(console, *ground_with_ripples);
  tcod::print(console, {3, 49}, "Click in water to trigger ripples", WHITE, std::nullopt);
}

int main(int argc, char* argv[]) {
  auto tileset = tcod::load_tilesheet("data/fonts/terminal8x8_gs_tc.png", {32, 8}, tcod::CHARMAP_TCOD);
  auto console = tcod::Console{CONSOLE_WIDTH, CONSOLE_HEIGHT};
  // initialize the game window
  TCOD_ContextParams params{};
  params.argc = argc;
  params.argv = argv;
  params.console = console.get();
  params.vsync = true;
  params.window_title = "Water ripples";
  params.sdl_window_flags = SDL_WINDOW_RESIZABLE;
  params.tileset = tileset.get();

  auto context = tcod::Context(params);

  bool endCredits = false;

  // create a 2d noise
  TCODHeightMap hm(CONSOLE_WIDTH * 2, CONSOLE_HEIGHT * 2);
  hm.addFbm(&noise2d, 3.0f, 3.0f, 0, 0, 7.0f, 1.0f, 0.5f);
  hm.normalize();
  // apply a color map to create a ground image
  TCODColor::genMap(&mapGradient[0], MAX_COLOR_KEY, keyColor.data(), keyIndex.data());
  ground = std::make_unique<TCODImage>(CONSOLE_WIDTH * 2, CONSOLE_HEIGHT * 2);
  ground_with_ripples = std::make_unique<TCODImage>(CONSOLE_WIDTH * 2, CONSOLE_HEIGHT * 2);
  // create a TCODMap defining water zones. Walkable = water
  TCODMap waterMap(CONSOLE_WIDTH * 2, CONSOLE_HEIGHT * 2);

  for (int y = 0; y < CONSOLE_HEIGHT * 2; ++y) {
    for (int x = 0; x < CONSOLE_WIDTH * 2; ++x) {
      const float h = hm.getValue(x, y);
      const bool isWater = h < sandHeight;
      waterMap.setProperties(x, y, isWater, isWater);
      const int ih = std::max(0, std::min(static_cast<int>(h * 256), 255));  // Clamp to 0...255.
      ground->putPixel(x, y, mapGradient[ih]);
    }
  }
  rippleManager = std::make_unique<RippleManager>(waterMap);

  auto timer = tcod::Timer();
  while (true) {
    const float delta_time = timer.sync();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      context.convert_event_coordinates(event);
      switch (event.type) {
        case SDL_EVENT_QUIT:
          std::exit(EXIT_SUCCESS);
          break;
        case SDL_EVENT_KEY_DOWN:
          switch (event.key.key) {
            case SDLK_PRINTSCREEN:
              context.save_screenshot();
              break;
            default:
              break;
          }
          break;
        case SDL_EVENT_MOUSE_MOTION:
          if (event.motion.state & SDL_BUTTON_LMASK) {
            rippleManager->startRipple((int)event.motion.x * 2, (int)event.motion.y * 2);
          }
          break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
          if (event.button.button == SDL_BUTTON_LEFT) {
            rippleManager->startRipple((int)event.button.x * 2, (int)event.button.y * 2);
          }
          break;
        default:
          break;
      }
    }
    rippleManager->updateRipples(delta_time);

    // render the game screen
    render(console);
    // render libtcod credits
    if (!endCredits) endCredits = TCOD_console_credits_render_ex(console.get(), 4, 4, true, delta_time);
    // flush updates to screen
    context.present(console);
  }
  return 0;
}
