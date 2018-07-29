
#ifndef LIBTCOD_COLOR_CANVAS_H_
#define LIBTCOD_COLOR_CANVAS_H_
#include "color.h"
#ifdef __cplusplus
namespace tcod {
template <typename T=ColorRGBA>
class Canvas {
 public:
  Canvas() {}
  Canvas(int width, int height) {
    width_ = std::max(0, width);
    height_ = std::max(0, height);
    pixels_.resize(width_ * height_ * 4);
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
   *  Return T for the pixel at `x`,`y`.
   *
   *  Out-of-bounds coordinates will return `T{}`.
   */
  T GetPixel(int x, int y) const noexcept {
    if (IsInBounds(x, y)) {
      return pixels_[y * width_ + x];
    } else {
      return T{};
    }
  }
  /**
   *  Set the pixel at `x`,`y` to `rgba`.
   *
   *  Out-of-bounds coordinates are silently ignored.
   */
  void SetPixel(int x, int y, const T &rgba) noexcept {
    if (IsInBounds(x, y)) {
      pixels_[y * width_ + x] = rgba;
    }
  }
 private:
  /**
   *  Return true if x and y are in the bounds of this canvas.
   */
  bool IsInBounds(int x, int y) const noexcept {
    return 0 <= x && x < width_ && 0 <= y && y < height_;
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
