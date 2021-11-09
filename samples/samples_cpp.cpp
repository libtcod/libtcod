/*
 * libtcod CPP samples
 * This code demonstrates various usages of libtcod modules
 * It's in the public domain.
 */

// uncomment this to disable SDL sample (might cause compilation issues on some systems)
//#define NO_SDL_SAMPLE

#include <SDL.h>
#include <libtcod.h>
#include <libtcod/timer.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

// Abstract class for samples.
class Sample {
 public:
  virtual ~Sample() = default;
  virtual void on_enter() = 0;
  virtual void on_event(TCOD_key_t* key, TCOD_mouse_t* mouse) = 0;
  virtual void on_draw(TCOD_Console& console) = 0;
};

// a sample has a name and a rendering function
typedef struct sample_t {
  std::string name;
  std::shared_ptr<Sample> sample;
} sample_t;

// sample screen size
static constexpr auto SAMPLE_SCREEN_WIDTH = 46;
static constexpr auto SAMPLE_SCREEN_HEIGHT = 20;
// sample screen position
static constexpr auto SAMPLE_SCREEN_X = 20;
static constexpr auto SAMPLE_SCREEN_Y = 10;

static constexpr auto WHITE = tcod::ColorRGB{255, 255, 255};
static constexpr auto BLACK = tcod::ColorRGB{0, 0, 0};

static constexpr auto GREY = tcod::ColorRGB{127, 127, 127};
static constexpr auto LIGHT_BLUE = tcod::ColorRGB{63, 63, 255};
static constexpr auto LIGHT_YELLOW = tcod::ColorRGB{255, 255, 63};

static float delta_time;  // Global delta time.

// clang-format off
static const char* SAMPLE_MAP[] = {
    "##############################################",
    "#######################      #################",
    "#####################    #     ###############",
    "######################  ###        ###########",
    "##################      #####             ####",
    "################       ########    ###### ####",
    "###############      #################### ####",
    "################    ######                  ##",
    "########   #######  ######   #     #     #  ##",
    "########   ######      ###                  ##",
    "########                                    ##",
    "####       ######      ###   #     #     #  ##",
    "#### ###   ########## ####                  ##",
    "#### ###   ##########   ###########=##########",
    "#### ##################   #####          #####",
    "#### ###             #### #####          #####",
    "####           #     ####                #####",
    "########       #     #### #####          #####",
    "########       #####      ####################",
    "##############################################",
};
// clang-format on

// ***************************
// samples rendering functions
// ***************************

// the offscreen console in which the samples are rendered
tcod::Console sampleConsole(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT);

// ***************************
// true colors sample
// ***************************
class TrueColors : public Sample {
 public:
  void on_enter() override { sampleConsole.clear(); };
  void on_event(TCOD_key_t*, TCOD_mouse_t*) override {}
  void on_draw(TCOD_Console& console) override {
    enum { TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT };
    static auto cols = std::array<tcod::ColorRGB, 4>{
        {{50, 40, 150}, {240, 85, 5}, {50, 35, 240}, {10, 200, 130}}};  // random corner colors
    static auto dir_r = std::array<int8_t, 4>{1, -1, 1, 1};
    static auto dir_g = std::array<int8_t, 4>{1, -1, -1, 1};
    static auto dir_b = std::array<int8_t, 4>{1, 1, 1, -1};
    // ==== slightly modify the corner colors ====
    for (int c = 0; c < 4; ++c) {
      // move each corner color
      const int component = TCODRandom::getInstance()->getInt(0, 2);
      switch (component) {
        case 0:
          cols[c].r += 5 * dir_r[c];
          if (cols[c].r == 255)
            dir_r[c] = -1;
          else if (cols[c].r == 0)
            dir_r[c] = 1;
          break;
        case 1:
          cols[c].g += 5 * dir_g[c];
          if (cols[c].g == 255)
            dir_g[c] = -1;
          else if (cols[c].g == 0)
            dir_g[c] = 1;
          break;
        case 2:
          cols[c].b += 5 * dir_b[c];
          if (cols[c].b == 255)
            dir_b[c] = -1;
          else if (cols[c].b == 0)
            dir_b[c] = 1;
          break;
      }
    }

    // ==== scan the whole screen, interpolating corner colors ====
    for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
      const float x_coef = static_cast<float>(x) / (SAMPLE_SCREEN_WIDTH - 1);
      // get the current column top and bottom colors
      const auto top = tcod::ColorRGB{TCODColor::lerp(cols[TOPLEFT], cols[TOPRIGHT], x_coef)};
      const auto bottom = tcod::ColorRGB{TCODColor::lerp(cols[BOTTOMLEFT], cols[BOTTOMRIGHT], x_coef)};
      for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
        const float y_coef = static_cast<float>(y) / (SAMPLE_SCREEN_HEIGHT - 1);
        // get the current cell color
        auto curColor = tcod::ColorRGB{TCODColor::lerp(top, bottom, y_coef)};
        sampleConsole.at({x, y}).bg = curColor;
      }
    }

    // ==== print the text with a random color ====
    // get the background color at the text position
    auto textColor = tcod::ColorRGB{sampleConsole.at({SAMPLE_SCREEN_WIDTH / 2, 5}).bg};
    // and invert it
    textColor.r = 255 - textColor.r;
    textColor.g = 255 - textColor.g;
    textColor.b = 255 - textColor.b;
    // put random text (for performance tests)
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        int c;
        auto col = tcod::ColorRGB{sampleConsole.at({x, y}).bg};
        col = tcod::ColorRGB{TCODColor::lerp(col, BLACK, 0.5f)};
        // use colored character 255 on first and last lines
        if (y == 0 || y == SAMPLE_SCREEN_HEIGHT - 1) {
          c = 255;
        } else {
          c = TCODRandom::getInstance()->getInt('a', 'z');
        }
        sampleConsole.at({x, y}).ch = c;
        sampleConsole.at({x, y}).fg = col;
      }
    }
    // the background behind the text is slightly darkened using the BKGND_MULTIPLY flag
    tcod::print_rect(
        sampleConsole,
        {1, 5, SAMPLE_SCREEN_WIDTH - 2, SAMPLE_SCREEN_HEIGHT - 1},
        "The Doryen library uses 24 bits colors, for both background and foreground.",
        textColor,
        GREY,
        TCOD_CENTER,
        TCOD_BKGND_MULTIPLY);
  }
};

// ***************************
// offscreen console sample
// ***************************
class OffscreenConsole : public Sample {
 public:
  OffscreenConsole() : screenshot(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT) {}
  void on_enter() override {
    counter = SDL_GetTicks();
    // get a "screenshot" of the current sample screen
    screenshot = sampleConsole;
  }
  void on_event(TCOD_key_t*, TCOD_mouse_t*) override {}
  void on_draw(TCOD_Console& console) override {
    static tcod::Console secondary{SAMPLE_SCREEN_WIDTH / 2, SAMPLE_SCREEN_HEIGHT / 2};  // second screen
    static bool init = false;  // draw the secondary screen only the first time
    static int x = 0;  // secondary screen position
    static int y = 0;
    static int x_dir = 1;  // movement direction
    static int y_dir = 1;
    if (!init) {
      init = true;
      static const auto FRAME_DECOR_SINGLE_PIPE =
          std::array<int, 9>{0x250C, 0x2500, 0x2510, 0x2502, 0x20, 0x2502, 0x2514, 0x2500, 0x2518};
      tcod::draw_frame(
          secondary,
          {0, 0, SAMPLE_SCREEN_WIDTH / 2, SAMPLE_SCREEN_HEIGHT / 2},
          FRAME_DECOR_SINGLE_PIPE,
          std::nullopt,
          std::nullopt);
      tcod::print_rect(
          secondary,
          {1, 2, SAMPLE_SCREEN_WIDTH / 2 - 2, SAMPLE_SCREEN_HEIGHT / 2},
          "You can render to an offscreen console and blit in on another one, simulating alpha transparency.",
          std::nullopt,
          std::nullopt,
          TCOD_CENTER);
    }
    if (SDL_TICKS_PASSED(SDL_GetTicks(), counter + 1000)) {  // Once every second.
      counter = SDL_GetTicks();
      // move the secondary screen every 2 seconds
      x += x_dir;
      y += y_dir;
      if (x == SAMPLE_SCREEN_WIDTH / 2 + 5)
        x_dir = -1;
      else if (x == -5)
        x_dir = 1;
      if (y == SAMPLE_SCREEN_HEIGHT / 2 + 5)
        y_dir = -1;
      else if (y == -5)
        y_dir = 1;
    }
    // restore the initial screen
    sampleConsole = screenshot;
    // blit the overlapping screen
    tcod::blit(sampleConsole, secondary, {x, y}, {}, 1.0f, 0.75f);
  }

 private:
  int counter;
  tcod::Console screenshot;
};

// ***************************
// line drawing sample
// ***************************
static int bkFlag = TCOD_BKGND_SET;  // current blending mode
class LineListener : public TCODLineListener {
 public:
  bool putPoint(int x, int y) {
    if (x >= 0 && y >= 0 && x < SAMPLE_SCREEN_WIDTH && y < SAMPLE_SCREEN_HEIGHT) {
      TCOD_console_set_char_background(sampleConsole.get(), x, y, LIGHT_BLUE, static_cast<TCOD_bkgnd_flag_t>(bkFlag));
    }
    return true;
  }
};
class LineDrawingSample : public Sample {
 public:
  void on_enter() override {}
  void on_event(TCOD_key_t* key, TCOD_mouse_t*) override {
    if (key->vk == TCODK_ENTER || key->vk == TCODK_KPENTER) {
      // switch to the next blending mode
      ++bkFlag;
      if ((bkFlag & 0xff) > TCOD_BKGND_ALPH) bkFlag = TCOD_BKGND_NONE;
    }
  }
  void on_draw(TCOD_Console& console) override {
    static auto bk = tcod::Console{SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT};  // colored background
    static auto init = false;
    static const auto flagNames = std::vector<std::string>{
        "TCOD_BKGND_NONE",
        "TCOD_BKGND_SET",
        "TCOD_BKGND_MULTIPLY",
        "TCOD_BKGND_LIGHTEN",
        "TCOD_BKGND_DARKEN",
        "TCOD_BKGND_SCREEN",
        "TCOD_BKGND_COLOR_DODGE",
        "TCOD_BKGND_COLOR_BURN",
        "TCOD_BKGND_ADD",
        "TCOD_BKGND_ADDALPHA",
        "TCOD_BKGND_BURN",
        "TCOD_BKGND_OVERLAY",
        "TCOD_BKGND_ALPHA"};
    if ((bkFlag & 0xff) == TCOD_BKGND_ALPH) {
      // for the alpha mode, update alpha every frame
      const float alpha = (1.0f + cosf(SDL_GetTicks() / 1000.0f * 2.0f)) / 2.0f;
      bkFlag = TCOD_BKGND_ALPHA(alpha);
    } else if ((bkFlag & 0xff) == TCOD_BKGND_ADDA) {
      // for the add alpha mode, update alpha every frame
      const float alpha = (1.0f + cosf(SDL_GetTicks() / 1000.0f * 2.0f)) / 2.0f;
      bkFlag = TCOD_BKGND_ADDALPHA(alpha);
    }
    if (!init) {
      // initialize the colored background
      for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
          bk.at({x, y}).bg = tcod::ColorRGB{
              static_cast<uint8_t>(x * 255 / (SAMPLE_SCREEN_WIDTH - 1)),
              static_cast<uint8_t>((x + y) * 255 / (SAMPLE_SCREEN_WIDTH - 1 + SAMPLE_SCREEN_HEIGHT - 1)),
              static_cast<uint8_t>(y * 255 / (SAMPLE_SCREEN_HEIGHT - 1)),
          };
        }
      }
      init = true;
    }
    // blit the background
    sampleConsole = bk;
    // render the gradient
    const auto rect_y = static_cast<int>((SAMPLE_SCREEN_HEIGHT - 2) * ((1.0f + cosf(SDL_GetTicks() / 1000.0f)) / 2.0f));
    for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
      const auto col = tcod::ColorRGB{
          static_cast<uint8_t>(x * 255 / SAMPLE_SCREEN_WIDTH),
          static_cast<uint8_t>(x * 255 / SAMPLE_SCREEN_WIDTH),
          static_cast<uint8_t>(x * 255 / SAMPLE_SCREEN_WIDTH),
      };
      tcod::draw_rect(sampleConsole, {x, rect_y, 1, 3}, 0, std::nullopt, col, static_cast<TCOD_bkgnd_flag_t>(bkFlag));
    }
    // calculate the segment ends
    const float angle = SDL_GetTicks() / 1000.0f * 2.0f;
    const int xo = static_cast<int>(SAMPLE_SCREEN_WIDTH / 2 * (1 + cosf(angle)));
    const int yo = static_cast<int>(SAMPLE_SCREEN_HEIGHT / 2 + sinf(angle) * SAMPLE_SCREEN_WIDTH / 2);
    const int xd = static_cast<int>(SAMPLE_SCREEN_WIDTH / 2 * (1 - cosf(angle)));
    const int yd = static_cast<int>(SAMPLE_SCREEN_HEIGHT / 2 - sinf(angle) * SAMPLE_SCREEN_WIDTH / 2);

    // render the line
    LineListener listener;
    TCODLine::line(xo, yo, xd, yd, &listener);
    // print the current flag
    tcod::print(sampleConsole, {2, 2}, flagNames.at(bkFlag & 0xff) + " (ENTER to change)", WHITE, std::nullopt);
  }
};

// ***************************
// noise sample
// ***************************
class NoiseSample : public Sample {
 public:
  void on_enter() override {}
  void on_event(TCOD_key_t* key, TCOD_mouse_t*) override {
    // handle keypress
    if (key->vk == TCODK_NONE) return;
    if (key->c >= '1' && key->c <= '9') {
      // change the noise function
      func = key->c - '1';
    } else if (key->c == 'E' || key->c == 'e') {
      // increase hurst
      hurst += 0.1f;
      delete noise;
      noise = new TCODNoise(2, hurst, lacunarity);
    } else if (key->c == 'D' || key->c == 'd') {
      // decrease hurst
      hurst -= 0.1f;
      delete noise;
      noise = new TCODNoise(2, hurst, lacunarity);
    } else if (key->c == 'R' || key->c == 'r') {
      // increase lacunarity
      lacunarity += 0.5f;
      delete noise;
      noise = new TCODNoise(2, hurst, lacunarity);
    } else if (key->c == 'F' || key->c == 'f') {
      // decrease lacunarity
      lacunarity -= 0.5f;
      delete noise;
      noise = new TCODNoise(2, hurst, lacunarity);
    } else if (key->c == 'T' || key->c == 't') {
      // increase octaves
      octaves += 0.5f;
    } else if (key->c == 'G' || key->c == 'g') {
      // decrease octaves
      octaves -= 0.5f;
    } else if (key->c == 'Y' || key->c == 'y') {
      // increase zoom
      zoom += 0.2f;
    } else if (key->c == 'H' || key->c == 'h') {
      // decrease zoom
      zoom -= 0.2f;
    }
  }
  void on_draw(TCOD_Console& console) override {
    static const std::vector<std::string> funcName = {
        "1 : perlin noise       ",
        "2 : simplex noise      ",
        "3 : wavelet noise      ",
        "4 : perlin fbm         ",
        "5 : perlin turbulence  ",
        "6 : simplex fbm        ",
        "7 : simplex turbulence ",
        "8 : wavelet fbm        ",
        "9 : wavelet turbulence ",
    };
    static TCODImage* img = NULL;
    if (!noise) noise = new TCODNoise(2, hurst, lacunarity);
    if (!img) img = new TCODImage(SAMPLE_SCREEN_WIDTH * 2, SAMPLE_SCREEN_HEIGHT * 2);

    // texture animation
    const float dx = SDL_GetTicks() * 0.0005f;
    const float dy = dx;
    // render the 2d noise function
    for (int y = 0; y < 2 * SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < 2 * SAMPLE_SCREEN_WIDTH; ++x) {
        const float f[2] = {zoom * x / (2 * SAMPLE_SCREEN_WIDTH) + dx, zoom * y / (2 * SAMPLE_SCREEN_HEIGHT) + dy};
        float value = 0.0f;
        switch (func) {
          case PERLIN:
            value = noise->get(f, TCOD_NOISE_PERLIN);
            break;
          case SIMPLEX:
            value = noise->get(f, TCOD_NOISE_SIMPLEX);
            break;
          case WAVELET:
            value = noise->get(f, TCOD_NOISE_WAVELET);
            break;
          case FBM_PERLIN:
            value = noise->getFbm(f, octaves, TCOD_NOISE_PERLIN);
            break;
          case TURBULENCE_PERLIN:
            value = noise->getTurbulence(f, octaves, TCOD_NOISE_PERLIN);
            break;
          case FBM_SIMPLEX:
            value = noise->getFbm(f, octaves, TCOD_NOISE_SIMPLEX);
            break;
          case TURBULENCE_SIMPLEX:
            value = noise->getTurbulence(f, octaves, TCOD_NOISE_SIMPLEX);
            break;
          case FBM_WAVELET:
            value = noise->getFbm(f, octaves, TCOD_NOISE_WAVELET);
            break;
          case TURBULENCE_WAVELET:
            value = noise->getTurbulence(f, octaves, TCOD_NOISE_WAVELET);
            break;
        }
        const auto c = static_cast<uint8_t>((value + 1.0f) / 2.0f * 255);
        // use a bluish color
        img->putPixel(x, y, tcod::ColorRGB{static_cast<uint8_t>(c / 2), static_cast<uint8_t>(c / 2), c});
      }
    }
    // blit the noise image on the console with subcell resolution
    tcod::draw_quartergraphics(sampleConsole, *img);
    // draw a transparent rectangle
    tcod::draw_rect(sampleConsole, {2, 2, 23, (func <= WAVELET ? 10 : 13)}, 0, std::nullopt, GREY, TCOD_BKGND_MULTIPLY);
    for (int y = 2; y < 2 + (func <= WAVELET ? 10 : 13); ++y) {
      for (int x = 2; x < 2 + 23; ++x) {
        sampleConsole.at({x, y}).fg.r /= 2;
        sampleConsole.at({x, y}).fg.g /= 2;
        sampleConsole.at({x, y}).fg.b /= 2;
      }
    }
    // draw the text
    for (int curfunc = PERLIN; curfunc <= TURBULENCE_WAVELET; ++curfunc) {
      const auto fg = curfunc == func ? WHITE : GREY;
      const auto bg = curfunc == func ? std::optional{LIGHT_BLUE} : std::nullopt;
      tcod::print(sampleConsole, {2, 2 + curfunc}, funcName.at(curfunc), fg, bg);
    }
    // draw parameters
    tcod::print(sampleConsole, {2, 11}, tcod::stringf("Y/H : zoom (%2.1f)", zoom), WHITE, std::nullopt);
    if (func > WAVELET) {
      tcod::print(sampleConsole, {2, 12}, tcod::stringf("E/D : hurst (%2.1f)", hurst), WHITE, std::nullopt);
      tcod::print(sampleConsole, {2, 13}, tcod::stringf("R/F : lacunarity (%2.1f)", lacunarity), WHITE, std::nullopt);
      tcod::print(sampleConsole, {2, 14}, tcod::stringf("T/G : octaves (%2.1f)", octaves), WHITE, std::nullopt);
    }
  }

 private:
  enum {
    PERLIN,
    SIMPLEX,
    WAVELET,
    FBM_PERLIN,
    TURBULENCE_PERLIN,
    FBM_SIMPLEX,
    TURBULENCE_SIMPLEX,
    FBM_WAVELET,
    TURBULENCE_WAVELET
  };  // which function we render
  int func = PERLIN;
  TCODNoise* noise = NULL;
  float octaves = 4.0f;
  float hurst = TCOD_NOISE_DEFAULT_HURST;
  float lacunarity = TCOD_NOISE_DEFAULT_LACUNARITY;
  float zoom = 3.0f;
};

// ***************************
// fov sample
// ***************************

static constexpr auto CHAR_WINDOW = 0x2550;  // "═" glyph.
static const std::vector<const char*> algo_names{
    "BASIC               ",
    "DIAMOND             ",
    "SHADOW              ",
    "PERMISSIVE0         ",
    "PERMISSIVE1         ",
    "PERMISSIVE2         ",
    "PERMISSIVE3         ",
    "PERMISSIVE4         ",
    "PERMISSIVE5         ",
    "PERMISSIVE6         ",
    "PERMISSIVE7         ",
    "PERMISSIVE8         ",
    "RESTRICTIVE         ",
    "SYMMETRIC_SHADOWCAST",
};

class FOVSample : public Sample {
 public:
  void on_enter() override {}
  /// Attempt to move the player.
  void try_move(int dx, int dy) noexcept {
    const int dest_x = px + dx;
    const int dest_y = py + dy;
    if (SAMPLE_MAP[dest_y][dest_x] == ' ') {
      px = dest_x;
      py = dest_y;
    }
  }
  void on_event(TCOD_key_t* key, TCOD_mouse_t*) override {
    if (key->c == 'I' || key->c == 'i') {
      try_move(0, -1);  // player move north
    } else if (key->c == 'K' || key->c == 'k') {
      try_move(0, 1);  // player move south
    } else if (key->c == 'J' || key->c == 'j') {
      try_move(-1, 0);  // player move west
    } else if (key->c == 'L' || key->c == 'l') {
      try_move(1, 0);  // player move east
    } else if (key->c == 'T' || key->c == 't') {
      torch = !torch;  // enable/disable the torch fx
    } else if (key->c == 'W' || key->c == 'w') {
      light_walls = !light_walls;
    } else if (key->c == '=' || key->c == '-') {
      algonum += key->c == '=' ? 1 : -1;
      algonum = (algonum + NB_FOV_ALGORITHMS) % NB_FOV_ALGORITHMS;
    }
  }
  void on_draw(TCOD_Console& console) override {
    static constexpr auto TORCH_RADIUS = 10.0f;
    static constexpr auto SQUARED_TORCH_RADIUS = (TORCH_RADIUS * TORCH_RADIUS);
    static constexpr auto darkWall = tcod::ColorRGB{0, 0, 100};
    static constexpr auto lightWall = tcod::ColorRGB{130, 110, 50};
    static constexpr auto darkGround = tcod::ColorRGB{50, 50, 150};
    static constexpr auto lightGround = tcod::ColorRGB{200, 180, 50};
    // draw the help text & player @
    sampleConsole.clear(0x20, BLACK, BLACK);
    tcod::print(
        sampleConsole,
        {1, 0},
        tcod::stringf(
            "IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s",
            torch ? "on " : "off",
            light_walls ? "on " : "off",
            algo_names.at(algonum)),
        WHITE,
        std::nullopt);
    sampleConsole.at({px, py}).ch = '@';

    // draw windows
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        if (SAMPLE_MAP[y][x] == '=') {
          sampleConsole.at({x, y}).ch = CHAR_WINDOW;
        }
      }
    }
    if (!map) {
      // initialize the map for the fov toolkit
      map = new TCODMap(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT);
      for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
          if (SAMPLE_MAP[y][x] == ' ')
            map->setProperties(x, y, true, true);  // ground
          else if (SAMPLE_MAP[y][x] == '=')
            map->setProperties(x, y, true, false);  // window
        }
      }
      // 1d noise used for the torch flickering
      noise = new TCODNoise(1);
    }
    // calculate the field of view from the player position
    map->computeFov(px, py, torch ? (int)(TORCH_RADIUS) : 0, light_walls, (TCOD_fov_algorithm_t)algonum);
    // torch position & intensity variation
    float dx = 0.0f, dy = 0.0f, di = 0.0f;
    if (torch) {
      // slightly change the perlin noise parameter
      torch_x += 0.2f;
      // randomize the light position between -1.5 and 1.5
      float tdx = torch_x + 20.0f;
      dx = noise->get(&tdx) * 1.5f;
      tdx += 30.0f;
      dy = noise->get(&tdx) * 1.5f;
      // randomize the light intensity between -0.2 and 0.2
      di = 0.2f * noise->get(&torch_x);
    }
    // draw the dungeon
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        const bool visible = map->isInFov(x, y);
        const bool wall = SAMPLE_MAP[y][x] == '#';
        if (!visible) {
          sampleConsole.at({x, y}).bg = wall ? darkWall : darkGround;
        } else {
          tcod::ColorRGB light;
          if (!torch) {
            light = wall ? lightWall : lightGround;
          } else {
            // torch flickering fx
            tcod::ColorRGB base = (wall ? darkWall : darkGround);
            light = (wall ? lightWall : lightGround);
            // cell distance to torch (squared)
            const float r = (float)((x - px + dx) * (x - px + dx) + (y - py + dy) * (y - py + dy));
            if (r < SQUARED_TORCH_RADIUS) {
              // l = 1.0 at player position, 0.0 at a radius of 10 cells
              const float l = std::clamp(0.0f, 1.0f, (SQUARED_TORCH_RADIUS - r) / SQUARED_TORCH_RADIUS + di);
              // interpolate the color
              base = tcod::ColorRGB{TCODColor::lerp(base, light, l)};
            }
            light = base;
          }
          sampleConsole.at({x, y}).bg = light;
        }
      }
    }
  }

 private:
  int px = 20, py = 10;  // player position
  bool torch = false;  // torch fx on ?
  TCODMap* map = NULL;
  TCODNoise* noise = NULL;
  bool light_walls = true;
  int algonum = 0;
  float torch_x = 0.0f;  // torch light position in the perlin noise
};

// ***************************
// image sample
// ***************************
class ImageSample : public Sample {
 public:
  void on_enter() override {}
  void on_event(TCOD_key_t*, TCOD_mouse_t*) {}
  void on_draw(TCOD_Console& console) override {
    static TCODImage *img = NULL, *circle = NULL;
    if (img == NULL) {
      img = new TCODImage("data/img/skull.png");
      img->setKeyColor(BLACK);
      circle = new TCODImage("data/img/circle.png");
    }
    sampleConsole.clear();
    const float x = SAMPLE_SCREEN_WIDTH / 2 + cosf(SDL_GetTicks() / 1000.0f) * 10.0f;
    const float y = (float)(SAMPLE_SCREEN_HEIGHT / 2);
    const float scale_x = 0.2f + 1.8f * (1.0f + cosf(SDL_GetTicks() / 1000.0f / 2.0f)) / 2.0f;
    const float scale_y = scale_x;
    const float angle = SDL_GetTicks() / 1000.0f;
    const long elapsed = SDL_GetTicks() / 2000;
    if (elapsed & 1) {
      // split the color channels of circle.png
      // the red channel
      tcod::draw_rect(sampleConsole, {0, 3, 15, 15}, 0, std::nullopt, {{255, 0, 0}});
      circle->blitRect(sampleConsole, 0, 3, -1, -1, TCOD_BKGND_MULTIPLY);
      // the green channel
      tcod::draw_rect(sampleConsole, {15, 3, 15, 15}, 0, std::nullopt, {{0, 255, 0}});
      circle->blitRect(sampleConsole, 15, 3, -1, -1, TCOD_BKGND_MULTIPLY);
      // the blue channel
      tcod::draw_rect(sampleConsole, {30, 3, 15, 15}, 0, std::nullopt, {{0, 0, 255}});
      circle->blitRect(sampleConsole, 30, 3, -1, -1, TCOD_BKGND_MULTIPLY);
    } else {
      // render circle.png with normal blitting
      circle->blitRect(sampleConsole, 0, 3, -1, -1, TCOD_BKGND_SET);
      circle->blitRect(sampleConsole, 15, 3, -1, -1, TCOD_BKGND_SET);
      circle->blitRect(sampleConsole, 30, 3, -1, -1, TCOD_BKGND_SET);
    }
    img->blit(sampleConsole, x, y, TCOD_BKGND_SET, scale_x, scale_y, angle);
  }
};

// ***************************
// mouse sample
// ***************************/
class MouseSample : public Sample {
 public:
  void on_enter() override {
    SDL_WarpMouseInWindow(NULL, 320, 200);
    SDL_ShowCursor(1);
  }
  void on_event(TCOD_key_t* key, TCOD_mouse_t* mouse) override {
    sampleConsole.clear(0x20, LIGHT_YELLOW, GREY);
    if (mouse->lbutton_pressed) left_button = !left_button;
    if (mouse->rbutton_pressed) right_button = !right_button;
    if (mouse->mbutton_pressed) middle_button = !middle_button;
    tcod::print(
        sampleConsole,
        {1, 1},
        tcod::stringf(
            "%s\n"
            "Mouse position : %4dx%4d %s\n"
            "Mouse cell     : %4dx%4d\n"
            "Mouse movement : %4dx%4d\n"
            "Left button    : %s (toggle %s)\n"
            "Right button   : %s (toggle %s)\n"
            "Middle button  : %s (toggle %s)\n"
            "Wheel          : %s\n",
            TCODConsole::isActive() ? "" : "APPLICATION INACTIVE",
            mouse->x,
            mouse->y,
            TCODConsole::hasMouseFocus() ? "" : "OUT OF FOCUS",
            mouse->cx,
            mouse->cy,
            mouse->dx,
            mouse->dy,
            mouse->lbutton ? " ON" : "OFF",
            left_button ? " ON" : "OFF",
            mouse->rbutton ? " ON" : "OFF",
            right_button ? " ON" : "OFF",
            mouse->mbutton ? " ON" : "OFF",
            middle_button ? " ON" : "OFF",
            mouse->wheel_up ? "UP" : (mouse->wheel_down ? "DOWN" : "")),
        std::nullopt,
        std::nullopt);
    tcod::print(sampleConsole, {1, 10}, "1 : Hide cursor\n2 : Show cursor", std::nullopt, std::nullopt);
    if (key->c == '1') {
      SDL_ShowCursor(0);
    } else if (key->c == '2') {
      SDL_ShowCursor(1);
    }
  }
  void on_draw(TCOD_Console& console) override {}

 private:
  bool left_button = false, right_button = false, middle_button = false;
};

// ***************************
// path sample
// ***************************
class PathfinderSample : public Sample {
 public:
  void on_enter() override {}
  /// Attempt to move the player.
  void try_move(int dx, int dy) noexcept {
    const int dest_x = player_x_ + dx;
    const int dest_y = player_y_ + dy;
    if (SAMPLE_MAP[dest_y][dest_x] == ' ') {
      player_x_ = dest_x;
      player_y_ = dest_y;
    }
  }
  void on_event(TCOD_key_t* key, TCOD_mouse_t* mouse) override {
    if ((key->c == 'I' || key->c == 'i') && dest_y_ > 0) {
      dest_y_ -= 1;  // destination move north
    } else if ((key->c == 'K' || key->c == 'k') && dest_y_ < SAMPLE_SCREEN_HEIGHT - 1) {
      dest_y_ += 1;  // destination move south
    } else if ((key->c == 'J' || key->c == 'j') && dest_x_ > 0) {
      dest_x_ -= 1;  // destination move west
    } else if ((key->c == 'L' || key->c == 'l') && dest_x_ < SAMPLE_SCREEN_WIDTH - 1) {
      dest_x_ += 1;  // destination move east
    } else if (key->vk == TCODK_TAB) {
      usingAstar = !usingAstar;
    }
    const int mx = mouse->cx - SAMPLE_SCREEN_X;
    const int my = mouse->cy - SAMPLE_SCREEN_Y;
    if (mx >= 0 && mx < SAMPLE_SCREEN_WIDTH && my >= 0 && my < SAMPLE_SCREEN_HEIGHT &&
        (dest_x_ != mx || dest_y_ != my)) {
      dest_x_ = mx;
      dest_y_ = my;
    }
  }
  void on_draw(TCOD_Console& console) override {
    static constexpr auto darkWall = tcod::ColorRGB{0, 0, 100};
    static constexpr auto darkGround = tcod::ColorRGB{50, 50, 150};
    static constexpr auto lightGround = tcod::ColorRGB{200, 180, 50};
    // draw the help text & player @
    sampleConsole.clear();
    sampleConsole.at({dest_x_, dest_y_}).ch = '+';
    sampleConsole.at({player_x_, player_y_}).ch = '@';
    tcod::print(sampleConsole, {1, 1}, "IJKL / mouse :\nmove destination\nTAB : A*/dijkstra", WHITE, std::nullopt);
    tcod::print(sampleConsole, {1, 4}, usingAstar ? "Using : A*" : "Using : Dijkstra", WHITE, std::nullopt);
    // draw windows
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        if (SAMPLE_MAP[y][x] == '=') {
          sampleConsole.at({x, y}).ch = CHAR_WINDOW;
        }
      }
    }
    if (!map) {
      // initialize the map for the fov toolkit
      map = new TCODMap(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT);
      for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
          if (SAMPLE_MAP[y][x] == ' ')
            map->setProperties(x, y, true, true);  // ground
          else if (SAMPLE_MAP[y][x] == '=')
            map->setProperties(x, y, true, false);  // window
        }
      }
      path = new TCODPath(map);
      dijkstra = new TCODDijkstra(map);
    }
    // Recalculate the path.
    if (usingAstar) {
      path->compute(player_x_, player_y_, dest_x_, dest_y_);
    } else {
      dijkstraDist = 0.0f;
      // compute the distance grid
      dijkstra->compute(player_x_, player_y_);
      // get the maximum distance (needed for ground shading only)
      for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
          const float d = dijkstra->getDistance(x, y);
          if (d > dijkstraDist) dijkstraDist = d;
        }
      }
      // compute the path
      dijkstra->setPath(dest_x_, dest_y_);
    }
    // draw the dungeon
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        const bool wall = SAMPLE_MAP[y][x] == '#';
        sampleConsole.at({x, y}).bg = wall ? darkWall : darkGround;
      }
    }
    // draw the path
    if (usingAstar) {
      for (int i = 0; i < path->size(); ++i) {
        int x, y;
        path->get(i, &x, &y);
        sampleConsole.at({x, y}).bg = lightGround;
      }
    } else {
      for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
          const bool wall = SAMPLE_MAP[y][x] == '#';
          if (!wall) {
            const float d = dijkstra->getDistance(x, y);
            sampleConsole.at({x, y}).bg =
                tcod::ColorRGB{TCODColor::lerp(lightGround, darkGround, 0.9f * d / dijkstraDist)};
          }
        }
      }
      for (int i = 0; i < dijkstra->size(); ++i) {
        int x, y;
        dijkstra->get(i, &x, &y);
        sampleConsole.at({x, y}).bg = lightGround;
      }
    }
    // move the creature
    busy -= delta_time;
    if (busy <= 0.0f) {
      busy = 0.2f;
      if (usingAstar) {
        if (!path->isEmpty()) {
          path->walk(&player_x_, &player_y_, true);
        }
      } else {
        if (!dijkstra->isEmpty()) {
          dijkstra->walk(&player_x_, &player_y_);
        }
      }
    }
  }

 private:
  int player_x_ = 20, player_y_ = 10;  // player position
  int dest_x_ = 24, dest_y_ = 1;  // destination
  TCODMap* map = NULL;
  TCODPath* path = NULL;
  bool usingAstar = true;
  float dijkstraDist = 0;
  TCODDijkstra* dijkstra = NULL;
  float busy;
  int oldChar = ' ';
};

// ***************************
// bsp sample
// ***************************

static int bspDepth = 8;
static int minRoomSize = 4;
static bool randomRoom = false;  // a room fills a random part of the node or the maximum available space ?
static bool roomWalls = true;  // if true, there is always a wall on north & west side of a room
typedef char map_t[SAMPLE_SCREEN_WIDTH][SAMPLE_SCREEN_HEIGHT];

// draw a vertical line
void vline(map_t* map, int x, int y1, int y2) {
  int y = y1;
  int dy = (y1 > y2 ? -1 : 1);
  (*map)[x][y] = ' ';
  if (y1 == y2) return;
  do {
    y += dy;
    (*map)[x][y] = ' ';
  } while (y != y2);
}

// draw a vertical line up until we reach an empty space
void vline_up(map_t* map, int x, int y) {
  while (y >= 0 && (*map)[x][y] != ' ') {
    (*map)[x][y] = ' ';
    y--;
  }
}

// draw a vertical line down until we reach an empty space
void vline_down(map_t* map, int x, int y) {
  while (y < SAMPLE_SCREEN_HEIGHT && (*map)[x][y] != ' ') {
    (*map)[x][y] = ' ';
    y++;
  }
}

// draw a horizontal line
void hline(map_t* map, int x1, int y, int x2) {
  int x = x1;
  int dx = (x1 > x2 ? -1 : 1);
  (*map)[x][y] = ' ';
  if (x1 == x2) return;
  do {
    x += dx;
    (*map)[x][y] = ' ';
  } while (x != x2);
}

// draw a horizontal line left until we reach an empty space
void hline_left(map_t* map, int x, int y) {
  while (x >= 0 && (*map)[x][y] != ' ') {
    (*map)[x][y] = ' ';
    x--;
  }
}

// draw a horizontal line right until we reach an empty space
void hline_right(map_t* map, int x, int y) {
  while (x < SAMPLE_SCREEN_WIDTH && (*map)[x][y] != ' ') {
    (*map)[x][y] = ' ';
    x++;
  }
}

// the class building the dungeon from the bsp nodes
//#include <stdio.h>
class BspListener : public ITCODBspCallback {
 public:
  bool visitNode(TCODBsp* node, void* userData) {
    map_t* map = (map_t*)userData;
    if (node->isLeaf()) {
      // calculate the room size
      int minx = node->x + 1;
      int maxx = node->x + node->w - 1;
      int miny = node->y + 1;
      int maxy = node->y + node->h - 1;
      if (!roomWalls) {
        if (minx > 1) minx--;
        if (miny > 1) miny--;
      }
      if (maxx == SAMPLE_SCREEN_WIDTH - 1) maxx--;
      if (maxy == SAMPLE_SCREEN_HEIGHT - 1) maxy--;
      if (randomRoom) {
        minx = TCODRandom::getInstance()->getInt(minx, maxx - minRoomSize + 1);
        miny = TCODRandom::getInstance()->getInt(miny, maxy - minRoomSize + 1);
        maxx = TCODRandom::getInstance()->getInt(minx + minRoomSize - 1, maxx);
        maxy = TCODRandom::getInstance()->getInt(miny + minRoomSize - 1, maxy);
      }
      // resize the node to fit the room
      // printf("node %dx%d %dx%d => room %dx%d
      // %dx%d\n",node->x,node->y,node->w,node->h,minx,miny,maxx-minx+1,maxy-miny+1);
      node->x = minx;
      node->y = miny;
      node->w = maxx - minx + 1;
      node->h = maxy - miny + 1;
      // dig the room
      for (int x = minx; x <= maxx; x++) {
        for (int y = miny; y <= maxy; y++) {
          (*map)[x][y] = ' ';
        }
      }
    } else {
      // printf("lvl %d %dx%d %dx%d\n",node->level, node->x,node->y,node->w,node->h);
      // resize the node to fit its sons
      TCODBsp* left = node->getLeft();
      TCODBsp* right = node->getRight();
      node->x = MIN(left->x, right->x);
      node->y = MIN(left->y, right->y);
      node->w = MAX(left->x + left->w, right->x + right->w) - node->x;
      node->h = MAX(left->y + left->h, right->y + right->h) - node->y;
      // create a corridor between the two lower nodes
      if (node->horizontal) {
        // vertical corridor
        if (left->x + left->w - 1 < right->x || right->x + right->w - 1 < left->x) {
          // no overlapping zone. we need a Z shaped corridor
          int x1 = TCODRandom::getInstance()->getInt(left->x, left->x + left->w - 1);
          int x2 = TCODRandom::getInstance()->getInt(right->x, right->x + right->w - 1);
          int y = TCODRandom::getInstance()->getInt(left->y + left->h, right->y);
          vline_up(map, x1, y - 1);
          hline(map, x1, y, x2);
          vline_down(map, x2, y + 1);
        } else {
          // straight vertical corridor
          int minx = MAX(left->x, right->x);
          int maxx = MIN(left->x + left->w - 1, right->x + right->w - 1);
          int x = TCODRandom::getInstance()->getInt(minx, maxx);
          vline_down(map, x, right->y);
          vline_up(map, x, right->y - 1);
        }
      } else {
        // horizontal corridor
        if (left->y + left->h - 1 < right->y || right->y + right->h - 1 < left->y) {
          // no overlapping zone. we need a Z shaped corridor
          int y1 = TCODRandom::getInstance()->getInt(left->y, left->y + left->h - 1);
          int y2 = TCODRandom::getInstance()->getInt(right->y, right->y + right->h - 1);
          int x = TCODRandom::getInstance()->getInt(left->x + left->w, right->x);
          hline_left(map, x - 1, y1);
          vline(map, x, y1, y2);
          hline_right(map, x + 1, y2);
        } else {
          // straight horizontal corridor
          int miny = MAX(left->y, right->y);
          int maxy = MIN(left->y + left->h - 1, right->y + right->h - 1);
          int y = TCODRandom::getInstance()->getInt(miny, maxy);
          hline_left(map, right->x - 1, y);
          hline_right(map, right->x, y);
        }
      }
    }
    return true;
  }
};

class BSPSample : public Sample {
 public:
  void on_enter() override {}
  void on_event(TCOD_key_t* key, TCOD_mouse_t*) override {
    if (key->vk == TCODK_ENTER || key->vk == TCODK_KPENTER) {
      generate = true;
    } else if (key->c == ' ') {
      refresh = true;
    } else if (key->c == '=') {
      ++bspDepth;
      generate = true;
    } else if (key->c == '-' && bspDepth > 1) {
      --bspDepth;
      generate = true;
    } else if (key->c == '*') {
      ++minRoomSize;
      generate = true;
    } else if (key->c == '/' && minRoomSize > 2) {
      --minRoomSize;
      generate = true;
    } else if (key->c == '1' || key->vk == TCODK_1 || key->vk == TCODK_KP1) {
      randomRoom = !randomRoom;
      if (!randomRoom) roomWalls = true;
      refresh = true;
    } else if (key->c == '2' || key->vk == TCODK_2 || key->vk == TCODK_KP2) {
      roomWalls = !roomWalls;
      refresh = true;
    }
  }
  void on_draw(TCOD_Console& console) override {
    static constexpr auto darkWall = tcod::ColorRGB{0, 0, 100};
    static constexpr auto darkGround = tcod::ColorRGB{50, 50, 150};
    if (generate || refresh) {
      // dungeon generation
      if (!bsp) {
        // create the bsp
        bsp = new TCODBsp(0, 0, SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT);
      } else {
        // restore the nodes size
        bsp->resize(0, 0, SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT);
      }
      memset(map, '#', sizeof(char) * SAMPLE_SCREEN_WIDTH * SAMPLE_SCREEN_HEIGHT);
      if (generate) {
        // build a new random bsp tree
        bsp->removeSons();
        bsp->splitRecursive(
            NULL, bspDepth, minRoomSize + (roomWalls ? 1 : 0), minRoomSize + (roomWalls ? 1 : 0), 1.5f, 1.5f);
      }
      // create the dungeon from the bsp
      bsp->traverseInvertedLevelOrder(&listener, &map);
      generate = false;
      refresh = false;
    }
    sampleConsole.clear(0x20, WHITE, LIGHT_BLUE);
    tcod::print(
        sampleConsole,
        {1, 1},
        tcod::stringf(
            "ENTER : rebuild bsp\nSPACE : rebuild dungeon\n+-: bsp depth %d\n*/: room size %d\n1 : random room size %s",
            bspDepth,
            minRoomSize,
            randomRoom ? "ON" : "OFF"),
        std::nullopt,
        std::nullopt);
    if (randomRoom) {
      tcod::print(
          sampleConsole,
          {1, 6},
          tcod::stringf("2 : room walls %s", roomWalls ? "ON" : "OFF"),
          std::nullopt,
          std::nullopt);
    }
    // render the level
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        const bool wall = (map[x][y] == '#');
        sampleConsole.at({x, y}).bg = wall ? darkWall : darkGround;
      }
    }
  }

 private:
  TCODBsp* bsp = NULL;
  bool generate = true;
  bool refresh = false;
  map_t map;
  BspListener listener;
};

/* ***************************
 * name generator sample
 * ***************************/

class NameGeneratorSample : public Sample {
 public:
  NameGeneratorSample() {
    TCODList<char*> files{TCODSystem::getDirectoryContent("data/namegen", "*.cfg")};
    // parse all the files
    for (char* it : files) {
      TCODNamegen::parse((std::string("data/namegen/") + it).c_str());
    }
    // get the sets list
    TCODList<char*> sets_list{TCODNamegen::getSets()};
    sets = {sets_list.begin(), sets_list.end()};
  }
  void on_enter() override {}
  void on_event(TCOD_key_t* key, TCOD_mouse_t*) override {
    if (key->c == '=') {
      curSet++;
      if (curSet == sets.size()) curSet = 0;
      names.emplace_back("======");
    } else if (key->c == '-') {
      curSet--;
      if (curSet < 0) curSet = static_cast<int>(sets.size()) - 1;
      names.emplace_back("======");
    }
  }
  void on_draw(TCOD_Console& console) override {
    while (names.size() >= 15) {
      names.erase(names.begin());  // remove the first element.
    }

    sampleConsole.clear(0x20, WHITE, LIGHT_BLUE);
    tcod::print(
        sampleConsole,
        {1, 1},
        tcod::stringf("%s\n\n+ : next generator\n- : prev generator", sets.at(curSet).c_str()),
        WHITE,
        LIGHT_BLUE);
    for (int i = 0; i < static_cast<int>(names.size()); ++i) {
      const std::string& name = names.at(i);
      if (name.length() < SAMPLE_SCREEN_WIDTH) {
        tcod::print(sampleConsole, {SAMPLE_SCREEN_WIDTH - 2, 2 + i}, name, WHITE, std::nullopt, TCOD_RIGHT);
      }
    }

    delay += delta_time;
    if (delay >= 0.5f) {
      delay -= 0.5f;
      // add a new name to the list
      names.emplace_back(TCODNamegen::generate(sets.at(curSet).c_str(), true));
    }
  }

 private:
  int curSet = 0;
  float delay = 0.0f;
  std::vector<std::string> names;
  std::vector<std::string> sets;
};

/* ***************************
 * SDL callback sample
 * ***************************/
#ifndef NO_SDL_SAMPLE
static bool sdl_callback_enabled = false;

class SampleRenderer : public ITCODSDLRenderer {
 public:
  SampleRenderer() : effectNum(0), delay(3.0f) { noise = new TCODNoise(3); }
  ~SampleRenderer() { delete noise; }
  void render(void* sdlSurface) {
    SDL_Surface* screen = (SDL_Surface*)sdlSurface;
    // now we have almighty access to the screen's precious pixels !!
    // get the font character size
    int char_w, char_h;
    TCODSystem::getCharSize(&char_w, &char_h);
    // compute the sample console position in pixels
    int sample_x = SAMPLE_SCREEN_X * char_w;
    int sample_y = SAMPLE_SCREEN_Y * char_h;
    delay -= delta_time;
    if (delay < 0.0f) {
      delay = 3.0f;
      effectNum = (effectNum + 1) % 3;
      if (effectNum == 2)
        sdl_callback_enabled = false;  // no forced redraw for burn effect
      else
        sdl_callback_enabled = true;
    }
    switch (effectNum) {
      case 0:
        blur(screen, sample_x, sample_y, SAMPLE_SCREEN_WIDTH * char_w, SAMPLE_SCREEN_HEIGHT * char_h);
        break;
      case 1:
        explode(screen, sample_x, sample_y, SAMPLE_SCREEN_WIDTH * char_w, SAMPLE_SCREEN_HEIGHT * char_h);
        break;
      case 2:
        burn(screen, sample_x, sample_y, SAMPLE_SCREEN_WIDTH * char_w, SAMPLE_SCREEN_HEIGHT * char_h);
        break;
    }
  }

 protected:
  TCODNoise* noise;
  int effectNum;
  float delay;

  void burn(SDL_Surface* screen, int sample_x, int sample_y, int sample_w, int sample_h) {
    int r_idx = screen->format->Rshift / 8;
    int g_idx = screen->format->Gshift / 8;
    int b_idx = screen->format->Bshift / 8;
    for (int x = sample_x; x < sample_x + sample_w; x++) {
      uint8_t* p = (uint8_t*)screen->pixels + x * screen->format->BytesPerPixel + sample_y * screen->pitch;
      for (int y = sample_y; y < sample_y + sample_h; y++) {
        int ir = 0, ig = 0, ib = 0;
        uint8_t* p2 = p + screen->format->BytesPerPixel;  // get pixel at x+1,y
        ir += p2[r_idx];
        ig += p2[g_idx];
        ib += p2[b_idx];
        p2 -= 2 * screen->format->BytesPerPixel;  // get pixel at x-1,y
        ir += p2[r_idx];
        ig += p2[g_idx];
        ib += p2[b_idx];
        p2 += screen->format->BytesPerPixel + screen->pitch;  // get pixel at x,y+1
        ir += p2[r_idx];
        ig += p2[g_idx];
        ib += p2[b_idx];
        p2 -= 2 * screen->pitch;  // get pixel at x,y-1
        ir += p2[r_idx];
        ig += p2[g_idx];
        ib += p2[b_idx];
        p[r_idx] = static_cast<uint8_t>(ir / 4);
        p[g_idx] = static_cast<uint8_t>(ig / 4);
        p[b_idx] = static_cast<uint8_t>(ib / 4);
        p += screen->pitch;
      }
    }
  }

  void explode(SDL_Surface* screen, int sample_x, int sample_y, int sample_w, int sample_h) {
    int r_idx = screen->format->Rshift / 8;
    int g_idx = screen->format->Gshift / 8;
    int b_idx = screen->format->Bshift / 8;
    TCODRandom* rng = TCODRandom::getInstance();
    int dist = (int)(10 * (3.0f - delay));
    for (int x = sample_x; x < sample_x + sample_w; x++) {
      uint8_t* p = (uint8_t*)screen->pixels + x * screen->format->BytesPerPixel + sample_y * screen->pitch;
      for (int y = sample_y; y < sample_y + sample_h; y++) {
        int ir = 0, ig = 0, ib = 0;
        for (int i = 0; i < 3; i++) {
          int dx = rng->getInt(-dist, dist);
          int dy = rng->getInt(-dist, dist);
          uint8_t* p2;
          p2 = p + dx * screen->format->BytesPerPixel;
          p2 += dy * screen->pitch;
          ir += p2[r_idx];
          ig += p2[g_idx];
          ib += p2[b_idx];
        }
        p[r_idx] = static_cast<uint8_t>(ir / 3);
        p[g_idx] = static_cast<uint8_t>(ig / 3);
        p[b_idx] = static_cast<uint8_t>(ib / 3);
        p += screen->pitch;
      }
    }
  }

  void blur(SDL_Surface* screen, int sample_x, int sample_y, int sample_w, int sample_h) {
    // let's blur that sample console
    float f[3], n = 0.0f;
    int r_idx = screen->format->Rshift / 8;
    int g_idx = screen->format->Gshift / 8;
    int b_idx = screen->format->Bshift / 8;
    f[2] = SDL_GetTicks() / 1000.0f;
    for (int x = sample_x; x < sample_x + sample_w; x++) {
      uint8_t* p = (uint8_t*)screen->pixels + x * screen->format->BytesPerPixel + sample_y * screen->pitch;
      f[0] = (float)(x) / sample_w;
      for (int y = sample_y; y < sample_y + sample_h; y++) {
        int ir = 0, ig = 0, ib = 0;
        if ((y - sample_y) % 8 == 0) {
          f[1] = (float)(y) / sample_h;
          n = noise->getFbm(f, 3.0f);
        }
        int dec = (int)(3 * (n + 1.0f));
        int count = 0;
        switch (dec) {
          case 4:
            count += 4;
            // get pixel at x,y
            ir += p[r_idx];
            ig += p[g_idx];
            ib += p[b_idx];
            p -= 2 * screen->format->BytesPerPixel;  // get pixel at x+2,y
            ir += p[r_idx];
            ig += p[g_idx];
            ib += p[b_idx];
            p -= 2 * screen->pitch;  // get pixel at x+2,y+2
            ir += p[r_idx];
            ig += p[g_idx];
            ib += p[b_idx];
            p += 2 * screen->format->BytesPerPixel;  // get pixel at x,y+2
            ir += p[r_idx];
            ig += p[g_idx];
            ib += p[b_idx];
            p += 2 * screen->pitch;
          case 3:
            count += 4;
            // get pixel at x,y
            ir += p[r_idx];
            ig += p[g_idx];
            ib += p[b_idx];
            p += 2 * screen->format->BytesPerPixel;  // get pixel at x+2,y
            ir += p[r_idx];
            ig += p[g_idx];
            ib += p[b_idx];
            p += 2 * screen->pitch;  // get pixel at x+2,y+2
            ir += p[r_idx];
            ig += p[g_idx];
            ib += p[b_idx];
            p -= 2 * screen->format->BytesPerPixel;  // get pixel at x,y+2
            ir += p[r_idx];
            ig += p[g_idx];
            ib += p[b_idx];
            p -= 2 * screen->pitch;
          case 2:
            count += 4;
            // get pixel at x,y
            ir += p[r_idx];
            ig += p[g_idx];
            ib += p[b_idx];
            p -= screen->format->BytesPerPixel;  // get pixel at x-1,y
            ir += p[r_idx];
            ig += p[g_idx];
            ib += p[b_idx];
            p -= screen->pitch;  // get pixel at x-1,y-1
            ir += p[r_idx];
            ig += p[g_idx];
            ib += p[b_idx];
            p += screen->format->BytesPerPixel;  // get pixel at x,y-1
            ir += p[r_idx];
            ig += p[g_idx];
            ib += p[b_idx];
            p += screen->pitch;
          case 1:
            count += 4;
            // get pixel at x,y
            ir += p[r_idx];
            ig += p[g_idx];
            ib += p[b_idx];
            p += screen->format->BytesPerPixel;  // get pixel at x+1,y
            ir += p[r_idx];
            ig += p[g_idx];
            ib += p[b_idx];
            p += screen->pitch;  // get pixel at x+1,y+1
            ir += p[r_idx];
            ig += p[g_idx];
            ib += p[b_idx];
            p -= screen->format->BytesPerPixel;  // get pixel at x,y+1
            ir += p[r_idx];
            ig += p[g_idx];
            ib += p[b_idx];
            p -= screen->pitch;
            p[r_idx] = static_cast<uint8_t>(ir / count);
            p[g_idx] = static_cast<uint8_t>(ig / count);
            p[b_idx] = static_cast<uint8_t>(ib / count);
            break;
          default:
            break;
        }
        p += screen->pitch;
      }
    }
  }
};

class SDLSample : public Sample {
 public:
  void on_enter() override {
    // use noise sample as background. rendering is done in SampleRenderer
    sampleConsole.clear(0x20, WHITE, LIGHT_BLUE);
    tcod::print_rect(
        sampleConsole,
        {0, 3, SAMPLE_SCREEN_WIDTH, 0},
        "The SDL callback gives you access to the screen surface so that you can alter the pixels one by one using SDL "
        "API or any API on top of SDL. SDL is used here to blur the sample console.\n\nHit TAB to enable/disable the "
        "callback. While enabled, it will be active on other samples too.\n\nNote that the SDL callback only works "
        "with SDL renderer.",
        std::nullopt,
        std::nullopt,
        TCOD_CENTER);
  }
  void on_event(TCOD_key_t* key, TCOD_mouse_t*) override {
    if (key->vk == TCODK_TAB) {
      sdl_callback_enabled = !sdl_callback_enabled;
      if (sdl_callback_enabled) {
        TCODSystem::registerSDLRenderer(new SampleRenderer());
      } else {
        TCODSystem::registerSDLRenderer(NULL);
      }
    }
  }
  void on_draw(TCOD_Console& console) override {}
};
#endif
// ***************************
// the list of samples
// ***************************
static const std::vector<sample_t> samples = {
    {"  True colors        ", std::make_unique<TrueColors>()},
    {"  Offscreen console  ", std::make_unique<OffscreenConsole>()},
    {"  Line drawing       ", std::make_unique<LineDrawingSample>()},
    {"  Noise              ", std::make_unique<NoiseSample>()},
    {"  Field of view      ", std::make_unique<FOVSample>()},
    {"  Path finding       ", std::make_unique<PathfinderSample>()},
    {"  Bsp toolkit        ", std::make_unique<BSPSample>()},
    {"  Image toolkit      ", std::make_unique<ImageSample>()},
    {"  Mouse support      ", std::make_unique<MouseSample>()},
    {"  Name generator     ", std::make_unique<NameGeneratorSample>()},
#ifndef NO_SDL_SAMPLE
    {"  SDL callback       ", std::make_unique<SDLSample>()},
#endif
};

// ***************************
// the main function
// ***************************
int main(int argc, char* argv[]) {
  SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN);
  int curSample = 0;  // index of the current sample
  bool first = true;  // first time we render a sample
  TCOD_key_t key;
  TCOD_mouse_t mouse;
  const char* font = "data/fonts/dejavu10x10_gs_tc.png";
  int nbCharHoriz = 0, nbCharVertic = 0;
  int argn;
  int fullscreenWidth = 0;
  int fullscreenHeight = 0;
  TCOD_renderer_t renderer = TCOD_RENDERER_SDL2;
  bool fullscreen = false;
  int fontFlags = TCOD_FONT_TYPE_GREYSCALE | TCOD_FONT_LAYOUT_TCOD, fontNewFlags = 0;
  bool creditsEnd = false;
  int cur_renderer = 0;
  static const std::vector<std::string> renderer_name = {
      "F1 GLSL   ", "F2 OPENGL ", "F3 SDL    ", "F4 SDL2   ", "F5 OPENGL2"};

  // initialize the root console (open the game window)
  for (argn = 1; argn < argc; argn++) {
    if (strcmp(argv[argn], "-font") == 0 && argn + 1 < argc) {
      argn++;
      font = argv[argn];
      fontFlags = 0;
    } else if (strcmp(argv[argn], "-font-nb-char") == 0 && argn + 2 < argc) {
      argn++;
      nbCharHoriz = atoi(argv[argn]);
      argn++;
      nbCharVertic = atoi(argv[argn]);
      fontFlags = 0;
    } else if (strcmp(argv[argn], "-fullscreen-resolution") == 0 && argn + 2 < argc) {
      argn++;
      fullscreenWidth = atoi(argv[argn]);
      argn++;
      fullscreenHeight = atoi(argv[argn]);
    } else if (strcmp(argv[argn], "-renderer") == 0 && argn + 1 < argc) {
      argn++;
      renderer = (TCOD_renderer_t)atoi(argv[argn]);
    } else if (strcmp(argv[argn], "-fullscreen") == 0) {
      fullscreen = true;
    } else if (strcmp(argv[argn], "-font-in-row") == 0) {
      fontNewFlags |= TCOD_FONT_LAYOUT_ASCII_INROW;
      fontFlags = 0;
    } else if (strcmp(argv[argn], "-font-greyscale") == 0) {
      fontNewFlags |= TCOD_FONT_TYPE_GREYSCALE;
      fontFlags = 0;
    } else if (strcmp(argv[argn], "-font-tcod") == 0) {
      fontNewFlags |= TCOD_FONT_LAYOUT_TCOD;
      fontFlags = 0;
    } else if (strcmp(argv[argn], "-help") == 0 || strcmp(argv[argn], "-?") == 0) {
      printf("options :\n");
      printf("-font <filename> : use a custom font\n");
      printf("-font-nb-char <nb_char_horiz> <nb_char_vertic> : number of characters in the font\n");
      printf("-font-in-row : the font layout is in row instead of columns\n");
      printf("-font-tcod : the font uses TCOD layout instead of ASCII\n");
      printf("-font-greyscale : antialiased font using greyscale bitmap\n");
      printf("-fullscreen : start in fullscreen\n");
      printf("-fullscreen-resolution <screen_width> <screen_height> : force fullscreen resolution\n");
      printf("-renderer <num> : set renderer. 0 : GLSL 1 : OPENGL 2 : SDL\n");
      exit(0);
    } else {
      // ignore parameter
    }
  }

  if (fontFlags == 0) fontFlags = fontNewFlags;
  TCODConsole::setCustomFont(font, fontFlags, nbCharHoriz, nbCharVertic);
  if (fullscreenWidth > 0) {
    TCODSystem::forceFullscreenResolution(fullscreenWidth, fullscreenHeight);
  }

  TCODConsole::initRoot(80, 50, "libtcod C++ sample", fullscreen, renderer);
  atexit(TCOD_quit);
  auto timer = tcod::Timer();
  do {
    delta_time = timer.sync();
    if (!creditsEnd) {
      creditsEnd = TCODConsole::renderCredits(56, 43, false);
    }

    // print the list of samples
    for (int i = 0; i < static_cast<int>(samples.size()); ++i) {
      if (i == curSample) {
        // set colors for currently selected sample
        TCODConsole::root->setDefaultForeground(WHITE);
        TCODConsole::root->setDefaultBackground(LIGHT_BLUE);
      } else {
        // set colors for other samples
        TCODConsole::root->setDefaultForeground(GREY);
        TCODConsole::root->setDefaultBackground(BLACK);
      }
      // print the sample name
      TCODConsole::root->print(
          2, 46 - (static_cast<int>(samples.size()) - i), samples.at(i).name, TCOD_LEFT, TCOD_BKGND_SET);
    }
    // print the help message
    TCODConsole::root->setDefaultForeground(GREY);
    TCODConsole::root->printf(
        79,
        46,
        TCOD_BKGND_NONE,
        TCOD_RIGHT,
        "last frame : %3.0f ms (%3.0f fps)",
        delta_time * 1000.0f,
        timer.get_mean_fps());
    TCODConsole::root->printf(
        79, 47, TCOD_BKGND_NONE, TCOD_RIGHT, "elapsed : %8dms %4.2fs", SDL_GetTicks(), SDL_GetTicks() / 1000.0f);
    TCODConsole::root->printf(2, 47, "↑↓ : select a sample");
    TCODConsole::root->printf(
        2, 48, "ALT-ENTER : switch to %s", TCODConsole::isFullscreen() ? "windowed mode  " : "fullscreen mode");

    // render current sample
    if (first) samples.at(curSample).sample->on_enter();
    samples.at(curSample).sample->on_event(&key, &mouse);
    samples.at(curSample).sample->on_draw(sampleConsole);
    first = false;

    // blit the sample console on the root console
    tcod::blit(*TCODConsole::root, sampleConsole, {SAMPLE_SCREEN_X, SAMPLE_SCREEN_Y});
    // erase the renderer in debug mode (needed because the root console is not cleared each frame)
    TCODConsole::root->printf(1, 1, "        ");
    /* display renderer list and current renderer */
    cur_renderer = TCODSystem::getRenderer();
    TCODConsole::root->setDefaultForeground(GREY);
    TCODConsole::root->setDefaultBackground(BLACK);
    TCODConsole::root->printf(42, 46 - (TCOD_NB_RENDERERS + 1), TCOD_BKGND_SET, TCOD_LEFT, "Renderer :");
    for (int i = 0; i < TCOD_NB_RENDERERS; i++) {
      if (i == cur_renderer) {
        /* set colors for current renderer */
        TCODConsole::root->setDefaultForeground(WHITE);
        TCODConsole::root->setDefaultBackground(LIGHT_BLUE);
      } else {
        /* set colors for other renderer */
        TCODConsole::root->setDefaultForeground(GREY);
        TCODConsole::root->setDefaultBackground(BLACK);
      }
      TCODConsole::root->print(42, 46 - (TCOD_NB_RENDERERS - i), renderer_name[i], TCOD_LEFT, TCOD_BKGND_SET);
    }

    // update the game screen
    TCODConsole::flush();

    // did the user hit a key ?
    TCODSystem::checkForEvent((TCOD_event_t)(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE), &key, &mouse);
    if (key.vk == TCODK_DOWN) {
      // down arrow : next sample
      curSample = (curSample + 1) % samples.size();
      first = true;
    } else if (key.vk == TCODK_UP) {
      // up arrow : previous sample
      curSample--;
      if (curSample < 0) curSample = static_cast<int>(samples.size()) - 1;
      first = true;
    } else if (key.vk == TCODK_ENTER && (key.lalt | key.ralt)) {
      // ALT-ENTER : switch fullscreen
      TCODConsole::setFullscreen(!TCODConsole::isFullscreen());
#ifdef TCOD_LINUX
    } else if (key.c == 'p') {
#else
    } else if (key.vk == TCODK_PRINTSCREEN) {
#endif
      if (key.lalt) {
        // ALT-PrintScreen : save to .asc format
        TCODConsole::root->saveApf("samples.apf");
      } else {
        // save screenshot
        TCODSystem::saveScreenshot(NULL);
      }
    } else if (key.vk == TCODK_F1) {
      // switch renderers with F1,F2,F3
      TCODSystem::setRenderer(TCOD_RENDERER_GLSL);
    } else if (key.vk == TCODK_F2) {
      TCODSystem::setRenderer(TCOD_RENDERER_OPENGL);
    } else if (key.vk == TCODK_F3) {
      TCODSystem::setRenderer(TCOD_RENDERER_SDL);
    } else if (key.vk == TCODK_F4) {
      TCODSystem::setRenderer(TCOD_RENDERER_SDL2);
    } else if (key.vk == TCODK_F5) {
      TCODSystem::setRenderer(TCOD_RENDERER_OPENGL2);
    }
  } while (!TCODConsole::isWindowClosed());
  TCOD_quit();
  return 0;
}
