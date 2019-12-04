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
#include "pathfinder.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static const int HEAP_DEFAULT_CAPACITY = 256;
#define HEAP_MAX_NODE_SIZE 256

struct HeapNode {
  int priority;
  unsigned char data[];
};

struct Heap {
  struct HeapNode* heap;
  int size;
  int capacity;
  size_t node_size;
};

static struct HeapNode* heap_free(struct Heap* heap)
{
  if (heap->heap) { free(heap->heap); }
  heap->heap = NULL;
  heap->size = 0;
  heap->capacity = 0;
}

static struct HeapNode* heap_clear(struct Heap* heap)
{
  heap->size = 0;
}

static struct HeapNode* heap_get(struct Heap* heap, int index)
{
  return (struct HeapNode*)((char*)(heap->heap) + index);
}

static struct HeapNode* heap_swap(struct Heap* heap, int lhs, int rhs)
{
  unsigned char buffer[HEAP_MAX_NODE_SIZE];
  memcpy(buffer, heap_get(heap, lhs), heap->node_size);
  memcpy(heap_get(heap, lhs), heap_get(heap, rhs), heap->node_size);
  memcpy(heap_get(heap, rhs), buffer, heap->node_size);
}

static struct HeapNode* heap_set(
    struct Heap* heap, int index, int priority, void* data)
{
  struct HeapNode* node = heap_get(heap, index);
  node->priority = priority;
  memcpy(&node->data, &data, heap->node_size - sizeof(struct HeapNode));
}

static struct HeapNode* heap_copy(struct Heap* heap, int dest, int src)
{
  memcpy(heap_get(heap, dest), heap_get(heap, src), heap->node_size);
}

static bool minheap_compare(struct Heap* minheap, int lhs, int rhs)
{
  return heap_get(minheap, lhs)->priority < heap_get(minheap, rhs)->priority;
}

static void minheap_heapify_down(struct Heap* minheap, int index)
{
  int canidate = index;
  int left = index * 2 + 1;
  int right = index * 2 + 2;
  if (left < minheap->size && minheap_compare(minheap, left, canidate)) {
    canidate = left;
  }
  if (right < minheap->size && minheap_compare(minheap, right, canidate)) {
    canidate = right;
  }
  if (canidate != index) {
    heap_swap(minheap, index, canidate);
    minheap_heapify_down(minheap, canidate);
  }
}

static void minheap_heapify_up(struct Heap* minheap, int index)
{
  if (index == 0) { return; }
  int parent = (index - 1) >> 1;
  if (minheap_compare(minheap, index, parent)) {
    heap_swap(minheap, index, parent);
    minheap_heapify_up(minheap, parent);
  }
}

static void minheap_heapify(struct Heap* minheap)
{
  for (int i = minheap->size / 2; i >= 0; --i) {
    minheap_heapify_down(minheap, i);
  }
}

static void minheap_pop(struct Heap* minheap)
{
  if (minheap->size == 0) { return; }
  heap_copy(minheap, 0, minheap->size - 1);
  --minheap->size;
  minheap_heapify_down(minheap, 0);
}

static int minheap_push(struct Heap* minheap, int priority, void* data)
{
  if (minheap->size == minheap->capacity) {
    int new_capacity = (
        minheap->capacity ? minheap->capacity * 2 : HEAP_DEFAULT_CAPACITY
    );
    void* new_heap = realloc(minheap->heap, minheap->node_size * new_capacity);
    if (!new_heap) { return -1; } // Out of memory.
    minheap->capacity = new_capacity;
    minheap->heap = (struct HeapNode*)new_heap;
  }
  ++minheap->size;
  heap_set(minheap, minheap->size - 1, priority, data);
  minheap_heapify_up(minheap, minheap->size - 1);
  return 0;
}
