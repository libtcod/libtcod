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
#include <SDL.h>
#include <libtcod.h>
#include <math.h>
#include <stdio.h>

#include <cstdbool>

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
static constexpr auto force = 12000.0f;
static constexpr auto source = 1250000.0f;
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

void get_from_UI(float* d, float* u, float* v, float delta_time, TCOD_Context& context) {
  float vx = 0.0f;
  float vy = 0.0f;

  const uint8_t* keyboard_state = SDL_GetKeyboardState(nullptr);

  stepDelay -= delta_time;
  if (stepDelay < 0.0f) {
    if (keyboard_state[SDL_SCANCODE_UP] && player_y > 0) {
      player_y--;
      vy -= force;
    }
    if (keyboard_state[SDL_SCANCODE_DOWN] && player_y < N / 2 - 1) {
      player_y++;
      vx += force;
    }
    if (keyboard_state[SDL_SCANCODE_LEFT] && player_x > 0) {
      player_x--;
      vx -= force;
    }
    if (keyboard_state[SDL_SCANCODE_RIGHT] && player_x < N / 2 - 1) {
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

  int mouse_pixel_x;
  int mouse_pixel_y;
  uint32_t mouse_buttons = SDL_GetMouseState(&mouse_pixel_x, &mouse_pixel_y);
  const auto mouse_tile_xy = context.pixel_to_tile_coordinates(std::array<int, 2>{mouse_pixel_x, mouse_pixel_y});

  int subtile_x = mouse_tile_xy.at(0) * 2;
  int subtile_y = mouse_tile_xy.at(1) * 2;
  if (subtile_x < 1 || subtile_x > N || subtile_y < 1 || subtile_y > N) return;

  if (mouse_buttons & SDL_BUTTON_LMASK) {
    float dx = (float)(mouse_tile_xy.at(0) - player_x);
    float dy = (float)(mouse_tile_xy.at(1) - player_y);
    float l = sqrtf(dx * dx + dy * dy);
    if (l > 0) {
      l = 1.0f / l;
      dx *= l;
      dy *= l;
      u[IX(player_x * 2, player_y * 2)] += force * dx * delta_time;
      v[IX(player_x * 2, player_y * 2)] += force * dy * delta_time;
      d[IX(player_x * 2, player_y * 2)] += source * delta_time;
    }
  }
}

void update(float elapsed, TCOD_Context& context) {
  get_from_UI(dens_prev, u_prev, v_prev, elapsed, context);
  update_velocity(u_current, v_current, u_prev, v_prev, VISCOSITY, elapsed);
  update_density(dens, dens_prev, u_current, v_current, diff, elapsed);
}

static constexpr TCOD_ColorRGB TEXT_COLOR{0, 0, 0};

void render(TCOD_Console& console) {
  static constexpr TCODColor deepBlue = {63, 15, 0};
  static constexpr TCODColor highBlue = {255, 255, 191};
  for (int x = 0; x <= N; x++) {
    for (int y = 0; y <= N; y++) {
      float coef = (float)(dens[IX(x, y)] / 128.0f);
      coef = CLAMP(0.0f, 1.0f, coef);
      img.putPixel(x, y, TCODColor::lerp(deepBlue, highBlue, coef));
    }
  }
  TCOD_image_blit_2x(img.get_data(), &console, 0, 0, 0, 0, -1, -1);

  console.at(player_x, player_y).ch = '@';
  console.at(player_x, player_y).fg = {0, 0, 0, 255};
}

int main(int argc, char* argv[]) {
  // initialize the game window
  auto tileset = tcod::load_tilesheet("data/fonts/terminal8x8_gs_tc.png", {32, 8}, tcod::CHARMAP_TCOD);
  TCOD_ContextParams params{};
  params.tcod_version = TCOD_COMPILEDVERSION;
  params.argc = argc;
  params.argv = argv;
  params.columns = WIDTH;
  params.rows = HEIGHT;
  params.tileset = tileset.get();
  params.window_title = "pyromancer flame spell";
  params.sdl_window_flags = SDL_WINDOW_RESIZABLE;
  params.vsync = true;

  auto context = tcod::new_context(params);
  auto console = tcod::new_console(WIDTH, HEIGHT);

  bool endCredits = false;
  init();

  uint64_t last_time = SDL_GetPerformanceCounter();

  while (true) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          return 0;
        case SDL_KEYDOWN:
          switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_RETURN:
            case SDL_SCANCODE_RETURN2:
            case SDL_SCANCODE_KP_ENTER:
              if (event.key.keysym.mod & KMOD_ALT) {
                auto window = context->get_sdl_window();
                const uint32_t flags = SDL_GetWindowFlags(window);
                if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
                  SDL_SetWindowFullscreen(window, 0);
                } else {
                  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                }
              }
              break;
          }
          break;
      }
    }
    // update the game
    const uint64_t current_time = SDL_GetPerformanceCounter();
    const float delta_time =
        static_cast<float>(std::max<int64_t>(1, current_time - last_time)) / SDL_GetPerformanceFrequency();
    last_time = current_time;
    update(delta_time, *context);
    // render the game screen
    render(*console);
    tcod::printf(
        *console, 2, HEIGHT - 2, &TEXT_COLOR, nullptr, TCOD_BKGND_SET, TCOD_LEFT, "%4.0f fps", 1.0f / delta_time);
    tcod::print(*console, 5, 49, "Arrows to move, left mouse button to cast", &TEXT_COLOR, nullptr);
    // render libtcod credits
    if (!endCredits) endCredits = TCOD_console_credits_render_ex(console.get(), 4, 4, true, delta_time);
    context->present(*console);
  }
}
