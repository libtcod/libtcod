/*
 * libtcod C samples
 * This code demonstrates various usages of libtcod modules
 * It's in the public domain.
 */

// uncomment this to disable SDL sample (might cause compilation issues on some systems)
// #define NO_SDL_SAMPLE

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <libtcod.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* a sample has a name and a rendering function */
typedef struct sample_t {
  const char* name;
  void (*render)(const SDL_Event* event);
} sample_t;

/* sample screen size */
#define SAMPLE_SCREEN_WIDTH 46
#define SAMPLE_SCREEN_HEIGHT 20
/* sample screen position */
#define SAMPLE_SCREEN_X 20
#define SAMPLE_SCREEN_Y 10

// A custom SDL event for when a sample is switched to.
#define ON_ENTER_USEREVENT (SDL_EVENT_USER + 0)
// A custom SDL event to tell a sample to draw.
#define ON_DRAW_USEREVENT (SDL_EVENT_USER + 1)

static const TCOD_ColorRGB BLACK = {0, 0, 0};
static const TCOD_ColorRGB WHITE = {255, 255, 255};
static const TCOD_ColorRGB GREY = {127, 127, 127};
static const TCOD_ColorRGB RED = {255, 0, 0};
static const TCOD_ColorRGB GREEN = {0, 255, 0};
static const TCOD_ColorRGB BLUE = {0, 0, 255};
static const TCOD_ColorRGB LIGHT_BLUE = {63, 63, 255};
static const TCOD_ColorRGB LIGHT_YELLOW = {255, 255, 63};

static float delta_time = 0.0f;  // The time in seconds of the current frame.
#define DELTA_SAMPLES_LENGTH 64
static float delta_samples[DELTA_SAMPLES_LENGTH] = {0};
static int last_delta_sample = 0;

static TCOD_Tileset* tileset = NULL;

// Report an error message and abort the program.
void fatal(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  exit(EXIT_FAILURE);
}

// Return true if string ends with suffix.
bool str_ends_with(const char* string, const char* suffix) {
  if (!string || !suffix) return false;
  size_t str_len = strlen(string);
  size_t suffix_len = strlen(suffix);
  if (suffix_len > str_len) return false;
  return strcmp(string + str_len - suffix_len, suffix) == 0;
}

/* ***************************
 * samples rendering functions
 * ***************************/

TCOD_Context* g_context;
static TCOD_Console* sample_console;  // the offscreen console in which the samples are rendered.

/* ***************************
 * true colors sample
 * ***************************/
// Return noise sample as a 0 to 255 value.
static uint8_t noise_sample_u8(TCOD_Noise* noise, const float* f) {
  return (uint8_t)((TCOD_noise_get(noise, f) + 1.0f) * 0.5f * 255.5f);
}
void render_colors(const SDL_Event* event) {
  enum { TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT };
  static TCOD_Noise* noise = NULL;  // Noise for random colors.
  if (!noise) {
    noise = TCOD_noise_new(2, 0, 0, NULL);
    TCOD_noise_set_type(noise, TCOD_NOISE_SIMPLEX);
  }
  if (event->type == ON_ENTER_USEREVENT) {
    TCOD_console_clear(sample_console);
  }
  const float t = SDL_GetTicks() * 0.001f;
  // Generate color corners from noise samples.
  const TCOD_color_t colors[4] = {
      {noise_sample_u8(noise, (float[]){t, 0}),
       noise_sample_u8(noise, (float[]){t, 1}),
       noise_sample_u8(noise, (float[]){t, 2})},
      {noise_sample_u8(noise, (float[]){t, 10}),
       noise_sample_u8(noise, (float[]){t, 11}),
       noise_sample_u8(noise, (float[]){t, 12})},
      {noise_sample_u8(noise, (float[]){t, 20}),
       noise_sample_u8(noise, (float[]){t, 21}),
       noise_sample_u8(noise, (float[]){t, 22})},
      {noise_sample_u8(noise, (float[]){t, 30}),
       noise_sample_u8(noise, (float[]){t, 31}),
       noise_sample_u8(noise, (float[]){t, 32})}};

  /* ==== scan the whole screen, interpolating corner colors ==== */
  for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
    const float y_coef = (float)(y) / (SAMPLE_SCREEN_HEIGHT - 1);
    /* get the current column top and bottom colors */
    TCOD_color_t left = TCOD_color_lerp(colors[TOPLEFT], colors[BOTTOMLEFT], y_coef);
    TCOD_color_t right = TCOD_color_lerp(colors[TOPRIGHT], colors[BOTTOMRIGHT], y_coef);
    for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
      const float x_coef = (float)(x) / (SAMPLE_SCREEN_WIDTH - 1);
      /* get the current cell color */
      TCOD_ColorRGB current_color = TCOD_color_lerp(left, right, x_coef);
      TCOD_console_put_rgb(sample_console, x, y, 0, NULL, &current_color, TCOD_BKGND_SET);
    }
  }

  /* ==== print the text ==== */
  /* put random text (for performance tests) */
  for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
    for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
      TCOD_ColorRGB color = TCOD_console_get_char_background(sample_console, x, y);
      color = TCOD_color_lerp(color, BLACK, 0.5f);
      const int c = TCOD_random_get_int(NULL, 'a', 'z');
      TCOD_console_put_rgb(sample_console, x, y, c, &color, NULL, TCOD_BKGND_SET);
    }
  }
  /* get the background color at the text position */
  TCOD_color_t textColor = TCOD_console_get_char_background(sample_console, SAMPLE_SCREEN_WIDTH / 2, 5);
  /* and invert it */
  textColor.r = 255 - textColor.r;
  textColor.g = 255 - textColor.g;
  textColor.b = 255 - textColor.b;
  /* the background behind the text is slightly darkened using the BKGND_MULTIPLY flag */
  TCOD_printf_rgb(
      sample_console,
      (TCOD_PrintParamsRGB){
          .x = 1,
          .y = 5,
          .width = SAMPLE_SCREEN_WIDTH - 2,
          .fg = &textColor,
          .bg = &GREY,
          .flag = TCOD_BKGND_MULTIPLY,
          .alignment = TCOD_CENTER,
      },
      "The Doryen library uses 24 bits colors, for both background and foreground.");
}

/* ***************************
 * offscreen console sample
 * ***************************/
void render_offscreen(const SDL_Event* event) {
  static TCOD_console_t secondary; /* second screen */
  static TCOD_console_t screenshot; /* second screen */
  static bool init = false; /* draw the secondary screen only the first time */
  static uint64_t counter;
  static int x = 0, y = 0; /* secondary screen position */
  static int x_dir = 1, y_dir = 1; /* movement direction */
  if (!init) {
    init = true;
    secondary = TCOD_console_new(SAMPLE_SCREEN_WIDTH / 2, SAMPLE_SCREEN_HEIGHT / 2);
    screenshot = TCOD_console_new(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT);
    TCOD_console_draw_frame_rgb(secondary, 0, 0, secondary->w, secondary->h, NULL, NULL, NULL, 0, 0);
    TCOD_printf_rgb(
        secondary,
        (TCOD_PrintParamsRGB){
            .x = 0, .y = 0, .width = secondary->w, .fg = &BLACK, .bg = &WHITE, .alignment = TCOD_CENTER},
        " Offscreen console ");
    TCOD_printf_rgb(
        secondary,
        (TCOD_PrintParamsRGB){.x = 1, .y = 2, .width = secondary->w - 2, .alignment = TCOD_CENTER},
        "You can render to an offscreen console and blit in on another one, simulating alpha transparency.");
  }
  if (event->type == ON_ENTER_USEREVENT) {
    counter = SDL_GetTicks();
    /* get a "screenshot" of the current sample screen */
    TCOD_console_blit(sample_console, 0, 0, SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT, screenshot, 0, 0, 1.0f, 1.0f);
  }
  if (SDL_GetTicks() >= counter + 1000) {  // Once every second.
    counter = SDL_GetTicks();
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
  /* restore the initial screen */
  TCOD_console_blit(screenshot, 0, 0, SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT, sample_console, 0, 0, 1.0f, 1.0f);
  /* blit the overlapping screen */
  TCOD_console_blit(
      secondary, 0, 0, SAMPLE_SCREEN_WIDTH / 2, SAMPLE_SCREEN_HEIGHT / 2, sample_console, x, y, 1.0f, 0.75f);
}

/* ***************************
 * line drawing sample
 * ***************************/
static int bk_flag = TCOD_BKGND_SET; /* current blending mode */
bool line_listener(int x, int y) {
  if (x >= 0 && y >= 0 && x < SAMPLE_SCREEN_WIDTH && y < SAMPLE_SCREEN_HEIGHT) {
    TCOD_console_put_rgb(sample_console, x, y, 0, NULL, &LIGHT_BLUE, (TCOD_bkgnd_flag_t)bk_flag);
  }
  return true;
}

void render_lines(const SDL_Event* event) {
  static TCOD_console_t bk; /* colored background */
  static bool init = false;
  static const char* flag_names[] = {
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
  switch (event->type) {
    case SDL_EVENT_KEY_DOWN:
      switch (event->key.scancode) {
        case SDL_SCANCODE_RETURN:
        case SDL_SCANCODE_RETURN2:
        case SDL_SCANCODE_KP_ENTER:
          if (event->key.mod & SDL_KMOD_ALT) break;
          /* switch to the next blending mode */
          ++bk_flag;
          if ((bk_flag & 0xff) > TCOD_BKGND_ALPH) bk_flag = TCOD_BKGND_NONE;
          break;
        default:
          break;
      }
      break;
  }
  float alpha; /* alpha value when blending mode = TCOD_BKGND_ALPHA */
  const float current_time = SDL_GetTicks() / 1000.0f;
  if ((bk_flag & 0xff) == TCOD_BKGND_ALPH) {
    /* for the alpha mode, update alpha every frame */
    alpha = (1.0f + cosf(current_time * 2)) / 2.0f;
    bk_flag = TCOD_BKGND_ALPHA(alpha);
  } else if ((bk_flag & 0xff) == TCOD_BKGND_ADDA) {
    /* for the add alpha mode, update alpha every frame */
    alpha = (1.0f + cosf(current_time * 2)) / 2.0f;
    bk_flag = TCOD_BKGND_ADDALPHA(alpha);
  }
  if (!init) {
    bk = TCOD_console_new(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT);
    /* initialize the colored background */
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        TCOD_ColorRGB color = {
            .r = (uint8_t)(x * 255 / (SAMPLE_SCREEN_WIDTH - 1)),
            .g = (uint8_t)((x + y) * 255 / (SAMPLE_SCREEN_WIDTH - 1 + SAMPLE_SCREEN_HEIGHT - 1)),
            .b = (uint8_t)(y * 255 / (SAMPLE_SCREEN_HEIGHT - 1)),
        };
        TCOD_console_put_rgb(bk, x, y, 0, NULL, &color, TCOD_BKGND_SET);
      }
    }
    init = true;
  }
  /* blit the background */
  TCOD_console_blit(bk, 0, 0, SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT, sample_console, 0, 0, 1.0f, 1.0f);
  /* render the gradient */
  /* gradient vertical position */
  const int rect_y = (int)((SAMPLE_SCREEN_HEIGHT - 2) * ((1.0f + cosf(current_time)) / 2.0f));
  for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
    const TCOD_ColorRGB color = {
        (uint8_t)(x * 255 / SAMPLE_SCREEN_WIDTH),
        (uint8_t)(x * 255 / SAMPLE_SCREEN_WIDTH),
        (uint8_t)(x * 255 / SAMPLE_SCREEN_WIDTH),
    };
    TCOD_console_draw_rect_rgb(sample_console, x, rect_y, 1, 3, 0, NULL, &color, (TCOD_bkgnd_flag_t)bk_flag);
  }
  /* calculate the segment ends */
  const float angle = current_time * 2.0f; /* segment angle data */
  /* segment starting and ending positions */
  const int xo = (int)(SAMPLE_SCREEN_WIDTH / 2 * (1 + cosf(angle)));
  const int yo = (int)(SAMPLE_SCREEN_HEIGHT / 2 + sinf(angle) * SAMPLE_SCREEN_WIDTH / 2);
  const int xd = (int)(SAMPLE_SCREEN_WIDTH / 2 * (1 - cosf(angle)));
  const int yd = (int)(SAMPLE_SCREEN_HEIGHT / 2 - sinf(angle) * SAMPLE_SCREEN_WIDTH / 2);
  /* render the line */
  TCOD_line(xo, yo, xd, yd, line_listener);
  /* print the current flag */
  TCOD_printf_rgb(
      sample_console,
      (TCOD_PrintParamsRGB){.x = 2, .y = 2, .fg = &(TCOD_ColorRGB){255, 255, 255}},
      "%s (ENTER to change)",
      flag_names[bk_flag & 0xff]);
}

/* ***************************
 * noise sample
 * ***************************/
void render_noise(const SDL_Event* event) {
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
  }; /* which function we render */
  static const char* funcName[] = {
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
  static int func = PERLIN;
  static TCOD_noise_t noise = NULL;
  static float octaves = 4.0f;
  static float hurst = TCOD_NOISE_DEFAULT_HURST;
  static float lacunarity = TCOD_NOISE_DEFAULT_LACUNARITY;
  static TCOD_image_t img = NULL;
  static float zoom = 3.0f;
  if (!noise) {
    noise = TCOD_noise_new(2, hurst, lacunarity, NULL);
    img = TCOD_image_new(SAMPLE_SCREEN_WIDTH * 2, SAMPLE_SCREEN_HEIGHT * 2);
  }
  TCOD_console_clear(sample_console);
  /* texture animation */
  const float dx = SDL_GetTicks() * 0.0005f;
  const float dy = dx;

  /* render the 2d noise function */
  for (int y = 0; y < 2 * SAMPLE_SCREEN_HEIGHT; ++y) {
    for (int x = 0; x < 2 * SAMPLE_SCREEN_WIDTH; ++x) {
      const float sample_xy[2] = {
          zoom * x / (2 * SAMPLE_SCREEN_WIDTH) + dx,
          zoom * y / (2 * SAMPLE_SCREEN_HEIGHT) + dy,
      };
      float value = 0.0f;
      switch (func) {
        case PERLIN:
          value = TCOD_noise_get_ex(noise, sample_xy, TCOD_NOISE_PERLIN);
          break;
        case SIMPLEX:
          value = TCOD_noise_get_ex(noise, sample_xy, TCOD_NOISE_SIMPLEX);
          break;
        case WAVELET:
          value = TCOD_noise_get_ex(noise, sample_xy, TCOD_NOISE_WAVELET);
          break;
        case FBM_PERLIN:
          value = TCOD_noise_get_fbm_ex(noise, sample_xy, octaves, TCOD_NOISE_PERLIN);
          break;
        case TURBULENCE_PERLIN:
          value = TCOD_noise_get_turbulence_ex(noise, sample_xy, octaves, TCOD_NOISE_PERLIN);
          break;
        case FBM_SIMPLEX:
          value = TCOD_noise_get_fbm_ex(noise, sample_xy, octaves, TCOD_NOISE_SIMPLEX);
          break;
        case TURBULENCE_SIMPLEX:
          value = TCOD_noise_get_turbulence_ex(noise, sample_xy, octaves, TCOD_NOISE_SIMPLEX);
          break;
        case FBM_WAVELET:
          value = TCOD_noise_get_fbm_ex(noise, sample_xy, octaves, TCOD_NOISE_WAVELET);
          break;
        case TURBULENCE_WAVELET:
          value = TCOD_noise_get_turbulence_ex(noise, sample_xy, octaves, TCOD_NOISE_WAVELET);
          break;
      }
      const uint8_t c = (uint8_t)((value + 1.0f) / 2.0f * 255);
      /* use a bluish color */
      TCOD_ColorRGB col = {c / 2, c / 2, c};
      TCOD_image_put_pixel(img, x, y, col);
    }
  }
  /* blit the noise image with subcell resolution */
  TCOD_image_blit_2x(img, sample_console, 0, 0, 0, 0, -1, -1);

  /* draw a transparent rectangle */
  TCOD_console_draw_rect_rgb(
      sample_console, 2, 2, 23, (func <= WAVELET ? 10 : 13), 0, NULL, &GREY, TCOD_BKGND_MULTIPLY);
  for (int y = 2; y < 2 + (func <= WAVELET ? 10 : 13); ++y) {
    for (int x = 2; x < 2 + 23; ++x) {
      TCOD_ConsoleTile* tile_ref = &sample_console->tiles[y * sample_console->w + x];
      TCOD_ColorRGB* fg_ref = (TCOD_ColorRGB*)(&tile_ref->fg);
      *fg_ref = TCOD_color_multiply(*fg_ref, GREY);
    }
  }

  /* draw the text */
  for (int curfunc = PERLIN; curfunc <= TURBULENCE_WAVELET; ++curfunc) {
    const bool is_highlighted = curfunc == func;
    TCOD_printf_rgb(
        sample_console,
        (TCOD_PrintParamsRGB){
            .x = 2,
            .y = 2 + curfunc,
            .fg = is_highlighted ? &WHITE : &GREY,
            .bg = is_highlighted ? &LIGHT_BLUE : NULL,
        },
        "%s",
        funcName[curfunc]);
  }
  /* draw parameters */
  TCOD_printf_rgb(sample_console, (TCOD_PrintParamsRGB){.x = 2, .y = 11, .fg = &WHITE}, "Y/H : zoom (%2.1f)", zoom);
  if (func > WAVELET) {
    TCOD_printf_rgb(
        sample_console,
        (TCOD_PrintParamsRGB){.x = 2, .y = 12, .fg = &WHITE},
        "E/D : hurst (%2.1f)\nR/F : lacunarity (%2.1f)\nT/G : octaves (%2.1f)",
        hurst,
        lacunarity,
        octaves);
  }
  /* handle keypress */
  switch (event->type) {
    case SDL_EVENT_KEY_DOWN:
      switch (event->key.scancode) {
        case SDL_SCANCODE_E:
          /* increase hurst */
          hurst += 0.1f;
          TCOD_noise_delete(noise);
          noise = TCOD_noise_new(2, hurst, lacunarity, NULL);
          break;
        case SDL_SCANCODE_D:
          /* decrease hurst */
          hurst -= 0.1f;
          TCOD_noise_delete(noise);
          noise = TCOD_noise_new(2, hurst, lacunarity, NULL);
          break;
        case SDL_SCANCODE_R:
          /* increase lacunarity */
          lacunarity += 0.5f;
          TCOD_noise_delete(noise);
          noise = TCOD_noise_new(2, hurst, lacunarity, NULL);
          break;
        case SDL_SCANCODE_F:
          /* decrease lacunarity */
          lacunarity -= 0.5f;
          TCOD_noise_delete(noise);
          noise = TCOD_noise_new(2, hurst, lacunarity, NULL);
          break;
        case SDL_SCANCODE_T:
          /* increase octaves */
          octaves += 0.5f;
          break;
        case SDL_SCANCODE_G:
          /* decrease octaves */
          octaves -= 0.5f;
          break;
        case SDL_SCANCODE_Y:
          /* increase zoom */
          zoom += 0.2f;
          break;
        case SDL_SCANCODE_H:
          /* decrease zoom */
          zoom -= 0.2f;
          break;
        default: {
          /* change the noise function */
          const int scancode = event->key.scancode;
          if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_9) {
            func = scancode - SDL_SCANCODE_1;
          }
          if (scancode >= SDL_SCANCODE_KP_1 && scancode <= SDL_SCANCODE_KP_9) {
            func = scancode - SDL_SCANCODE_KP_1;
          }
        } break;
      }
      break;
  }
}

/* ***************************
 * fov sample
 * ***************************/
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
static const int CHAR_WINDOW = 0x2550;  // "═" glyph.
#define TORCH_RADIUS 10.0f
#define SQUARED_TORCH_RADIUS (TORCH_RADIUS * TORCH_RADIUS)
void render_fov(const SDL_Event* event) {
  static int px = 20, py = 10; /* player position */
  static bool torch = false;
  static bool light_walls = true;
  static TCOD_map_t map = NULL;
  static const TCOD_color_t dark_wall = {0, 0, 100};
  static const TCOD_color_t dark_ground = {50, 50, 150};
  static const TCOD_color_t light_ground = {200, 180, 50};
  static const TCOD_color_t light_wall = {130, 110, 50};
  static TCOD_noise_t noise;
  static int algonum = 0;
  static const char* algo_names[] = {
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
  static float torch_x = 0.0f; /* torch light position in the perlin noise */
  /* torch position & intensity variation */
  float dx = 0.0f, dy = 0.0f, di = 0.0f;
  if (!map) {
    map = TCOD_map_new(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT);
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        if (SAMPLE_MAP[y][x] == ' ')
          TCOD_map_set_properties(map, x, y, true, true); /* ground */
        else if (SAMPLE_MAP[y][x] == '=')
          TCOD_map_set_properties(map, x, y, true, false); /* window */
      }
    }
    noise = TCOD_noise_new(1, 1.0f, 1.0f, NULL); /* 1d noise for the torch flickering */
  }
  if (event->type == ON_DRAW_USEREVENT) {
    /* we draw the foreground only the first time.
       during the player movement, only the @ is redrawn.
       the rest impacts only the background color */
    /* draw the help text & player @ */
    TCOD_console_clear(sample_console);
    TCOD_printf_rgb(
        sample_console,
        (TCOD_PrintParamsRGB){.x = 1, .y = 0, .fg = &WHITE},
        "IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s",
        torch ? "on " : "off",
        light_walls ? "on " : "off",
        algo_names[algonum]);
    TCOD_console_put_rgb(sample_console, px, py, '@', &BLACK, NULL, 0);
    /* draw windows */
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        if (SAMPLE_MAP[y][x] == '=') {
          TCOD_console_put_rgb(sample_console, x, y, CHAR_WINDOW, &BLACK, NULL, 0);  // ═
        }
      }
    }
    TCOD_map_compute_fov(map, px, py, torch ? (int)(TORCH_RADIUS) : 0, light_walls, (TCOD_fov_algorithm_t)algonum);
    if (torch) {
      /* slightly change the perlin noise parameter */
      torch_x += 0.2f;
      /* randomize the light position between -1.5 and 1.5 */
      float tdx = torch_x + 20.0f;
      dx = TCOD_noise_get(noise, &tdx) * 1.5f;
      tdx += 30.0f;
      dy = TCOD_noise_get(noise, &tdx) * 1.5f;
      di = 0.2f * TCOD_noise_get(noise, &torch_x);
    }
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        const bool visible = TCOD_map_is_in_fov(map, x, y);
        const bool wall = SAMPLE_MAP[y][x] == '#';
        if (!visible) {
          TCOD_console_put_rgb(sample_console, x, y, 0, NULL, wall ? &dark_wall : &dark_ground, TCOD_BKGND_SET);
        } else {
          if (!torch) {
            TCOD_console_put_rgb(sample_console, x, y, 0, NULL, wall ? &light_wall : &light_ground, TCOD_BKGND_SET);
          } else {
            TCOD_ColorRGB base = (wall ? dark_wall : dark_ground);
            const TCOD_ColorRGB light = (wall ? light_wall : light_ground);
            /* cell distance to torch (squared) */
            const float radius_squared = (x - px + dx) * (x - px + dx) + (y - py + dy) * (y - py + dy);
            if (radius_squared < SQUARED_TORCH_RADIUS) {
              const float l = (SQUARED_TORCH_RADIUS - radius_squared) / SQUARED_TORCH_RADIUS + di;
              base = TCOD_color_lerp(base, light, TCOD_CLAMP(0.0f, 1.0f, l));
            }
            TCOD_console_put_rgb(sample_console, x, y, 0, NULL, &base, TCOD_BKGND_SET);
          }
        }
      }
    }
  }
  switch (event->type) {
    case SDL_EVENT_KEY_DOWN:
      switch (event->key.scancode) {
        case SDL_SCANCODE_I:
          if (SAMPLE_MAP[py - 1][px] == ' ') {
            --py;
          }
          break;
        case SDL_SCANCODE_K:
          if (SAMPLE_MAP[py + 1][px] == ' ') {
            ++py;
          }
          break;
        case SDL_SCANCODE_J:
          if (SAMPLE_MAP[py][px - 1] == ' ') {
            --px;
          }
          break;
        case SDL_SCANCODE_L:
          if (SAMPLE_MAP[py][px + 1] == ' ') {
            ++px;
          }
          break;
        case SDL_SCANCODE_T:
          torch = !torch;
          break;
        case SDL_SCANCODE_W:
          light_walls = !light_walls;
          break;
        case SDL_SCANCODE_EQUALS:
        case SDL_SCANCODE_KP_PLUS:
        case SDL_SCANCODE_MINUS:
        case SDL_SCANCODE_KP_MINUS:
          if (event->key.scancode == SDL_SCANCODE_EQUALS || event->key.scancode == SDL_SCANCODE_KP_PLUS) {
            ++algonum;
          } else {
            --algonum;
          }
          algonum = (algonum + NB_FOV_ALGORITHMS) % NB_FOV_ALGORITHMS;
          break;
        default:
          break;
      }
      break;
  }
}

/* ***************************
 * image sample
 * ***************************/
void render_image(const SDL_Event* event) {
  static TCOD_image_t img = NULL;
  static TCOD_image_t circle = NULL;
  if (img == NULL) {
    img = TCOD_image_load("data/img/skull.png");
    TCOD_image_set_key_color(img, BLACK);
    circle = TCOD_image_load("data/img/circle.png");
  }
  if (event->type != ON_DRAW_USEREVENT) return;
  TCOD_console_clear(sample_console);
  const float current_time = SDL_GetTicks() / 1000.0f;
  const float x = SAMPLE_SCREEN_WIDTH / 2 + cosf(current_time) * 10.0f;
  const float y = (float)(SAMPLE_SCREEN_HEIGHT / 2);
  const float scale_x = 0.2f + 1.8f * (1.0f + cosf(current_time / 2)) / 2.0f;
  const float scale_y = scale_x;
  const float angle = current_time;
  if ((SDL_GetTicks() / 2000) & 1) {
    /* split the color channels of circle.png */
    /* the red channel */
    TCOD_console_draw_rect_rgb(sample_console, 0, 3, 15, 15, 0, NULL, &RED, TCOD_BKGND_SET);
    TCOD_image_blit_rect(circle, sample_console, 0, 3, -1, -1, TCOD_BKGND_MULTIPLY);
    /* the green channel */
    TCOD_console_draw_rect_rgb(sample_console, 15, 3, 15, 15, 0, NULL, &GREEN, TCOD_BKGND_SET);
    TCOD_image_blit_rect(circle, sample_console, 15, 3, -1, -1, TCOD_BKGND_MULTIPLY);
    /* the blue channel */
    TCOD_console_draw_rect_rgb(sample_console, 30, 3, 15, 15, 0, NULL, &BLUE, TCOD_BKGND_SET);
    TCOD_image_blit_rect(circle, sample_console, 30, 3, -1, -1, TCOD_BKGND_MULTIPLY);
  } else {
    /* render circle.png with normal blitting */
    TCOD_image_blit_rect(circle, sample_console, 0, 3, -1, -1, TCOD_BKGND_SET);
    TCOD_image_blit_rect(circle, sample_console, 15, 3, -1, -1, TCOD_BKGND_SET);
    TCOD_image_blit_rect(circle, sample_console, 30, 3, -1, -1, TCOD_BKGND_SET);
  }
  TCOD_image_blit(img, sample_console, x, y, TCOD_BKGND_SET, scale_x, scale_y, angle);
}

/* ***************************
 * mouse sample
 * ***************************/
void render_mouse(const SDL_Event* event) {
  static TCOD_mouse_t mouse = {0};
  static int tile_motion_x = 0;
  static int tile_motion_y = 0;
  float pixel_x;
  float pixel_y;
  uint32_t mouse_state = SDL_GetMouseState(&pixel_x, &pixel_y);
  int tile_x = (int)pixel_x;
  int tile_y = (int)pixel_y;
  TCOD_context_screen_pixel_to_tile_i(g_context, &tile_x, &tile_y);
  switch (event->type) {
    case ON_ENTER_USEREVENT:
      SDL_WarpMouseInWindow(NULL, 320, 200);
      SDL_ShowCursor();
      break;
    case SDL_EVENT_KEY_DOWN:
      switch (event->key.scancode) {
        case SDL_SCANCODE_1:
        case SDL_SCANCODE_KP_1:
          SDL_HideCursor();
          break;
        case SDL_SCANCODE_2:
        case SDL_SCANCODE_KP_2:
          SDL_ShowCursor();
          break;
        default:
          break;
      }
      break;
    case SDL_EVENT_MOUSE_MOTION:
      tile_motion_x = (int)event->motion.xrel;
      tile_motion_y = (int)event->motion.yrel;
      break;
  }
  TCOD_console_draw_rect_rgb(
      sample_console, 0, 0, sample_console->w, sample_console->h, 0x20, &LIGHT_YELLOW, &GREY, TCOD_BKGND_SET);
  TCOD_printf_rgb(
      sample_console,
      (TCOD_PrintParamsRGB){.x = 1, .y = 1},
      "Pixel position : %4dx%4d\n"
      "Tile position  : %4dx%4d\n"
      "Tile movement  : %4dx%4d\n"
      "Left button    : %s\n"
      "Right button   : %s\n"
      "Middle button  : %s\n",
      pixel_x,
      pixel_y,
      tile_x,
      tile_y,
      tile_motion_x,
      tile_motion_y,
      mouse_state & SDL_BUTTON_LMASK ? " ON" : "OFF",
      mouse_state & SDL_BUTTON_RMASK ? " ON" : "OFF",
      mouse_state & SDL_BUTTON_MMASK ? " ON" : "OFF");

  TCOD_printf_rgb(sample_console, (TCOD_PrintParamsRGB){.x = 1, .y = 10}, "1 : Hide cursor\n2 : Show cursor");
}

/* ***************************
 * path sample
 * ***************************/
void render_path(const SDL_Event* event) {
  static int px = 20, py = 10;  // player position
  static int dx = 24, dy = 1;  // destination
  static TCOD_map_t map = NULL;
  static TCOD_color_t dark_wall = {0, 0, 100};
  static TCOD_color_t dark_ground = {50, 50, 150};
  static TCOD_color_t light_ground = {200, 180, 50};
  static TCOD_path_t path = NULL;
  static bool usingAstar = true;
  static float dijkstraDist = 0;
  static TCOD_dijkstra_t dijkstra = NULL;
  static float busy = 0.0f;
  if (!map) {
    /* initialize the map */
    map = TCOD_map_new(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT);
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        if (SAMPLE_MAP[y][x] == ' ')
          TCOD_map_set_properties(map, x, y, true, true); /* ground */
        else if (SAMPLE_MAP[y][x] == '=')
          TCOD_map_set_properties(map, x, y, true, false); /* window */
      }
    }
    path = TCOD_path_new_using_map(map, 1.41f);
    dijkstra = TCOD_dijkstra_new(map, 1.41f);
  }
  if (event->type == ON_DRAW_USEREVENT) {
    /* we draw the foreground only the first time.
       during the player movement, only the @ is redrawn.
       the rest impacts only the background color */
    /* draw the help text & player @ */
    TCOD_console_clear(sample_console);
    TCOD_console_put_rgb(sample_console, dx, dy, '+', NULL, NULL, TCOD_BKGND_NONE);
    TCOD_console_put_rgb(sample_console, px, py, '@', NULL, NULL, TCOD_BKGND_NONE);
    TCOD_printf_rgb(
        sample_console, (TCOD_PrintParamsRGB){.x = 1, .y = 1}, "IJKL / mouse :\nmove destination\nTAB : A*/dijkstra");
    TCOD_printf_rgb(
        sample_console, (TCOD_PrintParamsRGB){.x = 1, .y = 4}, "Using : %s", usingAstar ? "A*" : "Dijkstra");
    /* draw windows */
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        if (SAMPLE_MAP[y][x] == '=') {
          TCOD_console_put_rgb(sample_console, x, y, CHAR_WINDOW, NULL, NULL, 0);  // ═
        }
      }
    }
    if (usingAstar) {
      TCOD_path_compute(path, px, py, dx, dy);
    } else {
      dijkstraDist = 0.0f;
      /* compute the distance grid */
      TCOD_dijkstra_compute(dijkstra, px, py);
      /* get the maximum distance (needed for ground shading only) */
      for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
          float d = TCOD_dijkstra_get_distance(dijkstra, x, y);
          if (d > dijkstraDist) dijkstraDist = d;
        }
      }
      // compute the path
      TCOD_dijkstra_path_set(dijkstra, dx, dy);
    }
    // draw the dungeon
    for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
        const bool wall = SAMPLE_MAP[y][x] == '#';
        TCOD_console_put_rgb(sample_console, x, y, 0, NULL, wall ? &dark_wall : &dark_ground, TCOD_BKGND_SET);
      }
    }
    // draw the path
    if (usingAstar) {
      for (int i = 0; i < TCOD_path_size(path); ++i) {
        int x, y;
        TCOD_path_get(path, i, &x, &y);
        TCOD_console_put_rgb(sample_console, x, y, 0, NULL, &light_ground, TCOD_BKGND_SET);
      }
    } else {
      for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
          const bool wall = SAMPLE_MAP[y][x] == '#';
          if (!wall) {
            const float d = TCOD_dijkstra_get_distance(dijkstra, x, y);
            const TCOD_ColorRGB bg = TCOD_color_lerp(light_ground, dark_ground, 0.9f * d / dijkstraDist);
            TCOD_console_put_rgb(sample_console, x, y, 0, NULL, &bg, TCOD_BKGND_SET);
          }
        }
      }
      for (int i = 0; i < TCOD_dijkstra_size(dijkstra); ++i) {
        int x, y;
        TCOD_dijkstra_get(dijkstra, i, &x, &y);
        TCOD_console_put_rgb(sample_console, x, y, 0, NULL, &light_ground, TCOD_BKGND_SET);
      }
    }
    // move the creature
    busy -= delta_time;
    if (busy <= 0.0f) {
      busy = 0.2f;
      if (usingAstar) {
        if (!TCOD_path_is_empty(path)) {
          TCOD_path_walk(path, &px, &py, true);
        }
      } else {
        if (!TCOD_dijkstra_is_empty(dijkstra)) {
          TCOD_dijkstra_path_walk(dijkstra, &px, &py);
        }
      }
    }
  }
  switch (event->type) {
    case SDL_EVENT_KEY_DOWN:
      switch (event->key.scancode) {
        case SDL_SCANCODE_TAB:
        case SDL_SCANCODE_KP_TAB:
          usingAstar = !usingAstar;
          break;
        case SDL_SCANCODE_I:  // destination move north
          dy = TCOD_MAX(0, dy - 1);
          break;
        case SDL_SCANCODE_K:  // destination move south
          dy = TCOD_MIN(dy + 1, SAMPLE_SCREEN_HEIGHT - 1);
          break;
        case SDL_SCANCODE_J:  // destination move west
          dx = TCOD_MAX(0, dx - 1);
          break;
        case SDL_SCANCODE_L:  // destination move east
          dx = TCOD_MIN(dx + 1, SAMPLE_SCREEN_WIDTH - 1);
          break;
        default:
          break;
      }
      break;
    case SDL_EVENT_MOUSE_MOTION: {
      const int mx = (int)event->motion.x - SAMPLE_SCREEN_X;
      const int my = (int)event->motion.y - SAMPLE_SCREEN_Y;
      if (mx >= 0 && mx < SAMPLE_SCREEN_WIDTH && my >= 0 && my < SAMPLE_SCREEN_HEIGHT && (dx != mx || dy != my)) {
        dx = mx;
        dy = my;
      }
    } break;
  }
}

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
  const int dy = (y1 > y2 ? -1 : 1);
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
    --y;
  }
}

// draw a vertical line down until we reach an empty space
void vline_down(map_t* map, int x, int y) {
  while (y < SAMPLE_SCREEN_HEIGHT && (*map)[x][y] != ' ') {
    (*map)[x][y] = ' ';
    ++y;
  }
}

// draw a horizontal line
void hline(map_t* map, int x1, int y, int x2) {
  int x = x1;
  const int dx = (x1 > x2 ? -1 : 1);
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
    --x;
  }
}

// draw a horizontal line right until we reach an empty space
void hline_right(map_t* map, int x, int y) {
  while (x < SAMPLE_SCREEN_WIDTH && (*map)[x][y] != ' ') {
    (*map)[x][y] = ' ';
    ++x;
  }
}

// the class building the dungeon from the bsp nodes
bool traverse_node(TCOD_bsp_t* node, void* userData) {
  map_t* map = (map_t*)userData;
  if (TCOD_bsp_is_leaf(node)) {
    // calculate the room size
    int minx = node->x + 1;
    int maxx = node->x + node->w - 1;
    int miny = node->y + 1;
    int maxy = node->y + node->h - 1;
    if (!roomWalls) {
      if (minx > 1) --minx;
      if (miny > 1) --miny;
    }
    if (maxx == SAMPLE_SCREEN_WIDTH - 1) --maxx;
    if (maxy == SAMPLE_SCREEN_HEIGHT - 1) --maxy;
    if (randomRoom) {
      minx = TCOD_random_get_int(NULL, minx, maxx - minRoomSize + 1);
      miny = TCOD_random_get_int(NULL, miny, maxy - minRoomSize + 1);
      maxx = TCOD_random_get_int(NULL, minx + minRoomSize - 1, maxx);
      maxy = TCOD_random_get_int(NULL, miny + minRoomSize - 1, maxy);
    }
    // resize the node to fit the room
    //	printf("node %dx%d %dx%d => room %dx%d
    //%dx%d\n",node->x,node->y,node->w,node->h,minx,miny,maxx-minx+1,maxy-miny+1);
    node->x = minx;
    node->y = miny;
    node->w = maxx - minx + 1;
    node->h = maxy - miny + 1;
    // dig the room
    for (int x = minx; x <= maxx; ++x) {
      for (int y = miny; y <= maxy; ++y) {
        (*map)[x][y] = ' ';
      }
    }
  } else {
    //	printf("lvl %d %dx%d %dx%d\n",node->level, node->x,node->y,node->w,node->h);
    // resize the node to fit its sons
    const TCOD_bsp_t* left = TCOD_bsp_left(node);
    const TCOD_bsp_t* right = TCOD_bsp_right(node);
    node->x = TCOD_MIN(left->x, right->x);
    node->y = TCOD_MIN(left->y, right->y);
    node->w = TCOD_MAX(left->x + left->w, right->x + right->w) - node->x;
    node->h = TCOD_MAX(left->y + left->h, right->y + right->h) - node->y;
    // create a corridor between the two lower nodes
    if (node->horizontal) {
      // vertical corridor
      if (left->x + left->w - 1 < right->x || right->x + right->w - 1 < left->x) {
        // no overlapping zone. we need a Z shaped corridor
        const int x1 = TCOD_random_get_int(NULL, left->x, left->x + left->w - 1);
        const int x2 = TCOD_random_get_int(NULL, right->x, right->x + right->w - 1);
        const int y = TCOD_random_get_int(NULL, left->y + left->h, right->y);
        vline_up(map, x1, y - 1);
        hline(map, x1, y, x2);
        vline_down(map, x2, y + 1);
      } else {
        // straight vertical corridor
        const int minx = TCOD_MAX(left->x, right->x);
        const int maxx = TCOD_MIN(left->x + left->w - 1, right->x + right->w - 1);
        const int x = TCOD_random_get_int(NULL, minx, maxx);
        vline_down(map, x, right->y);
        vline_up(map, x, right->y - 1);
      }
    } else {
      // horizontal corridor
      if (left->y + left->h - 1 < right->y || right->y + right->h - 1 < left->y) {
        // no overlapping zone. we need a Z shaped corridor
        const int y1 = TCOD_random_get_int(NULL, left->y, left->y + left->h - 1);
        const int y2 = TCOD_random_get_int(NULL, right->y, right->y + right->h - 1);
        const int x = TCOD_random_get_int(NULL, left->x + left->w, right->x);
        hline_left(map, x - 1, y1);
        vline(map, x, y1, y2);
        hline_right(map, x + 1, y2);
      } else {
        // straight horizontal corridor
        const int miny = TCOD_MAX(left->y, right->y);
        const int maxy = TCOD_MIN(left->y + left->h - 1, right->y + right->h - 1);
        const int y = TCOD_random_get_int(NULL, miny, maxy);
        hline_left(map, right->x - 1, y);
        hline_right(map, right->x, y);
      }
    }
  }
  return true;
}

void render_bsp(const SDL_Event* event) {
  static TCOD_bsp_t* bsp = NULL;
  static bool generate = true;
  static bool refresh = false;
  static char map[SAMPLE_SCREEN_WIDTH][SAMPLE_SCREEN_HEIGHT];
  static const TCOD_color_t darkWall = {0, 0, 100};
  static const TCOD_color_t darkGround = {50, 50, 150};
  switch (event->type) {
    case SDL_EVENT_KEY_DOWN:
      switch (event->key.scancode) {
        case SDL_SCANCODE_RETURN:
        case SDL_SCANCODE_RETURN2:
        case SDL_SCANCODE_KP_ENTER:
          if (event->key.mod & SDL_KMOD_ALT) break;
          generate = true;
          break;
        case SDL_SCANCODE_SPACE:
        case SDL_SCANCODE_KP_SPACE:
          refresh = true;
          break;
        case SDL_SCANCODE_EQUALS:
        case SDL_SCANCODE_KP_PLUS:
          ++bspDepth;
          generate = true;
          break;
        case SDL_SCANCODE_MINUS:
        case SDL_SCANCODE_KP_MINUS:
          if (bspDepth <= 1) break;
          --bspDepth;
          generate = true;
          break;
        case SDL_SCANCODE_8:
        case SDL_SCANCODE_KP_MULTIPLY:
          ++minRoomSize;
          generate = true;
          break;
        case SDL_SCANCODE_SLASH:
        case SDL_SCANCODE_KP_DIVIDE:
          if (minRoomSize <= 2) break;
          --minRoomSize;
          generate = true;
          break;
        case SDL_SCANCODE_1:
        case SDL_SCANCODE_KP_1:
          randomRoom = !randomRoom;
          if (!randomRoom) roomWalls = true;
          refresh = true;
          break;
        case SDL_SCANCODE_2:
        case SDL_SCANCODE_KP_2:
          roomWalls = !roomWalls;
          refresh = true;
          break;
        default:
          break;
      }
      break;
  }
  if (generate || refresh) {
    // dungeon generation
    if (!bsp) {
      // create the bsp
      bsp = TCOD_bsp_new_with_size(0, 0, SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT);
    } else {
      // restore the nodes size
      TCOD_bsp_resize(bsp, 0, 0, SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT);
    }
    memset(map, '#', sizeof(map));
    if (generate) {
      // build a new random bsp tree
      TCOD_bsp_remove_sons(bsp);
      TCOD_bsp_split_recursive(
          bsp, NULL, bspDepth, minRoomSize + (roomWalls ? 1 : 0), minRoomSize + (roomWalls ? 1 : 0), 1.5f, 1.5f);
    }
    // create the dungeon from the bsp
    TCOD_bsp_traverse_inverted_level_order(bsp, traverse_node, &map);
    generate = false;
    refresh = false;
  }
  TCOD_console_clear(sample_console);
  TCOD_printf_rgb(
      sample_console,
      (TCOD_PrintParamsRGB){.x = 1, .y = 1},
      "ENTER : rebuild bsp\nSPACE : rebuild dungeon\n+-: bsp depth %d\n*/: room size %d\n1 : random room size %s",
      bspDepth,
      minRoomSize,
      randomRoom ? "ON" : "OFF");
  if (randomRoom)
    TCOD_printf_rgb(
        sample_console, (TCOD_PrintParamsRGB){.x = 1, .y = 6}, "2 : room walls %s", roomWalls ? "ON" : "OFF");
  // render the level
  for (int y = 0; y < SAMPLE_SCREEN_HEIGHT; ++y) {
    for (int x = 0; x < SAMPLE_SCREEN_WIDTH; ++x) {
      const bool wall = (map[x][y] == '#');
      TCOD_console_put_rgb(sample_console, x, y, 0, NULL, wall ? &darkWall : &darkGround, TCOD_BKGND_SET);
    }
  }
}

/* ***************************
 * name generator sample
 * ***************************/
void render_name(const SDL_Event* event) {
  static int nbSets;
  static int curSet = 0;
  static float delay = 0.0f;
  static TCOD_list_t sets = NULL;
  static TCOD_list_t names = NULL;
  if (!names) {
    names = TCOD_list_new();
    int file_count = 0;
    char** files = SDL_GlobDirectory(".", "data/namegen/*.cfg", 0, &file_count);
    // parse all the files
    for (int i = 0; i < file_count; ++i) {
      TCOD_namegen_parse(files[i], NULL);
    }
    SDL_free(files);
    // get the sets list
    sets = TCOD_namegen_get_sets();
    nbSets = TCOD_list_size(sets);
  }

  while (TCOD_list_size(names) >= 15) {
    // remove the first element.
#ifndef TCOD_VISUAL_STUDIO
    char* nameToRemove = *(TCOD_list_begin(names));
#endif
    TCOD_list_remove_iterator(names, TCOD_list_begin(names));
    // for some reason, this crashes on MSVC...
#ifndef TCOD_VISUAL_STUDIO
    free(nameToRemove);
#endif
  }

  TCOD_console_draw_rect_rgb(
      sample_console, 0, 0, sample_console->w, sample_console->h, 0x20, &LIGHT_YELLOW, &GREY, TCOD_BKGND_SET);
  if (TCOD_list_size(sets) > 0) {
    TCOD_printf_rgb(
        sample_console,
        (TCOD_PrintParamsRGB){.x = 1, .y = 1},
        "%s\n\n+ : next generator\n- : prev generator",
        (char*)TCOD_list_get(sets, curSet));
    for (int i = 0; i < TCOD_list_size(names); ++i) {
      const char* name = (const char*)TCOD_list_get(names, i);
      if (strlen(name) < SAMPLE_SCREEN_WIDTH)
        TCOD_printf_rgb(
            sample_console,
            (TCOD_PrintParamsRGB){.x = 0, .width = SAMPLE_SCREEN_WIDTH - 2, .y = 2 + i, .alignment = TCOD_RIGHT},
            "%s",
            name);
    }

    delay += delta_time;
    if (delay >= 0.5f) {
      delay -= 0.5f;
      // add a new name to the list
      TCOD_list_push(names, TCOD_namegen_generate((const char*)TCOD_list_get(sets, curSet), true));
    }
    switch (event->type) {
      case SDL_EVENT_KEY_DOWN:
        switch (event->key.scancode) {
          case SDL_SCANCODE_EQUALS:
          case SDL_SCANCODE_KP_PLUS:
            ++curSet;
            if (curSet == nbSets) curSet = 0;
            TCOD_list_push(names, strdup("======"));
            break;
          case SDL_SCANCODE_MINUS:
          case SDL_SCANCODE_KP_MINUS:
            --curSet;
            if (curSet < 0) curSet = nbSets - 1;
            TCOD_list_push(names, strdup("======"));
            break;
          default:
            break;
        }
        break;
    }
  } else {
    TCOD_printf_rgb(sample_console, (TCOD_PrintParamsRGB){.x = 1, .y = 1}, "Unable to find name config data files.");
  }
}

/* ***************************
 * SDL callback sample
 * ***************************/
#ifndef NO_SDL_SAMPLE
static TCOD_noise_t noise = NULL;
static bool sdl_callback_enabled = false;
static int effectNum = 0;
static float delay = 3.0f;

void burn(SDL_Surface* screen, int sample_x, int sample_y, int sample_w, int sample_h) {
  const SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(screen->format);
  const int r_idx = format->Rshift / 8;
  const int g_idx = format->Gshift / 8;
  const int b_idx = format->Bshift / 8;
  for (int x = sample_x; x < sample_x + sample_w; ++x) {
    uint8_t* p = (uint8_t*)screen->pixels + x * format->bytes_per_pixel + sample_y * screen->pitch;
    for (int y = sample_y; y < sample_y + sample_h; ++y) {
      int ir = 0, ig = 0, ib = 0;
      const uint8_t* p2 = p + format->bytes_per_pixel;  // get pixel at x+1,y
      ir += p2[r_idx];
      ig += p2[g_idx];
      ib += p2[b_idx];
      p2 -= 2 * format->bytes_per_pixel;  // get pixel at x-1,y
      ir += p2[r_idx];
      ig += p2[g_idx];
      ib += p2[b_idx];
      p2 += format->bytes_per_pixel + screen->pitch;  // get pixel at x,y+1
      ir += p2[r_idx];
      ig += p2[g_idx];
      ib += p2[b_idx];
      p2 -= 2 * screen->pitch;  // get pixel at x,y-1
      ir += p2[r_idx];
      ig += p2[g_idx];
      ib += p2[b_idx];
      p[r_idx] = (uint8_t)(ir / 4);
      p[g_idx] = (uint8_t)(ig / 4);
      p[b_idx] = (uint8_t)(ib / 4);
      p += screen->pitch;
    }
  }
}

void explode(SDL_Surface* screen, int sample_x, int sample_y, int sample_w, int sample_h) {
  const SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(screen->format);
  const int r_idx = format->Rshift / 8;
  const int g_idx = format->Gshift / 8;
  const int b_idx = format->Bshift / 8;
  const int dist = (int)(10 * (3.0f - delay));
  for (int x = sample_x; x < sample_x + sample_w; ++x) {
    uint8_t* p = (uint8_t*)screen->pixels + x * format->bytes_per_pixel + sample_y * screen->pitch;
    for (int y = sample_y; y < sample_y + sample_h; ++y) {
      int ir = 0, ig = 0, ib = 0;
      for (int i = 0; i < 3; ++i) {
        const int dx = TCOD_random_get_int(NULL, -dist, dist);
        const int dy = TCOD_random_get_int(NULL, -dist, dist);
        const uint8_t* p2;
        p2 = p + dx * format->bytes_per_pixel;
        p2 += dy * screen->pitch;
        ir += p2[r_idx];
        ig += p2[g_idx];
        ib += p2[b_idx];
      }
      p[r_idx] = (uint8_t)(ir / 3);
      p[g_idx] = (uint8_t)(ig / 3);
      p[b_idx] = (uint8_t)(ib / 3);
      p += screen->pitch;
    }
  }
}

void blur(SDL_Surface* screen, int sample_x, int sample_y, int sample_w, int sample_h) {
  const SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(screen->format);
  // let's blur that sample console
  float n = 0.0f;
  const int r_idx = format->Rshift / 8;
  const int g_idx = format->Gshift / 8;
  const int b_idx = format->Bshift / 8;
  float f[3] = {0, 0, SDL_GetTicks() / 1000.0f};
  if (noise == NULL) noise = TCOD_noise_new(3, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, NULL);
  for (int x = sample_x; x < sample_x + sample_w; ++x) {
    uint8_t* p = (uint8_t*)screen->pixels + x * format->bytes_per_pixel + sample_y * screen->pitch;
    f[0] = (float)(x) / sample_w;
    for (int y = sample_y; y < sample_y + sample_h; ++y) {
      int ir = 0, ig = 0, ib = 0;
      if ((y - sample_y) % 8 == 0) {
        f[1] = (float)(y) / sample_h;
        n = TCOD_noise_get_fbm(noise, f, 3.0f);
      }
      const int dec = (int)(3 * (n + 1.0f));
      int count = 0;
      switch (dec) {
        case 4:
          count += 4;
          // get pixel at x,y
          ir += p[r_idx];
          ig += p[g_idx];
          ib += p[b_idx];
          p -= 2 * format->bytes_per_pixel;  // get pixel at x+2,y
          ir += p[r_idx];
          ig += p[g_idx];
          ib += p[b_idx];
          p -= 2 * screen->pitch;  // get pixel at x+2,y+2
          ir += p[r_idx];
          ig += p[g_idx];
          ib += p[b_idx];
          p += 2 * format->bytes_per_pixel;  // get pixel at x,y+2
          ir += p[r_idx];
          ig += p[g_idx];
          ib += p[b_idx];
          p += 2 * screen->pitch;
          //@fallthrough@
        case 3:
          count += 4;
          // get pixel at x,y
          ir += p[r_idx];
          ig += p[g_idx];
          ib += p[b_idx];
          p += 2 * format->bytes_per_pixel;  // get pixel at x+2,y
          ir += p[r_idx];
          ig += p[g_idx];
          ib += p[b_idx];
          p += 2 * screen->pitch;  // get pixel at x+2,y+2
          ir += p[r_idx];
          ig += p[g_idx];
          ib += p[b_idx];
          p -= 2 * format->bytes_per_pixel;  // get pixel at x,y+2
          ir += p[r_idx];
          ig += p[g_idx];
          ib += p[b_idx];
          p -= 2 * screen->pitch;
          //@fallthrough@
        case 2:
          count += 4;
          // get pixel at x,y
          ir += p[r_idx];
          ig += p[g_idx];
          ib += p[b_idx];
          p -= format->bytes_per_pixel;  // get pixel at x-1,y
          ir += p[r_idx];
          ig += p[g_idx];
          ib += p[b_idx];
          p -= screen->pitch;  // get pixel at x-1,y-1
          ir += p[r_idx];
          ig += p[g_idx];
          ib += p[b_idx];
          p += format->bytes_per_pixel;  // get pixel at x,y-1
          ir += p[r_idx];
          ig += p[g_idx];
          ib += p[b_idx];
          p += screen->pitch;
          //@fallthrough@
        case 1:
          count += 4;
          // get pixel at x,y
          ir += p[r_idx];
          ig += p[g_idx];
          ib += p[b_idx];
          p += format->bytes_per_pixel;  // get pixel at x+1,y
          ir += p[r_idx];
          ig += p[g_idx];
          ib += p[b_idx];
          p += screen->pitch;  // get pixel at x+1,y+1
          ir += p[r_idx];
          ig += p[g_idx];
          ib += p[b_idx];
          p -= format->bytes_per_pixel;  // get pixel at x,y+1
          ir += p[r_idx];
          ig += p[g_idx];
          ib += p[b_idx];
          p -= screen->pitch;
          p[r_idx] = (uint8_t)(ir / count);
          p[g_idx] = (uint8_t)(ig / count);
          p[b_idx] = (uint8_t)(ib / count);
          break;
        default:
          break;
      }
      p += screen->pitch;
    }
  }
}

void SDL_render(SDL_Surface* screen) {
  // now we have almighty access to the screen's precious pixels !!
  // get the font character size
  const int char_w = TCOD_tileset_get_tile_width_(tileset);
  const int char_h = TCOD_tileset_get_tile_height_(tileset);
  // compute the sample console position in pixels
  const int sample_x = SAMPLE_SCREEN_X * char_w;
  const int sample_y = SAMPLE_SCREEN_Y * char_h;
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

void render_sdl(const SDL_Event* event) {
  switch (event->type) {
    case ON_ENTER_USEREVENT:
      // use noise sample as background. rendering is done in SampleRenderer
      TCOD_console_draw_rect_rgb(
          sample_console, 0, 0, sample_console->w, sample_console->h, 0x20, &WHITE, &LIGHT_BLUE, TCOD_BKGND_SET);
      TCOD_printf_rgb(
          sample_console,
          (TCOD_PrintParamsRGB){
              .x = 0,
              .y = 3,
              .width = SAMPLE_SCREEN_WIDTH,
              .fg = &WHITE,
              .bg = &LIGHT_BLUE,
              .alignment = TCOD_CENTER,
          },
          "The SDL callback gives you access to the screen surface so that you can alter the pixels one by one using "
          "SDL API or any API on top of SDL. SDL is used here to blur the sample console.\n\nHit TAB to enable/disable "
          "the callback. While enabled, it will be active on other samples too.\n\nNote that the SDL callback only "
          "works with SDL renderer.");
      break;
    case SDL_EVENT_KEY_DOWN:
      switch (event->key.scancode) {
        case SDL_SCANCODE_TAB:
        case SDL_SCANCODE_KP_TAB:
          sdl_callback_enabled = !sdl_callback_enabled;
          if (sdl_callback_enabled) {
            TCOD_sys_register_SDL_renderer(SDL_render);
          } else {
            TCOD_sys_register_SDL_renderer(NULL);
          }
          break;
        default:
          break;
      }
      break;
  }
}
#endif  // NO_SDL_SAMPLE

/* ***************************
 * the list of samples
 * ***************************/
static const sample_t samples[] = {
    {"  True colors        ", render_colors},
    {"  Offscreen console  ", render_offscreen},
    {"  Line drawing       ", render_lines},
    {"  Noise              ", render_noise},
    {"  Field of view      ", render_fov},
    {"  Path finding       ", render_path},
    {"  Bsp toolkit        ", render_bsp},
    {"  Image toolkit      ", render_image},
    {"  Mouse support      ", render_mouse},
    {"  Name generator     ", render_name},
#ifndef NO_SDL_SAMPLE
    {"  SDL callback       ", render_sdl},
#endif
};
static const int nb_samples = sizeof(samples) / sizeof(*samples); /* total number of samples */

// A renderer selection.
typedef struct RendererOption {
  const char* name;
  TCOD_renderer_t renderer;
} RendererOption;
// The available renderers and their labels.
static const RendererOption RENDERER_OPTIONS[] = {
    {"OPENGL ", TCOD_RENDERER_OPENGL},
    {"SDL    ", TCOD_RENDERER_SDL},
    {"SDL2   ", TCOD_RENDERER_SDL2},
    {"OPENGL2", TCOD_RENDERER_OPENGL2},
    {"XTERM  ", TCOD_RENDERER_XTERM},
};
// The total number of renderer options.
static const int RENDERER_OPTIONS_COUNT = sizeof(RENDERER_OPTIONS) / sizeof(*RENDERER_OPTIONS);

// Return the current framerate from the sampled frames.
static float get_framerate() {
  double total_time = 0.0;
  for (int i = 0; i < DELTA_SAMPLES_LENGTH; ++i) total_time += delta_samples[i];
  if (total_time == 0) return 0.0f;
  return 1.0f / (float)(total_time / DELTA_SAMPLES_LENGTH);
}

// Print libtcod log messages.
void print_log(const TCOD_LogMessage* message, void* userdata) {
  (void)userdata;  // Unused
  printf("%s:%d:%d:%s\n", message->source, message->lineno, message->level, message->message);
}

/* ***************************
 * the main function
 * ***************************/
int main(int argc, char* argv[]) {
  SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
  TCOD_set_log_callback(print_log, NULL);
  TCOD_set_log_level(TCOD_LOG_DEBUG);
  static const SDL_Event on_enter_event = {.type = ON_ENTER_USEREVENT};
  static const SDL_Event on_draw_event = {.type = ON_DRAW_USEREVENT};
  static const char* FONT = "data/fonts/dejavu12x12_gs_tc.png";
  tileset = TCOD_tileset_load(FONT, 32, 8, 256, TCOD_CHARMAP_TCOD);
  if (!tileset) fatal("Could not load font %s: %s", FONT, TCOD_get_error());
  TCOD_Console* main_console = TCOD_console_new(80, 50);  // The main console to be presented.
  if (!main_console) fatal("Could not allocate console: %s", TCOD_get_error());
  TCOD_ContextParams params = {
      .console = main_console,
      .window_title = "libtcod C sample",
      .sdl_window_flags = SDL_WINDOW_RESIZABLE,
      .renderer_type = TCOD_RENDERER_SDL2,
      .tileset = tileset,
      .vsync = false,
      .argc = argc,
      .argv = (const char* const*)argv,
  };

  if (TCOD_context_new(&params, &g_context) < 0) fatal("Could not open context: %s", TCOD_get_error());
  atexit(TCOD_quit);

  uint64_t last_time = SDL_GetTicks();

  /* initialize the offscreen console for the samples */
  sample_console = TCOD_console_new(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT);
  if (!sample_console) fatal("Could not allocate console: %s", TCOD_get_error());
  int cur_sample = 0; /* index of the current sample */
  samples[cur_sample].render(&on_enter_event);
  while (true) {
    const uint64_t current_time = SDL_GetTicks();
    const int64_t delta_time_ms = TCOD_MAX(0, (int64_t)(current_time - last_time));
    last_time = current_time;
    delta_time = (float)(delta_time_ms) / 1000.0f;

    delta_samples[last_delta_sample] = delta_time;
    last_delta_sample = (last_delta_sample + 1) % DELTA_SAMPLES_LENGTH;

    TCOD_console_clear(main_console);

    static bool credits_end = false;
    if (!credits_end) {
      credits_end = TCOD_console_credits_render_ex(main_console, 56, 43, false, delta_time);
    }
    /* print the list of samples */
    for (int i = 0; i < nb_samples; ++i) {
      const bool is_highlighted = i == cur_sample;  // Hightligt colors for the active sample
      // Print the sample name
      TCOD_printf_rgb(
          main_console,
          (TCOD_PrintParamsRGB){
              .x = 2,
              .y = 46 - (nb_samples - i),
              .fg = is_highlighted ? &WHITE : &GREY,
              .bg = is_highlighted ? &LIGHT_BLUE : &BLACK,
          },
          "%s",
          samples[i].name);
    }
    /* print the help message */
    TCOD_printf_rgb(
        main_console,
        (TCOD_PrintParamsRGB){.x = 0, .width = main_console->w, .y = 46, .fg = &GREY, .alignment = TCOD_RIGHT},
        "last frame : %3d ms (%3.0f fps)",
        delta_time_ms,
        get_framerate());
    TCOD_printf_rgb(
        main_console,
        (TCOD_PrintParamsRGB){.x = 0, .width = main_console->w, .y = 47, .fg = &GREY, .alignment = TCOD_RIGHT},
        "elapsed : %5dms %5.2fs",
        SDL_GetTicks(),
        SDL_GetTicks() / 1000.0f);
    TCOD_printf_rgb(main_console, (TCOD_PrintParamsRGB){.x = 2, .y = 47, .fg = &GREY}, "↑↓ : select a sample");
    TCOD_printf_rgb(
        main_console,
        (TCOD_PrintParamsRGB){.x = 2, .y = 48, .fg = &GREY},
        "ALT-ENTER : switch to %s",
        SDL_GetWindowFlags(TCOD_context_get_sdl_window(g_context)) & SDL_WINDOW_FULLSCREEN ? "windowed mode"
                                                                                           : "fullscreen mode");

    /* blit the sample console on the root console */
    TCOD_console_blit(
        sample_console,
        0,
        0,
        SAMPLE_SCREEN_WIDTH,
        SAMPLE_SCREEN_HEIGHT, /* the source console & zone to blit */
        main_console,
        SAMPLE_SCREEN_X,
        SAMPLE_SCREEN_Y, /* the destination console & position */
        1.0f,
        1.0f /* alpha coefficients */
    );
    /* display renderer list and current renderer */
    const TCOD_renderer_t current_renderer = TCOD_context_get_renderer_type(g_context);
    TCOD_printf_rgb(
        main_console,
        (TCOD_PrintParamsRGB){.x = 42, .y = 46 - (RENDERER_OPTIONS_COUNT + 1), .fg = &GREY, .bg = &BLACK},
        "Renderer :");
    for (int i = 0; i < RENDERER_OPTIONS_COUNT; ++i) {
      // Highlight colors for selected renderer
      const bool is_highlighted = RENDERER_OPTIONS[i].renderer == current_renderer;
      TCOD_printf_rgb(
          main_console,
          (TCOD_PrintParamsRGB){
              .x = 42,
              .y = 46 - (RENDERER_OPTIONS_COUNT - i),
              .fg = is_highlighted ? &WHITE : &GREY,
              .bg = is_highlighted ? &LIGHT_BLUE : &BLACK,
          },
          "F%d %s",
          i + 1,
          RENDERER_OPTIONS[i].name);
    }

    samples[cur_sample].render(&on_draw_event);

    /* update the game screen */
    TCOD_context_present(g_context, main_console, NULL);

    // Check for events.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      TCOD_context_convert_event_coordinates(g_context, &event);
      switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
          switch (event.key.scancode) {
            case SDL_SCANCODE_DOWN:  // Next sample.
              if (++cur_sample >= nb_samples) cur_sample = 0;
              samples[cur_sample].render(&on_enter_event);
              break;
            case SDL_SCANCODE_UP:  // Previous sample.
              if (--cur_sample < 0) cur_sample = nb_samples - 1;
              samples[cur_sample].render(&on_enter_event);
              break;
            case SDL_SCANCODE_RETURN:  // Toggle fullscreen with Alt+Enter.
            case SDL_SCANCODE_RETURN2:
            case SDL_SCANCODE_KP_ENTER:
              if (event.key.mod & SDL_KMOD_ALT) {
                SDL_Window* sdl_window = TCOD_context_get_sdl_window(g_context);
                if (sdl_window) {
                  const bool is_fullscreen = (SDL_GetWindowFlags(sdl_window) & SDL_WINDOW_FULLSCREEN) != 0;
                  SDL_SetWindowFullscreen(sdl_window, !is_fullscreen);
                }
              }
              break;
            case SDL_SCANCODE_PRINTSCREEN:  // Save screenshot.
              TCOD_context_save_screenshot(g_context, NULL);
              break;
            default: {
              // Switch renderers with the function keys.
              const int renderer_pick = event.key.scancode - SDL_SCANCODE_F1;
              if (0 <= renderer_pick && renderer_pick < RENDERER_OPTIONS_COUNT) {
                // Preserve window flags and position during context switching.
                SDL_Window* sdl_window = TCOD_context_get_sdl_window(g_context);
                if (sdl_window) {
                  params.sdl_window_flags = (int)SDL_GetWindowFlags(sdl_window);
                  if ((params.sdl_window_flags & (SDL_WINDOW_MAXIMIZED | SDL_WINDOW_FULLSCREEN)) == 0) {
                    // Don't track window size/position when fullscreen.
                    SDL_GetWindowSize(sdl_window, &params.pixel_width, &params.pixel_height);
                    SDL_GetWindowPosition(sdl_window, &params.window_x, &params.window_y);
                    params.window_xy_defined = true;
                  }
                }
                params.renderer_type = RENDERER_OPTIONS[renderer_pick].renderer;
                TCOD_context_delete(g_context);
                if (TCOD_context_new(&params, &g_context) < 0) fatal("Could not open context: %s", TCOD_get_error());
              }
            } break;
          }
          break;
        case SDL_EVENT_DROP_FILE: {  // Change to a new tileset when one is dropped on the window.
          TCOD_Tileset* new_tileset = NULL;
          if (str_ends_with(event.drop.data, ".bdf")) {
            new_tileset = TCOD_load_bdf(event.drop.data);
          } else if (str_ends_with(event.drop.data, "_tc.png")) {
            new_tileset = TCOD_tileset_load(event.drop.data, 32, 8, 256, TCOD_CHARMAP_TCOD);
          } else {
            new_tileset = TCOD_tileset_load(event.drop.data, 16, 16, 256, TCOD_CHARMAP_CP437);
          }
          if (new_tileset) {
            TCOD_tileset_delete(tileset);
            params.tileset = tileset = new_tileset;
            TCOD_context_change_tileset(g_context, tileset);
          }
        } break;
        case SDL_EVENT_QUIT:
          return EXIT_SUCCESS;  // Exit program by returning from main.
      }
      /* render current sample */
      samples[cur_sample].render(&event);
    }
  }
}
