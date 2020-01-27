/* BSD 3-Clause License
 *
 * Copyright © 2008-2020, Jice and the libtcod contributors.
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
#ifndef TCOD_HEAPQ_H
#define TCOD_HEAPQ_H

#include <stdbool.h>
#include <stddef.h>

#include "config.h"

#define TCOD_HEAP_DEFAULT_CAPACITY 256
#define TCOD_HEAP_MAX_NODE_SIZE 256

struct TCOD_HeapNode {
  int priority;
#if !defined(__cplusplus)
  unsigned char data[];
#endif // !defined(__cplusplus)
};

struct TCOD_Heap {
  struct TCOD_HeapNode* heap;
  int size;
  int capacity;
  size_t node_size;
  size_t data_size;
};

TCODLIB_CAPI int TCOD_heap_init(struct TCOD_Heap* heap, size_t data_size);
TCODLIB_CAPI void TCOD_heap_uninit(struct TCOD_Heap* heap);

TCODLIB_CAPI void TCOD_heap_clear(struct TCOD_Heap* heap);

TCODLIB_CAPI int TCOD_minheap_push(
    struct TCOD_Heap* minheap, int priority, const void* data);
TCODLIB_CAPI void TCOD_minheap_pop(struct TCOD_Heap* minheap, void* out);

#endif // TCOD_HEAPQ_H
