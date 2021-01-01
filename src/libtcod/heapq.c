/* BSD 3-Clause License
 *
 * Copyright © 2008-2021, Jice and the libtcod contributors.
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
#include "heapq.h"

#include <stdlib.h>
#include <string.h>

#include "error.h"

void TCOD_heap_uninit(struct TCOD_Heap* heap) {
  if (heap->heap) {
    free(heap->heap);
  }
  heap->heap = NULL;
  heap->size = 0;
  heap->capacity = 0;
  heap->node_size = 0;
  heap->data_size = 0;
}

int TCOD_heap_init(struct TCOD_Heap* heap, size_t data_size) {
  size_t node_size = sizeof(struct TCOD_HeapNode) + data_size;
  if (node_size > TCOD_HEAP_MAX_NODE_SIZE) {
    return TCOD_set_errorvf("Heap data size is too large: %i", (int)node_size);
  }
  heap->heap = NULL;
  heap->size = 0;
  heap->capacity = 0;
  heap->node_size = node_size;
  heap->data_size = data_size;
  return 0;
}

void TCOD_heap_clear(struct TCOD_Heap* heap) { heap->size = 0; }

static struct TCOD_HeapNode* TCOD_heap_get(struct TCOD_Heap* heap, int index) {
  return (struct TCOD_HeapNode*)((char*)(heap->heap) + index * heap->node_size);
}

static void TCOD_heap_swap(struct TCOD_Heap* heap, int lhs, int rhs) {
  unsigned char buffer[TCOD_HEAP_MAX_NODE_SIZE];
  memcpy(buffer, TCOD_heap_get(heap, lhs), heap->node_size);
  memcpy(TCOD_heap_get(heap, lhs), TCOD_heap_get(heap, rhs), heap->node_size);
  memcpy(TCOD_heap_get(heap, rhs), buffer, heap->node_size);
}

static void TCOD_heap_set(struct TCOD_Heap* __restrict heap, int index, int priority, const void* __restrict data) {
  struct TCOD_HeapNode* node = TCOD_heap_get(heap, index);
  node->priority = priority;
  memcpy(node->data, data, heap->node_size - sizeof(struct TCOD_HeapNode));
}

static void TCOD_heap_copy(struct TCOD_Heap* heap, int dest, int src) {
  memcpy(TCOD_heap_get(heap, dest), TCOD_heap_get(heap, src), heap->node_size);
}

static bool TCOD_minheap_compare(struct TCOD_Heap* minheap, int lhs, int rhs) {
  return (TCOD_heap_get(minheap, lhs)->priority < TCOD_heap_get(minheap, rhs)->priority);
}

static void TCOD_TCOD_minheap_heapify_down(struct TCOD_Heap* minheap, int index) {
  int canidate = index;
  int left = index * 2 + 1;
  int right = index * 2 + 2;
  if (left < minheap->size && TCOD_minheap_compare(minheap, left, canidate)) {
    canidate = left;
  }
  if (right < minheap->size && TCOD_minheap_compare(minheap, right, canidate)) {
    canidate = right;
  }
  if (canidate != index) {
    TCOD_heap_swap(minheap, index, canidate);
    TCOD_TCOD_minheap_heapify_down(minheap, canidate);
  }
}

static void TCOD_TCOD_minheap_heapify_up(struct TCOD_Heap* minheap, int index) {
  if (index == 0) {
    return;
  }
  int parent = (index - 1) >> 1;
  if (TCOD_minheap_compare(minheap, index, parent)) {
    TCOD_heap_swap(minheap, index, parent);
    TCOD_TCOD_minheap_heapify_up(minheap, parent);
  }
}
void TCOD_minheap_heapify(struct TCOD_Heap* minheap) {
  for (int i = minheap->size / 2; i >= 0; --i) {
    TCOD_TCOD_minheap_heapify_down(minheap, i);
  }
}
void TCOD_minheap_pop(struct TCOD_Heap* __restrict minheap, void* __restrict out) {
  if (minheap->size == 0) {
    return;
  }
  if (out) {
    memcpy(out, minheap->heap[0].data, minheap->data_size);
  }
  TCOD_heap_copy(minheap, 0, minheap->size - 1);
  --minheap->size;
  TCOD_TCOD_minheap_heapify_down(minheap, 0);
}

int TCOD_minheap_push(struct TCOD_Heap* __restrict minheap, int priority, const void* __restrict data) {
  if (minheap->size == minheap->capacity) {
    int new_capacity = (minheap->capacity ? minheap->capacity * 2 : TCOD_HEAP_DEFAULT_CAPACITY);
    void* new_heap = realloc(minheap->heap, minheap->node_size * new_capacity);
    if (!new_heap) {
      return -1;
    }  // Out of memory.
    minheap->capacity = new_capacity;
    minheap->heap = (struct TCOD_HeapNode*)new_heap;
  }
  ++minheap->size;
  TCOD_heap_set(minheap, minheap->size - 1, priority, data);
  TCOD_TCOD_minheap_heapify_up(minheap, minheap->size - 1);
  return 0;
}
