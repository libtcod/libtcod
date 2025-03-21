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
#ifndef TCOD_PATHFINDER_H
#define TCOD_PATHFINDER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "heapq.h"
#include "portability.h"

#define TCOD_PATHFINDER_MAX_DIMENSIONS 4

struct TCOD_ArrayData {
  int8_t ndim;
  int int_type;
  size_t shape[TCOD_PATHFINDER_MAX_DIMENSIONS + 1];
  size_t strides[TCOD_PATHFINDER_MAX_DIMENSIONS + 1];
  unsigned char* data;
};

struct TCOD_BasicGraph2D {
  struct TCOD_ArrayData cost;
  int cardinal;
  int diagonal;
};

struct TCOD_Pathfinder {
  int8_t ndim;
  size_t shape[TCOD_PATHFINDER_MAX_DIMENSIONS];
  bool owns_distance;
  bool owns_graph;
  bool owns_traversal;
  struct TCOD_ArrayData distance;
  struct TCOD_BasicGraph2D graph;
  struct TCOD_ArrayData traversal;
  struct TCOD_Heap heap;
};

TCODLIB_CAPI struct TCOD_Pathfinder* TCOD_pf_new(int ndim, const size_t* shape);
TCODLIB_CAPI void TCOD_pf_delete(struct TCOD_Pathfinder* path);

TCODLIB_CAPI void TCOD_pf_set_distance_pointer(
    struct TCOD_Pathfinder* path, void* data, int int_type, const size_t* strides);
TCODLIB_CAPI void TCOD_pf_set_graph2d_pointer(
    struct TCOD_Pathfinder* path, void* data, int int_type, const size_t* strides, int cardinal, int diagonal);
TCODLIB_CAPI void TCOD_pf_set_traversal_pointer(
    struct TCOD_Pathfinder* path, void* data, int int_type, const size_t* strides);

TCODLIB_CAPI int TCOD_pf_recompile(struct TCOD_Pathfinder* path);
TCODLIB_CAPI int TCOD_pf_compute(struct TCOD_Pathfinder* path);
TCODLIB_CAPI int TCOD_pf_compute_step(struct TCOD_Pathfinder* path);

#endif  // TCOD_PATHFINDER_H
