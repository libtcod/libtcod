/*
* libtcod 1.5.0
* Copyright (c) 2008,2009 Jice
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

#ifndef _TCOD_PATH_HPP
#define _TCOD_PATH_HPP

class TCODLIB_API ITCODPathCallback {
public :
	virtual ~ITCODPathCallback() {}
	virtual float getWalkCost( int xFrom, int yFrom, int xTo, int yTo, void *userData ) const = 0;
};

class TCODLIB_API TCODPath {
public :
	TCODPath(const TCODMap *map, float diagonalCost=1.41f);
	TCODPath(int width, int height, const ITCODPathCallback *listener, void *userData, float diagonalCost=1.41f);
	virtual ~TCODPath();

	bool compute(int ox, int oy, int dx, int dy);
	bool walk(int *x, int *y, bool recalculateWhenNeeded);
	bool isEmpty() const;
	int size() const;
	void get(int index, int *x, int *y) const;
	void getOrigin(int *x,int *y) const;
	void getDestination(int *x,int *y) const;

protected :
	friend float TCOD_path_func(int xFrom, int yFrom, int xTo,int yTo, void *data);
	TCOD_path_t data;
	struct WrapperData {
		void *userData;
		const ITCODPathCallback *listener;
	} cppData;
};

//Dijkstra kit
class TCODLIB_API TCODDijkstra {
    public:
        TCODDijkstra (TCODMap *map, float diagonalCost=1.41f);
        TCODDijkstra (int width, int height, const ITCODPathCallback *listener, void *userData, float diagonalCost=1.41f);
        ~TCODDijkstra (void);
        void compute (int rootX, int rootY);
        float getDistance (int x, int y);
        bool setPath (int toX, int toY);
        bool walk (int *x, int *y);
		bool isEmpty() const;
		int size() const;
		void get(int index, int *x, int *y) const;
    private:
        TCOD_dijkstra_t data;
        struct WrapperData {
            void *userData;
            const ITCODPathCallback *listener;
        } cppData;
};

#endif

