#ifndef TCOD_NO_ZLIB
#include <array>
#include <catch2/catch_all.hpp>
#include <cstdio>
#include <filesystem>
#include <libtcod/console.hpp>
#include <libtcod/console_rexpaint.hpp>
#include <utility>
#include <vector>

const int COMPRESSION_LEVEL = 0;  // Disable compression.

TEST_CASE("REXPaint in-memory.") {
  const uint8_t WIDTH = 3;
  const uint8_t HEIGHT = 2;
  auto console1 = tcod::Console{WIDTH, HEIGHT};
  auto console2 = tcod::Console{WIDTH, HEIGHT};
  for (uint8_t y = 0; y < HEIGHT; ++y) {
    for (uint8_t x = 0; x < WIDTH; ++x) {
      console1.at(x, y) = {'!', {x, y, 1, 255}, {10, 20, 30, 255}};
      console2.at(x, y) = {'@', {x, y, 2, 255}, {10, 20, 30, 255}};
    }
  }
  std::array<TCOD_Console*, 2> console_array{console1.get(), console2.get()};
  int buffer_size = TCOD_save_xp_to_memory(
      static_cast<int>(console_array.size()), console_array.data(), 0, nullptr, COMPRESSION_LEVEL);
  REQUIRE(buffer_size > 0);
  std::vector<unsigned char> buffer(buffer_size);
  buffer_size = TCOD_save_xp_to_memory(
      static_cast<int>(console_array.size()), console_array.data(), buffer_size, &buffer[0], COMPRESSION_LEVEL);
  REQUIRE(buffer_size >= 0);
  std::array<TCOD_Console*, 2> console_out{nullptr, nullptr};
  REQUIRE(
      tcod::check_throw_error(TCOD_load_xp_from_memory(buffer_size, buffer.data(), 0, nullptr)) == console_out.size());
  REQUIRE(
      TCOD_load_xp_from_memory(buffer_size, buffer.data(), static_cast<int>(console_out.size()), &console_out[0]) ==
      console_out.size());
  for (int i = 0; i < WIDTH * HEIGHT; ++i) {
    REQUIRE(console1.begin()[i] == console_out.at(0)->tiles[i]);
    REQUIRE(console2.begin()[i] == console_out.at(1)->tiles[i]);
  }
  for (auto& it : console_out) TCOD_console_delete(it);
}

TEST_CASE("REXPaint save negative size") {
  char tmp_file[L_tmpnam]{};
  std::tmpnam(tmp_file);
  TCOD_Console bad_console = {};
  bad_console.w = -1;
  bad_console.h = -1;
  TCOD_Console* bad_console_ptr = &bad_console;
  REQUIRE(TCOD_save_xp_to_memory(1, &bad_console_ptr, 0, nullptr, COMPRESSION_LEVEL) == TCOD_E_ERROR);
  REQUIRE(TCOD_save_xp(1, &bad_console_ptr, tmp_file, COMPRESSION_LEVEL) == TCOD_E_ERROR);
  REQUIRE_FALSE(std::filesystem::exists(tmp_file));
}

TEST_CASE("REXPaint layer-count bomb") {
  /// @brief REXPaint file with INT_MAX layer count.
  constexpr std::array<uint8_t, 27> HUGE_LAYER_COUNT_XP = {120, 1,   1,   16,  0,   239, 255, 255, 255,
                                                           255, 255, 255, 255, 255, 127, 255, 255, 255,
                                                           127, 255, 255, 255, 127, 128, 8,   14,  113};
  REQUIRE(
      TCOD_load_xp_from_memory((int)HUGE_LAYER_COUNT_XP.size(), HUGE_LAYER_COUNT_XP.data(), 0, NULL) == TCOD_E_ERROR);

  TCOD_Console* console = NULL;
  REQUIRE(
      TCOD_load_xp_from_memory((int)HUGE_LAYER_COUNT_XP.size(), HUGE_LAYER_COUNT_XP.data(), 1, &console) ==
      TCOD_E_ERROR);
  REQUIRE_FALSE(console);
}

TEST_CASE("REXPaint layer-size bomb") {
  /// @brief REXPaint file with one layer with a width/height of INT_MAX.
  constexpr std::array<uint8_t, 27> HUGE_LAYER_SIZE_XP = {120, 1,   1,   16,  0,   239, 255, 255, 255,
                                                          255, 255, 1,   0,   0,   0,   255, 255, 255,
                                                          127, 255, 255, 255, 127, 90,  190, 10,  246};
  REQUIRE(TCOD_load_xp_from_memory((int)HUGE_LAYER_SIZE_XP.size(), HUGE_LAYER_SIZE_XP.data(), 0, NULL) == 1);

  TCOD_Console* console = NULL;
  REQUIRE(
      TCOD_load_xp_from_memory((int)HUGE_LAYER_SIZE_XP.size(), HUGE_LAYER_SIZE_XP.data(), 1, &console) == TCOD_E_ERROR);
  REQUIRE_FALSE(console);
}

TEST_CASE("REXPaint negative layers") {
  /// @brief REXPaint file with -1 layers.
  constexpr std::array<uint8_t, 27> NEGATIVE_LAYER_COUNT_XP = {
      120, 1, 1, 16, 0, 239, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 99, 172, 7, 249};

  REQUIRE(
      TCOD_load_xp_from_memory((int)NEGATIVE_LAYER_COUNT_XP.size(), NEGATIVE_LAYER_COUNT_XP.data(), 0, NULL) ==
      TCOD_E_ERROR);
}

TEST_CASE("REXPaint negative layer size") {
  /// @brief REXPaint file with one layer with a width/height of -1.
  constexpr std::array<uint8_t, 27> NEGATIVE_LAYER_SIZE_XP = {120, 1,   1,   16,  0,   239, 255, 255, 255,
                                                              255, 255, 1,   0,   0,   0,   255, 255, 255,
                                                              255, 255, 255, 255, 255, 93,  190, 11,  246};
  REQUIRE(
      TCOD_load_xp_from_memory((int)NEGATIVE_LAYER_SIZE_XP.size(), NEGATIVE_LAYER_SIZE_XP.data(), 0, NULL) ==
      TCOD_E_ERROR);

  TCOD_Console* console = NULL;
  REQUIRE(
      TCOD_load_xp_from_memory((int)NEGATIVE_LAYER_SIZE_XP.size(), NEGATIVE_LAYER_SIZE_XP.data(), 1, &console) ==
      TCOD_E_ERROR);
  REQUIRE_FALSE(console);
}

TEST_CASE("REXPaint to file.") {
  char tmp_file[L_tmpnam]{};
  std::tmpnam(tmp_file);
  const uint8_t WIDTH = 3;
  const uint8_t HEIGHT = 2;
  auto console1 = tcod::Console{WIDTH, HEIGHT};
  auto console2 = tcod::Console{WIDTH, HEIGHT};
  for (uint8_t y = 0; y < HEIGHT; ++y) {
    for (uint8_t x = 0; x < WIDTH; ++x) {
      console1.at(x, y) = {'!', {x, y, 1, 255}, {10, 20, 30, 255}};
      console2.at(x, y) = {'@', {x, y, 2, 255}, {10, 20, 30, 255}};
    }
  }
  tcod::save_xp({console1.get(), console2.get()}, tmp_file, 0);
  auto consoles = tcod::load_xp(tmp_file);
  REQUIRE(std::filesystem::remove(tmp_file));
  for (int i = 0; i < WIDTH * HEIGHT; ++i) {
    REQUIRE(console1.begin()[i] == consoles.at(0)->tiles[i]);
    REQUIRE(console2.begin()[i] == consoles.at(1)->tiles[i]);
  }
}

TEST_CASE("REXPaint from file.") {
  tcod::Console console{std::move(tcod::load_xp("data/rexpaint/test.xp").at(0))};
  REQUIRE(console.at(0, 0).ch == 'T');
  REQUIRE(console.at(1, 0).ch == 'e');
  REQUIRE(console.at(2, 0).ch == 's');
  REQUIRE(console.at(3, 0).ch == 't');
  REQUIRE(console.at(0, 1).bg == TCOD_ColorRGBA{255, 0, 0, 255});
  REQUIRE(console.at(1, 1).bg == TCOD_ColorRGBA{0, 255, 0, 255});
  REQUIRE(console.at(2, 1).bg == TCOD_ColorRGBA{0, 0, 255, 255});
  REQUIRE(console.at(3, 1).bg == TCOD_ColorRGBA{0, 0, 0, 255});
}
TEST_CASE("REXPaint empty list") {
  char tmp_file[L_tmpnam]{};
  std::tmpnam(tmp_file);

  tcod::save_xp({}, tmp_file, 0);

  auto consoles = tcod::load_xp(tmp_file);
  REQUIRE(consoles.size() == 0);

  REQUIRE(std::filesystem::remove(tmp_file));
}

TEST_CASE("REXPaint empty console") {
  char tmp_file[L_tmpnam]{};
  std::tmpnam(tmp_file);

  auto empty_console = tcod::Console{0, 0};
  tcod::save_xp({empty_console.get()}, tmp_file, 0);

  auto consoles = tcod::load_xp(tmp_file);
  REQUIRE(consoles.size() == 1);
  REQUIRE(consoles.at(0).get()->w == 0);
  REQUIRE(consoles.at(0).get()->h == 0);

  REQUIRE(std::filesystem::remove(tmp_file));
}
#endif  // TCOD_NO_ZLIB
