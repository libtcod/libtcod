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
#include "pathfinder_frontier.h"

#include <stdlib.h>

struct TCOD_Frontier* TCOD_frontier_new(int ndim) {
  if (ndim <= 0 || TCOD_PATHFINDER_MAX_DIMENSIONS < ndim) {
    TCOD_set_errorvf("Can not make a pathfinder with %i dimensions.", ndim);
    return NULL;
  }
  struct TCOD_Frontier* frontier = calloc(1, sizeof(*frontier));
  if (!frontier) {
    TCOD_set_errorv("Out of memory allocating pathfinder.");
    return NULL;
  }
  frontier->ndim = (int8_t)ndim;
  TCOD_heap_init(&frontier->heap, sizeof(int) * (ndim + 1));
  return frontier;
}
void TCOD_frontier_delete(struct TCOD_Frontier* frontier) {
  if (!frontier) {
    return;
  }
  TCOD_heap_uninit(&frontier->heap);
  free(frontier);
}
TCOD_Error TCOD_frontier_pop(struct TCOD_Frontier* frontier) {
  if (!frontier) {
    TCOD_set_errorv("Pointer argument must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  if (frontier->heap.size == 0) {
    TCOD_set_errorv("Heap is empty.");
    return TCOD_E_ERROR;
  }
  int node[TCOD_PATHFINDER_MAX_DIMENSIONS + 1];
  TCOD_minheap_pop(&frontier->heap, node);
  frontier->active_dist = node[0];
  for (int i = 0; i < frontier->ndim; ++i) {
    frontier->active_index[i] = node[i + 1];
  }
  return TCOD_E_OK;
}
TCOD_Error TCOD_frontier_push(
    struct TCOD_Frontier* __restrict frontier, const int* __restrict index, int dist, int heuristic) {
  if (!frontier) {
    TCOD_set_errorv("Pointer argument must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  int node[TCOD_PATHFINDER_MAX_DIMENSIONS + 1];
  node[0] = dist;
  for (int i = 0; i < frontier->ndim; ++i) {
    node[i + 1] = index[i];
  }
  TCOD_minheap_push(&frontier->heap, heuristic, node);
  return TCOD_E_OK;
}
int TCOD_frontier_size(const struct TCOD_Frontier* frontier) {
  if (!frontier) {
    TCOD_set_errorv("Pointer argument must not be NULL.");
    return 0;
  }
  return frontier->heap.size;
}
TCOD_Error TCOD_frontier_clear(struct TCOD_Frontier* frontier) {
  if (!frontier) {
    TCOD_set_errorv("Pointer argument must not be NULL.");
    return TCOD_E_INVALID_ARGUMENT;
  }
  TCOD_heap_clear(&frontier->heap);
  return TCOD_E_OK;
}
