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
#ifndef TCOD_GUI_BUTTON_HPP
#define TCOD_GUI_BUTTON_HPP
#ifndef TCOD_NO_UNICODE
#include <string.h>

#include <algorithm>
#include <functional>

#include "../console_printing.hpp"
#include "widget.hpp"

namespace tcod::gui {
class Button : public Widget {
 public:
  Button(std::string label, std::string tip, std::function<void()> callback) : label_{label}, callback_{callback} {
    tip_ = tip;
  }
  [[deprecated("Switch to a non-deprecated constructor")]] Button(
      const char* label, const char* tip, widget_callback_t callback, void* userData = nullptr)
      : Button{label ? label : "", tip ? tip : "", makeCallback_(callback, userData)} {}
  Button(int x, int y, int width, int height, std::string label, std::string tip, std::function<void()> callback)
      : Widget{x, y, width, height}, label_{label}, callback_{callback} {
    tip_ = tip;
  }
  [[deprecated("Switch to a non-deprecated constructor")]] Button(
      int x,
      int y,
      int width,
      int height,
      const char* label,
      const char* tip,
      widget_callback_t callback,
      void* userData = nullptr)
      : Button{x, y, width, height, label ? label : "", tip ? tip : "", makeCallback_(callback, userData)} {}

  void render() override {
    const auto fg = TCOD_ColorRGB(mouseIn ? foreFocus : fore);
    const auto bg = TCOD_ColorRGB(mouseIn ? backFocus : back);
    if (w > 0 && h > 0) {
      tcod::draw_rect(*con, {x, y, w, h}, ' ', fg, bg);
    }
    if (label_.size()) {
      if (pressed && mouseIn) {
        tcod::print(*con, {x + w / 2, y}, tcod::stringf("-%s-", label_.c_str()), fg, std::nullopt, TCOD_CENTER);
      } else {
        tcod::print(*con, {x + w / 2, y}, label_, fg, std::nullopt, TCOD_CENTER);
      }
    }
  }
  void setLabel(const char* newLabel) { label_ = newLabel ? newLabel : ""; }
  void computeSize() override {
    w = label_.size() ? static_cast<int>(label_.size() + 2) : 4;
    h = 1;
  }
  inline bool isPressed() { return pressed; }

 protected:
  void onButtonPress() override { pressed = true; }
  void onButtonRelease() override { pressed = false; }
  void onButtonClick() override {
    if (callback_) callback_();
  }
  void expand(int width, int) override { w = std::max(w, width); }

  bool pressed{};
  std::string label_{};
  std::function<void()> callback_{};
};
}  // namespace tcod::gui
#endif  // TCOD_NO_UNICODE
#endif /* TCOD_GUI_BUTTON_HPP */
