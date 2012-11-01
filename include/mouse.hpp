/*
* libtcod 1.5.2
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

	/* deprecated as of 1.5.1 */	
	static TCOD_mouse_t getStatus();
};

#endif
