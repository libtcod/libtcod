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
#ifndef TCOD_GUI_CONTAINER_HPP
#define TCOD_GUI_CONTAINER_HPP
#ifndef TCOD_NO_UNICODE
#include <memory>

#include "widget.hpp"

namespace tcod::gui {
class Container : public Widget {
 public:
  Container(int x, int y, int w, int h) : Widget{x, y, w, h} {}
  void addWidget(std::shared_ptr<Widget> wid) {
    content_.emplace_back(wid);
    widgets_.erase(std::find(widgets_.begin(), widgets_.end(), wid.get()));
  }
  [[deprecated("This function should be passed a std::shared_ptr instead.")]] void addWidget(Widget* wid) {
    addWidget(std::shared_ptr<Widget>{wid});
  }
  void removeWidget(const std::shared_ptr<Widget>& wid) {
    content_.erase(std::find(content_.begin(), content_.end(), wid));
  }
  void removeWidget(Widget* wid) {
    content_.erase(std::find_if(content_.begin(), content_.end(), [&](auto& it) { return it.get() == wid; }));
  }
  void setVisible(bool val) override { Widget::setVisible(val); }
  void render() override {
    for (auto wid : content_) {
      if (wid->isVisible()) wid->render();
    }
  }
  void clear() { content_.clear(); }
  void update(const TCOD_key_t k) override {
    Widget::update(k);
    for (auto wid : content_) {
      if (wid->isVisible()) wid->update(k);
    }
  }

 protected:
  std::vector<std::shared_ptr<Widget>> content_{};
};
}  // namespace tcod::gui
#endif  // TCOD_NO_UNICODE
#endif /* TCOD_GUI_CONTAINER_HPP */
