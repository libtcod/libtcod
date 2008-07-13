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
	TCODList(const TCODList<T> &l2) {
		array=NULL;
		fillSize=allocSize=0;
		while ( allocSize < l2.allocSize ) allocate();
		fillSize=l2.fillSize;
		int i=0;
		for (T *t=l2.begin(); t != l2.end(); t++) {
			array[i++]=*t;
		}
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
	void remove(const T elt) {
		for ( T* curElt = begin(); curElt != end(); curElt ++) {
			if ( *curElt == elt ) {
				remove(curElt);
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
	void insertBefore(const T elt,int before) {
		if ( fillSize+1 >= allocSize ) allocate();
		for (int idx=fillSize; idx > before; idx--) {
			array[idx]=array[idx-1];
		}
		array[before]=elt;
		fillSize++;
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
