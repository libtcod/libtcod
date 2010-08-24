#include <string.h>
#include "libtcod.hpp"
#include "gui.hpp"

void ToggleButton::render() {
	con->setDefaultBackground(mouseIn ? backFocus : back);
	con->setDefaultForeground(mouseIn ? foreFocus : fore);
	con->rect(x,y,w,h,true,TCOD_BKGND_SET);
	if ( label ) {
		con->printEx(x,y,TCOD_BKGND_NONE,TCOD_LEFT,"%c %s",pressed ? TCOD_CHAR_CHECKBOX_SET : TCOD_CHAR_CHECKBOX_UNSET, label);
	} else {
		con->printEx(x,y,TCOD_BKGND_NONE,TCOD_LEFT,"%c",pressed ? TCOD_CHAR_CHECKBOX_SET : TCOD_CHAR_CHECKBOX_UNSET);
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



