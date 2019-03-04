/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
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
#ifndef _TCOD_MOUSE_HPP
#define _TCOD_MOUSE_HPP

#include "mouse.h"

#ifdef TCOD_CONSOLE_SUPPORT

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
	@FuncTitle Get the last known mouse cursor position
	@FuncDesc This function is only valid, and only returns updated values, after you have called event-related functions.  Whether to check for events, or wait for events.  It does not provide the actual mouse position at the time the call is made.
	@Cpp static TCOD_mouse_t TCODMouse::getStatus ()
	@C void TCOD_mouse_get_status ()
	@Py mouse_get_status ()
	*/
	static TCOD_mouse_t getStatus();
};

#endif /* TCOD_CONSOLE_SUPPORT */

#endif /* _TCOD_MOUSE_HPP */
