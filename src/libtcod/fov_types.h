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
#ifndef TCOD_FOV_TYPES_H_
#define TCOD_FOV_TYPES_H_
#include "portability.h"
/**
 *  Private map cell struct.
 */
struct TCOD_MapCell {
  bool transparent;
  bool walkable;
  bool fov;
};
/**
 *  Private map struct.
 */
typedef struct TCOD_Map {
  int width;
  int height;
  int nbcells;
  struct TCOD_MapCell* __restrict cells;
} TCOD_Map;
typedef TCOD_Map* TCOD_map_t;
/**
    \rst
    Field-of-view options for :any:`TCOD_map_compute_fov`.
    \endrst
 */
typedef enum {
  /**
      Trace multiple Bresenham lines along the perimeter.

      Based on: http://www.roguebasin.com/index.php?title=Ray_casting
   */
  FOV_BASIC,
  /**
      Cast Bresenham line shadows on a per-tile basis.

      Based on: http://www.oocities.org/temerra/los_rays.html
   */
  FOV_DIAMOND,
  /**
      Recursive Shadowcast.

      Based on: http://www.roguebasin.com/index.php?title=FOV_using_recursive_shadowcasting
   */
  FOV_SHADOW,
  /**
      Precise Permissive Field of View.

      Based on: http://www.roguebasin.com/index.php?title=Precise_Permissive_Field_of_View
   */
  FOV_PERMISSIVE_0,
  FOV_PERMISSIVE_1,
  FOV_PERMISSIVE_2,
  FOV_PERMISSIVE_3,
  FOV_PERMISSIVE_4,
  FOV_PERMISSIVE_5,
  FOV_PERMISSIVE_6,
  FOV_PERMISSIVE_7,
  FOV_PERMISSIVE_8,
  /**
      Mingos' Restrictive Precise Angle Shadowcasting (contribution by Mingos)

      Based on: http://www.roguebasin.com/index.php?title=Restrictive_Precise_Angle_Shadowcasting
   */
  FOV_RESTRICTIVE,
  /**
      Symmetric Shadowcast.

      Based on: https://www.albertford.com/shadowcasting/
      \rst
      .. versionadded :: 1.16
      \endrst
   */
  FOV_SYMMETRIC_SHADOWCAST,
  NB_FOV_ALGORITHMS
} TCOD_fov_algorithm_t;
#define FOV_PERMISSIVE(x) ((TCOD_fov_algorithm_t)(FOV_PERMISSIVE_0 + (x)))
#endif /* TCOD_FOV_TYPES_H_ */
