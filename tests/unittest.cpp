#define CATCH_CONFIG_MAIN

#include <libtcod.h>
#include <libtcod/logging.h>
#include <libtcod/tileset_fallback.h>

#include <catch2/catch.hpp>
#include <clocale>
#include <cstddef>
#include <iostream>
#include <libtcod.hpp>
#include <limits>
#include <random>
#include <string>
#include <utility>

#include "common.h"

/**
    Convert a Unicode codepoint to a UTF-8 multi-byte string.
 */
std::string ucs4_to_utf8(int ucs4) {
  char utf8[4];
  if (ucs4 < 0) {
    throw std::invalid_argument("Invalid codepoint.");
  } else if (ucs4 <= 0x7F) {
    utf8[0] = ucs4;
    return std::string(utf8, 1);
  } else if (ucs4 <= 0x07FF) {
    utf8[0] = 0b11000000 | ((ucs4 & 0b11111'000000) >> 6);
    utf8[1] = 0b10000000 | ((ucs4 & 0b00000'111111) >> 0);
    return std::string(utf8, 2);
  } else if (ucs4 <= 0xFFFF) {
    utf8[0] = 0b11100000 | ((ucs4 & 0b1111'000000'000000) >> 12);
    utf8[1] = 0b10000000 | ((ucs4 & 0b0000'111111'000000) >> 6);
    utf8[2] = 0b10000000 | ((ucs4 & 0b0000'000000'111111) >> 0);
    return std::string(utf8, 3);
  } else if (ucs4 <= 0x10FFFF) {
    utf8[0] = 0b11110000 | ((ucs4 & 0b111'000000'000000'000000) >> 18);
    utf8[1] = 0b10000000 | ((ucs4 & 0b000'111111'000000'000000) >> 12);
    utf8[2] = 0b10000000 | ((ucs4 & 0b000'000000'111111'000000) >> 6);
    utf8[3] = 0b10000000 | ((ucs4 & 0b000'000000'000000'111111) >> 0);
    return std::string(utf8, 4);
  }
  throw std::invalid_argument("Invalid codepoint.");
}

/// Captures libtcod log output on tests.
struct HandleLogging : Catch::TestEventListenerBase {
  using TestEventListenerBase::TestEventListenerBase;  // inherit constructor
  /// Register logger before each test.
  void testCaseStarting(Catch::TestCaseInfo const&) override {
    TCOD_set_log_level(TCOD_LOG_DEBUG);
    TCOD_set_log_callback(&catch_log, nullptr);
  }
  static void catch_log(const TCOD_LogMessage* message, void*) {
    INFO("libtcod:" << message->source << ":" << message->lineno << ":" << message->level << ":" << message->message);
  }
};
CATCH_REGISTER_LISTENER(HandleLogging)

std::ostream& operator<<(std::ostream& out, const std::array<ptrdiff_t, 2>& data) {
  return out << '{' << data.at(0) << ',' << ' ' << data.at(1) << '}';
}

const int WIDTH = 20;
const int HEIGHT = 10;
const char* TITLE = "Unittest";

TCOD_Tileset* new_test_tileset(int width, int height) {
  TCOD_Tileset* tileset = TCOD_tileset_new(width, height);
  std::vector<TCOD_ColorRGBA> buffer{(size_t)(width * height + 1)};
  for (int i = 0; i < 256; ++i) {
    TCOD_tileset_set_tile_(tileset, i, buffer.data());
  }
  return tileset;
}

void test_renderer_old_api(TCOD_renderer_t renderer) {
  TCOD_console_set_custom_font(
      get_file("fonts/terminal8x8_gs_ro.png").c_str(), TCOD_FONT_LAYOUT_CP437 | TCOD_FONT_TYPE_GREYSCALE, 0, 0);
  tcod::console::init_root(WIDTH, HEIGHT, TITLE, 0, renderer);
  REQUIRE(TCOD_console_get_width(NULL) == WIDTH);
  REQUIRE(TCOD_console_get_height(NULL) == HEIGHT);
  TCODConsole::flush();
  TCOD_console_delete(NULL);
}

void test_renderer_new_api(TCOD_renderer_t renderer) {
  tcod::ConsolePtr console = tcod::new_console(16, 12);
  TCOD_ContextParams params = {TCOD_COMPILEDVERSION};
  params.tileset = new_test_tileset(25, 24);
  params.renderer_type = renderer;
  params.columns = console->w;
  params.rows = console->h;

  tcod::ContextPtr context = tcod::new_context(params);
  TCOD_tileset_delete(params.tileset);
  context->present(*console);

#if 0
  // Check for division by zero errors:
  TCOD_Tileset* tileset = new_test_tileset(0, 0);
  context->c_set_tileset_(context.get(), tileset);
  context->present(*console);
  TCOD_tileset_delete(tileset);
#endif
}

void test_renderer(TCOD_renderer_t renderer) {
  test_renderer_new_api(renderer);
  test_renderer_old_api(renderer);
}

TEST_CASE("SDL2 Renderer", "[!nonportable]") { test_renderer(TCOD_RENDERER_SDL2); }
TEST_CASE("SDL Renderer", "[!nonportable]") { test_renderer(TCOD_RENDERER_SDL); }
TEST_CASE("OPENGL Renderer", "[!nonportable]") { test_renderer(TCOD_RENDERER_OPENGL); }
TEST_CASE("OPENGL2 Renderer", "[!nonportable]") { test_renderer(TCOD_RENDERER_OPENGL2); }

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

TEST_CASE("Pathfinder Benchmarks", "[.benchmark]") {
  const int SIZE = 50;
  BENCHMARK("Classic libtcod A* 50x50") {
    TCOD_Map* map = TCOD_map_new(SIZE, SIZE);
    TCOD_map_clear(map, 1, 1);
    TCOD_Path* astar = TCOD_path_new_using_map(map, 1.0f);
    TCOD_path_compute(astar, 0, 0, SIZE - 1, SIZE - 1);
    TCOD_path_delete(astar);
    TCOD_map_delete(map);
  };
}

TEST_CASE("Fallback font.", "[!mayfail]") { REQUIRE(tcod::tileset::new_fallback_tileset()); }

void test_alignment(TCOD_alignment_t alignment) {
  // Compare alignment between the new and old functions.
  int x = GENERATE(0, 1, 2, 3, 4, 5, 6);
  int width = GENERATE(11, 12);
  INFO("x=" << x << ", width=" << width);
  TCOD_Console* console1 = TCOD_console_new(width, 1);
  TCOD_Console* console2 = TCOD_console_new(width, 1);
  SECTION("Print text.") {
    for (auto& tile : *console1) {
      tile.ch = static_cast<int>('.');
    }
    for (auto& tile : *console2) {
      tile.ch = static_cast<int>('.');
    }
    TCOD_console_print_ex(console1, x, 0, TCOD_BKGND_NONE, alignment, "123");
    TCOD_console_printf_ex(console2, x, 0, TCOD_BKGND_NONE, alignment, "123");
    CHECK(to_string(*console1) == to_string(*console2));
  }
  SECTION("Print rect.") {}
  TCOD_console_delete(console1);
  TCOD_console_delete(console2);
}
TEST_CASE("Left alignment regression.") { test_alignment(TCOD_LEFT); }
TEST_CASE("Center alignment regression.") { test_alignment(TCOD_CENTER); }
TEST_CASE("Right alignment regression.") { test_alignment(TCOD_RIGHT); }
TEST_CASE("Rectangle text alignment.") {
  auto console = tcod::new_console(12, 1);
  // TCOD_Console* console = TCOD_console_new(12, 1);
  for (auto& tile : *console) {
    tile.ch = static_cast<int>('.');
  }
  tcod::print_rect(*console, {0, 0, 0, 0}, "123", nullptr, nullptr, TCOD_BKGND_NONE, TCOD_LEFT);
  tcod::print_rect(*console, {0, 0, 0, 0}, "123", nullptr, nullptr, TCOD_BKGND_NONE, TCOD_CENTER);
  tcod::print_rect(*console, {0, 0, 0, 0}, "123", nullptr, nullptr, TCOD_BKGND_NONE, TCOD_RIGHT);
  CHECK(to_string(*console) == "123.123..123");
}
TEST_CASE("Print color codes.") {
  using namespace std::string_literals;
  auto console = tcod::new_console(8, 1);
  std::string text = "1\u0006\u0001\u0002\u00032\u00083"s;
  tcod::print(*console, {0, 0}, text, &TCOD_white, &TCOD_black, TCOD_BKGND_SET, TCOD_LEFT);
  REQUIRE(to_string(*console) == "123     ");
  CHECK(console->at(0, 0).fg.r == 255);
  CHECK(console->at(0, 0).fg.g == 255);
  CHECK(console->at(0, 0).fg.b == 255);
  CHECK(console->at(0, 0).fg.a == 255);
  CHECK(console->at(1, 0).fg.r == 1);
  CHECK(console->at(1, 0).fg.g == 2);
  CHECK(console->at(1, 0).fg.b == 3);
  CHECK(console->at(1, 0).fg.a == 255);
  CHECK(console->at(2, 0).fg.r == 255);
  CHECK(console->at(2, 0).fg.g == 255);
  CHECK(console->at(2, 0).fg.b == 255);
  CHECK(console->at(2, 0).fg.a == 255);
}
TEST_CASE("Color code formatting.") {
  using namespace std::string_literals;
  auto console = tcod::new_console(3, 3);
  std::string text = "1\u0006\u0001\u0002\u0003\n2 \u0008\n 3"s;
  for (auto& tile : *console) {
    tile.ch = static_cast<int>('.');
  }
  tcod::print(*console, {0, 0}, text, &TCOD_white, &TCOD_black, TCOD_BKGND_SET, TCOD_LEFT);
  REQUIRE(to_string(*console) == ("1..\n2 .\n 3."));
}
TEST_CASE("Malformed UTF-8.", "[!throws]") {
  auto console = tcod::new_console(8, 1);
  std::string text = "\x80";
  REQUIRE_THROWS(tcod::print(*console, {0, 0}, text, &TCOD_white, &TCOD_black, TCOD_BKGND_SET, TCOD_LEFT));
}
TEST_CASE("Unicode PUA.") {
  auto console = tcod::new_console(1, 1);
  auto check_character = [&](char32_t codepoint) {
    tcod::print(*console, {0, 0}, ucs4_to_utf8(codepoint), &TCOD_white, &TCOD_black, TCOD_BKGND_SET, TCOD_LEFT);
    REQUIRE(console->at(0, 0).ch == codepoint);
  };
  for (char32_t i = 0xE000; i <= 0xF8FF; ++i) check_character(i);
  for (char32_t i = 0xF0000; i <= 0xFFFFD; ++i) check_character(i);
  for (char32_t i = 0x100000; i <= 0x10FFFD; ++i) check_character(i);
}
TEST_CASE("Heap test.") {
  struct TCOD_Heap heap;
  TCOD_heap_init(&heap, sizeof(int));
  std::vector<int> INPUT{0, 1, 2, 3, 4, 5, 6, 7};
  for (int& it : INPUT) {
    TCOD_minheap_push(&heap, it, &it);
  }
  std::vector<int> output;
  while (heap.size) {
    int out;
    TCOD_minheap_pop(&heap, &out);
    output.emplace_back(out);
  }
  REQUIRE(INPUT == output);
  TCOD_heap_uninit(&heap);
}

TEST_CASE("Noise Benchmarks", "[.benchmark]") {
  TCOD_Random* rng = TCOD_random_new_from_seed(TCOD_RNG_MT, 0);
  TCOD_Noise* noise1d = TCOD_noise_new(1, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, rng);
  TCOD_Noise* noise2d = TCOD_noise_new(2, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, rng);
  TCOD_Noise* noise3d = TCOD_noise_new(3, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, rng);
  TCOD_Noise* noise4d = TCOD_noise_new(4, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, rng);
  const float POINT[4] = {0.5f, 0.5f, 0.5f, 0.5f};
  (void)!TCOD_noise_get_ex(noise1d, POINT, TCOD_NOISE_WAVELET);  // Pre-generate wavelet data.
  (void)!TCOD_noise_get_ex(noise2d, POINT, TCOD_NOISE_WAVELET);
  (void)!TCOD_noise_get_ex(noise3d, POINT, TCOD_NOISE_WAVELET);
  BENCHMARK("Perlin 1D") { (void)!TCOD_noise_get_ex(noise1d, POINT, TCOD_NOISE_PERLIN); };
  BENCHMARK("Perlin 2D") { (void)!TCOD_noise_get_ex(noise2d, POINT, TCOD_NOISE_PERLIN); };
  BENCHMARK("Perlin 3D") { (void)!TCOD_noise_get_ex(noise3d, POINT, TCOD_NOISE_PERLIN); };
  BENCHMARK("Perlin 4D") { (void)!TCOD_noise_get_ex(noise4d, POINT, TCOD_NOISE_PERLIN); };
  BENCHMARK("Simplex 1D") { (void)!TCOD_noise_get_ex(noise1d, POINT, TCOD_NOISE_SIMPLEX); };
  BENCHMARK("Simplex 2D") { (void)!TCOD_noise_get_ex(noise2d, POINT, TCOD_NOISE_SIMPLEX); };
  BENCHMARK("Simplex 3D") { (void)!TCOD_noise_get_ex(noise3d, POINT, TCOD_NOISE_SIMPLEX); };
  BENCHMARK("Simplex 4D") { (void)!TCOD_noise_get_ex(noise4d, POINT, TCOD_NOISE_SIMPLEX); };
  BENCHMARK("Wavelet 1D") { (void)!TCOD_noise_get_ex(noise1d, POINT, TCOD_NOISE_WAVELET); };
  BENCHMARK("Wavelet 2D") { (void)!TCOD_noise_get_ex(noise2d, POINT, TCOD_NOISE_WAVELET); };
  BENCHMARK("Wavelet 3D") { (void)!TCOD_noise_get_ex(noise3d, POINT, TCOD_NOISE_WAVELET); };

  BENCHMARK("Simplex fbM 1D octaves=4") { (void)!TCOD_noise_get_fbm_ex(noise1d, POINT, 4, TCOD_NOISE_SIMPLEX); };
  BENCHMARK("Simplex fbM 2D octaves=4") { (void)!TCOD_noise_get_fbm_ex(noise2d, POINT, 4, TCOD_NOISE_SIMPLEX); };
  BENCHMARK("Simplex fbM 3D octaves=4") { (void)!TCOD_noise_get_fbm_ex(noise3d, POINT, 4, TCOD_NOISE_SIMPLEX); };
  BENCHMARK("Simplex fbM 4D octaves=4") { (void)!TCOD_noise_get_fbm_ex(noise4d, POINT, 4, TCOD_NOISE_SIMPLEX); };
  BENCHMARK("Simplex turbulence 1D octaves=4") {
    (void)!TCOD_noise_get_turbulence_ex(noise1d, POINT, 4, TCOD_NOISE_SIMPLEX);
  };
  BENCHMARK("Simplex turbulence 2D octaves=4") {
    (void)!TCOD_noise_get_turbulence_ex(noise2d, POINT, 4, TCOD_NOISE_SIMPLEX);
  };
  BENCHMARK("Simplex turbulence 3D octaves=4") {
    (void)!TCOD_noise_get_turbulence_ex(noise3d, POINT, 4, TCOD_NOISE_SIMPLEX);
  };
  BENCHMARK("Simplex turbulence 4D octaves=4") {
    (void)!TCOD_noise_get_turbulence_ex(noise4d, POINT, 4, TCOD_NOISE_SIMPLEX);
  };

  TCOD_noise_delete(noise4d);
  TCOD_noise_delete(noise3d);
  TCOD_noise_delete(noise2d);
  TCOD_noise_delete(noise1d);
  TCOD_random_delete(rng);
}
TEST_CASE("Noise Vectorized Benchmarks", "[.benchmark]") {
  TCOD_Random* rng = TCOD_random_new_from_seed(TCOD_RNG_MT, 0);
  TCOD_Noise* noise1d = TCOD_noise_new(1, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, rng);
  TCOD_Noise* noise2d = TCOD_noise_new(2, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, rng);
  TCOD_Noise* noise3d = TCOD_noise_new(3, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, rng);
  TCOD_Noise* noise4d = TCOD_noise_new(4, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, rng);
  constexpr int ARRAY_SIZE = 1024;
  {
    const float POINT[4] = {0.5f, 0.5f, 0.5f, 0.5f};
    (void)!TCOD_noise_get_ex(noise1d, POINT, TCOD_NOISE_WAVELET);  // Pre-generate wavelet data.
    (void)!TCOD_noise_get_ex(noise2d, POINT, TCOD_NOISE_WAVELET);
    (void)!TCOD_noise_get_ex(noise3d, POINT, TCOD_NOISE_WAVELET);
  }
  float x[ARRAY_SIZE];
  float y[ARRAY_SIZE];
  float z[ARRAY_SIZE];
  float w[ARRAY_SIZE];
  float out[ARRAY_SIZE];
  BENCHMARK("Perlin 2Dx256") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[2] = {i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_ex(noise2d, point, TCOD_NOISE_PERLIN);
    }
  };
  BENCHMARK("Perlin 2Dx256 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = i * 0.1f;
    }
    TCOD_noise_get_vectorized(noise2d, TCOD_NOISE_PERLIN, ARRAY_SIZE, x, y, NULL, NULL, out);
  };
  BENCHMARK("Perlin 3Dx256") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[3] = {i * 0.1f, i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_ex(noise3d, point, TCOD_NOISE_PERLIN);
    }
  };
  BENCHMARK("Perlin 3Dx256 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = z[i] = i * 0.1f;
    }
    TCOD_noise_get_vectorized(noise3d, TCOD_NOISE_PERLIN, ARRAY_SIZE, x, y, z, NULL, out);
  };
  BENCHMARK("Perlin 4Dx256") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[4] = {i * 0.1f, i * 0.1f, i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_ex(noise4d, point, TCOD_NOISE_PERLIN);
    }
  };
  BENCHMARK("Perlin 4Dx256 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = z[i] = w[i] = i * 0.1f;
    }
    TCOD_noise_get_vectorized(noise4d, TCOD_NOISE_PERLIN, ARRAY_SIZE, x, y, z, w, out);
  };
  BENCHMARK("Simplex 2Dx256") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[2] = {i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_ex(noise2d, point, TCOD_NOISE_SIMPLEX);
    }
  };
  BENCHMARK("Simplex 2Dx256 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = i * 0.1f;
    }
    TCOD_noise_get_vectorized(noise2d, TCOD_NOISE_SIMPLEX, ARRAY_SIZE, x, y, NULL, NULL, out);
  };
  BENCHMARK("Simplex 3Dx256") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[3] = {i * 0.1f, i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_ex(noise3d, point, TCOD_NOISE_SIMPLEX);
    }
  };
  BENCHMARK("Simplex 3Dx256 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = z[i] = i * 0.1f;
    }
    TCOD_noise_get_vectorized(noise3d, TCOD_NOISE_SIMPLEX, ARRAY_SIZE, x, y, z, NULL, out);
  };
  BENCHMARK("Simplex 4Dx256") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[4] = {i * 0.1f, i * 0.1f, i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_ex(noise4d, point, TCOD_NOISE_SIMPLEX);
    }
  };
  BENCHMARK("Simplex 4Dx256 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = z[i] = w[i] = i * 0.1f;
    }
    TCOD_noise_get_vectorized(noise4d, TCOD_NOISE_SIMPLEX, ARRAY_SIZE, x, y, z, w, out);
  };
  BENCHMARK("Wavelet 2Dx256") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[2] = {i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_ex(noise2d, point, TCOD_NOISE_WAVELET);
    }
  };
  BENCHMARK("Wavelet 2Dx256 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = i * 0.1f;
    }
    TCOD_noise_get_vectorized(noise2d, TCOD_NOISE_WAVELET, ARRAY_SIZE, x, y, NULL, NULL, out);
  };
  BENCHMARK("Wavelet 3Dx256") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[3] = {i * 0.1f, i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_ex(noise3d, point, TCOD_NOISE_WAVELET);
    }
  };
  BENCHMARK("Wavelet 3Dx256 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = z[i] = i * 0.1f;
    }
    TCOD_noise_get_vectorized(noise3d, TCOD_NOISE_WAVELET, ARRAY_SIZE, x, y, z, NULL, out);
  };
  BENCHMARK("Perlin fBm 2Dx256 octaves=4") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[2] = {i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_fbm_ex(noise2d, point, 4, TCOD_NOISE_PERLIN);
    }
  };
  BENCHMARK("Perlin fBm 2Dx256 octaves=4 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = i * 0.1f;
    }
    TCOD_noise_get_fbm_vectorized(noise2d, TCOD_NOISE_PERLIN, 4, ARRAY_SIZE, x, y, NULL, NULL, out);
  };
  BENCHMARK("Perlin turbulence 2Dx256 octaves=4") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      const float point[2] = {i * 0.1f, i * 0.1f};
      (void)!TCOD_noise_get_turbulence_ex(noise2d, point, 4, TCOD_NOISE_PERLIN);
    }
  };
  BENCHMARK("Perlin turbulence 2Dx256 octaves=4 vectorized") {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      x[i] = y[i] = i * 0.1f;
    }
    TCOD_noise_get_turbulence_vectorized(noise2d, TCOD_NOISE_PERLIN, 4, ARRAY_SIZE, x, y, NULL, NULL, out);
  };
  TCOD_noise_delete(noise4d);
  TCOD_noise_delete(noise3d);
  TCOD_noise_delete(noise2d);
  TCOD_noise_delete(noise1d);
  TCOD_random_delete(rng);
}

static tcod::MapPtr_ new_map_with_radius(int radius, bool start_transparent) {
  int size = radius * 2 + 1;
  tcod::MapPtr_ map{TCOD_map_new(size, size)};
  TCOD_map_clear(map.get(), start_transparent, 0);
  return map;
}
static tcod::MapPtr_ new_empty_map(int radius) { return new_map_with_radius(radius, true); }
static tcod::MapPtr_ new_opaque_map(int radius) { return new_map_with_radius(radius, false); }
static tcod::MapPtr_ new_corridor_map(int radius) {
  tcod::MapPtr_ map{new_map_with_radius(radius, false)};
  for (int i = 0; i < radius * 2 + 1; ++i) {
    TCOD_map_set_properties(map.get(), radius, i, true, true);
    TCOD_map_set_properties(map.get(), i, radius, true, true);
  }
  return map;
}
static tcod::MapPtr_ new_forest_map(int radius) {
  // Forest map with 1 in 4 chance of a blocking tile.
  std::mt19937 rng(0);
  std::uniform_int_distribution<int> chance(0, 3);
  tcod::MapPtr_ map{new_map_with_radius(radius, true)};
  for (int i = 0; i < map->nbcells; ++i) {
    if (chance(rng) == 0) {
      map->cells[i].transparent = false;
    }
  }
  return map;
}
TEST_CASE("FOV Benchmarks", "[.benchmark]") {
  std::array<std::tuple<std::string, tcod::MapPtr_>, 16> test_maps{{
      {"empty_r4", new_empty_map(4)},
      {"empty_r10", new_empty_map(10)},
      {"empty_r50", new_empty_map(50)},
      {"empty_r300", new_empty_map(300)},
      {"opaque_r4", new_opaque_map(4)},
      {"opaque_r10", new_opaque_map(10)},
      {"opaque_r50", new_opaque_map(50)},
      {"opaque_r300", new_opaque_map(300)},
      {"corridor_r4", new_corridor_map(4)},
      {"corridor_r10", new_corridor_map(10)},
      {"corridor_r50", new_corridor_map(50)},
      {"corridor_r300", new_corridor_map(300)},
      {"forest_r4", new_forest_map(4)},
      {"forest_r10", new_forest_map(10)},
      {"forest_r50", new_forest_map(50)},
      {"forest_r300", new_forest_map(300)},
  }};
  for (auto& active_test : test_maps) {
    const std::string& map_name = std::get<0>(active_test);
    TCOD_Map* map = std::get<1>(active_test).get();
    const int radius = TCOD_map_get_width(map) / 2;
    BENCHMARK(map_name + " FOV_BASIC") { (void)!TCOD_map_compute_fov(map, radius, radius, 0, true, FOV_BASIC); };
    BENCHMARK(map_name + " FOV_DIAMOND") { (void)!TCOD_map_compute_fov(map, radius, radius, 0, true, FOV_DIAMOND); };
    BENCHMARK(map_name + " FOV_SHADOW") { (void)!TCOD_map_compute_fov(map, radius, radius, 0, true, FOV_SHADOW); };
    BENCHMARK(map_name + " FOV_RESTRICTIVE") {
      (void)!TCOD_map_compute_fov(map, radius, radius, 0, true, FOV_RESTRICTIVE);
    };
    BENCHMARK(map_name + " FOV_PERMISSIVE_8") {
      (void)!TCOD_map_compute_fov(map, radius, radius, 0, true, FOV_PERMISSIVE_8);
    };
    BENCHMARK(map_name + " FOV_SYMMETRIC_SHADOWCAST") {
      (void)!TCOD_map_compute_fov(map, radius, radius, 0, true, FOV_SYMMETRIC_SHADOWCAST);
    };
  }
}
TEST_CASE("Color control.") {
  auto console = tcod::new_console(3, 1);
  TCODConsole::setColorControl(TCOD_COLCTRL_1, {1, 2, 3}, {4, 5, 6});
  TCODConsole::setColorControl(TCOD_COLCTRL_2, {7, 8, 9}, {10, 11, 12});
  TCOD_console_printf(console.get(), 0, 0, "%c1%c2%c3", TCOD_COLCTRL_1, TCOD_COLCTRL_2, TCOD_COLCTRL_STOP);
  REQUIRE(to_string(*console) == ("123"));
  // Because console->bkgnd_flag is at its default this will not affect the background colors.
  CHECK(console->at(0, 0).fg == TCOD_ColorRGBA{1, 2, 3, 255});
  CHECK(console->at(0, 0).bg == TCOD_ColorRGBA{0, 0, 0, 255});  // Remains unset, since bkgnd_flag was unset.
  CHECK(console->at(1, 0).fg == TCOD_ColorRGBA{7, 8, 9, 255});
  CHECK(console->at(1, 0).bg == TCOD_ColorRGBA{0, 0, 0, 255});
  CHECK(console->at(2, 0).fg == TCOD_ColorRGBA{255, 255, 255, 255});
  CHECK(console->at(2, 0).bg == TCOD_ColorRGBA{0, 0, 0, 255});

  console = tcod::new_console(3, 1);
  console->bkgnd_flag = TCOD_BKGND_SET;
  TCOD_console_printf(console.get(), 0, 0, "%c1%c2%c3", TCOD_COLCTRL_1, TCOD_COLCTRL_2, TCOD_COLCTRL_STOP);
  REQUIRE(to_string(*console) == ("123"));
  CHECK(console->at(0, 0).fg == TCOD_ColorRGBA{1, 2, 3, 255});
  CHECK(console->at(0, 0).bg == TCOD_ColorRGBA{4, 5, 6, 255});
  CHECK(console->at(1, 0).fg == TCOD_ColorRGBA{7, 8, 9, 255});
  CHECK(console->at(1, 0).bg == TCOD_ColorRGBA{10, 11, 12, 255});
  CHECK(console->at(2, 0).fg == TCOD_ColorRGBA{255, 255, 255, 255});
  CHECK(console->at(2, 0).bg == TCOD_ColorRGBA{0, 0, 0, 255});
}
