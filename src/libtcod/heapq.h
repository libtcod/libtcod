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
#pragma once
#ifndef TCOD_HEAPQ_H
#define TCOD_HEAPQ_H

#include <stdbool.h>
#include <stddef.h>

#include "config.h"

struct TCOD_Heap {
  unsigned char* __restrict heap;
  int size;  // The current number of elements in heap.
  int capacity;  // The current capacity of heap.
  size_t node_size;  // The full size of each node in bytes.
  size_t data_size;  // The size of a nodes user data section in bytes.
  size_t data_offset;  // The offset of the user data section.
  int priority_type;  // Should be -4.
};

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
TCOD_PUBLIC int TCOD_heap_init(struct TCOD_Heap* heap, size_t data_size);
TCOD_PUBLIC void TCOD_heap_uninit(struct TCOD_Heap* heap);

TCOD_PUBLIC void TCOD_heap_clear(struct TCOD_Heap* heap);

TCOD_PUBLIC int TCOD_minheap_push(struct TCOD_Heap* __restrict minheap, int priority, const void* __restrict data);
TCOD_PUBLIC void TCOD_minheap_pop(struct TCOD_Heap* __restrict minheap, void* __restrict out);
TCOD_PUBLIC void TCOD_minheap_heapify(struct TCOD_Heap* minheap);
#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
#endif  // TCOD_HEAPQ_H
