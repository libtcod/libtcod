/* BSD 3-Clause License
 *
 * Copyright © 2008-2026, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/// @file list.hpp
/// Deprecated TCODList class.
#pragma once
#ifndef TCOD_LIST_HPP_
#define TCOD_LIST_HPP_

#include <stdlib.h>  // NULL
#include <string.h>  // memcpy

#include <utility>

#include "list.h"
/// @addtogroup List
/// @{
/***************************************************************************
    @brief Legacy TCOD List template.

    @tparam T Type of elements stored in this list.

    @deprecated
        All instances of TCODList should be replaced with std::vector where possible.
 */
template <class T>
class TCODList {
 public:
  /***************************************************************************
      @brief Construct a new TCODList object

      @code{.cpp}
      TCODList<int> intList;
      TCODList<float> *floatList = new TCODList<float>();
      @endcode
   */
  [[deprecated("TCODList is unsuitable as a C++ container.")]] TCODList() = default;
  /// @brief Copy from a TCOD_List
  TCODList(TCOD_List* lst) {
    for (void** it = TCOD_list_begin(lst); it != TCOD_list_end(lst); ++it) {
      push(*static_cast<T*>(static_cast<void*>(it)));
    }
  }
  /// @brief Copy from a TCODList
  TCODList(const TCODList<T>& rhs) { *this = rhs; }
  /// @brief Copy from a TCODList
  TCODList<T>& operator=(const TCODList<T>& rhs) {
    while (allocSize < rhs.allocSize) allocate();
    fillSize = rhs.fillSize;
    int i = 0;
    for (T* t = rhs.begin(); t != rhs.end(); ++t) {
      array[i++] = *t;
    }
    return *this;
  }
  /// @brief Move construct a TCODList
  TCODList(TCODList&& rhs) noexcept { swap(*this, rhs); };
  /// @brief Move construct a TCODList
  TCODList& operator=(TCODList&& rhs) noexcept {
    swap(*this, rhs);
    return *this;
  }
  /// @brief Destroy the TCODList object
  virtual ~TCODList() {
    if (array) delete[] array;
  }
  /// @brief Swap this with another list
  friend void swap(TCODList& lhs, TCODList& rhs) noexcept {
    using std::swap;
    swap(lhs.array, rhs.array);
    swap(lhs.fillSize, rhs.fillSize);
    swap(lhs.allocSize, rhs.allocSize);
  }
  /***************************************************************************
      @brief Construct a new TCODList object with preallocated memory.

      @param nbElements Number of elements to reserve.

      @code{.cpp}
      TCODList<int> intList(5); // create an empty list, pre-allocate memory for 5 elements
      @endcode
   */
  [[deprecated("TCODList is unsuitable as a C++ container.")]] TCODList(int nbElements) {
    fillSize = 0;
    allocSize = nbElements;
    array = new T[nbElements];
  }
  /***************************************************************************
      @brief Assign `element` to `index`. If needed, the array will allocate new elements up to `index`.

      @param element Element to put in the array.
      @param index Index of the element. 0 <= index

      @code{.cpp}
      TCODList<int> intList; // the array is empty (contains 0 elements)
      intList.set(5,0); // the array contains 1 element at position 0, value = 5
      intList.set(7,2); // the array contains 3 elements : 5, 0, 7
      @endcode
      @code{.c}
      TCOD_List* intList = TCOD_list_new();
      TCOD_list_set(intList,(const void *)5,0);
      TCOD_list_set(intList,(const void *)7,2);
      @endcode
   */
  void set(const T element, int index) {
    if (index < 0) return;
    while (allocSize < index + 1) allocate();
    array[index] = element;
    if (index + 1 > fillSize) fillSize = index + 1;
  }
  /***************************************************************************
      @brief Get an element at `index` from the list.

      @param index	Index of the element. Must be 0 <= idx < size of the array, not bounds checked.

      @code{.cpp}
      TCODList<int> intList;
      intList.set(5,0);
      int val = intList.get(0); // val == 5
      @endcode

      @code{.c}
      TCOD_List* intList = TCOD_list_new();
      TCOD_list_set(intList,(const void *)5,0);
      int val = (int)TCOD_list_get(intList,0); // val == 5
      @endcode
  */
  T get(int index) const { return array[index]; }
  /***************************************************************************
      @brief Return true if the list is empty.

      @code{.cpp}
      TCODList<int> intList;
      bool empty=intList.isEmpty(); // empty == true
      intList.set(3,0);
      empty=intList.isEmpty(); // empty == false
      @endcode
      @code{.c}
      TCOD_List* intList = TCOD_list_new();
      bool empty=TCOD_list_is_empty(intList); // empty == true
      TCOD_list_set(intList,(const void *)5,0);
      empty=TCOD_list_is_empty(intList); // empty == false
      @endcode
   */
  bool isEmpty() const { return (fillSize == 0); }
  /***************************************************************************
      @brief Return the count of items in this list.

      @code{.cpp}
      TCODList<int> intList;
      int size=intList.size(); // size == 0
      intList.set(3,0);
      size=intList.size(); // size == 1
      @endcode
      @code{.c}
      TCOD_List* intList = TCOD_list_new();
      int size=TCOD_list_size(intList); // size == 0
      TCOD_list_set(intList,(const void *)5,0);
      size=TCOD_list_size(intList); // size == 1
      @endcode
   */
  int size() const { return fillSize; }
  /***************************************************************************
      @brief Return true if the list contains `element`

      @code{.cpp}
      TCODList<int> intList;
      intList.set(3,0);
      bool has3 = intList.contains(3); // has3 == true
      bool has4 = intList.contains(4); // has4 == false
      @endcode
      @code{.c}
      TCOD_List* intList = TCOD_list_new();
      TCOD_list_set(intList,(const void *)3,0);
      bool has3 = TCOD_list_contains(intList,(const void *)3); // has3 == true
      bool has4 = TCOD_list_contains(intList,(const void *)4); // has4 == false
      @endcode
   */
  bool contains(const T element) const {
    for (T* curElt = begin(); curElt != end(); ++curElt) {
      if (*curElt == element) {
        return true;
      }
    }
    return false;
  }
  /***************************************************************************
      @brief Insert `element` in the list before `index`.

      @param element Element to insert in the list.
      @param index Index of the element after the insertion. Must be 0 <= index < list size.
      @return T* Iterator where the `element` was inserted.

      @code{.cpp}
      TCODList<int> intList; // the list is empty (contains 0 elements)
      intList.set(0,5); // the list contains 1 element at position 0, value = 5
      intList.insertBefore(2,0); // the list contains 2 elements : 2,5
      @endcode
      @code{.c}
      TCOD_List* intList = TCOD_list_new();
      TCOD_list_set(intList,0,(const void *)5);
      TCOD_list_insert_before(intList,(const void *)2,0);
      @endcode
   */
  T* insertBefore(const T element, int index) {
    if (fillSize + 1 >= allocSize) allocate();
    for (int idx = fillSize; idx > index; idx--) {
      array[idx] = array[idx - 1];
    }
    array[index] = element;
    fillSize++;
    return &array[index];
  }
  /***************************************************************************
      @brief Remove `element` from a list.

      @param element The element to remove.

      @code{.cpp}
      TCODList<int> intList; // the list is empty (contains 0 elements)
      intList.set(0,5); // the list contains 1 element at position 0, value = 5
      intList.remove(5); // the list is empty
      @endcode
      @code{.c}
      TCOD_List* intList = TCOD_list_new();
      TCOD_list_set(intList,0,(const void *)5);
      TCOD_list_remove(intList,(const void *)5);
      @endcode
   */
  void remove(const T element) {
    for (T* curElt = begin(); curElt != end(); curElt++) {
      if (*curElt == element) {
        remove(curElt);
        return;
      }
    }
  }
  /***************************************************************************
      @brief Remove `element` from a list without preserving the order of the list.
   */
  void removeFast(const T element) {
    for (T* curElt = begin(); curElt != end(); curElt++) {
      if (*curElt == element) {
        removeFast(curElt);
        return;
      }
    }
  }
  /***************************************************************************
      @brief Append all elements from another list onto this list.

      @code{.cpp}
      TCODList<int> intList;
      intList.set(1,3); // intList contains 2 elements : 0, 3
      TCODList<int> intList2; // intList2 is empty
      intList2.set(0,1); // intList2 contains 1 element : 1
      intList2.addAll(intList); // intList2 contains 3 elements : 1, 0, 3
      @endcode
      @code{.c}
      TCOD_List* intList = TCOD_list_new();
      TCOD_list_set(intList,1,(const void *)3);
      TCOD_List* intList2 = TCOD_list_new();
      TCOD_list_set(intList2,0,(const void *)1);
      TCOD_list_add_all(intList2,intList);
      @endcode
   */
  void addAll(const TCODList<T>& lst) {
    for (T* t = lst.begin(); t != lst.end(); t++) {
      push(*t);
    }
  }
  /***************************************************************************
      @brief Remove all elements in this list.

      @code{.cpp}
      TCODList<int> intList;
      intList.set(0, 3); // intList contains 1 element
      intList.clear(); // intList is empty
      @endcode
      @code{.c}
      TCOD_List* intList = TCOD_list_new();
      TCOD_list_set(intList, 0,(const void*)5);
      TCOD_list_clear(intList);
      @endcode

   */
  void clear() { fillSize = 0; }
  /***************************************************************************
      @brief Empty a list, invoking C++'s delete in all of its elements.

      @code{.cpp}
      TCODList<MyClass*> intList;
      MyClass* cl = new MyClass(); // new instance of MyClass allocated here
      intList.set(0, cl);
      intList.clear(); // the list is empty. cl is always valid
      intList.set(0, cl);
      intList.clearAndDelete(); // the list is empty. delete cl has been called. The address cl is no longer valid.
      @endcode
      @code{.c}
      TCOD_List* intList = TCOD_list_new();
      void *data=calloc(1,10); // some memory allocation here
      TCOD_list_set(intList, 0, (const void*)data);
      TCOD_list_clear(intList); // the list is empty, but data is always valid
      TCOD_list_set(intList, 0, (const void*)data);
      TCOD_list_clear_and_delete(intList); // the list is empty, free(data) was called. The address is no longer valid.
      @endcode

   */
  void clearAndDelete() {
    for (T* curElt = begin(); curElt != end(); curElt++) {
      delete (*curElt);
    }
    fillSize = 0;
  }
  /***************************************************************************
      @brief Reverse the order of elements in this list.

      @code{.cpp}
      TCODList<int> intList; // the list is empty (contains 0 elements)
      intList.push(5); // the list contains 1 element at position 0, value = 5
      intList.push(2); // the list contains 2 elements : 5,2
      intList.reverse(); // now order is 2,5
      @endcode
      @code{.c}
      TCOD_List* intList = TCOD_list_new();
      TCOD_list_push(intList,(const void *)5);
      TCOD_list_push(intList,(const void *)2);
      TCOD_list_reverse();
      @endcode
   */
  void reverse() {
    T* head = begin();
    T* tail = end() - 1;
    while (head < tail) {
      std::swap(*head, *tail);
      ++head;
      --tail;
    }
  }
  /***************************************************************************
      @brief Append `element` to the end of the list.

      @code{.cpp}
      TCODList<int> intList; // the list is empty (contains 0 elements)
      intList.push(5); // the list contains 1 element at position 0, value = 5
      intList.push(2); // the list contains 2 elements : 5,2
      @endcode
      @code{.c}
      TCOD_List* intList = TCOD_list_new();
      TCOD_list_push(intList,(const void *)5);
      TCOD_list_push(intList,(const void *)2);
      @endcode
   */
  void push(const T element) {
    if (fillSize + 1 >= allocSize) allocate();
    array[fillSize++] = element;
  }
  /***************************************************************************
      @brief Remove and return the last element from the list.
      If the list is empty than a default value is returned.

      @code{.cpp}
      TCODList<int> intList; // the list is empty (contains 0 elements)
      intList.push(5); // the list contains 1 element at position 0, value = 5
      intList.push(2); // the list contains 2 elements : 5,2
      int val = intList.pop(); // val == 2, the list contains 1 element : 5
      val = intList.pop(); // val == 5, the list is empty
      @endcode
      @code{.c}
      TCOD_List* intList = TCOD_list_new();
      TCOD_list_push(intList,(const void *)5);
      TCOD_list_push(intList,(const void *)2);
      int val = (int)TCOD_list_pop(intList);
      val = (int)TCOD_list_pop(intList);
      @endcode
   */
  T pop() {
    if (fillSize == 0) return T{};
    return array[--fillSize];
  }
  /***************************************************************************
      @brief Return the last element of the list.
      If the list is empty then returns a default value.

      @code{.cpp}
      TCODList<int> intList;
      intList.push(3); // intList contains 1 elements : 3
      int val = intList.peek(); // val == 3, inList contains 1 elements : 3
      intList.push(2); // intList contains 2 elements : 3, 2
      val = intList.peek(); // val == 2, inList contains 2 elements : 3, 2
      @endcode
      @code{.c}
      TCOD_List* intList = TCOD_list_new();
      TCOD_list_push(intList, (const void*)3);
      int val = (int)TCOD_list_peek(intList);
      TCOD_list_push(intList, (const void*)2);
      val = (int)TCOD_list_peek(intList);
      @endcode
   */
  T peek() const {
    if (fillSize == 0) return T{};
    return array[fillSize - 1];
  }
  /***************************************************************************
      @brief Return an iterator for this list.

      @details
      You can iterate through the elements of the list using an iterator.
      begin() returns the address of the first element of the list.
      You go to the next element using the increment operator ++.
      When the iterators value is equal to end(), you've gone through all the elements.

      @warning
      You cannot insert elements in the list while iterating through it.
      Inserting elements can result in reallocation of the list and your iterator will not longer be valid.

      @code{.cpp}
      TCODList<int> intList; // the list is empty (contains 0 elements)
      intList.push(5); // the list contains 1 element at position 0, value = 5
      intList.push(2); // the list contains 2 elements : 5,2
      for (int* iterator = intList.begin(); iterator != intList.end(); ++iterator) {
        int currentValue = *iterator;
        printf("value : %d\n", currentValue );
      }
      @endcode
      @code{.c}
      TCOD_List* intList = TCOD_list_new();
      TCOD_list_push(intList, (const void*)5);
      TCOD_list_push(intList, (const void*)2);
      for (int* iterator = (int*)TCOD_list_begin(intList); iterator != (int*)TCOD_list_end(intList); ++iterator) {
        int currentValue = *iterator;
        printf("value : %d\n", currentValue);
      }
      @endcode
   */
  T* begin() const {
    if (fillSize == 0) return (T*)NULL;
    return &array[0];
  }
  /***************************************************************************
      @brief Return the after-the-end iterator for this list.
   */
  T* end() const {
    if (fillSize == 0) return (T*)NULL;
    return &array[fillSize];
  }
  /***************************************************************************
      @brief Remove an element during iteration.

      @param iter_element Element to remove.
      @return T* Iterator adjusted for the removed element.

      @code{.cpp}
      TCODList<int> intList; // the list is empty (contains 0 elements)
      intList.push(5); // the list contains 1 element at position 0, value = 5
      intList.push(2); // the list contains 2 elements : 5,2
      intList.push(3); // the list contains 3 elements : 5,2,3
      for (int* iterator = intList.begin(); iterator != intList.end(); ++iterator) {
        int currentValue=*iterator;
        if (currentValue == 2) {
          // remove this value from the list and keep iterating on next element (value == 3)
          iterator = intList.remove(iterator);
        }
        printf("value : %d\n", currentValue); // all 3 values will be printed : 5,2,3
      }
      // now the list contains only two elements : 5,3
      @endcode
      @code{.c}
      TCOD_List* intList = TCOD_list_new();
      TCOD_list_push(intList, (const void*)5);
      TCOD_list_push(intList, (const void*)2);
      TCOD_list_push(intList, (const void*)3);
      for (int* iterator = (int*)TCOD_list_begin(intList); iterator != (int*)TCOD_list_end(intList); ++iterator) {
        int currentValue = *iterator;
        if (currentValue == 2) {
          iterator = (int*)TCOD_list_remove_iterator(intList, (void**)iterator);
        }
        printf("value : %d\n", currentValue);
      }
      @endcode
   */
  T* remove(T* iter_element) {
    for (T* curElt = iter_element; curElt < end() - 1; curElt++) {
      *curElt = *(curElt + 1);
    }
    fillSize--;
    if (fillSize == 0)
      return ((T*)NULL) - 1;
    else
      return iter_element - 1;
  }
  /***************************************************************************
      @brief Remove an element during iteration without preserving the order of elements in the list.
   */
  T* removeFast(T* iter_element) {
    *iter_element = array[fillSize - 1];
    fillSize--;
    if (fillSize == 0)
      return ((T*)NULL) - 1;
    else
      return iter_element - 1;
  }

 protected:
  /// @brief Allocate more memory for this list.
  /// @private
  void allocate() {
    int newSize = allocSize * 2;
    if (newSize == 0) newSize = 16;
    T* newArray = new T[newSize];
    if (array) {
      if (fillSize > 0) memcpy(newArray, array, sizeof(T) * fillSize);
      delete[] array;
    }
    array = newArray;
    allocSize = newSize;
  }

 private:
  T* array{};
  int fillSize{};
  int allocSize{};
};
/// @}
#endif  // TCOD_LIST_HPP_
