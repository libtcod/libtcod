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
#ifndef TCOD_GUI_SLIDER_HPP
#define TCOD_GUI_SLIDER_HPP
#ifndef TCOD_NO_UNICODE
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <string>

#include "textbox.hpp"

namespace tcod::gui {
class Slider : public TextBox {
 public:
  Slider(int x, int y, int w, float min, float max, const char* label, const char* tip = nullptr)
      : TextBox{x, y, w, 10, label, nullptr, tip}, min{min}, max{max}, value{(min + max) * 0.5f} {
    valueToText();
    this->w += 2;
  }
  void render() override {
    w -= 2;
    TextBox::render();
    w += 2;
    const auto fg = TCOD_ColorRGBA((onArrows || drag) ? foreFocus : fore);
    const auto bg = TCOD_ColorRGBA((onArrows || drag) ? backFocus : back);
    auto& console = static_cast<TCOD_Console&>(*con);
    if (console.in_bounds({x + w - 2, y})) {
      console.at({x + w - 2, y}) = {0x2190, fg, bg};  // ←
    }
    if (console.in_bounds({x + w - 1, y})) {
      console.at({x + w - 1, y}) = {0x2192, fg, bg};  // →
    }
  }
  void update(const TCOD_key_t k) override {
    const float old_value = value;
    TextBox::update(k);
    textToValue();
    onArrows = mouse.cx >= x + w - 2 && mouse.cx < x + w && mouse.cy == y;
    if (drag) {
      if (drag_y == -1) {
        drag_x = mouse.x;
        drag_y = mouse.y;
      } else {
        float mdx = ((mouse.x - drag_x) * sensitivity) / (con->getWidth() * 8);
        const float mdy = ((mouse.y - drag_y) * sensitivity) / (con->getHeight() * 8);
        const float old_value2 = value;
        if (std::abs(mdy) > std::abs(mdx)) mdx = -mdy;
        dragValue += (max - min) * mdx;
        value = std::max(min, std::min(value, max));
        if (value != old_value2) {
          valueToText();
          textToValue();
        }
      }
    }
    if (value != old_value && callback_) callback_(value);
  }
  void update(const SDL_Event& ev_tile, const SDL_Event& ev_pixel) override {
    const float old_value = value;
    TextBox::update(ev_tile, ev_pixel);
    textToValue();
    switch (ev_tile.type) {
      case SDL_MOUSEMOTION:
        onArrows = ev_tile.motion.x >= x + w - 2 && ev_tile.motion.x < x + w && ev_tile.motion.y == y;
        break;
      default:
        break;
    }
    switch (ev_pixel.type) {
      case SDL_MOUSEMOTION:
        if (drag) {
          const float old_value2 = value;
          float motion_dx = ((ev_pixel.motion.xrel) * sensitivity) / (64 * 8);
          const float motion_dy = ((ev_pixel.motion.yrel) * sensitivity) / (64 * 8);
          if (std::abs(motion_dy) > std::abs(motion_dx)) motion_dx = -motion_dy;
          dragValue += (max - min) * motion_dx;
          value = std::max(min, std::min(dragValue, max));
          if (value != old_value2) {
            valueToText();
          }
        }
        break;
      default:
        break;
    }
    if (value != old_value && callback_) callback_(value);
  }
  void setMinMax(float min_, float max_) {
    this->min = min_;
    this->max = max_;
  }
  [[deprecated("Use std::function overload instead")]] void setCallback(
      void (*callback)(Widget* wid, float val, void* data), void* data) {
    setCallback([&, callback, data](float value) { callback(this, value, data); });
  }
  void setCallback(std::function<void(float)> callback) { callback_ = callback; }
  void setFormat(const char* fmt_) {
    format_str_ = fmt_ ? fmt_ : DEFAULT_FORMAT;
    valueToText();
  }
  void setValue(float value_) {
    this->value = CLAMP(min, max, value_);
    valueToText();
  }
  void setSensitivity(float sensitivity_) { this->sensitivity = sensitivity_; }

 protected:
  void valueToText() {
    char tmp[128];
    std::snprintf(tmp, sizeof(tmp), format_str_.c_str(), static_cast<double>(value));
    setText(tmp);
  }

  void textToValue() {
    try {
      value = std::stof(text_);
    } catch (std::invalid_argument const&) {
    } catch (std::out_of_range const&) {
    }
    valueToText();
  }
  void onButtonPress() override {
    if (onArrows) {
      drag = true;
      drag_y = -1;
      dragValue = value;
      SDL_SetRelativeMouseMode(SDL_TRUE);
    }
  }
  void onButtonRelease() override {
    if (drag) {
      drag = false;
      SDL_WarpMouseInWindow(nullptr, (x + w - 2) * 8, y * 8);
      SDL_SetRelativeMouseMode(SDL_FALSE);
    }
  }

  float min{}, max{}, value{}, sensitivity{1.0f};
  bool onArrows{false};
  bool drag{false};
  int drag_x{};
  int drag_y{};
  float dragValue{};

 private:
  static constexpr auto DEFAULT_FORMAT{"%.2f"};
  std::string format_str_{DEFAULT_FORMAT};
  std::function<void(float)> callback_{};
};
}  // namespace tcod::gui
#endif  // TCOD_NO_UNICODE
#endif /* TCOD_GUI_SLIDER_HPP */
