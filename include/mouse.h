/*
* libtcod 1.4.2
* Copyright (c) 2008,2009 Jice
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Jice ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Jice BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _TCOD_MOUSE_H
#define _TCOD_MOUSE_H

/* mouse data */
typedef struct {
  int x,y; /* absolute position */
  int dx,dy; /* movement since last update in pixels */
  int cx,cy; /* cell coordinates in the root console */
  int dcx,dcy; /* movement since last update in console cells */
  unsigned lbutton :1; /* left button status */
  unsigned rbutton :1; /* right button status */
  unsigned mbutton :1; /* middle button status */
  unsigned lbutton_pressed :1; /* left button pressed event */ 
  unsigned rbutton_pressed :1; /* right button pressed event */ 
  unsigned mbutton_pressed :1; /* middle button pressed event */ 
  unsigned wheel_up :1; /* wheel up event */
  unsigned wheel_down :1; /* wheel down event */
} TCOD_mouse_t;

/* mouse support */
TCODLIB_API TCOD_mouse_t TCOD_mouse_get_status();
TCODLIB_API void TCOD_mouse_show_cursor(bool visible);
TCODLIB_API bool TCOD_mouse_is_cursor_visible();
TCODLIB_API void TCOD_mouse_move(int x, int y);

#endif
