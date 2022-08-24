#include <catch2/catch_all.hpp>
#include <libtcod/console_printing.hpp>

#include "common.hpp"

TEST_CASE("tcod::print") {
  auto console = tcod::Console{5, 1};
  static constexpr auto FG = TCOD_ColorRGB{1, 2, 3};
  static constexpr auto BG = TCOD_ColorRGB{4, 5, 6};
  tcod::print(console, {0, 0}, "Test", FG, BG);
  CHECK(to_string(console) == "Test ");
  CHECK(console.at(0, 0).fg == TCOD_ColorRGBA{1, 2, 3, 255});
  CHECK(console.at(0, 0).bg == TCOD_ColorRGBA{4, 5, 6, 255});
  CHECK(console.at(4, 0).fg == TCOD_ColorRGBA{255, 255, 255, 255});
  CHECK(console.at(4, 0).bg == TCOD_ColorRGBA{0, 0, 0, 255});
}

TEST_CASE("String from printf.") { CHECK(tcod::stringf("%s%s%s", "1", "2", "3") == "123"); }

void test_alignment(TCOD_alignment_t alignment) {
  // Compare alignment between the new and old functions.
  int x = GENERATE(0, 1, 2, 3, 4, 5, 6);
  int width = GENERATE(11, 12);
  INFO("x=" << x << ", width=" << width);
  auto console1 = tcod::Console{width, 1};
  auto console2 = tcod::Console{width, 1};
  SECTION("Print text.") {
    for (auto& tile : console1) tile.ch = static_cast<int>('.');
    for (auto& tile : console2) tile.ch = static_cast<int>('.');
    TCOD_console_print_ex(console1.get(), x, 0, TCOD_BKGND_NONE, alignment, "123");
    tcod::print(console2, {x, 0}, "123", std::nullopt, std::nullopt, alignment);
    CHECK(to_string(console1) == to_string(console2));
  }
  SECTION("Print rect.") {}
}
TEST_CASE("Left alignment regression.") { test_alignment(TCOD_LEFT); }
TEST_CASE("Center alignment regression.") { test_alignment(TCOD_CENTER); }
TEST_CASE("Right alignment regression.") { test_alignment(TCOD_RIGHT); }
TEST_CASE("Rectangle text alignment.") {
  auto console = tcod::Console(12, 1);
  // TCOD_Console* console = TCOD_console_new(12, 1);
  for (auto& tile : console) {
    tile.ch = static_cast<int>('.');
  }
  tcod::print_rect(console, {0, 0, 0, 0}, "123", std::nullopt, std::nullopt, TCOD_LEFT, TCOD_BKGND_NONE);
  tcod::print_rect(console, {0, 0, 0, 0}, "123", std::nullopt, std::nullopt, TCOD_CENTER, TCOD_BKGND_NONE);
  tcod::print_rect(console, {0, 0, 0, 0}, "123", std::nullopt, std::nullopt, TCOD_RIGHT, TCOD_BKGND_NONE);
  CHECK(to_string(console) == "123.123..123");
}
TEST_CASE("Print color codes.") {
  using namespace std::string_literals;
  auto console = tcod::Console{8, 1};
  std::string text = "1\u0006\u0001\u0002\u00032\u00083"s;
  tcod::print(console, {0, 0}, text, TCOD_white, TCOD_black, TCOD_LEFT, TCOD_BKGND_SET);
  REQUIRE(to_string(console) == "123     ");
  CHECK(console.at(0, 0).fg.r == 255);
  CHECK(console.at(0, 0).fg.g == 255);
  CHECK(console.at(0, 0).fg.b == 255);
  CHECK(console.at(0, 0).fg.a == 255);
  CHECK(console.at(1, 0).fg.r == 1);
  CHECK(console.at(1, 0).fg.g == 2);
  CHECK(console.at(1, 0).fg.b == 3);
  CHECK(console.at(1, 0).fg.a == 255);
  CHECK(console.at(2, 0).fg.r == 255);
  CHECK(console.at(2, 0).fg.g == 255);
  CHECK(console.at(2, 0).fg.b == 255);
  CHECK(console.at(2, 0).fg.a == 255);
}
TEST_CASE("Color code formatting.") {
  using namespace std::string_literals;
  auto console = tcod::Console(3, 3);
  std::string text = "1\u0006\u0001\u0002\u0003\n2 \u0008\n 3"s;
  for (auto& tile : console) {
    tile.ch = static_cast<int>('.');
  }
  tcod::print(console, {0, 0}, text, TCOD_white, TCOD_black, TCOD_LEFT);
  REQUIRE(to_string(console) == ("1..\n2 .\n 3."));
}
TEST_CASE("Malformed UTF-8.", "[!throws]") {
  auto console = tcod::Console{8, 1};
  std::string text = "\x80";
  REQUIRE_THROWS(tcod::print(console, {0, 0}, text, TCOD_white, TCOD_black));
}
TEST_CASE("Unicode PUA.") {
  auto console = tcod::Console{1, 1};
  auto check_character = [&](int codepoint) {
    tcod::print(console, {0, 0}, ucs4_to_utf8(codepoint), TCOD_white, TCOD_black);
    REQUIRE(console.at(0, 0).ch == codepoint);
  };
  for (int i = 0xE000; i <= 0xF8FF; ++i) check_character(i);
  for (int i = 0xF0000; i <= 0xFFFFD; ++i) check_character(i);
  for (int i = 0x100000; i <= 0x10FFFD; ++i) check_character(i);
}
