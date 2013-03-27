/*
* libtcod 1.6.0
* Copyright (c) 2008,2009,2010,2012,2013 Jice & Mingos
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

#ifndef _TCOD_SYS_HPP
#define _TCOD_SYS_HPP

/**
	@PageName system
	@PageCategory Core
	@PageTitle System layer
	@PageDesc This toolkit contains some system specific miscellaneous utilities. Use them is you want your code to be easily portable.
 */

class TCODLIB_API ITCODSDLRenderer {
public :
	virtual ~ITCODSDLRenderer() {}
	virtual void render(void *sdlSurface) = 0;
};

class TCODLIB_API TCODSystem {
public :
	/**
	@PageName system_time
	@PageFather system
	@PageTitle High precision time functions
	@PageDesc These are functions specifically aimed at real time game development.
	@FuncTitle Limit the frames per second
	@FuncDesc The setFps function allows you to limit the number of frames per second.
		If a frame is rendered faster than expected, the TCOD_console_flush function will wait so that the frame rate never exceed this value.
		You can call this function during your game initialization.
		You can dynamically change the frame rate. Just call this function once again.
		<b>You should always limit the frame rate, except during benchmarks, else your game will use 100% of the CPU power</b>
	@Cpp static void TCODSystem::setFps(int val)
	@C void TCOD_sys_set_fps(int val)
	@Py sys_set_fps(val)
	@C# static void TCODSystem::setFps(int val)
	@Lua tcod.system.setFps(val)
	@Param val Maximum number of frames per second. 0 means unlimited frame rate.
	*/
	static void setFps(int val);

	/**
	@PageName system_time
	@FuncTitle Get the number of frames rendered during the last second
	@FuncDesc The value returned by this function is updated every second.
	@Cpp static int TCODSystem::getFps()
	@C int TCOD_sys_get_fps()
	@Py sys_get_fps()
	@C# static int TCODSystem::getFps()
	@Lua tcod.system.getFps()
	*/
	static int getFps();

	/**
	@PageName system_time
	@FuncTitle Get the duration of the last frame
	@FuncDesc This function returns the length in seconds of the last rendered frame.
		You can use this value to update every time dependent object in the world.
	@Cpp static float TCODSystem::getLastFrameLength()
	@C float TCOD_sys_get_last_frame_length()
	@Py sys_get_last_frame_length()
	@C# static float TCODSystem::getLastFrameLength()
	@Lua tcod.system.getLastFrameLength()
	@CppEx
		// moving an objet at 5 console cells per second
		float x=0,y=0; // object coordinates
		x += 5 * TCODSystem::getLastFrameLength();
		TCODConsole::root->putChar((int)(x),(int)(y),'X');
	@CEx
		float x=0,y=0;
		x += 5 * TCOD_sys_get_last_frame_length();
		TCOD_console_put_char(NULL,(int)(x),(int)(y),'X');
	@PyEx
		x=0.0
		y=0.0
		x += 5 * libtcod.sys_get_last_frame_length()
		libtcod.console_put_char(0,int(x),int(y),'X')
	@LuaEx
		-- moving an objet at 5 console cells per second
		x=0
		y=0 -- object coordinates
		x = x + 5 * tcod.system.getLastFrameLength()
		libtcod.TCODConsole_root:putChar(x,y,'X')
	*/
	static float getLastFrameLength();

	/**
	@PageName system_time
	@FuncTitle Pause the program
	@FuncDesc Use this function to stop the program execution for a specified number of milliseconds.
	@Cpp static void TCODSystem::sleepMilli(uint32 val)
	@C void TCOD_sys_sleep_milli(uint32 val)
	@Py sys_sleep_milli(val)
	@C# static void TCODSystem::sleepMilli(uint val)
	@Lua tcod.system.sleepMilli(val)
	@Param val number of milliseconds before the function returns
	*/
	static void sleepMilli(uint32 val);

	/**
	@PageName system_time
	@FuncTitle Get global timer in milliseconds
	@FuncDesc This function returns the number of milliseconds since the program has started.
	@Cpp static uint32 TCODSystem::getElapsedMilli()
	@C uint32 TCOD_sys_elapsed_milli()
	@Py sys_elapsed_milli()
	@C# static uint TCODSystem::getElapsedMilli()
	@Lua tcod.system.getElapsedMilli()
	*/
	static uint32 getElapsedMilli();

	/**
	@PageName system_time
	@FuncTitle Get global timer in seconds
	@FuncDesc This function returns the number of seconds since the program has started.
	@Cpp static float TCODSystem::getElapsedSeconds()
	@C float TCOD_sys_elapsed_seconds()
	@Py sys_elapsed_seconds()
	@C# static float TCODSystem::getElapsedSeconds()
	@Lua tcod.system.getElapsedSeconds()
	*/
	static float getElapsedSeconds();

	/**
	@PageName console_blocking_input
	@FuncTitle Waiting for any event (mouse or keyboard)
	@FuncDesc This function waits for an event from the user. The eventMask shows what events we're waiting for.
		The return value indicate what event was actually triggered. Values in key and mouse structures are updated accordingly.
		If flush is false, the function waits only if there are no pending events, else it returns the first event in the buffer.
	@Cpp typedef enum {
		TCOD_EVENT_NONE=0,
		TCOD_EVENT_KEY_PRESS=1,
		TCOD_EVENT_KEY_RELEASE=2,
		TCOD_EVENT_KEY=TCOD_EVENT_KEY_PRESS|TCOD_EVENT_KEY_RELEASE,
		TCOD_EVENT_MOUSE_MOVE=4,
		TCOD_EVENT_MOUSE_PRESS=8,
		TCOD_EVENT_MOUSE_RELEASE=16,
		TCOD_EVENT_MOUSE=TCOD_EVENT_MOUSE_MOVE|TCOD_EVENT_MOUSE_PRESS|TCOD_EVENT_MOUSE_RELEASE,
		TCOD_EVENT_ANY=TCOD_EVENT_KEY|TCOD_EVENT_MOUSE,		
	} TCOD_event_t; 
	static TCOD_event_t TCODSystem::waitForEvent(int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse, bool flush)
	@C TCOD_event_t TCOD_sys_wait_for_event(int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse, bool flush)
	@Py sys_wait_for_event(eventMask,key,mouse,flush)
	@Param eventMask event types to wait for (other types are discarded)
	@Param key updated in case of a key event. Can be null if eventMask contains no key event type
	@Param mouse updated in case of a mouse event. Can be null if eventMask contains no mouse event type
	@Param flush if true, all pending events are flushed from the buffer. Else, return the first available event
	@CppEx
		TCOD_key_t key;
		TCOD_mouse_t mouse;
		TCOD_event_t ev = TCODSystem::waitForEvent(TCOD_EVENT_ANY,&key,&mouse,true);
		if ( ev == TCOD_EVENT_KEY_PRESS && key.c == 'i' ) { ... open inventory ... }
	@CEx
		TCOD_key_t key;
		TCOD_mouse_t mouse;
		TCOD_event_t ev = TCOD_sys_wait_for_event(TCOD_EVENT_ANY,&key,&mouse,true);
		if ( ev == TCOD_EVENT_KEY_PRESS && key.c == 'i' ) { ... open inventory ... }	
	*/
	static TCOD_event_t waitForEvent(int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse, bool flush);

	/**
	@PageName console_non_blocking_input
	@FuncTitle Checking for any event (mouse or keyboard)
	@FuncDesc This function checks if an event from the user is in the buffer. The eventMask shows what events we're waiting for.
		The return value indicate what event was actually found. Values in key and mouse structures are updated accordingly.
	@Cpp typedef enum {
		TCOD_EVENT_KEY_PRESS=1,
		TCOD_EVENT_KEY_RELEASE=2,
		TCOD_EVENT_KEY=TCOD_EVENT_KEY_PRESS|TCOD_EVENT_KEY_RELEASE,
		TCOD_EVENT_MOUSE_MOVE=4,
		TCOD_EVENT_MOUSE_PRESS=8,
		TCOD_EVENT_MOUSE_RELEASE=16,
		TCOD_EVENT_MOUSE=TCOD_EVENT_MOUSE_MOVE|TCOD_EVENT_MOUSE_PRESS|TCOD_EVENT_MOUSE_RELEASE,
		TCOD_EVENT_ANY=TCOD_EVENT_KEY|TCOD_EVENT_MOUSE,		
	} TCOD_event_t; 
	static TCOD_event_t TCODSystem::checkForEvent(int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse)
	@C TCOD_event_t TCOD_sys_check_for_event(int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse)
	@Py sys_check_for_event(eventMask,key,mouse)
	@Param eventMask event types to wait for (other types are discarded)
	@Param key updated in case of a key event. Can be null if eventMask contains no key event type
	@Param mouse updated in case of a mouse event. Can be null if eventMask contains no mouse event type
	@CppEx
		TCOD_key_t key;
		TCOD_mouse_t mouse;
		TCOD_event_t ev = TCODSystem::checkForEvent(TCOD_EVENT_ANY,&key,&mouse);
		if ( ev == TCOD_EVENT_KEY_PRESS && key.c == 'i' ) { ... open inventory ... }
	@CEx
		TCOD_key_t key;
		TCOD_mouse_t mouse;
		TCOD_event_t ev = TCOD_sys_check_for_event(TCOD_EVENT_ANY,&key,&mouse);
		if ( ev == TCOD_EVENT_KEY_PRESS && key.c == 'i' ) { ... open inventory ... }	
	*/
	static TCOD_event_t checkForEvent(int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse);
	/**
	@PageName system_screenshots
	@PageFather system
	@PageTitle Easy screenshots
	@FuncDesc This function allows you to save the current game screen in a png file, or possibly a bmp file if you provide a filename ending with .bmp.
	@Cpp static void TCODSystem::saveScreenshot(const char *filename)
	@C void TCOD_sys_save_screenshot(const char *filename)
	@Py sys_save_screenshot(filename)
	@C# static void TCODSystem::saveScreenshot(string filename);
	@Lua tcod.system.saveScreenshot(filename)
	@Param filename Name of the file. If NULL, a filename is automatically generated with the form "./screenshotNNN.png", NNN being the first free number (if a file named screenshot000.png already exist, screenshot001.png will be used, and so on...).
	*/
	static void saveScreenshot(const char *filename);

	/**
	@PageName system_filesystem
	@PageFather system
	@PageTitle Filesystem utilities
	@PageDesc Those are a few function that cannot be easily implemented in a portable way in C/C++. They have no python wrapper since python provides its own builtin functions. All those functions return false if an error occured.
	@FuncTitle Create a directory
	@Cpp static bool TCODSystem::createDirectory(const char *path)
	@C bool TCOD_sys_create_directory(const char *path)
	@Param path Directory path. The immediate father directory (<path>/..) must exist and be writable.
	*/
	static bool createDirectory(const char *path);

	/**
	@PageName system_filesystem
	@FuncTitle Delete an empty directory
	@Cpp static bool TCODSystem::deleteDirectory(const char *path)
	@C bool TCOD_sys_delete_directory(const char *path)
	@Param path directory path. This directory must exist, be writable and empty
	*/
	static bool deleteDirectory(const char *path);

	/**
	@PageName system_filesystem
	@FuncTitle Delete a file
	@Cpp static bool TCODSystem::deleteFile(const char *path)
	@C bool TCOD_sys_delete_file(const char *path)
	@Param path File path. This file must exist and be writable.
	*/
	static bool deleteFile(const char *path);

	/**
	@PageName system_filesystem
	@FuncTitle Check if a path is a directory
	@Cpp static bool TCODSystem::isDirectory(const char *path)
	@C bool TCOD_sys_is_directory(const char *path)
	@Param path a path to check
	*/
	static bool isDirectory(const char *path);

	/**
	@PageName system_filesystem
	@FuncTitle List files in a directory
	@FuncDesc To get the list of entries in a directory (including sub-directories, except . and ..).
		The returned list is allocated by the function and must be deleted by you. All the const char * inside must be also freed with TCODList::clearAndDelete.
	@Cpp static TCODList TCODSystem::getDirectoryContent(const char *path, const char *pattern)
	@C TCOD_list_t TCOD_sys_get_directory_content(const char *path)
	@Param path a directory
	@Param pattern If NULL or empty, returns all directory entries. Else returns only entries matching the pattern. The pattern is NOT a regular expression. It can only handle one '*' wildcard. Examples : *.png, saveGame*, font*.png
	*/
	static TCOD_list_t getDirectoryContent(const char *path, const char *pattern);

	/**
	@PageName system_filesystem
	@FuncTitle Check if a given file exists
	@FuncDesc In order to check whether a given file exists in the filesystem. Useful for detecting errors caused by missing files.
	@Cpp static bool TCODSystem::fileExists(const char *filename, ...)
	@C bool TCOD_sys_file_exists(const char * filename, ...)
	@Param filename the file name, using printf-like formatting
	@Param ... optional arguments for filename formatting
	@CppEx
		if (!TCODSystem::fileExists("myfile.%s","txt")) {
		    fprintf(stderr,"no such file!");
		}
	@CEx
		if (!TCOD_sys_file_exists("myfile.%s","txt")) {
		    fprintf(stderr,"no such file!");
		}
	*/
	static bool fileExists(const char * filename, ...);
	/**
	@PageName system_filesystem
	@FuncTitle Read the content of a file into memory
	@FuncDesc This is a portable function to read the content of a file from disk or from the application apk (android).
		buf must be freed with free(buf).
	@Cpp static bool TCODSystem::readFile(const char *filename, unsigned char **buf, uint32 *size)
	@C bool TCOD_sys_read_file(const char *filename, unsigned char **buf, uint32 *size)
	@Param filename the file name
	@Param buf a buffer to be allocated and filled with the file content
	@Param size the size of the allocated buffer.
	@CppEx
		unsigned char *buf;
		uint32 size;
		if (TCODSystem::readFile("myfile.dat",&buf,&size)) {
		    // do something with buf
		    free(buf);
		}
	@CEx
		if (TCOD_sys_read_file("myfile.dat",&buf,&size)) {
		    // do something with buf
		    free(buf);
		}
	*/	
	static bool readFile(const char *filename, unsigned char **buf, size_t *size);
	/**
	@PageName system_filesystem
	@FuncTitle Write the content of a memory buffer to a file
	@FuncDesc This is a portable function to write some data to a file.
	@Cpp static bool TCODSystem::writeFile(const char *filename, unsigned char *buf, uint32 size)
	@C bool TCOD_sys_write_file(const char *filename, unsigned char *buf, uint32 size)
	@Param filename the file name
	@Param buf a buffer containing the data to write
	@Param size the number of bytes to write.
	@CppEx
		TCODSystem::writeFile("myfile.dat",buf,size));
	@CEx
		TCOD_sys_write_file("myfile.dat",buf,size));
	*/		
	static bool writeFile(const char *filename, unsigned char *buf, uint32 size);
	/**
	@PageName system_sdlcbk
	@PageFather system
	@PageTitle Draw custom graphics on top of the root console
	@PageDesc You can register a callback that will be called after the libtcod rendering phase, but before the screen buffer is swapped. This callback receives the screen SDL_Surface reference.
		This makes it possible to use any SDL drawing functions (including openGL) on top of the libtcod console.
	@FuncTitle Render custom graphics
	@FuncDesc To disable the custom renderer, call the same method with a NULL parameter.
		Note that to keep libtcod from requiring the SDL headers, the callback parameter is a void pointer. You have to include SDL headers and cast it to SDL_Surface in your code.
	@Cpp
		class TCODLIB_API ITCODSDLRenderer {
		public :
			virtual void render(void *sdlSurface) = 0;
		};
		static void TCODSystem::registerSDLRenderer(ITCODSDLRenderer *callback);
	@C
		typedef void (*SDL_renderer_t) (void *sdl_surface);
		void TCOD_sys_register_SDL_renderer(SDL_renderer_t callback)
	@Py
		def renderer ( sdl_surface ) : ...
		TCOD_sys_register_SDL_renderer( callback )
	@Param callback The renderer to call before swapping the screen buffer. If NULL, custom rendering is disabled
	@CppEx
		class MyRenderer : public ITCODSDLRenderer {
		public :
			void render(void *sdlSurface) {
				SDL_Surface *s = (SDL_Surface *)sdlSurface;
				... draw something on s
			}
		};
		TCODSystem::registerSDLRenderer(new MyRenderer());
	@CEx
		void my_renderer( void *sdl_surface ) {
			SDL_Surface *s = (SDL_Surface *)sdl_surface;
			... draw something on s
		}
		TCOD_sys_register_SDL_renderer(my_renderer);
	@Py
		def my_renderer(sdl_surface) :
			... draw something on sdl_surface using pygame
		libtcod.sys_register_SDL_renderer(my_renderer)
	*/
	static void registerSDLRenderer(ITCODSDLRenderer *renderer);

	/**
	@PageName system_sdlcbk
	@FuncTitle Managing screen redraw
	@FuncDesc libtcod is not aware of the part of the screen your SDL renderer has updated. If no change occured in the console, it won't redraw them except if you tell him to do so with this function
	@Cpp void TCODConsole::setDirty(int x, int y, int w, int h)
	@C void TCOD_console_set_dirty(int x, int y, int w, int h)
	@Py TCOD_console_set_dirty(x, y, w, h)
	@Param x,y,w,h Part of the root console you want to redraw even if nothing has changed in the console back/fore/char.
	*/

	/**
	@PageName system_misc
	@PageFather system
	@PageTitle Miscellaneous utilities
	@FuncTitle Using a custom resolution for the fullscreen mode
	@FuncDesc This function allows you to force the use of a specific resolution in fullscreen mode.
		The default resolution depends on the root console size and the font character size.
	@Cpp static void TCODSystem::forceFullscreenResolution(int width, int height)
	@C void TCOD_sys_force_fullscreen_resolution(int width, int height)
	@Py sys_force_fullscreen_resolution(width, height)
	@C# static void TCODSystem::forceFullscreenResolution(int width, int height);
	@Lua tcod.system.forceFullscreenResolution(width,height)
	@Param width,height Resolution to use when switching to fullscreen.
		Will use the smallest available resolution so that :
		resolution width >= width and resolution width >= root console width * font char width
		resolution width >= height and resolution height >= root console height * font char height
	@CppEx
		TCODSystem::forceFullscreenResolution(800,600); // use 800x600 in fullscreen instead of 640x400
		TCODConsole::initRoot(80,50,"",true); // 80x50 console with 8x8 char => 640x400 default resolution
	@CEx
		TCOD_sys_force_fullscreen_resolution(800,600);
		TCOD_console_init_root(80,50,"",true);
	@PyEx
		libtcod.sys_force_fullscreen_resolution(800,600)
		libtcod.console_init_root(80,50,"",True)
	@LuaEx
		tcod.system.forceFullscreenResolution(800,600) -- use 800x600 in fullscreen instead of 640x400
		tcod.console.initRoot(80,50,"",true) -- 80x50 console with 8x8 char => 640x400 default resolution
	*/
	static void forceFullscreenResolution(int width, int height);

	/**
	@PageName system_misc
	@FuncTitle Get current resolution
	@FuncDesc You can get the current screen resolution with getCurrentResolution. You can use it for example to get the desktop resolution before initializing the root console.
	@Cpp static void TCODSystem::getCurrentResolution(int *width, int *height)
	@C void TCOD_sys_get_current_resolution(int *width, int *height)
	@Py sys_get_current_resolution() # returns w,h
	@C# static void TCODSystem::getCurrentResolution(out int w, out int h);
	@Param width,height contains current resolution when the function returns
	*/
	static void getCurrentResolution(int *w, int *h);
	/**
	@PageName system_misc
	@FuncTitle Get fullscreen offset
	@FuncDesc If the fullscreen resolution does not matches the console size in pixels, black borders are added. This function returns the position in pixels of the console top left corner in the screen.
	@Cpp static void TCODSystem::getFullscreenOffsets(int *offx, int *offy)
	@C void TCOD_sys_get_fullscreen_offsets(int *offx, int *offy)
	@C# static void TCODSystem::getFullscreenOffsets(out int offx, out int offy);
	@Param offx,offy contains the position of the console on the screen when using fullscreen mode.
	*/
	static void getFullscreenOffsets(int *offx, int *offy);
	/**
	@PageName system_misc
	@FuncTitle Get the font size
	@FuncDesc You can get the size of the characters in the font
	@Cpp static void TCODSystem::getCharSize(int *width, int *height)
	@C void TCOD_sys_get_char_size(int *width, int *height)
	@Py sys_get_char_size() # returns w,h
	@C# static void TCODSystem::getCharSize(out int w, out int h);
	@Param width,height contains a character size when the function returns
	*/
	static void getCharSize(int *w, int *h);

	/**
	@PageName system_misc
	@FuncTitle Dynamically updating the font bitmap
	@FuncDesc You can dynamically change the bitmap of a character in the font. All cells using this ascii code will be updated at next flush call.
	@Cpp static void TCODSystem::updateChar(int asciiCode, int fontx, int fonty,const TCODImage *img,int x,int y)
	@C void TCOD_sys_update_char(int asciiCode, int fontx, int fonty, TCOD_image_t img, int x, int y)
	@Py sys_update_char(asciiCode,fontx,fonty,img,x,y)
	@Param asciiCode ascii code corresponding to the character to update
	@Param fontx,fonty coordinate of the character in the bitmap font (in characters, not pixels)
	@Param img image containing the new character bitmap
	@Param x,y position in pixels of the top-left corner of the character in the image
	*/
	static void updateChar(int asciiCode, int fontx, int fonty,const TCODImage *img,int x,int y);

	/**
	@PageName system_misc
	@FuncTitle Dynamically change libtcod's internal renderer
	@FuncDesc As of 1.5.1, libtcod contains 3 different renderers :
		* SDL : historic libtcod renderer. Should work and be pretty fast everywhere
		* OpenGL : requires OpenGL compatible video card. Might be much faster or much slower than SDL, depending on the drivers
		* GLSDL : requires OpenGL 1.4 compatible video card with GL_ARB_shader_objects extension. Blazing fast if you have the proper hardware and drivers.
		This function switches the current renderer dynamically.
	@Cpp static void TCODSystem::setRenderer(TCOD_renderer_t renderer)
	@C void TCOD_sys_set_renderer(TCOD_renderer_t renderer)
	@Py sys_set_renderer(renderer)
	@C# static void TCODSystem::setRenderer(TCODRendererType renderer);
	@Param renderer Either TCOD_RENDERER_GLSL, TCOD_RENDERER_OPENGL or TCOD_RENDERER_SDL
	*/
	static void setRenderer(TCOD_renderer_t renderer);

	/**
	@PageName system_misc
	@FuncTitle Get the current internal renderer
	@Cpp static TCOD_renderer_t TCODSystem::getRenderer()
	@C TCOD_renderer_t TCOD_sys_get_renderer()
	@Py sys_get_renderer()
	@C# static TCODRendererType TCODSystem::getRenderer();
	*/
	static TCOD_renderer_t getRenderer();

	/**
	@PageName system_clipboard
	@PageTitle Clipboard integration
	@PageDesc With these functions, you can copy data in your OS' clipboard from the game or retrieve data from the clipboard.
	@PageFather system
	@FuncTitle Copy data to the clipboard
	@Cpp static void TCODSystem::setClipboard(const char *value)
	@C void TCOD_sys_clipboard_set(const char *value)
	@Param value Text to copy in the clipboard
	*/
	static void setClipboard(const char *value);

	/**
	@PageName system_clipboard
	@FuncTitle Paste data from the clipboard
	@Cpp static char *TCODSystem::getClipboard()
	@C char *TCOD_sys_clipboard_get()
	*/
	static char *getClipboard();

	// thread stuff
	static int getNumCores();
	static TCOD_thread_t newThread(int (*func)(void *), void *data);
	static void deleteThread(TCOD_thread_t th);
	static void waitThread(TCOD_thread_t th);
	// mutex
	static TCOD_mutex_t newMutex();
	static void mutexIn(TCOD_mutex_t mut);
	static void mutexOut(TCOD_mutex_t mut);
	static void deleteMutex(TCOD_mutex_t mut);
	// semaphore
	static TCOD_semaphore_t newSemaphore(int initVal);
	static void lockSemaphore(TCOD_semaphore_t sem);
	static void unlockSemaphore(TCOD_semaphore_t sem);
	static void deleteSemaphore( TCOD_semaphore_t sem);
	// condition
	static TCOD_cond_t newCondition();
	static void signalCondition(TCOD_cond_t sem);
	static void broadcastCondition(TCOD_cond_t sem);
	static void waitCondition(TCOD_cond_t sem, TCOD_mutex_t mut);
	static void deleteCondition( TCOD_cond_t sem);
};

#endif
