
#ifndef LIBTCOD_COLOR_CANVAS_H_
#define LIBTCOD_COLOR_CANVAS_H_
#ifdef __cplusplus
#include <algorithm>
#include <stdexcept>
#include <vector>
#endif /* __cplusplus */

#include "color.h"
#ifdef __cplusplus
namespace tcod {
template <typename T=ColorRGBA>
class Canvas {
 public:
  Canvas() = default;
  explicit Canvas(int width, int height) {
    width_ = std::max(0, width);
    height_ = std::max(0, height);
    pixels_.resize(width_ * height_);
  }
  Canvas(Canvas&&) = default;
  Canvas& operator=(Canvas&&) = default;
  Canvas(const Canvas&) = default;
  Canvas& operator=(const Canvas&) = default;
  /** Return the width of this canvas. */
  int GetWidth(void) const noexcept { return width_; }
  /** Return the height of this canvas. */
  int GetHeight(void) const noexcept { return height_; }
  /**
   *  Return a reference for the pixel at `x`,`y`.
   *
   *  Throws std::out_of_range if `x` or `y` are out of bounds.
   */
  T& at(int x, int y) {
    RangeCheck(x, y);
    return pixels_.at(y * width_ + x);
  }
  /**
   *  Return a constant reference for the pixel at `x`,`y`.
   *
   *  Throws std::out_of_range if `x` or `y` are out of bounds.
   */
  const T& at(int x, int y) const {
    RangeCheck(x, y);
    return pixels_.at(y * width_ + x);
  }
 private:
  /**
   *  Return true if x and y are in the bounds of this canvas.
   */
  bool IsInBounds(int x, int y) const noexcept {
    return 0 <= x && x < width_ && 0 <= y && y < height_;
  }
  /**
   *  Immediately throws std::out_of_range if `x` or `y` are out of bounds.
   */
  void RangeCheck(int x, int y) const {
    if (!IsInBounds(x, y)) {
      throw std::out_of_range("Position on Canvas out of bounds.");
    }
  }
  /** The width of this canvas. */
  int width_;
  /** The height of this canvas. */
  int height_;
  /** A 2D array of pixels. */
  std::vector<T> pixels_;
};
} // namespace tcod
#endif /* __cplusplus */
#endif /* LIBTCOD_COLOR_CANVAS_H_ */
