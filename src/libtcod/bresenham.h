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
class TCODLIB_API BresenhamLine {
  using cell = std::pair<int, int>;

 public:
  /**
      Initializes the object to draw a line from `xFrom`, `yFrom` to `xTo`, `yTo`.
  */
  BresenhamLine(int xFrom, int yFrom, int xTo, int yTo) { TCOD_line_init_mt(xFrom, yFrom, xTo, yTo, &data_); }
  BresenhamLine(cell from, cell to) { TCOD_line_init_mt(from.first, from.second, to.first, to.second, &data_); }

  /**
      Steps through each cell from the start to the end coordinates.

      The input variables `x`, `y` are passed in by reference and updated at each step of the line.
  */
  inline bool step(int& x, int& y) { return TCOD_line_step_mt(&x, &y, &data_); }

  struct iterator : public std::iterator<std::input_iterator_tag, cell>
  {
    iterator(value_type cur, TCOD_bresenham_data_t& data) : cur_(cur), data_(data)  {}

    inline reference operator*() { return cur_; }
    inline iterator& operator++() { TCOD_line_step_mt(&cur_.first, &cur_.second, &data_); return *this; }

    inline bool operator==(const iterator& rhs) const { return cur_ == rhs.cur_; }
    inline bool operator!=(const iterator& rhs) const { return cur_ != rhs.cur_; }

    private:
      value_type cur_;
      TCOD_bresenham_data_t& data_;
  };

  iterator begin() { return iterator({data_.origx, data_.origy}, data_); }
  iterator end() { return iterator({data_.destx, data_.desty}, data_); }

 private:
  TCOD_bresenham_data_t data_{};
};

/**
    Draw a Bresenham line, passing the indexes of the line to `callback`.

    This function is provisional.
 */
inline bool bresenham_line(std::pair<int, int> from, std::pair<int, int> to, const std::function<bool(std::pair<int, int>)>& callback) {
  auto line = BresenhamLine(from, to);
  do {
    if (!callback(from)) {
      return false;
    }
  } while (!line.step(from.first, from.second));
  return true;
}

inline bool bresenham_line(int xFrom, int yFrom, int xTo, int yTo, const std::function<bool(std::pair<int, int>)>& callback) {
  return bresenham_line({xFrom, yFrom}, {xTo, yTo}, callback);
}

}  // namespace tcod
#endif  // __cplusplus
#endif  // _TCOD_BRESENHAM_H
