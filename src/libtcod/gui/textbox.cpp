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
#include "textbox.hpp"

#include <string.h>
#include <stdio.h>

float TextBox::blinkingDelay=0.5f;
TextBox::TextBox(int x,int y,int w, int maxw, const char *label, const char *value, const char *tip)
	: Widget(x,y,w,1),txt(NULL),blink(0.0f),pos(0),offset(0),maxw(maxw),insert(true),txtcbk(NULL),
	data(NULL) {
	if ( maxw > 0 ) {
		txt = new char[maxw+1];
		memset(txt,0,sizeof(char)*(maxw+1));
		if ( value ) {
			strncpy(txt,value,maxw);
		}
	}
	if ( tip ) setTip(tip);
	if ( label ) this->label=TCOD_strdup(label);
	boxw=w;
	if (label ) {
		boxx=static_cast<int>(strlen(label) + 1);
		this->w+= boxx;
	}
}

TextBox::~TextBox() {
	if ( txt ) delete [] txt;
	if ( label ) free(label);
}

void TextBox::setText(const char *txt) {
	strncpy(this->txt,txt,maxw);
}

void TextBox::render() {
	con->setDefaultBackground(back);
	con->setDefaultForeground(fore);
	con->rect(x,y,w,h,true,TCOD_BKGND_SET);
	if ( label ) con->printEx(x,y,TCOD_BKGND_NONE,TCOD_LEFT,label);

	con->setDefaultBackground(keyboardFocus == this ? foreFocus : fore);
	con->setDefaultForeground(keyboardFocus == this ? backFocus : back);
	con->rect(x+boxx,y,boxw,h,false,TCOD_BKGND_SET);
	int len = static_cast<int>(strlen(txt) - offset);
	if (len > boxw) len = boxw;
	if ( txt ) con->printEx(x+boxx,y,TCOD_BKGND_NONE,TCOD_LEFT,"%.*s",len,&txt[offset]);
	if (keyboardFocus == this && blink > 0.0f) {
		if (insert) {
			con->setCharBackground(x+boxx+pos-offset,y,fore);
			con->setCharForeground(x+boxx+pos-offset,y,back);
		} else {
			con->setCharBackground(x+boxx+pos-offset,y,back);
			con->setCharForeground(x+boxx+pos-offset,y,fore);
		}
	}
}

void TextBox::update(TCOD_key_t k) {
	if ( keyboardFocus == this ) {
		blink -= elapsed;
		if ( blink < -blinkingDelay ) blink += 2*blinkingDelay;
		if ( k.vk == TCODK_CHAR ||
			(k.vk >= TCODK_0 && k.vk <= TCODK_9) ||
			(k.vk >= TCODK_KP0 && k.vk <= TCODK_KP9) ) {
			if (!insert || static_cast<int>(strlen(txt)) < maxw) {
				if (insert && pos < static_cast<int>(strlen(txt)))  {
					for (int i = static_cast<int>(strlen(txt)); i >= pos; --i) {
						txt[i+1]=txt[i];
					}
				}
				txt[pos]=k.c;
				if ( pos < maxw ) pos++;
				if ( pos >= w ) offset = pos-w+1;
				if (txtcbk) txtcbk(this,txt,data);
			}
			blink=blinkingDelay;
		}
		switch ( k.vk ) {
			case TCODK_LEFT :
				if (pos > 0) pos--;
				if ( pos < offset ) offset=pos;
				blink=blinkingDelay;
			break;
			case TCODK_RIGHT :
				if (pos < static_cast<int>(strlen(txt))) { pos++; }
				if ( pos >= w ) offset = pos-w+1;
				blink=blinkingDelay;
			break;
			case TCODK_HOME :
				pos = offset = 0;
				blink=blinkingDelay;
			break;
			case TCODK_BACKSPACE :
				if (pos > 0) {
					pos--;
					for (uint32_t i=pos; i <= strlen(txt); i++ ) {
						txt[i]=txt[i+1];
					}
					if (txtcbk) txtcbk(this,txt,data);
					if ( pos < offset ) offset=pos;
				}
				blink=blinkingDelay;
			break;
			case TCODK_DELETE :
				if (pos < static_cast<int>(strlen(txt))) {
					for (uint32_t i=pos; i <= strlen(txt); i++ ) {
						txt[i]=txt[i+1];
					}
					if (txtcbk) txtcbk(this,txt,data);
				}
				blink=blinkingDelay;
			break;
/*
			case TCODK_INSERT :
				insert=!insert;
				blink=blinkingDelay;
			break;
*/
			case TCODK_END :
				pos = static_cast<int>(strlen(txt));
				if ( pos >= w ) offset = pos-w+1;
				blink=blinkingDelay;
			break;
			default:break;
		}
	}
	Widget::update(k);
}

void TextBox::onButtonClick() {
	if ( mouse.cx >= x+boxx && mouse.cx < x+boxx+boxw ) keyboardFocus=this;
}
