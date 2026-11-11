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
/// @file list.h
/// Deprecated libtcod list module.
#pragma once
#ifndef TCOD_LIST_H_
#define TCOD_LIST_H_

#include "portability.h"

#ifdef __cplusplus
extern "C" {
#endif
/// @defgroup List List objects
/// Deprecated libtcod lists for use when required by older functions.
/// @{
/***************************************************************************
    @brief Libtcod's generic container, deprecated in favor of more standard tools.

    @deprecated
      This object is deprecated in favor of more standard tools.
      In C a library such as ``stb_ds.h`` should be used where a lower level interface isn't possible.
 */
typedef struct TCOD_List {
  /// A pointer to an array of void pointers.  Internal.
  void** array;
  /// The current count of items in the array.  Internal.
  int fillSize;
  /// The maximum number of items that `array` can currently hold.  Internal.
  int allocSize;
} TCOD_List;

typedef TCOD_List* TCOD_list_t;

/***************************************************************************
    @brief Return a new list.

    @return TCOD_List*

    @code{.c}
    TCOD_List* intList = TCOD_list_new();
    TCOD_List* floatList = TCOD_list_new();
    @endcode
 */
TCODLIB_API TCOD_DEPRECATED("TCOD_List is not a suitable container.  Use a custom array or a C++ container instead.")
    TCOD_List* TCOD_list_new(void);

/// @brief Return a new list which can hold up to `nb_elements` items.
TCODLIB_API TCOD_DEPRECATED("TCOD_List is not a suitable container.  Use a custom array or a C++ container instead.")
    TCOD_List* TCOD_list_allocate(int nb_elements);

/// @brief Return a copy of `lst`.
TCODLIB_API TCOD_List* TCOD_list_duplicate(TCOD_List* lst);

/// @brief Delete a list.
/// This only frees the list itself, if the list contains any pointers then those will need to be freed separately.
TCODLIB_API void TCOD_list_delete(TCOD_List* lst);

/// @brief Add `element` to the end of a list.
TCODLIB_API void TCOD_list_push(TCOD_List* lst, const void* element);

/// @brief Remove the last item from a list and return it. If the list is empty this will return NULL.
TCODLIB_API void* TCOD_list_pop(TCOD_List* lst);

/// @brief Return the list item from the list, without removing it. If the list is empty this will return NULL.
TCODLIB_API void* TCOD_list_peek(TCOD_List* lst);

/// @brief Add all items from `extend` to the end of `lst`.
TCODLIB_API void TCOD_list_add_all(TCOD_List* lst, TCOD_List* extend);

/// @brief Return the item at `index`.
TCODLIB_API void* TCOD_list_get(TCOD_List* lst, int index);

/// @brief Assign `element` to `lst` at `index`. The list will automatically resize to fit an item at `index`.
TCODLIB_API void TCOD_list_set(TCOD_List* lst, const void* element, int index);

/// @brief Return a pointer to the beginning of the list.
TCODLIB_API void** TCOD_list_begin(TCOD_List* lst);

/// @brief Return a pointer to the end of the list.
TCODLIB_API void** TCOD_list_end(TCOD_List* lst);

/// @brief Reverse the order of elements in the list.
TCODLIB_API void TCOD_list_reverse(TCOD_List* lst);

/// @brief Remove `iter_element` from the list and return the next iterator element.
TCODLIB_API void** TCOD_list_remove_iterator(TCOD_List* lst, void** iter_element);

/// @brief Remove `element` from the list.
TCODLIB_API void TCOD_list_remove(TCOD_List* lst, const void* element);

/// @brief Remove `iter_element` from the list and return a new iterator.
/// This replaces the element with the element from the end of the list instead of preserving the order of elements.
TCODLIB_API void** TCOD_list_remove_iterator_fast(TCOD_List* lst, void** iter_element);

/// @brief Remove an item from the list.
/// The removed item is replaced with an item from the end of the list instead of preserving the order of items.
TCODLIB_API void TCOD_list_remove_fast(TCOD_List* lst, const void* element);

/// @brief Return true if `element` is in the list.
TCODLIB_API bool TCOD_list_contains(TCOD_List* lst, const void* element);

/// @brief Remove all items from a list.
TCODLIB_API void TCOD_list_clear(TCOD_List* lst);

/// @brief Call ``free()`` on all items on the list, then remove them.
TCODLIB_API void TCOD_list_clear_and_delete(TCOD_List* lst);

/// @brief Return the current count of items in a list.
TCODLIB_API int TCOD_list_size(TCOD_List* lst);

/// @brief Insert `element` on the index before `index`.
TCODLIB_API void** TCOD_list_insert_before(TCOD_List* lst, const void* element, int index);

/// @brief Returns true if this list is empty.
TCODLIB_API bool TCOD_list_is_empty(TCOD_List* lst);
#ifdef __cplusplus
}
#endif
/// @}
#endif  // TCOD_LIST_H_
