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
#pragma once
#ifndef TCOD_GUI_TOGGLEBUTTON_HPP
#define TCOD_GUI_TOGGLEBUTTON_HPP
#ifndef TCOD_NO_UNICODE
#include <string.h>

#include "../console_printing.hpp"
#include "button.hpp"

namespace tcod::gui {
class ToggleButton : public Button {
 public:
  ToggleButton(std::string label, std::string tip, std::function<void()> callback) : Button{label, tip, callback} {}
  [[deprecated("Switch to a non-deprecated constructor")]] ToggleButton(
      const char* label, const char* tip, widget_callback_t callback, void* userData = nullptr)
      : Button{label ? label : "", tip ? tip : "", makeCallback_(callback, userData)} {}
  ToggleButton(int x, int y, int width, int height, std::string label, std::string tip, std::function<void()> callback)
      : Button{x, y, width, height, label, tip, callback} {}
  [[deprecated("Switch to a non-deprecated constructor")]] ToggleButton(
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
    tcod::draw_rect(*con, {x, y, w, h}, ' ', std::nullopt, bg);
    const char* check = pressed ? u8"\u2611" : u8"\u2610";
    const auto text = label_.size() ? tcod::stringf("%s %s", check, label_.c_str()) : check;
    tcod::print(*con, {x, y}, text, fg, std::nullopt);
  }

  bool isPressed() { return pressed; }
  void setPressed(bool val) { pressed = val; }

 protected:
  void onButtonPress() override {}
  void onButtonRelease() override {}
  void onButtonClick() override {
    pressed = !pressed;
    if (callback_) callback_();
  }
};
}  // namespace tcod::gui
#endif  // TCOD_NO_UNICODE
#endif /* TCOD_GUI_TOGGLEBUTTON_HPP */
