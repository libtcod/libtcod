/*
* Umbra
* Copyright (c) 2009, 2010 Mingos, Jice
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The names of Mingos or Jice may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY MINGOS & JICE ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL MINGOS OR JICE BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "libtcod.hpp"
#include "bsp_helper.hpp"

BspHelper::BspHelper() {
	bspDepth=8;
	minRoomSize=4;
	randomRoom=true; 
	roomWalls=false; 			
}

// draw a vertical line
void BspHelper::vline(TCODMap *map,int x, int y1, int y2) {
	int y=y1;
	int dy=(y1>y2?-1:1);
	map->setProperties(x,y,true,true);
	if ( y1 == y2 ) return;
	do {
		y+=dy;
		map->setProperties(x,y,true,true);
	} while (y!=y2);
}


// draw a vertical line up until we reach an empty space
void BspHelper::vline_up(TCODMap *map,int x, int y) {
	while (y >= 0 && !map->isTransparent(x,y)) {
		map->setProperties(x,y,true,true);
		y--;
	}
}

// draw a vertical line down until we reach an empty space
void BspHelper::vline_down(TCODMap *map,int x, int y) {
	while (y < map->getHeight() && !map->isTransparent(x,y)) {
		map->setProperties(x,y,true,true);
		y++;
	}
}

// draw a horizontal line
void BspHelper::hline(TCODMap *map,int x1, int y, int x2) {
	int x=x1;
	int dx=(x1>x2?-1:1);
	map->setProperties(x,y,true,true);
	if ( x1 == x2 ) return;
	do {
		x+=dx;
		map->setProperties(x,y,true,true);
	} while (x!=x2);
}

// draw a horizontal line left until we reach an empty space
void BspHelper::hline_left(TCODMap *map,int x, int y) {
	while (x >= 0 && !map->isTransparent(x,y)) {
		map->setProperties(x,y,true,true);
		x--;
	}
}

// draw a horizontal line right until we reach an empty space
void BspHelper::hline_right(TCODMap *map,int x, int y) {
	while (x < map->getWidth() && !map->isTransparent(x,y)) {
		map->setProperties(x,y,true,true);
		x++;
	}
}

bool BspHelper::visitNode(TCODBsp *node, void *userData) {
	TCODMap *map=(TCODMap *)userData;
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
		if (maxx == map->getWidth()-1 ) maxx--;
		if (maxy == map->getHeight()-1 ) maxy--;
		if ( randomRoom ) {
			minx = TCODRandom::getInstance()->getInt(minx,maxx-minRoomSize+1);
			miny = TCODRandom::getInstance()->getInt(miny,maxy-minRoomSize+1);
			maxx = TCODRandom::getInstance()->getInt(minx+minRoomSize-1,maxx);
			maxy = TCODRandom::getInstance()->getInt(miny+minRoomSize-1,maxy);
		}
		// keep walls on the map borders
		minx=MAX(1,minx);
		miny=MAX(1,miny);
		maxx=MIN(map->getWidth()-2,maxx);
		maxy=MIN(map->getHeight()-2,maxy);
		// resize the node to fit the room
//printf("node %dx%d %dx%d => room %dx%d %dx%d\n",node->x,node->y,node->w,node->h,minx,miny,maxx-minx+1,maxy-miny+1);
		node->x=minx;
		node->y=miny;
		node->w=maxx-minx+1;
		node->h=maxy-miny+1;
		// dig the room
		for (int x=minx; x <= maxx; x++ ) {
			for (int y=miny; y <= maxy; y++ ) {
				map->setProperties(x,y,true,true);
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

void BspHelper::createBspDungeon(TCODMap *map, TCODRandom *rng) {
	TCODBsp *bsp = new TCODBsp(0,0,map->getWidth(),map->getHeight());
	map->clear(false,false); // fill with walls
	// create the BSP tree
	bsp->splitRecursive(rng,bspDepth,minRoomSize+(roomWalls?1:0),minRoomSize+(roomWalls?1:0),1.5f,1.5f);
	// carve rooms and corridors
	bsp->traverseInvertedLevelOrder(this,map);	
}

