#include "libtcod.hpp"
#include "gui.hpp"

Label::Label(int x, int y, const char *label, const char *tip) : Widget(x,y,0,1) {
	this->label=label;
	if ( tip ) setTip(tip);
}

void Label::render() {
	con->setBackgroundColor(back);
	con->setForegroundColor(fore);
	con->printEx(x,y,TCOD_BKGND_NONE,TCOD_LEFT,label);
}

void Label::computeSize() {
	if ( label ) w=strlen(label);
	else w=0;
}

void Label::expand(int width, int height) {
	if ( w < width ) w=width;
}

