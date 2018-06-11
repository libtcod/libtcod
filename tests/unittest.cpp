
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

TEST_CASE("Console ascii") {
  TCODConsole console = TCODConsole(5, 1);
  console.print(0, 0, "Test");
  for (int i = 0; i < 5; ++i) {
    CHECK(console.getChar(i, 0) == (int)("Test "[i]));
  }
}

TEST_CASE("Console eascii") {
  TCODConsole console = TCODConsole(2, 1);
  const char test_str[] = { (char)0xff, (char)0x00 };
  console.print(0, 0, test_str);
  CHECK(console.getChar(0, 0) == 0xff);
  CHECK(console.getChar(1, 0) == 0x20);
}

/* libtcod needs new functionality to support utf-8 */
TEST_CASE("Console utf-8", "[!mayfail]") {
  TCODConsole console = TCODConsole(2, 1);
  console.print(0, 0, u8"\u2603");
  CHECK(console.getChar(0, 0) == 0x2603);
  CHECK(console.getChar(1, 0) == 0x20);
}

TEST_CASE("Console wchar BMP", "[!nonportable]") {
  TCODConsole console = TCODConsole(2, 1);
  console.print(0, 0, L"\u2603");
  CHECK(console.getChar(0, 0) == 0x2603);
  CHECK(console.getChar(1, 0) == 0x20);
}

/* Fails when sizeof(wchar_t) == 2 */
TEST_CASE("Console wchar SMP", "[!nonportable][!mayfail]") {
  TCODConsole console = TCODConsole(2, 1);
  console.print(0, 0, L"\U0001F30D");
  /* libtcod does not support utf-16, both of these checks normally fail. */
  CHECK(console.getChar(0, 0) == 0x1F30D);
  CHECK(console.getChar(1, 0) == 0x20);
}
