#include <SDL.h>
#include <libtcod/timer.h>

#include <catch2/catch.hpp>

TEST_CASE("tcod::Timer") {
  REQUIRE(SDL_InitSubSystem(SDL_INIT_TIMER) == 0);
  auto timer = tcod::Timer();

  // Check that getting the FPS with no samples won't crash.
  REQUIRE(timer.get_last_fps() == 0);
  REQUIRE(timer.get_mean_fps() == 0);
  REQUIRE(timer.get_median_fps() == 0);
  REQUIRE(timer.get_min_fps() == 0);
  REQUIRE(timer.get_max_fps() == 0);

  timer.sync(1000);
  timer.sync(0);

  static_cast<void>(timer.get_last_fps());
  static_cast<void>(timer.get_mean_fps());
  static_cast<void>(timer.get_median_fps());
  static_cast<void>(timer.get_min_fps());
  static_cast<void>(timer.get_max_fps());

  SDL_QuitSubSystem(SDL_INIT_TIMER);
}
