/*
* libtcod 1.5.2
* Copyright (c) 2008,2009,2010,2012 Jice & Mingos
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
#include "libtcod.hpp"


TCODPath::TCODPath(const TCODMap *map, float diagonalCost) {
	data=(void *)TCOD_path_new_using_map(map->data,diagonalCost);
}

TCODPath::~TCODPath() {
	TCOD_path_delete(data);
}

float TCOD_path_func(int xFrom, int yFrom, int xTo,int yTo, void *data) {
	TCODPath::WrapperData *cppData=(TCODPath::WrapperData *)data;
	return cppData->listener->getWalkCost(xFrom,yFrom,xTo,yTo,cppData->userData);
}

TCODPath::TCODPath(int width, int height, const ITCODPathCallback *listener, void *userData, float diagonalCost) {
	cppData.listener=listener;
	cppData.userData=userData;
	data=(void *)TCOD_path_new_using_function(width, height, TCOD_path_func, (void *)&cppData,diagonalCost);
}


bool TCODPath::compute(int ox, int oy, int dx, int dy) {
	return TCOD_path_compute(data,ox,oy,dx,dy) != 0;
}

bool TCODPath::walk(int *x, int *y, bool recalculateWhenNeeded) {
	return TCOD_path_walk(data,x,y,recalculateWhenNeeded) != 0;
}

bool TCODPath::isEmpty() const {
	return TCOD_path_is_empty(data) != 0;
}

void TCODPath::reverse() {
	TCOD_path_reverse(data);
}

int TCODPath::size() const {
	return TCOD_path_size(data);
}

void TCODPath::get(int index, int *x, int *y) const {
	return TCOD_path_get(data,index,x,y);
}

void TCODPath::getOrigin(int *x,int *y) const {
	TCOD_path_get_origin(data,x,y);
}

void TCODPath::getDestination(int *x,int *y) const {
	TCOD_path_get_destination(data,x,y);
}

// ----------------- //
// Dijkstra          //
// written by Mingos //
// ----------------- //

//ctor
TCODDijkstra::TCODDijkstra (TCODMap *map, float diagonalCost) {
    data = TCOD_dijkstra_new(map->data,diagonalCost);
}

//another ctor
TCODDijkstra::TCODDijkstra (int width, int height, const ITCODPathCallback *listener, void *userData, float diagonalCost) {
	cppData.listener=listener;
	cppData.userData=userData;
	data=(void *)TCOD_dijkstra_new_using_function(width, height, TCOD_path_func, (void *)&cppData,diagonalCost);
}

//dtor
TCODDijkstra::~TCODDijkstra (void) {
    TCOD_dijkstra_delete(data);
}

//compute distances grid
void TCODDijkstra::compute (int rootX, int rootY) {
    TCOD_dijkstra_compute(data,rootX,rootY);
}

//retrieve distance to a given cell
float TCODDijkstra::getDistance (int x, int y) {
    return TCOD_dijkstra_get_distance(data,x,y);
}

//create a path
bool TCODDijkstra::setPath (int toX, int toY) {
    return (TCOD_dijkstra_path_set(data,toX,toY) != 0);
}

void TCODDijkstra::reverse() {
	TCOD_dijkstra_reverse(data);
}

//walk a path
bool TCODDijkstra::walk (int *x, int *y) {
    return TCOD_dijkstra_path_walk(data,x,y) != 0;
}

bool TCODDijkstra::isEmpty() const {
	return TCOD_dijkstra_is_empty(data) != 0;
}

int TCODDijkstra::size() const {
	return TCOD_dijkstra_size(data);
}

void TCODDijkstra::get(int index, int *x, int *y) const {
	return TCOD_dijkstra_get(data,index,x,y);
}

