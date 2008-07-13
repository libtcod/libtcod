/*
* libtcod 1.3.2
* Copyright (c) 2007,2008 J.C.Wilk
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of J.C.Wilk may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY J.C.WILK ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL J.C.WILK BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <SDL/SDL.h>
#include "libtcod.h"
#include "libtcod_int.h"

TCOD_mouse_t TCOD_mouse_get_status() {
  TCOD_mouse_t ms;
  int charWidth, charHeight;
  static bool lbut=false;
  static bool rbut=false;
  static bool mbut=false;
  Uint8 buttons;
  SDL_PumpEvents();

  buttons = SDL_GetMouseState(&ms.x,&ms.y);
  SDL_GetRelativeMouseState(&ms.dx,&ms.dy);
  ms.lbutton = (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) ? 1 : 0;
  ms.lbutton_pressed=false;
  if (ms.lbutton) lbut=true;
  else if( lbut ) {
  	lbut=false;
  	ms.lbutton_pressed=true;
  }
  ms.rbutton = (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) ? 1 : 0;
  ms.rbutton_pressed=false;
  if (ms.rbutton) rbut=true;
  else if( rbut ) {
  	rbut=false;
  	ms.rbutton_pressed=true;
  }
  ms.mbutton = (buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) ? 1 : 0;
  ms.mbutton_pressed=false;
  if (ms.mbutton) mbut=true;
  else if( mbut ) {
  	mbut=false;
  	ms.mbutton_pressed=true;
  }
  ms.wheel_up = (buttons & SDL_BUTTON(SDL_BUTTON_WHEELUP)) ? 1 : 0;
  ms.wheel_down = (buttons & SDL_BUTTON(SDL_BUTTON_WHEELDOWN)) ? 1 : 0;
  TCOD_sys_get_char_size(&charWidth,&charHeight);
  ms.cx = ms.x / charWidth;
  ms.cy = ms.y / charHeight;
  ms.dcx = ms.dx / charWidth;
  ms.dcy = ms.dy / charHeight;
  return ms;
}

void TCOD_mouse_show_cursor(bool visible) {
  SDL_ShowCursor(visible ? 1 : 0);
}

bool TCOD_mouse_is_cursor_visible() {
  return SDL_ShowCursor(-1) ? true : false;
}

void TCOD_mouse_move(int x, int y) {
  SDL_WarpMouse((Uint16)x,(Uint16)y);
}

