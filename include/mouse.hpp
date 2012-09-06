/*
* libtcod 1.5.1
* Copyright (c) 2008,2009,2010,2012 Jice & Mingos
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice or Mingos may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY JICE AND MINGOS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE OR MINGOS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _TCOD_MOUSE_HPP
#define _TCOD_MOUSE_HPP

#include "mouse_types.h"

class TCODLIB_API TCODMouse {
public :
	/**
	@PageName mouse
	@PageTitle Mouse support
	@PageCategory Base toolkits
	@FuncTitle Display and hide the mouse cursor
	@FuncDesc By default, the mouse cursor in visible in windowed mode, hidden in fullscreen mode. You can change it with:
	@Cpp static void TCODMouse::showCursor (bool visible)
	@C void TCOD_mouse_show_cursor (bool visible)
	@Py mouse_show_cursor (visible)
	@C# void TCODMouse::showCursor(bool visible)
	@Param visible	If true, this function turns the mouse cursor on. Else it turns the mouse cursor off.
	*/
	static void showCursor(bool visible);

	/**
	@PageName mouse
	@FuncTitle Getting the cursor status
	@FuncDesc You can get the current cursor status (hidden or visible) with:
	@Cpp static bool TCODMouse::isCursorVisible (void)
	@C bool TCOD_mouse_is_cursor_visible (void)
	@Py mouse_is_cursor_visible ()
	@C# bool TCODMouse::isCursorVisible()
	*/
	static bool isCursorVisible();

	/**
	@PageName mouse
	@FuncTitle Setting the mouse cursor's position
	@FuncDesc You can set the cursor position (in pixel coordinates, where [0,0] is the window's top left corner) with:
	@Cpp static void TCODMouse::move (int x, int y)
	@C void TCOD_mouse_move (int x, int y)
	@Py mouse_move (x, y)
	@C# void TCODMouse::moveMouse(int x, int y)
	@Param x,y	New coordinates of the mouse cursor in pixels.
	*/
	static void move(int x, int y);

	/**
	@PageName mouse
	@FuncTitle Getting the mouse status
	@FuncDesc You can read the current mouse status with this function. Note that this function does not update the mouse status. You have to call either TCODSystem::checkForEvent or TCODSystem::waitForEvent for the mouse status to be updated.
<div class="code"><pre>typedef struct {
  int x,y;
  int dx,dy;
  int cx,cy;
  int dcx,dcy;
  bool lbutton; 
  bool rbutton;
  bool mbutton;
  bool lbutton_pressed; 
  bool rbutton_pressed; 
  bool mbutton_pressed; 
  bool wheel_up;
  bool wheel_down;
} TCOD_mouse_t;
</pre></div>
	@Cpp static TCOD_mouse_t TCODMouse::getStatus (void)
	@C TCOD_mouse_t TCOD_mouse_get_status (void)
	@Py mouse_get_status ()
	@C# TCODMouseData TCODMouse::getStatus()
	@Param x, y	Absolute position of the mouse cursor in pixels relative to the window top-left corner.
	@Param dx, dy	Movement of the mouse cursor since the last call in pixels.
	@Param cx, cy	Coordinates of the console cell under the mouse cursor (pixel coordinates divided by the font size).
	@Param dcx, dcy	Movement of the mouse since the last call in console cells (pixel coordinates divided by the font size).
	@Param lbutton	true if the left button is pressed.
	@Param rbutton	true if the right button is pressed.
	@Param mbutton	true if the middle button (or the wheel) is pressed.
	@Param lbutton_pressed	true if the left button was pressed and released.
	@Param rbutton_pressed	true if the right button was pressed and released.
	@Param mbutton_pressed	true if the middle button was pressed and released.
	@Param wheel_up	true if the wheel was rolled up.
	@Param wheel_down	true if the wheel was rolled down.
	*/
	static TCOD_mouse_t getStatus();
};

#endif
