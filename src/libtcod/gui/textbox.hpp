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
#ifndef TCOD_GUI_TEXTBOX_HPP
#define TCOD_GUI_TEXTBOX_HPP
#ifndef NO_SDL
#ifndef TCOD_NO_UNICODE
#include <SDL3/SDL_timer.h>
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <cctype>
#include <functional>
#include <utility>

#include "../console_printing.hpp"
#include "widget.hpp"

namespace tcod::gui {
class TextBox : public Widget {
 public:
  TextBox(int x, int y, int w, int max_width, const char* label, const char* value, const char* tip = nullptr)
      : Widget{x, y, w, 1}, label_{label ? label : ""}, max_width{max_width} {
    setText(value);
    if (tip) setTip(tip);
    box_width = w;
    if (label_.size()) {
      box_x = static_cast<int>(label_.size() + 1);
      this->w += box_x;
    }
  }
  void render() override {
    TCOD_Console& console = *con;
    const auto bg = TCOD_ColorRGB(back);
    tcod::draw_rect(console, {x, y, w, h}, ' ', std::nullopt, bg);
    const auto fg = TCOD_ColorRGB(fore);
    tcod::print(console, {x, y}, label_, fg, std::nullopt);

    const auto focus_bg = TCOD_ColorRGB(keyboardFocus == this ? backFocus : back);
    tcod::draw_rect(console, {x + box_x, y, box_width, h}, 0, std::nullopt, focus_bg);
    const int len = std::min(static_cast<int>(text_.size() - offset), box_width);
    const auto focus_fg = TCOD_ColorRGB(keyboardFocus == this ? foreFocus : fore);
    if (text_.size())
      tcod::print(console, {x + box_x, y}, tcod::stringf("%.*s", len, &text_[offset]), focus_fg, std::nullopt);

    const int blink_ms = static_cast<int>(blinkingDelay * 1000);
    const bool is_blinking = (SDL_GetTicks() - blink_start_ms_) % (blink_ms * 2) < blink_ms;
    if (keyboardFocus == this && is_blinking && console.in_bounds({x + box_x + pos - offset, y})) {
      auto& tile = console.at(x + box_x + pos - offset, y);
      tile.fg = TCOD_ColorRGBA(fore);
      tile.bg = TCOD_ColorRGBA(back);
      if (insert) std::swap(tile.fg, tile.bg);
    }
  }
  void update(const TCOD_key_t k) override {
    if (keyboardFocus == this && k.pressed) {
      if (k.vk == TCODK_CHAR || (k.vk >= TCODK_0 && k.vk <= TCODK_9) || (k.vk >= TCODK_KP0 && k.vk <= TCODK_KP9)) {
        if (!insert || static_cast<int>(text_.size()) < max_width) {
          text_.insert(pos, 1, k.c);
          if (pos < max_width) ++pos;
          if (pos >= w) offset = pos - w + 1;
          if (text_callback_) text_callback_(text_);
        }
        blink_start_ms_ = SDL_GetTicks();
      }
      switch (k.vk) {
        case TCODK_LEFT:
          if (pos > 0) pos--;
          if (pos < offset) offset = pos;
          blink_start_ms_ = SDL_GetTicks();
          break;
        case TCODK_RIGHT:
          if (pos < static_cast<int>(text_.size())) {
            pos++;
          }
          if (pos >= w) offset = pos - w + 1;
          blink_start_ms_ = SDL_GetTicks();
          break;
        case TCODK_HOME:
          pos = offset = 0;
          blink_start_ms_ = SDL_GetTicks();
          break;
        case TCODK_BACKSPACE:
          if (pos > 0) {
            pos--;
            text_.erase(text_.begin() + pos);
            if (text_callback_) text_callback_(text_);
            if (pos < offset) offset = pos;
          }
          blink_start_ms_ = SDL_GetTicks();
          break;
        case TCODK_DELETE:
          if (pos < static_cast<int>(text_.size())) {
            text_.erase(text_.begin() + pos);
            if (text_callback_) text_callback_(text_);
          }
          blink_start_ms_ = SDL_GetTicks();
          break;
        case TCODK_END:
          pos = static_cast<int>(text_.size());
          if (pos >= w) offset = pos - w + 1;
          blink_start_ms_ = SDL_GetTicks();
          break;
        default:
          break;
      }
    }
    Widget::update(k);
  }

  void update(const SDL_Event& ev_tile, const SDL_Event& ev_pixel) override {
    pos = std::clamp(pos, 0, static_cast<int>(text_.size()));
    switch (ev_tile.type) {
      case SDL_EVENT_KEY_DOWN:
        if (keyboardFocus == this) {
          blink_start_ms_ = SDL_GetTicks();
          switch (ev_tile.key.key) {
            case SDLK_LEFT:
              if (pos > 0) pos--;
              if (pos < offset) offset = pos;
              break;
            case SDLK_RIGHT:
              if (pos < static_cast<int>(text_.size())) pos++;
              break;
            case SDLK_HOME:
              pos = offset = 0;
              break;
            case SDLK_END:
              pos = static_cast<int>(text_.size());
              if (pos >= w) offset = pos - w + 1;
              break;
            case SDLK_BACKSPACE:
              if (pos > 0) {
                pos--;
                text_.erase(text_.begin() + pos);
                if (text_callback_) text_callback_(text_);
                if (pos < offset) offset = pos;
              }
              break;
            case SDLK_DELETE:
              if (pos < static_cast<int>(text_.size())) {
                text_.erase(text_.begin() + pos);
                if (text_callback_) text_callback_(text_);
              }
              break;
            default:
              const unsigned char ch = static_cast<unsigned char>(ev_tile.key.key);
              if (ev_tile.key.key <= 0xFF && std::isprint(ch)) {
                if (!insert || static_cast<int>(text_.size()) < max_width) {
                  text_.insert(pos, 1, ch);
                  if (pos < max_width) ++pos;
                  if (pos >= w) offset = pos - w + 1;
                  if (text_callback_) text_callback_(text_);
                }
              }
              break;
          }
        }
        break;

      default:
        break;
    }
    pos = std::clamp(pos, 0, static_cast<int>(text_.size()));
    Widget::update(ev_tile, ev_pixel);
  }

  void setText(const char* txt) {
    text_ = txt ? txt : "";
    if (max_width && text_.size() > max_width) text_.resize(max_width);
  }
  const char* getValue() { return text_.c_str(); }
  [[deprecated("Use std::function overload instead")]] void setCallback(
      void (*callback)(Widget* wid, char* val, void* data), void* data) {
    setCallback(
        [&, callback, data](const std::string& text) { callback(this, const_cast<char*>(text.c_str()), data); });
  }
  void setCallback(std::function<void(const std::string&)> callback) { text_callback_ = callback; }

  static void setBlinkingDelay(float delay) { blinkingDelay = delay; }

 protected:
  static inline float blinkingDelay{0.5f};
  std::string label_{};
  std::string text_{};
  int pos{}, offset{};
  int box_x{}, box_width{}, max_width{};
  bool insert{true};

  void onButtonClick() override {
    if (mouse.cx >= x + box_x && mouse.cx < x + box_x + box_width) {
      keyboardFocus = this;
      blink_start_ms_ = SDL_GetTicks();
    }
  }

 private:
  std::function<void(const std::string&)> text_callback_{};
  uint64_t blink_start_ms_{};
};
}  // namespace tcod::gui
#endif  // TCOD_NO_UNICODE
#endif  // NO_SDL
#endif /* TCOD_GUI_TEXTBOX_HPP */
