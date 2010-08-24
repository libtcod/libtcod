#include "libtcod.hpp"
#include "gui.hpp"

void StatusBar::render() {
	con->setDefaultBackground(back);
	con->rect(x,y,w,h,true,TCOD_BKGND_SET);
	if ( focus && focus->tip ) {
		con->setDefaultForeground(fore);
		con->printRectEx(x+1,y,w,h,TCOD_BKGND_NONE,TCOD_LEFT,focus->tip);
	}
}


