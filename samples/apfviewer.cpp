#include <stdio.h>
#include "libtcod.hpp"

static float version=1.0f;

int main(int argc, char *argv[]) {
	TCODConsole * rootcon = NULL;
	TCOD_key_t keyPress;
	TCOD_renderer_t renderer = TCOD_RENDERER_SDL;
	
	int row=50;
	int col=80;
	if ( argc < 2) {
		printf ("apfviewer <file1> <file2> ...\nversion %g\n",version);
		return 1;
	}
	TCODConsole::initRoot(col,row,"apfviewer",false,renderer );
	TCODConsole con(1,1);
	for (int i=1; i < argc; i++) {
		if (strstr(argv[i],".asc")) con.loadAsc(argv[1]);
		else con.loadApf(argv[i]); 
		delete TCODConsole::root;
		TCODConsole::root=NULL;
		TCODConsole::initRoot(con.getWidth(),con.getHeight(),argv[i],false,renderer );		
		TCODConsole::blit(&con,0,0,0,0,TCODConsole::root,0,0);
		TCODConsole::root->flush();
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_RELEASE|TCOD_EVENT_MOUSE_PRESS,NULL,NULL,true);
	}
	
	return 0;
}      
