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

#include <math.h>
#include <stdio.h>

// gas simulation
// based on Jos Stam, "Real-Time Fluid Dynamics for Games". Proceedings of the Game Developer Conference, March 2003.
// http://www.dgp.toronto.edu/people/stam/reality/Research/pub.html

static constexpr auto WIDTH = 50;
static constexpr auto HEIGHT = 50;

static constexpr auto WIDTHx2 = WIDTH * 2;
static constexpr auto HEIGHTx2 = HEIGHT * 2;
// use square map
static constexpr auto N = std::min(WIDTHx2, HEIGHTx2);
// store a 2D map in a 1D array
static constexpr auto SIZE = (N + 2) * (N + 2);
// convert x,y to array index
constexpr int IX(int x, int y) { return x + (N + 2) * y; }

// 2D velocity maps (current and previous)
static float u_current[SIZE], v_current[SIZE], u_prev[SIZE], v_prev[SIZE];
// density maps (current and previous)
static float dens[SIZE], dens_prev[SIZE];
TCODImage img(WIDTHx2, HEIGHTx2);

static constexpr auto VISCOSITY = 1E-6f;
static constexpr auto diff = 1E-5f;
static constexpr auto force = 40.0f;
static constexpr auto source = 5000.0f;
static float stepDelay = 0.0f;

static int player_x = N / 4, player_y = N / 4;

// set boundary conditions
void set_bnd(int b, float* x) {
  for (int i = 1; i <= N; i++) {
    // west and east walls
    x[IX(0, i)] = b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
    x[IX(N + 1, i)] = b == 1 ? -x[IX(N, i)] : x[IX(N, i)];
    // boundary doesn't work on north and south walls...
    // dunno why...
    x[IX(i, 0)] = b == 1 ? -x[IX(i, 1)] : x[IX(i, 1)];
    x[IX(i, N + 1)] = b == 1 ? -x[IX(i, N)] : x[IX(i, N)];
  }
  // boundary conditions at corners
  x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
  x[IX(0, N + 1)] = 0.5f * (x[IX(1, N + 1)] + x[IX(0, N)]);
  x[IX(N + 1, 0)] = 0.5f * (x[IX(N, 0)] + x[IX(N + 1, 1)]);
  x[IX(N + 1, N + 1)] = 0.5f * (x[IX(N, N + 1)] + x[IX(N + 1, N)]);
}

// update density map according to density sources
// x : density map
// s : density source map
// dt : elapsed time
void add_source(float* x, const float* s, float dt) {
  for (int i = 0; i < SIZE; i++) {
    x[i] += dt * s[i];
  }
}

// update density or velocity map for diffusion
// b : boundary width
// x : current density map
// x0 : previous density map
// diff : diffusion coef
// dt : elapsed time
void diffuse(int b, float* x, const float* x0, float diffusion_coef, float dt) {
  const float a = diffusion_coef * dt * N * N;
  for (int k = 0; k < 20; k++) {
    for (int i = 1; i <= N; i++) {
      for (int j = 1; j <= N; j++) {
        x[IX(i, j)] =
            (x0[IX(i, j)] + a * (x[IX(i - 1, j)] + x[IX(i + 1, j)] + x[IX(i, j - 1)] + x[IX(i, j + 1)])) / (1 + 4 * a);
      }
    }
    set_bnd(b, x);
  }
}

// update density map according to velocity map
// b : boundary width
// d : current density map
// d0 : previous density map
// u,v : current velocity map
// dt : elapsed time
void advect(int b, float* d, const float* d0, const float* u, const float* v, float dt) {
  const float dt0 = dt * N;
  for (int i = 1; i <= N; i++) {
    for (int j = 1; j <= N; j++) {
      float x = i - dt0 * u[IX(i, j)];
      float y = j - dt0 * v[IX(i, j)];
      if (x < 0.5) x = 0.5;
      if (x > N + 0.5) x = N + 0.5;
      const int i0 = (int)x;
      const int i1 = i0 + 1;
      if (y < 0.5) y = 0.5;
      if (y > N + 0.5) y = N + 0.5;
      const int j0 = (int)y;
      const int j1 = j0 + 1;
      const float s1 = x - i0;
      const float s0 = 1 - s1;
      const float t1 = y - j0;
      const float t0 = 1 - t1;
      d[IX(i, j)] = s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) + s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
    }
  }
  set_bnd(b, d);
}

void project(float* u, float* v, float* p, float* div) {
  const float h = 1.0f / N;
  for (int i = 1; i <= N; i++) {
    for (int j = 1; j <= N; j++) {
      div[IX(i, j)] = -0.5f * h * (u[IX(i + 1, j)] - u[IX(i - 1, j)] + v[IX(i, j + 1)] - v[IX(i, j - 1)]);
      p[IX(i, j)] = 0;
    }
  }
  set_bnd(0, div);
  set_bnd(0, p);

  for (int k = 0; k < 20; k++) {
    for (int i = 1; i <= N; i++) {
      for (int j = 1; j <= N; j++) {
        p[IX(i, j)] = (div[IX(i, j)] + p[IX(i - 1, j)] + p[IX(i + 1, j)] + p[IX(i, j - 1)] + p[IX(i, j + 1)]) / 4;
      }
    }
    set_bnd(0, p);
  }

  for (int i = 1; i <= N; i++) {
    for (int j = 1; j <= N; j++) {
      u[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) / h;
      v[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) / h;
    }
  }
  set_bnd(1, u);
  set_bnd(2, v);
}

// do all three density steps
void update_density(float* x, float* x0, const float* u, const float* v, float diffusion_coef, float dt) {
  add_source(x, x0, dt);
  std::swap(x0, x);
  diffuse(0, x, x0, diffusion_coef, dt);
  std::swap(x0, x);
  advect(0, x, x0, u, v, dt);
}

void update_velocity(float* u, float* v, float* u0, float* v0, float viscosity, float dt) {
  add_source(u, u0, dt);
  add_source(v, v0, dt);
  std::swap(u0, u);
  diffuse(1, u, u0, viscosity, dt);
  std::swap(v0, v);
  diffuse(2, v, v0, viscosity, dt);
  project(u, v, u0, v0);
  std::swap(u0, u);
  std::swap(v0, v);
  advect(1, u, u0, u0, v0, dt);
  advect(2, v, v0, u0, v0, dt);
  project(u, v, u0, v0);
}

void init() {
  memset(u_current, 0, sizeof(float) * SIZE);
  memset(v_current, 0, sizeof(float) * SIZE);
  memset(u_prev, 0, sizeof(float) * SIZE);
  memset(v_prev, 0, sizeof(float) * SIZE);
  for (int i = 0; i < SIZE; i++) dens[i] = 0.0f;
  memcpy(dens_prev, dens, sizeof(float) * SIZE);
}

void get_from_UI(float* d, float* u, float* v, float elapsed, TCOD_key_t, TCOD_mouse_t mouse) {
  float vx = 0.0f;
  float vy = 0.0f;

  stepDelay -= elapsed;
  if (stepDelay < 0.0f) {
    if (TCODConsole::isKeyPressed(TCODK_UP) && player_y > 0) {
      player_y--;
      vy -= force;
    }
    if (TCODConsole::isKeyPressed(TCODK_DOWN) && player_y < N / 2 - 1) {
      player_y++;
      vx += force;
    }
    if (TCODConsole::isKeyPressed(TCODK_LEFT) && player_x > 0) {
      player_x--;
      vx -= force;
    }
    if (TCODConsole::isKeyPressed(TCODK_RIGHT) && player_x < N / 2 - 1) {
      player_x++;
      vx += force;
    }
    stepDelay = 0.2f;  // move 5 cells per second
    // try to move smoke when you walk inside it. doesn't seem to work...
    u[IX(player_x * 2, player_y * 2)] = 5 * vx;
    v[IX(player_x * 2, player_y * 2)] = 5 * vy;
  }

  for (int i = 0; i < SIZE; ++i) {
    u[i] = v[i] = d[i] = 0.0f;
  }

  if (!mouse.lbutton && !mouse.rbutton) return;

  int i = mouse.cx * 2;
  int j = mouse.cy * 2;
  if (i < 1 || i > N || j < 1 || j > N) return;

  if (mouse.lbutton) {
    float dx = (float)(mouse.cx - player_x);
    float dy = (float)(mouse.cy - player_y);
    float l = sqrtf(dx * dx + dy * dy);
    if (l > 0) {
      l = 1.0f / l;
      dx *= l;
      dy *= l;
      u[IX(player_x * 2, player_y * 2)] = force * dx;
      v[IX(player_x * 2, player_y * 2)] = force * dy;
      d[IX(player_x * 2, player_y * 2)] = source;
    }
  }
}

void update(float elapsed, TCOD_key_t k, TCOD_mouse_t mouse) {
  get_from_UI(dens_prev, u_prev, v_prev, elapsed, k, mouse);
  update_velocity(u_current, v_current, u_prev, v_prev, VISCOSITY, elapsed);
  update_density(dens, dens_prev, u_current, v_current, diff, elapsed);
}

void render() {
  static const TCODColor deepBlue = TCODColor::darkestFlame;
  static const TCODColor highBlue = TCODColor::lightestYellow;
  for (int x = 0; x <= N; x++) {
    for (int y = 0; y <= N; y++) {
      float coef = (float)(dens[IX(x, y)] / 128.0f);
      coef = CLAMP(0.0f, 1.0f, coef);
      img.putPixel(x, y, TCODColor::lerp(deepBlue, highBlue, coef));
    }
  }
  img.blit2x(TCODConsole::root, 0, 0);
  TCODConsole::root->printf(2, HEIGHT - 2, "%4d fps", TCODSystem::getFps());
  TCODConsole::root->setDefaultForeground(TCODColor::white);
  TCODConsole::root->putChar(player_x, player_y, '@');
}

int main(int argc, char* argv[]) {
  // initialize the game window
  TCODConsole::initRoot(WIDTH, HEIGHT, "pyromancer flame spell v" VERSION, false, TCOD_RENDERER_OPENGL2);
  TCODSystem::setFps(25);
  TCODMouse::showCursor(true);

  bool endCredits = false;
  init();

  while (!TCODConsole::isWindowClosed()) {
    TCOD_key_t k;
    TCOD_mouse_t mouse;

    TCODSystem::checkForEvent(TCOD_EVENT_KEY | TCOD_EVENT_MOUSE, &k, &mouse);
    /*
                    v_prev[IX(N/2,0)] = 1.0f;
                    u_prev[IX(N/3,N/3)]=1.0f;
                    dens_prev[IX(N/2,0)] = 128.0f;
    */
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
    TCODConsole::root->printf(5, 49, "Arrows to move, left mouse button to cast");
    // render libtcod credits
    if (!endCredits) endCredits = TCODConsole::renderCredits(4, 4, true);
    // flush updates to screen
    TCODConsole::root->flush();
  }
  return 0;
}
