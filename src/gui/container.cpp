#include "libtcod.hpp"
#include "gui.hpp"

Container::~Container() { 
	content.clearAndDelete(); 
}

void Container::addWidget(Widget *wid) { 
	content.push(wid);
	widgets.remove(wid);
}

void Container::removeWidget(Widget *wid) {
	content.remove(wid);
}

void Container::setVisible(bool val) {
	Widget::setVisible(val);
}

void Container::clear() {
	content.clearAndDelete();
}

void Container::render() {
	for ( Widget **wid=content.begin(); wid != content.end(); wid ++ ) {
		if ( (*wid)->isVisible() ) (*wid)->render();
	}
}

void Container::update(const TCOD_key_t k) {
	Widget::update(k);
	for ( Widget **wid=content.begin(); wid != content.end(); wid ++ ) {
		if ( (*wid)->isVisible() ) (*wid)->update(k);
	}
}

