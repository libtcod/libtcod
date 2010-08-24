#include "libtcod.hpp"
#include "gui.hpp"

Image::Image(int x,int y,int w, int h, const char *tip) : Widget(x,y,w,h), back(TCODColor::black) {
	if ( tip ) setTip(tip);
}

Image::~Image() {
}

void Image::render() {
	con->setDefaultBackground(back);
	con->rect(x,y,w,h,TCOD_BKGND_SET);
}

void Image::setBackgroundColor(const TCODColor col) {
	back=col;
}

void Image::expand(int width, int height) {
	if ( width > w ) w=width;
	if ( height > h ) h=height;
}

