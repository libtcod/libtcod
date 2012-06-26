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
* THIS SOFTWARE IS PROVIDED BY Jice ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Jice BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <string.h>
#include "main.hpp"

// dummy height indicating that a cell is not water
#define NO_WATER -1000.0f
// how much wave energy is lost per second
#define DAMPING_COEF 1.3f
// wave height below which the water is considered still
#define ACTIVE_THRESHOLD 0.02f
// height on the triggering ripple
#define RIPPLE_TRIGGER 3.0f
// how many times ripples are updated per second
#define RIPPLE_FPS 10

RippleManager::RippleManager(TCODMap *waterMap) {
	init(waterMap);
}

void RippleManager::init(TCODMap *waterMap) {
	this->width=waterMap->getWidth();
	this->height=waterMap->getHeight();
	bool visited[width][height];
	memset(visited,0,sizeof(bool)*width*height);
	// first time : compute water zones
	zone.cumulatedElapsed=0.0f;
	zone.isActive=false;
	zone.data = new float[ width*height ];
	zone.oldData = new float[ width*height ];
	for (int dx=0; dx < width; dx++) {
		for (int dy=0; dy < height; dy++) {
			float value = waterMap->isWalkable(dx,dy) ? 0.0f : NO_WATER;
			// set water height to 0.0, not water cells to -1000
			zone.data[dx+dy*width] = value;
		}
	}
	memcpy(zone.oldData,zone.data,sizeof(float)*width * height);
}

void RippleManager::startRipple(int x, int y) {
	// look for the water zone
	int off=x+y*width;
	if ( zone.data[off] != NO_WATER ) {
		zone.data[off] = -RIPPLE_TRIGGER;
		zone.isActive=true;
	}
}

bool RippleManager::updateRipples(float elapsed) {
	bool updated=false;
	zone.cumulatedElapsed+=elapsed;
	if ( zone.isActive ) {
		// update the ripples
		if ( zone.cumulatedElapsed > 1.0f/RIPPLE_FPS  ) {
			zone.cumulatedElapsed=0.0f;
			// swap grids
			float *tmp=zone.data;
			zone.data=zone.oldData;
			zone.oldData=tmp;
			zone.isActive=false;
			for (int zx2=1; zx2 < width-1; zx2++) {
				for (int zy2=1; zy2 < height-1; zy2++) {
					int off = zx2 + zy2 * width;
					if (zone.data[off] != NO_WATER) {
						float sum=0.0f;
						int count=0;
						// wave smoothing + spreading
						if ( zone.oldData[off-1] != NO_WATER ) {
							sum += zone.oldData[off-1];
							count++;
						}
						if ( zone.oldData[off+1] != NO_WATER ) {
							sum += zone.oldData[off+1];
							count++;
						}
						if ( zone.oldData[off-width] != NO_WATER ) {
							sum += zone.oldData[off-width];
							count++;
						}
						if ( zone.oldData[off+width] != NO_WATER ) {
							sum += zone.oldData[off+width];
							count++;
						}
						sum = sum * 2 / count;
						zone.data[off] = sum - zone.data[off];
						// damping
						zone.data[off] *= 1.0f-DAMPING_COEF/RIPPLE_FPS;
						if ( ABS(zone.data[off]) > ACTIVE_THRESHOLD ) {
							zone.isActive=true;
							updated=true;
						}
					}
				}
			}
		}
	}

	return updated;
}

void RippleManager::renderRipples(const TCODImage *ground, TCODImage *groundWithRipples) {
	float elCoef=TCODSystem::getElapsedSeconds()*2.0f;
	for ( int x = 1; x < width-1; x++ ) {
		for ( int y = 1; y < height-1; y++ ) {
			if ( getData(x,y) != NO_WATER ) {
				float xOffset=(getData(x-1,y)-getData(x+1,y));
				float yOffset=(getData(x,y-1)-getData(x,y+1));
				float f[3]={x,y,elCoef};
				xOffset+=noise3d.get(f, TCOD_NOISE_SIMPLEX)*0.3f;
				if ( ABS(xOffset) < 250 && ABS(yOffset) < 250 ) {
					TCODColor col=ground->getPixel(x+(int)(xOffset),y+(int)(yOffset));
					col = col + TCODColor::white*xOffset*0.1f;
					groundWithRipples->putPixel(x,y,col);
				}
			}
		}
	}
}
