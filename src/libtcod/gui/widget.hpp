/* BSD 3-Clause License
 *
 * Copyright © 2008-2025, Jice and the libtcod contributors.
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
#ifndef TCOD_GUI_WIDGET_HPP
#define TCOD_GUI_WIDGET_HPP
#ifndef NO_SDL
#include <SDL3/SDL_events.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "../color.hpp"
#include "../console.hpp"
#include "../mouse.hpp"
#include "../portability.h"
#include "../sys.hpp"

namespace tcod::gui {
typedef void (*widget_callback_t)(class Widget* w, void* userData);
class Widget {
 public:
  Widget() : Widget{0, 0, 0, 0} {}
  Widget(int x, int y) : Widget{x, y, 0, 0} {}
  Widget(int x, int y, int w, int h) : x{x}, y{y}, w{w}, h{h} { widgets_.push_back(this); }
  virtual ~Widget() {
    if (focus == this) focus = nullptr;
    auto found = std::find(widgets_.begin(), widgets_.end(), this);
    if (found != widgets_.end()) widgets_.erase(found);
  }

  virtual void render() {}
  virtual void update(const TCOD_key_t) {
    const int cursor_visible = SDL_CursorVisible();
    if (cursor_visible) {
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
        if (this == focus) focus = nullptr;
      }
    }
    if (mouseIn || (!cursor_visible && this == focus)) {
      if (mouse.lbutton && !mouseL) {
        mouseL = true;
        onButtonPress();
      } else if (!mouse.lbutton && mouseL) {
        onButtonRelease();
        keyboardFocus = nullptr;
        if (mouseL) {
          onButtonClick();
        }
        mouseL = false;
      } else if (mouse.lbutton_pressed) {
        keyboardFocus = nullptr;
        onButtonClick();
      }
    }
  }
  virtual void update(const SDL_Event& ev_tile, [[maybe_unused]] const SDL_Event& ev_pixel) {
    const int cursor_visible = SDL_CursorVisible();
    const bool cursor_over = mouseIn || (!cursor_visible && this == focus);
    switch (ev_tile.type) {
      case SDL_EVENT_MOUSE_MOTION:
        if (cursor_visible) {
          if (ev_tile.motion.x >= x && ev_tile.motion.x < x + w && ev_tile.motion.y >= y && ev_tile.motion.y < y + h) {
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
            if (this == focus) focus = nullptr;
          }
        }
        break;
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (cursor_over && ev_tile.button.button == SDL_BUTTON_LEFT) {
          mouseL = true;
          onButtonPress();
        }
        break;
      case SDL_EVENT_MOUSE_BUTTON_UP:
        if (ev_tile.button.button == SDL_BUTTON_LEFT && mouseL) {
          onButtonRelease();
          keyboardFocus = nullptr;
          if (cursor_over) onButtonClick();
          mouseL = false;
        }
        break;

      default:
        break;
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
  static void updateWidgets() {
    for (auto& w : widgets_) {
      if (w->isVisible()) w->computeSize();
    }
  }
  static void updateWidgets(const SDL_Event& ev_tile, const SDL_Event& ev_pixel) {
    switch (ev_tile.type) {
      case SDL_EVENT_MOUSE_MOTION:
        mouse.cx = (int)ev_tile.motion.x;
        mouse.cy = (int)ev_tile.motion.y;
        mouse.dcx = (int)ev_tile.motion.xrel;
        mouse.dcy = (int)ev_tile.motion.yrel;
        break;
      default:
        break;
    }
    switch (ev_pixel.type) {
      case SDL_EVENT_MOUSE_MOTION:
        mouse.dx = (int)ev_pixel.motion.xrel;
        mouse.dy = (int)ev_pixel.motion.yrel;
        break;
      default:
        break;
    }
    updateWidgets();
    for (auto& w : widgets_) {
      if (w->isVisible()) w->update(ev_tile, ev_pixel);
    }
  }
  static void renderWidgets() {
    if (!con) con = TCODConsole::root;
    updateWidgets();
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

  static void updateWidgetsIntern() {
    for (auto& w : widgets_) {
      if (w->isVisible()) w->computeSize();
    }
  }
  static void updateWidgetsIntern(const TCOD_key_t k) {
    updateWidgetsIntern();
    for (auto& w : widgets_) {
      if (w->isVisible()) w->update(k);
    }
  }

  /***************************************************************************
      @brief Used for backwards compatibility, do not call this function directly.
   */
  std::function<void()> makeCallback_(widget_callback_t callback, void* userData) {
    return [&, callback, userData]() { callback(this, userData); };
  }

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
}  // namespace tcod::gui
#endif  // NO_SDL
#endif /* TCOD_GUI_WIDGET_HPP */
