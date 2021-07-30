#pragma once

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
