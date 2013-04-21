/*
* libtcod 1.6.0
* Copyright (c) 2008,2009,2010,2012,2013 Jice & Mingos
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice or Mingos may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY JICE AND MINGOS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE OR MINGOS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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

