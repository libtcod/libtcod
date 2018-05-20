
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "catch_reporter_automake.hpp"
#include "catch_reporter_tap.hpp"

#include <libtcod.h>
#include <libtcod.hpp>

const int WIDTH = 20;
const int HEIGHT = 10;
const char *TITLE = "Unittest";

TEST_CASE("SDL renderer") {
  TCOD_console_init_root(WIDTH, HEIGHT, TITLE, 0, TCOD_RENDERER_SDL);
  REQUIRE(TCOD_console_get_width(NULL) == WIDTH);
  REQUIRE(TCOD_console_get_height(NULL) == HEIGHT);

  TCOD_console_delete(NULL);
}
