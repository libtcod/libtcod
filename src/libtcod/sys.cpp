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
#include "sys.hpp"

#include <cstdio>
#include <cstdarg>

#include "libtcod_int.h"
#include "engine/display.h"

#ifdef TCOD_OSUTIL_SUPPORT
void TCODSystem::sleepMilli(uint32_t milliseconds) {
	TCOD_sys_sleep_milli(milliseconds);
}

uint32_t TCODSystem::getElapsedMilli() {
	return TCOD_sys_elapsed_milli();
}

float TCODSystem::getElapsedSeconds() {
	return TCOD_sys_elapsed_seconds();
}
#endif

#ifndef TCOD_BARE
void TCODSystem::saveScreenshot(const char *filename) {
	TCOD_sys_save_screenshot(filename);
}

void TCODSystem::forceFullscreenResolution(int width, int height) {
	TCOD_sys_force_fullscreen_resolution(width, height);
}

void TCODSystem::setRenderer(TCOD_renderer_t renderer) {
  if (renderer == TCOD_ctx.renderer) { return; }
  tcod::console::init_root(
      TCOD_ctx.root->w,
      TCOD_ctx.root->h,
      TCOD_ctx.window_title,
      TCOD_console_is_fullscreen(),
      renderer
  );
}
TCOD_event_t TCODSystem::waitForEvent(int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse, bool flush) {
	return TCOD_sys_wait_for_event(eventMask,key,mouse,flush);
}

TCOD_event_t TCODSystem::checkForEvent(int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse) {
	return TCOD_sys_check_for_event(eventMask,key,mouse);
}

TCOD_renderer_t TCODSystem::getRenderer() {
	return TCOD_sys_get_renderer();
}
#endif

#ifdef TCOD_OSUTIL_SUPPORT
void TCODSystem::setFps(int val) {
	TCOD_sys_set_fps(val);
}

int TCODSystem::getFps() {
	return TCOD_sys_get_fps();
}

float TCODSystem::getLastFrameLength() {
	return TCOD_sys_get_last_frame_length();
}
#endif

#ifndef TCOD_BARE
void TCODSystem::getCurrentResolution(int *w, int *h) {
	TCOD_sys_get_current_resolution(w, h);
}

void TCODSystem::getFullscreenOffsets(int *offx, int *offy) {
	TCOD_sys_get_fullscreen_offsets(offx, offy);
}

void TCODSystem::updateChar(int asciiCode, int fontx, int fonty,const TCODImage *img,int x,int y) {
	TCOD_sys_update_char(asciiCode,fontx,fonty,img->data,x,y);
}

void TCODSystem::getCharSize(int *w, int *h) {
	TCOD_sys_get_char_size(w, h);
}
#endif

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

bool TCODSystem::fileExists(const char * filename, ...) {
	FILE * in;
	bool ret = false;
	char f[1024];
	va_list ap;
	va_start(ap,filename);
	vsprintf(f,filename,ap);
	va_end(ap);
	in = fopen(f,"rb");
	if (in != NULL) {
		ret = true;
		fclose(in);
	}
	return ret;
}

bool TCODSystem::readFile(const char *filename, unsigned char **buf, size_t *size) {
	return TCOD_sys_read_file(filename,buf,size) != 0;
}

bool TCODSystem::writeFile(const char *filename, unsigned char *buf, uint32_t size) {
	return TCOD_sys_write_file(filename,buf,size) != 0;
}

#ifndef TCOD_BARE
// clipboard stuff
bool TCODSystem::setClipboard(const char *value) {
	return TCOD_sys_clipboard_set(value) != 0;
}

char *TCODSystem::getClipboard() {
  return TCOD_sys_clipboard_get();
}
#endif

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
static ITCODSDLRenderer *post_renderer=NULL;
extern "C" void TCOD_CRenderer(struct SDL_Surface* sdl_surface) {
  if (post_renderer) { post_renderer->render(sdl_surface); }
}
void TCODSystem::registerSDLRenderer(ITCODSDLRenderer *renderer) {
	::post_renderer = renderer;
#ifndef TCOD_BARE
	TCOD_sys_register_SDL_renderer(TCOD_CRenderer);
#endif
}
