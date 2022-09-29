/*
 * libtcod C samples
 * This code demonstrates various usages of libtcod modules
 * It's in the public domain.
 */

// uncomment this to disable SDL sample (might cause compilation issues on some systems)
//#define NO_SDL_SAMPLE

#include <SDL.h>
#include <libtcod.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* a sample has a name and a rendering function */
typedef struct {
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
#define ON_ENTER_USEREVENT (SDL_USEREVENT + 0)
// A custom SDL event to tell a sample to draw.
#define ON_DRAW_USEREVENT (SDL_USEREVENT + 1)

static float delta_time = 0.0f;  // The time in seconds of the current frame.
#define DELTA_SAMPLES_LENGTH 64
static float delta_samples[DELTA_SAMPLES_LENGTH] = {0};
static int last_delta_sample = 0;

/* ***************************
 * samples rendering functions
 * ***************************/

TCOD_Context* g_context;
static TCOD_Console* sample_console;  // the offscreen console in which the samples are rendered.

/* ***************************
 * the main function
 * ***************************/
int main(int argc, char* argv[]) {
  SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN);
  static const SDL_Event on_enter_event = {.type = ON_ENTER_USEREVENT};
  static const SDL_Event on_draw_event = {.type = ON_DRAW_USEREVENT};
  static const char* FONT = "data/fonts/dejavu12x12_gs_tc.png";
  TCOD_Tileset* tileset = TCOD_tileset_load(FONT, 32, 8, 256, TCOD_CHARMAP_TCOD);
  TCOD_Console* main_console = TCOD_console_new(80, 50);  // The main console to be presented.
  TCOD_ContextParams params = {
      .tcod_version = TCOD_COMPILEDVERSION,
      .console = main_console,
      .window_title = "libtcod C sample",
      .sdl_window_flags = SDL_WINDOW_RESIZABLE,
      .renderer_type = TCOD_RENDERER_SDL2,
      .tileset = tileset,
      .vsync = false,
      .argc = argc,
      .argv = argv,
  };

  // if (TCOD_context_new(&params, &g_context) < 0) fatal("Could not open context: %s", TCOD_get_error());
  TCOD_context_new(&params, &g_context);
  atexit(TCOD_quit);

  uint32_t last_time = SDL_GetTicks();

  while (true) {
    const uint32_t current_time = SDL_GetTicks();
    const int32_t delta_time_ms = MAX(0, (int32_t)(current_time - last_time));
    last_time = current_time;
    delta_time = (float)(delta_time_ms) / 1000.0f;
    /* update the game screen */
    TCOD_context_present(g_context, main_console, NULL);

    // Check for events.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      TCOD_context_convert_event_coordinates(g_context, &event);
      switch (event.type) {
        case SDL_QUIT:
          return EXIT_SUCCESS;  // Exit program by returning from main.
      }
    }

    TCOD_PrintParams pparams = {.x = 5, .y = 5, .fg = {255, 255, 255}};
    TCOD_printf(main_console, pparams, "%s", "Hello World");
  }
}
