/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
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
#include "list.h"

#include <stdlib.h> /* calloc */
#include <string.h> /* NULL/memcpy */

#include "utility.h"
/**
 *  A lightweight and generic container that provides array, list, and stack
 *  paradigms.
 */
struct TCOD_List {
  /**
   *  A pointer to an array of void pointers.
   */
  void **array;
  /** The current count of items in the array. */
  int fillSize;
  /** The maximum number of items that `array` can currently hold. */
  int allocSize;
};
/**
 *  Initialize or expand the array of a TCOD_list_t struct.
 *
 *  If `l->allocSize` is zero then a new array is allocated, and allocSize is
 *  set.  If `l->allocSize` is not zero then the allocated size is doubled.
 */
static void TCOD_list_allocate_int(TCOD_list_t l) {
  void **newArray;
  int newSize = l->allocSize * 2;
  if (newSize == 0) { newSize = 16; }
  newArray = (void **)calloc(sizeof(void *), newSize);
  if (l->array) {
    if (l->fillSize > 0) {
      memcpy(newArray, l->array, sizeof(void *) * l->fillSize);
    }
    free(l->array);
  }
  l->array = newArray;
  l->allocSize = newSize;
}
/**
 *  Set the number of items in a list.
 *
 *  The new size will be no more than `l->allocSize`.
 */
void TCOD_list_set_size(TCOD_list_t l, int size) {
  l->fillSize = MIN(size, l->allocSize);
}
/**
 *  Return a new list.
 */
TCOD_list_t TCOD_list_new(void) {
  return (TCOD_list_t)calloc(1, sizeof(struct TCOD_List));
}
/**
 *  Return a new list which can hold up to `nb_elements` items.
 */
TCOD_list_t TCOD_list_allocate(int nb_elements) {
  TCOD_list_t l = TCOD_list_new();
  l->array = (void **)calloc(sizeof(void *), nb_elements);
  l->allocSize = nb_elements;
  return l;
}
/**
 *  Return a new copy of `l`.
 */
TCOD_list_t TCOD_list_duplicate(TCOD_list_t l) {
  int i = 0;
  void **t;
  TCOD_list_t ret = TCOD_list_allocate(l->allocSize);
  ret->fillSize = l->fillSize;
  for (t = TCOD_list_begin(l); t != TCOD_list_end(l); ++t) {
    ret->array[i++] = *t;
  }
  return ret;
}
/**
 *  Delete a list.
 *
 *  This only frees the list itself, if the list contains any pointers then
 *  those will need to be freed separately.
 */
void TCOD_list_delete(TCOD_list_t l) {
  if (l) {
    free(l->array);
  }
  free(l);
}
/**
 *  Add `elt` to the end of a list.
 */
void TCOD_list_push(TCOD_list_t l, const void *elt) {
  if (l->fillSize + 1 >= l->allocSize) { TCOD_list_allocate_int(l); }
  l->array[l->fillSize++] = (void *)elt;
}
/**
 *  Remove the last item from a list and return it.
 *
 *  If the list is empty this will return NULL.
 */
void * TCOD_list_pop(TCOD_list_t l) {
  if (l->fillSize == 0) { return NULL; }
  return l->array[--(l->fillSize)];
}
/**
 *  Return the list item from the list, without removing it.
 *
 *  If the list is empty this will return NULL.
 */
void * TCOD_list_peek(TCOD_list_t l) {
  if (l->fillSize == 0) { return NULL; }
  return l->array[l->fillSize - 1];
}
/**
 *  Add all items from `l2` to the end of `l`.
 */
void TCOD_list_add_all(TCOD_list_t l, TCOD_list_t l2) {
  void **curElt;
  for (curElt = TCOD_list_begin(l2); curElt != TCOD_list_end(l2); ++curElt) {
    TCOD_list_push(l, *curElt);
  }
}
/**
 *  Return the item at index `idx`.
 */
void * TCOD_list_get(TCOD_list_t l,int idx) {
  return l->array[idx];
}
/**
 *  Set the item at `idx` to `elt`.
 *
 *  The list will automatically resize to fit and item at `idx`.
 */
void TCOD_list_set(TCOD_list_t l,const void *elt, int idx) {
  if (idx < 0) { return; }
  while (l->allocSize < idx + 1) { TCOD_list_allocate_int(l); }
  l->array[idx] = (void *)elt;
  if (idx + 1 > l->fillSize) l->fillSize = idx + 1;
}
/**
 *  Return a pointer to the beginning of the list.
 */
void ** TCOD_list_begin(TCOD_list_t l) {
  if (l->fillSize == 0) { return (void **)NULL; }
  return &l->array[0];
}
/**
 *  Return a pointer to the end of the list.
 */
void ** TCOD_list_end(TCOD_list_t l) {
  if (l->fillSize == 0) { return (void **)NULL; }
  return &l->array[l->fillSize];
}
/**
 *  Reverse the order of the list.
 */
void TCOD_list_reverse(TCOD_list_t l) {
  void **head = TCOD_list_begin(l);
  void **tail = TCOD_list_end(l) - 1;
  while (head < tail) {
    void *tmp = *head;
    *head = *tail;
    *tail = tmp;
    ++head;
    --tail;
  }
}
/**
 *  Remove an item from the list and return a new iterator.
 */
void **TCOD_list_remove_iterator(TCOD_list_t l, void **elt) {
  void **curElt;
  for (curElt = elt; curElt < TCOD_list_end(l) - 1; ++curElt) {
    *curElt = *(curElt + 1);
  }
  l->fillSize--;
  if (l->fillSize == 0) {
    return ((void **)NULL) - 1;
  } else {
    return elt - 1;
  }
}
/**
 *  Remove an item from the list.
 */
void TCOD_list_remove(TCOD_list_t l, const void *elt) {
  void **curElt;
  for (curElt = TCOD_list_begin(l); curElt != TCOD_list_end(l); ++curElt) {
    if (*curElt == elt) {
      TCOD_list_remove_iterator(l, curElt);
      return;
    }
  }
}
/**
 *  Remove an item from the list and return a new iterator.
 *
 *  This fast version replaces the removed item with the item at the end of the
 *  list.  This is faster but does not preserve the list order.
 */
void **TCOD_list_remove_iterator_fast(TCOD_list_t l, void **elt) {
  *elt = l->array[l->fillSize-1];
  l->fillSize--;
  if (l->fillSize == 0) {
    return ((void **)NULL) - 1;
  } else {
    return elt - 1;
  }
}
/**
 *  Remove an item from the list, not preserving the list order.
 *
 *  The removed item is replaced with the item from the end of the list.
 *  This is faster but does not preserve the list order.
 */
void TCOD_list_remove_fast(TCOD_list_t l, const void *elt) {
  void **curElt;
  for (curElt = TCOD_list_begin(l); curElt != TCOD_list_end(l); ++curElt) {
    if (*curElt == elt) {
      TCOD_list_remove_iterator_fast(l, curElt);
      return;
    }
  }
}
/**
 *  Return true if `elt` is in the list.
 */
bool TCOD_list_contains(TCOD_list_t l,const void *elt) {
  void **curElt;
  for (curElt = TCOD_list_begin(l); curElt != TCOD_list_end(l); ++curElt) {
    if (*curElt == elt) { return true; }
  }
  return false;
}
/**
 *  Remove ALL items from a list.
 */
void TCOD_list_clear(TCOD_list_t l) {
  l->fillSize = 0;
}
/**
 *  Call free() on all items on the list, then remove them.
 */
void TCOD_list_clear_and_delete(TCOD_list_t l) {
  void **curElt;
  for (curElt = TCOD_list_begin(l); curElt != TCOD_list_end(l); ++curElt) {
    free(*curElt);
  }
  l->fillSize = 0;
}
/**
 *  Return the current count of items in a list.
 */
int TCOD_list_size(TCOD_list_t l) {
  return l->fillSize;
}
/**
 *  Insert `elt` on the index before `before`.
 */
void **TCOD_list_insert_before(TCOD_list_t l,const void *elt,int before) {
  int idx;
  if (l->fillSize+1 >= l->allocSize) { TCOD_list_allocate_int(l); }
  for (idx = l->fillSize; idx > before; --idx) {
    l->array[idx] = l->array[idx - 1];
  }
  l->array[before] = (void *)elt;
  l->fillSize++;
  return &l->array[before];
}
/**
 *  Return true if this list is empty.
 */
bool TCOD_list_is_empty(TCOD_list_t l) {
  return (l->fillSize == 0);
}
