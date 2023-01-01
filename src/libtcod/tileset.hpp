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
#ifndef LIBTCOD_TILESET_HPP_
#define LIBTCOD_TILESET_HPP_
#include <array>
#include <filesystem>
#include <memory>

#include "error.hpp"
#include "tileset.h"

namespace tcod {
/**
    A character mapping of a Code Page 437 tileset to Unicode.
    \rst
    .. versionadded:: 1.19
    \endrst
 */
static constexpr std::array<int, 256> CHARMAP_CP437 = TCOD_CHARMAP_CP437_;
/**
    A character mapping of a deprecated TCOD tileset to Unicode.
    \rst
    .. versionadded:: 1.19
    \endrst
 */
static constexpr std::array<int, 256> CHARMAP_TCOD = TCOD_CHARMAP_TCOD_;

struct TilesetDeleter {
  void operator()(TCOD_Tileset* tileset) const { TCOD_tileset_delete(tileset); }
};
/**
    @brief A unique pointer to a TCOD_Tileset.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
typedef std::unique_ptr<TCOD_Tileset, TilesetDeleter> TilesetPtr;
/***************************************************************************
    @brief A C++ Tileset container.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
class Tileset {
 public:
  /***************************************************************************
      @brief Construct a new Tileset object.
   */
  Tileset() = default;
  /***************************************************************************
      @brief Construct a new Tileset object with tiles of the given size.  The tileset will be empty.

      @param tile_width The width of the tiles of this object in pixels.
      @param tile_height The width of the tiles of this object in pixels.
   */
  explicit Tileset(int tile_width, int tile_height) : tileset_{TCOD_tileset_new(tile_width, tile_height)} {
    if (!tileset_) throw std::runtime_error(TCOD_get_error());
  }
  /***************************************************************************
      @brief Construct a new Tileset object with tiles of the given size.  The tileset will be empty.

      @param tile_shape The `{width, height}` of the tiles in pixels.
   */
  explicit Tileset(const std::array<int, 2>& tile_shape) : Tileset{tile_shape.at(0), tile_shape.at(1)} {}
  /***************************************************************************
      @brief Pass ownership of a TilesetPtr to a new Tileset.

      @param ptr A `tcod::TilesetPtr`, must not be nullptr.
   */
  explicit Tileset(TilesetPtr ptr) : tileset_{std::move(ptr)} {
    if (!tileset_) throw std::invalid_argument("Pointer must not be nullptr.");
  }
  /***************************************************************************
      @brief Takes ownership of a raw TCOD_Tileset pointer.

      @param ptr A pointer which will now be managed by this object.
   */
  explicit Tileset(TCOD_Tileset* ptr) : tileset_{ptr} {
    if (!tileset_) throw std::invalid_argument("Pointer must not be nullptr.");
  }
  /***************************************************************************
      @brief Get the width of tiles in this Tileset.

      @return int The total width of tiles in pixels.
   */
  [[nodiscard]] auto get_tile_width() const noexcept -> int { return tileset_->tile_width; }
  /***************************************************************************
      @brief Get the height of tiles in this Tileset.

      @return int The total height of tiles in pixels.
   */
  [[nodiscard]] auto get_tile_height() const noexcept -> int { return tileset_->tile_height; }
  /***************************************************************************
      @brief Get the `{width, height}` shape of tiles in this Tileset.

      @return std::array<int, 2> The `{width, height}` of tiles in this Tileset in pixels.
   */
  [[nodiscard]] auto get_tile_shape() const noexcept -> std::array<int, 2> {
    return {tileset_->tile_width, tileset_->tile_height};
  }
  /***************************************************************************
      @brief Return a non-owning pointer to this objects TCOD_Tileset.

      @return TCOD_Tileset
   */
  [[nodiscard]] auto get() noexcept -> TCOD_Tileset* { return tileset_.get(); }
  /***************************************************************************
      @brief Return a non-owning pointer to this objects TCOD_Tileset.

      @return TCOD_Tileset
   */
  [[nodiscard]] auto get() const noexcept -> TCOD_Tileset* { return tileset_.get(); }
  /***************************************************************************
      @brief Release ownership of this Tileset's `TCOD_Tileset*` and return the pointer.

      Using this Tileset afterwards is undefined.
   */
  auto release() noexcept -> TCOD_Tileset* { return tileset_.release(); }
  /***************************************************************************
      @brief Allow implicit conversions to a TCOD_Console reference.
   */
  [[nodiscard]] operator TCOD_Tileset&() { return *tileset_; }
  /***************************************************************************
      @brief Allow implicit conversions to a const TCOD_Console reference.
   */
  [[nodiscard]] operator const TCOD_Tileset&() const { return *tileset_; }

 private:
  TilesetPtr tileset_ = nullptr;
};
#ifndef TCOD_NO_PNG
/**
    @brief Load a tilesheet from a PNG file.

    An exception will be thrown if the file is missing or corrupt.

    Tiles are indexed in row-major order and should be assigned to Unicode codepoints.

    \rst
    .. versionadded:: 1.19
    \endrst

    @tparam ArrayType Must be a `std::vector` or `std::array` like type.  With `size()` and `data()` methods.
    @param path The file path to the PNG tilesheet image.
    @param columns_rows The shape of the grid on the tileset as {columns, rows}.
    @param charmap
        An array of characters where `charmap[tile_index] = codepoint`.
        `tcod::CHARMAP_CP437` or `tcod::CHARMAP_TCOD` are typical values for this argument.

    @return TilesetPtr A unique pointer to a `TCOD_Tileset`.
 */
template <typename ArrayType>
TCOD_NODISCARD inline auto load_tilesheet(
    const std::filesystem::path& path, const std::array<int, 2>& columns_rows, const ArrayType& charmap) -> Tileset {
  tcod::check_path(path);
  TilesetPtr tileset{TCOD_tileset_load(
      path.string().c_str(), columns_rows.at(0), columns_rows.at(1), static_cast<int>(charmap.size()), charmap.data())};
  if (!tileset) throw std::runtime_error(TCOD_get_error());
  return Tileset{std::move(tileset)};
}
#endif  // TCOD_NO_PNG
}  // namespace tcod
#endif  // LIBTCOD_TILESET_HPP_
