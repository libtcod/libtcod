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
#include "flatlist.hpp"

#include <stdio.h>
#include <math.h>

FlatList::FlatList(int x,int y,int w, const char **list, const char *label, const char *tip)
	: TextBox(x,y,w,10,label,NULL,tip),value(list),list(list),
	onLeftArrow(false),onRightArrow(false),cbk(NULL),data(NULL) {
	valueToText();
	this->w+=2;
}

FlatList::~FlatList() {
}

void FlatList::render() {
	w--;
	boxx++;
	TextBox::render();
	boxx--;
	w++;
	con->setDefaultBackground((onLeftArrow) ? backFocus : back);
	con->setDefaultForeground((onLeftArrow) ? foreFocus : fore);
	con->putChar(x+boxx,y,TCOD_CHAR_ARROW_W);
	con->setDefaultBackground((onRightArrow) ? backFocus : back);
	con->setDefaultForeground((onRightArrow) ? foreFocus : fore);
	con->putChar(x+w-1,y,TCOD_CHAR_ARROW_E);
}

void FlatList::update(TCOD_key_t k) {
	onLeftArrow=onRightArrow=false;
	if ( mouse.cx == x+boxx && mouse.cy == y ) onLeftArrow=true;
	else if ( mouse.cx == x+w-1 && mouse.cy == y ) onRightArrow=true;
	Widget::update(k);
}

void FlatList::valueToText() {
	setText(*value);
}

void FlatList::textToValue() {
	const char **ptr=list;
	while (*ptr) {
		if ( strcmp(txt,*ptr) == 0 ) {
			value = ptr;
			break;
		}
		ptr++;
	}
}

void FlatList::onButtonClick() {
	const char **oldValue=value;
	if ( onLeftArrow ) {
		if ( value == list ) {
			while (*value) {
				value++;
			}
		}
		value--;
	} else if ( onRightArrow ) {
		value++;
		if ( *value == NULL ) value=list;
	}
	if ( value != oldValue && cbk ) {
		valueToText();
		cbk(this,*value,data);
	}

}

void FlatList::setValue(const char * v) {
	const char **ptr=list;
	while (*ptr) {
		if ( strcmp(v,*ptr) == 0 ) {
			value = ptr;
			valueToText();
			break;
		}
		ptr++;
	}
}

void FlatList::setList(const char **l) {
	value=list=l;
	valueToText();
}
