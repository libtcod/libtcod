/*
* Copyright (c) 2010 Jice
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice may not be used to endorse or promote products
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
#include "main.hpp"

#define WIDTH 80
#define HEIGHT 50

TCODNoise noise1d(1);
TCODNoise noise2d(2);
WorldGenerator worldGen;
// world map panning
float wx=0,wy=0, curwx=0,curwy=0;
// mouse coordinates in world map
float mx=0,my=0;

void update(float elapsed, TCOD_key_t k, TCOD_mouse_t mouse) {
	// destination wanted
	wx = (worldGen.getWidth()-2*WIDTH) * mouse.cx / WIDTH;
	wy = (worldGen.getHeight()-2*HEIGHT) * mouse.cy / HEIGHT;
	curwx += (wx-curwx)*elapsed;
	curwy += (wy-curwy)*elapsed;
	mx = curwx + mouse.cx*2;
	my = curwy + mouse.cy*2;
	worldGen.updateClouds(elapsed);	
}

TCODColor getMapShadedColor(float worldX,float worldY,bool clouds) {
    // sun color 
    static TCODColor sunCol(255,255,200);
    float wx = CLAMP(0.0f, worldGen.getWidth()-1,worldX);
    float wy = CLAMP(0.0f, worldGen.getHeight()-1,worldY);
	// apply cloud shadow
	float cloudAmount = clouds ? worldGen.getCloudThickness(wx,wy) : 0.0f;
	TCODColor col = worldGen.getInterpolatedColor(worldX,worldY);
	// apply sun light
    float intensity = worldGen.getInterpolatedIntensity(wx,wy);
	intensity = MIN(intensity, 1.5f-cloudAmount);
	int cr = (int)(intensity*(int)(col.r)*sunCol.r/255 );
	int cg = (int)(intensity*(int)(col.g)*sunCol.g/255 );
	int cb = (int)(intensity*(int)(col.b)*sunCol.b/255 );
	TCODColor col2;
	col2.r = CLAMP(0,255,cr);
	col2.g = CLAMP(0,255,cg);
	col2.b = CLAMP(0,255,cb);
	col2.r=MAX(col2.r,col.r/2);
	col2.g=MAX(col2.g,col.g/2);
	col2.b=MAX(col2.b,col.b/2);
	return col2;
}

void render() {
	// subcell resolution image
	static TCODImage map(WIDTH*2,HEIGHT*2);
	// compute the map image
	for (int px=0; px <2*WIDTH; px++) {
		for (int py=0; py <2*HEIGHT; py++) {
		    // world texel coordinate (with fisheye distorsion)
		    float wx = px+curwx;
		    float wy = py+curwy;
			map.putPixel(px,py,getMapShadedColor(wx,wy,true));
		}
	}
	map.blit2x(TCODConsole::root,0,0);
	
	TCODConsole::root->setDefaultForeground(TCODColor::white);
	static const char *biomeNames[] = {
		"Tundra","Cold desert","Grassland", "Boreal forest",
		"Temperate forest", "Tropical/Montane forest",
		"Hot desert", "Savanna", "Tropical dry forest", "Tropical evergreen forest",
		"Thorn forest"
	};
	if ( worldGen.isOnSea(mx,my) ) {
		// some information are irrelevant on sea
		TCODConsole::root->print(5,47,"Alt %5dm\n\nMove the mouse to scroll the map",
			(int)worldGen.getRealAltitude(mx,my)
		);
	} else {
		TCODConsole::root->print(5,47,"Alt %5dm  Prec %3dcm/sq. m/y  Temp %d deg C\nBiome : %s\nMove the mouse to scroll the map",
			(int)worldGen.getRealAltitude(mx,my),
			(int)worldGen.getPrecipitations(mx,my),
			(int)worldGen.getTemperature(mx,my),
			biomeNames[worldGen.getBiome(mx,my)]
		);
	}
}

int main (int argc, char *argv[]) {
	// initialize the game window
	TCODConsole::initRoot(WIDTH,HEIGHT,"World generator v"VERSION, false,TCOD_RENDERER_SDL);
	TCODSystem::setFps(25);
	TCODMouse::showCursor(true);

	TCOD_key_t k = {TCODK_NONE,0};
	TCOD_mouse_t mouse;
	
	bool endCredits=false;
	// generate the world with all data (rain, temperature and so on...)
	worldGen.generate(NULL);
	// compute light intensity on ground (depends on light direction and ground slope)
	static float lightDir[3] = {
        1.0f,1.0f,0.0f
    };	
	worldGen.computeSunLight(lightDir);
	
	while (! TCODConsole::isWindowClosed()) {
		//	read keyboard
//		TCOD_key_t k=TCODConsole::checkForKeypress(TCOD_KEY_PRESSED|TCOD_KEY_RELEASED);
//		TCOD_mouse_t mouse=TCODMouse::getStatus();

		TCODSystem::checkForEvent((TCOD_event_t)(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE),&k,&mouse);

		if ( k.vk == TCODK_PRINTSCREEN ) {
			// screenshot
			if (! k.pressed ) TCODSystem::saveScreenshot(NULL);
			k.vk=TCODK_NONE;
		} else if ( k.lalt && (k.vk == TCODK_ENTER || k.vk == TCODK_KPENTER) ) {
			// switch fullscreen
			if (! k.pressed ) TCODConsole::setFullscreen(!TCODConsole::isFullscreen());
			k.vk=TCODK_NONE;
		}
		// update the game
		update(TCODSystem::getLastFrameLength(),k,mouse);

		// render the game screen
		render();
		// render libtcod credits
		if (! endCredits ) endCredits = TCODConsole::renderCredits(4,4,true);
		// flush updates to screen
		TCODConsole::root->flush();
	}
	return 0;	
}
