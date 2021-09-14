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
#include <libtcod/timer.h>

#include <algorithm>
#include <array>

static constexpr auto WIDTH = 80;
static constexpr auto HEIGHT = 50;

static constexpr auto WHITE = TCOD_ColorRGB{255, 255, 255};

TCODNoise noise1d(1);
TCODNoise noise2d(2);
WorldGenerator worldGen;
// world map panning
float cur_w_x = 0, cur_w_y = 0;
// mouse coordinates in world map
float mx = 0, my = 0;
std::array<float, 2> mouse_subtile_xy{};  // Tracked mouse tile position.

void update(float elapsed) {
  // destination wanted
  const float world_x = (worldGen.getWidth() - 2 * WIDTH) * mouse_subtile_xy.at(0) / WIDTH;
  const float world_y = (worldGen.getHeight() - 2 * HEIGHT) * mouse_subtile_xy.at(1) / HEIGHT;
  cur_w_x += (world_x - cur_w_x) * elapsed;
  cur_w_y += (world_y - cur_w_y) * elapsed;
  mx = cur_w_x + mouse_subtile_xy.at(0) * 2;
  my = cur_w_y + mouse_subtile_xy.at(1) * 2;
  worldGen.updateClouds(elapsed);
}

TCODColor getMapShadedColor(float worldX, float worldY, bool clouds) {
  // sun color
  static constexpr TCODColor sunCol(255, 255, 200);
  const float wx = CLAMP(0.0f, worldGen.getWidth() - 1, worldX);
  const float wy = CLAMP(0.0f, worldGen.getHeight() - 1, worldY);
  // apply cloud shadow
  const float cloudAmount = clouds ? worldGen.getCloudThickness(wx, wy) : 0.0f;
  TCODColor col = worldGen.getInterpolatedColor(worldX, worldY);
  // apply sun light
  float intensity = worldGen.getInterpolatedIntensity(wx, wy);
  intensity = std::min(intensity, 1.5f - cloudAmount);
  const int cr = (int)(intensity * (int)(col.r) * sunCol.r / 255);
  const int cg = (int)(intensity * (int)(col.g) * sunCol.g / 255);
  const int cb = (int)(intensity * (int)(col.b) * sunCol.b / 255);
  return {
      std::max<uint8_t>(static_cast<uint8_t>(CLAMP(0, 255, cr)), col.r / 2),
      std::max<uint8_t>(static_cast<uint8_t>(CLAMP(0, 255, cg)), col.g / 2),
      std::max<uint8_t>(static_cast<uint8_t>(CLAMP(0, 255, cb)), col.b / 2),
  };
}

void render(TCOD_Console& console) {
  // subcell resolution image
  static TCODImage map(WIDTH * 2, HEIGHT * 2);
  // compute the map image
  for (int py = 0; py < 2 * HEIGHT; ++py) {
    for (int px = 0; px < 2 * WIDTH; ++px) {
      // world texel coordinate (with fisheye distorsion)
      const float wx = px + cur_w_x;
      const float wy = py + cur_w_y;
      map.putPixel(px, py, getMapShadedColor(wx, wy, true));
    }
  }
  TCOD_image_blit_2x(map.get_data(), &console, 0, 0, 0, 0, -1, -1);

  static const char* biomeNames[] = {
      "Tundra",
      "Cold desert",
      "Grassland",
      "Boreal forest",
      "Temperate forest",
      "Tropical/Montane forest",
      "Hot desert",
      "Savanna",
      "Tropical dry forest",
      "Tropical evergreen forest",
      "Thorn forest"};
  if (worldGen.isOnSea(mx, my)) {
    // some information are irrelevant on sea
    tcod::print(
        console,
        {5, 47},
        tcod::stringf("Alt %5dm\n\nMove the mouse to scroll the map", (int)worldGen.getRealAltitude(mx, my)),
        &WHITE,
        nullptr);
  } else {
    tcod::print(
        console,
        {5, 47},
        tcod::stringf(
            "Alt %5dm  Prec %3dcm/sq. m/y  Temp %d deg C\nBiome : %s\nMove the mouse to scroll the map",
            (int)worldGen.getRealAltitude(mx, my),
            (int)worldGen.getPrecipitations(mx, my),
            (int)worldGen.getTemperature(mx, my),
            biomeNames[worldGen.getBiome(mx, my)]),
        &WHITE,
        nullptr);
  }
}

int main(int argc, char* argv[]) {
  auto tileset = tcod::load_tilesheet("data/fonts/terminal8x8_gs_tc.png", {32, 8}, tcod::CHARMAP_TCOD);
  auto console = tcod::new_console({{WIDTH, HEIGHT}});

  // initialize the game window
  TCOD_ContextParams params{};
  params.tcod_version = TCOD_COMPILEDVERSION;
  params.argc = argc;
  params.argv = argv;
  params.console = console.get();
  params.vsync = true;
  params.sdl_window_flags = SDL_WINDOW_RESIZABLE;
  params.window_title = "World generator";
  params.tileset = tileset.get();

  auto context = tcod::new_context(params);

  int desired_fps = 0;

  bool endCredits = false;
  // generate the world with all data (rain, temperature and so on...)
  worldGen.generate(NULL);
  // compute light intensity on ground (depends on light direction and ground slope)
  static const float lightDir[3] = {1.0f, 1.0f, 0.0f};
  worldGen.computeSunLight(lightDir);

  auto timer = tcod::Timer();

  while (true) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          std::exit(EXIT_SUCCESS);
          break;
        case SDL_MOUSEMOTION: {
          const auto mouse_xy_d = context->pixel_to_tile_coordinates(
              std::array<double, 2>{static_cast<double>(event.motion.x), static_cast<double>(event.motion.y)});
          mouse_subtile_xy = {static_cast<float>(mouse_xy_d.at(0)), static_cast<float>(mouse_xy_d.at(1))};
        } break;
        default:
          break;
      }
    }

    // update the game
    const float delta_time = timer.sync(desired_fps);
    update(delta_time);

    // render the game screen
    render(*console);
    // render libtcod credits
    if (!endCredits) endCredits = TCOD_console_credits_render_ex(console.get(), 4, 4, true, delta_time);
    // flush updates to screen
    context->present(*console);
  }
  return 0;
}
