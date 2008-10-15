#include "libtcod.hpp"
#include "gui.hpp"

RadioButton *RadioButton::groupSelect[512];
int RadioButton::defaultGroup=0;
static bool init=false;

void RadioButton::select() {
	if (!init) {
		memset(groupSelect,0,sizeof(RadioButton *)*512);
		init=true;
	}
	groupSelect[group]=this;
}

void RadioButton::unSelect() {
	if (!init) {
		memset(groupSelect,0,sizeof(RadioButton *)*512);
		init=true;
	}
	groupSelect[group]=NULL;
}

void RadioButton::unSelectGroup(int group) {
	groupSelect[group]=NULL;
}

void RadioButton::render() {
	Button::render();
	if ( groupSelect[group] == this ) {
		con->setChar(x,y,'>');
	}
}

void RadioButton::onButtonClick() {
	select();
	Button::onButtonClick();
}

