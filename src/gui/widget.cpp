#include <string.h>
#include "libtcod.hpp"
#include "gui.hpp"

TCODColor Widget::back=TCODColor(40,40,120);
TCODColor Widget::fore=TCODColor(220,220,180);
TCODColor Widget::backFocus=TCODColor(70,70,130);
TCODColor Widget::foreFocus=TCODColor(255,255,255);
TCODConsole *Widget::con=NULL;
TCODList<Widget *> Widget::widgets;
TCOD_mouse_t Widget::mouse;
float Widget::elapsed;
Widget *Widget::focus=NULL;
Widget *Widget::keyboardFocus=NULL;

Widget::Widget() : x(0),y(0),w(0),h(0),tip(NULL),mouseIn(false),mouseL(false),visible(true) {
	widgets.push(this);
}

Widget::Widget(int x,int y) : x(x),y(y),w(0),h(0),tip(NULL),mouseIn(false),mouseL(false),visible(true) {
	widgets.push(this);
}

Widget::Widget(int x,int y, int w, int h) : x(x),y(y),w(w),h(h),tip(NULL),mouseIn(false),mouseL(false),visible(true) {
	widgets.push(this);
}

Widget::~Widget() {
	if ( tip ) free(tip);
	if ( focus == this ) focus=NULL;
	widgets.remove(this);
}

void Widget::setBackgroundColor(const TCODColor col,const TCODColor colFocus) {
	back=col;
	backFocus=colFocus;
}

void Widget::setForegroundColor(const TCODColor col,const TCODColor colFocus) {
	fore=col;
	foreFocus=colFocus;
}

void Widget::setConsole(TCODConsole *console) {
	con=console;
}

void Widget::update(const TCOD_key_t k) {
	bool curs=TCODMouse::isCursorVisible();
	if ( curs ) {
		if ( mouse.cx >= x && mouse.cx < x+w && mouse.cy >= y && mouse.cy < y+h ) {
			if ( ! mouseIn ) {
				mouseIn = true;
				onMouseIn();
			}
			if ( focus != this ) {
				focus=this;
			}
		} else {
			if ( mouseIn ) {
				mouseIn = false;
				onMouseOut();
			}
			mouseL=false;
			if ( this == focus ) {
				focus = NULL;
			}
		}
	}
	if ( mouseIn || (! curs && this == focus ) ) {
		if ( mouse.lbutton && ! mouseL ) {
			mouseL = true;
			onButtonPress();
		} else if (! mouse.lbutton && mouseL ) {
			onButtonRelease();
			keyboardFocus=NULL;
			if ( mouseL ) onButtonClick();
			mouseL=false;
		} else if ( mouse.lbutton_pressed ) {
			keyboardFocus=NULL;
			onButtonClick();
		}
	}
}

void Widget::updateWidgetsIntern(const TCOD_key_t k) {
	elapsed=TCODSystem::getLastFrameLength();
	for (Widget **w=widgets.begin(); w!= widgets.end(); w++) {
		if ( (*w)->isVisible() ) {
			(*w)->computeSize();
			(*w)->update(k);
		}
	}
}

void Widget::updateWidgets(const TCOD_key_t k,const TCOD_mouse_t pmouse) {
	mouse=pmouse;
	updateWidgetsIntern(k);
}

void Widget::renderWidgets() {
	if (!con) con=TCODConsole::root;
	for (Widget **w=widgets.begin(); w!= widgets.end(); w++) {
		if ((*w)->isVisible()) (*w)->render();
	}
}

void Widget::move(int x,int y) {
	this->x=x;
	this->y=y;
}

void Widget::setTip(const char *tip) {
	if ( this->tip ) free(this->tip);
	this->tip = TCOD_strdup(tip);
}

