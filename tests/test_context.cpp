#include <catch2/catch_all.hpp>
#include <cstddef>
#include <libtcod.hpp>
#include <utility>

#include "common.hpp"

#ifndef NO_SDL
const int WIDTH = 20;
const int HEIGHT = 10;
const char* TITLE = "Unittest";

auto new_test_tileset(int width, int height) -> tcod::TilesetPtr {
  auto tileset = tcod::TilesetPtr{TCOD_tileset_new(width, height)};
  std::vector<TCOD_ColorRGBA> buffer{(size_t)(width * height + 1)};
  for (int i = 0; i < 256; ++i) {
    TCOD_tileset_set_tile_(tileset.get(), i, buffer.data());
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
  auto console = tcod::Console{16, 12};
  TCOD_ContextParams params{};
  auto tileset = new_test_tileset(25, 24);
  params.tileset = tileset.get();
  params.renderer_type = renderer;
  params.console = console.get();

  auto context = tcod::Context(params);
  context.present(console);

  context.set_mouse_transform(TCOD_MouseTransform{0, 0, 1, 1});
#if 0
  // Check for division by zero errors:
  TCOD_Tileset* tileset = new_test_tileset(0, 0);
  context.c_set_tileset_(context.get(), tileset);
  context.present(*console);
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
#endif  // NO_SDL
