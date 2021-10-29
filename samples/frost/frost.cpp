#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif  // __EMSCRIPTEN__
#include <SDL.h>
#include <libtcod.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

static constexpr auto TAU = 6.28318530718f;  // The number of radians in a turn.  Same as `2 * PI`.

std::array<TCODColor, 256> frost_gradient;  // Frost color gradient.

static constexpr auto GROW = 5000.0f;
static constexpr auto ANGLE_DELAY = 0.2f;
static constexpr auto FROST_LEVEL = 0.8f;
static constexpr auto SMOOTH = 0.3f;
static constexpr auto PIX_PER_FRAME = 6;
static constexpr auto RANGE = 10;  // Maximum range from a frost origin to calculate and apply frost effects.

struct Frost {
  explicit Frost(int x, int y) : origin_x{x}, origin_y{y} {}
  int origin_x = 0, origin_y = 0;  // The frost origin position.
  int best_x = 0, best_y = 0;  // A relative position to the frost closest to rx,ry.
  int rx = 0, ry = 0;  // A random relative direction.
  int border = 0;  // The total number of frames this effect has touched the border.
  float timer = 0;  // Seconds remaining until this particle changes direction.
};

class FrostManager {
 public:
  explicit FrostManager(int w, int h) : grid({w, h}), img({w, h}), width{w}, height{h} { clear(); }
  inline void clear() {
    for (auto& it : img) it = {0, 0, 0};
    for (auto& it : grid) it = 0;
  }
  inline void addFrost(int x, int y) {
    frost_objs.emplace_back(Frost(x, y));
    setValue(x, y, 1.0f);
  }
  inline void update(float delta_time) {
    auto update_func = [&](auto& it) { return !frost_update(it, delta_time); };
    frost_objs.erase(std::remove_if(frost_objs.begin(), frost_objs.end(), update_func), frost_objs.end());
  }
  inline void render(tcod::Console& console) {
    for (auto& it : frost_objs) frost_render(it);
    TCOD_image_blit_2x(TCODImage(img).get_data(), console.get(), 0, 0, 0, 0, -1, -1);
  }

  /**
      Returns true if x,y are within the bounds of this manager.
   */
  inline bool in_bounds(int x, int y) const noexcept { return 0 <= x && 0 <= y && x < width && y < height; }
  inline float getValue(int x, int y) const noexcept {
    if (!(in_bounds(x, y))) return 0.0f;
    return grid.at({x, y});
  }
  inline void setValue(int x, int y, float value) noexcept {
    if (!(in_bounds(x, y))) return;
    grid.at({x, y}) = value;
  }

 private:
  inline float getValue(const Frost& frost, int x, int y) const noexcept {
    return getValue(frost.origin_x - RANGE + x, frost.origin_y - RANGE + y);
  }
  inline void setValue(const Frost& frost, int x, int y, float value) noexcept {
    setValue(frost.origin_x - RANGE + x, frost.origin_y - RANGE + y, value);
  }

  /**
      Updates a frost particle with a given delta time.

      Returns false if this particle is to be removed.
   */
  inline bool frost_update(Frost& frost, float delta_time) {
    for (int i = PIX_PER_FRAME; i > 0; i--) {
      frost.timer -= delta_time;
      if (frost.timer <= 0) {
        // find a new random frost direction
        const float random_angle = TCODRandom::getInstance()->getFloat(0.0f, TAU);
        const float random_range = TCODRandom::getInstance()->getFloat(0, 2 * RANGE);
        frost.timer = ANGLE_DELAY;
        frost.rx = static_cast<int>(RANGE + random_range * cosf(random_angle));
        frost.ry = static_cast<int>(RANGE + random_range * sinf(random_angle));
        int minDist = std::numeric_limits<int>::max();
        // find closest frost pixel
        for (int cy = 1; cy < 2 * RANGE; cy++) {
          for (int cx = 1; cx < 2 * RANGE; cx++) {
            if (in_bounds(frost.origin_x - RANGE + cx, frost.origin_y - RANGE + cy)) {
              if (getValue(frost, cx, cy) > FROST_LEVEL) {
                const int dist = (cx - frost.rx) * (cx - frost.rx) + (cy - frost.ry) * (cy - frost.ry);
                if (dist < minDist) {
                  minDist = dist;
                  frost.best_x = cx;
                  frost.best_y = cy;
                }
              }
            }
          }
        }
      }
      // smoothing
      for (int cy = 0; cy < 2 * RANGE + 1; cy++) {
        if (frost.origin_y - RANGE + cy < height - 1 && frost.origin_y - RANGE + cy > 0) {
          for (int cx = 0; cx < 2 * RANGE + 1; cx++) {
            if (frost.origin_x - RANGE + cx < width - 1 && frost.origin_x - RANGE + cx > 0) {
              if (getValue(frost, cx, cy) < 1.0f) {
                float f = getValue(frost, cx, cy);
                const float old_f = f;
                f = std::max(f, getValue(frost, cx + 1, cy));
                f = std::max(f, getValue(frost, cx - 1, cy));
                f = std::max(f, getValue(frost, cx, cy + 1));
                f = std::max(f, getValue(frost, cx, cy - 1));
                setValue(frost, cx, cy, old_f + (f - old_f) * SMOOTH * delta_time);
              }
            }
          }
        }
      }
      int cur_x = frost.best_x;
      int cur_y = frost.best_y;
      // frosting
      TCODLine::init(cur_x, cur_y, frost.rx, frost.ry);
      TCODLine::step(&cur_x, &cur_y);

      if (in_bounds(frost.origin_x - RANGE + cur_x, frost.origin_y - RANGE + cur_y)) {
        const float frost_value = std::min(1.0f, getValue(frost, cur_x, cur_y) + GROW * delta_time);
        setValue(frost, cur_x, cur_y, frost_value);
        if (frost_value == 1.0f) {
          frost.best_x = cur_x;
          frost.best_y = cur_y;
          if (frost.best_x == frost.rx && frost.best_y == frost.ry) frost.timer = 0.0f;
          frost.timer = 0.0f;
          if (cur_x == 0 || cur_x == 2 * RANGE || cur_y == 0 || cur_y == 2 * RANGE) {
            frost.border++;
            if (frost.border == 20) {
              return false;  // Delete this particle.
            }
          }
        }
      } else
        frost.timer = 0.0f;
    }
    return true;
  }

  /**
      Renders a frost particle.
   */
  inline void frost_render(const Frost& frost) {
    for (int cy = std::max(frost.origin_y - RANGE, 0); cy < std::min(frost.origin_y + RANGE + 1, height); ++cy) {
      for (int cx = std::max(frost.origin_x - RANGE, 0); cx < std::min(frost.origin_x + RANGE + 1, width); ++cx) {
        const float f = getValue(frost, cx - (frost.origin_x - RANGE), cy - (frost.origin_y - RANGE));
        const int idx = std::max(0, std::min(static_cast<int>(f * 255), 255));
        img.at({cx, cy}) = static_cast<TCOD_ColorRGB>(frost_gradient.at(idx));
      }
    }
  }
  std::vector<Frost> frost_objs;  // A vector of frost effects.
  tcod::Matrix<float, 2> grid;  // A canvas for holding the freeze effect values.
  tcod::Matrix<TCOD_ColorRGB, 2> img;  // An image for storing the freeze colors.
  int width, height;  // The size of the managed frost map.
};

static constexpr int CONSOLE_WIDTH = 80;
static constexpr int CONSOLE_HEIGHT = 50;

tcod::ContextPtr context;

void main_loop() {
  static uint32_t last_time_ms = SDL_GetTicks();
  static FrostManager frostManager{CONSOLE_WIDTH * 2, CONSOLE_HEIGHT * 2};
  static auto console = tcod::Console{CONSOLE_WIDTH, CONSOLE_HEIGHT};
  frostManager.render(console);
  context->present(console);

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_BACKSPACE) frostManager.clear();
        break;
      case SDL_MOUSEBUTTONDOWN: {
        auto tile_xy = context->pixel_to_tile_coordinates(std::array<int, 2>{{event.motion.x, event.motion.y}});
        if (event.button.button == SDL_BUTTON_LEFT) {
          frostManager.addFrost(tile_xy.at(0) * 2, tile_xy.at(1) * 2);
        }
      }
      case SDL_MOUSEMOTION: {
        auto tile_xy = context->pixel_to_tile_coordinates(std::array<int, 2>{{event.motion.x, event.motion.y}});
        if (event.motion.state & SDL_BUTTON_LMASK) {
          frostManager.addFrost(tile_xy.at(0) * 2, tile_xy.at(1) * 2);
        }
      } break;
      case SDL_QUIT:
        std::exit(EXIT_SUCCESS);
        break;
    }
  }
  uint32_t current_time_ms = SDL_GetTicks();
  int delta_time_ms = std::max<int>(0, current_time_ms - last_time_ms);
  last_time_ms = current_time_ms;
  frostManager.update(delta_time_ms / 1000.0f);
}

void on_quit() {
  context = nullptr;
  SDL_Quit();
}

int main(int argc, char** argv) {
  std::atexit(on_quit);
  SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);
  auto tileset = tcod::load_tilesheet("data/fonts/terminal8x8_gs_tc.png", {32, 8}, tcod::CHARMAP_TCOD);
  TCOD_ContextParams params{};
  params.tcod_version = TCOD_COMPILEDVERSION;
  params.tileset = tileset.get();
  params.argc = argc;
  params.argv = argv;
  params.window_title = "frost test";
  params.columns = CONSOLE_WIDTH;
  params.rows = CONSOLE_HEIGHT;
  params.sdl_window_flags = SDL_WINDOW_RESIZABLE;
#ifdef __EMSCRIPTEN__
  params.renderer_type = TCOD_RENDERER_SDL2;
#endif
  params.vsync = true;
  try {
    context = tcod::new_context(params);
  } catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }

  // Initialize the frost color gradient.
  static constexpr std::array<int, 4> key_indexes{0, 60, 200, 255};
  static constexpr std::array<TCODColor, 4> key_colors{{{0, 0, 0}, {0, 0, 127}, {127, 127, 255}, {191, 191, 255}}};
  TCODColor::genMap(&frost_gradient[0], static_cast<int>(key_indexes.size()), key_colors.data(), key_indexes.data());
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(main_loop, 0, 0);
#else
  while (true) main_loop();
#endif
  return EXIT_SUCCESS;
}
