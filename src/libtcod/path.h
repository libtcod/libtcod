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
#ifndef TCOD_PATH_H_
#define TCOD_PATH_H_

#include "fov_types.h"
#include "list.h"
#include "portability.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef float (*TCOD_path_func_t)(int xFrom, int yFrom, int xTo, int yTo, void* user_data);
struct TCOD_Path;
typedef struct TCOD_Path* TCOD_path_t;

TCODLIB_API TCOD_path_t TCOD_path_new_using_map(TCOD_Map* map, float diagonalCost);
TCODLIB_API TCOD_path_t
TCOD_path_new_using_function(int map_width, int map_height, TCOD_path_func_t func, void* user_data, float diagonalCost);

TCODLIB_API bool TCOD_path_compute(TCOD_path_t path, int ox, int oy, int dx, int dy);
TCODLIB_API bool TCOD_path_walk(TCOD_path_t path, int* x, int* y, bool recalculate_when_needed);
TCODLIB_API bool TCOD_path_is_empty(TCOD_path_t path);
TCODLIB_API int TCOD_path_size(TCOD_path_t path);
TCODLIB_API void TCOD_path_reverse(TCOD_path_t path);
TCODLIB_API void TCOD_path_get(TCOD_path_t path, int index, int* x, int* y);
TCODLIB_API void TCOD_path_get_origin(TCOD_path_t path, int* x, int* y);
TCODLIB_API void TCOD_path_get_destination(TCOD_path_t path, int* x, int* y);
TCODLIB_API void TCOD_path_delete(TCOD_path_t path);

/* Dijkstra stuff - by Mingos*/
/**
 *  Dijkstra data structure
 *
 *  All attributes are considered private.
 */
typedef struct TCOD_Dijkstra {
  int diagonal_cost;
  int width, height, nodes_max;
  TCOD_Map* map; /* a TCODMap with walkability data */
  TCOD_path_func_t func;
  void* user_data;
  unsigned int* distances; /* distances grid */
  unsigned int* nodes; /* the processed nodes */
  TCOD_list_t path;
} TCOD_Dijkstra;
typedef struct TCOD_Dijkstra* TCOD_dijkstra_t;

TCODLIB_API TCOD_Dijkstra* TCOD_dijkstra_new(TCOD_Map* map, float diagonalCost);
TCODLIB_API TCOD_Dijkstra* TCOD_dijkstra_new_using_function(
    int map_width, int map_height, TCOD_path_func_t func, void* user_data, float diagonalCost);
TCODLIB_API void TCOD_dijkstra_compute(TCOD_Dijkstra* dijkstra, int root_x, int root_y);
TCODLIB_API float TCOD_dijkstra_get_distance(TCOD_Dijkstra* dijkstra, int x, int y);
TCODLIB_API bool TCOD_dijkstra_path_set(TCOD_Dijkstra* dijkstra, int x, int y);
TCODLIB_API bool TCOD_dijkstra_is_empty(TCOD_Dijkstra* path);
TCODLIB_API int TCOD_dijkstra_size(TCOD_Dijkstra* path);
TCODLIB_API void TCOD_dijkstra_reverse(TCOD_Dijkstra* path);
TCODLIB_API void TCOD_dijkstra_get(TCOD_Dijkstra* path, int index, int* x, int* y);
TCODLIB_API bool TCOD_dijkstra_path_walk(TCOD_Dijkstra* dijkstra, int* x, int* y);
TCODLIB_API void TCOD_dijkstra_delete(TCOD_Dijkstra* dijkstra);
#ifdef __cplusplus
}
#endif
#endif  // TCOD_PATH_H_
