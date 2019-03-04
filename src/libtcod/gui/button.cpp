/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
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
#include "button.hpp"

#include <string.h>
#include <algorithm>

Button::Button(
    const char* label,
    const char* tip,
    widget_callback_t cbk,
    void* userData)
: pressed(false), label(NULL)
{
  if (label) { setLabel(label); }
  if (tip) { setTip(tip); }
  this->x=0;
  this->y=0;
  this->userData=userData;
  this->cbk=cbk;
}
Button::Button(
    int x,
    int y,
    int width,
    int height,
    const char* label,
    const char* tip,
    widget_callback_t cbk,
    void* userData)
: pressed(false), label(NULL)
{
  if (label) { setLabel(label); }
  if (tip) { setTip(tip); }
  w = width;
  h = height;
  this->x=x;
  this->y=y;
  this->userData=userData;
  this->cbk=cbk;
}
Button::~Button() {
  if (label) { free(label); }
}
void Button::setLabel(const char* newLabel) {
  if (label) { free(label); }
  label = TCOD_strdup(newLabel);
}
void Button::render() {
  con->setDefaultBackground(mouseIn ? backFocus : back);
  con->setDefaultForeground(mouseIn ? foreFocus : fore);
  if (w > 0 && h > 0) { con->rect(x, y, w, h, true, TCOD_BKGND_SET); }
  if (label) {
    if (pressed && mouseIn) {
      con->printEx(x + w / 2, y, TCOD_BKGND_NONE, TCOD_CENTER, "-%s-", label);
    } else {
      con->printEx(x + w / 2, y, TCOD_BKGND_NONE, TCOD_CENTER, label);
    }
  }
}
void Button::computeSize()
{
  w = label ? static_cast<int>(strlen(label) + 2) : 4;
  h = 1;
}
void Button::expand(int width, int)
{
  w = std::max(w, width);
}
void Button::onButtonPress()
{
  pressed = true;
}
void Button::onButtonRelease()
{
  pressed = false;
}
void Button::onButtonClick()
{
  if (cbk) { cbk(this, userData); }
}
