/*
* libtcod 1.5.1
* Copyright (c) 2008,2009,2010 Jice & Mingos
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

#ifndef _TCOD_BSP_HPP
#define _TCOD_BSP_HPP

class TCODBsp;

class TCODLIB_API ITCODBspCallback {
public :
	virtual ~ITCODBspCallback() {}
	virtual bool visitNode(TCODBsp *node, void *userData) = 0;
};

class TCODLIB_API TCODBsp : public TCODTree {
public :
	int x,y,w,h; // 
	int position; // position of splitting
	bool horizontal; // horizontal splitting ?
	uint8 level; // level in the tree 
	
	TCODBsp() : level(0) {}
	TCODBsp(int x,int y,int w, int h) : x(x),y(y),w(w),h(h),level(0) {}
	virtual ~TCODBsp();
	TCODBsp *getLeft() const {
		return (TCODBsp *)sons;
	}
	TCODBsp *getRight() const {
		return sons ? (TCODBsp *)(sons->next) : NULL;
	}
	TCODBsp *getFather() const {
		return (TCODBsp *)father;
	}
	bool isLeaf() const { return sons == NULL ; }
	bool traversePreOrder(ITCODBspCallback *listener, void *userData);
	bool traverseInOrder(ITCODBspCallback *listener, void *userData);
	bool traversePostOrder(ITCODBspCallback *listener, void *userData);
	bool traverseLevelOrder(ITCODBspCallback *listener, void *userData);
	bool traverseInvertedLevelOrder(ITCODBspCallback *listener, void *userData);
	bool contains(int x, int y) const;
	TCODBsp *findNode(int x, int y);
	void resize(int x,int y, int w, int h);
	void splitOnce(bool horizontal, int position);
	void splitRecursive(TCODRandom *randomizer, int nb, int minHSize, int minVSize, float maxHRatio, float maxVRatio);
	void removeSons();
protected :
	TCODBsp(TCODBsp *father, bool left);

};

#endif
