#pragma once

#include <cstdlib>
#include <libtcod/console.hpp>
#include <string>

#ifdef _MSC_VER
#define KNOWN_DEPRECATION _Pragma("warning(suppress : 4996)")
#else
#define KNOWN_DEPRECATION
#endif  // _MSC_VER

/***************************************************************************
    @brief Return the path to a test data file.

    @param path A relative path to the test data.
    @return std::string
 */
static inline std::string get_file(const std::string& path) {
  static const char* DEFAULT_DIR = "data";
  KNOWN_DEPRECATION const char* data_dir = std::getenv("DATA_DIR");  // Ignore MSVC warning.
  if (!data_dir) {
    data_dir = DEFAULT_DIR;
  }
  return std::string(data_dir) + "/" + path;
}
/*****************************************************************************
    @brief Convert a console to a multi-line string, used for tests.
 */
static inline std::string to_string(const TCOD_Console& console) {
  std::string result;
  for (int y = 0; y < console.h; ++y) {
    if (y != 0) {
      result += '\n';
    }
    for (int x = 0; x < console.w; ++x) {
      result += static_cast<char>(console.at(x, y).ch);
    }
  }
  return result;
}
/***************************************************************************
    @brief Convert a Unicode codepoint to a UTF-8 multi-byte string.
 */
static inline auto ucs4_to_utf8(int ucs4) -> std::string {
  char utf8[4];
  if (ucs4 < 0) {
    throw std::invalid_argument("Invalid codepoint.");
  } else if (ucs4 <= 0x7F) {
    utf8[0] = static_cast<uint8_t>(ucs4);
    return std::string(utf8, 1);
  } else if (ucs4 <= 0x07FF) {
    utf8[0] = 0b11000000 | static_cast<uint8_t>((ucs4 & 0b11111'000000) >> 6);
    utf8[1] = 0b10000000 | static_cast<uint8_t>((ucs4 & 0b00000'111111) >> 0);
    return std::string(utf8, 2);
  } else if (ucs4 <= 0xFFFF) {
    utf8[0] = 0b11100000 | static_cast<uint8_t>((ucs4 & 0b1111'000000'000000) >> 12);
    utf8[1] = 0b10000000 | static_cast<uint8_t>((ucs4 & 0b0000'111111'000000) >> 6);
    utf8[2] = 0b10000000 | static_cast<uint8_t>((ucs4 & 0b0000'000000'111111) >> 0);
    return std::string(utf8, 3);
  } else if (ucs4 <= 0x10FFFF) {
    utf8[0] = 0b11110000 | static_cast<uint8_t>((ucs4 & 0b111'000000'000000'000000) >> 18);
    utf8[1] = 0b10000000 | static_cast<uint8_t>((ucs4 & 0b000'111111'000000'000000) >> 12);
    utf8[2] = 0b10000000 | static_cast<uint8_t>((ucs4 & 0b000'000000'111111'000000) >> 6);
    utf8[3] = 0b10000000 | static_cast<uint8_t>((ucs4 & 0b000'000000'000000'111111) >> 0);
    return std::string(utf8, 4);
  }
  throw std::invalid_argument("Invalid codepoint.");
}
