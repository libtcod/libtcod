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
#include <assert.h>
#include <stdlib.h> /* calloc */
#include <string.h> /* NULL/memcpy */

#include "error.h"
#include "list.h"
#include "utility.h"

/// @brief Initialize or change the array of a list to a specific size.
/// @param lst Pointer to a list, NULL will return TCOD_E_INVALID_ARGUMENT
/// @param new_size Will preallocate this many items for the list.
/// @return Negative values on error.
static TCOD_Error list_allocate_set_size(TCOD_List* lst, int new_size) {
  if (!lst) return TCOD_E_INVALID_ARGUMENT;
  if (new_size <= 0) new_size = 0;

  void** new_array = NULL;
  if (new_size > 0) {
    new_array = (void**)calloc(new_size, sizeof(void*));
    if (!new_array) return TCOD_E_OUT_OF_MEMORY;
  }

  // Move previous array data if any.
  for (int i = 0; i < TCOD_MIN(lst->fillSize, new_size); ++i) new_array[i] = lst->array[i];
  free(lst->array);

  lst->array = new_array;
  lst->allocSize = new_size;
  return TCOD_E_OK;
}
/// @brief Ensures the list can hold at least `desired_size` total items.
/// @param lst NULL value will return TCOD_E_INVALID_ARGUMENT
/// @param desired_size Desired total elements.
/// @return Negative values on error.
static TCOD_Error list_reserve(TCOD_List* lst, int desired_size) {
  static const int INITIAL_ARRAY_SIZE = 16;
  if (!lst) return TCOD_E_INVALID_ARGUMENT;
  if (lst->allocSize >= desired_size) return TCOD_E_OK;

  const int new_allocate_size = TCOD_MAX(INITIAL_ARRAY_SIZE, TCOD_MAX(lst->allocSize * 2, desired_size));
  return list_allocate_set_size(lst, new_allocate_size);
}
void TCOD_list_set_size(TCOD_List* lst, int size) {
  assert(lst);
  if (lst) lst->fillSize = TCOD_MIN(size, lst->allocSize);
}
TCOD_List* TCOD_list_new(void) { return (TCOD_List*)calloc(1, sizeof(struct TCOD_List)); }
TCOD_List* TCOD_list_allocate(int nb_elements) {
  TCOD_List* lst = TCOD_list_new();
  if (!lst) return NULL;
  if (nb_elements < 0) nb_elements = 0;
  if (list_allocate_set_size(lst, nb_elements) < 0) {
    TCOD_list_delete(lst);
    return NULL;
  }
  return lst;
}
TCOD_List* TCOD_list_duplicate(TCOD_List* lst) {
  assert(lst);
  if (!lst) return NULL;
  TCOD_List* clone = TCOD_list_allocate(lst->allocSize);
  if (!clone) return NULL;
  clone->fillSize = lst->fillSize;
  for (int i = 0; i < lst->fillSize; ++i) clone->array[i] = lst->array[i];
  return clone;
}
void TCOD_list_delete(TCOD_List* lst) {
  if (lst) free(lst->array);
  free(lst);
}
void TCOD_list_push(TCOD_List* lst, const void* element) {
  assert(lst);
  if (!lst) return;
  if (list_reserve(lst, lst->fillSize + 1) < 0) return;
  lst->array[lst->fillSize++] = (void*)element;
}
void* TCOD_list_pop(TCOD_List* lst) {
  assert(lst);
  if (!lst || lst->fillSize == 0) return NULL;
  return lst->array[--(lst->fillSize)];
}
void* TCOD_list_peek(TCOD_List* lst) {
  assert(lst);
  if (!lst || lst->fillSize == 0) return NULL;
  return lst->array[lst->fillSize - 1];
}
void TCOD_list_add_all(TCOD_List* lst, TCOD_List* extend) {
  assert(lst);
  assert(extend);
  if (!lst || !extend) return;
  for (void** curElt = TCOD_list_begin(extend); curElt != TCOD_list_end(extend); ++curElt) {
    TCOD_list_push(lst, *curElt);
  }
}
void* TCOD_list_get(TCOD_List* lst, int index) {
  assert(lst);
  assert(index >= 0);
  assert(index < lst->fillSize);
  if (!lst) return NULL;
  if (index < 0 || index >= lst->fillSize) return NULL;
  return lst->array[index];
}
void TCOD_list_set(TCOD_List* lst, const void* element, int index) {
  assert(lst);
  if (index < 0) return;
  if (list_reserve(lst, index + 1) < 0) return;
  lst->array[index] = (void*)element;
  lst->fillSize = TCOD_MAX(lst->fillSize, index + 1);
}
void** TCOD_list_begin(TCOD_List* lst) {
  assert(lst);
  if (!lst || lst->fillSize == 0) return (void**)NULL;
  return &lst->array[0];
}
void** TCOD_list_end(TCOD_List* lst) {
  assert(lst);
  if (!lst || lst->fillSize == 0) return (void**)NULL;
  return &lst->array[lst->fillSize];
}
void TCOD_list_reverse(TCOD_List* lst) {
  assert(lst);
  if (!lst) return;
  void** head = TCOD_list_begin(lst);
  void** tail = TCOD_list_end(lst) - 1;
  while (head < tail) {
    void* tmp = *head;
    *head = *tail;
    *tail = tmp;
    ++head;
    --tail;
  }
}
void** TCOD_list_remove_iterator(TCOD_List* lst, void** iter_element) {
  assert(lst);
  for (void** curElt = iter_element; curElt < TCOD_list_end(lst) - 1; ++curElt) *curElt = *(curElt + 1);
  --lst->fillSize;
  if (lst->fillSize == 0) {
    return ((void**)NULL) - 1;
  } else {
    return iter_element - 1;
  }
}
void TCOD_list_remove(TCOD_List* lst, const void* element) {
  assert(lst);
  if (!lst) return;
  for (void** curElt = TCOD_list_begin(lst); curElt != TCOD_list_end(lst); ++curElt) {
    if (*curElt == element) {
      TCOD_list_remove_iterator(lst, curElt);
      return;
    }
  }
}
void** TCOD_list_remove_iterator_fast(TCOD_List* lst, void** iter_element) {
  assert(lst);
  *iter_element = lst->array[lst->fillSize - 1];
  --lst->fillSize;
  if (lst->fillSize == 0) {
    return ((void**)NULL) - 1;
  } else {
    return iter_element - 1;
  }
}
void TCOD_list_remove_fast(TCOD_List* lst, const void* element) {
  assert(lst);
  if (!lst) return;
  for (void** curElt = TCOD_list_begin(lst); curElt != TCOD_list_end(lst); ++curElt) {
    if (*curElt == element) {
      TCOD_list_remove_iterator_fast(lst, curElt);
      return;
    }
  }
}
bool TCOD_list_contains(TCOD_List* lst, const void* element) {
  assert(lst);
  if (!lst) return false;
  for (void** curElt = TCOD_list_begin(lst); curElt != TCOD_list_end(lst); ++curElt) {
    if (*curElt == element) return true;
  }
  return false;
}
void TCOD_list_clear(TCOD_List* lst) {
  assert(lst);
  if (lst) lst->fillSize = 0;
}
void TCOD_list_clear_and_delete(TCOD_List* lst) {
  assert(lst);
  if (!lst) return;
  for (void** curElt = TCOD_list_begin(lst); curElt != TCOD_list_end(lst); ++curElt) free(*curElt);
  lst->fillSize = 0;
}
int TCOD_list_size(TCOD_List* lst) {
  assert(lst);
  if (!lst) return 0;
  return lst->fillSize;
}
void** TCOD_list_insert_before(TCOD_List* lst, const void* element, int index) {
  assert(lst);
  if (list_reserve(lst, lst->fillSize + 1) < 0) return NULL;
  for (int i = lst->fillSize; i > index; --i) lst->array[i] = lst->array[i - 1];
  lst->array[index] = (void*)element;
  ++lst->fillSize;
  return &lst->array[index];
}
bool TCOD_list_is_empty(TCOD_List* lst) {
  assert(lst);
  if (!lst) return true;
  return lst->fillSize == 0;
}
