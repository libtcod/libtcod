#include <SDL.h>
#include <libtcod.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>

std::array<TCODColor, 256> frostCol;

#define GROW 5000.0f
#define ANGLE_DELAY 0.2f
#define FROST_LEVEL 0.8f
#define SMOOTH 0.3f
#define PIX_PER_FRAME 6
#define RANGE 10

struct Frost;

class FrostManager {
 public:
  FrostManager(int w, int h);
  void addFrost(int x, int y);
  void update(float elapsed);
  void render(TCOD_Console& console);
  void clear();
  inline bool in_bounds(int cx, int cy) const noexcept { return 0 <= cx && 0 <= cy && cx < w && cy < h; }
  inline float getValue(int cx, int cy) {
    if (!(in_bounds(cx, cy))) return 0.0f;
    return grid.at(cx + cy * w);
  }
  inline void setValue(int cx, int cy, float v) {
    if (!(in_bounds(cx, cy))) return;
    grid.at(cx + cy * w) = v;
  }

 protected:
  friend struct Frost;
  std::vector<std::unique_ptr<Frost>> list;
  std::vector<float> grid;
  std::unique_ptr<TCODImage> img;
  int w, h;
};

struct Frost {
  int x, y, best_x, best_y, rx, ry;
  int border;
  FrostManager* manager;
  float timer;
  float ra;
  float rr;
  Frost(int x, int y, FrostManager* manager);
  inline float getValue(int cx, int cy) { return manager->getValue(x - RANGE + cx, y - RANGE + cy); }
  inline void setValue(int cx, int cy, float v) { manager->setValue(x - RANGE + cx, y - RANGE + cy, v); }
  bool update(float elapsed);
  void render(TCODImage& img);
};

FrostManager::FrostManager(int w, int h) : grid(w * h), img{std::make_unique<TCODImage>(w, h)}, w{w}, h{h} { clear(); }

void FrostManager::addFrost(int x, int y) {
  list.emplace_back(std::make_unique<Frost>(x, y, this));
  setValue(x, y, 1.0f);
}

void FrostManager::clear() {
  img->clear(TCODColor::black);
  for (auto& it : grid) it = 0;
}

void FrostManager::update(float elapsed) {
  auto it_end = std::remove_if(list.begin(), list.end(), [&](auto& it) { return !it->update(elapsed); });
  list.erase(it_end, list.end());
}

void FrostManager::render(TCOD_Console& console) {
  for (auto& it : list) it->render(*img);
  TCOD_image_blit_2x(img->get_data(), &console, 0, 0, 0, 0, -1, -1);
}

Frost::Frost(int x, int y, FrostManager* manager) : x{x}, y{y}, manager{manager}, border{0}, timer{0} {}

bool Frost::update(float elapsed) {
  for (int i = PIX_PER_FRAME; i > 0; i--) {
    timer -= elapsed;
    if (timer <= 0) {
      // find a new random frost direction
      ra = TCODRandom::getInstance()->getFloat(0.0f, 2 * 3.1415926f);
      rr = TCODRandom::getInstance()->getFloat(0, 2 * RANGE);
      timer = ANGLE_DELAY;
      rx = (int)(RANGE + rr * cosf(ra));
      ry = (int)(RANGE + rr * sinf(ra));
      int minDist = 100000;
      // find closest frost pixel
      for (int cx = 1; cx < 2 * RANGE; cx++) {
        if ((unsigned)(x - RANGE + cx) < (unsigned)manager->w) {
          for (int cy = 1; cy < 2 * RANGE; cy++) {
            if ((unsigned)(y - RANGE + cy) < (unsigned)manager->h) {
              float f = getValue(cx, cy);
              if (f > FROST_LEVEL) {
                int dist = (cx - rx) * (cx - rx) + (cy - ry) * (cy - ry);
                if (dist < minDist) {
                  minDist = dist;
                  best_x = cx;
                  best_y = cy;
                }
              }
            }
          }
        }
      }
    }
    // smoothing
    for (int cx = 0; cx < 2 * RANGE + 1; cx++) {
      if (x - RANGE + cx < manager->w - 1 && x - RANGE + cx > 0) {
        for (int cy = 0; cy < 2 * RANGE + 1; cy++) {
          if (y - RANGE + cy < manager->h - 1 && y - RANGE + cy > 0) {
            if (getValue(cx, cy) < 1.0f) {
              float f = getValue(cx, cy);
              float old_f = f;
              f = MAX(f, getValue(cx + 1, cy));
              f = MAX(f, getValue(cx - 1, cy));
              f = MAX(f, getValue(cx, cy + 1));
              f = MAX(f, getValue(cx, cy - 1));
              setValue(cx, cy, old_f + (f - old_f) * SMOOTH * elapsed);
            }
          }
        }
      }
    }
    int cur_x = best_x;
    int cur_y = best_y;
    // frosting
    TCODLine::init(cur_x, cur_y, rx, ry);
    TCODLine::step(&cur_x, &cur_y);
    if ((unsigned)(x - RANGE + cur_x) < (unsigned)manager->w && (unsigned)(y - RANGE + cur_y) < (unsigned)manager->h) {
      float f = getValue(cur_x, cur_y);
      f += GROW * elapsed;
      f = MIN(1.0f, f);
      setValue(cur_x, cur_y, f);
      if (f == 1.0f) {
        best_x = cur_x;
        best_y = cur_y;
        if (best_x == rx && best_y == ry) timer = 0.0f;
        timer = 0.0f;
        if (cur_x == 0 || cur_x == 2 * RANGE || cur_y == 0 || cur_y == 2 * RANGE) {
          border++;
          if (border == 20) return false;
        }
      }
    } else
      timer = 0.0f;
  }
  return true;
}
void Frost::render(TCODImage& img) {
  int w, h;
  img.getSize(&w, &h);
  for (int cy = std::max(y - RANGE, 0); cy < std::min(y + RANGE + 1, h); ++cy) {
    for (int cx = std::max(x - RANGE, 0); cx < std::min(x + RANGE + 1, w); ++cx) {
      float f = getValue(cx - (x - RANGE), cy - (y - RANGE));
      int idx = std::max(0, std::min(static_cast<int>(f * 255), 255));
      img.putPixel(cx, cy, frostCol.at(idx));
    }
  }
}

int main(int argc, char** argv) {
  tcod::ConsolePtr console = tcod::new_console(80, 50);
  TCOD_ContextParams params = {TCOD_COMPILEDVERSION};
  params.argc = argc;
  params.argv = argv;
  params.columns = console->w;
  params.rows = console->h;
  params.window_title = "frost test";
  params.sdl_window_flags = SDL_WINDOW_RESIZABLE;
  params.vsync = true;
  tcod::ContextPtr context = tcod::new_context(params);

  const std::array<int, 4> keys{0, 60, 200, 255};
  const std::array<TCODColor, 4> keyCols{
      TCODColor::black, TCODColor::darkerBlue, TCODColor::lighterBlue, TCODColor::lightestBlue};
  TCODColor::genMap(&frostCol[0], static_cast<int>(keys.size()), keyCols.data(), keys.data());

  uint32_t last_time_ms = SDL_GetTicks();
  FrostManager frostManager(160, 100);
  while (true) {
    frostManager.render(*console);
    context->present(*console);

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
  return 0;
}
