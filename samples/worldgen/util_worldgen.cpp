/*
* Copyright (c) 2009 Jice
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

// world generator
// this was mostly generated with libtcod 1.4.2 heightmap tool !

#include <math.h>
#include <stdio.h>
#include "main.hpp"

// temperature / precipitation Biome diagram (Whittaker diagram)
EBiome biomeDiagram[5][5] = {
    // artic/alpine climate (below -5°C)
    { TUNDRA, TUNDRA, TUNDRA, TUNDRA, TUNDRA, },
    // cold climate (-5 / 5 °C)
    { COLD_DESERT, GRASSLAND, BOREAL_FOREST, BOREAL_FOREST, BOREAL_FOREST, },
    // temperate climate (5 / 15 °C)
    { COLD_DESERT, GRASSLAND, TEMPERATE_FOREST, TEMPERATE_FOREST, TROPICAL_MONTANE_FOREST, },
    // warm climate (15 - 20°C)
    { HOT_DESERT, SAVANNA, TROPICAL_DRY_FOREST, TROPICAL_EVERGREEN_FOREST, TROPICAL_EVERGREEN_FOREST, },
    // tropical climate (above 20 °C)
    { HOT_DESERT, THORN_FOREST, TROPICAL_DRY_FOREST, TROPICAL_EVERGREEN_FOREST, TROPICAL_EVERGREEN_FOREST, },
};

static const float sandHeight=0.12f;
static const float grassHeight=0.16f; //0.315f;
static const float rockHeight=0.655f;
static const float snowHeight=0.905f; //0.785f;
// TCOD's land color map
static const int MAX_COLOR_KEY=10;
#define COLOR_KEY_MAX_SEA ((int)(sandHeight*255)-1)
#define COLOR_KEY_MIN_LAND ((int)(sandHeight*255))
static const int keyIndex[MAX_COLOR_KEY] = {0,
	COLOR_KEY_MAX_SEA,
	COLOR_KEY_MIN_LAND,
	(int)(grassHeight*255),
	(int)(grassHeight*255)+10,
	(int)(rockHeight*255),
	(int)(rockHeight*255)+10,
	(int)(snowHeight*255),
	(int)(snowHeight*255)+10,
	255
};
static const TCODColor keyColor[MAX_COLOR_KEY]= {
	TCODColor(0,0,50),    // deep water
	TCODColor(20,20,200), // water-sand transition
	TCODColor(134,180,101),// sand
	TCODColor(80,120,10),// sand-grass transition
	TCODColor(17,109,7), // grass
	TCODColor(30,85,12), // grass-rock transisiton
	TCODColor(64,70,20), // rock
	TCODColor(120,140,40), // rock-snow transisiton
	TCODColor(208,208,239), // snow
	TCODColor(255,255,255)
};

// altitude color map
static const int MAX_ALT_KEY=8;
static const int altIndexes[MAX_ALT_KEY] = {
	0,15,(int)(sandHeight*255),(int)(sandHeight*255)+1,
	80,130,195,255
	};
static const float altitudes[MAX_ALT_KEY] = {
     -2000,-1000,-100,0,500,1000,2500,4000	// in meters
	};
static const TCODColor altColors[MAX_ALT_KEY]= {
	TCODColor(24,165,255), // -2000
	TCODColor(132,214,255), // -1000
	TCODColor(247,255,255), // -100
	TCODColor(49,149,44), // 0
	TCODColor(249,209,151), // 500
	TCODColor(165,148,24), // 1000
	TCODColor(153,110,6), // 2500
	TCODColor(172,141,138), // 4000
};

// precipitation color map
static const int MAX_PREC_KEY=19;
static const int precIndexes[MAX_PREC_KEY] = {
	4,8,12,16,20,24,28,32,36,40,50,60,70,80,100,120,140,160,255
	};
static const float precipitations[MAX_PREC_KEY] = {
	0,1,2,3,4,5,6,7,8,9,10,13,15,18,20,25,30,35,40  // cm / m² / year
};
static const TCODColor precColors[MAX_PREC_KEY]= {
	TCODColor(128,0,0), // < 4
	TCODColor(173,55,0), // 4-8
	TCODColor(227,102,0), // 8-12
	TCODColor(255,149,0), // 12-16
	TCODColor(255,200,0), // 16-20
	TCODColor(255,251,0), // 20-24
	TCODColor(191,255,0), // 24-28
	TCODColor(106,251,0), // 28-32
	TCODColor(25,255,48), // 32-36
	TCODColor(48,255,141), // 36-40
	TCODColor(28,255,232), // 40-50
	TCODColor(54,181,255), // 50-60
	TCODColor(41,71,191), // 60-70
	TCODColor(38,0,255), // 70-80
	TCODColor(140,0,255), // 80-100
	TCODColor(221,0,255), // 100-120
	TCODColor(255,87,255), // 120-140
	TCODColor(255,173,255), // 140-160
	TCODColor(255,206,255), // > 160
};

// temperature color map
static const int MAX_TEMP_KEY=7;
static const int tempIndexes[MAX_TEMP_KEY] = {0,42,84,126,168,210,255};
static const int temperatures[MAX_TEMP_KEY] = {-30,-20,-10,0,10,20,30};
static const TCODColor tempKeyColor[MAX_TEMP_KEY]= {
	TCODColor(180,8,130), // -30 °C
	TCODColor(32,1,139), // -20 °C
	TCODColor(0,65,252),// -10 °C
	TCODColor(37,255,236),// 0 °C
	TCODColor(255,255,1), // 10 °C
	TCODColor(255,29,4), // 20 °C
	TCODColor(80,3,0), // 30 °C
};


int WorldGenerator::getWidth() const {
	return HM_WIDTH;
}

int WorldGenerator::getHeight() const {
	return HM_HEIGHT;
}

float WorldGenerator::getAltitude(int x, int y) const {
	return hm->getValue(x,y);
}

float WorldGenerator::getRealAltitude(float x, float y) const {
	int ih=(int)(256*getInterpolatedAltitude(x,y));
	int idx;
    ih = CLAMP(0,255,ih);
	for (idx=0; idx < MAX_ALT_KEY-1; idx++) {
		if ( altIndexes[idx+1] > ih ) break;
	}
	float alt = altitudes[idx] + (altitudes[idx+1]-altitudes[idx]) * (ih-altIndexes[idx])/(altIndexes[idx+1]-altIndexes[idx]);
	return alt;
}

float WorldGenerator::getPrecipitations(float x, float y) const {
	int iprec=(int)(256*precipitation->getValue((int)x,(int)y));
	int idx;
	iprec=CLAMP(0,255,iprec);
	for (idx=0; idx < MAX_PREC_KEY-1; idx++) {
		if ( precIndexes[idx+1] > iprec ) break;
	}
	float prec = precipitations[idx] + (precipitations[idx+1]-precipitations[idx]) * (iprec-precIndexes[idx])/(precIndexes[idx+1]-precIndexes[idx]);
	return prec;
}

float WorldGenerator::getTemperature(float x, float y) const {
	return temperature->getValue((int)x,(int)y);
}

EBiome WorldGenerator::getBiome(float x, float y) const {
	return biomeMap[(int)x+(int)y*HM_WIDTH];
}

float WorldGenerator::getInterpolatedAltitude(float x, float y) const {
	return hm->getInterpolatedValue(x,y);
}

void WorldGenerator::getInterpolatedNormal(float x, float y, float n[3]) const {
    return hm2->getNormal(x,y,n,sandHeight);
}

float WorldGenerator::getSandHeight() const {
	return sandHeight;
}

bool WorldGenerator::isOnSea(float x, float y) const {
	return getInterpolatedAltitude(x,y) <= sandHeight;
}

void WorldGenerator::addHill(int nbHill, float baseRadius, float radiusVar, float height)  {
	for (int i=0; i<  nbHill; i++ ) {
		float hillMinRadius=baseRadius*(1.0f-radiusVar);
		float hillMaxRadius=baseRadius*(1.0f+radiusVar);
		float radius = wgRng->getFloat(hillMinRadius, hillMaxRadius);
		int xh = wgRng->getInt(0,HM_WIDTH-1);
		int yh = wgRng->getInt(0,HM_HEIGHT-1);
		hm->addHill((float)xh,(float)yh,radius,height);
	}
}

void WorldGenerator::setLandMass(float landMass, float waterLevel) {
	// fix land mass. We want a proportion of landMass above sea level
#ifndef NDEBUG	
	float t0=TCODSystem::getElapsedSeconds();
#endif	
	int heightcount[256];
	memset(heightcount,0,sizeof(heightcount));
	for ( int x=0; x < HM_WIDTH; x++) {
	    for (int y=0; y < HM_HEIGHT; y++) {
			float h=hm->getValue(x,y);
			int ih=(int)(h*255);
			ih = CLAMP(0,255,ih);
			heightcount[ih]++;
		}
	}
	int i=0, totalcount=0;
	while (totalcount < HM_WIDTH*HM_HEIGHT*(1.0f-landMass) ) {
		totalcount += heightcount[i];
		i++;
	}
	float newWaterLevel=i/255.0f;
	float landCoef = (1.0f-waterLevel)/(1.0f-newWaterLevel);
	float waterCoef = waterLevel / newWaterLevel;
	// water level should be rised/lowered to newWaterLevel
	for ( int x=0; x < HM_WIDTH; x++) {
	    for (int y=0; y < HM_HEIGHT; y++) {
			float h=hm->getValue(x,y);
			if ( h > newWaterLevel ) {
				h = waterLevel + (h-newWaterLevel)*landCoef;
			} else {
				h = h * waterCoef;
			}
			hm->setValue(x,y,h);
		}
	}
#ifndef NDEBUG	
	float t1=TCODSystem::getElapsedSeconds();
	DBG(("  Landmass... %g\n", t1-t0 ));
#endif
}

// function building the heightmap
void WorldGenerator::buildBaseMap() {
	float t0=TCODSystem::getElapsedSeconds();
	addHill(600,16.0*HM_WIDTH/200,0.7,0.3);
	hm->normalize();
	float t1=TCODSystem::getElapsedSeconds();
	DBG(("  Hills... %g\n", t1-t0 ));
	t0=t1;

	hm->addFbm(noise,2.20*HM_WIDTH/400,2.20*HM_WIDTH/400,0,0,10.0f,1.0,2.05);
	hm->normalize();
	hm2->copy(hm);
	t1=TCODSystem::getElapsedSeconds();
	DBG(("  Fbm... %g\n", t1-t0 ));
	t0=t1;

    setLandMass(0.6f,sandHeight);

	// fix land/mountain ratio using x^3 curve above sea level
	for ( int x=0; x < HM_WIDTH; x++) {
	    for (int y=0; y < HM_HEIGHT; y++) {
			float h=hm->getValue(x,y);
			if ( h >= sandHeight ) {
                float coef = (h-sandHeight) / (1.0f - sandHeight);
			    h = sandHeight + coef * coef * coef * (1.0f - sandHeight);
			    hm->setValue(x,y,h);
			}
	    }
	}
	t1=TCODSystem::getElapsedSeconds();
	DBG(("  Flatten plains... %g\n", t1-t0 ));
	t0=t1;


	// we use a custom erosion algo
	//hm->rainErosion(15000*HM_WIDTH/400,0.03,0.01,wgRng);
	//t1=TCODSystem::getElapsedSeconds();
	//DBG(("  Erosion... %g\n", t1-t0 ));
	//t0=t1;
	// compute clouds
	float f[2];
	for ( int x=0; x < HM_WIDTH; x++) {
        f[0] = 6.0f*((float)(x) / HM_WIDTH);
	    for (int y=0; y < HM_HEIGHT; y++) {
            f[1] = 6.0f*((float)(y) / HM_HEIGHT);
            // clouds[x][y] = 0.5f * (1.0f + 0.8f * noise->getFbmSimplex(f,4.0f));
			clouds[x][y] = 0.5f * (1.0f + 0.8f * noise->getFbm(f,4.0f,TCOD_NOISE_SIMPLEX));
	    }
	}
	t1=TCODSystem::getElapsedSeconds();
	DBG(("  Init clouds... %g\n", t1-t0 ));
	t0=t1;
}

// function blurring the heightmap
void WorldGenerator::smoothMap() {
	// 3x3 kernel for smoothing operations
	static const int smoothKernelSize=9;
	static const int smoothKernelDx[9]={-1,0,1,-1,0,1,-1,0,1};
	static const int smoothKernelDy[9]={-1,-1,-1,0,0,0,1,1,1};
	static const float smoothKernelWeight[9]={2,8,2,8,20,8,2,8,2};

#ifndef NDEBUG	
	float t0=TCODSystem::getElapsedSeconds();
#endif
	hm->kernelTransform(smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-1000,1000);
	hm2->kernelTransform(smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-1000,1000);
	hm->normalize();
#ifndef NDEBUG	
	float t1=TCODSystem::getElapsedSeconds();
	DBG(("  Blur... %g\n", t1-t0 ));
#endif
}

static const int dirx[9] = { 0, -1,0,1,-1,1,-1,0,1 };
static const int diry[9] = { 0, -1,-1,-1,0,0,1,1,1 };
static const float dircoef[9] = { 1.0f, 1.0f/1.414f, 1.0f, 1.0f/1.414f,1.0f,1.0f,1.0f/1.414f, 1.0f,1.0f/1.414f };
static const int oppdir[9] = {0, 8, 7, 6, 5, 4, 3, 2, 1 };

// erosion parameters
#define EROSION_FACTOR 0.01f
#define SEDIMENTATION_FACTOR 0.01f
#define MAX_EROSION_ALT 0.9f
#define MUDSLIDE_COEF 0.4f

void WorldGenerator::erodeMap() {
	TCODHeightMap newMap(HM_WIDTH,HM_HEIGHT);
	for (int i=5; i != 0; i --) {
		// compute flow and slope maps
		map_data_t *md = mapData;
		for (int y=0; y < HM_HEIGHT; y++) {
            for (int x=0; x < HM_WIDTH; x++) {
				float h=hm->getValue(x,y);
				float hmin=h, hmax=h;
				int minDir=0, maxDir=0;
				for (int i=1; i< 9; i++ ) {
					int ix = x+dirx[i];
					int iy = y+diry[i];
					if ( IN_RECTANGLE(ix,iy,HM_WIDTH,HM_HEIGHT)) {
						float h2=hm->getValue(ix,iy);
						if ( h2 < hmin ) {
							hmin=h2;
							minDir=i;
						} else if ( h2 > hmax ) {
							hmax=h2;
							maxDir=i;
						}
					}
				}
				md->flowDir = minDir;
				md->upDir = maxDir;
				float slope = hmin - h; // this is negative
				slope *= dircoef[minDir];
				md->slope = slope;
				md++;
			}
		}

		md=mapData;
		for (int y=0; y < HM_HEIGHT; y++) {
            for (int x=0; x < HM_WIDTH; x++) {
                float sediment=0.0f;
                bool end=false;
                int ix=x,iy=y;
                uint8 oldFlow=md->flowDir;
                map_data_t *md2=md;
                while ( !end ) {
                    float h = hm->getValue(ix,iy);
                    if (h < sandHeight-0.01f) break;
                    if ( md2->flowDir == oppdir[oldFlow] ) {
                        h += SEDIMENTATION_FACTOR * sediment;
                        hm->setValue(ix,iy,h);
                        end = true;
                    } else {
                        // remember, slope is negative
                        h += precipitation->getValue(ix,iy)* EROSION_FACTOR * md2->slope;
                        h=MAX(h,sandHeight);
                        sediment -= md2->slope;
                        hm->setValue(ix,iy,h);
                        oldFlow = md2->flowDir;
                        ix += dirx[oldFlow];
                        iy += diry[oldFlow];
                        md2 = &mapData[ix+iy*HM_WIDTH];
                    }
                }
                md++;
			}
        }
		DBG( ("  Erosion pass %d\n",i));

		// mudslides (smoothing)
		float sandCoef = 1.0f/(1.0f-sandHeight);
		for (int x=0; x < HM_WIDTH; x++) {
			for (int y=0; y < HM_HEIGHT; y++) {
				float h=hm->getValue(x,y);
				if ( h < sandHeight-0.01f || h >= MAX_EROSION_ALT ) {
					newMap.setValue(x,y,h);
					continue;
				}
				float sumDelta1 = 0.0f, sumDelta2 = 0.0f;
				int nb1 = 1, nb2 = 1;
				for (int i=1; i < 9; i++ ) {
					int ix = x+dirx[i];
					int iy = y+diry[i];
					if ( IN_RECTANGLE(ix,iy,HM_WIDTH,HM_HEIGHT)) {
						float ih = hm->getValue(ix,iy);
						if ( ih < h ) {
							if ( i ==1 || i == 3 || i == 6 || i == 8 ) {
								// diagonal neighbour
								sumDelta1 += (ih - h)*0.4f;
								nb1++;
							} else {
								// adjacent neighbour
								sumDelta2 += (ih -h)*1.6f;
								nb2++;
							}
						}
					}
				}
				// average height difference with lower neighbours
				float dh = sumDelta1/nb1 + sumDelta2/nb2;
				dh *= MUDSLIDE_COEF;
				float hcoef=(h-sandHeight)* sandCoef;
				dh *= (1.0f-hcoef*hcoef*hcoef); // less smoothing at high altitudes

				newMap.setValue(x,y,h+dh);
			}
		}
		hm->copy(&newMap);
	}

}

// interpolated cloud thickness
float WorldGenerator::getCloudThickness(float x, float y) const {
    x += cloudDx;
    int ix=(int)x;
    int iy=(int)y;
    int ix1 = MIN(HM_WIDTH-1,ix+1);
    int iy1 = MIN(HM_HEIGHT-1,iy+1);
    float fdx = x - ix;
    float fdy = y - iy;
    float v1 = clouds[ix][iy];
    float v2 = clouds[ix1][iy];
    float v3 = clouds[ix][iy1];
    float v4 = clouds[ix1][iy1];
    float vx1 = ((1.0f - fdx) * v1 + fdx * v2);
    float vx2 = ((1.0f - fdx) * v3 + fdx * v4);
    float v = ((1.0f - fdy) * vx1 + fdy * vx2);
    return v;
}

TCODColor WorldGenerator::getMapColor(float h) {
	int colorIdx;
	if ( h < sandHeight ) colorIdx = (int)(h/sandHeight * COLOR_KEY_MAX_SEA);
	else colorIdx = COLOR_KEY_MIN_LAND + (int)((h-sandHeight)/(1.0f-sandHeight) * (255-COLOR_KEY_MIN_LAND));
	colorIdx=CLAMP(0,255,colorIdx);
	return mapGradient[colorIdx];
}

void WorldGenerator::computeSunLight(float lightDir[3]) {
	for (int x=0; x < HM_WIDTH; x++) {
		for (int y=0; y < HM_HEIGHT; y++) {
			worldint[x+y*HM_WIDTH]=getMapIntensity(x+0.5f,y+0.5f,lightDir);
		}
	}
}


float WorldGenerator::getMapIntensity(float worldX,float worldY, float lightDir[3]) {
    // sun color & direction
    static const TCODColor sunCol(255,255,160);
	float normal[3];
    float wx = CLAMP(0.0f, HM_WIDTH-1,worldX);
    float wy = CLAMP(0.0f, HM_HEIGHT-1,worldY);
	// apply sun light
    getInterpolatedNormal(wx,wy,normal);
normal[2] *= 3.0f;
    float intensity = 0.75f
		- (normal[0]*lightDir[0]+normal[1]*lightDir[1]+normal[2]*lightDir[2])*0.75f;
	intensity=CLAMP(0.75f,1.5f,intensity);
	return intensity;
}

TCODColor WorldGenerator::getInterpolatedColor(float worldX,float worldY) {
	return getInterpolatedColor(worldmap,worldX,worldY);
}

TCODColor WorldGenerator::getInterpolatedColor(TCODImage *img,float x,float y) {
	int w,h;
	img->getSize(&w,&h);
	float wx = CLAMP(0.0f, w-1,x);
	float wy = CLAMP(0.0f, h-1,y);
	int iwx = (int)wx;
	int iwy = (int)wy;
	float dx = wx - iwx;
	float dy = wy - iwy;

	TCODColor colNW = img->getPixel(iwx,iwy);
	TCODColor colNE = (iwx < w-1 ? img->getPixel(iwx+1,iwy) : colNW);
	TCODColor colSW = (iwy < h-1 ? img->getPixel(iwx,iwy+1) : colNW);
	TCODColor colSE = (iwx < w-1 && iwy < h-1 ? img->getPixel(iwx+1,iwy+1) : colNW);
	TCODColor colN = TCODColor::lerp(colNW,colNE,dx);
	TCODColor colS = TCODColor::lerp(colSW,colSE,dx);
	TCODColor col = TCODColor::lerp(colN,colS,dy);
	return col;
}


float WorldGenerator::getInterpolatedIntensity(float worldX, float worldY) {
	return getInterpolatedFloat(worldint,worldX,worldY,HM_WIDTH,HM_HEIGHT);
}

void WorldGenerator::updateClouds(float elapsedTime) {
    cloudTotalDx += elapsedTime * 5;
    cloudDx += elapsedTime * 5;
    if ( cloudDx >= 1.0f ) {
        int colsToTranslate=(int)cloudDx;
        cloudDx -= colsToTranslate;
        // translate the cloud map
        for ( int x=colsToTranslate; x < HM_WIDTH; x++) {
            for (int y=0; y < HM_HEIGHT; y++) {
                clouds[x-colsToTranslate][y]=clouds[x][y];
            }
        }
        // compute a new column
        float f[2];
        float cdx = (int)cloudTotalDx ;
        for ( int x=HM_WIDTH-colsToTranslate; x < HM_WIDTH; x++) {
            for (int y=0; y < HM_HEIGHT; y++) {
                f[0] = 6.0f*((float)(x+cdx) / HM_WIDTH);
                f[1] = 6.0f*((float)(y) / HM_HEIGHT);
                // clouds[x][y] = 0.5f * (1.0f + 0.8f * noise->getFbmSimplex(f,4.0f));
                clouds[x][y] = 0.5f * (1.0f + 0.8f * noise->getFbm(f,4.0f,TCOD_NOISE_SIMPLEX));
            }
        }
    }
}

class RiverPathCbk : public ITCODPathCallback {
public:
    float getWalkCost( int xFrom, int yFrom, int xTo, int yTo, void *userData ) const {
        WorldGenerator *world = (WorldGenerator *)userData;
        float h1=world->hm->getValue(xFrom,yFrom);
        float h2=world->hm->getValue(xTo,yTo);
        if ( h2 < sandHeight ) return 0.0f;
//        float f[2] = {xFrom*10.0f/HM_WIDTH,yFrom*10.0f/HM_HEIGHT};
//        return (1.0f+h2-h1)*10+5*(1.0f+noise2d.getSimplex(f));
        return (0.01f+h2-h1)*100;
    }
};

/*
void WorldGenerator::generateRivers() {
    static int riverId=0;
    static RiverPathCbk cbk;
//    static TCODPath *path=NULL;
    static TCODDijkstra *path=NULL;
	// the source
	int sx,sy;
	// the destination
	int dx=-1,dy=-1;
	// get a random point near the coast
	sx = wgRng->getInt(0,HM_WIDTH-1);
	sy = wgRng->getInt(0,HM_HEIGHT-1);
	float h = hm->getValue(sx,sy);
	while ( h <  sandHeight - 0.02 || h >= sandHeight ) {
		sx++;
		if ( sx == HM_WIDTH ) {
			sx=0;
			sy++;
			if ( sy == HM_HEIGHT ) sy=0;
		}
		h = hm->getValue(sx,sy);
	}
	riverId++;
	// get a closes mountain point
	float minDist=1E10f;
	int minx = sx - HM_WIDTH/4;
	int maxx = sx + HM_WIDTH/4;
	int miny = sy - HM_HEIGHT/4;
	int maxy = sy + HM_HEIGHT/4;
	minx = MAX(0,minx);
	maxx = MIN(HM_WIDTH-1,maxx);
	miny = MAX(0,miny);
	maxy = MIN(HM_HEIGHT-1,maxy);
	h = MIN(snowHeight,h + wgRng->getFloat(0.1f,0.5f));
	for (int y=miny; y < maxy; y++) {
        for (int x=minx; x < maxx; x++) {
            float dh=hm->getValue(x,y);
            if ( dh >= h ) {
                dx=x;
                dy=y;
                break;
            }
        }
	}

	if (! path) {
//	    path = new TCODPath(HM_WIDTH,HM_HEIGHT,&cbk,this);
	    path = new TCODDijkstra(HM_WIDTH,HM_HEIGHT,&cbk,this);
	}
	path->compute(dx,dy);
//	if ( dx >= 0 && path->compute(dx,dy,sx,sy) ) {
	if ( dx >= 0 ) { path->setPath(sx,sy) ;
        DBG( ("river : %d %d -> %d %d\n",sx,sy,dx,dy));
        int x,y;
	    while (path->walk(&x,&y)) {
            map_data_t *md=&mapData[x+y*HM_WIDTH];
            if ( md->riverId != 0 ) break;
            md->riverId = riverId;
	    }
	}

}
*/

void WorldGenerator::generateRivers() {
    static int riverId=0;
	// the source
	int sx,sy;
	// the destination
	int dx,dy;
	// get a random point near the coast
	sx = wgRng->getInt(0,HM_WIDTH-1);
	sy = wgRng->getInt(HM_HEIGHT/5,4*HM_HEIGHT/5);
	float h = hm->getValue(sx,sy);
	while ( h <  sandHeight - 0.02 || h >= sandHeight ) {
		sx++;
		if ( sx == HM_WIDTH ) {
			sx=0;
			sy++;
			if ( sy == HM_HEIGHT ) sy=0;
		}
		h = hm->getValue(sx,sy);
	}
	TCODList<int> tree;
	TCODList<int> randPt;
	tree.push(sx+sy*HM_WIDTH);
	riverId++;
	dx = sx;
	dy = sy;
	for (int i=0; i< wgRng->getInt(50,200); i++) {
	    int rx = wgRng->getInt(sx-200,sx+200);
	    int ry = wgRng->getInt(sy-200,sy+200);
//	    if ( IN_RECTANGLE(rx,ry,HM_WIDTH,HM_HEIGHT) ) {
//	        float h=hm->getValue(rx,ry);
//	        if ( h >= sandHeight ) {
	            randPt.push(rx+ry*HM_WIDTH);
//	        }
//	    }
	}
    for (int i=0; i < randPt.size(); i++) {
        int rx = randPt.get(i)%HM_WIDTH;
        int ry = randPt.get(i)/HM_WIDTH;

        float minDist=1E10;
        int bestx=-1,besty=-1;
        for (int j=0;j < tree.size(); j++) {
            int tx=tree.get(j)%HM_WIDTH;
            int ty=tree.get(j)/HM_WIDTH;
            float dist = (tx-rx)*(tx-rx)+(ty-ry)*(ty-ry);
            if ( dist < minDist) {
                minDist=dist;
                bestx=tx;
                besty=ty;
            }
        }
        TCODLine::init(bestx,besty,rx,ry);
        int len = 3,cx=bestx,cy=besty;
        map_data_t *md=&mapData[cx+cy*HM_WIDTH];
        if (md->riverId == riverId ) md->riverId=0;
        do {
            md=&mapData[cx+cy*HM_WIDTH];
            if (md->riverId > 0 ) return;
	        float h=hm->getValue(cx,cy);
	        if ( h >= sandHeight ) {
                md->riverId = riverId;
                precipitation->setValue(cx,cy,1.0f);
	        }
	        if (cx ==0 || cx == HM_WIDTH-1 || cy == 0 || cy == HM_HEIGHT-1 ) len = 0;
            else if (TCODLine::step(&cx,&cy)) len=0;
            len --;
        } while(len > 0 );
        int newNode = cx+cy*HM_WIDTH;
        if (newNode != bestx+besty*HM_WIDTH ) {
            tree.push(newNode);
        }
    }
}


/*
void WorldGenerator::generateRivers() {
    static int riverId=0;
	// the source
	int sx,sy;
	// the destination
	int dx,dy;
	// get a random point near the coast
	sx = wgRng->getInt(0,HM_WIDTH-1);
	sy = wgRng->getInt(HM_HEIGHT/5,4*HM_HEIGHT/5);
	float h = hm->getValue(sx,sy);
	map_data_t *md=&mapData[sx+sy*HM_WIDTH];
	while ( md->riverId == 0 && (h <  sandHeight - 0.02 || h >= sandHeight) ) {
		sx++;
		if ( sx == HM_WIDTH ) {
			sx=0;
			sy++;
			if ( sy == HM_HEIGHT ) sy=0;
		}
		h = hm->getValue(sx,sy);
		md=&mapData[sx+sy*HM_WIDTH];
	}
	riverId++;
	dx = sx;
	dy = sy;
	DBG( ("source : %d %d\n",sx,sy));
	// travel down to the see
	// get the hiwest point around current position
	bool deadEnd=false;
	int len=0;
	river_t *river=new river_t();
	rivers.push(river);
	int maxlen=HM_WIDTH,lastdx=1,lastdy=1;
	do {
        int coord = sx + sy*HM_WIDTH;
	    map_data_t *md=&mapData[coord];
	    if ( md->riverId != 0 ) {
	        river_t *joined = rivers.get(md->riverId-1);
	        int i=0;
	        while (joined->coords.get(i) != coord ) i++;
	        while ( i < joined->coords.size() ) {
	            int newStrength=joined->strength.get(i)+1;
	            joined->strength.set(newStrength,i);
	            i++;
	        }
	        break;
	    }
        md->riverId = riverId;
        md->riverLength = len++;
        river->coords.push(coord);
        river->strength.push(1);
		if ( md->upDir != 0 ) {
		    lastdx=dirx[md->upDir];
			sx += lastdx;
			lastdy=diry[md->upDir];
			sy += lastdy;
			deadEnd=false;
		} else if ( deadEnd ) {
		    break;
		} else {
			sx += lastdx;
			sy += lastdy;
			if ( ! IN_RECTANGLE(sx,sy,HM_WIDTH,HM_HEIGHT ) ) break;
			deadEnd=true;
		}
		h=hm->getValue(sx,sy);
		maxlen--;
	} while ( maxlen > 0 && h <= snowHeight);

}
*/

EClimate WorldGenerator::getClimateFromTemp(float temp) {
    if ( temp <= -5 ) return ARTIC_ALPINE;
    if ( temp <= 5 ) return COLD;
    if ( temp <= 15 ) return TEMPERATE;
    if ( temp <= 20 ) return WARM;
    return TROPICAL;
}

float WorldGenerator::getInterpolatedFloat(float *arr,float x,float y, int width, int height) {
	float wx = CLAMP(0.0f, width-1,x);
	float wy = CLAMP(0.0f, height-1,y);
	int iwx = (int)wx;
	int iwy = (int)wy;
	float dx = wx - iwx;
	float dy = wy - iwy;

	float iNW = arr[iwx+iwy*width];
	float iNE = (iwx < width-1 ? arr[iwx+1+iwy*width] : iNW);
	float iSW = (iwy < height-1 ? arr[iwx+(iwy+1)*width] : iNW);
	float iSE = (iwx < width-1 && iwy < height-1 ? arr[iwx+1+(iwy+1)*width] : iNW);
	float iN = (1.0f-dx)*iNW + dx*iNE;
	float iS = (1.0f-dx)*iSW + dx*iSE;
	return (1.0f-dy)*iN + dy * iS;
}

int WorldGenerator::getRiverStrength(int riverId) {
    //river_t *river = rivers.get(riverId-1);
    //return river->strength.get(river->strength.size()-1);
    return 2;
}

void WorldGenerator::computePrecipitations() {
    static const float waterAdd = 0.03f;
    static const float slopeCoef = 2.0f;
    static const float basePrecip = 0.01f; // precipitation coef when slope == 0
	float t0=TCODSystem::getElapsedSeconds();
    // north/south winds
    for (int diry=-1; diry <= 1; diry += 2 ) {
        for (int x=0; x < HM_WIDTH; x++) {
            float noisex = (float)(x)*5/HM_WIDTH;
            // float waterAmount=(1.0f+noise1d.getFbmSimplex(&noisex,3.0f));
            float waterAmount=(1.0f+noise1d.getFbm(&noisex,3.0f,TCOD_NOISE_SIMPLEX));
            int starty = (diry == -1 ? HM_HEIGHT-1 : 0);
            int endy = (diry == -1 ? -1 : HM_HEIGHT);
            for (int y=starty; y != endy; y += diry) {
                float h = hm->getValue(x,y);
                if ( h < sandHeight ) {
                    waterAmount += waterAdd;
                } else if (waterAmount > 0.0f ){
                    float slope;
                    if ( (unsigned)(y + diry) < (unsigned)HM_HEIGHT ) slope = hm->getValue(x,y+diry) - h;
                    else slope = h - hm->getValue(x,y-diry);
                    if ( slope >= 0.0f ) {
                        float precip = waterAmount * (basePrecip + slope * slopeCoef);
                        precipitation->setValue(x,y,precipitation->getValue(x,y)+precip);
                        waterAmount -= precip;
                        waterAmount = MAX(0.0f,waterAmount);
                    }
                }
            }
        }
    }
	float t1=TCODSystem::getElapsedSeconds();
	DBG(("  North/south winds... %g\n", t1-t0 ));
	t0=t1;

    // east/west winds
    for (int dirx=-1; dirx <= 1; dirx += 2 ) {
        for (int y=0; y < HM_HEIGHT; y++) {
            float noisey = (float)(y)*5/HM_HEIGHT;
            // float waterAmount=(1.0f+noise1d.getFbmSimplex(&noisey,3.0f));
            float waterAmount=(1.0f+noise1d.getFbm(&noisey,3.0f,TCOD_NOISE_SIMPLEX));
            int startx = (dirx == -1 ? HM_WIDTH-1 : 0);
            int endx = (dirx == -1 ? -1 : HM_WIDTH);
            for (int x=startx; x != endx; x += dirx) {
                float h = hm->getValue(x,y);
                if ( h < sandHeight ) {
                    waterAmount += waterAdd;
                } else if (waterAmount > 0.0f ){
                    float slope;
                    if ( (unsigned)(x + dirx) < (unsigned)HM_WIDTH ) slope = hm->getValue(x+dirx,y) - h;
                    else slope = h - hm->getValue(x-dirx,y);
                    if ( slope >= 0.0f ) {
                        float precip = waterAmount * (basePrecip + slope * slopeCoef);
                        precipitation->setValue(x,y,precipitation->getValue(x,y)+precip);
                        waterAmount -= precip;
                        waterAmount = MAX(0.0f,waterAmount);
                    }
                }
            }
        }
    }
	t1=TCODSystem::getElapsedSeconds();
	DBG(("  East/west winds... %g\n", t1-t0 ));
	t0=t1;

	float min,max;
	precipitation->getMinMax(&min,&max);

    // latitude impact
	for (int y=HM_HEIGHT/4; y < 3*HM_HEIGHT/4; y++) {
        // latitude (0 : equator, -1/1 : pole)
        float lat = (float)(y-HM_HEIGHT/4) * 2 / HM_HEIGHT ;
        float coef = sinf(2*3.1415926*lat );
        for (int x=0; x < HM_WIDTH; x++) {
            float f[2] = { (float)(x)/HM_WIDTH, (float)(y)/HM_HEIGHT };
            // float xcoef = coef + 0.5f*noise2d.getFbmSimplex(f,3.0f);
            float xcoef = coef + 0.5f*noise2d.getFbm(f,3.0f,TCOD_NOISE_SIMPLEX);
            float precip = precipitation->getValue(x,y);
            precip +=  (max-min) * xcoef * 0.1f;
            precipitation->setValue(x,y,precip);
		}
	}
	t1=TCODSystem::getElapsedSeconds();
	DBG(("  latitude... %g\n", t1-t0 ));
	t0=t1;

	// very fast blur by scaling down and up
	static const int factor=8;
	static const int smallWidth = (HM_WIDTH+factor-1)/factor;
	static const int smallHeight = (HM_HEIGHT+factor-1)/factor;
	float *lowResMap = new float[smallWidth * smallHeight];
	memset(lowResMap,0,sizeof(float)*smallWidth*smallHeight);
	for (int x=0; x < HM_WIDTH; x++) {
		for (int y=0; y < HM_HEIGHT; y++) {
			float v = precipitation->getValue(x,y);
			int ix=x/factor;
			int iy=y/factor;
			lowResMap[ix + iy*smallWidth ] += v;
		}
	}
	float coef = 1.0f/factor;
	for (int x=0; x < HM_WIDTH; x++) {
		for (int y=0; y < HM_HEIGHT; y++) {
			float v=getInterpolatedFloat(lowResMap,x*coef,y*coef,smallWidth,smallHeight);
			precipitation->setValue(x,y,v);
		}
	}
	delete [] lowResMap;


}

void WorldGenerator::smoothPrecipitations() {
	float t0=TCODSystem::getElapsedSeconds();

	// better quality polishing blur using a 5x5 kernel
	// faster than TCODHeightmap kernelTransform function
	TCODHeightMap temphm(HM_WIDTH, HM_HEIGHT);
	temphm.copy(precipitation);
	for (int i=4; i != 0; i--) {
		for (int x=0; x < HM_WIDTH; x++) {
			int minx = x - 2;
			int maxx = x + 2;
			int miny=0;
			int maxy=2;
			float sum=0.0f;
			int count=0;
			minx = MAX( 0, minx );
			maxx = MIN( HM_WIDTH-1, maxx);
			// compute the kernel sum at x,0
			for (int ix=minx; ix <= maxx; ix++) {
				for (int iy =miny; iy <= maxy; iy++) {
					sum += precipitation->getValue(ix,iy);
					count++;
				}
			}
			temphm.setValue(x,0,sum/count);
			for (int y=1; y < HM_HEIGHT; y++) {
				if ( y-2 >= 0 ) {
					// remove the top-line sum
					for (int ix=minx; ix <= maxx; ix++) {
						sum -= precipitation->getValue(ix,y-2);
						count--;
					}
				}
				if ( y+2 < HM_HEIGHT ) {
					// add the bottom-line sum
					for (int ix=minx; ix <= maxx; ix++) {
						sum += precipitation->getValue(ix,y+2);
						count++;
					}
				}
				temphm.setValue(x,y,sum/count);
			}
		}
	}
	precipitation->copy(&temphm);

	float t1=TCODSystem::getElapsedSeconds();
	DBG(("  Blur... %g\n", t1-t0 ));
	t0=t1;

	precipitation->normalize();
	t1=TCODSystem::getElapsedSeconds();
	DBG(("  Normalization... %g\n", t1-t0 ));
	t0=t1;

}

void WorldGenerator::computeTemperaturesAndBiomes() {
	// temperature shift with altitude : -25°C at 6000 m
	// mean temp at sea level : 25°C at lat 0  5°C at lat 45 -25°C at lat 90 (sinusoide)
	float sandCoef=1.0f/(1.0f-sandHeight);
	float waterCoef=1.0f/sandHeight;
	for (int y=0; y < HM_HEIGHT; y++) {
		float lat = (float)(y-HM_HEIGHT/2) * 2 / HM_HEIGHT ;
		float latTemp = 0.5f*(1.0f+pow(sin(3.1415926*(lat+0.5f)),5)); // between 0 and 1
		if ( latTemp > 0.0f) latTemp = sqrt(latTemp);
		latTemp = -30 + latTemp*60;
		for (int x=0; x < HM_WIDTH; x++) {
			float h0 = hm->getValue(x,y);
			float h = h0 - sandHeight;
			if (h < 0.0f ) h *= waterCoef;
			else h *= sandCoef;
			float altShift = -35 * h;
			float temp=latTemp+altShift;
			temperature->setValue(x,y,temp);
			float humid = precipitation->getValue(x,y);
			// compute biome
			EClimate climate = getClimateFromTemp(temp);
			int iHumid = (int)(humid * 5);
			iHumid = MIN(4,iHumid);
			EBiome biome = biomeDiagram[climate][iHumid];
			biomeMap[x+y*HM_WIDTH]=biome;
		}
	}
	float min,max;
	temperature->getMinMax(&min,&max);
	DBG( ("Temperatures min/max: %g / %g\n",min,max));
}

TCODColor WorldGenerator::getBiomeColor(EBiome biome,int x,int y) {
    static const TCODColor biomeColors[] = {
    // TUNDRA,
    TCODColor(200,240,255),
    // COLD_DESERT,
    TCODColor(180,210,210),
    // GRASSLAND,
    TCODColor::sea,
    // BOREAL_FOREST,
    TCODColor(14,93,43),
    // TEMPERATE_FOREST,
    TCODColor(44,177,83),
    // TROPICAL_MONTANE_FOREST,
    TCODColor(185,232,164),
    // HOT_DESERT,
    TCODColor(235,255,210),
    // SAVANNA,
    TCODColor(255,205,20),
    // TROPICAL_DRY_FOREST,
    TCODColor(60,130,40),
    // TROPICAL_EVERGREEN_FOREST,
    TCODColor::green,
    // THORN_FOREST,
    TCODColor(192,192,112),
    };
    int r=0,g=0,b=0, count=1;
    r +=biomeColors[biome].r;
    g +=biomeColors[biome].g;
    b +=biomeColors[biome].b;
    for (int i = 0; i < 4; i++ ) {
        int ix=x+wgRng->getInt(-10,10);
        int iy=y+wgRng->getInt(-10,10);
        if ( IN_RECTANGLE(ix,iy,HM_WIDTH,HM_HEIGHT) ) {
            TCODColor c=biomeColors[biomeMap[ix+iy*HM_WIDTH]];
            r+=c.r + wgRng->getInt(-10,10);
            g+=c.g + wgRng->getInt(-10,10);
            b+=c.b + wgRng->getInt(-10,10);
            count++;
        }
    }
    r/=count;
    g/=count;
    b/=count;
    r=CLAMP(0,255,r);
    g=CLAMP(0,255,g);
    b=CLAMP(0,255,b);
    return TCODColor(r,g,b);
}

void WorldGenerator::computeColors() {
	// alter map color using temperature & precipitation maps
	map_data_t *md=mapData;
	for (int y=0; y < HM_HEIGHT; y++) {
        for (int x=0; x < HM_WIDTH; x++) {
			float h=hm->getValue(x,y);
			float temp = temperature->getValue(x,y);
			EBiome biome = biomeMap[x+y*HM_WIDTH];
			TCODColor c;
			if (h < sandHeight ) c = getMapColor(h);
			else {
				c = getMapColor(h);
                c=TCODColor::lerp(c,getBiomeColor(biome,x,y),0.5f);
			}

			// snow near poles
			temp += 10*(clouds[HM_WIDTH-1-x][HM_HEIGHT-1-y]); // cheap 2D noise ;)
			if ( temp < -10.0f && h < sandHeight ) worldmap->putPixel(x,y,TCODColor::lerp(TCODColor::white,c,0.3f));
			else if ( temp < -8.0f && h < sandHeight ) worldmap->putPixel(x,y,TCODColor::lerp(TCODColor::white,c,0.3f + 0.7f * (10.0f+temp)/2.0f));
			else if ( temp < -2.0f && h >= sandHeight) worldmap->putPixel(x,y,TCODColor::white);
			else if ( temp < 2.0f && h >= sandHeight ) {
				//TCODColor snow = mapGradient[(int)(snowHeight*255) + (int)((255 - (int)(snowHeight*255)) * (0.6f-temp)/0.4f)];
				c = TCODColor::lerp(TCODColor::white,c,(temp+2)/4.0f);
				worldmap->putPixel(x,y,c);
			} else {
				worldmap->putPixel(x,y,c);
			}
			md++;
		}
	}
	// draw rivers
	/*
	for (river_t **it=rivers.begin(); it != rivers.end(); it++) {
	    for (int i=0; i < (*it)->coords.size(); i++ ) {
	        int coord = (*it)->coords.get(i);
	        int strength = (*it)->strength.get(i);
	        int x = coord % HM_WIDTH;
	        int y = coord / HM_WIDTH;
	        TCODColor c= worldmap->getPixel(x,y);
	        c = TCODColor::lerp(c,TCODColor::blue,(float)(strength)/5.0f);
	        worldmap->putPixel(x,y,c);
	    }
	}
	*/
	md=mapData;
	for (int y=0; y < HM_HEIGHT; y++) {
        for (int x=0; x < HM_WIDTH; x++) {
            if ( md->riverId > 0 ) {
                TCODColor c= worldmap->getPixel(x,y);
                c = TCODColor::lerp(c,TCODColor::blue,0.3f);
                worldmap->putPixel(x,y,c);
            }
            md++;
        }
	}
	// blur
	static const int dx[]={0,-1,0,1,0};
	static const int dy[]={0,0,-1,0,1};
	static const int coef[]={1,2,2,2,2};
	for (int x=0; x < HM_WIDTH; x++) {
		for (int y=0; y < HM_HEIGHT; y++) {
	        int r=0,g=0,b=0,count=0;
		    for (int i=0; i < 5; i++) {
		        int ix=x+dx[i];
		        int iy=y+dy[i];
		        if (IN_RECTANGLE(ix,iy,HM_WIDTH,HM_HEIGHT)){
		            TCODColor c=worldmap->getPixel(ix,iy);
		            r += coef[i]*c.r;
		            g += coef[i]*c.g;
		            b += coef[i]*c.b;
		            count += coef[i];
		        }
		    }
            r /= count;
            g /= count;
            b /= count;
            worldmap->putPixel(x,y,TCODColor(r,g,b));
		}
	}
	drawCoasts(worldmap);
}

void WorldGenerator::generate(TCODRandom *wRng) {
	float t00,t0=TCODSystem::getElapsedSeconds();
	t00=t0;
    cloudDx=cloudTotalDx=0.0f;
	TCODColor::genMap(mapGradient,MAX_COLOR_KEY,keyColor,keyIndex);
	if ( wRng == NULL ) wRng=TCODRandom::getInstance();
	wgRng = wRng;
	noise=new TCODNoise(2,wgRng);
	hm=new TCODHeightMap(HM_WIDTH,HM_HEIGHT);
	hm2=new TCODHeightMap(HM_WIDTH,HM_HEIGHT);
	worldmap = new TCODImage(HM_WIDTH,HM_HEIGHT);
	worldint = new float[HM_WIDTH*HM_HEIGHT];
	temperature =  new TCODHeightMap(HM_WIDTH,HM_HEIGHT);
	precipitation = new TCODHeightMap(HM_WIDTH,HM_HEIGHT);
	biomeMap = new EBiome[HM_WIDTH*HM_HEIGHT];
	mapData = new map_data_t[HM_WIDTH*HM_HEIGHT];
	memset(mapData,0,sizeof(map_data_t)*HM_WIDTH*HM_HEIGHT);
	float t1=TCODSystem::getElapsedSeconds();
	DBG(("Initialization... %g\n", t1-t0 ));
	t0=t1;

	buildBaseMap();
	t1=TCODSystem::getElapsedSeconds();
	DBG(("Heightmap construction... %g\n", t1-t0 ));
	t0=t1;

	computePrecipitations();
	t1=TCODSystem::getElapsedSeconds();
	DBG(("Precipitation map... %g\n", t1-t0 ));
	t0=t1;

	erodeMap();
	t1=TCODSystem::getElapsedSeconds();
	DBG(("Erosion... %g\n", t1-t0 ));
	t0=t1;

	smoothMap();
	t1=TCODSystem::getElapsedSeconds();
	DBG(("Smooth... %g\n", t1-t0 ));
	t0=t1;

    setLandMass(0.6f,sandHeight);

	for (int i=0; i < HM_WIDTH*HM_HEIGHT/3000; i++) {
//	for (int i=0; i < 1; i++) {
		generateRivers();
	}
	t1=TCODSystem::getElapsedSeconds();
	DBG(("Rivers... %g\n", t1-t0 ));
	t0=t1;

    smoothPrecipitations();
	t1=TCODSystem::getElapsedSeconds();
	DBG(("Smooth precipitations... %g\n", t1-t0 ));
	t0=t1;

	computeTemperaturesAndBiomes();
	t1=TCODSystem::getElapsedSeconds();
	DBG(("Temperature map... %g\n", t1-t0 ));
	t0=t1;

	computeColors();
	t1=TCODSystem::getElapsedSeconds();
	DBG(("Color map... %g\n", t1-t0 ));
	t0=t1;

	t1=TCODSystem::getElapsedSeconds();
	DBG(("TOTAL TIME... %g\n", t1-t00 ));
}

void WorldGenerator::drawCoasts(TCODImage *img) {
    // detect coasts
    for (int x=0; x < HM_WIDTH-1; x++) {
        for (int y=0; y < HM_HEIGHT-1; y++) {
            float h = hm->getValue(x,y);
            float h2 = hm->getValue(x+1,y);
            if ( ( h < sandHeight && h2 >= sandHeight )
                || ( h2 < sandHeight && h >= sandHeight ) ) img->putPixel(x,y,TCODColor::black);
            else {
                h = hm->getValue(x,y);
                h2 = hm->getValue(x,y+1);
                if ( ( h < sandHeight && h2 >= sandHeight )
                    || ( h2 < sandHeight && h >= sandHeight ) ) img->putPixel(x,y,TCODColor::black);
            }
        }
    }
}

void WorldGenerator::saveBiomeMap(const char *filename) {
    static TCODImage *legend=NULL;
    static int legendHeight,legendWidth;
    static const TCODColor biomeColors[] = {
    // TUNDRA,
    TCODColor(88,234,250),
    // COLD_DESERT,
    TCODColor(129,174,170),
    // GRASSLAND,
    TCODColor::sea,
    // BOREAL_FOREST,
    TCODColor(14,93,43),
    // TEMPERATE_FOREST,
    TCODColor(44,177,83),
    // TROPICAL_MONTANE_FOREST,
    TCODColor(185,232,164),
    // HOT_DESERT,
    TCODColor(229,247,184),
    // SAVANNA,
    TCODColor::orange,
    // TROPICAL_DRY_FOREST,
    TCODColor::darkYellow,
    // TROPICAL_EVERGREEN_FOREST,
    TCODColor::green,
    // THORN_FOREST,
    TCODColor(192,192,112),
    };
    if ( legend == NULL ) {
        legend = new TCODImage("data/img/legend_biome.png");
        legend->getSize(&legendWidth,&legendHeight);
    }
    if ( filename == NULL ) filename="world_biome.png";
    TCODImage img(MAX(HM_WIDTH,legendWidth),HM_HEIGHT+legendHeight);
    // draw biome map
	for (int x=0; x < HM_WIDTH; x++) {
		for (int y=0; y < HM_HEIGHT; y++) {
		    float h=hm->getValue(x,y);
		    if ( h < sandHeight ) img.putPixel(x,y,TCODColor(100,100,255));
		    else img.putPixel(x,y,biomeColors[biomeMap[x+y*HM_WIDTH]]);
		}
	}
	drawCoasts(&img);
	// blit legend
	int legendx = MAX(HM_WIDTH,legendWidth) / 2 - legendWidth/2;
	for (int x=0; x < legendWidth; x++) {
		for (int y=0; y < legendHeight; y++) {
		    img.putPixel(legendx+x,HM_HEIGHT+y,legend->getPixel(x,y));
		}
	}
	// fill legend colors
	for (int i=0; i < 6; i++ ) {
        for (int x=17; x < 47; x++)
            for (int y=4+i*14; y < 14+i*14; y++ )
                img.putPixel(legendx+x,HM_HEIGHT+y,biomeColors[i]);
	}
	for (int i=6; i < NB_BIOMES; i++ ) {
        for (int x=221; x < 251; x++)
            for (int y=4+(i-6)*14; y < 14+(i-6)*14; y++ )
                img.putPixel(legendx+x,HM_HEIGHT+y,biomeColors[i]);
	}
	img.save(filename);
}

void WorldGenerator::saveTemperatureMap(const char *filename) {
    static TCODColor tempGradient[256];

    static TCODImage *legend=NULL;
    static int legendHeight,legendWidth;
    if ( legend == NULL ) {
        legend = new TCODImage("data/img/legend_temperature.png");
        legend->getSize(&legendWidth,&legendHeight);
        TCODColor::genMap(tempGradient,MAX_TEMP_KEY,tempKeyColor,tempIndexes);
    }

    if ( filename == NULL ) filename="world_temperature.png";
    TCODImage img(MAX(HM_WIDTH,legendWidth),HM_HEIGHT+legendHeight);
    float minTemp,maxTemp;
    temperature->getMinMax(&minTemp,&maxTemp);
    // render temperature map
    for (int x=0; x < HM_WIDTH; x++) {
        for (int y=0; y < HM_HEIGHT; y++) {
		    float h=hm->getValue(x,y);
		    if ( h < sandHeight ) img.putPixel(x,y,TCODColor(100,100,255));
		    else {
	            float temp=temperature->getValue(x,y);
	            temp = (temp - minTemp) / (maxTemp-minTemp);
	            int colorIdx = (int)(temp*255);
	            colorIdx=CLAMP(0,255,colorIdx);
	            img.putPixel(x,y,tempGradient[colorIdx]);
	        }
        }
    }
    drawCoasts(&img);

	// blit legend
	int legendx = MAX(HM_WIDTH,legendWidth) / 2 - legendWidth/2;
	for (int x=0; x < legendWidth; x++) {
		for (int y=0; y < legendHeight; y++) {
		    img.putPixel(legendx+x,HM_HEIGHT+y,legend->getPixel(x,y));
		}
	}
    img.save(filename);
}

void WorldGenerator::savePrecipitationMap(const char *filename) {
    static TCODImage *legend=NULL;
    static int legendHeight,legendWidth;
    if ( legend == NULL ) {
        legend = new TCODImage("data/img/legend_precipitation.png");
        legend->getSize(&legendWidth,&legendHeight);
    }

    if ( filename == NULL ) filename="world_precipitation.png";
    TCODImage img(MAX(HM_WIDTH,legendWidth),HM_HEIGHT+legendHeight);
    // render precipitation map
    for (int x=0; x < HM_WIDTH; x++) {
        for (int y=0; y < HM_HEIGHT; y++) {
		    float h=hm->getValue(x,y);
		    if ( h < sandHeight ) img.putPixel(x,y,TCODColor(100,100,255));
		    else {
	            float prec=precipitation->getValue(x,y);
	            int iprec = (int)(prec * 180);
	            int colorIdx=0;
	            while (colorIdx < MAX_PREC_KEY && iprec > precIndexes[colorIdx]) colorIdx++;
	            colorIdx = CLAMP(0,MAX_PREC_KEY,colorIdx);
	            img.putPixel(x,y,precColors[colorIdx]);
	        }
        }
    }
    drawCoasts(&img);

	// blit legend
	int legendx = MAX(HM_WIDTH,legendWidth) / 2 - legendWidth/2;
	for (int x=0; x < legendWidth; x++) {
		for (int y=0; y < legendHeight; y++) {
		    img.putPixel(legendx+x,HM_HEIGHT+y,legend->getPixel(x,y));
		}
	}
    img.save(filename);
}

void WorldGenerator::saveAltitudeMap(const char *filename) {
    static TCODColor altGradient[256];

    static TCODImage *legend=NULL;
    static int legendHeight,legendWidth;
    if ( legend == NULL ) {
        legend = new TCODImage("data/img/legend_altitude.png");
        legend->getSize(&legendWidth,&legendHeight);
        TCODColor::genMap(altGradient,MAX_ALT_KEY,altColors,altIndexes);
    }

    if ( filename == NULL ) filename="world_altitude.png";
    TCODImage img(HM_WIDTH+legendWidth,MAX(HM_HEIGHT,legendHeight));
    // render altitude map
    for (int x=0; x < HM_WIDTH; x++) {
        for (int y=0; y < HM_HEIGHT; y++) {
            float h=hm->getValue(x,y);
            int ialt = (int)(h * 256);
            ialt = CLAMP(0,255,ialt);
            img.putPixel(x,y,altGradient[ialt]);
        }
    }

	// blit legend
	int legendy = MAX(HM_HEIGHT,legendHeight) / 2 - legendHeight/2;
	for (int x=0; x < legendWidth; x++) {
		for (int y=0; y < legendHeight; y++) {
		    img.putPixel(HM_WIDTH+x,legendy+y,legend->getPixel(x,y));
		}
	}
    img.save(filename);
}
