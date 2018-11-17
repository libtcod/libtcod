/* libtcod
 * Copyright © 2008-2018 Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The name of copyright holder nor the names of its contributors may not
 *       be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef LIBTCOD_UTILITY_VECTOR2_H_
#define LIBTCOD_UTILITY_VECTOR2_H_
#ifdef __cplusplus
#include <algorithm>
#include <stdexcept>
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
  Vector2() = default;
  explicit Vector2(int width, int height)
  : width_(std::max(0, width)),
    height_(std::max(0, height)),
    vector_(width_ * height_) {}
  Vector2(Vector2&&) = default;
  Vector2& operator=(Vector2&&) = default;
  Vector2(const Vector2&) = default;
  Vector2& operator=(const Vector2&) = default;
  /** Return the width of this 2d vector. */
  int width(void) const noexcept { return width_; }
  /** Return the height of this 2d vector. */
  int height(void) const noexcept { return height_; }
  /**
   *  Return a reference for the pixel at `x`,`y`.
   *
   *  Throws std::out_of_range if `x` or `y` are out of bounds.
   */
  T& at(int x, int y) {
    range_check(x, y);
    return vector_.at(y * width_ + x);
  }
  /**
   *  Return a constant reference for the pixel at `x`,`y`.
   *
   *  Throws std::out_of_range if `x` or `y` are out of bounds.
   */
  const T& at(int x, int y) const {
    range_check(x, y);
    return vector_.at(y * width_ + x);
  }
  T* data() noexcept
  {
    return vector_.data();
  }
  typename std::vector<T>::iterator begin()
  {
    return vector_.begin();
  }
  typename std::vector<T>::iterator end()
  {
    return vector_.end();
  }
  bool operator==(const Vector2& rhs) const noexcept
  {
    return vector_ == rhs.vector_;
  }
 private:
  /**
   *  Return true if x and y are in the bounds of this canvas.
   */
  bool in_bounds(int x, int y) const noexcept {
    return 0 <= x && x < width_ && 0 <= y && y < height_;
  }
  /**
   *  Immediately throws std::out_of_range if `x` or `y` are out of bounds.
   */
  void range_check(int x, int y) const {
    if (!in_bounds(x, y)) {
      throw std::out_of_range("Out of bounds lookup on Vector2.");
    }
  }
  /** The width of this canvas. */
  int width_ = 0;
  /** The height of this canvas. */
  int height_ = 0;
  /** A 1d vector mapped to a 2d array of pixels. */
  std::vector<T> vector_;
};
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_UTILITY_VECTOR2_H_
