/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "fallback.h"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "truetype.h"
#include "../engine/error.h"
namespace tcod {
namespace tileset {
#ifdef __unix__
/**
 *  Run and return the output from the given command.
 *
 *  https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-output-of-command-within-c-using-posix
 */
std::string exec_(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}
#endif
auto new_fallback_tileset(const std::array<int, 2>& tile_size)
-> std::unique_ptr<Tileset>
{
#if defined(_WIN32) // Windows.
  std::string font_path(std::getenv("SystemRoot"));
  font_path += "\\Fonts\\LUCON.TTF";
  return load_truetype(font_path, tile_size);
#elif defined(__APPLE__) // MacOS.
  return load_truetype("/System/Library/Fonts/SFCompactDisplay-Regular.otf",
                       tile_size);
#elif defined(__unix__) // Linux
  return load_truetype(
      exec_("fc-match --format=%{file} monospace"), tile_size
  );
#else
  throw std::runtime_error("Fallback font not supported for this OS.");
#endif
}
} // namespace tileset
} // namespace tcod
TCOD_Tileset* TCOD_tileset_new_fallback_font_(int tile_width, int tile_height)
{
  using tcod::tileset::Tileset;
  using tcod::tileset::new_fallback_tileset;
  try {
    return new std::shared_ptr<Tileset>(new_fallback_tileset({ tile_width , tile_height }));
  } catch (const std::exception& e) {
    tcod::set_error(e);
    return nullptr;
  }
}
