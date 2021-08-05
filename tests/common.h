#pragma once

#include <libtcod/console.h>

#include <cstdlib>
#include <string>

static inline std::string get_file(const std::string& path) {
  static const char* DEFAULT_DIR = "data";
  const char* data_dir = std::getenv("DATA_DIR");
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
