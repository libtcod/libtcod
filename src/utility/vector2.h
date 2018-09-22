
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
  int width_;
  /** The height of this canvas. */
  int height_;
  /** A 1d vector mapped to a 2d array of pixels. */
  std::vector<T> vector_;
};
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_UTILITY_VECTOR2_H_
