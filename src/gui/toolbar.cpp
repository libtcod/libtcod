#include <string.h>
#include "libtcod.hpp"
#include "gui.hpp"

class Separator : public Widget {
public :
	Separator(const char *txt, const char *tip=NULL) : Widget(0,0,0,1),txt(NULL) {
		if ( txt ) {
			this->txt=TCOD_strdup(txt);
		}
		if ( tip ) setTip(tip);
	}
	virtual ~Separator() {
		if ( txt ) {
			free(txt);
		}
	}
	void computeSize() {
		w = (txt ? (int)strlen(txt)+2 : 0 );
	}
	void expand(int width, int height) {
		if ( w < width ) w=width;
	}
	void render() {
		con->setDefaultBackground(back);
		con->setDefaultForeground(fore);
		con->hline(x,y,w, TCOD_BKGND_SET);
		con->setChar(x-1,y,TCOD_CHAR_TEEE);
		con->setChar(x+w,y,TCOD_CHAR_TEEW);
		con->setDefaultBackground(fore);
		con->setDefaultForeground(back);
		con->printEx(x+w/2,y,TCOD_BKGND_SET,TCOD_CENTER," %s ",txt);
	}
	char *txt;
};

ToolBar::ToolBar(int x, int y, const char *name, const char *tip) 
	: Container(x,y,0,2),name(NULL),fixedWidth(0) {
	if ( name ) {
		this->name = TCOD_strdup(name);
		w = strlen(name)+4;
	}
	if ( tip ) setTip(tip);
}

ToolBar::ToolBar(int x, int y, int w, const char *name, const char *tip) 
	: Container(x,y,w,2),name(NULL),fixedWidth(w) {
	if ( name ) {
		this->name = TCOD_strdup(name);
		fixedWidth = w = MAX((int)strlen(name)+4,w);
	}
	if ( tip ) setTip(tip);
}

ToolBar::~ToolBar() {
	if ( name ) free(name);
}

void ToolBar::setName(const char *name) {
	if ( this->name ) free(this->name);
	if ( name ) {
		this->name = TCOD_strdup(name);
		fixedWidth = MAX((int)strlen(name)+4,fixedWidth);
	} else {
		name=NULL;
	}
}

void ToolBar::render() {
	con->setDefaultBackground(back);
	con->setDefaultForeground(fore);
	con->printFrame(x,y,w,h,true,TCOD_BKGND_SET,name);
	Container::render();
}

void ToolBar::computeSize() {
	int cury=y+1;
	w=name ? strlen(name)+4 : 2;
	for (Widget **wid=content.begin(); wid != content.end(); wid ++ ) {
		if ( (*wid)->isVisible() ) {
			(*wid)->x=x+1;
			(*wid)->y=cury;
			(*wid)->computeSize();
			if ((*wid)->w +2 > w) w=(*wid)->w + 2;
			cury+=(*wid)->h;
		}
	}
	if ( w < fixedWidth ) w=fixedWidth;
	h=cury-y+1;
	for (Widget **wid=content.begin(); wid != content.end(); wid ++ ) {
		if ( (*wid)->isVisible() ) {
			(*wid)->expand(w-2,(*wid)->h);
		}
	}	
}
void ToolBar::addSeparator(const char *txt, const char *tip) {
	addWidget(new Separator(txt,tip));
}

