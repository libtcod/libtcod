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
#ifndef TCOD_GUI_RADIOBUTTON_HPP
#define TCOD_GUI_RADIOBUTTON_HPP
#ifndef TCOD_NO_UNICODE
#include "button.hpp"
class RadioButton : public Button {
 public:
  RadioButton(std::string label, std::string tip, std::function<void()> callback)
      : Button{label, tip, callback}, group{defaultGroup} {}
  [[deprecated("Switch to a non-deprecated constructor")]] RadioButton(
      const char* label, const char* tip, widget_callback_t callback, void* userData = nullptr)
      : RadioButton{label ? label : "", tip ? tip : "", makeCallback_(callback, userData)} {}
  RadioButton(int x, int y, int width, int height, std::string label, std::string tip, std::function<void()> callback)
      : Button{x, y, width, height, label, tip, callback}, group{defaultGroup} {}
  [[deprecated("Switch to a non-deprecated constructor")]] RadioButton(
      int x,
      int y,
      int width,
      int height,
      const char* label,
      const char* tip,
      widget_callback_t callback,
      void* userData = nullptr)
      : RadioButton{x, y, width, height, label ? label : "", tip ? tip : "", makeCallback_(callback, userData)} {}

  void setGroup(int group_) { this->group = group_; }
  void render() override {
    Button::render();
    if (groupSelect[group] == this) {
      auto& console = static_cast<TCOD_Console&>(*con);
      if (console.in_bounds({x, y})) console.at({x, y}).ch = '>';
    }
  }
  void select() { groupSelect[group] = this; }
  void unSelect() { groupSelect[group] = nullptr; }
  static void unSelectGroup(int group) { groupSelect[group] = nullptr; }
  static void setDefaultGroup(int group) { defaultGroup = group; }

 protected:
  void onButtonClick() override {
    select();
    Button::onButtonClick();
  }

  static inline int defaultGroup{0};
  static inline RadioButton* groupSelect[512]{};
  int group{defaultGroup};
};
#endif  // TCOD_NO_UNICODE
#endif /* TCOD_GUI_RADIOBUTTON_HPP */
