/*
* libtcod 1.4.0
* Copyright (c) 2008 J.C.Wilk
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
#include "libtcod.hpp"

void TCODSystem::saveScreenshot(const char *filename) {
	TCOD_sys_save_screenshot(filename);
}

void TCODSystem::sleepMilli(uint32 milliseconds) {
	TCOD_sys_sleep_milli(milliseconds);
}

uint32 TCODSystem::getElapsedMilli() {
	return TCOD_sys_elapsed_milli();
}

float TCODSystem::getElapsedSeconds() {
	return TCOD_sys_elapsed_seconds();
}

void TCODSystem::forceFullscreenResolution(int width, int height) {
	TCOD_sys_force_fullscreen_resolution(width,height);
}

void TCODSystem::setFps(int val) {
	TCOD_sys_set_fps(val);
}

int TCODSystem::getFps() {
	return TCOD_sys_get_fps();
}

float TCODSystem::getLastFrameLength() {
	return TCOD_sys_get_last_frame_length();
}

void TCODSystem::getCurrentResolution(int *w, int *h) {
	TCOD_sys_get_current_resolution(w,h);
}
