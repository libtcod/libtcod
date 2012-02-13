/*
* Photon reactor
* Copyright (c) 2011 Jice
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The names of Jice may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY JICE ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <math.h>
#include "libtcod.hpp"
#include "bsp_helper.hpp"
#include "rad_shader.hpp"

#define CON_WIDTH 80
#define CON_HEIGHT 50

#define MAP_WIDTH 39
#define MAP_HEIGHT 50 

#define LIGHT_RADIUS 10
#define CELL_REFLECTIVITY 1.5
#define CELL_SELF_ILLUMINATION 0.4

TCODMap *map;
BspHelper bsp;
int playerx=0,playery=0,playerBack;
Shader *leftShader=NULL;
Shader *rightShader=NULL;
TCODColor darkWall(50,50,150);
TCODColor lightWall(130,110,50);
TCODColor darkGround(0,0,100);
TCODColor lightGround(200,180,50);
int torchIndex;

float stdTime=0.0f;
float radTime=0.0f;
float stdLength=0.0f;
float radLength=0.0f;
int timeSecond;
int framesCount=0;

// gamma correction lookup table
bool enableGammaCorrection=true;
int gammaLookup[256];
#define GAMMA (1/2.2f)
                                   
// find the closest walkable position
void findPos(int *x, int *y) {
	for (; (*x) < MAP_WIDTH; (*x)++) {
		for (; (*y)< MAP_HEIGHT; (*y)++) {
			if ( map->isWalkable((*x),(*y)) ) return;
		}
	}
	for (*x=0; (*x) < MAP_WIDTH; (*x)++) {
		for (*y=0; (*y)< MAP_HEIGHT; (*y)++) {
			if ( map->isWalkable((*x),(*y)) ) return;
		}
	}
}

void init() {
	TCODConsole::root->clear();

	// build the dungeon
	map=new TCODMap(MAP_WIDTH,MAP_HEIGHT);
	bsp.createBspDungeon(map,NULL);
	// empty map
	//map->clear(true,true);	
	
	// create shaders
	leftShader=new StandardShader();
	rightShader=new PhotonShader(CELL_REFLECTIVITY, CELL_SELF_ILLUMINATION, 3);

	// put random lights
	for (int i=0; i < 10; i++ ) {
		int lx=TCODRandom::getInstance()->getInt(1,MAP_WIDTH-2);
		int ly=TCODRandom::getInstance()->getInt(1,MAP_HEIGHT-2);
		findPos(&lx,&ly);
		leftShader->addLight(lx,ly,LIGHT_RADIUS,TCODColor::white);
		rightShader->addLight(lx,ly,LIGHT_RADIUS,TCODColor::white);
		TCODConsole::root->setChar(lx,ly,'*');
		TCODConsole::root->setChar(lx+CON_WIDTH/2,ly,'*');
	}	
	
	// find a starting position for the player
	findPos(&playerx,&playery); 
	playerBack=TCODConsole::root->getChar(playerx,playery);
	TCODConsole::root->setChar(playerx,playery,'@');
	TCODConsole::root->setChar(playerx+CON_WIDTH/2,playery,'@');

	// add the player's torch
	torchIndex = leftShader->addLight(playerx,playery,10,TCODColor::white);
	rightShader->addLight(playerx,playery,LIGHT_RADIUS,TCODColor::white);

	// init shaders (must be done after adding lights for photon shader)
	leftShader->init(map);
	rightShader->init(map);

	timeSecond=TCODSystem::getElapsedMilli()/1000;
	
	if (enableGammaCorrection) {
		for (int i=0; i< 256; i++) {
			float v=i/255.0f;
			float correctedV = pow(v,GAMMA);
			gammaLookup[i] = (int)(correctedV*255);
		}
	} else {
		for (int i=0; i< 256; i++) {
			gammaLookup[i]=i;
		}
	}
}

void render() {
	// compute lights
	framesCount++;
	uint32 start=TCODSystem::getElapsedMilli();
	leftShader->compute();
	uint32 leftEnd=TCODSystem::getElapsedMilli();
	rightShader->compute();
	uint32 rightEnd=TCODSystem::getElapsedMilli();
	stdTime+=(leftEnd-start)*0.001f;
	radTime+=(rightEnd-leftEnd)*0.001f;
	if ( (int)(start/1000) != timeSecond ) {
		timeSecond=start/1000;
		stdLength=stdTime*1000/framesCount;
		radLength=radTime*1000/framesCount;
		stdTime=0.0f;
		radTime=0.0f;
		framesCount=0;
	}
	
	for (int x=0; x < MAP_WIDTH; x++) {
		for (int y=0; y < MAP_HEIGHT; y++) {
			TCODColor darkCol,lightCol;
			// get the cell dark and lit colors
			if (map->isWalkable(x,y)) {
				darkCol=darkGround;
				lightCol=lightGround;
			} else {
				darkCol=darkWall;
				lightCol=lightWall;
			}			
			// render left map
			// hack : for a better look, lights are white and we only use them as 
			// a lerp coefficient between dark and light colors.
			// a true light model would multiply the light color with the cell color 
			TCODColor leftLight=leftShader->getLightColor(x,y);
			TCODColor cellLeftCol=TCODColor::lerp(darkCol,lightCol,gammaLookup[leftLight.r]/255.0f);
			TCODConsole::root->setCharBackground(x,y,cellLeftCol);

			// render right map
			TCODColor rightLight=rightShader->getLightColor(x,y);
			TCODColor cellRightCol=TCODColor::lerp(darkCol,lightCol,gammaLookup[rightLight.r]/255.0f);
			TCODConsole::root->setCharBackground(x+CON_WIDTH/2,y,cellRightCol);
		}
	}
	TCODConsole::root->print(CON_WIDTH/4,0,"Standard lighting %1.2fms",stdLength);
	TCODConsole::root->print(3*CON_WIDTH/4,0,"Photon reactor %1.2fms",radLength);

}

void move(int dx, int dy) {
	if (map->isWalkable(playerx+dx,playery+dy)) {
		// restore the previous map char
		TCODConsole::root->setChar(playerx,playery,playerBack);
		TCODConsole::root->setChar(playerx+CON_WIDTH/2,playery,playerBack);
		// move the player
		playerx+=dx;
		playery+=dy;
		playerBack=TCODConsole::root->getChar(playerx,playery);
		// render the player
		TCODConsole::root->setChar(playerx,playery,'@');
		TCODConsole::root->setChar(playerx+CON_WIDTH/2,playery,'@');
		// update the player's torch position
		leftShader->updateLight(torchIndex,playerx,playery,LIGHT_RADIUS,TCODColor::white);
		rightShader->updateLight(torchIndex,playerx,playery,LIGHT_RADIUS,TCODColor::white);
	}
}

int main() {
	TCODConsole::initRoot(80,50,"Photon reactor - radiosity engine for roguelikes",false,TCOD_RENDERER_SDL);
	TCODConsole::root->setAlignment(TCOD_CENTER);
	TCOD_key_t k = {TCODK_NONE,0};
	TCOD_mouse_t mouse;

	init();
	while (! TCODConsole::isWindowClosed()) {
		render();
		TCODConsole::flush();
		TCODSystem::checkForEvent((TCOD_event_t)TCOD_EVENT_KEY_PRESS,&k,&mouse);
		switch(k.vk) {
			// move with arrows or numpad (2468)
			case TCODK_KP8 : case TCODK_UP : move(0,-1); break;
			case TCODK_KP2 : case TCODK_DOWN : move(0,1); break;
			case TCODK_KP4 : case TCODK_LEFT : move(-1,0); break;
			case TCODK_KP6 : case TCODK_RIGHT : move(1,0); break;
			case TCODK_CHAR :
				switch(k.c) {
					// move with vi keys (HJKL) or FPS keys (WSAD or ZQSD)
					case 'q' : case 'Q' : case 'a' : case 'A' : case 'h': case 'H' : move(-1,0);break;
					case 's' : case 'S' : case 'j': case 'J' : move(0,1);break;
					case 'z' : case 'Z' : case 'w' : case 'W' : case 'k': case 'K' : move(0,-1);break;
					case 'd' : case 'D' : case 'l': case 'L' : move(1,0);break;
					default:break;
				}
			break;
			case TCODK_PRINTSCREEN : TCODSystem::saveScreenshot(NULL); break;
			default:break;
		}
	}
	return 0;
}
