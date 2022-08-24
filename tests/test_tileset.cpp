#include <catch2/catch_all.hpp>
#include <libtcod/tileset.hpp>
#include <libtcod/tileset_bdf.hpp>

#include "common.hpp"

#ifndef TCOD_NO_PNG
TEST_CASE("Load tilesheet.") {
  auto tileset = tcod::load_tilesheet(get_file("fonts/terminal8x8_gs_ro.png"), {16, 16}, tcod::CHARMAP_CP437);
  REQUIRE(tileset.get());
  tileset = tcod::load_tilesheet(get_file("fonts/dejavu8x8_gs_tc.png"), {32, 8}, tcod::CHARMAP_TCOD);
  REQUIRE(tileset.get());
}

TEST_CASE("Missing tilesheet.", "[!throws]") {
  REQUIRE_THROWS(tcod::load_tilesheet("/nonexistant.file", {16, 16}, tcod::CHARMAP_CP437));
}
#endif  // TCOD_NO_PNG

TEST_CASE("Load BDF.") {
  auto tileset = tcod::load_bdf(get_file("fonts/ucs-fonts/4x6.bdf"));
  REQUIRE(tileset);
  tileset = tcod::load_bdf(get_file("fonts/Tamzen5x9r.bdf"));
  REQUIRE(tileset);
}
