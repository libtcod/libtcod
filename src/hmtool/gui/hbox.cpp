#include "libtcod.hpp"
#include "gui.hpp"

HBox::HBox(int x, int y, int padding) : VBox(x,y,padding) {
}

void HBox::computeSize() {
	int curx=x;
	h=0;
	for (Widget **wid=content.begin(); wid != content.end(); wid ++ ) {
		if ( (*wid)->isVisible() ) {
			(*wid)->y=y;
			(*wid)->x=curx;
			(*wid)->computeSize();
			if ((*wid)->h > h) h=(*wid)->h;
			curx+=(*wid)->w+padding;
		}
	}
	w=curx-x;
	for (Widget **wid=content.begin(); wid != content.end(); wid ++ ) {
		if ( (*wid)->isVisible() ) {
			(*wid)->expand((*wid)->w,h);
		}
	}	
}

