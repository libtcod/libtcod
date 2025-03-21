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
#ifndef TCOD_BSP_H_
#define TCOD_BSP_H_

#include "mersenne_types.h"
#include "portability.h"
#include "tree.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
  TCOD_tree_t tree; /* pseudo oop : bsp inherit tree */
  int x, y, w, h; /* node position & size */
  int position; /* position of splitting */
  uint8_t level; /* level in the tree */
  bool horizontal; /* horizontal splitting ? */
} TCOD_bsp_t;

typedef bool (*TCOD_bsp_callback_t)(TCOD_bsp_t* node, void* userData);

TCODLIB_API TCOD_bsp_t* TCOD_bsp_new(void);
TCODLIB_API TCOD_bsp_t* TCOD_bsp_new_with_size(int x, int y, int w, int h);
TCODLIB_API void TCOD_bsp_delete(TCOD_bsp_t* node);

TCODLIB_API TCOD_bsp_t* TCOD_bsp_left(TCOD_bsp_t* node);
TCODLIB_API TCOD_bsp_t* TCOD_bsp_right(TCOD_bsp_t* node);
TCODLIB_API TCOD_bsp_t* TCOD_bsp_father(TCOD_bsp_t* node);

TCODLIB_API bool TCOD_bsp_is_leaf(TCOD_bsp_t* node);
TCODLIB_API bool TCOD_bsp_traverse_pre_order(TCOD_bsp_t* node, TCOD_bsp_callback_t listener, void* userData);
TCODLIB_API bool TCOD_bsp_traverse_in_order(TCOD_bsp_t* node, TCOD_bsp_callback_t listener, void* userData);
TCODLIB_API bool TCOD_bsp_traverse_post_order(TCOD_bsp_t* node, TCOD_bsp_callback_t listener, void* userData);
TCODLIB_API bool TCOD_bsp_traverse_level_order(TCOD_bsp_t* node, TCOD_bsp_callback_t listener, void* userData);
TCODLIB_API bool TCOD_bsp_traverse_inverted_level_order(TCOD_bsp_t* node, TCOD_bsp_callback_t listener, void* userData);
TCODLIB_API bool TCOD_bsp_contains(TCOD_bsp_t* node, int x, int y);
TCODLIB_API TCOD_bsp_t* TCOD_bsp_find_node(TCOD_bsp_t* node, int x, int y);
TCODLIB_API void TCOD_bsp_resize(TCOD_bsp_t* node, int x, int y, int w, int h);
TCODLIB_API void TCOD_bsp_split_once(TCOD_bsp_t* node, bool horizontal, int position);
TCODLIB_API void TCOD_bsp_split_recursive(
    TCOD_bsp_t* node, TCOD_Random* randomizer, int nb, int minHSize, int minVSize, float maxHRatio, float maxVRatio);
TCODLIB_API void TCOD_bsp_remove_sons(TCOD_bsp_t* node);
#ifdef __cplusplus
}
#endif
#endif  // TCOD_BSP_H_
