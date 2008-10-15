#include <string.h>
#include "libtcod.hpp"
#include "gui.hpp"

void ToggleButton::render() {
	con->setBackgroundColor(mouseIn ? backFocus : back);
	con->setForegroundColor(mouseIn ? foreFocus : fore);
	con->rect(x,y,w,h,true,TCOD_BKGND_SET);
	if ( label ) {
		con->printLeft(x,y,TCOD_BKGND_NONE,"%c %s",pressed ? 'X' : '_', label);
	} else {
		con->printLeft(x,y,TCOD_BKGND_NONE,"%c",pressed ? 'X' : '_');
	}
}

void ToggleButton::onButtonPress() {
}

void ToggleButton::onButtonRelease() {
}

void ToggleButton::onButtonClick() {
	pressed=!pressed;
	if ( cbk ) cbk(this,userData);
}



