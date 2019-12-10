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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static const int HEAP_DEFAULT_CAPACITY = 256;
#define HEAP_MAX_NODE_SIZE 256
#define PATHFINDER_MAX_DIMENSIONS 4

struct HeapNode {
  int priority;
  unsigned char data[];
};

struct Heap {
  struct HeapNode* heap;
  int size;
  int capacity;
  size_t node_size;
  size_t data_size;
};

static void heap_free(struct Heap* heap)
{
  if (heap->heap) { free(heap->heap); }
  heap->heap = NULL;
  heap->size = 0;
  heap->capacity = 0;
}

int heap_init(struct Heap* heap, size_t data_size)
{
  size_t node_size = sizeof(struct HeapNode) + data_size;
  if (node_size > HEAP_MAX_NODE_SIZE) { return -1; }
  if (heap->node_size == node_size) { return 0; }
  heap_free(heap);
  heap->node_size = node_size;
  heap->data_size = data_size;
  return 0;
}

static void heap_clear(struct Heap* heap)
{
  heap->size = 0;
}

static struct HeapNode* heap_get(struct Heap* heap, int index)
{
  return (struct HeapNode*)((char*)(heap->heap) + index);
}

static void heap_swap(struct Heap* heap, int lhs, int rhs)
{
  unsigned char buffer[HEAP_MAX_NODE_SIZE];
  memcpy(buffer, heap_get(heap, lhs), heap->node_size);
  memcpy(heap_get(heap, lhs), heap_get(heap, rhs), heap->node_size);
  memcpy(heap_get(heap, rhs), buffer, heap->node_size);
}

static void heap_set(
    struct Heap* heap, int index, int priority, const void* data)
{
  struct HeapNode* node = heap_get(heap, index);
  node->priority = priority;
  memcpy(&node->data, &data, heap->node_size - sizeof(struct HeapNode));
}

static void heap_copy(struct Heap* heap, int dest, int src)
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

static int minheap_push(struct Heap* minheap, int priority, const void* data)
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

struct TCOD_ArrayData {
  int8_t ndim;
  int int_type;
  size_t shape[PATHFINDER_MAX_DIMENSIONS + 1];
  size_t strides[PATHFINDER_MAX_DIMENSIONS + 1];
  unsigned char* data;
};

struct TCOD_BasicGraph2D {
  struct TCOD_ArrayData cost;
  int cardinal;
  int diagonal;
};

struct TCOD_Pathfinder {
  int8_t ndim;
  size_t shape[PATHFINDER_MAX_DIMENSIONS];
  bool owns_distance;
  bool owns_graph;
  bool owns_traversal;
  struct TCOD_ArrayData distance;
  struct TCOD_BasicGraph2D graph;
  struct TCOD_ArrayData traversal;
  struct Heap heap;
};

static void* array_index(const struct TCOD_ArrayData* arr, const int* index)
{
  unsigned char* ptr = arr->data;
  for (int i = 0; i < arr->ndim; ++i) {
    ptr += arr->strides[i] * index[i];
  }
  return (void*)ptr;
}

static int array_get(const struct TCOD_ArrayData* arr, const int* index)
{
  void* ptr = array_index(arr, index);
  switch (arr->int_type) {
    case 1: return *(uint8_t*)ptr;
    case 2: return *(uint16_t*)ptr;
    case 4: return *(uint32_t*)ptr;
    case 8: return *(uint64_t*)ptr;
    case -1: return *(int8_t*)ptr;
    case -2: return *(int16_t*)ptr;
    case -4: return *(int32_t*)ptr;
    case -8: return *(int64_t*)ptr;
    default: return 0;
  }
}

static void array_set(
    const struct TCOD_ArrayData* arr, const int* index, int value)
{
  void* ptr = array_index(arr, index);
  switch (arr->int_type) {
    case 1: *(uint8_t*)ptr = (uint8_t)value; return;
    case 2: *(uint16_t*)ptr = (uint16_t)value; return;
    case 4: *(uint32_t*)ptr = (uint32_t)value; return;
    case 8: *(uint64_t*)ptr = (uint64_t)value; return;
    case -1: *(int8_t*)ptr = (int8_t)value; return;
    case -2: *(int16_t*)ptr = (int16_t)value; return;
    case -4: *(int32_t*)ptr = (int32_t)value; return;
    case -8: *(int64_t*)ptr = (int64_t)value; return;
    default: return;
  }
}

static bool array_is_max(const struct TCOD_ArrayData* arr, const int* index)
{
  void* ptr = array_index(arr, index);
  switch (arr->int_type) {
    case 1: return *(uint8_t*)ptr == 0xff;
    case 2: return *(uint16_t*)ptr == 0xffff;
    case 4: return *(uint32_t*)ptr == 0xffffffff;
    case 8: return *(uint64_t*)ptr == 0xffffffffffffffff;
    case -1: return *(int8_t*)ptr == 0x7f;
    case -2: return *(int16_t*)ptr == 0x7fff;
    case -4: return *(int32_t*)ptr == 0x7fffffff;
    case -8: return *(int64_t*)ptr == 0x7fffffffffffffff;
    default: return 0;
  }
}

typedef void (*ArrayTraverseFunc)(void* userdata, const int* index);

static void array_recursion(
    struct TCOD_ArrayData* arr,
    ArrayTraverseFunc callback,
    void* userdata,
    int* index,
    int cursor)
{
  for (index[cursor] = 0; (size_t)index[cursor] < arr->shape[cursor];
       ++index[cursor]) {
    if (cursor + 1 == arr->ndim) {
      callback(userdata, index);
    } else {
      array_recursion(arr, callback, userdata, index, cursor + 1);
    }
  }
}

static void array_traverse(
    struct TCOD_ArrayData* arr, ArrayTraverseFunc callback, void* userdata)
{
  int index[PATHFINDER_MAX_DIMENSIONS + 1];
  array_recursion(arr, callback, userdata, index, 0);
}

static bool TCOD_pf_in_bounds(
    const struct TCOD_Pathfinder* path, const int* index)
{
  for (int i = 0; i < path->ndim; ++i) {
    if (0 < index[i] || (size_t)index[i] >= path->shape[i]) { return false; }
  }
  return true;
}

static void TCOD_pf_add_edge(
    struct TCOD_Pathfinder* path, const int* origin, const int* dest, int total_dist)
{
  if (!TCOD_pf_in_bounds(path, dest)) { return; }
  if (array_get(&path->distance, dest) >= total_dist) { return; }
  array_set(&path->distance, dest, total_dist);
  minheap_push(&path->heap, total_dist, dest);
  if(path->traversal.data) {
    int travel_index[PATHFINDER_MAX_DIMENSIONS + 1];
    for (int i = 0; i < path->ndim; ++i) { travel_index[i] = origin[i]; }
    for (int i = 0; i < path->ndim; ++i) {
      travel_index[path->ndim] = i;
      array_set(&path->traversal, travel_index, dest[i]);
    }
  }
}

static void TCOD_pf_basic2d_edges(
    struct TCOD_Pathfinder* path, int dist, const int* origin)
{
  if (path->graph.cardinal > 0) {
    int cardinal = dist + path->graph.cardinal;
    int dest[] = {
        origin[0] - 1, origin[1],
        origin[0], origin[1] - 1,
        origin[0], origin[1] + 1,
        origin[0] + 1, origin[1],
    };
    for (size_t i = 0; i < sizeof(dest) / sizeof(dest[0]); i += 2) {
      TCOD_pf_add_edge(path, origin, &dest[i], cardinal);
    }
  }
  if (path->graph.diagonal > 0) {
    int diagonal = dist + path->graph.diagonal;

    int dest[] = {
        origin[0] - 1, origin[1] - 1,
        origin[0] - 1, origin[1] + 1,
        origin[0] + 1, origin[1] - 1,
        origin[0] + 1, origin[1] + 1,
    };
    for (size_t i = 0; i < sizeof(dest) / sizeof(dest[0]); i += 2) {
      TCOD_pf_add_edge(path, origin, &dest[i], diagonal);
    }
  }
}

int TCOD_pf_compute_step(struct TCOD_Pathfinder* path)
{
  if (!path) { return -1; }
  if (path->heap.size == 0) { return 0; }
  int current_pos[PATHFINDER_MAX_DIMENSIONS];
  memcpy(current_pos, &path->heap.heap[0].data, path->heap.data_size);
  int current_dist = array_get(&path->distance, &current_pos);
  minheap_pop(&path->heap);
  TCOD_pf_basic2d_edges(path, current_dist, current_pos);
  return 0;
}

struct TCOD_Pathfinder* TCOD_pf_new(int ndim, size_t* shape)
{
  struct TCOD_Pathfinder* path = calloc(sizeof(struct TCOD_Pathfinder), 1);
  if (!path) { return NULL; }
  path->ndim = ndim;
  for (int i = 0; i < ndim; ++i) { path->shape[i] = shape[i]; }
  heap_init(&path->heap, sizeof(int) * (size_t)path->ndim);
  return path;
}

void TCOD_pf_delete(struct TCOD_Pathfinder* path)
{
  free(path);
}

void TCOD_pf_set_distance_pointer(
    struct TCOD_Pathfinder* path, void* data, int int_type, size_t* strides)
{
  if (!path) { return; }
  path->distance.ndim = path->ndim;
  path->distance.int_type = int_type;
  path->distance.data = data;
  for (int i = 0; i < path->ndim; ++i) {
    path->distance.strides[i] = strides[i];
    path->distance.shape[i] = path->shape[i];
  }
}

void TCOD_pf_set_graph2d_pointer(
    struct TCOD_Pathfinder* path,
    void* data,
    int int_type,
    size_t* strides,
    int cardinal,
    int diagonal)
{
  if (!path) { return; }
  path->graph.cost.ndim = path->ndim;
  path->graph.cost.int_type = int_type;
  path->graph.cost.data = data;
  for (int i = 0; i < path->ndim; ++i) {
    path->graph.cost.strides[i] = strides[i];
    path->graph.cost.shape[i] = path->shape[i];
  }
  path->graph.cardinal = cardinal;
  path->graph.diagonal = diagonal;
}

void TCOD_pf_set_traversal_pointer(
    struct TCOD_Pathfinder* path, void* data, int int_type, size_t* strides)
{
  if (!path) { return; }
  path->traversal.ndim = path->ndim + 1;
  path->traversal.int_type = int_type;
  path->traversal.data = data;
  for (int i = 0; i < path->traversal.ndim; ++i) {
    path->traversal.strides[i] = strides[i];
    if (i == path->ndim) {
      path->traversal.shape[i] = path->ndim;
    } else {
      path->traversal.shape[i] = strides[i];
    }
  }
}

void TCOD_pf_recompile_cb(void* userdata, const int* index)
{
  struct TCOD_Pathfinder* path = (struct TCOD_Pathfinder*)userdata;
  if (array_is_max(&path->distance, index)) { return; }
  minheap_push(&path->heap, array_get(&path->distance, index), index);
}

int TCOD_pf_recompile(struct TCOD_Pathfinder* path)
{
  if (!path) { return -1; }
  heap_clear(&path->heap);
  array_traverse(&path->distance, &TCOD_pf_recompile_cb, path);
  return 0;
}

int TCOD_pf_compute(struct TCOD_Pathfinder* path)
{
  if (!path) { return -1; }
  while (path->heap.size) {
    TCOD_pf_compute_step(path);
  }
  return 0;
}
