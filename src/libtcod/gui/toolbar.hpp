/* BSD 3-Clause License
 *
 * Copyright © 2008-2026, Jice and the libtcod contributors.
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
#ifndef TCOD_GUI_TOOLBAR_HPP
#define TCOD_GUI_TOOLBAR_HPP
#ifndef NO_SDL
#ifndef TCOD_NO_UNICODE
#include <string.h>

#include <algorithm>
#include <string>

#include "../console_printing.hpp"
#include "container.hpp"

namespace tcod::gui {
class ToolBar : public Container {
 public:
  ToolBar(int x, int y, const char* name, const char* tip = nullptr) : Container{x, y, 0, 2}, title_{name ? name : ""} {
    if (title_.size()) w = static_cast<int>(title_.size() + 4);
    setTip(tip);
  }
  ToolBar(int x, int y, int w, const char* name, const char* tip = nullptr)
      : Container{x, y, w, 2}, title_{name ? name : ""}, fixed_width_{w} {
    if (title_.size()) fixed_width_ = w = std::max<int>(static_cast<int>(title_.size() + 4), w);
    setTip(tip);
  }
  void render() {
    static constexpr auto FRAME_SINGLE_PIPE =
        std::array{0x250C, 0x2500, 0x2510, 0x2502, 0x20, 0x2502, 0x2514, 0x2500, 0x2518};
    const auto fg = tcod::ColorRGB(fore);
    const auto bg = tcod::ColorRGB(back);
    tcod::draw_frame(*con, {x, y, w, h}, FRAME_SINGLE_PIPE, fg, bg, TCOD_BKGND_SET, true);
    if (title_.size()) {
      tcod::print_rect(*con, {x, y, w, 1}, tcod::stringf(" %s ", title_.c_str()), bg, fg, TCOD_CENTER);
    }
    Container::render();
  }
  void setName(const char* name) {
    title_ = name ? name : "";
    if (title_.size()) fixed_width_ = std::max(static_cast<int>(title_.size() + 4), fixed_width_);
  }

  void addSeparator(const char* txt, const char* tip = nullptr) { addWidget(std::make_unique<Separator>(txt, tip)); }
  void computeSize() override {
    int current_y = y + 1;
    w = title_.size() ? static_cast<int>(title_.size() + 4) : 2;
    for (auto& wid : content_) {
      if (wid->isVisible()) {
        wid->x = x + 1;
        wid->y = current_y;
        wid->computeSize();
        if (wid->w + 2 > w) w = wid->w + 2;
        current_y += wid->h;
      }
    }
    if (w < fixed_width_) w = fixed_width_;
    h = current_y - y + 1;
    for (auto& wid : content_) {
      if (wid->isVisible()) {
        wid->expand(w - 2, wid->h);
      }
    }
  }

 protected:
  std::string title_{};  // Title bar text of this toolbar
  int fixed_width_{0};

 private:
  struct Separator : public Widget {
    Separator(const char* txt, const char* tip = nullptr) : Widget{0, 0, 0, 1}, text_{txt ? txt : ""} { setTip(tip); }
    void computeSize() override { w = text_.size() ? static_cast<int>(text_.size() + 2) : 0; }
    void expand(int width, int) override { w = std::max(w, width); }
    void render() override {
      auto& console = static_cast<TCOD_Console&>(*con);
      const auto fg = TCOD_ColorRGB(fore);
      const auto bg = TCOD_ColorRGB(back);
      tcod::draw_rect(console, {x, y, w, 1}, 0x2500, fg, bg);
      if (console.in_bounds({x - 1, y})) {
        console.at({x - 1, y}) = {0x251C, {fg.r, fg.g, fg.b, 255}, {bg.r, bg.g, bg.b, 255}};  // ├
      }
      if (console.in_bounds({x + w, y})) {
        console.at({x + w, y}) = {0x2524, {fg.r, fg.g, fg.b, 255}, {bg.r, bg.g, bg.b, 255}};  // ┤
      }
      tcod::print_rect(console, {x, y, w, 1}, tcod::stringf(" %s ", text_.c_str()), bg, fg, TCOD_CENTER);
    }
    std::string text_{};  // Text displayed over separator
  };
};
}  // namespace tcod::gui
#endif  // TCOD_NO_UNICODE
#endif  // NO_SDL
#endif /* TCOD_GUI_TOOLBAR_HPP */
