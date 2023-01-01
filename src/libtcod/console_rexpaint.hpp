/* BSD 3-Clause License
 *
 * Copyright © 2008-2023, Jice and the libtcod contributors.
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
#ifndef TCOD_CONSOLE_REXPAINT_HPP_
#define TCOD_CONSOLE_REXPAINT_HPP_
#ifndef TCOD_NO_ZLIB

#include <filesystem>
#include <vector>

#include "console_rexpaint.h"
#include "console_types.hpp"

namespace tcod {
/**
    @brief Load an array of consoles from a REXPaint file.

    @param path The path to the REXPaint file to load.
    @return Returns a vector of consoles.

    \rst
    .. versionadded:: 1.18
    \endrst
 */
inline std::vector<tcod::ConsolePtr> load_xp(const std::filesystem::path& path) {
  const auto path_str = path.string();
  int layer_count = tcod::check_throw_error(TCOD_load_xp(path_str.c_str(), 0, nullptr));
  std::vector<TCOD_Console*> tmp(layer_count, nullptr);
  tcod::check_throw_error(TCOD_load_xp(path_str.c_str(), layer_count, &tmp[0]));
  return std::vector<tcod::ConsolePtr>(tmp.begin(), tmp.end());
}
/**
    @brief Save an array of consoles to a REXPaint file.

    @param consoles A vector of consoles to save.
    @param path The path to write the REXPaint file to.
    @param compress_level A compression level for the zlib library.

    \rst
    .. versionadded:: 1.18
    \endrst
 */
inline void save_xp(
    const std::vector<const TCOD_Console*>& consoles, const std::filesystem::path& path, int compress_level = 9) {
  tcod::check_throw_error(
      TCOD_save_xp(static_cast<int>(consoles.size()), consoles.data(), path.string().c_str(), compress_level));
}
}  // namespace tcod
#endif  // TCOD_NO_ZLIB
#endif  // TCOD_CONSOLE_REXPAINT_HPP_
