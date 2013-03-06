#include <math.h>
#include <stdio.h>
#include "libtcod.hpp"
#include "libtcod_int.h"


TCODColor frostCol[256];
int keys[4]={0,60,200,255};
TCODColor keyCols[4]={TCODColor::black,TCODColor::darkerBlue,TCODColor::lighterBlue,TCODColor::lightestBlue};

#define GROW 5000.0f
#define ANGLE_DELAY 0.2f 
#define FROST_LEVEL 0.8f
#define SMOOTH 0.3f
#define PIX_PER_FRAME 6
#define RANGE 10

struct Frost;

class FrostManager {
public :
	FrostManager(int w, int h);
	~FrostManager();
	void addFrost(int x, int y);
	void update(float elapsed);
	void render();
	void clear();
	inline float getValue(int cx, int cy) {
		TCOD_IFNOT(cx+cy*w >= 0 && cx+cy*w < w*h) return 0.0f;
		return grid[cx+cy*w];
	}
	inline void setValue(int cx, int cy,float v) {
		TCOD_IFNOT(cx+cy*w >= 0 && cx+cy*w < w*h) return;
		grid[cx+cy*w]=v;
	}	
protected :
	friend struct Frost;
	TCODList<Frost *> list;
	float *grid;
	TCODImage *img;
	int w,h;
};

struct Frost {
	int x,y,bestx,besty,rx,ry;
	int border;
	FrostManager *manager;
	float timer;
	float ra;
	float rr;
	Frost(int x, int y, FrostManager *manager);
	inline float getValue(int cx, int cy) {
		return manager->getValue(x-RANGE+cx,y-RANGE+cy);
	}
	inline void setValue(int cx, int cy,float v) {
		manager->setValue(x-RANGE+cx,y-RANGE+cy,v);
	}
	bool update(float elapsed);
	void render(TCODImage *img);
}; 


FrostManager::FrostManager(int w, int h) : w(w),h(h) {
	grid = new float[w*h];
	img=new TCODImage(w,h);
}
FrostManager::~FrostManager() {
	delete [] grid;
	delete img;
}
void FrostManager::addFrost(int x, int y) {
	list.push(new Frost(x,y,this));
	setValue(x,y,1.0f);
}

void FrostManager::clear() {
	img->clear(TCODColor::black);
	memset(grid,0,sizeof(float)*w*h);
}

void FrostManager::update(float elapsed) {
	TCODList<Frost *>toRemove;
	for (Frost **it=list.begin();it!=list.end();it++) {
		if (!(*it)->update(elapsed)) toRemove.push(*it);
	}
	for (Frost **it=toRemove.begin();it!=toRemove.end();it++) {
		list.removeFast(*it);
	}
	toRemove.clearAndDelete();			
}

void FrostManager::render() {
	for (Frost **it=list.begin();it!=list.end();it++) {
		(*it)->render(img);
	}
	img->blit2x(TCODConsole::root,0,0);
}

Frost::Frost(int x, int y, FrostManager *manager) :x(x),y(y),manager(manager) {
	border=0;
	timer=0.0f;
}
bool Frost::update(float elapsed) {
	for (int i=PIX_PER_FRAME;i> 0; i--) {
		timer -= elapsed;
		if ( timer <= 0 ) {
			// find a new random frost direction
			ra = TCODRandom::getInstance()->getFloat(0.0f,2*3.1415926f);
			rr = TCODRandom::getInstance()->getFloat(0,2*RANGE);
			timer=ANGLE_DELAY;
			rx=(int)(RANGE + rr*cosf(ra));
			ry=(int)(RANGE + rr*sinf(ra));
			int minDist=100000;
			// find closest frost pixel
			for (int cx=1; cx < 2*RANGE; cx++) {
				if ( (unsigned)(x-RANGE+cx) < (unsigned)manager->w ) {
					for (int cy=1; cy < 2*RANGE; cy++) {
						if ( (unsigned)(y-RANGE+cy) < (unsigned)manager->h ) {
							float f=getValue(cx,cy);
							if ( f > FROST_LEVEL ) {
								int dist=(cx-rx)*(cx-rx)+(cy-ry)*(cy-ry);
								if ( dist < minDist ) {
									minDist=dist;
									bestx=cx;
									besty=cy;
								}
							}
						}
					}
				}
			}
		}
		// smoothing
		for (int cx=0; cx < 2*RANGE+1; cx++) {
			if ( x-RANGE+cx < manager->w-1 && x-RANGE+cx > 0 ) {
				for (int cy=0; cy < 2*RANGE+1; cy++) {
					if ( y-RANGE+cy < manager->h-1 && y-RANGE+cy > 0 ) {
						if ( getValue(cx,cy)< 1.0f) {
							float f=getValue(cx,cy);
							float oldf=f;
							f=MAX(f,getValue(cx+1,cy));
							f=MAX(f,getValue(cx-1,cy));
							f=MAX(f,getValue(cx,cy+1));
							f=MAX(f,getValue(cx,cy-1));
							setValue(cx,cy,oldf+(f-oldf)*SMOOTH*elapsed);
						}
					}
				}
			}
		}
		int curx=bestx;
		int cury=besty;
		// frosting
		TCODLine::init(curx,cury,rx,ry);
		TCODLine::step(&curx,&cury);
		if ((unsigned)(x-RANGE+curx) < (unsigned)manager->w && (unsigned)(y-RANGE+cury) < (unsigned)manager->h) { 
			float f=getValue(curx,cury);
			f+=GROW*elapsed;
			f=MIN(1.0f,f);
			setValue(curx,cury,f);
			if ( f==1.0f ) {
				bestx=curx;besty=cury;
				if ( bestx==rx && besty==ry ) timer=0.0f;
				timer=0.0f;
				if ( curx == 0 || curx == 2*RANGE || cury ==0 || cury == 2*RANGE ) {
					border++;
					if ( border==20 ) return false;
				}
			}
		} else timer=0.0f;
	}
	return true;
}
void Frost::render(TCODImage *img) {
	int w,h;
	img->getSize(&w,&h);
	for (int cx=x-RANGE; cx <= x+RANGE;cx++) {
		if ( (unsigned)cx < (unsigned)w ) { 
			for (int cy=y-RANGE; cy <= y+RANGE;cy++) {
				if ( (unsigned)cy < (unsigned)h ) {
					float f=getValue(cx - (x-RANGE), cy-(y-RANGE));
					int idx=(int)(f*255);
					idx=MIN(255,idx);
					img->putPixel(cx,cy,frostCol[idx]);
				}
			}
		}
	}
}

int main() {
	TCODConsole::initRoot(80,50,"frost test",false);
	TCOD_mouse_t mouse;
	TCOD_key_t key;
	TCODSystem::setFps(25);
	TCODColor::genMap(frostCol,sizeof(keys)/sizeof(int),keyCols,keys);
	FrostManager frostManager(160,100);
	while (! TCODConsole::isWindowClosed()) {
		frostManager.render();
		TCODConsole::flush();
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_RELEASE|TCOD_EVENT_MOUSE,&key,&mouse);
		if ( key.vk == TCODK_BACKSPACE ) frostManager.clear();
		if ( mouse.lbutton ) {
			frostManager.addFrost(mouse.cx*2,mouse.cy*2);
		}
		frostManager.update(TCODSystem::getLastFrameLength());
	}
	return 0;
}