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
#ifndef TCOD_MOUSE_TYPES_H_
#define TCOD_MOUSE_TYPES_H_
/***************************************************************************
    @file mouse_types.h
    @brief Mouse state provided by the libtcod event system.
 */
#include "portability.h"
/***************************************************************************
    @brief Mouse state provided by the libtcod event system.
    This may be a moved, pressed, or released event.

    @deprecated
    The libtcod mouse state has several known issues such as missing or broken functionality.
    In its current state it exists only for backwards compatibility.
    These issues should be resolved by using SDL directly for mouse and keyboard events.
 */
typedef struct TCOD_mouse_t {
  /***************************************************************************
      @brief The mouse absolute pixel position, according to SDL.
   */
  int x, y;
  /***************************************************************************
      @brief The mouse relative pixel motion, according to SDL.
   */
  int dx, dy;
  /***************************************************************************
      @brief The mouse cell coordinates for the root console or the last presented console.
   */
  int cx, cy;
  /***************************************************************************
      @brief The mouse cell movement for the root console or the last presented console.
   */
  int dcx, dcy;
  /***************************************************************************
      @brief True when the Left mouse button is held.
   */
  bool lbutton;
  /***************************************************************************
      @brief True when the right mouse button is held.
   */
  bool rbutton;
  /***************************************************************************
      @brief True when the middle mouse button is held.
   */
  bool mbutton;
  /***************************************************************************
      @brief True when the left mouse button has just been released.
   */
  bool lbutton_pressed;
  /***************************************************************************
      @brief True when the right mouse button has just been released.
   */
  bool rbutton_pressed;
  /***************************************************************************
      @brief True when the middle mouse button has just been released.
   */
  bool mbutton_pressed;
  /***************************************************************************
      @brief True when the mouse wheel was rolled up. Multiple scroll events per frame are lost.
   */
  bool wheel_up;
  /***************************************************************************
      @brief True when the mouse wheel was rolled down. Multiple scroll events per frame are lost.
   */
  bool wheel_down;
} TCOD_mouse_t;
/***************************************************************************
    @brief Info needed to convert between mouse pixel and tile coordinates.
    Internal use only.

    @code{.cpp}
    double pixel_x, pixel_y, tile_x, tile_y;
    TCOD_MouseTransform transform;
    // Convert pixel coordinates to tile coordinates.
    tile_x = (pixel_x - transform.offset_x) * transform.scale_x;
    tile_y = (pixel_y - transform.offset_y) * transform.scale_y;
    // Convert tile coordinates to pixel coordinates.
    pixel_x = tile_x / transform.scale_x + transform.offset_x;
    pixel_y = tile_y / transform.scale_y + transform.offset_y;
    @endcode

    @versionadded{1.24}
 */
typedef struct TCOD_MouseTransform {
  double offset_x;
  double offset_y;
  double scale_x;
  double scale_y;
} TCOD_MouseTransform;
#endif  // TCOD_MOUSE_TYPES_H_
