
#include <libtcod/console.h>
#include <libtcod/console_rexpaint.h>

#include <array>
#include <catch2/catch.hpp>
#include <cstdio>
#include <utility>
#include <vector>


const int COMPRESSION_LEVEL = 0;  // Disable compression.

TEST_CASE("REXPaint in-memory.") {
  const uint8_t WIDTH = 3;
  const uint8_t HEIGHT = 2;
  tcod::ConsolePtr console1 = tcod::new_console(WIDTH, HEIGHT);
  tcod::ConsolePtr console2 = tcod::new_console(WIDTH, HEIGHT);
  for (uint8_t y = 0; y < HEIGHT; ++y) {
    for (uint8_t x = 0; x < WIDTH; ++x) {
      console1->at(x, y) = {'!', {x, y, 1, 255}, {10, 20, 30, 255}};
      console2->at(x, y) = {'@', {x, y, 2, 255}, {10, 20, 30, 255}};
    }
  }
  std::array<TCOD_Console*, 2> console_array{console1.get(), console2.get()};
  int buffer_size = TCOD_save_xp_to_memory(
      static_cast<int>(console_array.size()), console_array.data(), 0, nullptr, COMPRESSION_LEVEL);
  REQUIRE(buffer_size > 0);
  std::vector<unsigned char> buffer(buffer_size);
  TCOD_save_xp_to_memory(
      static_cast<int>(console_array.size()), console_array.data(), buffer_size, &buffer[0], COMPRESSION_LEVEL);
  std::array<TCOD_Console*, 2> console_out{nullptr, nullptr};
  REQUIRE(TCOD_load_xp_from_memory(buffer_size, buffer.data(), 0, nullptr) == console_out.size());
  REQUIRE(
      TCOD_load_xp_from_memory(buffer_size, buffer.data(), static_cast<int>(console_out.size()), &console_out[0]) ==
      console_out.size());
  for (int i = 0; i < WIDTH * HEIGHT; ++i) {
    REQUIRE(console1->tiles[i] == console_out.at(0)->tiles[i]);
    REQUIRE(console2->tiles[i] == console_out.at(1)->tiles[i]);
  }
  for (auto& it : console_out) TCOD_console_delete(it);
}

TEST_CASE("REXPaint bad data.") {
  char tmp_file[L_tmpnam];
  std::tmpnam(tmp_file);
  TCOD_Console bad_console = {};
  bad_console.w = -1;
  bad_console.h = -1;
  TCOD_Console* bad_console_ptr = &bad_console;
  REQUIRE(TCOD_save_xp_to_memory(1, &bad_console_ptr, 0, nullptr, COMPRESSION_LEVEL) == TCOD_E_ERROR);
  REQUIRE(TCOD_save_xp(1, &bad_console_ptr, tmp_file, COMPRESSION_LEVEL) == TCOD_E_ERROR);
}

TEST_CASE("REXPaint to file.") {
  char tmp_file[L_tmpnam];
  std::tmpnam(tmp_file);
  const uint8_t WIDTH = 3;
  const uint8_t HEIGHT = 2;
  tcod::ConsolePtr console1 = tcod::new_console(WIDTH, HEIGHT);
  tcod::ConsolePtr console2 = tcod::new_console(WIDTH, HEIGHT);
  for (uint8_t y = 0; y < HEIGHT; ++y) {
    for (uint8_t x = 0; x < WIDTH; ++x) {
      console1->at(x, y) = {'!', {x, y, 1, 255}, {10, 20, 30, 255}};
      console2->at(x, y) = {'@', {x, y, 2, 255}, {10, 20, 30, 255}};
    }
  }
  tcod::save_xp({console1.get(), console2.get()}, tmp_file, 0);
  auto consoles = tcod::load_xp(tmp_file);
  REQUIRE(std::remove(tmp_file) == 0);
  for (int i = 0; i < WIDTH * HEIGHT; ++i) {
    REQUIRE(console1->tiles[i] == consoles.at(0)->tiles[i]);
    REQUIRE(console2->tiles[i] == consoles.at(1)->tiles[i]);
  }
}

TEST_CASE("REXPaint from file.") {
  tcod::ConsolePtr console{std::move(tcod::load_xp("data/rexpaint/test.xp").at(0))};
  REQUIRE(console);
  REQUIRE(console->at(0, 0).ch == 'T');
  REQUIRE(console->at(1, 0).ch == 'e');
  REQUIRE(console->at(2, 0).ch == 's');
  REQUIRE(console->at(3, 0).ch == 't');
  REQUIRE(console->at(0, 1).bg == TCOD_ColorRGBA{255, 0, 0, 255});
  REQUIRE(console->at(1, 1).bg == TCOD_ColorRGBA{0, 255, 0, 255});
  REQUIRE(console->at(2, 1).bg == TCOD_ColorRGBA{0, 0, 255, 255});
  REQUIRE(console->at(3, 1).bg == TCOD_ColorRGBA{0, 0, 0, 255});
}
