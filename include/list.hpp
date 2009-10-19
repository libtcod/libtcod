/*
* libtcod 1.4.3
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

#ifndef _TCOD_LIST_HPP
#define _TCOD_LIST_HPP

#include <string.h> // memcpy
#include <stdlib.h> // NULL

// fast & lightweight list template
template <class T> class TCODList {
	T *array;
	int fillSize;
	int allocSize;

public :
	TCODList() {
		array=NULL;
		fillSize=allocSize=0;
	}
	TCODList(int nbElements) {
		fillSize=0;
		allocSize=nbElements;
		array=new T[ nbElements ];
	}
	TCODList(const TCOD_list_t l) {
		array=NULL;
		fillSize=allocSize=0;
		for ( void **it=TCOD_list_begin(l); it != TCOD_list_end(l); it++ ) {
			push((T)(*it));
		}
	}
	TCODList<T> & operator = (TCODList<T> const & l2) {
		while ( allocSize < l2.allocSize ) allocate();
		fillSize=l2.fillSize;
		int i=0;
		for (T *t=l2.begin(); t != l2.end(); t++) {
			array[i++]=*t;
		}
		return *this;
	}  
	TCODList(const TCODList<T> &l2) {
		array=NULL;
		fillSize=allocSize=0;
		*this = l2;
	}
	virtual ~TCODList() {
		if ( array ) delete [] array;
	}

	void push(const T elt) {
		if ( fillSize+1 >= allocSize ) allocate();
		array[fillSize++] = elt;
	}
	T pop() {
		if ( fillSize == 0 ) return (T)0;
		return array[--fillSize];
	}
	T peek() const {
		if ( fillSize == 0 ) return (T)0;
		return array[fillSize-1];
	}
	T get(int idx) const {
		return array[idx];
	}
	void set(const T elt, int idx) {
		if ( idx < 0 ) return;
		while ( allocSize < idx+1 ) allocate();
		array[idx] = elt;
		if ( idx+1 > fillSize ) fillSize = idx+1;
	}
	void addAll(const TCODList<T> &l2) {
		for (T *t=l2.begin(); t!= l2.end(); t++) {
			push(*t);
		}
	}
	T * begin() const {
		if ( fillSize == 0 ) return (T *)NULL;
		return &array[0];
	}
	T * end() const {
		if ( fillSize == 0 ) return (T *)NULL;
		return &array[fillSize];
	}
	T *remove(T *elt) {
		for ( T* curElt = elt; curElt < end()-1; curElt ++) {
			*curElt = *(curElt+1);
		}
		fillSize--;
		if ( fillSize == 0 ) return ((T *)NULL)-1;
		else return elt-1;
	}
	T *removeFast(T *elt) {
		*elt = array[fillSize-1];
		fillSize--;
		if ( fillSize == 0 ) return ((T *)NULL)-1;
		else return elt-1;
	}
	void remove(const T elt) {
		for ( T* curElt = begin(); curElt != end(); curElt ++) {
			if ( *curElt == elt ) {
				remove(curElt);
				return;
			}
		}
	}
	void removeFast(const T elt) {
		for ( T* curElt = begin(); curElt != end(); curElt ++) {
			if ( *curElt == elt ) {
				removeFast(curElt);
				return;
			}
		}
	}
	bool contains(const T elt) const {
		for ( T* curElt = begin(); curElt != end(); curElt ++) {
			if ( *curElt == elt ) return true;
		}
		return false;
	}
	void clear() {
		fillSize=0;
	}
	void clearAndDelete() {
		for ( T* curElt = begin(); curElt != end(); curElt ++ ) {
			delete (*curElt);
		}
		fillSize=0;
	}
	int size() const {
		return fillSize;
	}
	T * insertBefore(const T elt,int before) {
		if ( fillSize+1 >= allocSize ) allocate();
		for (int idx=fillSize; idx > before; idx--) {
			array[idx]=array[idx-1];
		}
		array[before]=elt;
		fillSize++;
		return &array[before];
	}
	bool isEmpty() const {
		return ( fillSize == 0 );
	}
protected :
	void allocate() {
		int newSize = allocSize * 2;
		if ( newSize == 0 ) newSize = 16;
		T *newArray = new T[ newSize ];
		if ( array ) {
			if ( fillSize > 0 ) memcpy(newArray, array, sizeof(T)*fillSize);
			delete [] array;
		}
		array=newArray;
		allocSize=newSize;
	}
};

#endif
