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
#ifndef TCOD_GUI_WIDGET_HPP
#define TCOD_GUI_WIDGET_HPP
#include <memory>
#include <string>
#include <vector>

#include "../color.hpp"
#include "../console.hpp"
#include "../list.hpp"
#include "../mouse.hpp"
#include "../sys.hpp"
#include "gui_portability.hpp"

typedef void (*widget_callback_t)(class Widget* w, void* userData);
class Widget {
 public:
  Widget() : Widget{0, 0, 0, 0} {}
  Widget(int x, int y) : Widget{x, y, 0, 0} {}
  Widget(int x, int y, int w, int h) : x{x}, y{y}, w{w}, h{h} { widgets_.push_back(this); }
  virtual ~Widget() {
    if (focus == this) focus = NULL;
    auto found = std::find(widgets_.begin(), widgets_.end(), this);
    if (found != widgets_.end()) widgets_.erase(found);
  }

  virtual void render() {}
  virtual void update(const TCOD_key_t) {
    const bool curs = TCODMouse::isCursorVisible();
    if (curs) {
      if (mouse.cx >= x && mouse.cx < x + w && mouse.cy >= y && mouse.cy < y + h) {
        if (!mouseIn) {
          mouseIn = true;
          onMouseIn();
        }
        focus = this;
      } else {
        if (mouseIn) {
          mouseIn = false;
          onMouseOut();
        }
        mouseL = false;
        if (this == focus) {
          focus = NULL;
        }
      }
    }
    if (mouseIn || (!curs && this == focus)) {
      if (mouse.lbutton && !mouseL) {
        mouseL = true;
        onButtonPress();
      } else if (!mouse.lbutton && mouseL) {
        onButtonRelease();
        keyboardFocus = NULL;
        if (mouseL) {
          onButtonClick();
        }
        mouseL = false;
      } else if (mouse.lbutton_pressed) {
        keyboardFocus = NULL;
        onButtonClick();
      }
    }
  }
  void move(int x_, int y_) {
    this->x = x_;
    this->y = y_;
  }
  void setTip(const char* tip) { this->tip_ = (tip ? tip : ""); }
  virtual void setVisible(bool val) { visible = val; }
  bool isVisible() const noexcept { return visible; }
  virtual void computeSize() {}
  static void setBackgroundColor(const TCODColor col, const TCODColor colFocus) {
    back = col;
    backFocus = colFocus;
  }

  static void setForegroundColor(const TCODColor col, const TCODColor colFocus) {
    fore = col;
    foreFocus = colFocus;
  }
  static void setConsole(TCODConsole* console) { con = console; }
  static void updateWidgets(const TCOD_key_t k, const TCOD_mouse_t p_mouse) {
    mouse = p_mouse;
    updateWidgetsIntern(k);
  }
  static void renderWidgets() {
    if (!con) con = TCODConsole::root;
    for (auto& w : widgets_) {
      if (w->isVisible()) w->render();
    }
  }

  int x{}, y{}, w{}, h{};
  static inline Widget* focus{};
  static inline Widget* keyboardFocus{};
  static inline TCOD_mouse_t mouse{};
  static inline TCODColor fore{220, 220, 180};
  virtual void expand(int, int) {}  // parameters: width, height
 protected:
  friend class StatusBar;
  friend class ToolBar;
  friend class VBox;
  friend class HBox;

  virtual void onMouseIn() {}
  virtual void onMouseOut() {}
  virtual void onButtonPress() {}
  virtual void onButtonRelease() {}
  virtual void onButtonClick() {}

  static void updateWidgetsIntern(const TCOD_key_t k) {
    elapsed = TCODSystem::getLastFrameLength();
    for (auto& w : widgets_) {
      if (w->isVisible()) {
        w->computeSize();
        w->update(k);
      }
    }
  }

  /***************************************************************************
      @brief Used for backwards compatibility, do not call this function directly.
   */
  std::function<void()> makeCallback_(widget_callback_t callback, void* userData) {
    return [&, callback, userData]() { callback(this, userData); };
  }

  static inline float elapsed{};
  static inline TCODColor back{40, 40, 120};
  static inline TCODColor backFocus{70, 70, 130};
  static inline TCODColor foreFocus{255, 255, 255};
  static inline TCODConsole* con{};
  static inline std::vector<Widget*> widgets_{};
  std::string tip_{};
  bool mouseIn{false};
  bool mouseL{false};
  bool visible{true};
};
#endif /* TCOD_GUI_WIDGET_HPP */
