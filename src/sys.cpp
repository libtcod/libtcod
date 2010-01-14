/*
* libtcod 1.5.0
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

void TCODSystem::updateChar(int asciiCode, int fontx, int fonty,const TCODImage *img,int x,int y) {
	TCOD_sys_update_char(asciiCode,fontx,fonty,img->data,x,y);
}

// filesystem stuff
bool TCODSystem::createDirectory(const char *path) {
	return TCOD_sys_create_directory(path) != 0;
}

bool TCODSystem::deleteFile(const char *path) {
	return TCOD_sys_delete_file(path) != 0;
}

bool TCODSystem::deleteDirectory(const char *path) {
	return TCOD_sys_delete_directory(path) != 0;
}

bool TCODSystem::isDirectory(const char *path) {
	return TCOD_sys_is_directory(path) != 0;
}

TCOD_list_t TCODSystem::getDirectoryContent(const char *path, const char *pattern) {
	return TCOD_sys_get_directory_content(path,pattern);
}

// thread stuff
int TCODSystem::getNumCores() {
	return TCOD_sys_get_num_cores();
}

TCOD_thread_t TCODSystem::newThread(int (*func)(void *), void *data) {
	return TCOD_thread_new(func,data);
}

void TCODSystem::deleteThread(TCOD_thread_t th) {
	TCOD_thread_delete(th);
}

void TCODSystem::waitThread(TCOD_thread_t th) {
	TCOD_thread_wait(th);
}

// mutex
TCOD_mutex_t TCODSystem::newMutex() {
	return TCOD_mutex_new();
}

void TCODSystem::mutexIn(TCOD_mutex_t mut) {
	TCOD_mutex_in(mut);
}

void TCODSystem::mutexOut(TCOD_mutex_t mut) {
	TCOD_mutex_out(mut);
}

void TCODSystem::deleteMutex(TCOD_mutex_t mut) {
	TCOD_mutex_delete(mut);
}

// semaphore
TCOD_semaphore_t TCODSystem::newSemaphore(int initVal) {
	return TCOD_semaphore_new(initVal);
}

void TCODSystem::lockSemaphore(TCOD_semaphore_t sem) {
	TCOD_semaphore_lock(sem);
}

void TCODSystem::unlockSemaphore(TCOD_semaphore_t sem) {
	TCOD_semaphore_unlock(sem);
}

void TCODSystem::deleteSemaphore( TCOD_semaphore_t sem) {
	TCOD_semaphore_delete(sem);
}

// condition
TCOD_cond_t TCODSystem::newCondition() {
	return TCOD_condition_new();
}

void TCODSystem::signalCondition(TCOD_cond_t cond) {
	TCOD_condition_signal(cond);
}

void TCODSystem::broadcastCondition(TCOD_cond_t cond) {
	TCOD_condition_broadcast(cond);
}

void TCODSystem::waitCondition(TCOD_cond_t cond, TCOD_mutex_t mut) {
	TCOD_condition_wait(cond, mut);
}

void TCODSystem::deleteCondition( TCOD_cond_t cond) {
	TCOD_condition_delete(cond);
}

// custom post-renderer
static ITCODSDLRenderer *renderer=NULL;
extern "C" void TCOD_CRenderer(void *sdl_surface) {
	if ( renderer ) renderer->render(sdl_surface);
}
void TCODSystem::registerSDLRenderer(ITCODSDLRenderer *renderer) {
	::renderer = renderer;
	TCOD_sys_register_SDL_renderer(TCOD_CRenderer);
}
