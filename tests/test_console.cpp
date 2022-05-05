
#include <libtcod/console.h>

#include <catch2/catch.hpp>
#include <libtcod/console.hpp>

#include "common.hpp"

TEST_CASE("Console basics") {
  auto console = tcod::Console{3, 2};
  for (const auto& tile : console) {
    REQUIRE(tile.ch == 0x20);
    REQUIRE(tile.fg == TCOD_ColorRGBA{255, 255, 255, 255});
    REQUIRE(tile.bg == TCOD_ColorRGBA{0, 0, 0, 255});
  }
  for (int y = 0; y < console.get_height(); ++y) {
    for (int x = 0; x < console.get_width(); ++x) {
      REQUIRE(console.in_bounds({x, y}));
      console.at({x, y}) = {0x20, {255, 255, 255, 255}, {static_cast<uint8_t>(x), static_cast<uint8_t>(y), 0, 255}};
    }
  }
  REQUIRE(console.at(2, 1) == console[{2, 1}]);
  REQUIRE(console.at({1, 1}) == console[{1, 1}]);
  REQUIRE(console.in_bounds({1000, 1000}) == false);
}

TEST_CASE("Console bounds", "[!throws]") {
  auto console = tcod::Console{3, 2};
  REQUIRE_THROWS(console.at({1000, 1000}));
}

TEST_CASE("TCODConsole conversion") {
  auto console = TCODConsole(3, 2);
  tcod::print(console, {0, 0}, "@", std::nullopt, std::nullopt);
  REQUIRE(static_cast<const TCOD_Console&>(console).at(0, 0).ch == '@');
}

TEST_CASE("Color control.") {
  auto console = tcod::Console(3, 1);
  TCODConsole::setColorControl(TCOD_COLCTRL_1, {1, 2, 3}, {4, 5, 6});
  TCODConsole::setColorControl(TCOD_COLCTRL_2, {7, 8, 9}, {10, 11, 12});
  TCOD_console_printf(console.get(), 0, 0, "%c1%c2%c3", TCOD_COLCTRL_1, TCOD_COLCTRL_2, TCOD_COLCTRL_STOP);
  REQUIRE(to_string(console) == ("123"));
  // Because console->bkgnd_flag is at its default this will not affect the background colors.
  CHECK(console.at(0, 0).fg == TCOD_ColorRGBA{1, 2, 3, 255});
  CHECK(console.at(0, 0).bg == TCOD_ColorRGBA{0, 0, 0, 255});  // Remains unset, since bkgnd_flag was unset.
  CHECK(console.at(1, 0).fg == TCOD_ColorRGBA{7, 8, 9, 255});
  CHECK(console.at(1, 0).bg == TCOD_ColorRGBA{0, 0, 0, 255});
  CHECK(console.at(2, 0).fg == TCOD_ColorRGBA{255, 255, 255, 255});
  CHECK(console.at(2, 0).bg == TCOD_ColorRGBA{0, 0, 0, 255});

  console = tcod::Console(3, 1);
  static_cast<TCOD_Console&>(console).bkgnd_flag = TCOD_BKGND_SET;
  TCOD_console_printf(console.get(), 0, 0, "%c1%c2%c3", TCOD_COLCTRL_1, TCOD_COLCTRL_2, TCOD_COLCTRL_STOP);
  REQUIRE(to_string(console) == ("123"));
  CHECK(console.at(0, 0).fg == TCOD_ColorRGBA{1, 2, 3, 255});
  CHECK(console.at(0, 0).bg == TCOD_ColorRGBA{4, 5, 6, 255});
  CHECK(console.at(1, 0).fg == TCOD_ColorRGBA{7, 8, 9, 255});
  CHECK(console.at(1, 0).bg == TCOD_ColorRGBA{10, 11, 12, 255});
  CHECK(console.at(2, 0).fg == TCOD_ColorRGBA{255, 255, 255, 255});
  CHECK(console.at(2, 0).bg == TCOD_ColorRGBA{0, 0, 0, 255});
}

TEST_CASE("Console ascii") {
  TCODConsole console = TCODConsole(5, 1);
  console.print(0, 0, "Test");
  CHECK(to_string(*console.get_data()) == "Test ");
}

TEST_CASE("Console print") {
  TCODConsole console = TCODConsole(5, 1);
  console.print(0, 0, std::string("plop"));
  CHECK(to_string(*console.get_data()) == "plop ");
}
TEST_CASE("Console print empty") {
  TCODConsole console = TCODConsole(5, 1);
  console.print(0, 0, std::string(""));
}

TEST_CASE("Console eascii") {
  TCODConsole console = TCODConsole(2, 1);
  const char test_str[] = {static_cast<char>(0xff), static_cast<char>(0x00)};
  console.print(0, 0, test_str);
  CHECK(console.getChar(0, 0) == 0xff);
  CHECK(console.getChar(1, 0) == 0x20);
}

TEST_CASE("Console UTF-8 BMP") {
  TCODConsole console = TCODConsole(2, 1);
  console.printf(0, 0, "☃");
  CHECK(console.getChar(0, 0) == 0x2603);
  CHECK(console.getChar(1, 0) == 0x20);
}

TEST_CASE("Console UTF-8 SMP") {
  TCODConsole console = TCODConsole(2, 1);
  console.printf(0, 0, "🌍");
  CHECK(console.getChar(0, 0) == 0x1F30D);
  CHECK(console.getChar(1, 0) == 0x20);
}

TEST_CASE("Console wchar BMP", "[!nonportable]") {
  TCODConsole console = TCODConsole(2, 1);
  console.print(0, 0, L"\u2603");
  CHECK(console.getChar(0, 0) == 0x2603);
  CHECK(console.getChar(1, 0) == 0x20);
}

/* Fails when sizeof(wchar_t) == 2 */
TEST_CASE("Console wchar SMP", "[!nonportable][!mayfail][!hide]") {
  TCODConsole console = TCODConsole(2, 1);
  console.print(0, 0, L"\U0001F30D");
  CHECK(console.getChar(0, 0) == 0x1F30D);
  CHECK(console.getChar(1, 0) == 0x20);
}
