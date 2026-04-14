#ifndef TCOD_NO_ZLIB
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <fstream>
#include <libtcod/zip.hpp>

static constexpr auto SAVE_FILE_NAME = "libtcod_unittest_file.sav";

TEST_CASE("TCODZip") {
  const auto temp_dir = std::filesystem::temp_directory_path();
  const auto zip_path = temp_dir / SAVE_FILE_NAME;
  {
    auto zip = TCODZip{};
    zip.put('c');
    zip.put(42);
    zip.put("foobar");
    zip.put(-42);
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
    REQUIRE(zip.get<std::string>() == "foobar");
    REQUIRE(zip.get<int>() == -42);
    REQUIRE_FALSE(zip.get<std::optional<std::string>>());
    REQUIRE(zip.get<tcod::ColorRGB>() == tcod::ColorRGB{1, 2, 3});
    REQUIRE(tcod::ColorRGB{zip.get<TCODColor>()} == tcod::ColorRGB{4, 5, 6});
    REQUIRE(zip.get<TCODImage>().getSize() == std::array{3, 2});
    zip.get<TCODRandom>();
    REQUIRE(zip.get<tcod::Console>().get_width() == 3);
    REQUIRE(zip.get<TCODConsole>().getWidth() == 3);
  }
}

TEST_CASE("TCODZip 32bit") {
  // Load data saved in x86, test might be ineffective due to aligned data
  const auto temp_dir = std::filesystem::temp_directory_path();
  const auto zip_path = temp_dir / SAVE_FILE_NAME;
  {
    // (16, 42, 3, b'f', b'o', b'o', b'\x00', -42)
    static const auto COMPRESSED_DATA = std::basic_string<uint8_t>{
        31, 139, 8,   0,   0,   0,   0,   0,   0,   10, 19,  96,  96, 96,  208, 2, 98, 102, 32,
        78, 203, 207, 103, 184, 246, 255, 255, 127, 0,  225, 203, 88, 234, 20,  0, 0,  0};
    auto file = std::ofstream(zip_path, std::ios::binary);
    for (const auto it : COMPRESSED_DATA) file << it;
  }
  {
    auto zip = TCODZip{};
    zip.loadFromFile(zip_path);
    REQUIRE(zip.get<int>() == 42);
    REQUIRE(zip.get<std::string>() == "foo");
    REQUIRE(zip.get<int>() == -42);
  }
}

TEST_CASE("TCODZip 64bit") {
  // Load data saved in x64, test might be ineffective due to aligned data
  const auto temp_dir = std::filesystem::temp_directory_path();
  const auto zip_path = temp_dir / SAVE_FILE_NAME;
  {
    // (19, 42, 6, b'f', b'o', b'o', b'b', b'a', b'r', b'\x00', -42)
    static const auto COMPRESSED_DATA = std::basic_string<uint8_t>{
        31,  139, 8,  0,  0,  0,  0,   0,   0,   10,  19,  102, 96, 96,  208, 2,   98, 54, 32, 78,
        203, 207, 79, 74, 44, 98, 184, 246, 255, 255, 127, 0,   94, 227, 111, 157, 23, 0,  0,  0};
    auto file = std::ofstream(zip_path, std::ios::binary);
    for (const auto it : COMPRESSED_DATA) file << it;
  }
  {
    auto zip = TCODZip{};
    zip.loadFromFile(zip_path);
    REQUIRE(zip.get<int>() == 42);
    REQUIRE(zip.get<std::string>() == "foobar");
    REQUIRE(zip.get<int>() == -42);
  }
}
#endif  // TCOD_NO_ZLIB
