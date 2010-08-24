#include <stdio.h>
#include <math.h>
#include "libtcod.hpp"
#include "gui.hpp"

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


