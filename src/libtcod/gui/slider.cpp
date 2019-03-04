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
#include "slider.hpp"

#include <stdio.h>
#include <string.h>
#include <math.h>

Slider::Slider(int x,int y,int w, float min, float max, const char *label, const char *tip)
	: TextBox(x,y,w,10,label,NULL,tip),min(min),max(max),value((min+max)*0.5f),sensitivity(1.0f),
	onArrows(false),drag(false),fmt(NULL),cbk(NULL),data(NULL) {
	valueToText();
	this->w+=2;
}

Slider::~Slider() {
	if (fmt)
		free(fmt);
}

void Slider::setFormat(const char *fmt) {
	if (this->fmt)
		free(this->fmt);
	if (fmt)
		this->fmt = TCOD_strdup(fmt);
	else
		this->fmt = NULL;
	valueToText();
}

void Slider::render() {
	w-=2;
	TextBox::render();
	w+=2;
	con->setDefaultBackground((onArrows || drag) ? backFocus : back);
	con->setDefaultForeground((onArrows || drag) ? foreFocus : fore);
	con->rect(x+w-2,y,2,1,TCOD_BKGND_SET);
	con->setChar(x+w-2,y,TCOD_CHAR_ARROW_W);
	con->setChar(x+w-1,y,TCOD_CHAR_ARROW_E);
}

void Slider::update(TCOD_key_t k) {
  float old_value = value;
  TextBox::update(k);
  textToValue();
  if (mouse.cx >= x + w - 2 && mouse.cx < x + w && mouse.cy == y) {
    onArrows = true;
  } else {
    onArrows = false;
  }
  if (drag) {
    if (dragy == -1) {
      dragx = mouse.x;
      dragy = mouse.y;
    } else {
      float mdx = ((mouse.x - dragx) * sensitivity) / (con->getWidth() * 8);
      float mdy = ((mouse.y - dragy) * sensitivity) / (con->getHeight() * 8);
      float old_value2 = value;
      if (fabs(mdy) > fabs(mdx)) { mdx = -mdy; }
      value = dragValue + (max - min) * mdx;
      value = std::max(min, std::min(value, max));
      if (value != old_value2) {
        valueToText();
        textToValue();
      }
    }
  }
  if (value != old_value && cbk) {
    cbk(this, value, data);
  }
}

void Slider::valueToText()
{
  char tmp[128];
  sprintf(tmp, fmt ? fmt : "%.2f", static_cast<double>(value));
  setText(tmp);
}

void Slider::textToValue() {
#ifdef TCOD_VISUAL_STUDIO
    value=(float)atof(txt);
#else
	char *endptr;
	float f=strtof(txt,&endptr);
	if ( f != 0.0f || endptr != txt ) value=f;
#endif
}

void Slider::setValue(float value) {
	this->value=CLAMP(min,max,value);
	valueToText();
}

void Slider::onButtonPress() {
	if ( onArrows ) {
		drag=true;
		dragy=-1;
		dragValue=value;
		TCODMouse::showCursor(false);
	}
}

void Slider::onButtonRelease() {
	if ( drag ) {
		drag=false;
		TCODMouse::move((x+w-2)*8,y*8);
		TCODMouse::showCursor(true);
	}
}
