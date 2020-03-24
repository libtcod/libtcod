
#include <climits>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>

#include <libtcod.h>
#include <libtcod.hpp>
#include <libtcod/pathfinding/astar.h>
#include <libtcod/pathfinding/breadth-first.h>
#include <libtcod/pathfinding/dijkstra.h>
#include <libtcod/pathfinding/hill-climb.h>
#include <libtcod/tileset_fallback.h>
#include <libtcod/utility/matrix.h>

#include "catch.hpp"

namespace std {
ostream& operator<<(ostream &out, const array<ptrdiff_t, 2>& data)
{
  return out << '{' << data.at(0) << ',' << ' ' << data.at(1) << '}';
}
} // namespace std

const int WIDTH = 20;
const int HEIGHT = 10;
const char *TITLE = "Unittest";

std::string get_file(const std::string& path) {
  static const char* DEFAULT_DIR = "data";
  const char* data_dir = std::getenv("DATA_DIR");
  if (!data_dir) { data_dir = DEFAULT_DIR; }
  return std::string(data_dir) + "/" + path;
}

void test_renderer(TCOD_renderer_t renderer)
{
  TCOD_console_set_custom_font(
      get_file("fonts/terminal8x8_gs_ro.png").c_str(),
      TCOD_FONT_LAYOUT_CP437 | TCOD_FONT_TYPE_GREYSCALE,
      0, 0);
  tcod::console::init_root(WIDTH, HEIGHT, TITLE, 0, renderer);
  REQUIRE(TCOD_console_get_width(NULL) == WIDTH);
  REQUIRE(TCOD_console_get_height(NULL) == HEIGHT);
  TCODConsole::flush();
  TCOD_console_delete(NULL);
}

TEST_CASE("SDL2 Renderer") {
  test_renderer(TCOD_RENDERER_SDL2);
}
TEST_CASE("SDL Renderer", "[!nonportable]") {
  test_renderer(TCOD_RENDERER_SDL);
}
TEST_CASE("OPENGL Renderer", "[!nonportable]") {
  test_renderer(TCOD_RENDERER_OPENGL);
}
TEST_CASE("GLSL Renderer", "[!nonportable]") {
  test_renderer(TCOD_RENDERER_SDL);
}
TEST_CASE("OPENGL2 Renderer", "[!nonportable]") {
  test_renderer(TCOD_RENDERER_OPENGL2);
}

TEST_CASE("Console ascii") {
  TCODConsole console = TCODConsole(5, 1);
  console.print(0, 0, "Test");
  for (int i = 0; i < 5; ++i) {
    CHECK(console.getChar(i, 0) == static_cast<int>("Test "[i]));
  }
}

TEST_CASE("Console print") {
  TCODConsole console = TCODConsole(5, 1);
  console.print(0, 0, std::string("plop"));
  for (int i = 0; i < 5; ++i) {
    CHECK(console.getChar(i, 0) == static_cast<int>("plop "[i]));
  }
}
TEST_CASE("Console print empty") {
  TCODConsole console = TCODConsole(5, 1);
  console.print(0, 0, std::string(""));
}

TEST_CASE("Console eascii") {
  TCODConsole console = TCODConsole(2, 1);
  const char test_str[] = { static_cast<char>(0xff), static_cast<char>(0x00) };
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

TEST_CASE("New Dijkstra")
{
  using Catch::Matchers::Equals;
  using index_type = std::array<ptrdiff_t, 2>;
  auto dist = tcod::Vector2<int>(3, 2, INT_MAX);
  auto path_map = tcod::Vector2<index_type>(3, 2);
  tcod::pathfinding::path_clear(path_map);
  dist.atf(0, 0) = 0;
  tcod::Vector2<int> cost{
      {1, 1, 1},
      {0, 1, 2},
  };
  tcod::pathfinding::dijkstra_compute(dist, cost, 2, 3, &path_map);
  const tcod::Vector2<int> EXPECTED_DIST{
      {0, 2, 4},
      {INT_MAX, 3, 7},
  };
  CHECK(dist == EXPECTED_DIST);
  const tcod::Vector2<index_type> EXPECTED_PATH_MAP{
      {{0, 0}, {0, 0}, {0, 1}},
      {{1, 0}, {0, 0}, {1, 1}},
  };
  CHECK(path_map == EXPECTED_PATH_MAP);
  const std::vector<index_type> EXPECTED_TRAVEL_PATH{{1, 1}, {0, 0}};
  CHECK_THAT(tcod::pathfinding::get_path(path_map, {1, 2}),
             Equals(EXPECTED_TRAVEL_PATH));
}
TEST_CASE("Pathfinder Benchmarks", "[benchmark]")
{
  const int SIZE = 1000;
  tcod::Matrix<uint8_t, 2> plain_cost({ SIZE, SIZE }, 1);
  tcod::pathfinding::SimpleGraph2D<decltype(plain_cost)> plain_graph(plain_cost);
  BENCHMARK("Old Dijkstra 1000x1000") {
    TCOD_Map* map = TCOD_map_new(SIZE, SIZE);
    TCOD_map_clear(map, 1, 1);
    TCOD_Dijkstra* dijkstra = TCOD_dijkstra_new(map, 1.5f);
    TCOD_dijkstra_compute(dijkstra, 0, 0);
    TCOD_dijkstra_delete(dijkstra);
    TCOD_map_delete(map);
  }
  BENCHMARK("Breadth-first 1000x1000") {
    tcod::Matrix<int, 2> map({ SIZE, SIZE }, std::numeric_limits<int>::max());
    tcod::Matrix<int8_t, 2> cost({ SIZE, SIZE }, 1);
    tcod::pathfinding::breadth_first2d(map, cost, { {0, 0} });
    CHECK(map.at({ SIZE - 1, SIZE - 1 }) == SIZE - 1);
  }
  BENCHMARK("Breadth-first (graph) 1000x1000") {
    tcod::Matrix<int, 2> map({ SIZE, SIZE }, std::numeric_limits<int>::max());
    tcod::pathfinding::breadth_first(map, plain_graph, { {0, 0} });
    CHECK(map.at({ SIZE - 1, SIZE - 1 }) == SIZE - 1);
    CHECK(tcod::pathfinding::simple_hillclimb(map, plain_graph, { SIZE - 1, SIZE - 1 }).size() == SIZE);
  }
  BENCHMARK("New Dijkstra 1000x1000") {
    tcod::Matrix<int, 2> map({ SIZE, SIZE }, std::numeric_limits<int>::max());
    tcod::Matrix<int8_t, 2> cost({ SIZE, SIZE }, 1);
    map.at({ 0, 0 }) = 0;
    tcod::pathfinding::dijkstra2d(map, cost);
    CHECK(map.at({ SIZE - 1, SIZE - 1 }) == SIZE - 1);
  }
  BENCHMARK("New Dijkstra (graph) 1000x1000") {
    tcod::Matrix<int, 2> map({ SIZE, SIZE }, std::numeric_limits<int>::max());
    map.at({ 0, 0 }) = 0;
    tcod::pathfinding::dijkstra(map, plain_graph);
    CHECK(map.at({ SIZE - 1, SIZE - 1 }) == SIZE - 1);
    CHECK(tcod::pathfinding::simple_hillclimb(map, plain_graph, { SIZE - 1, SIZE - 1 }).size() == SIZE);
  }
  BENCHMARK("Advanced Dijkstra 1000x1000") {
    using index_type = std::array<ptrdiff_t, 2>;
    auto dist = tcod::Vector2<int>(SIZE, SIZE, INT_MAX);
    auto path_map = tcod::Vector2<index_type>(SIZE, SIZE);
    tcod::pathfinding::path_clear(path_map);
    dist.at({ 0, 0 }) = 0;
    auto cost = tcod::Vector2<int>(SIZE, SIZE, 1);
    tcod::pathfinding::dijkstra_compute(dist, cost, 2, 3, &path_map);
  }
  BENCHMARK("Old A* 1000x1000") {
    TCOD_Map* map = TCOD_map_new(SIZE, SIZE);
    TCOD_map_clear(map, 1, 1);
    TCOD_Path* astar = TCOD_path_new_using_map(map, 1.0f);
    TCOD_path_compute(astar, 0, 0, SIZE - 1, SIZE - 1);
    TCOD_path_delete(astar);
    TCOD_map_delete(map);
  }
  BENCHMARK("New A* 1000x1000") {
    tcod::Matrix<int, 2> map({ SIZE, SIZE }, std::numeric_limits<int>::max());
    tcod::Matrix<int8_t, 2> cost({ SIZE, SIZE }, 1);
    map.at({ 0, 0 }) = 0;
    tcod::pathfinding::astar2d(map, cost, { SIZE - 1, SIZE - 1 });
    CHECK(map.at({ SIZE - 1, SIZE - 1 }) == SIZE - 1);
  }
  BENCHMARK("New A* (graph) 1000x1000") {
    tcod::Matrix<int, 2> map({ SIZE, SIZE }, std::numeric_limits<int>::max());
    map.at({ 0, 0 }) = 0;
    tcod::pathfinding::astar(map, plain_graph, { SIZE - 1, SIZE - 1 });
    CHECK(map.at({ SIZE - 1, SIZE - 1 }) == (SIZE - 1));
    CHECK(tcod::pathfinding::simple_hillclimb(map, plain_graph, { SIZE - 1, SIZE - 1 }).size() == SIZE);
  }
}

TEST_CASE("Fallback font.", "[!mayfail]")
{
  REQUIRE(tcod::tileset::new_fallback_tileset());
}

std::string to_string(const TCOD_Console& console)
{
  std::string result;
  for (const auto& tile : console) {
    result += static_cast<char>(tile.ch);
  }
  return result;
}

void test_alignment(TCOD_alignment_t alignment)
{
  // Compare alignment between the new and old functions.
  int x = GENERATE(0, 1, 2, 3, 4, 5, 6);
  int width = GENERATE(11, 12);
  INFO("x=" << x << ", width=" << width);
  TCOD_Console* console1 = TCOD_console_new(width, 1);
  TCOD_Console* console2 = TCOD_console_new(width, 1);
  SECTION("Print text.") {
    for (auto& tile : *console1) { tile.ch = static_cast<int>('.'); }
    for (auto& tile : *console2) { tile.ch = static_cast<int>('.'); }
    TCOD_console_print_ex(console1, x, 0, TCOD_BKGND_NONE, alignment, "123");
    TCOD_console_printf_ex(console2, x, 0, TCOD_BKGND_NONE, alignment, "123");
    CHECK(to_string(*console1) == to_string(*console2));
  }
  SECTION("Print rect.") {
  }
  TCOD_console_delete(console1);
  TCOD_console_delete(console2);
}
TEST_CASE("Left alignment regression.")
{
  test_alignment(TCOD_LEFT);
}
TEST_CASE("Center alignment regression.")
{
  test_alignment(TCOD_CENTER);
}
TEST_CASE("Right alignment regression.")
{
  test_alignment(TCOD_RIGHT);
}
TEST_CASE("Rectangle text alignment.")
{
  auto console = tcod::new_console(12, 1);
  //TCOD_Console* console = TCOD_console_new(12, 1);
  for (auto& tile : *console) { tile.ch = static_cast<int>('.'); }
  tcod::print_rect(
      *console, 0, 0, 0, 0, "123", nullptr, nullptr, TCOD_BKGND_NONE, TCOD_LEFT
  );
  tcod::print_rect(
      *console, 0, 0, 0, 0, "123", nullptr, nullptr, TCOD_BKGND_NONE, TCOD_CENTER
  );
  tcod::print_rect(
      *console, 0, 0, 0, 0, "123", nullptr, nullptr, TCOD_BKGND_NONE, TCOD_RIGHT
  );
  CHECK(to_string(*console) == "123.123..123");
}
TEST_CASE("Load BDF.", "[!nonportable]")
{
  REQUIRE(tcod::load_bdf(get_file("fonts/ucs-fonts/4x6.bdf")));
}
