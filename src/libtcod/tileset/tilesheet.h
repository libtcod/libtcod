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
#ifndef LIBTCOD_TILESET_TILESHEET_H_
#define LIBTCOD_TILESET_TILESHEET_H_
#ifdef __cplusplus
#include <memory>
#include <string>
#include <utility>
#endif // __cplusplus

#include "../color/canvas.h"
#include "tile.h"
#ifdef __cplusplus
namespace tcod {
namespace tileset {
using image::Image;
/**
 *  The layout of tiles on a tile-sheet.
 */
struct TilesheetLayout {
  int tile_width;
  int tile_height;
  int columns;
  int rows;
};
class Tilesheet {
 public:
  Tilesheet();
  explicit Tilesheet(const Image& canvas, const TilesheetLayout& layout);
  explicit Tilesheet(const Image& canvas, const std::pair<int, int>& layout);
  Tilesheet(Tilesheet&&) noexcept;
  Tilesheet& operator=(Tilesheet&&) noexcept;
  Tilesheet(const Tilesheet&);
  Tilesheet& operator=(const Tilesheet&);
  ~Tilesheet() noexcept;
  /**
   *  Return the tile at `x` and `y`.
   */
  Image get_tile(int x, int y) const;
  /**
   *  Return the tile at `n`.
   */
  Image get_tile(int n) const;
  int get_tile_width() const noexcept;
  int get_tile_height() const noexcept;
  int get_columns() const noexcept;
  int get_rows() const noexcept;
  int count() const noexcept;
 private:
  class impl;
  std::unique_ptr<impl> impl_;
};
/**
 *  Load a Tilesheet from an image file.
 */
Tilesheet LoadTilesheet(const std::string &filename);
} // namespace tileset
} // namespace tcod
#endif // __cplusplus
#endif /* LIBTCOD_TILESET_TILESHEET_H_ */
