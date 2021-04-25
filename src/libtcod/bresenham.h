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

    This class is provisional.
 */
class BresenhamLine {
 public:
  using Point2 = std::array<int, 2>;

  struct iterator : public std::iterator<std::random_access_iterator_tag, Point2> {
    explicit iterator(const Point2& origin, const Point2& dest, int index) noexcept
        : origin_{origin}, dest_{dest}, state_{init_state(origin, dest)}, index_{index} {}

    inline iterator& operator++() noexcept {
      ++index_;
      return *this;
    }
    inline iterator operator++(int) noexcept {
      auto tmp = *this;
      ++(*this);
      return tmp;
    }
    inline iterator& operator--() noexcept {
      --index_;
      return *this;
    }
    inline iterator operator--(int) noexcept {
      auto tmp = *this;
      --(*this);
      return tmp;
    }
    inline value_type operator*() noexcept { return get_matrix().transform(state_[index_].cursor_); }
    inline value_type operator[](int index) noexcept { return get_matrix().transform(state_[index_ + index].cursor_); }
    inline constexpr bool operator==(const iterator& rhs) const noexcept { return index_ == rhs.index_; }
    inline constexpr bool operator!=(const iterator& rhs) const noexcept { return !(*this == rhs); }
    inline constexpr difference_type operator-(const iterator& rhs) const noexcept { return index_ - rhs.index_; }

   private:
    /**
        Internal state of the Bresenham line algorithm.
     */
    struct State {
      Point2 cursor_;  // Normalized cursor position. First axis acts as the current index.
      Point2 delta_;   // Normalized delta vector. First axis is always longer. All values are non-negative.
      int err_;        // Fractional difference between Y indexes.  Is always `-delta[0] < err <= 0`.
      /**
          Advance one step using the Bresenham algorithm.
       */
      State& operator++() {
        err_ += delta_.at(1);
        if (err_ > 0) {
          ++cursor_.at(1);
          err_ -= delta_.at(0);
        };
        ++cursor_.at(0);
        return *this;
      }
      /**
          Inverse Bresenham algorithm.  Takes one step backwards.
       */
      State& operator--() {
        err_ -= delta_.at(1);
        if (err_ <= -delta_.at(0)) {
          --cursor_.at(1);
          err_ += delta_.at(0);
        };
        --cursor_.at(0);
        return *this;
      }
      /**
          Seek to the given index.
       */
      State& operator[](int index) {
        while (cursor_.at(0) < index) ++(*this);
        while (cursor_.at(0) > index) --(*this);
        return *this;
      }
    };
    /**
        Transform matrix to convert from normalized state cursor to the real world coordinates.
     */
    struct Matrix {
      /**
          Convert a state cursor vector to the a world vector.
       */
      Point2 transform(const Point2& cursor) const noexcept {
        return {ax + cursor.at(0) * xx + cursor.at(1) * yx, ay + cursor.at(0) * xy + cursor.at(1) * yy};
      }
      int ax;          // Affine transformation on X.
      int ay;          // Affine transformation on Y.
      int_fast8_t xx;  // Index to world X.
      int_fast8_t xy;  // Index to world Y.
      int_fast8_t yx;  // Cursor Y to world X.
      int_fast8_t yy;  // Cursor Y to world Y.
    };
    /**
        Normalize the delta vector and return a new State.
     */
    static State init_state(const Point2& origin, const Point2& dest) noexcept {
      int delta_x = std::abs(dest.at(0) - origin.at(0));
      int delta_y = std::abs(dest.at(1) - origin.at(1));
      if (delta_y > delta_x) std::swap(delta_x, delta_y);
      return {{0, 0}, {delta_x, delta_y}, -delta_x / 2};
    }
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
    Point2 origin_;  // Starting point.
    Point2 dest_;    // Ending point.
    State state_;    // The current Bresenham state and real position.
    int index_;      // Current index.
  };
  /**
      Initializes the object to draw a line from `xFrom`, `yFrom` to `xTo`, `yTo`.
  */
  explicit BresenhamLine(Point2 from, Point2 to) noexcept
      : origin_(from), dest_(to), index_begin_{0}, index_end_{length(from, to)} {}

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
    new_data.index_begin_ += shift_begin;
    new_data.index_end_ += shift_end;
    new_data.index_end_ = std::max(new_data.index_begin_, new_data.index_end_);
    return new_data;
  }

  inline iterator begin() const noexcept { return iterator(origin_, dest_, index_begin_); }
  inline iterator end() const noexcept { return iterator(dest_, dest_, index_end_); }

 private:
  /**
      The total length of the Bresenham line including both endpoints.
   */
  inline int length(const Point2& origin, const Point2& dest) const noexcept {
    return std::max(std::abs(origin.at(0) - dest.at(0)), std::abs(origin.at(1) - dest.at(1))) + 1;
  }

  Point2 origin_;    // Starting point.
  Point2 dest_;      // Ending point.
  int index_begin_;  // The starting index returned by `begin`.
  int index_end_;    // The past-the-end index returned by `end`.
};

}  // namespace tcod
#endif  // __cplusplus
#endif  // _TCOD_BRESENHAM_H
