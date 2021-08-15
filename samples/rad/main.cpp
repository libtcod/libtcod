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

#include <SDL.h>
#include <libtcod.h>

#include <cmath>

#include "bsp_helper.hpp"
#include "rad_shader.hpp"

static constexpr auto CON_WIDTH = 80;
static constexpr auto CON_HEIGHT = 50;

static constexpr auto MAP_WIDTH = 39;
static constexpr auto MAP_HEIGHT = 50;

static constexpr auto LIGHT_RADIUS = 10;
static constexpr auto CELL_REFLECTIVITY = 1.5f;
static constexpr auto CELL_SELF_ILLUMINATION = 0.4f;

static constexpr auto WHITE = TCODColor{255, 255, 255};

static TCODMap* map;
static BspHelper bsp;
static int player_x = 0, player_y = 0, playerBack;
static Shader* leftShader = NULL;
static Shader* rightShader = NULL;
static const TCODColor darkWall(50, 50, 150);
static const TCODColor lightWall(130, 110, 50);
static const TCODColor darkGround(0, 0, 100);
static const TCODColor lightGround(200, 180, 50);
static int torchIndex;

static float stdTime = 0.0f;
static float radTime = 0.0f;
static float stdLength = 0.0f;
static float radLength = 0.0f;
static int timeSecond;
static int framesCount = 0;

// gamma correction lookup table
static bool enableGammaCorrection = true;
static int gammaLookup[256];
static constexpr auto GAMMA = 1.0f / 2.2f;

// find the closest walkable position
void findPos(int* x, int* y) {
  for (; (*x) < MAP_WIDTH; (*x)++) {
    for (; (*y) < MAP_HEIGHT; (*y)++) {
      if (map->isWalkable((*x), (*y))) return;
    }
  }
  for (*x = 0; (*x) < MAP_WIDTH; (*x)++) {
    for (*y = 0; (*y) < MAP_HEIGHT; (*y)++) {
      if (map->isWalkable((*x), (*y))) return;
    }
  }
}

void init(TCOD_Console& console) {
  for (auto& tile : console) tile = {' ', {255, 255, 255, 255}, {0, 0, 0, 255}};
  // build the dungeon
  map = new TCODMap(MAP_WIDTH, MAP_HEIGHT);
  bsp.createBspDungeon(map, NULL);
  // empty map
  // map->clear(true,true);

  // create shaders
  leftShader = new StandardShader();
  rightShader = new PhotonShader(CELL_REFLECTIVITY, CELL_SELF_ILLUMINATION, 3);

  // put random lights
  for (int i = 0; i < 10; i++) {
    int lx = TCODRandom::getInstance()->getInt(1, MAP_WIDTH - 2);
    int ly = TCODRandom::getInstance()->getInt(1, MAP_HEIGHT - 2);
    findPos(&lx, &ly);
    leftShader->addLight(lx, ly, LIGHT_RADIUS, WHITE);
    rightShader->addLight(lx, ly, LIGHT_RADIUS, WHITE);
    console.at(lx, ly).ch = '*';
    console.at(lx + CON_WIDTH / 2, ly).ch = '*';
  }

  // find a starting position for the player
  findPos(&player_x, &player_y);
  playerBack = console.at(player_x, player_y).ch;

  console.at(player_x, player_y).ch = '@';
  console.at(player_x + CON_WIDTH / 2, player_y).ch = '@';

  // add the player's torch
  torchIndex = leftShader->addLight(player_x, player_y, 10, WHITE);
  rightShader->addLight(player_x, player_y, LIGHT_RADIUS, WHITE);

  // init shaders (must be done after adding lights for photon shader)
  leftShader->init(map);
  rightShader->init(map);

  timeSecond = SDL_GetTicks() / 1000;

  if (enableGammaCorrection) {
    for (int i = 0; i < 256; ++i) {
      const float v = i / 255.0f;
      const float correctedV = powf(v, GAMMA);
      gammaLookup[i] = (int)(correctedV * 255);
    }
  } else {
    for (int i = 0; i < 256; ++i) {
      gammaLookup[i] = i;
    }
  }
}

void render(TCOD_Console& console) {
  // compute lights
  ++framesCount;
  const uint32_t start = SDL_GetTicks();
  leftShader->compute();
  const uint32_t leftEnd = SDL_GetTicks();
  rightShader->compute();
  const uint32_t rightEnd = SDL_GetTicks();
  stdTime += (leftEnd - start) * 0.001f;
  radTime += (rightEnd - leftEnd) * 0.001f;
  if ((int)(start / 1000) != timeSecond) {
    timeSecond = start / 1000;
    stdLength = stdTime * 1000 / framesCount;
    radLength = radTime * 1000 / framesCount;
    stdTime = 0.0f;
    radTime = 0.0f;
    framesCount = 0;
  }

  for (int y = 0; y < MAP_HEIGHT; ++y) {
    for (int x = 0; x < MAP_WIDTH; ++x) {
      TCODColor darkCol, lightCol;
      // get the cell dark and lit colors
      if (map->isWalkable(x, y)) {
        darkCol = darkGround;
        lightCol = lightGround;
      } else {
        darkCol = darkWall;
        lightCol = lightWall;
      }
      // render left map
      // hack : for a better look, lights are white and we only use them as
      // a lerp coefficient between dark and light colors.
      // a true light model would multiply the light color with the cell color
      const TCODColor leftLight = leftShader->getLightColor(x, y);
      const TCODColor cellLeftCol = TCODColor::lerp(darkCol, lightCol, gammaLookup[leftLight.r] / 255.0f);
      console.at(x, y).bg = {cellLeftCol.r, cellLeftCol.g, cellLeftCol.b, 255};

      // render right map
      const TCODColor rightLight = rightShader->getLightColor(x, y);
      const TCODColor cellRightCol = TCODColor::lerp(darkCol, lightCol, gammaLookup[rightLight.r] / 255.0f);
      console.at(x + CON_WIDTH / 2, y).bg = {cellRightCol.r, cellRightCol.g, cellRightCol.b, 255};
    }
  }
  tcod::print(
      console,
      {console.w / 4, 0},
      tcod::stringf("Standard lighting %1.2fms", stdLength),
      nullptr,
      nullptr,
      TCOD_BKGND_SET,
      TCOD_CENTER);

  tcod::print(
      console,
      {3 * CON_WIDTH / 4, 0},
      tcod::stringf("Photon reactor %1.2fms", radLength),
      nullptr,
      nullptr,
      TCOD_BKGND_SET,
      TCOD_CENTER);
}

void move(TCOD_Console& console, int dx, int dy) {
  if (map->isWalkable(player_x + dx, player_y + dy)) {
    // restore the previous map char
    console.at(player_x, player_y).ch = playerBack;
    console.at(player_x + CON_WIDTH / 2, player_y).ch = playerBack;
    // move the player
    player_x += dx;
    player_y += dy;
    playerBack = console.at(player_x, player_y).ch;
    // render the player
    console.at(player_x, player_y).ch = '@';
    console.at(player_x + CON_WIDTH / 2, player_y).ch = '@';
    // update the player's torch position
    leftShader->updateLight(torchIndex, player_x, player_y, LIGHT_RADIUS, WHITE);
    rightShader->updateLight(torchIndex, player_x, player_y, LIGHT_RADIUS, WHITE);
  }
}

int main(int argc, char* argv[]) {
  auto console = tcod::new_console(80, 50);
  auto tileset = tcod::load_tilesheet("data/fonts/terminal8x8_gs_tc.png", {32, 8}, tcod::CHARMAP_TCOD);
  TCOD_ContextParams params{};
  params.tcod_version = TCOD_COMPILEDVERSION;
  params.console = console.get();
  params.tileset = tileset.get();
  params.vsync = true;
  params.sdl_window_flags = SDL_WINDOW_RESIZABLE;
  params.argc = argc;
  params.argv = argv;
  params.window_title = "Photon reactor - radiosity engine for roguelikes";

  auto context = tcod::new_context(params);

  init(*console);
  while (true) {
    render(*console);
    context->present(*console);
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          return 0;
          break;
        case SDL_KEYDOWN:
          switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_UP:
            case SDL_SCANCODE_KP_8:
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_K:
              move(*console, 0, -1);
              break;
            case SDL_SCANCODE_DOWN:
            case SDL_SCANCODE_KP_2:
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_J:
              move(*console, 0, 1);
              break;
            case SDL_SCANCODE_LEFT:
            case SDL_SCANCODE_KP_4:
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_H:
              move(*console, -1, 0);
              break;
            case SDL_SCANCODE_RIGHT:
            case SDL_SCANCODE_KP_6:
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_L:
              move(*console, 1, 0);
              break;
            case SDL_SCANCODE_PRINTSCREEN:
              context->save_screenshot(nullptr);
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
    }
  }
  return 0;
}
