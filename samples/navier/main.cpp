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
#include <libtcod/timer.h>
#include <math.h>
#include <stdio.h>

#include <array>
#include <cstdbool>
#include <vector>

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
static std::array<float, SIZE> u_current, v_current, u_prev, v_prev;
// density maps (current and previous)
static std::array<float, SIZE> dens, dens_prev;
TCODImage img(WIDTHx2, HEIGHTx2);

static constexpr auto VISCOSITY = 1E-6f;
static constexpr auto DIFF = 1E-5f;
static constexpr auto FORCE = 12000.0f;
static constexpr auto SOURCE = 1250000.0f;
static float stepDelay = 0.0f;

static int player_x = N / 4, player_y = N / 4;

// set boundary conditions
void set_bnd(int b, std::array<float, SIZE>& x) {
  for (int i = 1; i <= N; ++i) {
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
void add_source(std::array<float, SIZE>& density, const std::array<float, SIZE>& source, float delta) {
  for (size_t i = 0; i < source.size(); ++i) {
    density[i] += delta * source[i];
  }
}

// update density or velocity map for diffusion
// b : boundary width
// x : current density map
// x0 : previous density map
// diff : diffusion coef
// dt : elapsed time
void diffuse(
    int b,
    std::array<float, SIZE>& density,
    const std::array<float, SIZE>& density_prev,
    float diffusion_coef,
    float delta_time) {
  const float a = diffusion_coef * delta_time * N * N;
  for (int k = 0; k < 20; ++k) {
    for (int i = 1; i <= N; ++i) {
      for (int j = 1; j <= N; ++j) {
        density[IX(i, j)] = (density_prev[IX(i, j)] + a * (density[IX(i - 1, j)] + density[IX(i + 1, j)] +
                                                           density[IX(i, j - 1)] + density[IX(i, j + 1)])) /
                            (1 + 4 * a);
      }
    }
    set_bnd(b, density);
  }
}

// update density map according to velocity map
// b : boundary width
// d : current density map
// d0 : previous density map
// u,v : current velocity map
// dt : elapsed time
void advect(
    int b,
    std::array<float, SIZE>& density,
    const std::array<float, SIZE>& density_prev,
    const std::array<float, SIZE>& velocity_u,
    const std::array<float, SIZE>& velocity_v,
    float delta_time) {
  const float dt0 = delta_time * N;
  for (int i = 1; i <= N; ++i) {
    for (int j = 1; j <= N; ++j) {
      float x = i - dt0 * velocity_u[IX(i, j)];
      float y = j - dt0 * velocity_v[IX(i, j)];
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
      density[IX(i, j)] = s0 * (t0 * density_prev[IX(i0, j0)] + t1 * density_prev[IX(i0, j1)]) +
                          s1 * (t0 * density_prev[IX(i1, j0)] + t1 * density_prev[IX(i1, j1)]);
    }
  }
  set_bnd(b, density);
}

void project(
    std::array<float, SIZE>& velocity_u,
    std::array<float, SIZE>& velocity_v,
    std::array<float, SIZE>& p,
    std::array<float, SIZE>& div) {
  const float h = 1.0f / N;
  for (int i = 1; i <= N; ++i) {
    for (int j = 1; j <= N; ++j) {
      div[IX(i, j)] =
          -0.5f * h *
          (velocity_u[IX(i + 1, j)] - velocity_u[IX(i - 1, j)] + velocity_v[IX(i, j + 1)] - velocity_v[IX(i, j - 1)]);
      p[IX(i, j)] = 0;
    }
  }
  set_bnd(0, div);
  set_bnd(0, p);

  for (int k = 0; k < 20; ++k) {
    for (int i = 1; i <= N; ++i) {
      for (int j = 1; j <= N; ++j) {
        p[IX(i, j)] = (div[IX(i, j)] + p[IX(i - 1, j)] + p[IX(i + 1, j)] + p[IX(i, j - 1)] + p[IX(i, j + 1)]) / 4;
      }
    }
    set_bnd(0, p);
  }

  for (int i = 1; i <= N; ++i) {
    for (int j = 1; j <= N; ++j) {
      velocity_u[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) / h;
      velocity_v[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) / h;
    }
  }
  set_bnd(1, velocity_u);
  set_bnd(2, velocity_v);
}

// do all three density steps
void update_density(
    std::array<float, SIZE>& density,
    std::array<float, SIZE>& density_prev,
    const std::array<float, SIZE>& velocity_u,
    const std::array<float, SIZE>& velocity_v,
    float diffusion_coef,
    float delta_time) {
  add_source(density, density_prev, delta_time);
  std::swap(density_prev, density);
  diffuse(0, density, density_prev, diffusion_coef, delta_time);
  std::swap(density_prev, density);
  advect(0, density, density_prev, velocity_u, velocity_v, delta_time);
}

void update_velocity(
    std::array<float, SIZE>& velocity_u,
    std::array<float, SIZE>& velocity_v,
    std::array<float, SIZE>& velocity_u_prev,
    std::array<float, SIZE>& velocity_v_prev,
    float viscosity,
    float delta_time) {
  add_source(velocity_u, velocity_u_prev, delta_time);
  add_source(velocity_v, velocity_v_prev, delta_time);
  std::swap(velocity_u_prev, velocity_u);
  diffuse(1, velocity_u, velocity_u_prev, viscosity, delta_time);
  std::swap(velocity_v_prev, velocity_v);
  diffuse(2, velocity_v, velocity_v_prev, viscosity, delta_time);
  project(velocity_u, velocity_v, velocity_u_prev, velocity_v_prev);
  std::swap(velocity_u_prev, velocity_u);
  std::swap(velocity_v_prev, velocity_v);
  advect(1, velocity_u, velocity_u_prev, velocity_u_prev, velocity_v_prev, delta_time);
  advect(2, velocity_v, velocity_v_prev, velocity_u_prev, velocity_v_prev, delta_time);
  project(velocity_u, velocity_v, velocity_u_prev, velocity_v_prev);
}

void init() {
  for (auto& it : u_current) it = 0;
  for (auto& it : v_current) it = 0;
  for (auto& it : u_prev) it = 0;
  for (auto& it : v_prev) it = 0;
  for (auto& it : dens) it = 0;
  for (auto& it : dens_prev) it = 0;
}

void get_from_UI(
    std::array<float, SIZE>& density,
    std::array<float, SIZE>& velocity_u,
    std::array<float, SIZE>& velocity_v,
    float delta_time,
    TCOD_Context& context) {
  float vx = 0.0f;
  float vy = 0.0f;

  const uint8_t* keyboard_state = SDL_GetKeyboardState(nullptr);

  stepDelay -= delta_time;
  if (stepDelay < 0.0f) {
    if ((keyboard_state[SDL_SCANCODE_UP] || keyboard_state[SDL_SCANCODE_W]) && player_y > 0) {
      --player_y;
      vy -= FORCE;
    }
    if ((keyboard_state[SDL_SCANCODE_DOWN] || keyboard_state[SDL_SCANCODE_S]) && player_y < N / 2 - 1) {
      ++player_y;
      vx += FORCE;
    }
    if ((keyboard_state[SDL_SCANCODE_LEFT] || keyboard_state[SDL_SCANCODE_A]) && player_x > 0) {
      --player_x;
      vx -= FORCE;
    }
    if ((keyboard_state[SDL_SCANCODE_RIGHT] || keyboard_state[SDL_SCANCODE_D]) && player_x < N / 2 - 1) {
      ++player_x;
      vx += FORCE;
    }
    stepDelay = 0.2f;  // move 5 cells per second
    // try to move smoke when you walk inside it. doesn't seem to work...
    velocity_u[IX(player_x * 2, player_y * 2)] = 5 * vx;
    velocity_v[IX(player_x * 2, player_y * 2)] = 5 * vy;
  }

  for (int i = 0; i < SIZE; ++i) {
    velocity_u[i] = velocity_v[i] = density[i] = 0.0f;
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
      velocity_u[IX(player_x * 2, player_y * 2)] += FORCE * dx * delta_time;
      velocity_v[IX(player_x * 2, player_y * 2)] += FORCE * dy * delta_time;
      density[IX(player_x * 2, player_y * 2)] += SOURCE * delta_time;
    }
  }
}

void update(float elapsed, TCOD_Context& context) {
  get_from_UI(dens_prev, u_prev, v_prev, elapsed, context);
  update_velocity(u_current, v_current, u_prev, v_prev, VISCOSITY, elapsed);
  update_density(dens, dens_prev, u_current, v_current, DIFF, elapsed);
}

static constexpr auto TEXT_COLOR = TCOD_ColorRGB{0, 0, 0};

void render(TCOD_Console& console) {
  static constexpr TCODColor deepBlue = {63, 15, 0};
  static constexpr TCODColor highBlue = {255, 255, 191};
  for (int x = 0; x <= N; ++x) {
    for (int y = 0; y <= N; ++y) {
      float coef = dens[IX(x, y)] / 128.0f;
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
  auto console = tcod::new_console(WIDTH, HEIGHT);
  TCOD_ContextParams params{};
  params.tcod_version = TCOD_COMPILEDVERSION;
  params.argc = argc;
  params.argv = argv;
  params.console = console.get();
  params.tileset = tileset.get();
  params.window_title = "pyromancer flame spell";
  params.sdl_window_flags = SDL_WINDOW_RESIZABLE;
  params.vsync = false;

  auto context = tcod::new_context(params);

  bool endCredits = false;
  init();

  auto timer = tcod::Timer();
  int desired_fps = 30;

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
            case SDL_SCANCODE_F1:
              desired_fps = 0;
              break;
            case SDL_SCANCODE_F2:
              desired_fps = 30;
              break;
            case SDL_SCANCODE_F3:
              desired_fps = 60;
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
    }
    // update the game
    const float delta_time = timer.sync(desired_fps);
    update(delta_time, *context);
    // render the game screen
    render(*console);
    tcod::print(
        *console,
        {1, HEIGHT - 2 - 6},
        tcod::stringf(
            "FPS:\n%6.2f mean\n%6.2f median\n%6.2f last\n%6.2f min\n%6.2f max\nlimit (F1=0,F2=30,F3=60): %2i fps",
            timer.get_mean_fps(),
            timer.get_median_fps(),
            timer.get_last_fps(),
            timer.get_min_fps(),
            timer.get_max_fps(),
            desired_fps),
        &TEXT_COLOR,
        nullptr);
    tcod::print(*console, {5, 49}, "Arrows to move, left mouse button to cast", &TEXT_COLOR, nullptr);
    // render libtcod credits
    if (!endCredits) endCredits = TCOD_console_credits_render_ex(console.get(), 4, 4, true, delta_time);
    context->present(*console);
  }
}
