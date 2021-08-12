/* BSD 3-Clause License
 *
 * Copyright © 2008-2021, Jice and the libtcod contributors.
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
#ifndef _TCOD_BRESENHAM_H
#define _TCOD_BRESENHAM_H

#ifdef __cplusplus
#include <array>
#include <functional>
#include <iterator>
#include <vector>
#endif  // __cplusplus

#include "portability.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 *  \brief A callback to be passed to TCOD_line
 *
 *  The points given to the callback include both the starting and ending
 *  positions.
 *
 *  \param x
 *  \param y
 *  \return As long as this callback returns true it will be called with the
 *          next x,y point on the line.
 */
typedef bool (*TCOD_line_listener_t)(int x, int y);

TCODLIB_API TCOD_DEPRECATED("This function is not reentrant. Use TCOD_line_init_mt instead.") void TCOD_line_init(
    int xFrom, int yFrom, int xTo, int yTo);

/** advance one step. returns true if we reach destination */
TCODLIB_API TCOD_DEPRECATED("This function is not reentrant.") bool TCOD_line_step(
    int* __restrict xCur, int* __restrict yCur);

/* atomic callback function. Stops when the callback returns false */
TCODLIB_API bool TCOD_line(int xFrom, int yFrom, int xTo, int yTo, TCOD_line_listener_t listener);
/**
 *  \brief A struct used for computing a bresenham line.
 */
typedef struct {
  int stepx;
  int stepy;
  int e;
  int deltax;
  int deltay;
  int origx;
  int origy;
  int destx;
  int desty;
} TCOD_bresenham_data_t;

TCODLIB_API void TCOD_line_init_mt(int xFrom, int yFrom, int xTo, int yTo, TCOD_bresenham_data_t* data);
TCODLIB_API bool TCOD_line_step_mt(int* __restrict xCur, int* __restrict yCur, TCOD_bresenham_data_t* __restrict data);

TCOD_DEPRECATED("Use TCOD_line instead.")
TCODLIB_API bool TCOD_line_mt(
    int xFrom, int yFrom, int xTo, int yTo, TCOD_line_listener_t listener, TCOD_bresenham_data_t* data);

#ifdef __cplusplus
}  // extern "C"
namespace tcod {
/**
    Encapsulates a Bresenham line drawing algorithm.

    \rst
    .. versionadded:: 1.17
    \endrst
 */
class BresenhamLine {
 public:
  using Point2 = std::array<int, 2>;
  using iterator_category = std::random_access_iterator_tag;
  using value_type = Point2;
  using difference_type = int;
  using pointer = void;
  using reference = value_type;
  /**
      Construct a new Bresenham line from `begin` to `end`.

      Iterating over this instance will include both endpoints.
   */
  explicit BresenhamLine(Point2 begin, Point2 end) noexcept
      : origin_{begin},
        dest_{end},
        index_{0},
        index_end_{get_delta_x() + 1},
        cursor_{0, 0},
        y_error_{-get_delta_x() / 2} {}
  /**
      Construct a new Bresenham line with a manually given error value.
   */
  explicit BresenhamLine(Point2 begin, Point2 end, int error) noexcept
      : origin_{begin},
        dest_{end},
        index_{0},
        index_end_{get_delta_x() + 1},
        cursor_{0, 0},
        y_error_{error > 0 ? error % get_delta_x() - get_delta_x() : error % get_delta_x()} {}

  inline BresenhamLine& operator++() noexcept {
    ++index_;
    return *this;
  }
  inline BresenhamLine operator++(int) noexcept {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }
  inline BresenhamLine& operator--() noexcept {
    --index_;
    return *this;
  }
  inline BresenhamLine operator--(int) noexcept {
    auto tmp = *this;
    --(*this);
    return tmp;
  }
  /**
      Return the world position of the Bresenham at the index relative to the current index.

      BresenhamLine is not restricted by any bounds so you can freely give a index past the end or before zero.

      The internal state must always seek to the position being indexed, this will affect performance depending on if
      successive indexes are close together or far apart.
   */
  inline value_type operator[](int index) noexcept { return bresenham_get(index_ + index); }
  /**
      Return the world position of the Bresenham at the current index.
   */
  inline value_type operator*() noexcept { return (*this)[0]; }
  inline constexpr bool operator==(const BresenhamLine& rhs) const noexcept { return index_ == rhs.index_; }
  inline constexpr bool operator!=(const BresenhamLine& rhs) const noexcept { return !(*this == rhs); }
  inline constexpr difference_type operator-(const BresenhamLine& rhs) const noexcept { return index_ - rhs.index_; }

  /**
      Return a new version of this BresenhamLine with an adjusted range.

      `shift_begin` and `shift_end` change the beginning and ending of the line
      when iterators over.

      Example::

        // Remove the endpoints of a bresenham line.
        auto line = tcod::BresenhamLine(from, to).adjust_range(1, -1);
   */
  inline BresenhamLine adjust_range(int shift_begin, int shift_end) const noexcept {
    BresenhamLine new_data{*this};
    new_data.index_ += shift_begin;
    new_data.index_end_ += shift_end;
    new_data.index_end_ = std::max(new_data.index_, new_data.index_end_);
    return new_data;
  }
  /**
      Remove the staring endpoint of a line.

      Example::

        for (auto&& [x, y] : tcod::BresenhamLine(from, to).without_start()) {
          // All positions excluding `from`.
        }
   */
  inline BresenhamLine without_start() const noexcept { return adjust_range(1, 0); }
  /**
      Remove the final endpoint of a line.

      Example::

        for (auto&& [x, y] : tcod::BresenhamLine(from, to).without_end()) {
          // All positions excluding `to`.
        }
   */
  inline BresenhamLine without_end() const noexcept { return adjust_range(0, -1); }
  /**
      Remove both endpoints of a line.

      Example::

        for (auto&& [x, y] : tcod::BresenhamLine(from, to).without_endpoints()) {
          // All positions between and excluding `from` and `to`.
        }
   */
  inline BresenhamLine without_endpoints() const noexcept { return adjust_range(1, -1); }
  /**
      Return the beginning iterator, which is a copy of the current object.
   */
  inline BresenhamLine begin() const noexcept { return {*this}; }
  /**
      Return the past-the-end iterator.
   */
  inline BresenhamLine end() const noexcept {
    return BresenhamLine{origin_, dest_, index_end_, index_end_, cursor_, y_error_};
  }

 private:
  /**
      Transform matrix to convert from normalized state cursor to the real world coordinates.
   */
  struct Matrix {
    /**
        Convert a state cursor vector to the a world vector.
     */
    inline Point2 transform(const Point2& cursor) const noexcept {
      return {ax + cursor.at(0) * xx + cursor.at(1) * yx, ay + cursor.at(0) * xy + cursor.at(1) * yy};
    }
    int ax;  // Affine transformation on X.
    int ay;  // Affine transformation on Y.
    int_fast8_t xx;  // Index to world X.
    int_fast8_t xy;  // Index to world Y.
    int_fast8_t yx;  // Cursor Y to world X.
    int_fast8_t yy;  // Cursor Y to world Y.
  };
  /**
      Return a Matrix that converts a normalized cursor to the correct octant.
   */
  inline Matrix get_matrix() const noexcept { return get_matrix(origin_, dest_); }
  static Matrix get_matrix(const Point2& origin, const Point2& dest) noexcept {
    const int delta_x = dest.at(0) - origin.at(0);
    const int delta_y = dest.at(1) - origin.at(1);
    Matrix matrix{
        origin.at(0),
        origin.at(1),
        1,
        0,
        0,
        1,
    };
    if (delta_x < 0) matrix.xx = -1;
    if (delta_y < 0) matrix.yy = -1;
    if (std::abs(delta_y) > std::abs(delta_x)) {
      std::swap(matrix.xx, matrix.yx);
      std::swap(matrix.xy, matrix.yy);
    }
    return matrix;
  }
  explicit BresenhamLine(Point2 begin, Point2 end, int index_begin, int index_end, Point2 cursor, int error) noexcept
      : origin_{begin}, dest_{end}, index_{index_begin}, index_end_{index_end}, cursor_{cursor}, y_error_{error} {}
  /**
      Return the normalized delta vector.

      The first axis is always the longest. All values are non-negative.
   */
  inline Point2 get_normalized_delta() const noexcept { return get_normalized_delta(origin_, dest_); }
  static Point2 get_normalized_delta(const Point2& origin, const Point2& dest) noexcept {
    return std::abs(dest.at(0) - origin.at(0)) > std::abs(dest.at(1) - origin.at(1))
               ? Point2{std::abs(dest.at(0) - origin.at(0)), std::abs(dest.at(1) - origin.at(1))}
               : Point2{std::abs(dest.at(1) - origin.at(1)), std::abs(dest.at(0) - origin.at(0))};
  }
  /**
      Return the normalized delta X value.

      This is the value of the longest delta axis as a positive integer and is often used to determine the line length.
   */
  inline int get_delta_x() const noexcept { return get_normalized_delta().at(0); }
  /**
      Advance one step using the Bresenham algorithm.
   */
  inline BresenhamLine& bresenham_next() {
    const Point2 delta = get_normalized_delta();
    y_error_ += delta.at(1);
    if (y_error_ > 0) {
      ++cursor_.at(1);
      y_error_ -= delta.at(0);
    };
    ++cursor_.at(0);
    return *this;
  }
  /**
      Inverse Bresenham algorithm.  Takes one step backwards.
   */
  inline BresenhamLine& bresenham_prev() {
    const Point2 delta = get_normalized_delta();
    y_error_ -= delta.at(1);
    if (y_error_ <= -delta.at(0)) {
      --cursor_.at(1);
      y_error_ += delta.at(0);
    };
    --cursor_.at(0);
    return *this;
  }
  /**
      Seek to the given index and return the world position of the cursor.
   */
  inline Point2 bresenham_get(int index) {
    while (cursor_.at(0) < index) bresenham_next();
    while (cursor_.at(0) > index) bresenham_prev();
    return get_matrix().transform(cursor_);
  }
  Point2 origin_;  // Starting point.
  Point2 dest_;  // Ending point.
  int index_;  // The starting index returned by `begin`.
  int index_end_;  // The past-the-end index returned by `end`.
  Point2 cursor_;  // Normalized Bresenham low-slope position.  First axis acts as the current index.
  int y_error_;  // Fractional difference between Y indexes.  Is always `-delta[0] < err <= 0`.
};

}  // namespace tcod
#endif  // __cplusplus
#endif  // _TCOD_BRESENHAM_H
