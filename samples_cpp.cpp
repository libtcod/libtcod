#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "libtcod.hpp"

// a sample has a name and a rendering function
typedef struct {
	char name[64];
	void (*render)(bool first, TCOD_key_t*key);
} sample_t;

// sample screen size
#define SAMPLE_SCREEN_WIDTH 46
#define SAMPLE_SCREEN_HEIGHT 20
// sample screen position
#define SAMPLE_SCREEN_X 20
#define SAMPLE_SCREEN_Y 10

// ***************************
// samples rendering functions
// ***************************

// the offscreen console in which the samples are rendered
TCODConsole sampleConsole(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);

// ***************************
// true colors sample
// ***************************
void render_colors(bool first, TCOD_key_t*key) {
	enum { TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT };
	static TCODColor cols[4]={TCODColor(50,40,150),TCODColor(240,85,5),TCODColor(50,35,240),TCODColor(10,200,130)}; // random corner colors
	static int dirr[4]={1,-1,1,1},dirg[4]={1,-1,-1,1},dirb[4]={1,1,1,-1};
	if ( first ) {
		TCODSystem::setFps(0); // unlimited fps
		sampleConsole.clear();
	}
	// ==== slighty modify the corner colors ====
	for (int c=0; c < 4; c++) {
		// move each corner color
		int component=TCODRandom::getInstance()->getInt(0,2);
		switch (component) {
		  case 0 :
		    cols[c].r+=5*dirr[c];
		    if ( cols[c].r == 255 ) dirr[c]=-1;
		    else if (cols[c].r==0) dirr[c]=1;
		  break;
		  case 1 :
		    cols[c].g+=5*dirg[c];
		    if ( cols[c].g == 255 ) dirg[c]=-1;
		    else if (cols[c].g==0) dirg[c]=1;
		  break;
		  case 2 :
		    cols[c].b+=5*dirb[c];
		    if ( cols[c].b == 255 ) dirb[c]=-1;
		    else if (cols[c].b==0) dirb[c]=1;
		  break;
    }
	}

	// ==== scan the whole screen, interpolating corner colors ====
	for (int x=0; x < SAMPLE_SCREEN_WIDTH; x++) {
		float xcoef = (float)(x)/(SAMPLE_SCREEN_WIDTH-1);
		// get the current column top and bottom colors
		TCODColor top = TCODColor::lerp(cols[TOPLEFT], cols[TOPRIGHT],xcoef);
		TCODColor bottom = TCODColor::lerp(cols[BOTTOMLEFT], cols[BOTTOMRIGHT],xcoef);
		for (int y=0; y < SAMPLE_SCREEN_HEIGHT; y++) {
			float ycoef = (float)(y)/(SAMPLE_SCREEN_HEIGHT-1);
			// get the current cell color
			TCODColor curColor = TCODColor::lerp(top,bottom,ycoef);
			sampleConsole.setBack(x,y,curColor,TCOD_BKGND_SET);
		}
	}

	// ==== print the text with a random color ====
	// get the background color at the text position
	TCODColor textColor=sampleConsole.getBack(SAMPLE_SCREEN_WIDTH/2,5);
	// and invert it
	textColor.r=255-textColor.r;
	textColor.g=255-textColor.g;
	textColor.b=255-textColor.b;
	sampleConsole.setForegroundColor(textColor);
	// the background behind the text is slightly darkened using the BKGND_MULTIPLY flag
	sampleConsole.setBackgroundColor(TCODColor::grey);
	sampleConsole.printCenterRect(SAMPLE_SCREEN_WIDTH/2,5,SAMPLE_SCREEN_WIDTH-2,SAMPLE_SCREEN_HEIGHT-1,TCOD_BKGND_MULTIPLY,
		"The Doryen library uses 24 bits colors, for both background and foreground.");
	/* put random text (for performance tests) */
	for (int x=0; x < SAMPLE_SCREEN_WIDTH; x++) {
		for (int y=12; y < SAMPLE_SCREEN_HEIGHT; y++) {
			TCODColor col=sampleConsole.getBack(x,y);
			col=TCODColor::lerp(col,TCODColor::black,0.5f);
			int c=TCODRandom::getInstance()->getInt('a','z');
			sampleConsole.setForegroundColor(col);
			sampleConsole.putChar(x,y,c,TCOD_BKGND_NONE);
		}
	}
}

// ***************************
// offscreen console sample
// ***************************
void render_offscreen(bool first, TCOD_key_t*key) {
	static TCODConsole secondary(SAMPLE_SCREEN_WIDTH/2,SAMPLE_SCREEN_HEIGHT/2); // second screen
	static TCODConsole screenshot(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT); // second screen
	static uint8 alpha=0; // alpha value for the blit operation
	static bool init=false; // draw the secondary screen only the first time
	static int counter=0;
	static int x=0,y=0; // secondary screen position
	static int xdir=1,ydir=1; // movement direction
	if (! init ) {
		init=true;
		secondary.printFrame(0,0,SAMPLE_SCREEN_WIDTH/2,SAMPLE_SCREEN_HEIGHT/2,false,"Offscreen console");
		secondary.printCenterRect(SAMPLE_SCREEN_WIDTH/4,2,SAMPLE_SCREEN_WIDTH/2-2,SAMPLE_SCREEN_HEIGHT/2,
			TCOD_BKGND_NONE,"You can render to an offscreen console and blit in on another one, simulating alpha transparency.");
	}
	if ( first ) {
		TCODSystem::setFps(30); // fps limited to 30
		// get a "screenshot" of the current sample screen
		TCODConsole::blit(&sampleConsole,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT,
							&screenshot,0,0,255);
	}
	counter++;
	if ( counter % 20 == 0 ) {
		// move the secondary screen every 2 seconds
		x+=xdir;y+=ydir;
		if ( x == SAMPLE_SCREEN_WIDTH/2 ) xdir=-1;
		else if ( x == 0 ) xdir=1;
		if ( y == SAMPLE_SCREEN_HEIGHT/2 ) ydir=-1;
		else if ( y == 0 ) ydir=1;
	}
	alpha=(uint8)(255*(1.0f+cosf(TCODSystem::getElapsedSeconds()*2.0f))/2.0f);
	// restore the initial screen
	TCODConsole::blit(&screenshot,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT,
					&sampleConsole,0,0,255);
	// blit the overlapping screen
	TCODConsole::blit(&secondary,0,0,SAMPLE_SCREEN_WIDTH/2,SAMPLE_SCREEN_HEIGHT/2,
					&sampleConsole,x,y,alpha);

}

// ***************************
// line drawing sample
// ***************************
void render_lines(bool first, TCOD_key_t*key) {
	static TCODConsole bk(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT); // colored background
	static int bkFlag=TCOD_BKGND_SET; // current blending mode
	static bool init=false;
	static const char *flagNames[]={
		"TCOD_BKGND_NONE",
		"TCOD_BKGND_SET",
		"TCOD_BKGND_MULTIPLY",
		"TCOD_BKGND_LIGHTEN",
		"TCOD_BKGND_DARKEN",
		"TCOD_BKGND_SCREEN",
		"TCOD_BKGND_COLOR_DODGE",
		"TCOD_BKGND_COLOR_BURN",
		"TCOD_BKGND_ADD",
		"TCOD_BKGND_ADDALPHA",
		"TCOD_BKGND_BURN",
		"TCOD_BKGND_OVERLAY",
		"TCOD_BKGND_ALPHA"
	};
	if ( key->vk == TCODK_ENTER || key->vk == TCODK_KPENTER ) {
		// switch to the next blending mode
		bkFlag++;
		if ( (bkFlag &0xff) > TCOD_BKGND_ALPH) bkFlag=TCOD_BKGND_NONE;
	}
	if ( (bkFlag & 0xff) == TCOD_BKGND_ALPH) {
		// for the alpha mode, update alpha every frame
		float alpha = (1.0f+cosf(TCODSystem::getElapsedSeconds()*2))/2.0f;
		bkFlag=TCOD_BKGND_ALPHA(alpha);
	} else if ( (bkFlag & 0xff) == TCOD_BKGND_ADDA) {
		// for the add alpha mode, update alpha every frame
		float alpha = (1.0f+cosf(TCODSystem::getElapsedSeconds()*2))/2.0f;
		bkFlag=TCOD_BKGND_ADDALPHA(alpha);
	}
	if (!init) {
		// initialize the colored background
		for (int x=0; x < SAMPLE_SCREEN_WIDTH; x ++) {
			for (int y=0; y < SAMPLE_SCREEN_HEIGHT; y++) {
				TCODColor col;
				col.r = (uint8)(x* 255 / (SAMPLE_SCREEN_WIDTH-1));
				col.g = (uint8)((x+y)* 255 / (SAMPLE_SCREEN_WIDTH-1+SAMPLE_SCREEN_HEIGHT-1));
				col.b = (uint8)(y* 255 / (SAMPLE_SCREEN_HEIGHT-1));
				bk.setBack(x,y,col);
			}
		}
		init=true;
	}
	if ( first ) {
		TCODSystem::setFps(30); // fps limited to 30
		sampleConsole.setForegroundColor(TCODColor::white);
	}
	// blit the background
	TCODConsole::blit(&bk,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT,&sampleConsole,0,0);
	// render the gradient
	int recty=(int)((SAMPLE_SCREEN_HEIGHT-2)*((1.0f+cosf(TCOD_sys_elapsed_seconds()))/2.0f));
	for (int x=0;x < SAMPLE_SCREEN_WIDTH; x++) {
		TCODColor col;
		col.r=(uint8)(x*255/SAMPLE_SCREEN_WIDTH);
		col.g=(uint8)(x*255/SAMPLE_SCREEN_WIDTH);
		col.b=(uint8)(x*255/SAMPLE_SCREEN_WIDTH);
		sampleConsole.setBack(x,recty,col,(TCOD_bkgnd_flag_t)bkFlag);
		sampleConsole.setBack(x,recty+1,col,(TCOD_bkgnd_flag_t)bkFlag);
		sampleConsole.setBack(x,recty+2,col,(TCOD_bkgnd_flag_t)bkFlag);
	}
	// calculate the segment ends
	float angle = TCOD_sys_elapsed_seconds()*2.0f;
	float cosAngle=cosf(angle);
	float sinAngle=sinf(angle);
	int xo = (int)(SAMPLE_SCREEN_WIDTH/2*(1 + cosAngle));
	int yo = (int)(SAMPLE_SCREEN_HEIGHT/2 + sinAngle * SAMPLE_SCREEN_WIDTH/2);
	int xd = (int)(SAMPLE_SCREEN_WIDTH/2*(1 - cosAngle));
	int yd = (int)(SAMPLE_SCREEN_HEIGHT/2 - sinAngle * SAMPLE_SCREEN_WIDTH/2);

	// render the line
	int x=xo,y=yo;
	TCODLine::init(x,y,xd,yd);
	do {
		if ( x>= 0 && y >= 0 && x < SAMPLE_SCREEN_WIDTH && y < SAMPLE_SCREEN_HEIGHT) {
			sampleConsole.setBack(x,y,TCODColor::lightBlue,(TCOD_bkgnd_flag_t)bkFlag);
		}
	} while ( ! TCODLine::step(&x,&y));

	// print the current flag
	sampleConsole.printLeft(2,2,TCOD_BKGND_NONE,"%s (ENTER to change)",flagNames[bkFlag&0xff]);
}

// ***************************
// perlin noise sample
// ***************************
void render_noise(bool first, TCOD_key_t*key) {
	enum { NOISE,FBM,TURBULENCE }; // which function we render
	static const char *funcName[]={
		"1 : perlin noise",
		"2 : fbm         ",
		"3 : turbulence  "
	};
	static int func=NOISE;
	static TCODNoise *noise=NULL;
	static float dx=0.0f, dy=0.0f;
	static float octaves=4.0f;
	static float hurst=TCOD_NOISE_DEFAULT_HURST;
	static float lacunarity=TCOD_NOISE_DEFAULT_LACUNARITY;
	static float zoom=3.0f;
	if ( !noise) {
		noise = new TCODNoise(2,hurst,lacunarity);
	}
	if ( first ) {
		TCODSystem::setFps(30); // fps limited to 30
	}
	sampleConsole.clear();
	// texture animation
	dx+=0.01f;
	dy+=0.01f;
	// render the 2d noise function
	for (int y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
		for (int x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
			float f[2];
			f[0] = zoom*x / SAMPLE_SCREEN_WIDTH + dx;
			f[1] = zoom*y / SAMPLE_SCREEN_HEIGHT + dy;
			float value = 0.0f;
			switch (func ) {
				case NOISE : value = noise->getNoise(f); break;
				case FBM : value = noise->getFbm(f,octaves); break;
				case TURBULENCE : value = noise->getTurbulence(f,octaves); break;
			}
			uint8 c=(uint8)((value+1.0f)/2.0f*255);
			// use a bluish color
			TCODColor col((uint8)(c/2),(uint8)(c/2),c);
			sampleConsole.setBack(x,y,col,TCOD_BKGND_SET);
		}
	}
	// draw a transparent rectangle
	sampleConsole.setBackgroundColor(TCODColor::grey);
	sampleConsole.rect(2,2,(func == NOISE ? 16 : 24),(func == NOISE ? 4 : 7),false,TCOD_BKGND_MULTIPLY);
	// draw the text
	for (int curfunc=NOISE; curfunc <= TURBULENCE; curfunc++) {
		if ( curfunc == func ) {
				sampleConsole.setForegroundColor(TCODColor::white);
				sampleConsole.setBackgroundColor(TCODColor::lightBlue);
				sampleConsole.printLeft(2,2+curfunc,TCOD_BKGND_SET,funcName[curfunc]);
		} else {
				sampleConsole.setForegroundColor(TCODColor::grey);
				sampleConsole.printLeft(2,2+curfunc,TCOD_BKGND_NONE,funcName[curfunc]);
		}
	}
	// draw parameters
	sampleConsole.setForegroundColor(TCODColor::white);
	sampleConsole.printLeft(2,5,TCOD_BKGND_NONE,"Y/H : zoom (%2.1f)",zoom);
	if ( func != NOISE ) {
		sampleConsole.printLeft(2,6,TCOD_BKGND_NONE,"E/D : hurst (%2.1f)",hurst);
		sampleConsole.printLeft(2,7,TCOD_BKGND_NONE,"R/F : lacunarity (%2.1f)",lacunarity);
		sampleConsole.printLeft(2,8,TCOD_BKGND_NONE,"T/G : octaves (%2.1f)",octaves);
	}
	// handle keypress
	if ( key->vk == TCODK_NONE) return;
	if ( key->c >= '1' && key->c <= '3') {
		// change the noise function
		func = key->c - '1';
	} else if ( key->c == 'E' || key->c == 'e' ) {
		// increase hurst
		hurst+=0.1f;
		delete noise;
		noise = new TCODNoise(2,hurst,lacunarity);
	} else if ( key->c == 'D' || key->c == 'd' ) {
		// decrease hurst
		hurst-=0.1f;
		delete noise;
		noise = new TCODNoise(2,hurst,lacunarity);
	} else if ( key->c == 'R' || key->c == 'r' ) {
		// increase lacunarity
		lacunarity+=0.5f;
		delete noise;
		noise = new TCODNoise(2,hurst,lacunarity);
	} else if ( key->c == 'F' || key->c == 'f' ) {
		// decrease lacunarity
		lacunarity-=0.5f;
		delete noise;
		noise = new TCODNoise(2,hurst,lacunarity);
	} else if ( key->c == 'T' || key->c == 't' ) {
		// increase octaves
		octaves+=0.5f;
	} else if ( key->c == 'G' || key->c == 'g' ) {
		// decrease octaves
		octaves-=0.5f;
	} else if ( key->c == 'Y' || key->c == 'y' ) {
		// increase zoom
		zoom+=0.2f;
	} else if ( key->c == 'H' || key->c == 'h' ) {
		// decrease zoom
		zoom-=0.2f;
	}
}

// ***************************
// fov sample
// ***************************
void render_fov(bool first, TCOD_key_t*key) {
	static const char *smap[] = {
		"##############################################",
		"#######################      #################",
		"#####################    #     ###############",
		"######################  ###        ###########",
		"##################      #####             ####",
		"################       ########    ###### ####",
		"###############      #################### ####",
		"################    ######                  ##",
		"########   #######  ######   #     #     #  ##",
		"########   ######      ###                  ##",
		"########                                    ##",
		"####       ######      ###   #     #     #  ##",
		"#### ###   ########## ####                  ##",
		"#### ###   ##########   ###########=##########",
		"#### ##################   #####          #####",
		"#### ###             #### #####          #####",
		"####           #     ####                #####",
		"########       #     #### #####          #####",
		"########       #####      ####################",
		"##############################################",
	};
	#define TORCH_RADIUS 10.0f
	#define SQUARED_TORCH_RADIUS (TORCH_RADIUS*TORCH_RADIUS)
	static int px=20,py=10; // player position
	static bool recomputeFov=true; // the player moved. must recompute fov
	static bool torch=false; // torch fx on ?
	static TCODMap *map=NULL;
	static TCODColor darkWall(0,0,100);
	static TCODColor lightWall(130,110,50);
	static TCODColor darkGround(50,50,150);
	static TCODColor lightGround(200,180,50);
	static TCODNoise *noise=NULL;
	static float torchx=0.0f; // torch light position in the perlin noise
	if ( ! map) {
		// initialize the map for the fov toolkit
		map = new TCODMap(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);
		for (int y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
			for (int x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
				if ( smap[y][x] == ' ' ) map->setProperties(x,y,true,true);// ground
				else if ( smap[y][x] == '=' ) map->setProperties(x,y,true,false); // window
			}
		}
		// 1d noise used for the torch flickering
		noise=new TCODNoise(1);
	}
	if ( first ) {
		TCODSystem::setFps(30); // fps limited to 30
		// we draw the foreground only the first time.
		// during the player movement, only the @ is redrawn.
		// the rest impacts only the background color
		// draw the help text & player @
		sampleConsole.clear();
		sampleConsole.setForegroundColor(TCODColor::white);
		sampleConsole.printLeft(1,1,TCOD_BKGND_NONE,"IJKL : move around\nT : torch fx %s",
			torch ? "off" : "on ");
		sampleConsole.setForegroundColor(TCODColor::black);
		sampleConsole.putChar(px,py,'@',TCOD_BKGND_NONE);
		// draw windows
		for (int y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
			for (int x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
				if ( smap[y][x] == '=' ) {
					sampleConsole.putChar(x,y,'=',TCOD_BKGND_NONE);
				}
			}
		}
	}
	if ( recomputeFov ) {
		// calculate the field of view from the player position
		recomputeFov=false;
		map->computeFov(px,py,torch ? (int)(TORCH_RADIUS) : 0);
	}
	// torch position & intensity variation
	float dx=0.0f,dy=0.0f,di=0.0f;
	if ( torch ) {
		// slightly change the perlin noise parameter
		torchx+=0.2f;
		// randomize the light position between -1.5 and 1.5
		float tdx=torchx+20.0f;
		dx = noise->getNoise(&tdx)*1.5f;
		tdx += 30.0f;
		dy = noise->getNoise(&tdx)*1.5f;
		// randomize the light intensity between -0.2 and 0.2
		di = 0.2f * noise->getNoise(&torchx);
	}
	// draw the dungeon
	for (int y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
		for (int x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
			bool visible = map->isInFov(x,y);
			bool wall=smap[y][x]=='#';
			if (! visible ) {
				sampleConsole.setBack(x,y,wall ? darkWall:darkGround,TCOD_BKGND_SET);
			} else {
				if ( !torch ) {
					sampleConsole.setBack(x,y,wall ? lightWall : lightGround, TCOD_BKGND_SET );
				} else {
					// torch flickering fx
					TCODColor base=(wall ? darkWall : darkGround);
					TCODColor light=(wall ? lightWall : lightGround);
					// cell distance to torch (squared)
					float r=(float)((x-px+dx)*(x-px+dx)+(y-py+dy)*(y-py+dy));
					if ( r < SQUARED_TORCH_RADIUS ) {
						// l = 1.0 at player position, 0.0 at a radius of 10 cells
						float l = (SQUARED_TORCH_RADIUS-r)/SQUARED_TORCH_RADIUS +di;
						// clamp between 0 and 1
						if (l  < 0.0f ) l=0.0f;
						else if ( l> 1.0f ) l =1.0f;
						// interpolate the color
						base.r = (uint8)(base.r + (light.r-base.r)*l);
						base.g = (uint8)(base.g + (light.g-base.g)*l);
						base.b = (uint8)(base.b + (light.b-base.b)*l);
					}
					sampleConsole.setBack(x,y,base,TCOD_BKGND_SET);
				}
			}
		}
	}
	if ( key->c == 'I' || key->c == 'i' ) {
		// player move north
		if ( smap[py-1][px] == ' ' ) {
			sampleConsole.putChar(px,py,' ',TCOD_BKGND_NONE);
			py--;
			sampleConsole.putChar(px,py,'@',TCOD_BKGND_NONE);
			recomputeFov=true;
		}
	} else if ( key->c == 'K' || key->c == 'k' ) {
		// player move south
		if ( smap[py+1][px] == ' ' ) {
			sampleConsole.putChar(px,py,' ',TCOD_BKGND_NONE);
			py++;
			sampleConsole.putChar(px,py,'@',TCOD_BKGND_NONE);
			recomputeFov=true;
		}
	} else if ( key->c == 'J' || key->c == 'j' ) {
		// player move west
		if ( smap[py][px-1] == ' ' ) {
			sampleConsole.putChar(px,py,' ',TCOD_BKGND_NONE);
			px--;
			sampleConsole.putChar(px,py,'@',TCOD_BKGND_NONE);
			recomputeFov=true;
		}
	} else if ( key->c == 'L' || key->c == 'l' ) {
		// player move east
		if ( smap[py][px+1] == ' ' ) {
			sampleConsole.putChar(px,py,' ',TCOD_BKGND_NONE);
			px++;
			sampleConsole.putChar(px,py,'@',TCOD_BKGND_NONE);
			recomputeFov=true;
		}
	} else if ( key->c == 'T' || key->c == 't' ) {
		// enable/disable the torch fx
		torch=!torch;
		sampleConsole.setForegroundColor(TCODColor::white);
		sampleConsole.printLeft(1,1,TCOD_BKGND_NONE,"IJKL : move around\nT : torch fx %s",
			torch ? "off" : "on ");
		sampleConsole.setForegroundColor(TCODColor::black);
	}
}

// ***************************
// image sample
// ***************************
void render_image(bool first, TCOD_key_t*key) {
	static TCODImage * img=NULL, *circle = NULL;
	static TCODColor blue(0,0,255);
	static TCODColor green(0,255,0);
	if ( img == NULL ) {
		img=new TCODImage("skull.bmp");
		circle=new TCODImage("circle.bmp");
	}
	if ( first ) {
		TCODSystem::setFps(30); // fps limited to 30
	}
	sampleConsole.setBackgroundColor(TCODColor::black);
	sampleConsole.clear();
	float x=SAMPLE_SCREEN_WIDTH/2+cosf(TCOD_sys_elapsed_seconds())*10.0f;
	float y=(float)(SAMPLE_SCREEN_HEIGHT/2);
	float scalex=0.2f+1.8f*(1.0f+cosf(TCOD_sys_elapsed_seconds()/2))/2.0f;
	float scaley=scalex;
	float angle=TCODSystem::getElapsedSeconds();
	long elapsed=TCODSystem::getElapsedMilli()/2000;
	if ( elapsed & 1 ) {
		// split the color channels of circle.bmp
		// the red channel
		sampleConsole.setBackgroundColor(TCODColor::red);
		sampleConsole.rect(0,3,15,15,false,TCOD_BKGND_SET);
		circle->blitRect(&sampleConsole,0,3,-1,-1,TCOD_BKGND_MULTIPLY);
		// the green channel
		sampleConsole.setBackgroundColor(green);
		sampleConsole.rect(15,3,15,15,false,TCOD_BKGND_SET);
		circle->blitRect(&sampleConsole,15,3,-1,-1,TCOD_BKGND_MULTIPLY);
		// the blue channel
		sampleConsole.setBackgroundColor(blue);
		sampleConsole.rect(30,3,15,15,false,TCOD_BKGND_SET);
		circle->blitRect(&sampleConsole,30,3,-1,-1,TCOD_BKGND_MULTIPLY);
	} else {
		// render circle.bmp with normal blitting
		circle->blitRect(&sampleConsole,0,3,-1,-1,TCOD_BKGND_SET);
		circle->blitRect(&sampleConsole,15,3,-1,-1,TCOD_BKGND_SET);
		circle->blitRect(&sampleConsole,30,3,-1,-1,TCOD_BKGND_SET);
	}	
	img->blit(&sampleConsole,x,y,
		TCOD_BKGND_ADDALPHA(0.6f),scalex,scaley,angle);
}

// ***************************
// mouse sample
// ***************************/
void render_mouse(bool first, TCOD_key_t*key) {
  TCOD_mouse_t mouse;
  static bool lbut=false,rbut=false,mbut=false;
  
  if ( first ) {
    sampleConsole.setBackgroundColor(TCODColor::grey);
    sampleConsole.setForegroundColor(TCODColor::lightYellow);
    TCODMouse::move(320,200);
    TCODMouse::showCursor(true);
  }
  
  sampleConsole.clear();
  mouse=TCODMouse::getStatus();
  if ( mouse.lbutton_pressed ) lbut=!lbut;
  if ( mouse.rbutton_pressed ) rbut=!rbut;
  if ( mouse.mbutton_pressed ) mbut=!mbut;
  sampleConsole.printLeft(1,1,TCOD_BKGND_NONE,
    "Mouse position : %4dx%4d\n"
    "Mouse cell     : %4dx%4d\n"
    "Mouse movement : %4dx%4d\n"
    "Left button    : %s (toggle %s)\n"
    "Right button   : %s (toggle %s)\n"
    "Middle button  : %s (toggle %s)\n",
    mouse.x,mouse.y,
    mouse.cx,mouse.cy,
    mouse.dx,mouse.dy,
    mouse.lbutton ? " ON" : "OFF",lbut ? " ON" : "OFF",
    mouse.rbutton ? " ON" : "OFF",rbut ? " ON" : "OFF",
    mouse.mbutton ? " ON" : "OFF",mbut ? " ON" : "OFF");
  sampleConsole.printLeft(1,10,TCOD_BKGND_NONE,"1 : Hide cursor\n2 : Show cursor");
  if (key->c == '1') TCODMouse::showCursor(false);
  else if( key->c == '2' ) TCODMouse::showCursor(true);
}

// ***************************
// the list of samples
// ***************************
sample_t samples[] = {
	{"  True colors        ",render_colors},
	{"  Offscreen console  ",render_offscreen},
	{"  Line drawing       ",render_lines},
	{"  Perlin noise       ",render_noise},
	{"  Field of view      ",render_fov},
	{"  Image toolkit      ",render_image},
	{"  Mouse support      ",render_mouse},
};
int nbSamples = sizeof(samples)/sizeof(sample_t); // total number of samples

// ***************************
// the main function
// ***************************
int main( int argc, char *argv[] ) {
	int curSample=0; // index of the current sample
	bool first=true; // first time we render a sample
	TCOD_key_t key = {TCODK_NONE,0};
	const char *font="terminal.bmp";
	int charWidth=8,charHeight=8,nbCharH=16,nbCharV=16;
	int argn;
	int fullscreenWidth=0;
	int fullscreenHeight=0;
	bool fullscreen=false;
	bool inRow=false;
	TCODColor keyColor=TCODColor::black;

	// initialize the root console (open the game window)
	for (argn=1; argn < argc; argn++) {
		if ( strcmp(argv[argn],"-font") == 0 && argn+1 < argc) {
			argn++;
			font=argv[argn];
		} else if ( strcmp(argv[argn],"-font-char-size") == 0 && argn+2 < argc ) {
			argn++;
			charWidth=atoi(argv[argn]);
			argn++;
			charHeight=atoi(argv[argn]);
		} else if ( strcmp(argv[argn],"-font-layout") == 0 && argn+2 < argc ) {
			argn++;
			nbCharH=atoi(argv[argn]);
			argn++;
			nbCharV=atoi(argv[argn]);
		} else if ( strcmp(argv[argn],"-fullscreen-resolution") == 0 && argn+2 < argc ) {
			argn++;
			fullscreenWidth=atoi(argv[argn]);
			argn++;
			fullscreenHeight=atoi(argv[argn]);
		} else if ( strcmp(argv[argn],"-fullscreen") == 0 ) {
			fullscreen=true;
		} else if ( strcmp(argv[argn],"-font-in-row") == 0 ) {
			inRow=true;
		} else if ( strcmp(argv[argn],"-font-key-color") == 0 && argn+3 < argc) {
			argn++;
			keyColor.r = (uint8)atol(argv[argn++]);
			keyColor.g = (uint8)atol(argv[argn++]);
			keyColor.b = (uint8)atol(argv[argn]);
		} else {
			argn++; // ignore parameter
		}
	}

	TCODConsole::setCustomFont(font,charWidth,charHeight,nbCharH,nbCharV,inRow,keyColor);
	if ( fullscreenWidth > 0 ) {
		TCODSystem::forceFullscreenResolution(fullscreenWidth,fullscreenHeight);
	}

	TCODConsole::initRoot(80,50,"libtcod C++ sample",fullscreen);
	do {
		// print the list of samples
		for (int i=0; i < nbSamples; i++ ) {
			if ( i == curSample ) {
				// set colors for currently selected sample
				TCODConsole::root->setForegroundColor(TCODColor::white);
				TCODConsole::root->setBackgroundColor(TCODColor::lightBlue);
			} else {
				// set colors for other samples
				TCODConsole::root->setForegroundColor(TCODColor::grey);
				TCODConsole::root->setBackgroundColor(TCODColor::black);
			}
			// print the sample name
			TCODConsole::root->printLeft(2,47-(nbSamples-i)*2,TCOD_BKGND_SET,samples[i].name);
		}
		// print the help message
		TCODConsole::root->setForegroundColor(TCODColor::grey);
		TCODConsole::root->printRight(79,46,TCOD_BKGND_NONE,"last frame : %3d ms (%3d fps)", (int)(TCODSystem::getLastFrameLength()*1000), TCODSystem::getFps());
		TCODConsole::root->printRight(79,47,TCOD_BKGND_NONE,"elapsed : %8dms %4.2fs", TCODSystem::getElapsedMilli(),TCODSystem::getElapsedSeconds());
		TCODConsole::root->printLeft(2,47,TCOD_BKGND_NONE,"%c%c : select a sample", TCOD_CHAR_ARROW_N, TCOD_CHAR_ARROW_S);
		TCODConsole::root->printLeft(2,48,TCOD_BKGND_NONE,"ALT-ENTER : switch to %s",
			TCODConsole::isFullscreen() ? "windowed mode  " : "fullscreen mode");

		// render current sample
		samples[curSample].render(first,&key);
		first=false;

		// blit the sample console on the root console
		TCODConsole::blit(&sampleConsole,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT, // the source console & zone to blit
							TCODConsole::root,SAMPLE_SCREEN_X,SAMPLE_SCREEN_Y, // the destination console & position
							255 // alpha coef
						 );
		// update the game screen
		TCODConsole::flush();

		// did the user hit a key ?
		key = TCODConsole::checkForKeypress();
		if ( key.vk == TCODK_DOWN ) {
			// down arrow : next sample
			curSample = (curSample+1) % nbSamples;
			first=true;
		} else if ( key.vk == TCODK_UP ) {
			// up arrow : previous sample
			curSample--;
			if ( curSample < 0 ) curSample = nbSamples-1;
			first=true;
		} else if ( key.vk == TCODK_ENTER && key.lalt ) {
			// ALT-ENTER : switch fullscreen
			TCODConsole::setFullscreen(!TCODConsole::isFullscreen());
		}
	} while (!TCODConsole::isWindowClosed());
	return 0;
}
