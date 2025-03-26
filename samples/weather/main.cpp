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

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cstdio>
#include <string>

static tcod::Context tcod_context;
static tcod::Console tcod_console;
static Weather weather;
static float dayTime = 6 * 3600.0f;  // starts at 6.00am
static TCODImage ground;
static bool endCredits = false;

static constexpr auto LIGHTNING_COLOR = TCODColor{220, 220, 255};

void update(float delta_time) {
  const auto state = SDL_GetKeyboardState(nullptr);
  if (state[SDL_GetScancodeFromKey(SDLK_RETURN, nullptr)] || state[SDL_GetScancodeFromKey(SDLK_KP_ENTER, nullptr)]) {
    delta_time *= 20.0f;
  }

  dayTime += delta_time * 60 * 3;  // 1 real sec = 3 min
  if (dayTime >= 24 * 3600) dayTime -= 24 * 3600;
  weather.update(delta_time);
  weather.calculateAmbient(dayTime);
}

std::string getDaytime() {
  const int hour = (int)(dayTime / 3600);
  const int minute = (int)((dayTime - hour * 3600) / 60);
  return tcod::stringf("%02d:%02d", hour, minute);
}

void render() {
  static TCODImage img(tcod_console.get_width() * 2, tcod_console.get_height() * 2);
  for (int x = 0; x < tcod_console.get_width() * 2; x++) {
    for (int y = 0; y < tcod_console.get_height() * 2; y++) {
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
        const int lr = (int)(2 * lightning * LIGHTNING_COLOR.r);
        const int lg = (int)(2 * lightning * LIGHTNING_COLOR.g);
        const int lb = (int)(2 * lightning * LIGHTNING_COLOR.b);
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
  tcod::draw_quartergraphics(tcod_console, img);
  // rain drops
  for (int y = 0; y < tcod_console.get_height(); ++y) {
    for (int x = 0; x < tcod_console.get_width(); ++x) {
      if (weather.hasRainDrop()) {
        const float lightning = weather.getLightning(x * 2, y * 2);
        const float cloudCoef = weather.getCloud(x * 2, y * 2);
        TCODColor color = TCODColor{0, 0, 191} * cloudCoef;
        color = color * weather.getAmbientLightColor();
        if (lightning > 0.0f) color = color + 2 * lightning * LIGHTNING_COLOR;
        tcod_console.at({x, y}).ch = '/';
        tcod_console.at({x, y}).fg = tcod::ColorRGB{color};
      }
    }
  }
  tcod::print(
      tcod_console,
      {5, CONSOLE_HEIGHT - 12},
      tcod::stringf(
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
          weather.getIndicatorDelta()),
      tcod::ColorRGB{255, 255, 255},
      {});
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

SDL_AppResult SDL_AppIterate(void*) {
  float delta_time = tcod_clock.sync();
  update(delta_time);

  render();  // render the game screen
  if (!endCredits) endCredits = TCOD_console_credits_render_ex(tcod_console.get(), 4, 4, true, delta_time);
  tcod_context.present(tcod_console);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void*, SDL_Event* event) {
  switch (event->type) {
    case SDL_EVENT_QUIT:
      return SDL_APP_SUCCESS;
    case SDL_EVENT_KEY_DOWN:
      switch (event->key.key) {
        case SDLK_PLUS:
        case SDLK_EQUALS:
        case SDLK_KP_PLUS:
          weather.setIndicatorDelta(weather.getIndicatorDelta() + 0.1f);
          break;
        case SDLK_MINUS:
        case SDLK_KP_MINUS:
          weather.setIndicatorDelta(weather.getIndicatorDelta() - 0.1f);
          break;
        case SDLK_PRINTSCREEN:
          tcod_context.save_screenshot();
          break;
      }
      break;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void**, int argc, char** argv) {
  static constexpr char* FONT = "data/fonts/dejavu12x12_gs_tc.png";
  auto tileset = tcod::load_tilesheet(FONT, {32, 8}, tcod::CHARMAP_TCOD);

  tcod_console = tcod::Console{CONSOLE_WIDTH, CONSOLE_HEIGHT};

  auto params = TCOD_ContextParams{};
  params.argc = argc;
  params.argv = argv;
  params.vsync = true;
  params.sdl_window_flags = SDL_WINDOW_RESIZABLE;
  params.console = tcod_console.get();
  params.window_title = "Weather system";
  params.tileset = tileset.get();

  tcod_context = tcod::Context(params);

  weather = Weather{tcod_console.get_width() * 2, tcod_console.get_height() * 2};
  ground = generate_ground_texture(tcod_console.get_width() * 2, tcod_console.get_height() * 2);
  // for this demo, we want the weather to evolve quite rapidly
  weather.setChangeFactor(3.0f);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void*, SDL_AppResult) {}
