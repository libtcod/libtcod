/*
 * libtcod CPP samples
 * This code demonstrates various usages of libtcod modules
 * It's in the public domain.
 */

// uncomment this to disable SDL sample (might cause compilation issues on some systems)
//#define NO_SDL_SAMPLE
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif  // __EMSCRIPTEN__
#include <SDL.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <libtcod.hpp>
#include <libtcod/timer.hpp>
#include <memory>
#include <string>
#include <vector>

// Return true if string ends with suffix.
bool str_ends_with(std::string_view string, std::string_view suffix) {
  if (suffix.length() > string.length()) return false;
  return std::equal(suffix.rbegin(), suffix.rend(), string.rbegin());
}

// Abstract class for samples.
class Sample {
 public:
  virtual ~Sample() = default;
  virtual void on_enter() = 0;
  virtual void on_event(SDL_Event& event) = 0;
  virtual void on_draw(tcod::Console& console) = 0;
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
static constexpr char* SAMPLE_MAP[] = {
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

// Global variables.
static tcod::ContextPtr g_context;  // A global tcod context object.
static auto g_console = tcod::Console{80, 50};  // The main console to be presented.
static int g_current_sample = 0;  // index of the current sample
TCOD_ContextParams g_context_params{};  // The active context parameters.  Saved so that they can be modified.
static tcod::Tileset g_tileset;  // The currently active tileset.
// The offscreen console in which the samples are rendered
static tcod::Console g_sample_console(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT);

// ***************************
// true colors sample
// ***************************
class TrueColors : public Sample {
 public:
  void on_enter() override{};
  void on_event(SDL_Event&) override {}
  void on_draw(tcod::Console& console) override {
    // ==== slightly modify the corner colors ====
    for (int c = 0; c < 4; ++c) {
      // move each corner color
      const int component = TCODRandom::getInstance()->getInt(0, 2);
      switch (component) {
        case 0:
          colors_[c].r += 5 * dir_r_[c];
          if (colors_[c].r == 255)
            dir_r_[c] = -1;
          else if (colors_[c].r == 0)
            dir_r_[c] = 1;
          break;
        case 1:
          colors_[c].g += 5 * dir_g_[c];
          if (colors_[c].g == 255)
            dir_g_[c] = -1;
          else if (colors_[c].g == 0)
            dir_g_[c] = 1;
          break;
        case 2:
          colors_[c].b += 5 * dir_b_[c];
          if (colors_[c].b == 255)
            dir_b_[c] = -1;
          else if (colors_[c].b == 0)
            dir_b_[c] = 1;
          break;
      }
    }

    // ==== scan the whole screen, interpolating corner colors ====
    for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
      const float x_coef = static_cast<float>(x) / (SAMPLE_SCREEN_WIDTH - 1);
      // get the current column top and bottom colors
      const auto top = tcod::ColorRGB{TCODColor::lerp(colors_[TOPLEFT], colors_[TOPRIGHT], x_coef)};
      const auto bottom = tcod::ColorRGB{TCODColor::lerp(colors_[BOTTOMLEFT], colors_[BOTTOMRIGHT], x_coef)};
      for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
        const float y_coef = static_cast<float>(y) / (SAMPLE_SCREEN_HEIGHT - 1);
        // get the current cell color
        auto curColor = tcod::ColorRGB{TCODColor::lerp(top, bottom, y_coef)};
        console.at({x, y}).bg = curColor;
      }
    }

    // ==== print the text with a random color ====
    // get the background color at the text position
    auto textColor = tcod::ColorRGB{console.at({SAMPLE_SCREEN_WIDTH / 2, 5}).bg};
    // and invert it
    textColor.r = 255 - textColor.r;
    textColor.g = 255 - textColor.g;
    textColor.b = 255 - textColor.b;
    // put random text (for performance tests)
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        int c;
        auto col = tcod::ColorRGB{console.at({x, y}).bg};
        col = tcod::ColorRGB{TCODColor::lerp(col, BLACK, 0.5f)};
        // use colored character 255 on first and last lines
        if (y == 0 || y == SAMPLE_SCREEN_HEIGHT - 1) {
          c = 255;
        } else {
          c = TCODRandom::getInstance()->getInt('a', 'z');
        }
        console.at({x, y}).ch = c;
        console.at({x, y}).fg = col;
      }
    }
    // the background behind the text is slightly darkened using the BKGND_MULTIPLY flag
    tcod::print_rect(
        console,
        {1, 5, SAMPLE_SCREEN_WIDTH - 2, SAMPLE_SCREEN_HEIGHT - 1},
        "The Doryen library uses 24 bits colors, for both background and foreground.",
        textColor,
        GREY,
        TCOD_CENTER,
        TCOD_BKGND_MULTIPLY);
  }

 private:
  enum { TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT };
  std::array<tcod::ColorRGB, 4> colors_{
      {{50, 40, 150}, {240, 85, 5}, {50, 35, 240}, {10, 200, 130}}};  // random corner colors
  std::array<int8_t, 4> dir_r_{1, -1, 1, 1};
  std::array<int8_t, 4> dir_g_{1, -1, -1, 1};
  std::array<int8_t, 4> dir_b_{1, 1, 1, -1};
};

// ***************************
// offscreen console sample
// ***************************
class OffscreenConsole : public Sample {
 public:
  static constexpr auto FRAME_DECOR_SINGLE_PIPE =
      std::array<int, 9>{0x250C, 0x2500, 0x2510, 0x2502, 0x20, 0x2502, 0x2514, 0x2500, 0x2518};
  OffscreenConsole() {
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
  void on_enter() override {
    counter = SDL_GetTicks();
    screenshot = g_sample_console;  // get a "screenshot" of the current sample screen
  }
  void on_event(SDL_Event&) override {}
  void on_draw(tcod::Console& console) override {
    if (SDL_TICKS_PASSED(SDL_GetTicks(), counter + 1000)) {  // Once every second.
      counter = SDL_GetTicks();
      x_ += x_dir_;
      y_ += y_dir_;
      if (x_ == SAMPLE_SCREEN_WIDTH / 2 + 5) {
        x_dir_ = -1;
      } else if (x_ == -5) {
        x_dir_ = 1;
      }
      if (y_ == SAMPLE_SCREEN_HEIGHT / 2 + 5) {
        y_dir_ = -1;
      } else if (y_ == -5) {
        y_dir_ = 1;
      }
    }
    console = screenshot;  // restore the initial screen
    tcod::blit(console, secondary, {x_, y_}, {}, 1.0f, 0.75f);  // blit the overlapping screen
  }

 private:
  int counter;
  tcod::Console screenshot{SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT};
  tcod::Console secondary{SAMPLE_SCREEN_WIDTH / 2, SAMPLE_SCREEN_HEIGHT / 2};  // second screen
  int x_ = 0;  // secondary screen position
  int y_ = 0;
  int x_dir_ = 1;  // movement direction
  int y_dir_ = 1;
};

// ***************************
// line drawing sample
// ***************************
class LineDrawingSample : public Sample {
 public:
  LineDrawingSample() : background_{SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT} {
    // initialize the colored background
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        background_.at({x, y}).bg = tcod::ColorRGB{
            static_cast<uint8_t>(x * 255 / (SAMPLE_SCREEN_WIDTH - 1)),
            static_cast<uint8_t>((x + y) * 255 / (SAMPLE_SCREEN_WIDTH - 1 + SAMPLE_SCREEN_HEIGHT - 1)),
            static_cast<uint8_t>(y * 255 / (SAMPLE_SCREEN_HEIGHT - 1)),
        };
      }
    }
  }
  void on_enter() override {}
  void on_event(SDL_Event& event) override {
    switch (event.type) {
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_RETURN:
          case SDLK_RETURN2:
          case SDLK_KP_ENTER:
            // switch to the next blending mode
            ++blend_flag_;
            if ((blend_flag_ & 0xff) > TCOD_BKGND_ALPH) blend_flag_ = TCOD_BKGND_NONE;
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
  void on_draw(tcod::Console& console) override {
    if ((blend_flag_ & 0xff) == TCOD_BKGND_ALPH) {
      // for the alpha mode, update alpha every frame
      const auto alpha = float{(1.0f + cosf(SDL_GetTicks() / 1000.0f * 2.0f)) / 2.0f};
      blend_flag_ = TCOD_BKGND_ALPHA(alpha);
    } else if ((blend_flag_ & 0xff) == TCOD_BKGND_ADDA) {
      // for the add alpha mode, update alpha every frame
      const auto alpha = float{(1.0f + cosf(SDL_GetTicks() / 1000.0f * 2.0f)) / 2.0f};
      blend_flag_ = TCOD_BKGND_ADDALPHA(alpha);
    }
    // blit the background
    console = background_;
    // render the gradient
    const auto rect_y = static_cast<int>((SAMPLE_SCREEN_HEIGHT - 2) * ((1.0f + cosf(SDL_GetTicks() / 1000.0f)) / 2.0f));
    for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
      const auto col = tcod::ColorRGB{
          static_cast<uint8_t>(x * 255 / SAMPLE_SCREEN_WIDTH),
          static_cast<uint8_t>(x * 255 / SAMPLE_SCREEN_WIDTH),
          static_cast<uint8_t>(x * 255 / SAMPLE_SCREEN_WIDTH),
      };
      tcod::draw_rect(console, {x, rect_y, 1, 3}, 0, std::nullopt, col, static_cast<TCOD_bkgnd_flag_t>(blend_flag_));
    }
    // calculate the segment ends
    const float angle = SDL_GetTicks() / 1000.0f * 2.0f;
    const int xo = static_cast<int>(SAMPLE_SCREEN_WIDTH / 2 * (1 + cosf(angle)));
    const int yo = static_cast<int>(SAMPLE_SCREEN_HEIGHT / 2 + sinf(angle) * SAMPLE_SCREEN_WIDTH / 2);
    const int xd = static_cast<int>(SAMPLE_SCREEN_WIDTH / 2 * (1 - cosf(angle)));
    const int yd = static_cast<int>(SAMPLE_SCREEN_HEIGHT / 2 - sinf(angle) * SAMPLE_SCREEN_WIDTH / 2);

    // render the line
    for (const auto xy : tcod::BresenhamLine({xo, yo}, {xd, yd})) {
      if (console.in_bounds(xy)) {
        TCOD_console_set_char_background(
            console.get(), xy.at(0), xy.at(1), LIGHT_BLUE, static_cast<TCOD_bkgnd_flag_t>(blend_flag_));
      }
    }
    // print the current flag
    tcod::print(
        console,
        {2, 2},
        tcod::stringf("%s (ENTER to change)", flag_names_.at(blend_flag_ & 0xff)),
        WHITE,
        std::nullopt);
  }

 private:
  tcod::Console background_{SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT};  // The background of this sample.
  int blend_flag_ = TCOD_BKGND_SET;  // current blending mode
  // Readable names for the blend modes.
  static constexpr auto flag_names_ = std::array<const char*, 13>{
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
      "TCOD_BKGND_ALPHA",
  };
};

// ***************************
// noise sample
// ***************************
class NoiseSample : public Sample {
 public:
  void on_enter() override {}
  void on_event(SDL_Event& event) override {
    switch (event.type) {
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_e:  // increase hurst
            hurst_ += 0.1f;
            noise_ = TCODNoise(2, hurst_, lacunarity_);
            break;
          case SDLK_d:  // decrease hurst
            hurst_ -= 0.1f;
            noise_ = TCODNoise(2, hurst_, lacunarity_);
            break;
          case SDLK_r:  // increase lacunarity
            lacunarity_ += 0.5f;
            noise_ = TCODNoise(2, hurst_, lacunarity_);
            break;
          case SDLK_f:  // decrease lacunarity
            lacunarity_ -= 0.5f;
            noise_ = TCODNoise(2, hurst_, lacunarity_);
            break;
          case SDLK_t:  // increase octaves
            octaves_ += 0.5f;
            break;
          case SDLK_g:  // decrease octaves
            octaves_ -= 0.5f;
            break;
          case SDLK_y:  // increase zoom
            zoom += 0.2f;
            break;
          case SDLK_h:  // decrease zoom
            zoom -= 0.2f;
            break;
          default:
            const auto set_func = [&](int n) {
              if (0 <= n && n < 9) func_ = n;
            };
            set_func(event.key.keysym.sym - SDLK_1);
            set_func(event.key.keysym.sym - SDLK_KP_1);
            break;
        }
        break;
      default:
        break;
    }
  }
  void on_draw(tcod::Console& console) override {
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

    // texture animation
    const float dx = SDL_GetTicks() * 0.0005f;
    const float dy = dx;
    // render the 2d noise function
    for (int y = 0; y < 2 * SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < 2 * SAMPLE_SCREEN_WIDTH; ++x) {
        const float f[2] = {zoom * x / (2 * SAMPLE_SCREEN_WIDTH) + dx, zoom * y / (2 * SAMPLE_SCREEN_HEIGHT) + dy};
        float value = 0.0f;
        switch (func_) {
          case PERLIN:
            value = noise_.get(f, TCOD_NOISE_PERLIN);
            break;
          case SIMPLEX:
            value = noise_.get(f, TCOD_NOISE_SIMPLEX);
            break;
          case WAVELET:
            value = noise_.get(f, TCOD_NOISE_WAVELET);
            break;
          case FBM_PERLIN:
            value = noise_.getFbm(f, octaves_, TCOD_NOISE_PERLIN);
            break;
          case TURBULENCE_PERLIN:
            value = noise_.getTurbulence(f, octaves_, TCOD_NOISE_PERLIN);
            break;
          case FBM_SIMPLEX:
            value = noise_.getFbm(f, octaves_, TCOD_NOISE_SIMPLEX);
            break;
          case TURBULENCE_SIMPLEX:
            value = noise_.getTurbulence(f, octaves_, TCOD_NOISE_SIMPLEX);
            break;
          case FBM_WAVELET:
            value = noise_.getFbm(f, octaves_, TCOD_NOISE_WAVELET);
            break;
          case TURBULENCE_WAVELET:
            value = noise_.getTurbulence(f, octaves_, TCOD_NOISE_WAVELET);
            break;
        }
        const auto c = static_cast<uint8_t>((value + 1.0f) / 2.0f * 255);
        // use a bluish color
        img_.putPixel(x, y, tcod::ColorRGB{static_cast<uint8_t>(c / 2), static_cast<uint8_t>(c / 2), c});
      }
    }
    // blit the noise image on the console with subcell resolution
    tcod::draw_quartergraphics(console, img_);
    // draw a transparent rectangle
    tcod::draw_rect(console, {2, 2, 23, (func_ <= WAVELET ? 10 : 13)}, 0, std::nullopt, GREY, TCOD_BKGND_MULTIPLY);
    for (int y = 2; y < 2 + (func_ <= WAVELET ? 10 : 13); ++y) {
      for (int x = 2; x < 2 + 23; ++x) {
        console.at({x, y}).fg.r /= 2;
        console.at({x, y}).fg.g /= 2;
        console.at({x, y}).fg.b /= 2;
      }
    }
    // draw the text
    for (int curfunc = PERLIN; curfunc <= TURBULENCE_WAVELET; ++curfunc) {
      const auto fg = curfunc == func_ ? WHITE : GREY;
      const auto bg = curfunc == func_ ? std::optional{LIGHT_BLUE} : std::nullopt;
      tcod::print(g_sample_console, {2, 2 + curfunc}, funcName.at(curfunc), fg, bg);
    }
    // draw parameters
    tcod::print(g_sample_console, {2, 11}, tcod::stringf("Y/H : zoom (%2.1f)", zoom), WHITE, std::nullopt);
    if (func_ > WAVELET) {
      tcod::print(g_sample_console, {2, 12}, tcod::stringf("E/D : hurst (%2.1f)", hurst_), WHITE, std::nullopt);
      tcod::print(
          g_sample_console, {2, 13}, tcod::stringf("R/F : lacunarity (%2.1f)", lacunarity_), WHITE, std::nullopt);
      tcod::print(g_sample_console, {2, 14}, tcod::stringf("T/G : octaves (%2.1f)", octaves_), WHITE, std::nullopt);
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
  int func_ = PERLIN;
  float octaves_ = 4.0f;
  float hurst_ = TCOD_NOISE_DEFAULT_HURST;
  float lacunarity_ = TCOD_NOISE_DEFAULT_LACUNARITY;
  float zoom = 3.0f;
  TCODNoise noise_{2, hurst_, lacunarity_};
  TCODImage img_{SAMPLE_SCREEN_WIDTH * 2, SAMPLE_SCREEN_HEIGHT * 2};
};

// ***************************
// fov sample
// ***************************

static constexpr auto CHAR_WINDOW = 0x2550;  // "═" glyph.
static constexpr std::array<const char*, 14> algo_names{
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
  FOVSample() {
    // initialize the map for the fov toolkit
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        if (SAMPLE_MAP[y][x] == ' ')
          map_.setProperties(x, y, true, true);  // ground
        else if (SAMPLE_MAP[y][x] == '=')
          map_.setProperties(x, y, true, false);  // window
      }
    }
  }
  void on_enter() override {}
  /// Attempt to move the player.
  void try_move(int dx, int dy) noexcept {
    const int dest_x = px_ + dx;
    const int dest_y = py_ + dy;
    if (SAMPLE_MAP[dest_y][dest_x] == ' ') {
      px_ = dest_x;
      py_ = dest_y;
    }
  }
  void on_event(SDL_Event& event) override {
    switch (event.type) {
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_i:
            try_move(0, -1);  // player move north
            break;
          case SDLK_k:
            try_move(0, 1);  // player move south
            break;
          case SDLK_j:
            try_move(-1, 0);  // player move west
            break;
          case SDLK_l:
            try_move(1, 0);  // player move east
            break;
          case SDLK_t:
            torch_ = !torch_;  // enable/disable the torch fx
            break;
          case SDLK_w:
            light_walls_ = !light_walls_;
            break;
          case SDLK_EQUALS:
          case SDLK_KP_PLUS:
            algonum_ = (algonum_ + 1) % NB_FOV_ALGORITHMS;
            break;
          case SDLK_KP_MINUS:
          case SDLK_MINUS:
            algonum_ = (algonum_ + NB_FOV_ALGORITHMS - 1) % NB_FOV_ALGORITHMS;
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
  void on_draw(tcod::Console& console) override {
    static constexpr auto TORCH_RADIUS = 10.0f;
    static constexpr auto SQUARED_TORCH_RADIUS = (TORCH_RADIUS * TORCH_RADIUS);
    static constexpr auto darkWall = tcod::ColorRGB{0, 0, 100};
    static constexpr auto lightWall = tcod::ColorRGB{130, 110, 50};
    static constexpr auto darkGround = tcod::ColorRGB{50, 50, 150};
    static constexpr auto lightGround = tcod::ColorRGB{200, 180, 50};
    // draw the help text & player @
    console.clear({0x20, BLACK, BLACK});
    tcod::print(
        console,
        {1, 0},
        tcod::stringf(
            "IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s",
            torch_ ? "on " : "off",
            light_walls_ ? "on " : "off",
            algo_names.at(algonum_)),
        WHITE,
        std::nullopt);
    console.at({px_, py_}).ch = '@';

    // draw windows
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        if (SAMPLE_MAP[y][x] == '=') {
          console.at({x, y}).ch = CHAR_WINDOW;
        }
      }
    }
    // calculate the field of view from the player position
    map_.computeFov(px_, py_, torch_ ? (int)(TORCH_RADIUS) : 0, light_walls_, (TCOD_fov_algorithm_t)algonum_);
    // torch position & intensity variation
    float dx = 0.0f, dy = 0.0f, di = 0.0f;
    if (torch_) {
      // slightly change the perlin noise parameter
      torch_x_ += 0.2f;
      // randomize the light position between -1.5 and 1.5
      float tdx = torch_x_ + 20.0f;
      dx = noise_.get(&tdx) * 1.5f;
      tdx += 30.0f;
      dy = noise_.get(&tdx) * 1.5f;
      // randomize the light intensity between -0.2 and 0.2
      di = 0.2f * noise_.get(&torch_x_);
    }
    // draw the dungeon
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        const bool visible = map_.isInFov(x, y);
        const bool wall = SAMPLE_MAP[y][x] == '#';
        if (!visible) {
          console.at({x, y}).bg = wall ? darkWall : darkGround;
        } else {
          tcod::ColorRGB light;
          if (!torch_) {
            light = wall ? lightWall : lightGround;
          } else {
            // torch flickering fx
            tcod::ColorRGB base = (wall ? darkWall : darkGround);
            light = (wall ? lightWall : lightGround);
            // cell distance to torch (squared)
            const float r = (float)((x - px_ + dx) * (x - px_ + dx) + (y - py_ + dy) * (y - py_ + dy));
            if (r < SQUARED_TORCH_RADIUS) {
              // l = 1.0 at player position, 0.0 at a radius of 10 cells
              const float l = std::clamp((SQUARED_TORCH_RADIUS - r) / SQUARED_TORCH_RADIUS + di, 0.0f, 1.0f);
              // interpolate the color
              base = tcod::ColorRGB{TCODColor::lerp(base, light, l)};
            }
            light = base;
          }
          console.at({x, y}).bg = light;
        }
      }
    }
  }

 private:
  int px_ = 20, py_ = 10;  // player position
  bool torch_ = false;  // torch fx on ?
  TCODMap map_{SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT};
  TCODNoise noise_{1};  // 1d noise used for the torch flickering
  bool light_walls_ = true;
  int algonum_ = 0;
  float torch_x_ = 0.0f;  // torch light position in the perlin noise
};

// ***************************
// image sample
// ***************************
class ImageSample : public Sample {
 public:
  ImageSample() { img_.setKeyColor(BLACK); }
  void on_enter() override {}
  void on_event(SDL_Event&) override {}
  void on_draw(tcod::Console& console) override {
    console.clear();
    const float x = SAMPLE_SCREEN_WIDTH / 2 + cosf(SDL_GetTicks() / 1000.0f) * 10.0f;
    const float y = (float)(SAMPLE_SCREEN_HEIGHT / 2);
    const float scale_x = 0.2f + 1.8f * (1.0f + cosf(SDL_GetTicks() / 1000.0f / 2.0f)) / 2.0f;
    const float scale_y = scale_x;
    const float angle = SDL_GetTicks() / 1000.0f;
    const long elapsed = SDL_GetTicks() / 2000;
    if (elapsed & 1) {
      // split the color channels of circle.png
      // the red channel
      tcod::draw_rect(console, {0, 3, 15, 15}, 0, std::nullopt, {{255, 0, 0}});
      circle_.blitRect(console, 0, 3, -1, -1, TCOD_BKGND_MULTIPLY);
      // the green channel
      tcod::draw_rect(console, {15, 3, 15, 15}, 0, std::nullopt, {{0, 255, 0}});
      circle_.blitRect(console, 15, 3, -1, -1, TCOD_BKGND_MULTIPLY);
      // the blue channel
      tcod::draw_rect(console, {30, 3, 15, 15}, 0, std::nullopt, {{0, 0, 255}});
      circle_.blitRect(console, 30, 3, -1, -1, TCOD_BKGND_MULTIPLY);
    } else {
      // render circle.png with normal blitting
      circle_.blitRect(console, 0, 3, -1, -1, TCOD_BKGND_SET);
      circle_.blitRect(console, 15, 3, -1, -1, TCOD_BKGND_SET);
      circle_.blitRect(console, 30, 3, -1, -1, TCOD_BKGND_SET);
    }
    img_.blit(console, x, y, TCOD_BKGND_SET, scale_x, scale_y, angle);
  }

 private:
  TCODImage img_{"data/img/skull.png"};
  TCODImage circle_{"data/img/circle.png"};
};

// ***************************
// mouse sample
// ***************************/
class MouseSample : public Sample {
 public:
  void on_enter() override {
    SDL_WarpMouseInWindow(NULL, 320, 200);
    SDL_ShowCursor(1);
    last_motion_ = SDL_Event{};
  }
  void on_event(SDL_Event& event) override {
    switch (event.type) {
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_1:
          case SDLK_KP_1:
            SDL_ShowCursor(0);
            break;
          case SDLK_2:
          case SDLK_KP_2:
            SDL_ShowCursor(1);
            break;
          default:
            break;
        }
        break;
      case SDL_MOUSEMOTION:
        last_motion_ = event;  // Track mouse motion.
        break;
      default:
        break;
    }
  }
  void on_draw(tcod::Console& console) override {
    auto last_tile = last_motion_;  // SDL event to be converted into tile coordinates.
    g_context->convert_event_coordinates(last_tile);
    auto sdl_window = g_context->get_sdl_window();

    const auto window_flags = sdl_window ? SDL_GetWindowFlags(sdl_window) : 0;
    const auto& mouse = last_motion_.motion;
    const auto& mouse_tile = last_tile.motion;
    const auto state = SDL_GetMouseState(nullptr, nullptr);

    console.clear({0x20, LIGHT_YELLOW, GREY});
    tcod::print(
        console,
        {1, 1},
        tcod::stringf(
            "%s\n"
            "Mouse pixel position : %4dx%4d %s\n"
            "Mouse pixel motion   : %4dx%4d\n"
            "Mouse tile           : %4dx%4d\n"
            "Mouse tile motion    : %4dx%4d\n"
            "Left button          : %s\n"
            "Right button         : %s\n"
            "Middle button        : %s\n",
            window_flags & SDL_WINDOW_INPUT_FOCUS ? "" : "APPLICATION INACTIVE",
            mouse.x,
            mouse.y,
            window_flags & SDL_WINDOW_MOUSE_FOCUS ? "" : "OUT OF FOCUS",
            mouse.xrel,
            mouse.yrel,
            mouse_tile.x,
            mouse_tile.y,
            mouse_tile.xrel,
            mouse_tile.yrel,
            (state & SDL_BUTTON_LMASK) ? " ON" : "OFF",
            (state & SDL_BUTTON_RMASK) ? " ON" : "OFF",
            (state & SDL_BUTTON_MMASK) ? " ON" : "OFF"),
        std::nullopt,
        std::nullopt);
    tcod::print(console, {1, 10}, "1 : Hide cursor\n2 : Show cursor", std::nullopt, std::nullopt);
  }

 private:
  bool left_button = false, right_button = false, middle_button = false;
  SDL_Event last_motion_{};
};

// ***************************
// path sample
// ***************************
class PathfinderSample : public Sample {
 public:
  PathfinderSample() {
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        if (SAMPLE_MAP[y][x] == ' ')
          map_.setProperties(x, y, true, true);  // ground
        else if (SAMPLE_MAP[y][x] == '=')
          map_.setProperties(x, y, true, false);  // window
      }
    }
  }
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
  void on_event(SDL_Event& event) override {
    g_context->convert_event_coordinates(event);
    switch (event.type) {
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_i:
            dest_y_ -= 1;  // destination move north
            break;
          case SDLK_k:
            dest_y_ += 1;  // destination move south
            break;
          case SDLK_j:
            dest_x_ -= 1;  // destination move west
            break;
          case SDLK_l:
            dest_x_ += 1;  // destination move east
            break;
          case SDLK_TAB:  // Toggle pathfinder.
          case SDLK_KP_TAB:
            using_astar_ = !using_astar_;
            break;
          default:
            break;
        }
        break;
      case SDL_MOUSEMOTION: {
        const auto dest_x = event.motion.x - SAMPLE_SCREEN_X;
        const auto dest_y = event.motion.y - SAMPLE_SCREEN_Y;
        if (0 <= dest_x && dest_x < SAMPLE_SCREEN_WIDTH && 0 <= dest_y && dest_y < SAMPLE_SCREEN_HEIGHT) {
          dest_x_ = dest_x;
          dest_y_ = dest_y;
        }
      } break;
      default:
        break;
    }
    dest_x_ = std::clamp(dest_x_, 0, SAMPLE_SCREEN_WIDTH - 1);
    dest_y_ = std::clamp(dest_y_, 0, SAMPLE_SCREEN_HEIGHT - 1);
  }
  void on_draw(tcod::Console& console) override {
    static constexpr auto darkWall = tcod::ColorRGB{0, 0, 100};
    static constexpr auto darkGround = tcod::ColorRGB{50, 50, 150};
    static constexpr auto lightGround = tcod::ColorRGB{200, 180, 50};
    // draw the help text & player @
    console.clear();
    console.at({dest_x_, dest_y_}).ch = '+';
    console.at({player_x_, player_y_}).ch = '@';
    tcod::print(console, {1, 1}, "IJKL / mouse :\nmove destination\nTAB : A*/dijkstra", WHITE, std::nullopt);
    tcod::print(console, {1, 4}, using_astar_ ? "Using : A*" : "Using : Dijkstra", WHITE, std::nullopt);
    // draw windows
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        if (SAMPLE_MAP[y][x] == '=') {
          console.at({x, y}).ch = CHAR_WINDOW;
        }
      }
    }
    // Recalculate the path.
    if (using_astar_) {
      path_.compute(player_x_, player_y_, dest_x_, dest_y_);
    } else {
      dijkstra_dist_ = 0.0f;
      // compute the distance grid
      dijkstra_.compute(player_x_, player_y_);
      // get the maximum distance (needed for ground shading only)
      for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
          const float d = dijkstra_.getDistance(x, y);
          if (d > dijkstra_dist_) dijkstra_dist_ = d;
        }
      }
      // compute the path
      dijkstra_.setPath(dest_x_, dest_y_);
    }
    // draw the dungeon
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        const bool wall = SAMPLE_MAP[y][x] == '#';
        g_sample_console.at({x, y}).bg = wall ? darkWall : darkGround;
      }
    }
    // draw the path
    if (using_astar_) {
      for (int i = 0; i < path_.size(); ++i) {
        int x, y;
        path_.get(i, &x, &y);
        g_sample_console.at({x, y}).bg = lightGround;
      }
    } else {
      for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
          const bool wall = SAMPLE_MAP[y][x] == '#';
          if (!wall) {
            const float d = dijkstra_.getDistance(x, y);
            g_sample_console.at({x, y}).bg =
                tcod::ColorRGB{TCODColor::lerp(lightGround, darkGround, 0.9f * d / dijkstra_dist_)};
          }
        }
      }
      for (int i = 0; i < dijkstra_.size(); ++i) {
        int x, y;
        dijkstra_.get(i, &x, &y);
        g_sample_console.at({x, y}).bg = lightGround;
      }
    }
    // move the creature
    busy_ -= delta_time;
    if (busy_ <= 0.0f) {
      busy_ = 0.2f;
      if (using_astar_) {
        if (!path_.isEmpty()) {
          path_.walk(&player_x_, &player_y_, true);
        }
      } else {
        if (!dijkstra_.isEmpty()) {
          dijkstra_.walk(&player_x_, &player_y_);
        }
      }
    }
  }

 private:
  int player_x_ = 20, player_y_ = 10;  // player position
  int dest_x_ = 24, dest_y_ = 1;  // destination
  TCODMap map_{SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT};
  TCODPath path_{&map_};
  TCODDijkstra dijkstra_{&map_};
  bool using_astar_ = true;
  float dijkstra_dist_ = 0;
  float busy_ = 0;
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
  void on_event(SDL_Event& event) override {
    switch (event.type) {
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_RETURN:
          case SDLK_RETURN2:
          case SDLK_KP_ENTER:
            generate_ = true;
            break;
          case SDLK_SPACE:
          case SDLK_KP_SPACE:
            refresh_ = true;
            break;
          case SDLK_EQUALS:
          case SDLK_KP_PLUS:
            ++bspDepth;
            generate_ = true;
            break;
          case SDLK_MINUS:
          case SDLK_KP_MINUS:
            bspDepth = std::max(1, bspDepth - 1);
            generate_ = true;
            break;
          case SDLK_8:
          case SDLK_KP_MULTIPLY:
            ++minRoomSize;
            generate_ = true;
            break;
          case SDLK_SLASH:
          case SDLK_KP_DIVIDE:
            minRoomSize = std::max(2, minRoomSize - 1);
            generate_ = true;
            break;
          case SDLK_1:
          case SDLK_KP_1:
            randomRoom = !randomRoom;
            if (!randomRoom) roomWalls = true;
            refresh_ = true;
            break;
          case SDLK_2:
          case SDLK_KP_2:
            roomWalls = !roomWalls;
            refresh_ = true;
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
  void on_draw(tcod::Console& console) override {
    static constexpr auto darkWall = tcod::ColorRGB{0, 0, 100};
    static constexpr auto darkGround = tcod::ColorRGB{50, 50, 150};
    if (generate_ || refresh_) {  // dungeon generation
      bsp_.resize(0, 0, SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT);  // restore the nodes size
      memset(map_, '#', sizeof(char) * SAMPLE_SCREEN_WIDTH * SAMPLE_SCREEN_HEIGHT);
      if (generate_) {
        // build a new random bsp tree
        bsp_.removeSons();
        bsp_.splitRecursive(
            NULL, bspDepth, minRoomSize + (roomWalls ? 1 : 0), minRoomSize + (roomWalls ? 1 : 0), 1.5f, 1.5f);
      }
      // create the dungeon from the bsp
      bsp_.traverseInvertedLevelOrder(&listener_, &map_);
      generate_ = false;
      refresh_ = false;
    }
    console.clear({0x20, WHITE, LIGHT_BLUE});
    tcod::print(
        console,
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
          console, {1, 6}, tcod::stringf("2 : room walls %s", roomWalls ? "ON" : "OFF"), std::nullopt, std::nullopt);
    }
    // render the level
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        const bool wall = (map_[x][y] == '#');
        g_sample_console.at({x, y}).bg = wall ? darkWall : darkGround;
      }
    }
  }

 private:
  TCODBsp bsp_{};
  bool generate_ = true;
  bool refresh_ = false;
  map_t map_;
  BspListener listener_;
};

/* ***************************
 * name generator sample
 * ***************************/

class NameGeneratorSample : public Sample {
 public:
  NameGeneratorSample() {
    // Parse name data from *.cfg files.
    for (const auto& path : std::filesystem::directory_iterator("data/namegen")) {
      if (!path.is_regular_file()) continue;
      if (!(path.path().extension() == ".cfg")) continue;
      TCODNamegen::parse(path.path().string().c_str());
    }
    // get the sets list
    TCODList<char*> sets_list{TCODNamegen::getSets()};
    sets = {sets_list.begin(), sets_list.end()};
  }
  void on_enter() override {}
  void on_event(SDL_Event& event) override {
    switch (event.type) {
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_EQUALS:
          case SDLK_KP_PLUS:
            curSet = (curSet + 1) % static_cast<int>(sets.size());
            names.emplace_back("======");
            break;
          case SDLK_MINUS:
          case SDLK_KP_MINUS:
            curSet = (curSet + static_cast<int>(sets.size()) - 1) % static_cast<int>(sets.size());
            names.emplace_back("======");
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
  void on_draw(tcod::Console& console) override {
    while (names.size() >= 15) {
      names.erase(names.begin());  // remove the first element.
    }

    console.clear({0x20, WHITE, LIGHT_BLUE});
    tcod::print(
        console,
        {1, 1},
        tcod::stringf("%s\n\n+ : next generator\n- : prev generator", sets.at(curSet).c_str()),
        WHITE,
        LIGHT_BLUE);
    for (int i = 0; i < static_cast<int>(names.size()); ++i) {
      const std::string& name = names.at(i);
      if (name.length() < SAMPLE_SCREEN_WIDTH) {
        tcod::print(console, {SAMPLE_SCREEN_WIDTH - 2, 2 + i}, name, WHITE, std::nullopt, TCOD_RIGHT);
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
  void render(void* sdlSurface) {
    SDL_Surface* screen = (SDL_Surface*)sdlSurface;
    // now we have almighty access to the screen's precious pixels !!
    // get the font character size
    const auto char_w = g_tileset.get_tile_width();
    const auto char_h = g_tileset.get_tile_height();
    // compute the sample console position in pixels
    int sample_x = SAMPLE_SCREEN_X * char_w;
    int sample_y = SAMPLE_SCREEN_Y * char_h;
    delay_ -= delta_time;
    if (delay_ < 0.0f) {
      delay_ = 3.0f;
      effectNum_ = (effectNum_ + 1) % 3;
      if (effectNum_ == 2)
        sdl_callback_enabled = false;  // no forced redraw for burn effect
      else
        sdl_callback_enabled = true;
    }
    switch (effectNum_) {
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
  TCODNoise noise_{3};
  int effectNum_ = 0;
  float delay_ = 3.0f;

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
    int dist = (int)(10 * (3.0f - delay_));
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
          n = noise_.getFbm(f, 3.0f);
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
    g_sample_console.clear({0x20, WHITE, LIGHT_BLUE});
    tcod::print_rect(
        g_sample_console,
        {0, 3, SAMPLE_SCREEN_WIDTH, 0},
        "The SDL callback gives you access to the screen surface so that you can alter the pixels one by one using SDL "
        "API or any API on top of SDL. SDL is used here to blur the sample console.\n\nHit TAB to enable/disable the "
        "callback. While enabled, it will be active on other samples too.\n\nNote that the SDL callback only works "
        "with SDL renderer.",
        std::nullopt,
        std::nullopt,
        TCOD_CENTER);
  }
  void on_event(SDL_Event& event) override {
    switch (event.type) {
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_TAB:
          case SDLK_KP_TAB:
            sdl_callback_enabled = !sdl_callback_enabled;
            if (sdl_callback_enabled) {
              TCODSystem::registerSDLRenderer(new SampleRenderer());
            } else {
              TCODSystem::registerSDLRenderer(NULL);
            }
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
  void on_draw(tcod::Console&) override {}
};
#endif

struct RendererOption {
  const char* name;
  TCOD_renderer_t renderer;
};

static constexpr std::array<RendererOption, 5> RENDERERS{
    RendererOption{"OPENGL ", TCOD_RENDERER_OPENGL},
    RendererOption{"SDL    ", TCOD_RENDERER_SDL},
    RendererOption{"SDL2   ", TCOD_RENDERER_SDL2},
    RendererOption{"OPENGL2", TCOD_RENDERER_OPENGL2},
    RendererOption{"XTERM  ", TCOD_RENDERER_XTERM},
};

// ***************************
// the list of samples
// ***************************
static const std::vector<sample_t> g_samples = {
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

void main_loop() {
  static auto timer = tcod::Timer();
  static bool creditsEnd = false;
  delta_time = timer.sync();
  if (!creditsEnd) {
    creditsEnd = TCOD_console_credits_render_ex(g_console.get(), 56, 43, false, delta_time);
  }

  // print the list of samples
  for (int i = 0; i < static_cast<int>(g_samples.size()); ++i) {
    const auto fg = (i == g_current_sample) ? WHITE : GREY;
    const auto bg = (i == g_current_sample) ? LIGHT_BLUE : BLACK;
    // print the sample name
    tcod::print(g_console, {2, 46 - (static_cast<int>(g_samples.size()) - i)}, g_samples.at(i).name, fg, bg);
  }
  // print the help message
  tcod::print(
      g_console,
      {79, 46},
      tcod::stringf("last frame : %3.0f ms (%3.0f fps)", delta_time * 1000.0f, timer.get_mean_fps()),
      GREY,
      std::nullopt,
      TCOD_RIGHT);
  tcod::print(
      g_console,
      {79, 47},
      tcod::stringf("elapsed : %8dms %4.2fs", SDL_GetTicks(), SDL_GetTicks() / 1000.0f),
      GREY,
      std::nullopt,
      TCOD_RIGHT);
  tcod::print(g_console, {2, 47}, "↑↓ : select a sample", GREY, std::nullopt);
  {
    auto sdl_window = g_context->get_sdl_window();
    if (sdl_window) {
      const auto is_fullscreen =
          SDL_GetWindowFlags(sdl_window) & (SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_FULLSCREEN);
      tcod::print(
          g_console,
          {2, 48},
          tcod::stringf("ALT-ENTER : switch to %s", is_fullscreen ? "windowed mode  " : "fullscreen mode"),
          GREY,
          std::nullopt);
    }
  }

  // render current sample
  g_samples.at(g_current_sample).sample->on_draw(g_sample_console);

  // blit the sample console on the root console
  tcod::blit(g_console, g_sample_console, {SAMPLE_SCREEN_X, SAMPLE_SCREEN_Y});
  /* display renderer list and current renderer */
  tcod::print(g_console, {42, 46 - (static_cast<int>(RENDERERS.size()) + 1)}, "Renderer :", GREY, BLACK);

  for (int i = 0; i < static_cast<int>(RENDERERS.size()); ++i) {
    const bool is_current_renderer = RENDERERS.at(i).renderer == g_context->get_renderer_type();
    const auto fg = is_current_renderer ? WHITE : GREY;
    const auto bg = is_current_renderer ? LIGHT_BLUE : BLACK;
    tcod::print(
        g_console,
        {42, 46 - (static_cast<int>(RENDERERS.size()) - i)},
        tcod::stringf("F%d %s", i + 1, RENDERERS.at(i).name),
        fg,
        bg);
  }

  // update the game screen
  g_context->present(g_console);
  g_console.clear();

  // did the user hit a key ?
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        std::exit(EXIT_SUCCESS);
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_DOWN:
            g_current_sample = (g_current_sample + 1) % static_cast<int>(g_samples.size());
            g_samples.at(g_current_sample).sample->on_enter();
            break;
          case SDLK_UP:
            g_current_sample =
                (g_current_sample + static_cast<int>(g_samples.size()) - 1) % static_cast<int>(g_samples.size());
            g_samples.at(g_current_sample).sample->on_enter();
            break;
          case SDLK_RETURN:
          case SDLK_RETURN2:
          case SDLK_KP_ENTER:
            if (event.key.keysym.mod & KMOD_ALT) {
              auto sdl_window = g_context->get_sdl_window();
              if (sdl_window) {
                const auto flags = SDL_GetWindowFlags(sdl_window);
                const auto is_fullscreen = flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP);
                // Change fullscreen mode.
                SDL_SetWindowFullscreen(sdl_window, is_fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
              }
            }
            break;
          case SDLK_p:
          case SDLK_PRINTSCREEN:
            g_context->save_screenshot(nullptr);  // Save screenshot.
            break;
          default:
            if (SDLK_F1 <= event.key.keysym.sym && event.key.keysym.sym < SDLK_F1 + RENDERERS.size()) {
              auto sdl_window = g_context->get_sdl_window();
              if (sdl_window) {
                // Save fullscreen and maximized state to params, so that they are kept on the new renderer.
                const auto current_flags = SDL_GetWindowFlags(sdl_window);
                static constexpr auto TRACKED_FLAGS = SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_MAXIMIZED;
                g_context_params.sdl_window_flags =
                    (g_context_params.sdl_window_flags & ~TRACKED_FLAGS) | (current_flags & TRACKED_FLAGS);
              }
              g_context_params.renderer_type = RENDERERS.at(event.key.keysym.sym - SDLK_F1).renderer;
              g_context = nullptr;
              g_context = tcod::new_context(g_context_params);
            }
            break;
        }
        break;
      case SDL_DROPFILE: {  // Change to a new tileset when one is dropped on the window.
        tcod::Tileset new_tileset;
        if (str_ends_with(event.drop.file, ".bdf")) {
          new_tileset = tcod::Tileset(tcod::load_bdf(event.drop.file));
        } else if (str_ends_with(event.drop.file, "_tc.png")) {
          new_tileset = tcod::load_tilesheet(event.drop.file, {32, 8}, tcod::CHARMAP_TCOD);
        } else {
          new_tileset = tcod::load_tilesheet(event.drop.file, {16, 16}, tcod::CHARMAP_CP437);
        }
        if (new_tileset.get()) {
          g_tileset = std::move(new_tileset);
          g_context_params.tileset = g_tileset.get();
          TCOD_context_change_tileset(g_context.get(), g_tileset.get());
        }
        SDL_free(event.drop.file);
      } break;
      default:
        break;
    }
    g_samples.at(g_current_sample).sample->on_event(event);
  }
}

// ***************************
// the main function
// ***************************
int main(int argc, char* argv[]) {
  try {
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN);
    static constexpr char* FONT = "data/fonts/dejavu10x10_gs_tc.png";
    g_tileset = tcod::load_tilesheet(FONT, {32, 8}, tcod::CHARMAP_TCOD);

    // Context parameters, this is reused when the renderer is changed.
    g_context_params.tcod_version = TCOD_COMPILEDVERSION;
    g_context_params.renderer_type = TCOD_RENDERER_SDL2;
    g_context_params.tileset = g_tileset.get();
    g_context_params.vsync = 0;
    g_context_params.sdl_window_flags = SDL_WINDOW_RESIZABLE;
    g_context_params.window_title = "libtcod C++ samples";
    g_context_params.argc = argc;
    g_context_params.argv = argv;
    g_context_params.console = g_console.get();

    g_context = tcod::new_context(g_context_params);

    atexit(TCOD_quit);
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 0);
#else
    while (true) main_loop();
#endif
  } catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
