/* BSD 3-Clause License
 *
 * Copyright © 2008-2025, Jice and the libtcod contributors.
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

TCODLIB_API TCOD_List* TCOD_list_allocate(int nb_elements);
TCODLIB_API TCOD_List* TCOD_list_duplicate(TCOD_List* l);
TCODLIB_API void TCOD_list_delete(TCOD_List* l);
TCODLIB_API void TCOD_list_push(TCOD_List* l, const void* elt);
TCODLIB_API void* TCOD_list_pop(TCOD_List* l);
TCODLIB_API void* TCOD_list_peek(TCOD_List* l);
TCODLIB_API void TCOD_list_add_all(TCOD_List* l, TCOD_List* l2);
TCODLIB_API void* TCOD_list_get(TCOD_List* l, int idx);
TCODLIB_API void TCOD_list_set(TCOD_List* l, const void* elt, int idx);
TCODLIB_API void** TCOD_list_begin(TCOD_List* l);
TCODLIB_API void** TCOD_list_end(TCOD_List* l);
TCODLIB_API void TCOD_list_reverse(TCOD_List* l);
TCODLIB_API void** TCOD_list_remove_iterator(TCOD_List* l, void** elt);
TCODLIB_API void TCOD_list_remove(TCOD_List* l, const void* elt);
TCODLIB_API void** TCOD_list_remove_iterator_fast(TCOD_List* l, void** elt);
TCODLIB_API void TCOD_list_remove_fast(TCOD_List* l, const void* elt);
TCODLIB_API bool TCOD_list_contains(TCOD_List* l, const void* elt);
TCODLIB_API void TCOD_list_clear(TCOD_List* l);
TCODLIB_API void TCOD_list_clear_and_delete(TCOD_List* l);
TCODLIB_API int TCOD_list_size(TCOD_List* l);
TCODLIB_API void** TCOD_list_insert_before(TCOD_List* l, const void* elt, int before);
TCODLIB_API bool TCOD_list_is_empty(TCOD_List* l);
#ifdef __cplusplus
}
#endif
#endif  // TCOD_LIST_H_
