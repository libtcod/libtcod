#include "libtcod.hpp"
#include "gui.hpp"

void VBox::computeSize() {
	int cury=y;
	w=0;
	for (Widget **wid=content.begin(); wid != content.end(); wid ++ ) {
		if ( (*wid)->isVisible() ) {
			(*wid)->x=x;
			(*wid)->y=cury;
			(*wid)->computeSize();
			if ((*wid)->w > w) w=(*wid)->w;
			cury+=(*wid)->h+padding;
		}
	}
	h=cury-y;
	for (Widget **wid=content.begin(); wid != content.end(); wid ++ ) {
		if ( (*wid)->isVisible() ) {
			(*wid)->expand(w,(*wid)->h);
		}
	}	
}

