#ifndef TCOD_NO_ZLIB
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <libtcod/zip.hpp>

TEST_CASE("TCODZip") {
  const auto temp_dir = std::filesystem::temp_directory_path();
  const auto zip_path = temp_dir / "saved.sav";
  {
    auto zip = TCODZip{};
    zip.put('c');
    zip.put(42);
    zip.put("Test");
    zip.put(std::optional<std::string>{});
    zip.put(tcod::ColorRGB{1, 2, 3});
    zip.put(TCODColor{4, 5, 6});
    zip.put(TCODImage{3, 2});
    zip.put(TCODRandom{42});
    zip.put(TCODConsole{3, 2});
    zip.put(tcod::Console{3, 2});

    zip.saveToFile(zip_path);
  }
  {
    auto zip = TCODZip{};
    zip.loadFromFile(zip_path);
    REQUIRE(zip.get<char>() == 'c');
    REQUIRE(zip.get<int>() == 42);
    REQUIRE(zip.get<std::string>() == "Test");
    REQUIRE_FALSE(zip.get<std::optional<std::string>>());
    REQUIRE(zip.get<tcod::ColorRGB>() == tcod::ColorRGB{1, 2, 3});
    REQUIRE(tcod::ColorRGB{zip.get<TCODColor>()} == tcod::ColorRGB{4, 5, 6});
    REQUIRE(zip.get<TCODImage>().getSize() == std::array{3, 2});
    zip.get<TCODRandom>();
    REQUIRE(zip.get<tcod::Console>().get_width() == 3);
    REQUIRE(zip.get<TCODConsole>().getWidth() == 3);
  }
}
#endif  // TCOD_NO_ZLIB
