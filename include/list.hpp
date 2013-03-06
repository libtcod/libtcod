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

#ifndef _TCOD_LIST_HPP
#define _TCOD_LIST_HPP

#include <string.h> // memcpy
#include <stdlib.h> // NULL

/**
 @PageName list
 @PageCategory Base toolkits
 @PageTitle All purposes container
 @PageDesc This is a fast, lightweight and generic container, that provides array, list and stack paradigms.
Note that this module has no python wrapper. Use python built-in containers instead.
 */

// fast & lightweight list template
template <class T> class TCODList {
	T *array;
	int fillSize;
	int allocSize;

public :
	/**
	@PageName list_create
	@PageFather list
	@PageTitle Creating a list
	@FuncTitle Using the default constructor
	@FuncDesc You can create an empty list with the default constructor. The C version returns a handler on the list.
	@Cpp template <class T> TCODList::TCODList()
	@C TCOD_list_t TCOD_list_new()
	@CppEx 
		TCODList<int> intList;
		TCODList<float> *floatList = new TCODList<float>();
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		TCOD_list_t floatList = TCOD_list_new();
	*/
	TCODList() {
		array=NULL;
		fillSize=allocSize=0;
	}

	/**
	@PageName list_create
	@FuncTitle Duplicating an existing list
	@FuncDesc You can create a list by duplicating an existing list.
	@Cpp template <class T> TCODList::TCODList(const TCODList &l)
	@C TCOD_list_t TCOD_list_duplicate(TCOD_list_t l)
	@Param l	Existing list to duplicate.
	@CppEx 
		TCODList<int> intList;
		intList.push(3);
		intList.push(5);
		TCODList<int> intList2(intList); // intList2 contains two elements : 3 and 5
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		TCOD_list_push(intList,(const void *)3);
		TCOD_list_push(intList,(const void *)5);
		TCOD_list_t intList2 = TCOD_list_duplicate(intList); // intList2 contains two elements : 3 and 5
	*/
	TCODList(const TCOD_list_t l) {
		array=NULL;
		fillSize=allocSize=0;
		for ( void **it=TCOD_list_begin(l); it != TCOD_list_end(l); it++ ) {
			push(*((T *)(it)));
		}
	}
	TCODList(const TCODList<T> &l2) {
		array=NULL;
		fillSize=allocSize=0;
		*this = l2;
	}

	/**
	@PageName list_create
	@FuncTitle Preallocating memory
	@FuncDesc You can also create an empty list and pre-allocate memory for elements. Use this if you know the list size and want the memory to fit it perfectly.
	@Cpp template <class T> TCODList::TCODList(int nbElements)
	@C TCOD_list_t TCOD_list_allocate(int nbElements)
	@Param nbElements	Allocate memory for nbElements.
	@CppEx TCODList<int> intList(5); // create an empty list, pre-allocate memory for 5 elements
	@CEx TCOD_list_t intList = TCOD_list_allocate(5);
	*/
	TCODList(int nbElements) {
		fillSize=0;
		allocSize=nbElements;
		array=new T[ nbElements ];
	}

	/**
	@PageName list_create
	@FuncTitle Deleting a list
	@FuncDesc You can delete a list, freeing any allocated resources. Note that deleting the list does not delete it's elements. You have to use clearAndDelete before deleting the list if you want to destroy the elements too.
	@Cpp virtual template <class T> TCODList::~TCODList()
	@C void TCOD_list_delete(TCOD_list_t l)
	@Param l	In the C version, the list handler, returned by a constructor.
	@CppEx 
		TCODList<int> *intList = new TCODList<int>(); // allocate a new empty list
		intList->push(5); // the list contains 1 element at position 0, value = 5
		delete intList; // destroy the list
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		TCOD_list_push(intList,(const void *)5);
		TCOD_list_delete(intList);
	*/
	virtual ~TCODList() {
		if ( array ) delete [] array;
	}

	/**
	@PageName list_array
	@PageTitle Basic array operations
	@PageFather list
	@FuncTitle Setting an element
	@FuncDesc You can assign a value with set. If needed, the array will allocate new elements up to idx.
	@Cpp template <class T> void TCODList::set(const T elt, int idx)
	@C void TCOD_list_set(TCOD_list_t l,const void *elt, int idx)
	@Param elt	Element to put in the array.
	@Param idx	Index of the element.
		0 <= idx
	@Param l	In the C version, the handler, returned by a constructor.
	@CppEx 
		TCODList<int> intList; // the array is empty (contains 0 elements)
		intList.set(5,0); // the array contains 1 element at position 0, value = 5
		intList.set(7,2); // the array contains 3 elements : 5, 0, 7
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		TCOD_list_set(intList,(const void *)5,0);
		TCOD_list_set(intList,(const void *)7,2);
	*/
	void set(const T elt, int idx) {
		if ( idx < 0 ) return;
		while ( allocSize < idx+1 ) allocate();
		array[idx] = elt;
		if ( idx+1 > fillSize ) fillSize = idx+1;
	}

	/**
	@PageName list_array
	@FuncTitle Getting an element value
	@FuncDesc You can retrieve a value with get.
	@Cpp template <class T> T TCODList::get(int idx) const
	@C void * TCOD_list_get(TCOD_list_t l,int idx)
	@Param idx	Index of the element.
		0 <= idx < size of the array
	@Param l	In the C version, the handler, returned by a constructor.
	@CppEx 
		TCODList<int> intList;
		intList.set(5,0);
		int val = intList.get(0); // val == 5
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		TCOD_list_set(intList,(const void *)5,0);
		int val = (int)TCOD_list_get(intList,0); // val == 5
	*/
	T get(int idx) const {
		return array[idx];
	}

	/**
	@PageName list_array
	@FuncTitle Checking if a list is empty
	@Cpp template <class T> bool TCODList::isEmpty() const
	@C bool TCOD_list_is_empty(TCOD_list_t l)
	@Param l	In the C version, the handler, returned by a constructor.
	@CppEx 
		TCODList<int> intList;
		bool empty=intList.isEmpty(); // empty == true
		intList.set(3,0);
		empty=intList.isEmpty(); // empty == false
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		bool empty=TCOD_list_is_empty(intList); // empty == true 
		TCOD_list_set(intList,(const void *)5,0);
		empty=TCOD_list_is_empty(intList); // empty == false 
	*/
	bool isEmpty() const {
		return ( fillSize == 0 );
	}

	/**
	@PageName list_array
	@FuncTitle Getting the list size
	@Cpp template <class T> int TCODList::size() const
	@C int TCOD_list_size(TCOD_list_t l)
	@Param l	In the C version, the handler, returned by a constructor.
	@CppEx 
		TCODList<int> intList;
		int size=intList.size(); // size == 0
		intList.set(3,0);
		size=intList.size(); // size == 1
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		int size=TCOD_list_size(intList); // size == 0 
		TCOD_list_set(intList,(const void *)5,0);
		size=TCOD_list_size(intList); // size == 1 
	*/
	int size() const {
		return fillSize;
	}

	/**
	@PageName list_array
	@FuncTitle Checking if an element is in the list
	@Cpp template <class T> bool TCODList::contains(const T elt) const
	@C bool TCOD_list_contains(TCOD_list_t l,const void * elt)
	@Param elt	The element.
	@Param l	In the C version, the handler, returned by a constructor.
	@CppEx 
		TCODList<int> intList;
		intList.set(3,0);
		bool has3 = intList.contains(3); // has3 == true
		bool has4 = intList.contains(4); // has4 == false
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		TCOD_list_set(intList,(const void *)3,0);
		bool has3 = TCOD_list_contains(intList,(const void *)3); // has3 == true 
		bool has4 = TCOD_list_contains(intList,(const void *)4); // has4 == false 
	*/
	bool contains(const T elt) const {
		for ( T* curElt = begin(); curElt != end(); curElt ++) {
			if ( *curElt == elt ) return true;
		}
		return false;
	}

	/**
	@PageName list_list
	@PageFather list
	@PageTitle Basic list operations
	@FuncTitle Insert an element in the list
	@Cpp template <class T> void TCODList::insertBefore(const T elt,int before)
	@C void TCOD_list_insert_before(TCOD_list_t l,const void *elt,int before)
	@Param elt	Element to insert in the list.
	@Param idx	Index of the element after the insertion.
		0 <= idx < list size
	@Param l	In the C version, the list handler, returned by a constructor.
	@CppEx 
		TCODList<int> intList; // the list is empty (contains 0 elements)
		intList.set(0,5); // the list contains 1 element at position 0, value = 5
		intList.insertBefore(2,0); // the list contains 2 elements : 2,5
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		TCOD_list_set(intList,0,(const void *)5);
		TCOD_list_insert_before(intList,(const void *)2,0);
	*/
	T * insertBefore(const T elt,int before) {
		if ( fillSize+1 >= allocSize ) allocate();
		for (int idx=fillSize; idx > before; idx--) {
			array[idx]=array[idx-1];
		}
		array[before]=elt;
		fillSize++;
		return &array[before];
	}

	/**
	@PageName list_list
	@FuncTitle Removing an element from the list
	@FuncDesc The _fast versions replace the element to remove with the last element of the list. They're faster, but do not preserve the list order.
	@Cpp 
		template <class T> void TCODList::remove(const T elt)
		template <class T> void TCODList::removeFast(const T elt)
	@C 
		void TCOD_list_remove(TCOD_list_t l, const void * elt)
		void TCOD_list_remove_fast(TCOD_list_t l, const void * elt)
	@Param elt	The element to remove
	@Param l	In the C version, the list handler, returned by a constructor.
	@CppEx 
		TCODList<int> intList; // the list is empty (contains 0 elements)
		intList.set(0,5); // the list contains 1 element at position 0, value = 5
		intList.remove(5); // the list is empty
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		TCOD_list_set(intList,0,(const void *)5);
		TCOD_list_remove(intList,(const void *)5);
	*/
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

	/**
	@PageName list_list
	@FuncTitle Concatenating two lists
	@FuncDesc You can concatenate two lists. Every element of l2 will be added to current list (or l in the C version) :
	@Cpp template <class T> void TCODList::addAll(const TCODList &l2)
	@C void TCOD_list_add_all(TCOD_list_t l, TCOD_list_t l2)
	@Param l	The list inside which elements will be added.
	@Param l2	the list handler containing elements to insert.
	@CppEx 
		TCODList<int> intList;
		intList.set(1,3); // intList contains 2 elements : 0, 3
		TCODList<int> intList2; // intList2 is empty
		intList2.set(0,1); // intList2 contains 1 element : 1
		intList2.addAll(intList); // intList2 contains 3 elements : 1, 0, 3
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		TCOD_list_set(intList,1,(const void *)3);
		TCOD_list_t intList2 = TCOD_list_new();
		TCOD_list_set(intList2,0,(const void *)1);
		TCOD_list_add_all(intList2,intList);
	*/
	void addAll(const TCODList<T> &l2) {
		for (T *t=l2.begin(); t!= l2.end(); t++) {
			push(*t);
		}
	}

	/**
	@PageName list_list
	@FuncTitle Emptying a list
	@Cpp template <class T> void TCODList::clear()
	@C void TCOD_list_clear(TCOD_list_t l)
	@Param l	In the C version, the list handler, returned by a constructor.
	@CppEx 
		TCODList<int> intList;
		intList.set(0,3); // intList contains 1 element
		intList.clear(); // intList is empty
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		TCOD_list_set(intList,0,(const void *)5);
		TCOD_list_clear(intList);
	*/
	void clear() {
		fillSize=0;
	}
	/**
	@PageName list_list
	@FuncTitle Emptying a list and destroying its elements
	@FuncDesc For lists containing pointers, you can clear the list and delete (or free for C) the elements :
	@Cpp template <class T> void TCODList::clearAndDelete()
	@C void TCOD_list_clear_and_delete(TCOD_list_t l)
	@Param l	In the C version, the list handler, returned by a constructor.
	@CppEx 
		TCODList<MyClass *> intList;
		MyClass * cl=new MyClass(); // new instance of MyClass allocated here
		intList.set(0,cl); 
		intList.clear(); // the list is empty. cl is always valid
		intList.set(0,cl); 
		intList.clearAndDelete(); // the list is empty. delete cl has been called. The address cl is no longer valid.
	@C 
		TCOD_list_t intList = TCOD_list_new();
		void *data=calloc(10,1); // some memory allocation here 
		TCOD_list_set(intList,0,(const void *)data);
		TCOD_list_clear(intList); // the list is empty, but data is always valid 
		TCOD_list_set(intList,0,(const void *)data);
		TCOD_list_clear_and_delete(intList); // the list is empty, free(data) has been called. The address data is no longer valid 
	*/
	void clearAndDelete() {
		for ( T* curElt = begin(); curElt != end(); curElt ++ ) {
			delete (*curElt);
		}
		fillSize=0;
	}

	/**
	@PageName list_list
	@FuncTitle Reversing a list
	@FuncDesc This function reverses the order of the elements in the list.</b>
	@Cpp 
		void TCODList::reverse()
	@C 
		void TCOD_list_reverse(TCOD_list_t l)
	@Param l	In the C version, the list handler, returned by a constructor.
	@CppEx 
		TCODList<int> intList; // the list is empty (contains 0 elements)
		intList.push(5); // the list contains 1 element at position 0, value = 5
		intList.push(2); // the list contains 2 elements : 5,2
		intList.reverse(); // now order is 2,5
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		TCOD_list_push(intList,(const void *)5);
		TCOD_list_push(intList,(const void *)2);
		TCOD_list_reverse();
	*/
	void reverse() {
		T* head = begin();
		T* tail = end();
		while ( head < tail ) {
			T tmp = *head;
			*head=*tail;
			*tail=tmp;
			head++;
			tail--;
		}
	}

	/**
	@PageName list_stack
	@PageTitle Basic stack operations
	@PageFather list
	@FuncTitle Pushing an element on the stack
	@FuncDesc You can push an element on the stack (append it to the end of the list) :
	@Cpp template <class T> void TCODList::push(const T elt) 
	@C void TCOD_list_push(TCOD_list_t l, const void * elt)
	@Param elt	Element to append to the list.
	@Param l	In the C version, the list handler, returned by a constructor.
	@CppEx 
		TCODList<int> intList; // the list is empty (contains 0 elements)
		intList.push(5); // the list contains 1 element at position 0, value = 5
		intList.push(2); // the list contains 2 elements : 5,2
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		TCOD_list_push(intList,(const void *)5);
		TCOD_list_push(intList,(const void *)2);
	*/
	void push(const T elt) {
		if ( fillSize+1 >= allocSize ) allocate();
		array[fillSize++] = elt;
	}

	/**
	@PageName list_stack
	@FuncTitle Poping an element from the stack
	@FuncDesc You can pop an element from the stack (remove the last element of the list).
	@Cpp template <class T> T TCODList::pop()
	@C void * TCOD_list_pop(TCOD_list_t l)
	@Param l	In the C version, the list handler, returned by a constructor.
	@CppEx 
		TCODList<int> intList; // the list is empty (contains 0 elements)
		intList.push(5); // the list contains 1 element at position 0, value = 5
		intList.push(2); // the list contains 2 elements : 5,2
		int val = intList.pop(); // val == 2, the list contains 1 element : 5
		val = intList.pop(); // val == 5, the list is empty
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		TCOD_list_push(intList,(const void *)5);
		TCOD_list_push(intList,(const void *)2);
		int val = (int)TCOD_list_pop(intList);
		val = (int)TCOD_list_pop(intList);
	*/
	T pop() {
		if ( fillSize == 0 ) return (T)0;
		return array[--fillSize];
	}

	/**
	@PageName list_stack
	@FuncTitle Peeking the last element of the stack
	@FuncDesc You can read the last element of the stack without removing it :
	@Cpp template <class T> T TCODList::peek() const
	@C void * TCOD_list_peek(TCOD_list_t l)
	@Param l	In the C version, the list handler, returned by a constructor.
	@CppEx 
		TCODList<int> intList;
		intList.push(3); // intList contains 1 elements : 3
		int val = intList.peek(); // val == 3, inList contains 1 elements : 3
		intList.push(2); // intList contains 2 elements : 3, 2
		val = intList.peek(); // val == 2, inList contains 2 elements : 3, 2
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		TCOD_list_push(intList,(const void *)3);
		int val = (int)TCOD_list_peek(intList);
		TCOD_list_push(intList,(const void *)2);
		val = (int)TCOD_list_peek(intList);
	*/
	T peek() const {
		if ( fillSize == 0 ) return (T)0;
		return array[fillSize-1];
	}

	/**
	@PageName list_iterator
	@PageFather list
	@PageTitle Iterators
	@FuncDesc You can iterate through the elements of the list using an iterator. begin() returns the address of the first element of the list. You go to the next element using the increment operator ++. When the iterator's value is equal to end(), you've gone through all the elements. <b>Warning ! You cannot insert elements in the list while iterating through it. Inserting elements can result in reallocation of the list and your iterator will not longer be valid.</b>
	@Cpp 
		template <class T> T * TCODList::begin() const
		template <class T> T * TCODList::end() const
	@C 
		void ** TCOD_list_begin(TCOD_list_t l)
		void ** TCOD_list_end(TCOD_list_t l)
	@Param l	In the C version, the list handler, returned by a constructor.
	@CppEx 
		TCODList<int> intList; // the list is empty (contains 0 elements)
		intList.push(5); // the list contains 1 element at position 0, value = 5
		intList.push(2); // the list contains 2 elements : 5,2
		for ( int * iterator = intList.begin(); iterator != intList.end(); iterator ++ ) {
			int currentValue=*iterator;
			printf("value : %d\n", currentValue );
		}
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		TCOD_list_push(intList,(const void *)5);
		TCOD_list_push(intList,(const void *)2);
		for ( int * iterator = (int *)TCOD_list_begin(intList); iterator != (int *)TCOD_list_end(intList); iterator ++ ) {
			int currentValue=*iterator;
			printf("value : %d\n", currentValue );
		}
	*/
	T * begin() const {
		if ( fillSize == 0 ) return (T *)NULL;
		return &array[0];
	}
	T * end() const {
		if ( fillSize == 0 ) return (T *)NULL;
		return &array[fillSize];
	}

	/**
	@PageName list_iterator
	@FuncDesc You can remove an element from the list while iterating. The element at the iterator position will be removed. The function returns the new iterator. The _fast versions replace the element to remove with the last element of the list. They're faster, but do not preserve the list order.
	@Cpp 
		template <class T> T *TCODList::remove(T *iterator)
		template <class T> T *TCODList::removeFast(T *iterator)
	@C 
		void **TCOD_list_remove_iterator(TCOD_list_t l, void **iterator)
		void **TCOD_list_remove_iterator_fast(TCOD_list_t l, void **iterator)
	@Param iterator	The list iterator.
	@Param l	In the C version, the list handler, returned by a constructor.
	@CppEx 
		TCODList<int> intList; // the list is empty (contains 0 elements)
		intList.push(5); // the list contains 1 element at position 0, value = 5
		intList.push(2); // the list contains 2 elements : 5,2
		intList.push(3); // the list contains 3 elements : 5,2,3
		for ( int * iterator = intList.begin(); iterator != intList.end(); iterator ++ ) {
			int currentValue=*iterator;
			if ( currentValue == 2 ) {
				// remove this value from the list and keep iterating on next element (value == 3)
				iterator = intList.remove(iterator);
			}
			printf("value : %d\n", currentValue ); // all 3 values will be printed : 5,2,3
		}
		// now the list contains only two elements : 5,3
	@CEx 
		TCOD_list_t intList = TCOD_list_new();
		TCOD_list_push(intList,(const void *)5);
		TCOD_list_push(intList,(const void *)2);
		TCOD_list_push(intList,(const void *)3);
		for ( int * iterator = (int *)TCOD_list_begin(intList); iterator != (int *)TCOD_list_end(intList); iterator ++ ) {
			int currentValue=*iterator;
			if ( currentValue == 2 ) {
				iterator = (int *)TCOD_list_remove_iterator(intList,(void **)iterator);
			}
			printf("value : %d\n", currentValue );
		}
	*/
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

	TCODList<T> & operator = (TCODList<T> const & l2) {
		while ( allocSize < l2.allocSize ) allocate();
		fillSize=l2.fillSize;
		int i=0;
		for (T *t=l2.begin(); t != l2.end(); t++) {
			array[i++]=*t;
		}
		return *this;
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
