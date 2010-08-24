#include <stdio.h>
#include <string.h>
#include <math.h>
#include "libtcod.hpp"
#include "gui.hpp"

Slider::Slider(int x,int y,int w, float min, float max, const char *label, const char *tip)
	: TextBox(x,y,w,10,label,NULL,tip),min(min),max(max),value((min+max)*0.5f),sensitivity(1.0f),
	onArrows(false),drag(false),fmt(NULL),cbk(NULL),data(NULL) {
	valueToText();
	this->w+=2;
}

Slider::~Slider() {
	if ( fmt ) free(fmt);
}

void Slider::setFormat(const char *fmt) {
	if ( this->fmt ) free(this->fmt);
	if ( fmt ) this->fmt = TCOD_strdup(fmt);
	else fmt=NULL;
	valueToText();
}

void Slider::render() {
	w-=2;
	TextBox::render();
	w+=2;
	con->setDefaultBackground((onArrows || drag) ? backFocus : back);
	con->setDefaultForeground((onArrows || drag) ? foreFocus : fore);
	con->rect(x+w-2,y,2,1,TCOD_BKGND_SET);
	con->setChar(x+w-2,y,TCOD_CHAR_ARROW_W);
	con->setChar(x+w-1,y,TCOD_CHAR_ARROW_E);
}

void Slider::update(TCOD_key_t k) {
	float oldValue=value;
	TextBox::update(k);
	textToValue();
	if ( mouse.cx >= x+w-2 && mouse.cx < x+w && mouse.cy == y ) onArrows=true;
	else onArrows=false;
	if ( drag ) {
		if ( dragy == -1 ) {
			dragx = mouse.x;
			dragy = mouse.y;
		} else {
			float mdx = (float)((mouse.x-dragx)*sensitivity) / (con->getWidth()*8);
			float mdy = (float)((mouse.y-dragy)*sensitivity) / (con->getHeight()*8);
			float oldValue=value;
			if ( fabs(mdy) > fabs(mdx) ) mdx=-mdy;
			value = dragValue+(max-min)*mdx;
			value=CLAMP(min,max,value);
			if ( value != oldValue ) {
				valueToText();
				textToValue();
			}
		}
	}
	if ( value != oldValue && cbk ) {
		cbk(this,value,data);
	}
}

void Slider::valueToText() {
	char tmp[128];
	sprintf(tmp, fmt ? fmt : "%.2f",value);
	setText(tmp);
}

void Slider::textToValue() {
#ifdef TCOD_VISUAL_STUDIO
    value=(float)atof(txt);
#else
	char *endptr;
	float f=strtof(txt,&endptr);
	if ( f != 0.0f || endptr != txt ) value=f;
#endif
}

void Slider::setValue(float value) {
	this->value=CLAMP(min,max,value);
	valueToText();
}

void Slider::onButtonPress() {
	if ( onArrows ) {
		drag=true;
		dragy=-1;
		dragValue=value;
		TCODMouse::showCursor(false);
	}
}

void Slider::onButtonRelease() {
	if ( drag ) {
		drag=false;
		TCODMouse::move((x+w-2)*8,y*8);
		TCODMouse::showCursor(true);
	}
}

