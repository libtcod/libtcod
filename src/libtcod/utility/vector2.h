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
#ifndef LIBTCOD_UTILITY_VECTOR2_H_
#define LIBTCOD_UTILITY_VECTOR2_H_
#ifdef __cplusplus
#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <vector>
#endif // __cplusplus

#ifdef __cplusplus
namespace tcod {
template <typename T>
/**
 *  A 2-dimensional vector array.
 */
class Vector2 {
 public:
  using value_type = T;
  using size_type = ptrdiff_t;
  using iterator = typename std::vector<T>::iterator;
  using const_iterator = typename std::vector<T>::const_iterator;
  Vector2() = default;
  explicit Vector2(size_type width, size_type height, const T& fill)
  : width_(std::max<size_type>(0, width)),
    height_(std::max<size_type>(0, height)),
    vector_(width_ * height_, fill)
  {}
  explicit Vector2(size_type width, size_type height)
  : width_(std::max<size_type>(0, width)),
    height_(std::max<size_type>(0, height)),
    vector_(width_ * height_) {}
  explicit Vector2(
      const std::initializer_list<std::initializer_list<T>>& array)
  : Vector2(
      std::max(array,
               [](auto a, auto b){ return a.size() < b.size(); }).size(),
      array.size()
    )
  {
    auto iter_out = vector_.begin();
    for (const auto row : array) {
      std::copy(row.begin(), row.end(), iter_out);
      iter_out += width_;
    }
  }
  Vector2(Vector2&&) = default;
  Vector2& operator=(Vector2&&) = default;
  Vector2(const Vector2&) = default;
  Vector2& operator=(const Vector2&) = default;
  /** Return the width of this 2d vector. */
  size_type width(void) const noexcept { return width_; }
  /** Return the height of this 2d vector. */
  size_type height(void) const noexcept { return height_; }
  value_type& operator[](const std::array<size_type, 2>& index) noexcept
  {
    return vector_[index[0] * width_ + index[1]];
  }
  const value_type& operator[](const std::array<size_type, 2>& index) const noexcept
  {
    return vector_[index[0] * width_ + index[1]];
  }
  value_type& at(const std::array<size_type, 2>& index)
  {
    range_check(index[1], index[0]);
    return (*this)[index];
  }
  const value_type& at(const std::array<size_type, 2>& index) const
  {
    range_check(index[1], index[0]);
    return (*this)[index];
  }
  /**
   *  Return a reference for the pixel at `x`,`y`.
   *
   *  Throws std::out_of_range if `x` or `y` are out of bounds.
   */
  T& atf(size_type x, size_type y) {
    range_check(x, y);
    return vector_.at(y * width_ + x);
  }
  /**
   *  Return a reference for the value at `index`.
   */
  T& atf(const std::tuple<size_type, size_type>& index)
  {
    return atf(std::get<0>(index), std::get<1>(index));
  }
  T& atf(const std::array<size_type, 2>& index)
  {
    return atf(index.at(0), index.at(1));
  }
  /**
   *  Return a constant reference for the pixel at `x`,`y`.
   *
   *  Throws std::out_of_range if `x` or `y` are out of bounds.
   */
  const T& atf(size_type x, size_type y) const
  {
    range_check(x, y);
    return vector_.at(y * width_ + x);
  }
  const T& atf(const std::array<size_type, 2>& index) const
  {
    return atf(index.at(0), index.at(1));
  }
  /**
   *  Return a constant reference for the value at `index`.
   */
  const T& atf(const std::tuple<size_type, size_type>& index) const
  {
    return atf(std::get<0>(index), std::get<1>(index));
  }
  T* data() noexcept
  {
    return vector_.data();
  }
  const T* data() const noexcept
  {
    return vector_.data();
  }
  iterator begin() noexcept
  {
    return vector_.begin();
  }
  iterator end() noexcept
  {
    return vector_.end();
  }
  const_iterator begin() const noexcept
  {
    return vector_.begin();
  }
  const_iterator end() const noexcept
  {
    return vector_.end();
  }
  bool operator==(const Vector2& rhs) const noexcept
  {
    return width_ == rhs.width_ && vector_ == rhs.vector_;
  }
 private:
  /**
   *  Return true if x and y are in the bounds of this canvas.
   */
  bool in_boundsf(size_type x, size_type y) const noexcept {
    return 0 <= x && x < width_ && 0 <= y && y < height_;
  }
  /**
   *  Immediately throws std::out_of_range if `x` or `y` are out of bounds.
   */
  void range_check(size_type x, size_type y) const {
    if (!in_boundsf(x, y)) {
      throw std::out_of_range(
          std::string("Out of bounds lookup {")
          + std::to_string(x)
          + ", "
          + std::to_string(y)
          + "} on Vector2 of size {"
          + std::to_string(width_)
          + ", "
          + std::to_string(height_)
          + "}.");
    }
  }
  /** The width of this canvas. */
  size_type width_ = 0;
  /** The height of this canvas. */
  size_type height_ = 0;
  /** A 1d vector mapped to a 2d array of pixels. */
  std::vector<T> vector_;
};
template <typename T>
std::ostream& operator<< (std::ostream &out, const Vector2<T>& data)
{
  out << '{';
  for (int y = 0; y < data.height(); ++y) {
    out << '{';
    for (int x = 0; x < data.width(); ++x) {
      out << data.atf(x, y);
      if (x != data.width() - 1) { out << ',' << ' '; }
    }
    out << '}';
    if (y != data.height() - 1) { out << ',' << ' '; }
  }
  out << '}';
  return out;
}
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_UTILITY_VECTOR2_H_
