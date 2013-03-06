/*
 * libtcod CPP samples
 * This code demonstrates various usages of libtcod modules
 * It's in the public domain.
 */

// uncomment this to disable SDL sample (might cause compilation issues on some systems)
//#define NO_SDL_SAMPLE

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "libtcod.hpp"
#define _SDL_main_h
#if defined (__HAIKU__) || defined (__ANDROID__)
#include <SDL.h>
#elif defined (TCOD_SDL2)
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#endif


// a sample has a name and a rendering function
typedef struct {
	char name[64];
	void (*render)(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse);
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
void render_colors(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
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
			sampleConsole.setCharBackground(x,y,curColor,TCOD_BKGND_SET);
		}
	}

	// ==== print the text with a random color ====
	// get the background color at the text position
	TCODColor textColor=sampleConsole.getCharBackground(SAMPLE_SCREEN_WIDTH/2,5);
	// and invert it
	textColor.r=255-textColor.r;
	textColor.g=255-textColor.g;
	textColor.b=255-textColor.b;
	// put random text (for performance tests) 
	for (int x=0; x < SAMPLE_SCREEN_WIDTH; x++) {
		for (int y=0; y < SAMPLE_SCREEN_HEIGHT; y++) {
			int c;
			TCODColor col=sampleConsole.getCharBackground(x,y);
			col=TCODColor::lerp(col,TCODColor::black,0.5f);
			// use colored character 255 on first and last lines
			if ( y == 0 || y == SAMPLE_SCREEN_HEIGHT-1) {
				c=255;
			} else {
				c=TCODRandom::getInstance()->getInt('a','z');
			}
			
			sampleConsole.setDefaultForeground(col);
			sampleConsole.putChar(x,y,c,TCOD_BKGND_NONE);
		}
	}
	sampleConsole.setDefaultForeground(textColor);
	// the background behind the text is slightly darkened using the BKGND_MULTIPLY flag
	sampleConsole.setDefaultBackground(TCODColor::grey);
	sampleConsole.printRectEx(SAMPLE_SCREEN_WIDTH/2,5,SAMPLE_SCREEN_WIDTH-2,SAMPLE_SCREEN_HEIGHT-1,
		TCOD_BKGND_MULTIPLY,TCOD_CENTER,
		"The Doryen library uses 24 bits colors, for both background and foreground.");
}

// ***************************
// offscreen console sample
// ***************************
void render_offscreen(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
	static TCODConsole secondary(SAMPLE_SCREEN_WIDTH/2,SAMPLE_SCREEN_HEIGHT/2); // second screen
	static TCODConsole screenshot(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT); // second screen
	static bool init=false; // draw the secondary screen only the first time
	static int counter=0;
	static int x=0,y=0; // secondary screen position
	static int xdir=1,ydir=1; // movement direction
	if (! init ) {
		init=true;
		secondary.printFrame(0,0,SAMPLE_SCREEN_WIDTH/2,SAMPLE_SCREEN_HEIGHT/2,false,TCOD_BKGND_SET,"Offscreen console");
		secondary.printRectEx(SAMPLE_SCREEN_WIDTH/4,2,SAMPLE_SCREEN_WIDTH/2-2,SAMPLE_SCREEN_HEIGHT/2,
			TCOD_BKGND_NONE,TCOD_CENTER,"You can render to an offscreen console and blit in on another one, simulating alpha transparency.");
	}
	if ( first ) {
		TCODSystem::setFps(30); // fps limited to 30
		// get a "screenshot" of the current sample screen
		TCODConsole::blit(&sampleConsole,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT,
							&screenshot,0,0);
	}
	counter++;
	if ( counter % 20 == 0 ) {
		// move the secondary screen every 2 seconds
		x+=xdir;y+=ydir;
		if ( x == SAMPLE_SCREEN_WIDTH/2+5 ) xdir=-1;
		else if ( x == -5 ) xdir=1;
		if ( y == SAMPLE_SCREEN_HEIGHT/2+5 ) ydir=-1;
		else if ( y == -5 ) ydir=1;
	}
	// restore the initial screen
	TCODConsole::blit(&screenshot,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT,
					&sampleConsole,0,0);
	// blit the overlapping screen
	TCODConsole::blit(&secondary,0,0,SAMPLE_SCREEN_WIDTH/2,SAMPLE_SCREEN_HEIGHT/2,
					&sampleConsole,x,y,1.0f,0.75f);

}

// ***************************
// line drawing sample
// ***************************
static int bkFlag=TCOD_BKGND_SET; // current blending mode
class LineListener : public TCODLineListener {
	public :
		bool putPoint(int x, int y) {
			if ( x>= 0 && y >= 0 && x < SAMPLE_SCREEN_WIDTH && y < SAMPLE_SCREEN_HEIGHT) {
				sampleConsole.setCharBackground(x,y,TCODColor::lightBlue,(TCOD_bkgnd_flag_t)bkFlag);
			}
			return true;
		}
};
void render_lines(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
	static TCODConsole bk(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT); // colored background
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
				bk.setCharBackground(x,y,col, TCOD_BKGND_SET);
			}
		}
		init=true;
	}
	if ( first ) {
		TCODSystem::setFps(30); // fps limited to 30
		sampleConsole.setDefaultForeground(TCODColor::white);
	}
	// blit the background
	TCODConsole::blit(&bk,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT,&sampleConsole,0,0);
	// render the gradient
	int recty=(int)((SAMPLE_SCREEN_HEIGHT-2)*((1.0f+cosf(TCODSystem::getElapsedSeconds()))/2.0f));
	for (int x=0;x < SAMPLE_SCREEN_WIDTH; x++) {
		TCODColor col;
		col.r=(uint8)(x*255/SAMPLE_SCREEN_WIDTH);
		col.g=(uint8)(x*255/SAMPLE_SCREEN_WIDTH);
		col.b=(uint8)(x*255/SAMPLE_SCREEN_WIDTH);
		sampleConsole.setCharBackground(x,recty,col,(TCOD_bkgnd_flag_t)bkFlag);
		sampleConsole.setCharBackground(x,recty+1,col,(TCOD_bkgnd_flag_t)bkFlag);
		sampleConsole.setCharBackground(x,recty+2,col,(TCOD_bkgnd_flag_t)bkFlag);
	}
	// calculate the segment ends
	float angle = TCODSystem::getElapsedSeconds()*2.0f;
	float cosAngle=cosf(angle);
	float sinAngle=sinf(angle);
	int xo = (int)(SAMPLE_SCREEN_WIDTH/2*(1 + cosAngle));
	int yo = (int)(SAMPLE_SCREEN_HEIGHT/2 + sinAngle * SAMPLE_SCREEN_WIDTH/2);
	int xd = (int)(SAMPLE_SCREEN_WIDTH/2*(1 - cosAngle));
	int yd = (int)(SAMPLE_SCREEN_HEIGHT/2 - sinAngle * SAMPLE_SCREEN_WIDTH/2);

	// render the line
	LineListener listener;
	TCODLine::line(xo,yo,xd,yd,&listener);
	// print the current flag
	sampleConsole.print(2,2,"%s (ENTER to change)",flagNames[bkFlag&0xff]);
}

// ***************************
// noise sample
// ***************************
void render_noise(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
	enum { PERLIN,SIMPLEX,WAVELET,
		FBM_PERLIN,TURBULENCE_PERLIN,
		FBM_SIMPLEX,TURBULENCE_SIMPLEX,
		FBM_WAVELET,TURBULENCE_WAVELET }; // which function we render
	static const char *funcName[]={
		"1 : perlin noise       ",
		"2 : simplex noise      ",
		"3 : wavelet noise      ",
		"4 : perlin fbm         ",
		"5 : perlin turbulence  ",
		"6 : simplex fbm        ",
		"7 : simplex turbulence ",
		"8 : wavelet fbm        ",
		"9 : wavelet turbulence ",
	};
	static int func=PERLIN;
	static TCODNoise *noise=NULL;
	static float dx=0.0f, dy=0.0f;
	static float octaves=4.0f;
	static float hurst=TCOD_NOISE_DEFAULT_HURST;
	static float lacunarity=TCOD_NOISE_DEFAULT_LACUNARITY;
	static TCODImage *img=NULL;
	static float zoom=3.0f;
	if ( !noise) {
		noise = new TCODNoise(2,hurst,lacunarity);
		img = new TCODImage(SAMPLE_SCREEN_WIDTH*2,SAMPLE_SCREEN_HEIGHT*2);
	}
	if ( first ) {
		TCODSystem::setFps(30); // fps limited to 30
	}

	// texture animation
	dx+=0.01f;
	dy+=0.01f;
	// render the 2d noise function
	for (int y=0; y < 2*SAMPLE_SCREEN_HEIGHT; y++ ) {
		for (int x=0; x < 2*SAMPLE_SCREEN_WIDTH; x++ ) {
			float f[2];
			f[0] = zoom*x / (2*SAMPLE_SCREEN_WIDTH) + dx;
			f[1] = zoom*y / (2*SAMPLE_SCREEN_HEIGHT) + dy;
			float value = 0.0f;
			switch (func ) {
				case PERLIN : value = noise->get(f,TCOD_NOISE_PERLIN); break;
				case SIMPLEX : value = noise->get(f,TCOD_NOISE_SIMPLEX); break;
				case WAVELET : value = noise->get(f,TCOD_NOISE_WAVELET); break;
				case FBM_PERLIN : value = noise->getFbm(f,octaves,TCOD_NOISE_PERLIN); break;
				case TURBULENCE_PERLIN : value = noise->getTurbulence(f,octaves,TCOD_NOISE_PERLIN); break;
				case FBM_SIMPLEX : value = noise->getFbm(f,octaves,TCOD_NOISE_SIMPLEX); break;
				case TURBULENCE_SIMPLEX : value = noise->getTurbulence(f,octaves,TCOD_NOISE_SIMPLEX); break;
				case FBM_WAVELET : value = noise->getFbm(f,octaves,TCOD_NOISE_WAVELET); break;
				case TURBULENCE_WAVELET : value = noise->getTurbulence(f,octaves,TCOD_NOISE_WAVELET); break;
			}
			uint8 c=(uint8)((value+1.0f)/2.0f*255);
			// use a bluish color
			TCODColor col((uint8)(c/2),(uint8)(c/2),c);
			img->putPixel(x,y,col);
		}
	}
	// blit the noise image on the console with subcell resolution
	img->blit2x(&sampleConsole,0,0);
	// draw a transparent rectangle
	sampleConsole.setDefaultBackground(TCODColor::grey);
	sampleConsole.rect(2,2,23,(func <= WAVELET ? 10 : 13),false,TCOD_BKGND_MULTIPLY);
	for (int y=2; y < 2+(func <= WAVELET ? 10 : 13); y++ ) {
		for (int x=2; x < 2+23; x++ ) {
			TCODColor col=sampleConsole.getCharForeground(x,y);
			col = col * TCODColor::grey;
			sampleConsole.setCharForeground(x,y,col);
		}
	}

	// draw the text
	for (int curfunc=PERLIN; curfunc <= TURBULENCE_WAVELET; curfunc++) {
		if ( curfunc == func ) {
				sampleConsole.setDefaultForeground(TCODColor::white);
				sampleConsole.setDefaultBackground(TCODColor::lightBlue);
				sampleConsole.printEx(2,2+curfunc,TCOD_BKGND_SET,TCOD_LEFT,funcName[curfunc]);
		} else {
				sampleConsole.setDefaultForeground(TCODColor::grey);
				sampleConsole.print(2,2+curfunc,funcName[curfunc]);
		}
	}
	// draw parameters
	sampleConsole.setDefaultForeground(TCODColor::white);
	sampleConsole.print(2,11,"Y/H : zoom (%2.1f)",zoom);
	if ( func > WAVELET ) {
		sampleConsole.print(2,12,"E/D : hurst (%2.1f)",hurst);
		sampleConsole.print(2,13,"R/F : lacunarity (%2.1f)",lacunarity);
		sampleConsole.print(2,14,"T/G : octaves (%2.1f)",octaves);
	}
	// handle keypress
	if ( key->vk == TCODK_NONE) return;
	if ( key->c >= '1' && key->c <= '9') {
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
void render_fov(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
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
	static bool light_walls=true;
	static int algonum=0;
	static const char *algo_names[]={"BASIC      ", "DIAMOND    ", "SHADOW     ",
		"PERMISSIVE0","PERMISSIVE1","PERMISSIVE2","PERMISSIVE3","PERMISSIVE4",
		"PERMISSIVE5","PERMISSIVE6","PERMISSIVE7","PERMISSIVE8","RESTRICTIVE"};
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
		sampleConsole.setDefaultForeground(TCODColor::white);
		sampleConsole.print(1,0,"IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s",
			torch ? "on " : "off", light_walls ? "on "  : "off", algo_names[algonum]);
		sampleConsole.setDefaultForeground(TCODColor::black);
		sampleConsole.putChar(px,py,'@',TCOD_BKGND_NONE);
		// draw windows
		for (int y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
			for (int x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
				if ( smap[y][x] == '=' ) {
					sampleConsole.putChar(x,y,TCOD_CHAR_DHLINE,TCOD_BKGND_NONE);
				}
			}
		}
	}
	if ( recomputeFov ) {
		// calculate the field of view from the player position
		recomputeFov=false;
		map->computeFov(px,py,torch ? (int)(TORCH_RADIUS) : 0,light_walls,(TCOD_fov_algorithm_t)algonum);
	}
	// torch position & intensity variation
	float dx=0.0f,dy=0.0f,di=0.0f;
	if ( torch ) {
		// slightly change the perlin noise parameter
		torchx+=0.2f;
		// randomize the light position between -1.5 and 1.5
		float tdx=torchx+20.0f;
		dx = noise->get(&tdx)*1.5f;
		tdx += 30.0f;
		dy = noise->get(&tdx)*1.5f;
		// randomize the light intensity between -0.2 and 0.2
		di = 0.2f * noise->get(&torchx);
	}
	// draw the dungeon
	for (int y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
		for (int x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
			bool visible = map->isInFov(x,y);
			bool wall=smap[y][x]=='#';
			if (! visible ) {
				sampleConsole.setCharBackground(x,y,wall ? darkWall:darkGround,TCOD_BKGND_SET);
			} else {
				TCODColor light;
				if ( !torch ) {
					light = wall ? lightWall : lightGround;
				} else {
					// torch flickering fx
					TCODColor base=(wall ? darkWall : darkGround);
					light=(wall ? lightWall : lightGround);
					// cell distance to torch (squared)
					float r=(float)((x-px+dx)*(x-px+dx)+(y-py+dy)*(y-py+dy));
					if ( r < SQUARED_TORCH_RADIUS ) {
						// l = 1.0 at player position, 0.0 at a radius of 10 cells
						float l = (SQUARED_TORCH_RADIUS-r)/SQUARED_TORCH_RADIUS +di;
						l=CLAMP(0.0f,1.0f,l);
						// interpolate the color
						base = TCODColor::lerp(base,light,l);
					}
					light=base;
				}
				sampleConsole.setCharBackground(x,y,light, TCOD_BKGND_SET );
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
		sampleConsole.setDefaultForeground(TCODColor::white);
		sampleConsole.print(1,0,"IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s",
			torch ? "on " : "off", light_walls ? "on "  : "off", algo_names[algonum]);
		sampleConsole.setDefaultForeground(TCODColor::black);
	} else if ( key->c == 'W' || key->c == 'w' ) {
		light_walls=!light_walls;
		sampleConsole.setDefaultForeground(TCODColor::white);
		sampleConsole.print(1,0,"IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s",
			torch ? "on " : "off", light_walls ? "on "  : "off", algo_names[algonum]);
		sampleConsole.setDefaultForeground(TCODColor::black);
		recomputeFov=true;
	} else if ( key->c == '+' || key->c == '-' ) {
		algonum+= key->c == '+' ? 1 : -1;
		algonum=CLAMP(0,NB_FOV_ALGORITHMS-1,algonum);
		sampleConsole.setDefaultForeground(TCODColor::white);
		sampleConsole.print(1,0,"IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s",
			torch ? "on " : "off", light_walls ? "on "  : "off", algo_names[algonum]);
		sampleConsole.setDefaultForeground(TCODColor::black);
		recomputeFov=true;
	}
}

// ***************************
// image sample
// ***************************
void render_image(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
	static TCODImage * img=NULL, *circle = NULL;
	static TCODColor blue(0,0,255);
	static TCODColor green(0,255,0);
	if ( img == NULL ) {
		img=new TCODImage("data/img/skull.png");
		img->setKeyColor(TCODColor::black);
		circle=new TCODImage("data/img/circle.png");
	}
	if ( first ) {
		TCODSystem::setFps(30); // fps limited to 30
	}
	sampleConsole.setDefaultBackground(TCODColor::black);
	sampleConsole.clear();
	float x=SAMPLE_SCREEN_WIDTH/2+cosf(TCODSystem::getElapsedSeconds())*10.0f;
	float y=(float)(SAMPLE_SCREEN_HEIGHT/2);
	float scalex=0.2f+1.8f*(1.0f+cosf(TCODSystem::getElapsedSeconds()/2))/2.0f;
	float scaley=scalex;
	float angle=TCODSystem::getElapsedSeconds();
	long elapsed=TCODSystem::getElapsedMilli()/2000;
	if ( elapsed & 1 ) {
		// split the color channels of circle.png
		// the red channel
		sampleConsole.setDefaultBackground(TCODColor::red);
		sampleConsole.rect(0,3,15,15,false,TCOD_BKGND_SET);
		circle->blitRect(&sampleConsole,0,3,-1,-1,TCOD_BKGND_MULTIPLY);
		// the green channel
		sampleConsole.setDefaultBackground(green);
		sampleConsole.rect(15,3,15,15,false,TCOD_BKGND_SET);
		circle->blitRect(&sampleConsole,15,3,-1,-1,TCOD_BKGND_MULTIPLY);
		// the blue channel
		sampleConsole.setDefaultBackground(blue);
		sampleConsole.rect(30,3,15,15,false,TCOD_BKGND_SET);
		circle->blitRect(&sampleConsole,30,3,-1,-1,TCOD_BKGND_MULTIPLY);
	} else {
		// render circle.png with normal blitting
		circle->blitRect(&sampleConsole,0,3,-1,-1,TCOD_BKGND_SET);
		circle->blitRect(&sampleConsole,15,3,-1,-1,TCOD_BKGND_SET);
		circle->blitRect(&sampleConsole,30,3,-1,-1,TCOD_BKGND_SET);
	}
	img->blit(&sampleConsole,x,y,
		TCOD_BKGND_SET,scalex,scaley,angle);
}

// ***************************
// mouse sample
// ***************************/
void render_mouse(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
  static bool lbut=false,rbut=false,mbut=false;

  if ( first ) {
    TCODSystem::setFps(30); // fps limited to 30
    sampleConsole.setDefaultBackground(TCODColor::grey);
    sampleConsole.setDefaultForeground(TCODColor::lightYellow);
    TCODMouse::move(320,200);
    TCODMouse::showCursor(true);
  }

  sampleConsole.clear();
  if ( mouse->lbutton_pressed ) lbut=!lbut;
  if ( mouse->rbutton_pressed ) rbut=!rbut;
  if ( mouse->mbutton_pressed ) mbut=!mbut;
  sampleConsole.print(1,1,
  	"%s\n"
    "Mouse position : %4dx%4d %s\n"
    "Mouse cell     : %4dx%4d\n"
    "Mouse movement : %4dx%4d\n"
    "Left button    : %s (toggle %s)\n"
    "Right button   : %s (toggle %s)\n"
    "Middle button  : %s (toggle %s)\n"
	"Wheel          : %s\n",
	TCODConsole::isActive() ? "" : "APPLICATION INACTIVE",
    mouse->x,mouse->y,TCODConsole::hasMouseFocus() ? "" : "OUT OF FOCUS",
    mouse->cx,mouse->cy,
    mouse->dx,mouse->dy,
    mouse->lbutton ? " ON" : "OFF",lbut ? " ON" : "OFF",
    mouse->rbutton ? " ON" : "OFF",rbut ? " ON" : "OFF",
    mouse->mbutton ? " ON" : "OFF",mbut ? " ON" : "OFF",
	mouse->wheel_up ? "UP" : (mouse->wheel_down ? "DOWN" : "") );
  sampleConsole.print(1,10,"1 : Hide cursor\n2 : Show cursor");
  if (key->c == '1') TCODMouse::showCursor(false);
  else if( key->c == '2' ) TCODMouse::showCursor(true);
}

// ***************************
// path sample
// ***************************
void render_path(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
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
	static int dx=24,dy=1; // destination
	static TCODMap *map=NULL;
	static TCODColor darkWall(0,0,100);
	static TCODColor darkGround(50,50,150);
	static TCODColor lightGround(200,180,50);
	static TCODPath *path=NULL;
	static bool usingAstar=true;
	static float dijkstraDist=0;
	static TCODDijkstra *dijkstra = NULL;
	static bool recalculatePath=false;
	static float busy;
	static int oldChar=' ';
	int mx,my;
	if ( ! map) {
		// initialize the map for the fov toolkit
		map = new TCODMap(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);
		for (int y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
			for (int x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
				if ( smap[y][x] == ' ' ) map->setProperties(x,y,true,true);// ground
				else if ( smap[y][x] == '=' ) map->setProperties(x,y,true,false); // window
			}
		}
		path=new TCODPath(map);
		dijkstra = new TCODDijkstra(map);
	}
	if ( first ) {
		TCODSystem::setFps(30); // fps limited to 30
		// we draw the foreground only the first time.
		// during the player movement, only the @ is redrawn.
		// the rest impacts only the background color
		// draw the help text & player @
		sampleConsole.clear();
		sampleConsole.setDefaultForeground(TCODColor::white);
		sampleConsole.putChar(dx,dy,'+',TCOD_BKGND_NONE);
		sampleConsole.putChar(px,py,'@',TCOD_BKGND_NONE);
		sampleConsole.print(1,1,"IJKL / mouse :\nmove destination\nTAB : A*/dijkstra");
		sampleConsole.print(1,4,"Using : A*");
		// draw windows
		for (int y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
			for (int x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
				if ( smap[y][x] == '=' ) {
					sampleConsole.putChar(x,y,TCOD_CHAR_DHLINE,TCOD_BKGND_NONE);
				}
			}
		}
		recalculatePath=true;
	}
	if ( recalculatePath ) {
		if (usingAstar) {
			path->compute(px,py,dx,dy);
		} else {
			dijkstraDist=0.0f;
			// compute the distance grid
			dijkstra->compute(px,py);
			// get the maximum distance (needed for ground shading only)
			for (int y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
				for (int x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
					float d= dijkstra->getDistance(x,y);
					if ( d > dijkstraDist ) dijkstraDist=d;
				}
			}
			// compute the path
			dijkstra->setPath(dx,dy);
		}
		recalculatePath=false;
		busy=0.2f;
	}
	// draw the dungeon
	for (int y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
		for (int x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
			bool wall=smap[y][x]=='#';
			sampleConsole.setCharBackground(x,y,wall ? darkWall : darkGround, TCOD_BKGND_SET );
		}
	}
	// draw the path
	if ( usingAstar ) {
		for (int i=0; i< path->size(); i++ ) {
			int x,y;
			path->get(i,&x,&y);
			sampleConsole.setCharBackground(x,y,lightGround, TCOD_BKGND_SET );
		}
	} else {
		for (int y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
			for (int x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
				bool wall=smap[y][x]=='#';
				if (! wall) {
					float d= dijkstra->getDistance(x,y);
					sampleConsole.setCharBackground(x,y,TCODColor::lerp(lightGround,darkGround,0.9f * d / dijkstraDist), TCOD_BKGND_SET );
				}
			}
		}
		for (int i=0; i< dijkstra->size(); i++ ) {
			int x,y;
			dijkstra->get(i,&x,&y);
			sampleConsole.setCharBackground(x,y,lightGround, TCOD_BKGND_SET );
		}
	}
	// move the creature
	busy-=TCODSystem::getLastFrameLength();
	if (busy <= 0.0f ) {
		busy = 0.2f;
		if ( usingAstar ) {
			if (!path->isEmpty()) {
				sampleConsole.putChar(px,py,' ',TCOD_BKGND_NONE);
				path->walk(&px,&py,true);
				sampleConsole.putChar(px,py,'@',TCOD_BKGND_NONE);
			}
		} else {
			if (!dijkstra->isEmpty()) {
				sampleConsole.putChar(px,py,' ',TCOD_BKGND_NONE);
				dijkstra->walk(&px,&py);
				sampleConsole.putChar(px,py,'@',TCOD_BKGND_NONE);
				recalculatePath=true;
			}
		}
	}
	if ( (key->c == 'I' || key->c == 'i') && dy > 0 ) {
		// destination move north
		sampleConsole.putChar(dx,dy,oldChar,TCOD_BKGND_NONE);
		dy--;
		oldChar=sampleConsole.getChar(dx,dy);
		sampleConsole.putChar(dx,dy,'+',TCOD_BKGND_NONE);
		if ( smap[dy][dx] == ' ' ) {
			recalculatePath=true;
		}
	} else if (( key->c == 'K' || key->c == 'k' ) && dy < SAMPLE_SCREEN_HEIGHT-1 ) {
		// destination move south
		sampleConsole.putChar(dx,dy,oldChar,TCOD_BKGND_NONE);
		dy++;
		oldChar=sampleConsole.getChar(dx,dy);
		sampleConsole.putChar(dx,dy,'+',TCOD_BKGND_NONE);
		if ( smap[dy][dx] == ' ' ) {
			recalculatePath=true;
		}
	} else if (( key->c == 'J' || key->c == 'j' ) && dx > 0 ) {
		// destination move west
		sampleConsole.putChar(dx,dy,oldChar,TCOD_BKGND_NONE);
		dx--;
		oldChar=sampleConsole.getChar(dx,dy);
		sampleConsole.putChar(dx,dy,'+',TCOD_BKGND_NONE);
		if ( smap[dy][dx] == ' ' ) {
			recalculatePath=true;
		}
	} else if (( key->c == 'L' || key->c == 'l' ) && dx < SAMPLE_SCREEN_WIDTH -1 ) {
		// destination move east
		sampleConsole.putChar(dx,dy,oldChar,TCOD_BKGND_NONE);
		dx++;
		oldChar=sampleConsole.getChar(dx,dy);
		sampleConsole.putChar(dx,dy,'+',TCOD_BKGND_NONE);
		if ( smap[dy][dx] == ' ' ) {
			recalculatePath=true;
		}
	} else if ( key->vk == TCODK_TAB ) {
		usingAstar = ! usingAstar;
		if ( usingAstar )
			sampleConsole.print(1,4,"Using : A*      ");
		else
			sampleConsole.print(1,4,"Using : Dijkstra");
		recalculatePath=true;
	}
	mx = mouse->cx-SAMPLE_SCREEN_X;
	my = mouse->cy-SAMPLE_SCREEN_Y;
	if ( mx >= 0 && mx < SAMPLE_SCREEN_WIDTH && my >= 0 && my < SAMPLE_SCREEN_HEIGHT && ( dx != mx || dy != my ) ) {
		sampleConsole.putChar(dx,dy,oldChar,TCOD_BKGND_NONE);
		dx=mx;dy=my;
		oldChar=sampleConsole.getChar(dx,dy);
		sampleConsole.putChar(dx,dy,'+',TCOD_BKGND_NONE);
		if ( smap[dy][dx] == ' ' ) {
			recalculatePath=true;
		}
	}
}

// ***************************
// bsp sample
// ***************************

static int bspDepth=8;
static int minRoomSize=4;
static bool randomRoom=false; // a room fills a random part of the node or the maximum available space ?
static bool roomWalls=true; // if true, there is always a wall on north & west side of a room
typedef	char map_t[SAMPLE_SCREEN_WIDTH][SAMPLE_SCREEN_HEIGHT];

// draw a vertical line
void vline(map_t *map,int x, int y1, int y2) {
	int y=y1;
	int dy=(y1>y2?-1:1);
	(*map)[x][y]=' ';
	if ( y1 == y2 ) return;
	do {
		y+=dy;
		(*map)[x][y]=' ';
	} while (y!=y2);
}


// draw a vertical line up until we reach an empty space
void vline_up(map_t *map,int x, int y) {
	while (y >= 0 && (*map)[x][y] != ' ') {
		(*map)[x][y]=' ';
		y--;
	}
}

// draw a vertical line down until we reach an empty space
void vline_down(map_t *map,int x, int y) {
	while (y < SAMPLE_SCREEN_HEIGHT && (*map)[x][y] != ' ') {
		(*map)[x][y]=' ';
		y++;
	}
}

// draw a horizontal line
void hline(map_t *map,int x1, int y, int x2) {
	int x=x1;
	int dx=(x1>x2?-1:1);
	(*map)[x][y]=' ';
	if ( x1 == x2 ) return;
	do {
		x+=dx;
		(*map)[x][y]=' ';
	} while (x!=x2);
}

// draw a horizontal line left until we reach an empty space
void hline_left(map_t *map,int x, int y) {
	while (x >= 0 && (*map)[x][y] != ' ') {
		(*map)[x][y]=' ';
		x--;
	}
}

// draw a horizontal line right until we reach an empty space
void hline_right(map_t *map,int x, int y) {
	while (x < SAMPLE_SCREEN_WIDTH && (*map)[x][y] != ' ') {
		(*map)[x][y]=' ';
		x++;
	}
}

// the class building the dungeon from the bsp nodes
//#include <stdio.h>
class BspListener : public ITCODBspCallback {
public :
	bool visitNode(TCODBsp *node, void *userData) {
		map_t *map=(map_t *)userData;
		if ( node->isLeaf() ) {
			// calculate the room size
			int minx = node->x+1;
			int maxx = node->x+node->w-1;
			int miny = node->y+1;
			int maxy = node->y+node->h-1;
			if (! roomWalls ) {
				if ( minx > 1 ) minx--;
				if ( miny > 1 ) miny--;
			}
			if (maxx == SAMPLE_SCREEN_WIDTH-1 ) maxx--;
			if (maxy == SAMPLE_SCREEN_HEIGHT-1 ) maxy--;
			if ( randomRoom ) {
				minx = TCODRandom::getInstance()->getInt(minx,maxx-minRoomSize+1);
				miny = TCODRandom::getInstance()->getInt(miny,maxy-minRoomSize+1);
				maxx = TCODRandom::getInstance()->getInt(minx+minRoomSize-1,maxx);
				maxy = TCODRandom::getInstance()->getInt(miny+minRoomSize-1,maxy);
			}
			// resize the node to fit the room
//printf("node %dx%d %dx%d => room %dx%d %dx%d\n",node->x,node->y,node->w,node->h,minx,miny,maxx-minx+1,maxy-miny+1);
			node->x=minx;
			node->y=miny;
			node->w=maxx-minx+1;
			node->h=maxy-miny+1;
			// dig the room
			for (int x=minx; x <= maxx; x++ ) {
				for (int y=miny; y <= maxy; y++ ) {
					(*map)[x][y]=' ';
				}
			}
		} else {
//printf("lvl %d %dx%d %dx%d\n",node->level, node->x,node->y,node->w,node->h);
			// resize the node to fit its sons
			TCODBsp *left=node->getLeft();
			TCODBsp *right=node->getRight();
			node->x=MIN(left->x,right->x);
			node->y=MIN(left->y,right->y);
			node->w=MAX(left->x+left->w,right->x+right->w)-node->x;
			node->h=MAX(left->y+left->h,right->y+right->h)-node->y;
			// create a corridor between the two lower nodes
			if (node->horizontal) {
				// vertical corridor
				if ( left->x+left->w -1 < right->x || right->x+right->w-1 < left->x ) {
					// no overlapping zone. we need a Z shaped corridor
					int x1=TCODRandom::getInstance()->getInt(left->x,left->x+left->w-1);
					int x2=TCODRandom::getInstance()->getInt(right->x,right->x+right->w-1);
					int y=TCODRandom::getInstance()->getInt(left->y+left->h,right->y);
					vline_up(map,x1,y-1);
					hline(map,x1,y,x2);
					vline_down(map,x2,y+1);
				} else {
					// straight vertical corridor
					int minx=MAX(left->x,right->x);
					int maxx=MIN(left->x+left->w-1,right->x+right->w-1);
					int x=TCODRandom::getInstance()->getInt(minx,maxx);
					vline_down(map,x,right->y);
					vline_up(map,x,right->y-1);
				}
			} else {
				// horizontal corridor
				if ( left->y+left->h -1 < right->y || right->y+right->h-1 < left->y ) {
					// no overlapping zone. we need a Z shaped corridor
					int y1=TCODRandom::getInstance()->getInt(left->y,left->y+left->h-1);
					int y2=TCODRandom::getInstance()->getInt(right->y,right->y+right->h-1);
					int x=TCODRandom::getInstance()->getInt(left->x+left->w,right->x);
					hline_left(map,x-1,y1);
					vline(map,x,y1,y2);
					hline_right(map,x+1,y2);
				} else {
					// straight horizontal corridor
					int miny=MAX(left->y,right->y);
					int maxy=MIN(left->y+left->h-1,right->y+right->h-1);
					int y=TCODRandom::getInstance()->getInt(miny,maxy);
					hline_left(map,right->x-1,y);
					hline_right(map,right->x,y);
				}
			}
		}
		return true;
	}
};
void render_bsp(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
	static TCODBsp *bsp=NULL;
	static bool generate=true;
	static bool refresh=false;
	static map_t map;
	static TCODColor darkWall(0,0,100);
	static TCODColor darkGround(50,50,150);
	static BspListener listener;
	if ( generate || refresh ) {
		// dungeon generation
		if (! bsp ) {
			// create the bsp
			bsp = new TCODBsp(0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);
		} else {
			// restore the nodes size
			bsp->resize(0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);
		}
		memset(map,'#',sizeof(char)*SAMPLE_SCREEN_WIDTH*SAMPLE_SCREEN_HEIGHT);
		if ( generate ) {
			// build a new random bsp tree
			bsp->removeSons();
			bsp->splitRecursive(NULL,bspDepth,minRoomSize+(roomWalls?1:0),minRoomSize+(roomWalls?1:0),1.5f,1.5f);
		}
		// create the dungeon from the bsp
		bsp->traverseInvertedLevelOrder(&listener,&map);
		generate=false;
		refresh=false;
	}
	sampleConsole.clear();
	sampleConsole.setDefaultForeground(TCODColor::white);
	sampleConsole.print(1,1,"ENTER : rebuild bsp\nSPACE : rebuild dungeon\n+-: bsp depth %d\n*/: room size %d\n1 : random room size %s",
		bspDepth,minRoomSize,
		randomRoom ? "ON" : "OFF");
	if ( randomRoom )
	sampleConsole.print(1,6,"2 : room walls %s",
		roomWalls ? "ON" : "OFF"	);
	// render the level
	for (int y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
		for (int x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
			bool wall= ( map[x][y] == '#' );
			sampleConsole.setCharBackground(x,y,wall ? darkWall : darkGround, TCOD_BKGND_SET );
		}
	}
	if ( key->vk == TCODK_ENTER || key->vk == TCODK_KPENTER ) {
		generate=true;
	} else if (key->c==' ') {
		refresh=true;
	} else if (key->c=='+') {
		bspDepth++;
		generate=true;
	} else if (key->c=='-' && bspDepth > 1) {
		bspDepth--;
		generate=true;
	} else if (key->c=='*') {
		minRoomSize++;
		generate=true;
	} else if (key->c=='/' && minRoomSize > 2) {
		minRoomSize--;
		generate=true;
	} else if (key->c=='1' || key->vk == TCODK_1 || key->vk == TCODK_KP1) {
		randomRoom=!randomRoom;
		if (! randomRoom ) roomWalls=true;
		refresh=true;
	} else if (key->c=='2' || key->vk == TCODK_2 || key->vk == TCODK_KP2) {
		roomWalls=!roomWalls;
		refresh=true;
	}
}

/* ***************************
 * name generator sample
 * ***************************/
void render_name(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
	static int nbSets=0;
	static int curSet=0;
	static float delay=0.0f;
	static TCODList<char *> names;
	static TCODList<char *> sets;
	int i;
	if ( nbSets == 0 ) {
		TCODList<char *> files=TCODSystem::getDirectoryContent("data/namegen","*.cfg");
		// parse all the files
		for (char **it=files.begin(); it != files.end(); it++) {
			char tmp[256];
			sprintf(tmp, "data/namegen/%s",*it);
			TCODNamegen::parse(tmp);
		}
		// get the sets list
		sets = TCODNamegen::getSets();
		nbSets = sets.size();
	}
	if ( first ) {
		TCODSystem::setFps(30); /* limited to 30 fps */
	}

	while ( names.size() >= 15 ) {
		// remove the first element.
#ifndef TCOD_VISUAL_STUDIO
		char *nameToRemove= * (names.begin());
#endif
		names.remove(names.begin());
		// for some reason, this crashes on MSVC...
#ifndef TCOD_VISUAL_STUDIO
		free(nameToRemove);
#endif
	}

	sampleConsole.setDefaultBackground(TCODColor::lightBlue);
	sampleConsole.clear();
	sampleConsole.setDefaultForeground(TCODColor::white);
	sampleConsole.print(1,1,"%s\n\n+ : next generator\n- : prev generator",
		sets.get(curSet));
	for (i=0; i < names.size(); i++) {
		char *name=names.get(i);
		if ( strlen(name)< SAMPLE_SCREEN_WIDTH )
			sampleConsole.printEx(SAMPLE_SCREEN_WIDTH-2,2+i,TCOD_BKGND_NONE,TCOD_RIGHT,name);
	}

	delay += TCODSystem::getLastFrameLength();
	if ( delay >= 0.5f ) {
		delay -= 0.5f;
		// add a new name to the list
		names.push(TCODNamegen::generate(sets.get(curSet),true));
	}
	if ( key->c == '+' ) {
		curSet ++;
		if ( curSet == nbSets ) curSet=0;
		names.push(strdup("======"));
	} else if ( key->c == '-'  ) {
		curSet --;
		if ( curSet < 0 ) curSet=nbSets-1;
		names.push(strdup("======"));
	}
}

/* ***************************
 * SDL callback sample
 * ***************************/
#ifndef NO_SDL_SAMPLE
static bool sdl_callback_enabled=false;

class SampleRenderer : public ITCODSDLRenderer {
public :
	SampleRenderer() : effectNum(0), delay(3.0f) {
		noise=new TCODNoise(3);
	}
	~SampleRenderer() {
		delete noise;
	}
	void render(void *sdlSurface) {
		SDL_Surface *screen = (SDL_Surface *)sdlSurface;
		// now we have almighty access to the screen's precious pixels !!
		// get the font character size
		int charw,charh;
		TCODSystem::getCharSize(&charw,&charh);
		// compute the sample console position in pixels
		int samplex = SAMPLE_SCREEN_X * charw;
		int sampley = SAMPLE_SCREEN_Y * charh;
		delay -= TCODSystem::getLastFrameLength();
		if ( delay < 0.0f ) {
			delay = 3.0f;
			effectNum = (effectNum + 1) % 3;
			if ( effectNum == 2 ) sdl_callback_enabled=false; // no forced redraw for burn effect
			else sdl_callback_enabled=true;
		}
		switch(effectNum) {
			case 0 : blur(screen,samplex,sampley,SAMPLE_SCREEN_WIDTH * charw,SAMPLE_SCREEN_HEIGHT * charh); break;
			case 1 : explode(screen,samplex,sampley,SAMPLE_SCREEN_WIDTH * charw,SAMPLE_SCREEN_HEIGHT * charh); break;
			case 2 : burn(screen,samplex,sampley,SAMPLE_SCREEN_WIDTH * charw,SAMPLE_SCREEN_HEIGHT * charh); break;
		}
	}

protected :
	TCODNoise *noise;
	int effectNum;
	float delay;

	void burn(SDL_Surface *screen, int samplex, int sampley, int samplew, int sampleh) {
		int ridx=screen->format->Rshift/8;
		int gidx=screen->format->Gshift/8;
		int bidx=screen->format->Bshift/8;
		for (int x=samplex; x < samplex + samplew; x ++ ) {
			Uint8 *p = (Uint8 *)screen->pixels + x * screen->format->BytesPerPixel + sampley * screen->pitch;
			for (int y=sampley; y < sampley + sampleh; y ++ ) {
				int ir=0,ig=0,ib=0;
				Uint8 *p2 = p + screen->format->BytesPerPixel; // get pixel at x+1,y
				ir += p2[ridx];
				ig += p2[gidx];
				ib += p2[bidx];
				p2 -= 2*screen->format->BytesPerPixel; // get pixel at x-1,y
				ir += p2[ridx];
				ig += p2[gidx];
				ib += p2[bidx];
				p2 += screen->format->BytesPerPixel+ screen->pitch; // get pixel at x,y+1
				ir += p2[ridx];
				ig += p2[gidx];
				ib += p2[bidx];
				p2 -= 2*screen->pitch; // get pixel at x,y-1
				ir += p2[ridx];
				ig += p2[gidx];
				ib += p2[bidx];
				ir/=4;
				ig/=4;
				ib/=4;
				p[ridx]=ir;
				p[gidx]=ig;
				p[bidx]=ib;
				p += screen->pitch;
			}
		}
	}

	void explode(SDL_Surface *screen, int samplex, int sampley, int samplew, int sampleh) {
		int ridx=screen->format->Rshift/8;
		int gidx=screen->format->Gshift/8;
		int bidx=screen->format->Bshift/8;
		TCODRandom *rng=TCODRandom::getInstance();
		int dist=(int)(10*(3.0f - delay));
		for (int x=samplex; x < samplex + samplew; x ++ ) {
			Uint8 *p = (Uint8 *)screen->pixels + x * screen->format->BytesPerPixel + sampley * screen->pitch;
			for (int y=sampley; y < sampley + sampleh; y ++ ) {
				int ir=0,ig=0,ib=0;
				for (int i=0; i < 3; i++) {
					int dx = rng->getInt(-dist,dist);
					int dy = rng->getInt(-dist,dist);
					Uint8 *p2;
					p2 = p + dx * screen->format->BytesPerPixel;
					p2 += dy * screen->pitch;
					ir += p2[ridx];
					ig += p2[gidx];
					ib += p2[bidx];
				}
				ir/=3;
				ig/=3;
				ib/=3;
				p[ridx]=ir;
				p[gidx]=ig;
				p[bidx]=ib;
				p += screen->pitch;
			}
		}
	}

	void blur(SDL_Surface *screen, int samplex, int sampley, int samplew, int sampleh) {
		// let's blur that sample console
		float f[3],n=0.0f;
		int ridx=screen->format->Rshift/8;
		int gidx=screen->format->Gshift/8;
		int bidx=screen->format->Bshift/8;
		f[2]=TCODSystem::getElapsedSeconds();
		for (int x=samplex; x < samplex + samplew; x ++ ) {
			Uint8 *p = (Uint8 *)screen->pixels + x * screen->format->BytesPerPixel + sampley * screen->pitch;
			f[0]=(float)(x)/samplew;
			for (int y=sampley; y < sampley + sampleh; y ++ ) {
				int ir=0,ig=0,ib=0;
				if ( (y-sampley)%8 == 0 ) {
					f[1]=(float)(y)/sampleh;
					n=noise->getFbm(f,3.0f);
				}
				int dec = (int)(3*(n+1.0f));
				int count=0;
				switch(dec) {
					case 4:
						count += 4;
						// get pixel at x,y
						ir += p[ridx];
						ig += p[gidx];
						ib += p[bidx];
						p -= 2*screen->format->BytesPerPixel; // get pixel at x+2,y
						ir += p[ridx];
						ig += p[gidx];
						ib += p[bidx];
						p -= 2*screen->pitch; // get pixel at x+2,y+2
						ir += p[ridx];
						ig += p[gidx];
						ib += p[bidx];
						p+= 2*screen->format->BytesPerPixel; // get pixel at x,y+2
						ir += p[ridx];
						ig += p[gidx];
						ib += p[bidx];
						p += 2*screen->pitch;
					case 3:
						count += 4;
						// get pixel at x,y
						ir += p[ridx];
						ig += p[gidx];
						ib += p[bidx];
						p += 2*screen->format->BytesPerPixel; // get pixel at x+2,y
						ir += p[ridx];
						ig += p[gidx];
						ib += p[bidx];
						p += 2*screen->pitch; // get pixel at x+2,y+2
						ir += p[ridx];
						ig += p[gidx];
						ib += p[bidx];
						p-= 2*screen->format->BytesPerPixel; // get pixel at x,y+2
						ir += p[ridx];
						ig += p[gidx];
						ib += p[bidx];
						p -= 2*screen->pitch;
					case 2:
						count += 4;
						// get pixel at x,y
						ir += p[ridx];
						ig += p[gidx];
						ib += p[bidx];
						p -= screen->format->BytesPerPixel; // get pixel at x-1,y
						ir += p[ridx];
						ig += p[gidx];
						ib += p[bidx];
						p -= screen->pitch; // get pixel at x-1,y-1
						ir += p[ridx];
						ig += p[gidx];
						ib += p[bidx];
						p+= screen->format->BytesPerPixel; // get pixel at x,y-1
						ir += p[ridx];
						ig += p[gidx];
						ib += p[bidx];
						p += screen->pitch;
					case 1:
						count += 4;
						// get pixel at x,y
						ir += p[ridx];
						ig += p[gidx];
						ib += p[bidx];
						p += screen->format->BytesPerPixel; // get pixel at x+1,y
						ir += p[ridx];
						ig += p[gidx];
						ib += p[bidx];
						p += screen->pitch; // get pixel at x+1,y+1
						ir += p[ridx];
						ig += p[gidx];
						ib += p[bidx];
						p-= screen->format->BytesPerPixel; // get pixel at x,y+1
						ir += p[ridx];
						ig += p[gidx];
						ib += p[bidx];
						p -= screen->pitch;
						ir/=count;
						ig/=count;
						ib/=count;
						p[ridx]=ir;
						p[gidx]=ig;
						p[bidx]=ib;
					break;
					default:break;
				}
				p += screen->pitch;
			}
		}
	}
};

void render_sdl(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
	if ( first ) {
		TCODSystem::setFps(30); /* limited to 30 fps */
		// use noise sample as background. rendering is done in SampleRenderer
		sampleConsole.setDefaultBackground(TCODColor::lightBlue);
		sampleConsole.setDefaultForeground(TCODColor::white);
		sampleConsole.clear();
		sampleConsole.printRectEx(SAMPLE_SCREEN_WIDTH/2,3,SAMPLE_SCREEN_WIDTH,0, TCOD_BKGND_NONE,TCOD_CENTER,
			"The SDL callback gives you access to the screen surface so that you can alter the pixels one by one using SDL API or any API on top of SDL. SDL is used here to blur the sample console.\n\nHit TAB to enable/disable the callback. While enabled, it will be active on other samples too.\n\nNote that the SDL callback only works with SDL renderer.");
	}
	if ( key->vk == TCODK_TAB ) {
		sdl_callback_enabled = !sdl_callback_enabled;
		if (sdl_callback_enabled) {
			TCODSystem::registerSDLRenderer(new SampleRenderer());
		} else {
			TCODSystem::registerSDLRenderer(NULL);
			// we want libtcod to redraw the sample console even if nothing has changed in it
			TCODConsole::root->setDirty(SAMPLE_SCREEN_X,SAMPLE_SCREEN_Y,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);
		}
	}
}

#endif
// ***************************
// the list of samples
// ***************************
sample_t samples[] = {
	{"  True colors        ",render_colors},
	{"  Offscreen console  ",render_offscreen},
	{"  Line drawing       ",render_lines},
	{"  Noise              ",render_noise},
	{"  Field of view      ",render_fov},
	{"  Path finding       ",render_path},
	{"  Bsp toolkit        ",render_bsp},
	{"  Image toolkit      ",render_image},
	{"  Mouse support      ",render_mouse},
	{"  Name generator     ",render_name},
#ifndef NO_SDL_SAMPLE
	{"  SDL callback       ",render_sdl},
#endif
};
int nbSamples = sizeof(samples)/sizeof(sample_t); // total number of samples

// ***************************
// the main function
// ***************************
int main( int argc, char *argv[] ) {
	int curSample=0; // index of the current sample
	bool first=true; // first time we render a sample
	TCOD_key_t key = {TCODK_NONE,0};
	TCOD_mouse_t mouse;
	const char *font="data/fonts/consolas10x10_gs_tc.png";
	int nbCharHoriz=0,nbCharVertic=0;
	int argn;
	int fullscreenWidth=0;
	int fullscreenHeight=0;
	TCOD_renderer_t renderer=TCOD_RENDERER_SDL;
	bool fullscreen=false;
	int fontFlags=TCOD_FONT_TYPE_GREYSCALE|TCOD_FONT_LAYOUT_TCOD, fontNewFlags=0;
	bool creditsEnd=false;
	int cur_renderer=0;
	static const char *renderer_name[TCOD_NB_RENDERERS] = {
		"F1 GLSL   ","F2 OPENGL ","F3 SDL    "
	};

	// initialize the root console (open the game window)
	for (argn=1; argn < argc; argn++) {
		if ( strcmp(argv[argn],"-font") == 0 && argn+1 < argc) {
			argn++;
			font=argv[argn];
			fontFlags=0;
		} else if ( strcmp(argv[argn],"-font-nb-char") == 0 && argn+2 < argc ) {
			argn++;
			nbCharHoriz=atoi(argv[argn]);
			argn++;
			nbCharVertic=atoi(argv[argn]);
			fontFlags=0;
		} else if ( strcmp(argv[argn],"-fullscreen-resolution") == 0 && argn+2 < argc ) {
			argn++;
			fullscreenWidth=atoi(argv[argn]);
			argn++;
			fullscreenHeight=atoi(argv[argn]);
		} else if ( strcmp(argv[argn],"-renderer") == 0 && argn+1 < argc ) {
			argn++;
			renderer=(TCOD_renderer_t)atoi(argv[argn]);
		} else if ( strcmp(argv[argn],"-fullscreen") == 0 ) {
			fullscreen=true;
		} else if ( strcmp(argv[argn],"-font-in-row") == 0 ) {
			fontNewFlags |= TCOD_FONT_LAYOUT_ASCII_INROW;
			fontFlags=0;
		} else if ( strcmp(argv[argn],"-font-greyscale") == 0 ) {
			fontNewFlags |= TCOD_FONT_TYPE_GREYSCALE;
			fontFlags=0;
		} else if ( strcmp(argv[argn],"-font-tcod") == 0 ) {
			fontNewFlags |= TCOD_FONT_LAYOUT_TCOD;
			fontFlags=0;
		} else if ( strcmp(argv[argn],"-help") == 0 || strcmp(argv[argn],"-?") == 0) {
			printf ("options :\n");
			printf ("-font <filename> : use a custom font\n");
			printf ("-font-nb-char <nb_char_horiz> <nb_char_vertic> : number of characters in the font\n");
			printf ("-font-in-row : the font layout is in row instead of columns\n");
			printf ("-font-tcod : the font uses TCOD layout instead of ASCII\n");
			printf ("-font-greyscale : antialiased font using greyscale bitmap\n");
			printf ("-fullscreen : start in fullscreen\n");
			printf ("-fullscreen-resolution <screen_width> <screen_height> : force fullscreen resolution\n");
			printf ("-renderer <num> : set renderer. 0 : GLSL 1 : OPENGL 2 : SDL\n");
			exit(0);
		} else {
			// ignore parameter
		}
	}

	if ( fontFlags == 0 ) fontFlags=fontNewFlags;
	TCODConsole::setCustomFont(font,fontFlags,nbCharHoriz,nbCharVertic);
	if ( fullscreenWidth > 0 ) {
		TCODSystem::forceFullscreenResolution(fullscreenWidth,fullscreenHeight);
	}

	TCODConsole::initRoot(80,50,"libtcod C++ sample",fullscreen,renderer);
	do {
		if (! creditsEnd) {
			creditsEnd=TCODConsole::renderCredits(60,43,false);
		}

		// print the list of samples
		for (int i=0; i < nbSamples; i++ ) {
			if ( i == curSample ) {
				// set colors for currently selected sample
				TCODConsole::root->setDefaultForeground(TCODColor::white);
				TCODConsole::root->setDefaultBackground(TCODColor::lightBlue);
			} else {
				// set colors for other samples
				TCODConsole::root->setDefaultForeground(TCODColor::grey);
				TCODConsole::root->setDefaultBackground(TCODColor::black);
			}
			// print the sample name
			TCODConsole::root->printEx(2,46-(nbSamples-i),TCOD_BKGND_SET,TCOD_LEFT,samples[i].name);
		}
		// print the help message
		TCODConsole::root->setDefaultForeground(TCODColor::grey);
		TCODConsole::root->printEx(79,46,TCOD_BKGND_NONE,TCOD_RIGHT,"last frame : %3d ms (%3d fps)", (int)(TCODSystem::getLastFrameLength()*1000), 			TCODSystem::getFps());
		TCODConsole::root->printEx(79,47,TCOD_BKGND_NONE,TCOD_RIGHT,"elapsed : %8dms %4.2fs", TCODSystem::getElapsedMilli(),TCODSystem::getElapsedSeconds());
		TCODConsole::root->print(2,47,"%c%c : select a sample", TCOD_CHAR_ARROW_N, TCOD_CHAR_ARROW_S);
		TCODConsole::root->print(2,48,"ALT-ENTER : switch to %s",
			TCODConsole::isFullscreen() ? "windowed mode  " : "fullscreen mode");

		// render current sample
		samples[curSample].render(first,&key,&mouse);
		first=false;

		// blit the sample console on the root console
		TCODConsole::blit(&sampleConsole,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT, // the source console & zone to blit
							TCODConsole::root,SAMPLE_SCREEN_X,SAMPLE_SCREEN_Y // the destination console & position
						 );
		// erase the renderer in debug mode (needed because the root console is not cleared each frame)
		TCODConsole::root->print(1,1,"        ");
#ifndef NO_SDL_SAMPLE
		if ( sdl_callback_enabled ) {
			// we want libtcod to redraw the sample console even if nothing has changed in it
			TCODConsole::root->setDirty(SAMPLE_SCREEN_X,SAMPLE_SCREEN_Y,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);
		}
#endif
		/* display renderer list and current renderer */
		cur_renderer=TCODSystem::getRenderer();
		TCODConsole::root->setDefaultForeground(TCODColor::grey);
		TCODConsole::root->setDefaultBackground(TCODColor::black);
		TCODConsole::root->printEx(42,46-(TCOD_NB_RENDERERS+1),TCOD_BKGND_SET,TCOD_LEFT,"Renderer :");
		for (int i=0; i < TCOD_NB_RENDERERS; i++) {
			if (i==cur_renderer) {
				/* set colors for current renderer */
				TCODConsole::root->setDefaultForeground(TCODColor::white);
				TCODConsole::root->setDefaultBackground(TCODColor::lightBlue);
			} else {
				/* set colors for other renderer */
				TCODConsole::root->setDefaultForeground(TCODColor::grey);
				TCODConsole::root->setDefaultBackground(TCODColor::black);
			}
			TCODConsole::root->printEx(42,46-(TCOD_NB_RENDERERS-i),TCOD_BKGND_SET,TCOD_LEFT,renderer_name[i]);
		}

		// update the game screen
		TCODConsole::flush();

		// did the user hit a key ?
		TCODSystem::checkForEvent((TCOD_event_t)(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE),&key,&mouse);
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
#ifdef TCOD_LINUX
		} else if ( key.c == 'p' ) {
#else
		} else if ( key.vk == TCODK_PRINTSCREEN ) {
#endif
			if ( key.lalt ) {
				// ALT-PrintScreen : save to .asc format
				TCODConsole::root->saveApf("samples.apf");
			} else {
				// save screenshot 
				TCODSystem::saveScreenshot(NULL);
			}
		} else if (key.vk==TCODK_F1) {
			// switch renderers with F1,F2,F3 
			TCODSystem::setRenderer(TCOD_RENDERER_GLSL);
		} else if (key.vk==TCODK_F2) {
			TCODSystem::setRenderer(TCOD_RENDERER_OPENGL);
		} else if (key.vk==TCODK_F3) {
			TCODSystem::setRenderer(TCOD_RENDERER_SDL);
		}
	} while (!TCODConsole::isWindowClosed());
	return 0;
}
