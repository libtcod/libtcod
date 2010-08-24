#include <string.h>
#include "libtcod.hpp"
#include "gui.hpp"

Button::Button(const char *label,const char *tip,widget_callback_t cbk, void *userData) 
	: pressed(false),label(NULL) {
	if ( label != NULL ) {
		setLabel(label);
	}
	if ( tip != NULL ) setTip(tip);
	this->x=0;
	this->y=0;
	this->userData=userData;
	this->cbk=cbk;
}

Button::Button(int x,int y,int width, int height,const char *label,const char *tip,widget_callback_t cbk, void *userData) 
	: pressed(false), label(NULL) {
	if ( label != NULL ) setLabel(label);
	if ( tip != NULL ) setTip(tip);
	w=width;
	h=height;
	this->x=x;
	this->y=y;
	this->userData=userData;
	this->cbk=cbk;
}

Button::~Button() {
	if ( label ) free(label);
}

void Button::setLabel(const char *newLabel) {
	if ( label ) free(label);
	label=TCOD_strdup(newLabel);
}

void Button::render() {
	con->setDefaultBackground(mouseIn ? backFocus : back);
	con->setDefaultForeground(mouseIn ? foreFocus : fore);
	if ( w > 0 && h > 0 ) con->rect(x,y,w,h,true,TCOD_BKGND_SET);
	if ( label ) {
		if ( pressed && mouseIn ) {
			con->printEx(x+w/2,y,TCOD_BKGND_NONE,TCOD_CENTER,"-%s-",label);
		} else {
			con->printEx(x+w/2,y,TCOD_BKGND_NONE,TCOD_CENTER,label);
		}
	}
}

void Button::computeSize() {
	if ( label != NULL ) {
		w=strlen(label)+2;
	} else {
		w=4;
	}
	h=1;	
}

void Button::expand(int width, int height) {
	if ( w < width ) w = width;
}

void Button::onButtonPress() {
	pressed=true;
}

void Button::onButtonRelease() {
	pressed=false;
}

void Button::onButtonClick() {
	if ( cbk ) cbk(this,userData);
}



