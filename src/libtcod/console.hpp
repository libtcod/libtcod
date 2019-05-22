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
#ifndef _TCOD_CONSOLE_HPP
#define _TCOD_CONSOLE_HPP

#include <string>

#include "console.h"
#include "console/drawing.h"
#include "console/printing.h"
#include "console/rexpaint.h"
#include "engine/display.h"

#ifdef TCOD_CONSOLE_SUPPORT

#include "image.hpp"
#include "color.hpp"

class TCODImage;
/**
	@PageName console
	@PageCategory Core
	@PageTitle Console
	@PageDesc The console emulator handles the rendering of the game screen and the keyboard input.
Classic real time game loop:
	@Cpp
		TCODConsole::initRoot(80,50,"my game",false);
		TCODSystem::setFps(25); // limit framerate to 25 frames per second
		while (!endGame && !TCODConsole::isWindowClosed()) {
			TCOD_key_t key;
			TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL);
			updateWorld (key, TCODSystem::getLastFrameLength());
			// updateWorld(TCOD_key_t key, float elapsed) (using key if key.vk != TCODK_NONE)
			// use elapsed to scale any update that is time dependent.
			// ... draw world+GUI on TCODConsole::root
			TCODConsole::flush();
		}
	@Lua
		tcod.console.initRoot(80,50,"my game", false)
		root=libtcod.TCODConsole_root
		tcod.system.setFps(25)
		while not tcod.console.isWindowClosed() do
			-- ... draw on root
			tcod.console.flush()
			key=tcod.console.checkForKeypress()
			-- ... update world, using key and tcod.system.getLastFrameLength
		end

*/
/**
	@PageName console
	@FuncDesc Classic turn by turn game loop:
	@Cpp
		TCODConsole::initRoot(80,50,"my game",false);
		while (!endGame && !TCODConsole::isWindowClosed()) {
			// ... draw on TCODConsole::root
			TCODConsole::flush();
			TCOD_key_t key;
			TCODConsole::waitForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL,true);
			//... update world, using key
		}
*/

class TCODLIB_API TCODConsole {
public :
	/**
	@PageName console_init
	@PageTitle Initializing the console
	@PageFather console
	*/

	static TCODConsole *root;

	/**
	@PageName console_init_root
	@PageTitle Creating the game window
	@PageFather console_init
	@Cpp static void TCODConsole::initRoot (int w, int h, const char * title, bool fullscreen = false, TCOD_renderer_t renderer = TCOD_RENDERER_SDL)
	@C void TCOD_console_init_root (int w, int h, const char * title, bool fullscreen, TCOD_renderer_t renderer)
	@Py console_init_root (w, h, title, fullscreen = False, renderer = RENDERER_SDL)
	@C#
		static void TCODConsole::initRoot(int w, int h, string title)
		static void TCODConsole::initRoot(int w, int h, string title, bool fullscreen)
		static void TCODConsole::initRoot(int w, int h, string title, bool fullscreen, TCODRendererType renderer)
	@Lua
		tcod.console.initRoot(w,h,title) -- fullscreen = false, renderer = SDL
		tcod.console.initRoot(w,h,title,fullscreen) -- renderer = SDL
		tcod.console.initRoot(w,h,title,fullscreen,renderer)
		-- renderers : tcod.GLSL, tcod.OpenGL, tcod.SDL
	@Param w,h size of the console(in characters). The default font in libtcod (./terminal.png) uses 8x8 pixels characters.
		You can change the font by calling TCODConsole::setCustomFont before calling initRoot.
	@Param title title of the window. It's not visible when you are in fullscreen.
		Note 1 : you can dynamically change the window title with TCODConsole::setWindowTitle
	@Param fullscreen whether you start in windowed or fullscreen mode.
		Note 1 : you can dynamically change this mode with TCODConsole::setFullscreen
		Note 2 : you can get current mode with TCODConsole::isFullscreen
	@Param renderer which renderer to use. Possible values are :
    * TCOD_RENDERER_GLSL : works only on video cards with pixel shaders
    * TCOD_RENDERER_OPENGL : works on all video cards supporting OpenGL 1.4
    * TCOD_RENDERER_SDL : should work everywhere!
		Note 1: if you select a renderer that is not supported by the player's machine, libtcod scan the lower renderers until it finds a working one.
		Note 2: on recent video cards, GLSL results in up to 900% increase of framerates in the true color sample compared to SDL renderer.
		Note 3: whatever renderer you use, it can always be overridden by the player through the libtcod.cfg file.
		Note 4: you can dynamically change the renderer after calling initRoot with TCODSystem::setRenderer.
		Note 5: you can get current renderer with TCODSystem::getRenderer. It might be different from the one you set in initRoot in case it's not supported on the player's computer.
	@CppEx TCODConsole::initRoot(80, 50, "The Chronicles Of Doryen v0.1");
	@CEx TCOD_console_init_root(80, 50, "The Chronicles Of Doryen v0.1", false, TCOD_RENDERER_OPENGL);
	@PyEx libtcod.console_init_root(80, 50, 'The Chronicles Of Doryen v0.1')
	@LuaEx tcod.console.initRoot(80,50,"The Chronicles Of Doryen v0.1")
	*/
	static void initRoot(int w, int h, const char * title, bool fullscreen = false, TCOD_renderer_t renderer=TCOD_RENDERER_SDL);

	/**
	@PageName console_set_custom_font
	@PageTitle Using a custom bitmap font
	@PageFather console_init
	@FuncTitle setCustomFont
	@FuncDesc This function allows you to use a bitmap font (png or bmp) with custom character size or layout.
		It should be called before initializing the root console with initRoot.
		Once this function is called, you can define your own custom mappings using mapping functions
		<h5>Different font layouts</h5>
		<table>
		<tr><td>ASCII_INROW</td><td>ASCII_INCOL</td><td>TCOD</td></tr>

		<tr><td><img src='terminal8x8_gs_ro.png' /></td><td><img src='terminal8x8_gs_as.png' /></td><td><img src='terminal8x8_gs_tc.png' /></td></tr>
		</table>
		<ul>
		<li>ascii, in columns : characters 0 to 15 are in the first column. The space character is at coordinates 2,0.</li>
		<li>ascii, in rows : characters 0 to 15 are in the first row. The space character is at coordinates 0,2.</li>
		<li>tcod : special mapping. Not all ascii values are mapped. The space character is at coordinates 0,0.</li>
		</ul>
		<h5>Different font types</h5>
		<table>
		<tr><td>standard<br />(non antialiased)</td><td>antialiased<br />(32 bits PNG)</td><td>antialiased<br />(greyscale)</td></tr>

		<tr><td><img src='terminal.png' /></td><td><img src='terminal8x8_aa_as.png' /></td><td><img src='terminal8x8_gs_as2.png' /></td></tr>
		</table>
		<ul>
		<li>standard : transparency is given by a key color automatically detected by looking at the color of the space character</li>
		<li>32 bits : transparency is given by the png alpha layer. The font color does not matter but it must be desaturated</li>
		<li>greyscale : transparency is given by the pixel value. You can use white characters on black background or black characters on white background. The background color is automatically detected by looking at the color of the space character</li>
		</ul>
		Examples of fonts can be found in libtcod's fonts directory. Check the Readme file there.
	@Cpp static void TCODConsole::setCustomFont(const char *fontFile, int flags=TCOD_FONT_LAYOUT_ASCII_INCOL,int nbCharHoriz=0, int nbCharVertic=0)
	@C void TCOD_console_set_custom_font(const char *fontFile, int flags,int nb_char_horiz, int nb_char_vertic)
	@Py console_set_custom_font(fontFile, flags=FONT_LAYOUT_ASCII_INCOL,nb_char_horiz=0, nb_char_vertic=0)
	@C#
		static void TCODConsole::setCustomFont(string fontFile)
		static void TCODConsole::setCustomFont(string fontFile, int flags)
		static void TCODConsole::setCustomFont(string fontFile, int flags, int nbCharHoriz)
		static void TCODConsole::setCustomFont(string fontFile, int flags, int nbCharHoriz, int nbCharVertic)
	@Lua
		tcod.console.setCustomFont(fontFile)
		tcod.console.setCustomFont(fontFile, flags)
		tcod.console.setCustomFont(fontFile, nbCharHoriz)
		tcod.console.setCustomFont(fontFile, flags, nbCharHoriz, nbCharVertic)
		-- flags : tcod.LayoutAsciiInColumn, tcod.LayoutAsciiInRow, tcod.LayoutTCOD, tcod.Greyscale
	@Param fontFile Name of a .bmp or .png file containing the font.
	@Param flags Used to define the characters layout in the bitmap and the font type :
		TCOD_FONT_LAYOUT_ASCII_INCOL : characters in ASCII order, code 0-15 in the first column
		TCOD_FONT_LAYOUT_ASCII_INROW : characters in ASCII order, code 0-15 in the first row
		TCOD_FONT_LAYOUT_TCOD : simplified layout. See examples below.
		TCOD_FONT_TYPE_GREYSCALE : create an anti-aliased font from a greyscale bitmap
		For Python, remove TCOD _ :
		libtcod.FONT_LAYOUT_ASCII_INCOL
	@Param nbCharHoriz,nbCharVertic Number of characters in the font.
		Should be 16x16 for ASCII layouts, 32x8 for TCOD layout.
		But you can use any other layout.
		If set to 0, there are deduced from the font layout flag.
	@CppEx
		TCODConsole::setCustomFont("standard_8x8_ascii_in_col_font.bmp",TCOD_FONT_LAYOUT_ASCII_INCOL);
		TCODConsole::setCustomFont("32bits_8x8_ascii_in_row_font.png",TCOD_FONT_LAYOUT_ASCII_INROW);
		TCODConsole::setCustomFont("greyscale_8x8_tcod_font.png",TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE);
	@CEx
		TCOD_console_set_custom_font("standard_8x8_ascii_in_col_font.bmp",TCOD_FONT_LAYOUT_ASCII_INCOL,16,16);
		TCOD_console_set_custom_font("32bits_8x8_ascii_in_row_font.png",TCOD_FONT_LAYOUT_ASCII_INROW,32,8);
		TCOD_console_set_custom_font("greyscale_8x8_tcod_font.png",TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE,32,8);
	@PyEx
		libtcod.console_set_custom_font("standard_8x8_ascii_in_col_font.bmp",libtcod.FONT_LAYOUT_ASCII_INCOL)
		libtcod.console_set_custom_font("32bits_8x8_ascii_in_row_font.png",libtcod.FONT_LAYOUT_ASCII_INROW)
		libtcod.console_set_custom_font("greyscale_8x8_tcod_font.png",libtcod.FONT_LAYOUT_TCOD | libtcod.FONT_TYPE_GREYSCALE)
	@LuaEx
		tcod.console.setCustomFont("standard_8x8_ascii_in_col_font.bmp",tcod.LayoutAsciiInColumn);
		tcod.console.setCustomFont("32bits_8x8_ascii_in_row_font.png",tcod.LayoutAsciiInRow);
		tcod.console.setCustomFont("greyscale_8x8_tcod_font.png",tcod.LayoutTCOD + tcod.Greyscale);
	*/
	static void setCustomFont(const char *fontFile, int flags=TCOD_FONT_LAYOUT_ASCII_INCOL,int nbCharHoriz=0, int nbCharVertic=0);

	/**
	@PageName console_map
	@PageTitle Using custom characters mapping
	@PageFather console_init
	@FuncTitle Mapping a single ASCII code to a character
	@PageDesc These functions allow you to map characters in the bitmap font to ASCII codes.
		They should be called after initializing the root console with initRoot.
		You can dynamically change the characters mapping at any time, allowing to use several fonts in the same screen.
	@Cpp static void TCODConsole::mapAsciiCodeToFont(int asciiCode, int fontCharX, int fontCharY)
	@C void TCOD_console_map_ascii_code_to_font(int asciiCode, int fontCharX, int fontCharY)
	@Py console_map_ascii_code_to_font(asciiCode, fontCharX, fontCharY)
	@C# static void TCODConsole::mapAsciiCodeToFont(int asciiCode, int fontCharX, int fontCharY)
	@Lua tcod.console.mapAsciiCodeToFont(asciiCode, fontCharX, fontCharY)
	@Param asciiCode ASCII code to map.
	@Param fontCharX,fontCharY Coordinate of the character in the bitmap font (in characters, not pixels).
	*/
	static void mapAsciiCodeToFont(int asciiCode, int fontCharX, int fontCharY);

	/**
	@PageName console_map
	@FuncTitle Mapping consecutive ASCII codes to consecutive characters
	@Cpp static void TCODConsole::mapAsciiCodesToFont(int firstAsciiCode, int nbCodes, int fontCharX, int fontCharY)
	@C void TCOD_console_map_ascii_codes_to_font(int firstAsciiCode, int nbCodes, int fontCharX, int fontCharY)
	@Py console_map_ascii_codes_to_font(firstAsciiCode, nbCodes, fontCharX, fontCharY)
	@C# static void TCODConsole::mapAsciiCodesToFont(int firstAsciiCode, int nbCodes, int fontCharX, int fontCharY)
	@Lua tcod.console.mapAsciiCodesToFont(firstAsciiCode, nbCodes, fontCharX, fontCharY)
	@Param firstAsciiCode first ASCII code to map
	@Param nbCodes number of consecutive ASCII codes to map
	@Param fontCharX,fontCharY coordinate of the character in the bitmap font (in characters, not pixels) corresponding to the first ASCII code
	*/
	static void mapAsciiCodesToFont(int firstAsciiCode, int nbCodes, int fontCharX, int fontCharY);

	/**
	@PageName console_map
	@FuncTitle Mapping ASCII code from a string to consecutive characters
	@Cpp static void TCODConsole::mapStringToFont(const char *s, int fontCharX, int fontCharY)
	@C void TCOD_console_map_string_to_font(const char *s, int fontCharX, int fontCharY)
	@Py console_map_string_to_font(s, fontCharX, fontCharY)
	@C# static void TCODConsole::mapStringToFont(string s, int fontCharX, int fontCharY)
	@Lua tcod.console.mapStringToFont(s, fontCharX, fontCharY)
	@Param s string containing the ASCII codes to map
	@Param fontCharX,fontCharY coordinate of the character in the bitmap font (in characters, not pixels) corresponding to the first ASCII code in the string
	*/
	static void mapStringToFont(const char *s, int fontCharX, int fontCharY);

	/**
	@PageName console_fullscreen
	@PageTitle Fullscreen mode
	@PageFather console_init
	@FuncTitle Getting the current mode
	@FuncDesc This function returns true if the current mode is fullscreen.
	@Cpp static bool TCODConsole::isFullscreen()
	@C bool TCOD_console_is_fullscreen()
	@Py console_is_fullscreen()
	@C# static bool TCODConsole::isFullscreen()
	@Lua tcod.console.isFullscreen()
	*/
	static bool isFullscreen();
	/**
	@PageName console_fullscreen
	@FuncTitle Switching between windowed and fullscreen modes
	@FuncDesc This function switches the root console to fullscreen or windowed mode.
		Note that there is no predefined key combination to switch to/from fullscreen. You have to do this in your own code.
	@Cpp static void TCODConsole::setFullscreen(bool fullscreen)
	@C void TCOD_console_set_fullscreen(bool fullscreen)
	@Py console_set_fullscreen(fullscreen)
	@C# static void TCODConsole::setFullscreen(bool fullscreen)
	@Lua tcod.console.setFullscreen(fullscreen)
	@Param fullscreen true to switch to fullscreen mode.
		false to switch to windowed mode.
	@CppEx
		TCOD_key_t key;
		TCODConsole::checkForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL);
		if ( key.vk == TCODK_ENTER && key.lalt )
			TCODConsole::setFullscreen(!TCODConsole::isFullscreen());
	@CEx
		TCOD_key_t key;
		TCOD_console_check_for_event(TCOD_EVENT_KEY_PRESS,&key,NULL);
		if ( key.vk == TCODK_ENTER && key.lalt )
			TCOD_console_set_fullscreen(!TCOD_console_is_fullscreen());
	@PyEx
		key=Key()
		libtcod.console_check_for_event(libtcod.EVENT_KEY_PRESS,key,0)
		if key.vk == libtcod.KEY_ENTER and key.lalt :
			libtcod.console_set_fullscreen(not libtcod.console_is_fullscreen())
	@LuaEx
		key=tcod.console.checkForKeypress()
		if key.KeyCode == tcod.Enter and key.LeftAlt then
			tcod.console.setFullscreen(not tcod.console.isFullscreen())
		end
	*/
	static void setFullscreen(bool fullscreen);

	/**
	@PageName console_window
	@PageFather console_init
	@PageTitle Communicate with the window manager
	@FuncTitle Changing the window title
	@FuncDesc This function dynamically changes the title of the game window.
		Note that the window title is not visible while in fullscreen.
	@Cpp static void TCODConsole::setWindowTitle(const char *title)
 	@C void TCOD_console_set_window_title(const char *title)
	@Py console_set_window_title(title)
	@C# static void TCODConsole::setWindowTitle(string title)
	@Lua tcod.console.setWindowTitle(title)
	@Param title New title of the game window
	*/
	static void setWindowTitle(const char *title);

	/**
	@PageName console_window
	@FuncTitle Handling "close window" events
	@FuncDesc When you start the program, this returns false. Once a "close window" event has been sent by the window manager, it will always return true. You're supposed to exit cleanly the game.
	@Cpp static bool TCODConsole::isWindowClosed()
	@C bool TCOD_console_is_window_closed()
	@Py console_is_window_closed()
	@C# static bool TCODConsole::isWindowClosed()
	@Lua tcod.console.isWindowClosed()
	*/
	static bool isWindowClosed();

	/**
	@PageName console_window
	@FuncTitle Check if the mouse cursor is inside the game window
	@FuncDesc Returns true if the mouse cursor is inside the game window area and the game window is the active application.
	@Cpp static bool TCODConsole::hasMouseFocus()
	@C bool TCOD_console_has_mouse_focus()
	@Py console_has_mouse_focus()
	*/
	static bool hasMouseFocus();

	/**
	@PageName console_window
	@FuncTitle Check if the game application is active
	@FuncDesc Returns false if the game window is not the active window or is iconified.
	@Cpp static bool TCODConsole::isActive()
	@C bool TCOD_console_is_active()
	@Py console_is_active()
	*/
	static bool isActive();

	/**
	@PageName console_credits
	@PageTitle libtcod's credits
	@PageFather console_init
	@PageDesc Use these functions to display credits, as seen in the samples.
	@FuncTitle Using a separate credit page
	@FuncDesc You can print a "Powered by libtcod x.y.z" screen during your game startup simply by calling this function after initRoot.
		The credits screen can be skipped by pressing any key.
	@Cpp static void TCODConsole::credits()
	@C void TCOD_console_credits()
	@Py console_credits()
	@C# static void TCODConsole::credits()
	@Lua tcod.console.credits()
	*/
	static void credits();

	/**
	@PageName console_credits
	@FuncTitle Embedding credits in an existing page
	@FuncDesc You can also print the credits on one of your game screens (your main menu for example) by calling this function in your main loop.
		This function returns true when the credits screen is finished, indicating that you no longer need to call it.
	@Cpp static bool TCODConsole::renderCredits(int x, int y, bool alpha)
	@C bool TCOD_console_credits_render(int x, int y, bool alpha)
	@Py bool TCOD_console_credits_render(int x, int y, bool alpha)
	@C# static bool TCODConsole::renderCredits(int x, int y, bool alpha)
	@Lua tcod.console.renderCredits(x, y, alpha)
	@Param x,y Position of the credits text in your root console
	@Param alpha If true, credits are transparently added on top of the existing screen.
		For this to work, this function must be placed between your screen rendering code and the console flush.
	@CppEx
      TCODConsole::initRoot(80,50,"The Chronicles Of Doryen v0.1",false); // initialize the root console
      bool endCredits=false;
      while ( ! TCODConsole::isWindowClosed() ) { // your game loop
          // your game rendering here...
          // render transparent credits near the center of the screen
          if (! endCredits ) endCredits=TCODConsole::renderCredits(35,25,true);
          TCODConsole::flush();
      }
	@CEx
      TCOD_console_init_root(80,50,"The Chronicles Of Doryen v0.1",false);
      bool end_credits=false;
      while ( ! TCOD_console_is_window_closed() ) {
          // your game rendering here...
          // render transparent credits near the center of the screen
          if (! end_credits ) end_credits=TCOD_console_credits_render(35,25,true);
          TCOD_console_flush();
      }
	@PyEx
      libtcod.console_init_root(80,50,"The Chronicles Of Doryen v0.1",False)
      end_credits=False
      while not libtcod.console_is_window_closed() :
          // your game rendering here...
          // render transparent credits near the center of the screen
          if (not end_credits )  : end_credits=libtcod.console_credits_render(35,25,True)
          libtcod.console_flush()
	@LuaEx
		tcod.console.initRoot(80,50,"The Chronicles Of Doryen v0.1") -- initialize the root console
		endCredits=false
		while not tcod.console.isWindowClosed() do -- your game loop
			-- your game rendering here...
			-- render transparent credits near the center of the screen
			if not endCredits then endCredits=tcod.console.renderCredits(35,25,true) end
			tcod.console.flush()
		end
	*/
	static bool renderCredits(int x, int y, bool alpha);

	/**
	@PageName console_credits
	@FuncTitle Restart the credits animation
	@FuncDesc When using rederCredits, you can restart the credits animation from the beginning before it's finished by calling this function.
	@Cpp static void TCODConsole::resetCredits()
	@C void TCOD_console_credits_reset()
	@Py console_credits_reset()
	@C# static void TCODConsole::resetCredits()
	@Lua tcod.console.resetCredits()
	*/
	static void resetCredits();

	/**
	@PageName console_draw
	@PageTitle Drawing on the root console
	@PageFather console
	*/

	/**
	@PageName console_draw_basic
	@PageTitle Basic printing functions
	@PageFather console_draw
	@FuncTitle Setting the default background color
	@FuncDesc This function changes the default background color for a console. The default background color is used by several drawing functions like clear, putChar, ...
	@Cpp void TCODConsole::setDefaultBackground(TCODColor back)
	@C void TCOD_console_set_default_background(TCOD_console_t con,TCOD_color_t back)
	@Py console_set_default_background(con,back)
	@C# void TCODConsole::setBackgroundColor(TCODColor back)
	@Lua Console:setBackgroundColor(back)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param back the new default background color for this console
	@CppEx TCODConsole::root->setDefaultBackground(myColor)
	@CEx TCOD_console_set_default_background(NULL, my_color)
	@PyEx litbcod.console_set_default_background(0, my_color)
	@Lua libtcod.TCODConsole_root:setBackgroundColor( myColor )
	*/
	void setDefaultBackground(TCODColor back);

	/**
	@PageName console_draw_basic
	@FuncTitle Setting the default foreground color
	@FuncDesc This function changes the default foreground color for a console. The default foreground color is used by several drawing functions like clear, putChar, ...
	@Cpp void TCODConsole::setDefaultForeground(TCODColor fore)
	@C void TCOD_console_set_default_foreground(TCOD_console_t con,TCOD_color_t fore)
	@Py console_set_default_foreground(con, fore)
	@C# void TCODConsole::setForegroundColor(TCODColor fore)
	@Lua Console:setForegroundColor(fore)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param fore the new default foreground color for this console
	@CppEx TCODConsole::root->setDefaultForeground(myColor)
	@CEx TCOD_console_set_default_foreground(NULL, my_color)
	@PyEx litbcod.console_set_default_foreground(0, my_color)
	@LuaEx libtcod.TCODConsole_root:setForegroundColor( myColor )
	*/
	void setDefaultForeground(TCODColor fore);

	/**
	@PageName console_draw_basic
	@FuncTitle Clearing a console
	@FuncDesc This function modifies all cells of a console :
		* set the cell's background color to the console default background color
		* set the cell's foreground color to the console default foreground color
		* set the cell's ASCII code to 32 (space)
	@Cpp void TCODConsole::clear()
	@C void TCOD_console_clear(TCOD_console_t con)
	@Py console_clear(con)
	@C# void TCODConsole::clear()
	@Lua Console:clear()
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	*/
	void clear();

	/**
	@PageName console_draw_basic
	@FuncTitle Setting the background color of a cell
	@FuncDesc This function modifies the background color of a cell, leaving other properties (foreground color and ASCII code) unchanged.
	@Cpp void TCODConsole::setCharBackground(int x, int y, const TCODColor &col, TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET)
	@C void TCOD_console_set_char_background(TCOD_console_t con,int x, int y, TCOD_color_t col, TCOD_bkgnd_flag_t flag)
	@Py console_set_char_background(con, x,  y, col, flag=BKGND_SET)
	@C#
		void TCODConsole::setCharBackground(int x, int y, TCODColor col)
		void TCODConsole::setCharBackground(int x, int y, TCODColor col, TCODBackgroundFlag flag)
	@Lua
		Console:setCharBackground(x, y, col)
		Console:setCharBackground(x, y, col, flag)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinates of the cell in the console.
		0 <= x < console width
		0 <= y < console height
	@Param col the background color to use. You can use color constants
	@Param flag this flag defines how the cell's background color is modified. See <a href="console_bkgnd_flag_t.html">TCOD_bkgnd_flag_t</a>
	*/
	void setCharBackground(int x, int y, const TCODColor &col, TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET);
	/**
	@PageName console_draw_basic
	@FuncTitle Setting the foreground color of a cell
	@FuncDesc This function modifies the foreground color of a cell, leaving other properties (background color and ASCII code) unchanged.
	@Cpp void TCODConsole::setCharForeground(int x, int y, const TCODColor &col)
	@C void TCOD_console_set_char_foreground(TCOD_console_t con,int x, int y, TCOD_color_t col)
	@Py console_set_char_foreground(con, x, y, col)
	@C# void TCODConsole::setCharForeground(int x, int y, TCODColor col)
	@Lua Console:setCharForeground(x, y, col)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinates of the cell in the console.
		0 <= x < console width
		0 <= y < console height
	@Param col the foreground color to use. You can use color constants
	*/
	void setCharForeground(int x, int y, const TCODColor &col);

	/**
	@PageName console_draw_basic
	@FuncTitle Setting the ASCII code of a cell
	@FuncDesc This function modifies the ASCII code of a cell, leaving other properties (background and foreground colors) unchanged.
		Note that since a clear console has both background and foreground colors set to black for every cell, using setchar will produce black characters on black background. Use putchar instead.
	@Cpp void TCODConsole::setChar(int x, int y, int c)
	@C void TCOD_console_set_char(TCOD_console_t con,int x, int y, int c)
	@Py console_set_char(con, x,  y, c)
	@C# void TCODConsole::setChar(int x, int y, int c)
	@Lua Console:setChar(x, y, c)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinates of the cell in the console.
		0 <= x < console width
		0 <= y < console height
	@Param c the new ASCII code for the cell. You can use ASCII constants
	*/
	void setChar(int x, int y, int c);

	/**
	@PageName console_draw_basic
	@FuncTitle Setting every property of a cell using default colors
	@FuncDesc This function modifies every property of a cell :
		* update the cell's background color according to the console default background color (see TCOD_bkgnd_flag_t).
		* set the cell's foreground color to the console default foreground color
		* set the cell's ASCII code to c
	@Cpp void TCODConsole::putChar(int x, int y, int c, TCOD_bkgnd_flag_t flag = TCOD_BKGND_DEFAULT)
	@C void TCOD_console_put_char(TCOD_console_t con,int x, int y, int c, TCOD_bkgnd_flag_t flag)
	@Py console_put_char( con, x,  y, c, flag=BKGND_DEFAULT)
	@C#
		void TCODConsole::putChar(int x, int y, int c)
		void TCODConsole::putChar(int x, int y, int c, TCODBackgroundFlag flag)
	@Lua
		Console:putChar(x, y, c)
		Console:putChar(x, y, c, flag)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinates of the cell in the console.
		0 <= x < console width
		0 <= y < console height
	@Param c the new ASCII code for the cell. You can use ASCII constants
	@Param flag this flag defines how the cell's background color is modified. See TCOD_bkgnd_flag_t
	*/
	void putChar(int x, int y, int c, TCOD_bkgnd_flag_t flag = TCOD_BKGND_DEFAULT);

	/**
	@PageName console_draw_basic
	@FuncTitle Setting every property of a cell using specific colors
	@FuncDesc This function modifies every property of a cell :
		* set the cell's background color to back.
		* set the cell's foreground color to fore.
		* set the cell's ASCII code to c.
	@Cpp void TCODConsole::putCharEx(int x, int y, int c, const TCODColor & fore, const TCODColor & back)
	@C void TCOD_console_put_char_ex(TCOD_console_t con,int x, int y, int c, TCOD_color_t fore, TCOD_color_t back)
	@Py console_put_char_ex( con, x,  y, c, fore, back)
	@C# void TCODConsole::putCharEx(int x, int y, int c, TCODColor fore, TCODColor back)
	@Lua Console:putCharEx(x, y, c, fore, back)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinates of the cell in the console.
		0 <= x < console width
		0 <= y < console height
	@Param c the new ASCII code for the cell. You can use ASCII constants
	@Param fore,back new foreground and background colors for this cell
	*/
	void putCharEx(int x, int y, int c, const TCODColor &fore, const TCODColor &back);

	/**
	@PageName console_bkgnd_flag_t
	@PageTitle Background effect flags
	@PageFather console_draw
	@PageDesc This flag is used by most functions that modify a cell background color. It defines how the console's current background color is used to modify the cell's existing background color :
		TCOD_BKGND_NONE : the cell's background color is not modified.
		TCOD_BKGND_SET : the cell's background color is replaced by the console's default background color : newbk = curbk.
		TCOD_BKGND_MULTIPLY : the cell's background color is multiplied by the console's default background color : newbk = oldbk * curbk
		TCOD_BKGND_LIGHTEN : newbk = MAX(oldbk,curbk)
		TCOD_BKGND_DARKEN : newbk = MIN(oldbk,curbk)
		TCOD_BKGND_SCREEN : newbk = white - (white - oldbk) * (white - curbk) // inverse of multiply : (1-newbk) = (1-oldbk)*(1-curbk)
		TCOD_BKGND_COLOR_DODGE : newbk = curbk / (white - oldbk)
		TCOD_BKGND_COLOR_BURN : newbk = white - (white - oldbk) / curbk
		TCOD_BKGND_ADD : newbk = oldbk + curbk
		TCOD_BKGND_ADDALPHA(alpha) : newbk = oldbk + alpha*curbk
		TCOD_BKGND_BURN : newbk = oldbk + curbk - white
		TCOD_BKGND_OVERLAY : newbk = curbk.x <= 0.5 ? 2*curbk*oldbk : white - 2*(white-curbk)*(white-oldbk)
		TCOD_BKGND_ALPHA(alpha) : newbk = (1.0f-alpha)*oldbk + alpha*(curbk-oldbk)
		TCOD_BKGND_DEFAULT : use the console's default background flag
		Note that TCOD_BKGND_ALPHA and TCOD_BKGND_ADDALPHA are MACROS that needs a float parameter between (0.0 and 1.0). TCOD_BKGND_ALPH and TCOD_BKGND_ADDA should not be used directly (else they will have the same effect as TCOD_BKGND_NONE).
		For Python, remove TCOD_ : libtcod.BKGND_NONE
		For C# : None, Set, Multiply, Lighten, Darken, Screen, ColodDodge, ColorBurn, Add, Burn Overlay, Default
		With lua, use tcod.None, ..., tcod.Default, BUT tcod.console.Alpha(value) and tcod.console.AddAlpha(value)
	*/

	/**
	@PageName console_print
	@PageTitle String drawing functions
	@PageFather console_draw
	@FuncTitle Setting the default background flag
	@FuncDesc This function defines the background mode (see TCOD_bkgnd_flag_t) for the console.
		This default mode is used by several functions (print, printRect, ...)
	@Cpp void TCODConsole::setBackgroundFlag(TCOD_bkgnd_flag_t flag)
	@C void TCOD_console_set_background_flag(TCOD_console_t con,TCOD_bkgnd_flag_t flag)
	@Py console_set_background_flag(con, flag)
	@C# void TCODConsole::setBackgroundFlag(TCODBackgroundFlag flag)
	@Lua Console:setBackgroundFlag(flag)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param flag this flag defines how the cell's background color is modified. See TCOD_bkgnd_flag_t
	*/
	void setBackgroundFlag(TCOD_bkgnd_flag_t flag);

	/**
	@PageName console_print
	@FuncTitle Getting the default background flag
	@FuncDesc This function returns the background mode (see TCOD_bkgnd_flag_t) for the console.
		This default mode is used by several functions (print, printRect, ...)
	@Cpp TCOD_bkgnd_flag_t TCODConsole::getBackgroundFlag() const
	@C TCOD_bkgnd_flag_t TCOD_console_get_background_flag(TCOD_console_t con)
	@Py console_get_background_flag(con)
	@C# TCODBackgroundFlag TCODConsole::getBackgroundFlag()
	@Lua Console:getBackgroundFlag()
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	*/
	TCOD_bkgnd_flag_t getBackgroundFlag() const;

	/**
	@PageName console_print
	@FuncTitle Setting the default alignment
	@FuncDesc This function defines the default alignment (see TCOD_alignment_t) for the console.
		This default alignment is used by several functions (print, printRect, ...).
		Values for alignment : TCOD_LEFT, TCOD_CENTER, TCOD_RIGHT (in Python, remove TCOD_ : libtcod.LEFT).
		For C# and Lua : LeftAlignment, RightAlignment, CenterAlignment
	@Cpp void TCODConsole::setAlignment(TCOD_alignment_t alignment)
	@C void TCOD_console_set_alignment(TCOD_console_t con,TCOD_bkgnd_flag_t alignment)
	@Py console_set_alignment(con, alignment)
	@C# void TCODConsole::setAlignment(TCODAlignment alignment)
	@Lua Console:setAlignment(alignment)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param alignment defines how the strings are printed on screen.
	*/
	void setAlignment(TCOD_alignment_t alignment);

	/**
	@PageName console_print
	@FuncTitle Getting the default alignment
	@FuncDesc This function returns the default alignment (see TCOD_alignment_t) for the console.
		This default mode is used by several functions (print, printRect, ...).
		Values for alignment : TCOD_LEFT, TCOD_CENTER, TCOD_RIGHT (in Python, remove TCOD_ : libtcod.LEFT).
		For C# and Lua : LeftAlignment, RightAlignment, CenterAlignment
	@Cpp TCOD_alignment_t TCODConsole::getAlignment() const
	@C TCOD_alignment_t TCOD_console_get_alignment(TCOD_console_t con)
	@Py console_get_alignment(con)
	@C# TCODAlignment TCODConsole::getAlignment()
	@Lua Console:getAlignment()
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	*/
	TCOD_alignment_t getAlignment() const;
  /**
   *  Print an EASCII formatted string to the console.
   *  \rst
   *  .. deprecated:: 1.8
   *    EASCII is being phased out.  Use TCODConsole::printf or one of the
   *    UTF-8 overloads.
   *  \endrst
   */
  TCOD_DEPRECATED("Use TCODConsole::printf or the std::string overload for"
                  " this function.")
  void print(int x, int y, const char *fmt, ...);
  /**
   *  Print a UTF-8 string to the console.
   *
   *  This method will use this consoles default alignment, blend mode, and
   *  colors.
   *  \rst
   *  .. versionadded:: 1.8
   *  \endrst
   */
  void print(int x, int y, const std::string &str);
  /**
   *  Print a UTF-8 string to the console with specific alignment and blend
   *  mode.
   *  \rst
   *  .. versionadded:: 1.8
   *  \endrst
   */
  void print(int x, int y, const std::string &str,
             TCOD_alignment_t alignment, TCOD_bkgnd_flag_t flag);
  /**
   *  Format and print a UTF-8 string to the console.
   *
   *  This method will use this consoles default alignment, blend mode, and
   *  colors.
   *  \rst
   *  .. versionadded:: 1.8
   *  \endrst
   */
  TCODLIB_FORMAT(4, 5)
  void printf(int x, int y, const char *fmt, ...);
  /**
   *  Format and print a UTF-8 string to the console with specific alignment
   *  and blend mode.
   *  \rst
   *  .. versionadded:: 1.8
   *  \endrst
   */
  TCODLIB_FORMAT(6, 7)
  void printf(int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment,
              const char *fmt, ...);
  /**
   *  Print an EASCII formatted string to the console.
   *  \rst
   *  .. deprecated:: 1.8
   *    Use `TCODConsole::print` or `TCODConsole::printf`.
   *    These functions have overloads for specifying flag and alignment.
   *  \endrst
   */
  TCOD_DEPRECATED("Use TCODConsole::print or TCODConsole::printf instead of"
                  " this function.")
  void printEx(int x, int y, TCOD_bkgnd_flag_t flag,
               TCOD_alignment_t alignment, const char *fmt, ...);
	/**
	@PageName console_print
	@FuncTitle Printing a string with default parameters and autowrap
	@FuncDesc This function draws a string in a rectangle inside the console, using default colors, alignment and background mode.
		If the string reaches the borders of the rectangle, carriage returns are inserted.
		If h > 0 and the bottom of the rectangle is reached, the string is truncated. If h = 0, the string is only truncated if it reaches the bottom of the console.
		The function returns the height (number of console lines) of the printed string.
	@Cpp int TCODConsole::printRect(int x, int y, int w, int h, const char *fmt, ...)
	@C int TCOD_console_print_rect(TCOD_console_t con,int x, int y, int w, int h, const char *fmt, ...)
	@Py console_print_rect(con, x, y, w, h, fmt)
	@C# int TCODConsole::printRect(int x, int y, int w, int h, string fmt)
	@Lua Console:printRect(x, y, w, h, fmt)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinate of the character in the console, depending on the alignment :
		* TCOD_LEFT : leftmost character of the string
		* TCOD_CENTER : center character of the string
		* TCOD_RIGHT : rightmost character of the string
	@Param w,h size of the rectangle
		x <= x+w < console width
		y <= y+h < console height
	@Param fmt printf-like format string, eventually followed by parameters. You can use control codes to change the colors inside the string, except in C#.
	*/
  TCODLIB_FORMAT(6, 7)
  int printRect(int x, int y, int w, int h, const char *fmt, ...);

	/**
	@PageName console_print
	@FuncTitle Printing a string with specific alignment and background mode and autowrap
	@FuncDesc This function draws a string in a rectangle inside the console, using default colors, but specific alignment and background mode.
		If the string reaches the borders of the rectangle, carriage returns are inserted.
		If h > 0 and the bottom of the rectangle is reached, the string is truncated. If h = 0, the string is only truncated if it reaches the bottom of the console.
		The function returns the height (number of console lines) of the printed string.
	@Cpp int TCODConsole::printRectEx(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char *fmt, ...)
	@C int TCOD_console_print_rect_ex(TCOD_console_t con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char *fmt, ...)
	@Py console_print_rect_ex(con, x, y, w, h, flag, alignment, fmt)
	@C# int TCODConsole::printRectEx(int x, int y, int w, int h, TCODBackgroundFlag flag, TCODAlignment alignment, string fmt)
	@Lua Console:printRectEx(x, y, w, h, flag, alignment, fmt)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinate of the character in the console, depending on the alignment :
		* TCOD_LEFT : leftmost character of the string
		* TCOD_CENTER : center character of the string
		* TCOD_RIGHT : rightmost character of the string
	@Param w,h size of the rectangle
		x <= x+w < console width
		y <= y+h < console height
	@Param flag this flag defines how the cell's background color is modified. See TCOD_bkgnd_flag_t
	@Param alignment defines how the strings are printed on screen.
	@Param fmt printf-like format string, eventually followed by parameters. You can use control codes to change the colors inside the string, except in C#.
	*/
  TCODLIB_FORMAT(8, 9)
  int printRectEx(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag,
                  TCOD_alignment_t alignment, const char *fmt, ...);

	/**
	@PageName console_print
	@FuncTitle Compute the height of an autowrapped string
	@FuncDesc This function returns the expected height of an autowrapped string without actually printing the string with printRect or printRectEx
	@Cpp int TCODConsole::getHeightRect(int x, int y, int w, int h, const char *fmt, ...)

	@C int TCOD_console_get_height_rect(TCOD_console_t con,int x, int y, int w, int h, const char *fmt, ...)
	@Py console_get_height_rect(con, x, y, w, h, fmt)
	@C# int TCODConsole::getHeightRect(int x, int y, int w, int h, string fmt)
	@Lua Console:getHeightRect(x, y, w, h, fmt)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinate of the rectangle upper-left corner in the console
	@Param w,h size of the rectangle
		x <= x+w < console width
		y <= y+h < console height
	@Param fmt printf-like format string, eventually followed by parameters. You can use control codes to change the colors inside the string, except in C#.
	*/
  TCODLIB_FORMAT(6, 7)
  int getHeightRect(int x, int y, int w, int h, const char *fmt, ...);

	/**
	@PageName console_print
	@FuncTitle Changing the colors while printing a string
	@FuncDesc If you want to draw a string using different colors for each word, the basic solution is to call a string printing function several times, changing the default colors between each call.
		The TCOD library offers a simpler way to do this, allowing you to draw a string using different colors in a single call. For this, you have to insert color control codes in your string.
		A color control code is associated with a color set (a foreground color and a background color). If you insert this code in your string, the next characters will use the colors associated with the color control code.
		There are 5 predefined color control codes :
		For Python, remove TCOD_ : libtcod.COLCTRL_1
			TCOD_COLCTRL_1
			TCOD_COLCTRL_2
			TCOD_COLCTRL_3
			TCOD_COLCTRL_4
			TCOD_COLCTRL_5
		To associate a color with a code, use setColorControl.
		To go back to the console's default colors, insert in your string the color stop control code :
			TCOD_COLCTRL_STOP

		You can also use any color without assigning it to a control code, using the generic control codes :
			TCOD_COLCTRL_FORE_RGB
			TCOD_COLCTRL_BACK_RGB

		Those controls respectively change the foreground and background color used to print the string characters. In the string, you must insert the r,g,b components of the color (between 1 and 255. The value 0 is forbidden because it represents the end of the string in C/C++) immediately after this code.
	@Cpp static void TCODConsole::setColorControl(TCOD_colctrl_t con, const TCODColor &fore, const TCODColor &back)
	@C void TCOD_console_set_color_control(TCOD_colctrl_t con, TCOD_color_t fore, TCOD_color_t back)
	@Py console_set_color_control(con,fore,back)
	@C# Not supported directly, use getRGBColorControlString and getColorControlString.
	@Lua Not supported
	@Param con the color control TCOD_COLCTRL_x, 1<=x<=5
	@Param fore foreground color when this control is activated
	@Param back background color when this control is activated
	@CppEx
		// A string with a red over black word, using predefined color control codes
		TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::red,TCODColor::black);
		TCODConsole::root->print(1,1,"String with a %cred%c word.",TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
		// A string with a red over black word, using generic color control codes
		TCODConsole::root->print(1,1,"String with a %c%c%c%c%c%c%c%cred%c word.",
	          TCOD_COLCTRL_FORE_RGB,255,1,1,TCOD_COLCTRL_BACK_RGB,1,1,1,TCOD_COLCTRL_STOP);
		// A string with a red over black word, using generic color control codes
		TCODConsole::root->print(1,1,"String with a %c%c%c%c%c%c%c%cred%c word.",
	          TCOD_COLCTRL_FORE_RGB,255,1,1,TCOD_COLCTRL_BACK_RGB,1,1,1,TCOD_COLCTRL_STOP);
	@CEx
		// A string with a red over black word, using predefined color control codes
		TCOD_console_set_color_control(TCOD_COLCTRL_1,red,black);
		TCOD_console_print(NULL,1,1,"String with a %cred%c word.",TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
		// A string with a red word (over default background color), using generic color control codes
		TCOD_console_print(NULL,1,1,"String with a %c%c%c%cred%c word.",
			TCOD_COLCTRL_FORE_RGB,255,1,1,TCOD_COLCTRL_STOP);
		// A string with a red over black word, using generic color control codes
		TCOD_console_print(NULL,1,1,"String with a %c%c%c%c%c%c%c%cred%c word.",
			TCOD_COLCTRL_FORE_RGB,255,1,1,TCOD_COLCTRL_BACK_RGB,1,1,1,TCOD_COLCTRL_STOP);
	@PyEx
		# A string with a red over black word, using predefined color control codes
		libtcod.console_set_color_control(libtcod.COLCTRL_1,litbcod.red,litbcod.black)
		libtcod.console_print(0,1,1,"String with a %cred%c word."%(libtcod.COLCTRL_1,libtcod.COLCTRL_STOP))
		# A string with a red word (over default background color), using generic color control codes
		litbcod.console_print(0,1,1,"String with a %c%c%c%cred%c word."%(libtcod.COLCTRL_FORE_RGB,255,1,1,libtcod.COLCTRL_STOP))
		# A string with a red over black word, using generic color control codes
		libtcod.console_print(0,1,1,"String with a %c%c%c%c%c%c%c%cred%c word."%
		        (libtcod.COLCTRL_FORE_RGB,255,1,1,libtcod.COLCTRL_BACK_RGB,1,1,1,libtcod.COLCTRL_STOP))

	@C#Ex
		TCODConsole.root.print(1,1,String.Format("String with a {0}red{1} word.",
			TCODConsole.getRGBColorControlString(ColorControlForeground,TCODColor.red),
			TCODConsole.getColorControlString(ColorControlStop));
	*/
	static void setColorControl(TCOD_colctrl_t con, const TCODColor &fore, const TCODColor &back);

#ifndef NO_UNICODE
	/**
	@PageName console_print
	@FuncTitle Unicode functions
	@FuncDesc those functions are similar to their ASCII equivalent, but work with unicode strings (wchar_t in C/C++).
		Note that unicode is not supported in the Python wrapper.
	@Cpp static void TCODConsole::mapStringToFont(const wchar_t *s, int fontCharX, int fontCharY)
	@C void TCOD_console_map_string_to_font_utf(const wchar_t *s, int fontCharX, int fontCharY)
	*/
	static void mapStringToFont(const wchar_t *s, int fontCharX, int fontCharY);
	/**
	@PageName console_print
	@Cpp void TCODConsole::print(int x, int y, const wchar_t *fmt, ...)
	@C void TCOD_console_print_utf(TCOD_console_t con,int x, int y, const wchar_t *fmt, ...)
	*/
	void print(int x, int y, const wchar_t *fmt, ...);
	/**
	@PageName console_print
	@Cpp void TCODConsole::printEx(int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const wchar_t *fmt, ...)
	@C void TCOD_console_print_ex_utf(TCOD_console_t con,int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const wchar_t *fmt, ...)
	*/
	void printEx(int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const wchar_t *fmt, ...);
	/**
	@PageName console_print
	@Cpp int TCODConsole::printRect(int x, int y, int w, int h, const wchar_t *fmt, ...)
	@C int TCOD_console_print_rect_utf(TCOD_console_t con,int x, int y, int w, int h, const wchar_t *fmt, ...)
	*/
   	int printRect(int x, int y, int w, int h, const wchar_t *fmt, ...);

	/**
	@PageName console_print
	@Cpp int TCODConsole::printRectEx(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const wchar_t *fmt, ...)
	@C int TCOD_console_print_rect_ex_utf(TCOD_console_t con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const wchar_t *fmt, ...)
	*/
	int printRectEx(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const wchar_t *fmt, ...);

	/**
	@PageName console_print
	@Cpp int TCODConsole::getHeightRect(int x, int y, int w, int h, const wchar_t *fmt, ...)
	@C int TCOD_console_get_height_rect_utf(TCOD_console_t con,int x, int y, int w, int h, const wchar_t *fmt, ...)
	*/
	int getHeightRect(int x, int y, int w, int h, const wchar_t *fmt, ...);
#endif

	/**
	@PageName console_advanced
	@PageFather console_draw
	@PageTitle Advanced printing functions
	@FuncTitle Filling a rectangle with the background color
	@FuncDesc Fill a rectangle inside a console. For each cell in the rectangle :
		* set the cell's background color to the console default background color
		* if clear is true, set the cell's ASCII code to 32 (space)
	@Cpp void TCODConsole::rect(int x, int y, int w, int h, bool clear, TCOD_bkgnd_flag_t flag = TCOD_BKGND_DEFAULT)
	@C void TCOD_console_rect(TCOD_console_t con,int x, int y, int w, int h, bool clear, TCOD_bkgnd_flag_t flag)
	@Py console_rect(con,x,  y,  w, h, clear, flag=BKGND_DEFAULT)
	@C#
		void TCODConsole::rect(int x, int y, int w, int h, bool clear)
		void TCODConsole::rect(int x, int y, int w, int h, bool clear, TCODBackgroundFlag flag)
	@Lua
		Console:rect(x, y, w, h, clear)
		Console:rect(x, y, w, h, clear, flag)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinates of rectangle upper-left corner in the console.
		0 <= x < console width
		0 <= y < console height
	@Param w,h size of the rectangle in the console.
		x <= x+w < console width
		y <= y+h < console height
	@Param clear if true, all characters inside the rectangle are set to ASCII code 32 (space).
		If false, only the background color is modified
	@Param flag this flag defines how the cell's background color is modified. See TCOD_bkgnd_flag_t
	*/
	void rect(int x, int y, int w, int h, bool clear, TCOD_bkgnd_flag_t flag = TCOD_BKGND_DEFAULT);

	/**
	@PageName console_advanced
	@FuncTitle Drawing an horizontal line
	@FuncDesc Draws an horizontal line in the console, using ASCII code TCOD_CHAR_HLINE (196), and the console's default background/foreground colors.
	@Cpp void TCODConsole::hline(int x,int y, int l, TCOD_bkgnd_flag_t flag = TCOD_BKGND_DEFAULT)
	@C void TCOD_console_hline(TCOD_console_t con,int x,int y, int l, TCOD_bkgnd_flag_t flag)
	@Py console_hline(con,x,y,l,flag=BKGND_DEFAULT)
	@C#
		void TCODConsole::hline(int x,int y, int l)
		void TCODConsole::hline(int x,int y, int l, TCODBackgroundFlag flag)
	@Lua
		Console:hline(x,y, l)
		Console:hline(x,y, l, flag)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y Coordinates of the line's left end in the console.
		0 <= x < console width
		0 <= y < console height
	@Param l The length of the line in cells 1 <= l <= console width - x
	@Param flag this flag defines how the cell's background color is modified. See TCOD_bkgnd_flag_t
	*/
	void hline(int x,int y, int l, TCOD_bkgnd_flag_t flag = TCOD_BKGND_DEFAULT);

	/**
	@PageName console_advanced
	@FuncTitle Drawing an vertical line
	@FuncDesc Draws an vertical line in the console, using ASCII code TCOD_CHAR_VLINE (179), and the console's default background/foreground colors.
	@Cpp void TCODConsole::vline(int x,int y, int l, TCOD_bkgnd_flag_t flag = TCOD_BKGND_DEFAULT)
	@C void TCOD_console_vline(TCOD_console_t con,int x,int y, int l, TCOD_bkgnd_flag_t flag)
	@Py console_vline(con,x,y,l,flag=BKGND_DEFAULT)
	@C#
		void TCODConsole::vline(int x,int y, int l)
		void TCODConsole::vline(int x,int y, int l, TCODBackgroundFlag flag)
	@Lua
		Console:vline(x,y, l)
		Console:vline(x,y, l, flag)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y Coordinates of the line's upper end in the console.
		0 <= x < console width
		0 <= y < console height
	@Param l The length of the line in cells 1 <= l <= console height - y
	@Param flag this flag defines how the cell's background color is modified. See TCOD_bkgnd_flag_t
	*/
	void vline(int x,int y, int l, TCOD_bkgnd_flag_t flag = TCOD_BKGND_DEFAULT);

	/**
	@PageName console_advanced
	@FuncTitle Drawing a window frame
	@FuncDesc This function calls the rect function using the supplied background mode flag, then draws a rectangle with the console's default foreground color. If fmt is not NULL, it is printed on the top of the rectangle, using inverted colors.
	@Cpp void TCODConsole::printFrame(int x,int y,int w,int h, bool clear=true, TCOD_bkgnd_flag_t flag = TCOD_BKGND_DEFAULT, const char *fmt=NULL, ...)
	@C void TCOD_console_print_frame(TCOD_console_t con,int x,int y,int w,int h, bool clear, TCOD_bkgnd_flag_t flag, const char *fmt, ...)
	@Py console_print_frame(con,x, y, w, h, clear=True, flag=BKGND_DEFAULT, fmt=0)
	@C#
		void TCODConsole::printFrame(int x,int y, int w,int h)
		void TCODConsole::printFrame(int x,int y, int w,int h, bool clear)
		void TCODConsole::printFrame(int x,int y, int w,int h, bool clear, TCODBackgroundFlag flag)
		void TCODConsole::printFrame(int x,int y, int w,int h, bool clear, TCODBackgroundFlag flag, string fmt)
	@Lua
		Console:printFrame(x,y, w,h)
		Console:printFrame(x,y, w,h, clear)
		Console:printFrame(x,y, w,h, clear, flag)
		Console:printFrame(x,y, w,h, clear, flag, fmt)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y Coordinates of the rectangle's upper-left corner in the console.
		0 <= x < console width
		0 <= y < console height
	@Param w,h size of the rectangle in the console.
		x <= x+w < console width
		y <= y+h < console height
	@Param clear if true, all characters inside the rectangle are set to ASCII code 32 (space).
		If false, only the background color is modified
	@Param flag this flag defines how the cell's background color is modified. See TCOD_bkgnd_flag_t
	@Param fmt if NULL, the function only draws a rectangle.
		Else, printf-like format string, eventually followed by parameters. You can use control codes to change the colors inside the string.
	*/
	void printFrame(int x,int y,int w,int h, bool clear=true, TCOD_bkgnd_flag_t flag = TCOD_BKGND_DEFAULT, const char *fmt=NULL, ...);

	/**
	@PageName console_read
	@PageTitle Reading the content of the console
	@PageFather console_draw
	@FuncTitle Get the console's width
	@FuncDesc This function returns the width of a console (either the root console or an offscreen console)
	@Cpp int TCODConsole::getWidth() const
	@C int TCOD_console_get_width(TCOD_console_t con)
	@Py console_get_width(con)
	@C# int TCODConsole::getWidth()
	@Lua Console:getWidth()
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	*/
	int getWidth() const;

	/**
	@PageName console_read
	@FuncTitle Get the console's height
	@FuncDesc This function returns the height of a console (either the root console or an offscreen console)
	@Cpp int TCODConsole::getHeight() const
	@C int TCOD_console_get_height(TCOD_console_t con)
	@Py console_get_height(con)
	@C# int TCODConsole::getHeight()
	@Lua Console:getHeight()
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	*/
	int getHeight() const;

	/**
	@PageName console_read
	@FuncTitle Reading the default background color
	@FuncDesc This function returns the default background color of a console.
	@Cpp TCODColor TCODConsole::getDefaultBackground() const
	@C TCOD_color_t TCOD_console_get_default_background(TCOD_console_t con)
	@Py console_get_default_background(con)
	@C# TCODColor TCODConsole::getBackgroundColor()
	@Lua Console:getBackgroundColor()
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	*/
	TCODColor getDefaultBackground() const;

	/**
	@PageName console_read
	@FuncTitle Reading the default foreground color
	@FuncDesc This function returns the default foreground color of a console.
	@Cpp TCODColor TCODConsole::getDefaultForeground() const
	@C TCOD_color_t TCOD_console_get_default_foreground(TCOD_console_t con)
	@Py console_get_default_foreground(con)
	@C# TCODColor TCODConsole::getForegroundColor()
	@Lua Console:getForegroundColor()
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	*/
	TCODColor getDefaultForeground() const;

	/**
	@PageName console_read
	@FuncTitle Reading the background color of a cell
	@FuncDesc This function returns the background color of a cell.
	@Cpp TCODColor TCODConsole::getCharBackground(int x, int y) const
	@C TCOD_color_t TCOD_console_get_char_background(TCOD_console_t con,int x, int y)
	@Py console_get_char_background(con,x,y)
	@C# TCODColor TCODConsole::getCharBackground(int x, int y)
	@Lua Console::getCharBackground(x, y)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinates of the cell in the console.
		0 <= x < console width
		0 <= y < console height
	*/
	TCODColor getCharBackground(int x, int y) const;

	/**
	@PageName console_read
	@FuncTitle Reading the foreground color of a cell
	@FuncDesc This function returns the foreground color of a cell.
	@Cpp TCODColor TCODConsole::getCharForeground(int x, int y) const
	@C TCOD_color_t TCOD_console_get_char_foreground(TCOD_console_t con,int x, int y)
	@Py console_get_char_foreground(con,x,y)
	@C# TCODColor TCODConsole::getCharForeground(int x, int y)
	@Lua Console::getCharForeground(x, y)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinates of the cell in the console.
		0 <= x < console width
		0 <= y < console height
	*/
	TCODColor getCharForeground(int x, int y) const;

	/**
	@PageName console_read
	@FuncTitle Reading the ASCII code of a cell
	@FuncDesc This function returns the ASCII code of a cell.
	@Cpp int TCODConsole::getChar(int x, int y) const
	@C int TCOD_console_get_char(TCOD_console_t con,int x, int y)
	@Py console_get_char(con,x,y)
	@C# int TCODConsole::getChar(int x, int y)
	@Lua Console::getChar(x, y)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinates of the cell in the console.
		0 <= x < console width
		0 <= y < console height
	*/
	int getChar(int x, int y) const;

	/**
	@PageName console_fading
	@PageTitle Screen fading functions
	@PageFather console_draw
	@PageDesc Use these functions to easily fade to/from a color
	@FuncTitle Changing the fading parameters
	@FuncDesc This function defines the fading parameters, allowing to easily fade the game screen to/from a color. Once they are defined, the fading parameters are valid for ever. You don't have to call setFade for each rendered frame (unless you change the fading parameters).
	@Cpp static void TCODConsole::setFade(uint8_t fade, const TCODColor &fadingColor)
	@C void TCOD_console_set_fade(uint8_t fade, TCOD_color_t fadingColor)
	@Py console_set_fade(fade, fadingColor)
	@C# static void TCODConsole::setFade(byte fade, TCODColor fadingColor)
	@Lua tcod.console.setFade(fade, fadingColor)
	@Param fade the fading amount. 0 => the screen is filled with the fading color. 255 => no fading effect
	@Param fadingColor the color to use during the console flushing operation
	@CppEx
		for (int fade=255; fade >= 0; fade --) {
			TCODConsole::setFade(fade,TCODColor::black);
			TCODConsole::flush();
		}
	@CEx
		int fade;
		for (fade=255; fade >= 0; fade --) {
			TCOD_console_setFade(fade,TCOD_black);
			TCOD_console_flush();
		}
	@PyEx
		for fade in range(255,0) :
			libtcod.console_setFade(fade,libtcod.black)
			libtcod.console_flush()
	@LuaEx
		for fade=255,0,-1 do
			tcod.console.setFade(fade,tcod.color.black)
			tcod.console.flush()
		end
	*/
	static void setFade(uint8_t fade, const TCODColor &fadingColor);

	/**
	@PageName console_fading
	@FuncTitle Reading the fade amount
	@FuncDesc This function returns the current fade amount, previously defined by setFade.
	@Cpp static uint8_t TCODConsole::getFade()
	@C uint8_t TCOD_console_get_fade()
	@Py console_get_fade()
	@C# static byte TCODConsole::getFade()
	@Lua tcod.console.getFade()
	*/
	static uint8_t getFade();

	/**
	@PageName console_fading
	@FuncTitle Reading the fading color
	@FuncDesc This function returns the current fading color, previously defined by setFade.
	@Cpp static TCODColor TCODConsole::getFadingColor()
	@C TCOD_color_t TCOD_console_get_fading_color()
	@Py console_get_fading_color()
	@C# static TCODColor TCODConsole::getFadingColor()
	@Lua tcod.console.getFadingColor()
	*/
	static TCODColor getFadingColor();

	/**
	@PageName console_flush
	@PageFather console
	@PageTitle Flushing the root console
	@FuncDesc Once the root console is initialized, you can use one of the printing functions to change the background colors, the foreground colors or the ASCII characters on the console.
		Once you've finished rendering the root console, you have to actually apply the updates to the screen with this function.
	@Cpp static void TCODConsole::flush()
	@C void TCOD_console_flush()
	@Py console_flush()
	@C# static void TCODConsole::flush()
	@Lua tcod.console.flush()
	*/
	static void flush();

	/**
	@PageName console_ascii
	@PageTitle ASCII constants
	@PageFather console_draw
	@FuncDesc Some useful graphic characters in the terminal.bmp font. For the Python version, remove TCOD_ from the constants. C# and Lua is in parenthesis :
		Single line walls:
		TCOD_CHAR_HLINE=196 (HorzLine)
		TCOD_CHAR_VLINE=179 (VertLine)
		TCOD_CHAR_NE=191 (NE)
		TCOD_CHAR_NW=218 (NW)
		TCOD_CHAR_SE=217 (SE)
		TCOD_CHAR_SW=192 (SW)

		Double lines walls:
		TCOD_CHAR_DHLINE=205 (DoubleHorzLine)
		TCOD_CHAR_DVLINE=186 (DoubleVertLine)
		TCOD_CHAR_DNE=187 (DoubleNE)
		TCOD_CHAR_DNW=201 (DoubleNW)
		TCOD_CHAR_DSE=188 (DoubleSE)
		TCOD_CHAR_DSW=200 (DoubleSW)

		Single line vertical/horizontal junctions (T junctions):
		TCOD_CHAR_TEEW=180 (TeeWest)
		TCOD_CHAR_TEEE=195 (TeeEast)
		TCOD_CHAR_TEEN=193 (TeeNorth)
		TCOD_CHAR_TEES=194 (TeeSouth)

		Double line vertical/horizontal junctions (T junctions):
		TCOD_CHAR_DTEEW=185 (DoubleTeeWest)
		TCOD_CHAR_DTEEE=204 (DoubleTeeEast)
		TCOD_CHAR_DTEEN=202 (DoubleTeeNorth)
		TCOD_CHAR_DTEES=203 (DoubleTeeSouth)

		Block characters:
		TCOD_CHAR_BLOCK1=176 (Block1)
		TCOD_CHAR_BLOCK2=177 (Block2)
		TCOD_CHAR_BLOCK3=178 (Block3)

		Cross-junction between two single line walls:
		TCOD_CHAR_CROSS=197 (Cross)

		Arrows:
		TCOD_CHAR_ARROW_N=24 (ArrowNorth)
		TCOD_CHAR_ARROW_S=25 (ArrowSouth)
		TCOD_CHAR_ARROW_E=26 (ArrowEast)
		TCOD_CHAR_ARROW_W=27 (ArrowWest)

		Arrows without tail:
		TCOD_CHAR_ARROW2_N=30 (ArrowNorthNoTail)
		TCOD_CHAR_ARROW2_S=31 (ArrowSouthNoTail)
		TCOD_CHAR_ARROW2_E=16 (ArrowEastNoTail)
		TCOD_CHAR_ARROW2_W=17 (ArrowWestNoTail)

		Double arrows:
		TCOD_CHAR_DARROW_H=29 (DoubleArrowHorz)
		TCOD_CHAR_ARROW_V=18 (DoubleArrowVert)

		GUI stuff:
		TCOD_CHAR_CHECKBOX_UNSET=224
		TCOD_CHAR_CHECKBOX_SET=225
		TCOD_CHAR_RADIO_UNSET=9
		TCOD_CHAR_RADIO_SET=10

		Sub-pixel resolution kit:
		TCOD_CHAR_SUBP_NW=226 (SubpixelNorthWest)
		TCOD_CHAR_SUBP_NE=227 (SubpixelNorthEast)
		TCOD_CHAR_SUBP_N=228 (SubpixelNorth)
		TCOD_CHAR_SUBP_SE=229 (SubpixelSouthEast)
		TCOD_CHAR_SUBP_DIAG=230 (SubpixelDiagonal)
		TCOD_CHAR_SUBP_E=231 (SubpixelEast)
		TCOD_CHAR_SUBP_SW=232 (SubpixelSouthWest)

		Miscellaneous characters:
		TCOD_CHAR_SMILY = 1 (Smilie)
		TCOD_CHAR_SMILY_INV = 2 (SmilieInv)
		TCOD_CHAR_HEART = 3 (Heart)
		TCOD_CHAR_DIAMOND = 4 (Diamond)
		TCOD_CHAR_CLUB = 5 (Club)
		TCOD_CHAR_SPADE = 6 (Spade)
		TCOD_CHAR_BULLET = 7 (Bullet)
		TCOD_CHAR_BULLET_INV = 8 (BulletInv)
		TCOD_CHAR_MALE = 11 (Male)
		TCOD_CHAR_FEMALE = 12 (Female)
		TCOD_CHAR_NOTE = 13 (Note)
		TCOD_CHAR_NOTE_DOUBLE = 14 (NoteDouble)
		TCOD_CHAR_LIGHT = 15 (Light)
		TCOD_CHAR_EXCLAM_DOUBLE = 19 (ExclamationDouble)
		TCOD_CHAR_PILCROW = 20 (Pilcrow)
		TCOD_CHAR_SECTION = 21 (Section)
		TCOD_CHAR_POUND = 156 (Pound)
		TCOD_CHAR_MULTIPLICATION = 158 (Multiplication)
		TCOD_CHAR_FUNCTION = 159 (Function)
		TCOD_CHAR_RESERVED = 169 (Reserved)
		TCOD_CHAR_HALF = 171 (Half)
		TCOD_CHAR_ONE_QUARTER = 172 (OneQuarter)
		TCOD_CHAR_COPYRIGHT = 184 (Copyright)
		TCOD_CHAR_CENT = 189 (Cent)
		TCOD_CHAR_YEN = 190 (Yen)
		TCOD_CHAR_CURRENCY = 207 (Currency)
		TCOD_CHAR_THREE_QUARTERS = 243 (ThreeQuarters)
		TCOD_CHAR_DIVISION = 246 (Division)
		TCOD_CHAR_GRADE = 248 (Grade)
		TCOD_CHAR_UMLAUT = 249 (Umlaut)
		TCOD_CHAR_POW1 = 251 (Pow1)
		TCOD_CHAR_POW3 = 252 (Pow2)
		TCOD_CHAR_POW2 = 253 (Pow3)
		TCOD_CHAR_BULLET_SQUARE = 254 (BulletSquare)
	*/

	/**
	@PageName console_input
	@PageTitle Handling user input
	@PageDesc The user handling functions allow you to get keyboard and mouse input from the user, either for turn by turn games (the function wait until the user press a key or a mouse button), or real time games (non blocking function).
	<b>WARNING : those functions also handle screen redraw event, so TCODConsole::flush function won't redraw screen if no user input function is called !</b>
	@PageFather console
	*/

	/* deprecated as of 1.5.1 */
	static TCOD_key_t waitForKeypress(bool flush);
	/* deprecated as of 1.5.1 */
	static TCOD_key_t checkForKeypress(int flags=TCOD_KEY_RELEASED);

	/**
	@PageName console_blocking_input
	@PageCategory Core
	@PageFather console_input
	@PageTitle Blocking user input
	@PageDesc This function stops the application until an event occurs.
	*/

	/**
	@PageName console_non_blocking_input
	@PageTitle Non blocking user input
	@PageFather console_input
	@FuncDesc The preferred way to check for user input is to use checkForEvent below, but you can also get the status of any special key at any time with :
	@Cpp static bool TCODConsole::isKeyPressed(TCOD_keycode_t key)
	@C bool TCOD_console_is_key_pressed(TCOD_keycode_t key)
	@Py console_is_key_pressed(key)
	@C# static bool TCODConsole::isKeyPressed(TCODKeyCode key)
	@Lua tcod.console.isKeyPressed(key)
	@Param key Any key code defined in keycode_t except TCODK_CHAR (Char) and TCODK_NONE (NoKey)
	*/
	static bool isKeyPressed(TCOD_keycode_t key);

	/**
	@PageName console_key_t
	@PageTitle 	Keyboard event structure
	@PageFather console_input
	@PageDesc This structure contains information about a key pressed/released by the user.
	@C
		typedef struct {
			TCOD_keycode_t vk;
			char c;
			char text[32];
			bool pressed;
			bool lalt;
			bool lctrl;
			bool lmeta;
			bool ralt;
			bool rctrl;
			bool rmeta;
			bool shift;
		} TCOD_key_t;
	@Lua
		key.KeyCode
		key.Character
		key.Text
		key.Pressed
		key.LeftAlt
		key.LeftControl
		key.LeftMeta
		key.RightAlt
		key.RightControl
		key.RightMeta
		key.Shift
	@Param vk An arbitrary value representing the physical key on the keyboard. Possible values are stored in the TCOD_keycode_t enum. If no key was pressed, the value is TCODK_NONE
	@Param c If the key correspond to a printable character, the character is stored in this field. Else, this field contains 0.
	@Param text If vk is TCODK_TEXT, this will contain the text entered by the user.
	@Param pressed true if the event is a key pressed, or false for a key released.
	@Param lalt This field represents the status of the left Alt key : true => pressed, false => released.
	@Param lctrl This field represents the status of the left Control key : true => pressed, false => released.
	@Param lmeta This field represents the status of the left Meta (Windows/Command/..) key : true => pressed, false => released.
	@Param ralt This field represents the status of the right Alt key : true => pressed, false => released.
	@Param rctrl This field represents the status of the right Control key : true => pressed, false => released.
	@Param rmeta This field represents the status of the right Meta (Windows/Command/..) key : true => pressed, false => released.
	@Param shift This field represents the status of the shift key : true => pressed, false => released.
	*/

	/**
	@PageName console_mouse_t
	@PageTitle Mouse event structure
	@PageFather console_input
	@PageDesc This structure contains information about a mouse move/press/release event.
	@C
		typedef struct {
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
	@Param x,y Absolute position of the mouse cursor in pixels relative to the window top-left corner.
	@Param dx,dy Movement of the mouse cursor since the last call in pixels.
	@Param cx,cy Coordinates of the console cell under the mouse cursor (pixel coordinates divided by the font size).
	@Param dcx,dcy Movement of the mouse since the last call in console cells (pixel coordinates divided by the font size).
	@Param lbutton true if the left button is pressed.
	@Param rbutton true if the right button is pressed.
	@Param mbutton true if the middle button (or the wheel) is pressed.
	@Param lbutton_pressed true if the left button was pressed and released.
	@Param rbutton_pressed true if the right button was pressed and released.
	@Param mbutton_pressed true if the middle button was pressed and released.
	@Param wheel_up true if the wheel was rolled up.
	@Param wheel_down true if the wheel was rolled down.
	*/

	/**
	@PageName console_keycode_t
	@PageTitle 	Key codes
	@PageFather console_input
	@PageDesc TCOD_keycode_t is a libtcod specific code representing a key on the keyboard.
		For Python, replace TCODK by KEY: libtcod.KEY_NONE. C# and Lua, the value is in parenthesis. Possible values are :
		When no key was pressed (see checkForEvent) : TCOD_NONE (NoKey)
		Special keys :
		TCODK_ESCAPE (Escape)
		TCODK_BACKSPACE (Backspace)
		TCODK_TAB (Tab)
		TCODK_ENTER (Enter)
		TCODK_SHIFT (Shift)
		TCODK_CONTROL (Control)
		TCODK_ALT (Alt)
		TCODK_PAUSE (Pause)
		TCODK_CAPSLOCK (CapsLock)
		TCODK_PAGEUP (PageUp)
		TCODK_PAGEDOWN (PageDown)
		TCODK_END (End)
		TCODK_HOME (Home)
		TCODK_UP (Up)
		TCODK_LEFT (Left)
		TCODK_RIGHT (Right)
		TCODK_DOWN (Down)
		TCODK_PRINTSCREEN (Printscreen)
		TCODK_INSERT (Insert)
		TCODK_DELETE (Delete)
		TCODK_LWIN (Lwin)
		TCODK_RWIN (Rwin)
		TCODK_APPS (Apps)
		TCODK_KPADD (KeypadAdd)
		TCODK_KPSUB (KeypadSubtract)
		TCODK_KPDIV (KeypadDivide)
		TCODK_KPMUL (KeypadMultiply)
		TCODK_KPDEC (KeypadDecimal)
		TCODK_KPENTER (KeypadEnter)
		TCODK_F1 (F1)
		TCODK_F2 (F2)
		TCODK_F3 (F3)
		TCODK_F4 (F4)
		TCODK_F5 (F5)
		TCODK_F6 (F6)
		TCODK_F7 (F7)
		TCODK_F8 (F8)
		TCODK_F9 (F9)
		TCODK_F10 (F10)
		TCODK_F11 (F11)
		TCODK_F12 (F12)
		TCODK_NUMLOCK (Numlock)
		TCODK_SCROLLLOCK (Scrolllock)
		TCODK_SPACE (Space)

		numeric keys :

		TCODK_0 (Zero)
		TCODK_1 (One)
		TCODK_2 (Two)
		TCODK_3 (Three)
		TCODK_4 (Four)
		TCODK_5 (Five)
		TCODK_6 (Six)
		TCODK_7 (Seven)
		TCODK_8 (Eight)
		TCODK_9 (Nine)
		TCODK_KP0 (KeypadZero)
		TCODK_KP1 (KeypadOne)
		TCODK_KP2 (KeypadTwo)
		TCODK_KP3 (KeypadThree)
		TCODK_KP4 (KeypadFour)
		TCODK_KP5 (KeypadFive)
		TCODK_KP6 (KeypadSix)
		TCODK_KP7 (KeypadSeven)
		TCODK_KP8 (KeypadEight)
		TCODK_KP9 (KeypadNine)

		Any other (printable) key :

		TCODK_CHAR (Char)

		Codes starting with TCODK_KP represents keys on the numeric keypad (if available).
	*/

	/**
	@PageName console_offscreen
	@PageFather console
	@PageTitle Using off-screen consoles
	@PageDesc The offscreen consoles allow you to draw on secondary consoles as you would do with the root console. You can then blit those secondary consoles on the root console. This allows you to use local coordinate space while rendering a portion of the final screen, and easily move components of the screen without modifying the rendering functions.
	@FuncTitle Creating an offscreen console
	@FuncDesc You can create as many off-screen consoles as you want by using this function. You can draw on them as you would do with the root console, but you cannot flush them to the screen. Else, you can blit them on other consoles, including the root console. See blit. The C version of this function returns a console handler that you can use in most console drawing functions.
	@Cpp TCODConsole::TCODConsole(int w, int h)
	@C TCOD_console_t TCOD_console_new(int w, int h)
	@Py console_new(w,h)
	@C# TCODConsole::TCODConsole(int w, int h)
	@Lua tcod.Console(w,h)
	@Param w,h the console size.
		0 < w
		0 < h
	@CppEx
		// Creating a 40x20 offscreen console, filling it with red and blitting it on the root console at position 5,5
		TCODConsole *offscreenConsole = new TCODConsole(40,20);
		offscreenConsole->setDefaultBackground(TCODColor::red);
		offscreenConsole->clear();
		TCODConsole::blit(offscreenConsole,0,0,40,20,TCODConsole::root,5,5,255);
	@CEx
		TCOD_console_t offscreen_console = TCOD_console_new(40,20);
		TCOD_console_set_default_background(offscreen_console,TCOD_red);
		TCOD_console_clear(offscreen_console);
		TCOD_console_blit(offscreen_console,0,0,40,20,NULL,5,5,255);
	@PyEx
		offscreen_console = libtcod.console_new(40,20)
		libtcod.console_set_background_color(offscreen_console,libtcod.red)
		libtcod.console_clear(offscreen_console)
		libtcod.console_blit(offscreen_console,0,0,40,20,0,5,5,255)
	@LuaEx
		-- Creating a 40x20 offscreen console, filling it with red and blitting it on the root console at position 5,5
		offscreenConsole = tcod.Console(40,20)
		offscreenConsole:setBackgroundColor(tcod.color.red)
		offscreenConsole:clear()
		tcod.console.blit(offscreenConsole,0,0,40,20,libtcod.TCODConsole_root,5,5,255)
	*/
	TCODConsole(int w, int h);

	/**
	@PageName console_offscreen
	@FuncTitle Creating an offscreen console from a .asc or .apf file
	@FuncDesc You can create an offscreen console from a file created with Ascii Paint with this constructor
	@Cpp TCODConsole::TCODConsole(const char *filename)
	@C TCOD_console_t TCOD_console_from_file(const char *filename)
	@Param filename path to the .asc or .apf file created with Ascii Paint
	@CppEx
		// Creating an offscreen console, filling it with data from the .asc file
		TCODConsole *offscreenConsole = new TCODConsole("myfile.asc");
	@CEx
		TCOD_console_t offscreen_console = TCOD_console_from_file("myfile.apf");
	*/
	TCODConsole(const char *filename);

	/**
	@PageName console_offscreen
	@FuncTitle Loading an offscreen console from a .asc file
	@FuncDesc You can load data from a file created with Ascii Paint with this function. When needed, the console will be resized to fit the file size. The function returns false if it couldn't read the file.
	@Cpp bool TCODConsole::loadAsc(const char *filename)
	@C bool TCOD_console_load_asc(TCOD_console_t con, const char *filename)
	@Param con in the C and Python versions, the offscreen console handler
	@Param filename path to the .asc file created with Ascii Paint
	@CppEx
		// Creating a 40x20 offscreen console
		TCODConsole *offscreenConsole = new TCODConsole(40,20);
		// possibly resizing it and filling it with data from the .asc file
		offscreenConsole->loadAsc("myfile.asc");
	@CEx
		TCOD_console_t offscreen_console = TCOD_console_new(40,20);
		TCOD_console_load_asc(offscreen_console,"myfile.asc");
	*/
	bool loadAsc(const char *filename);
	/**
	@PageName console_offscreen
	@FuncTitle Loading an offscreen console from a .apf file
	@FuncDesc You can load data from a file created with Ascii Paint with this function. When needed, the console will be resized to fit the file size. The function returns false if it couldn't read the file.
	@Cpp bool TCODConsole::loadApf(const char *filename)
	@C bool TCOD_console_load_apf(TCOD_console_t con, const char *filename)
	@Param con in the C and Python versions, the offscreen console handler

	@Param filename path to the .apf file created with Ascii Paint

	@CppEx
		// Creating a 40x20 offscreen console
		TCODConsole *offscreenConsole = new TCODConsole(40,20);
		// possibly resizing it and filling it with data from the .apf file
		offscreenConsole->loadApf("myfile.apf");
	@CEx
		TCOD_console_t offscreen_console = TCOD_console_new(40,20);
		TCOD_console_load_apf(offscreen_console,"myfile.asc");
	*/
	bool loadApf(const char *filename);

    /**
	@PageName console_offscreen
	@FuncTitle Saving a console to a .asc file
	@FuncDesc You can save data from a console to Ascii Paint format with this function. The function returns false if it couldn't write the file. This is the only ASC function that works also with the root console !
	@Cpp bool TCODConsole::saveAsc(const char *filename) const
	@C bool TCOD_console_save_asc(TCOD_console_t con, const char *filename)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param filename path to the .asc file to be created
	@CppEx
		console->saveAsc("myfile.asc");
	@CEx
		TCOD_console_save_asc(console,"myfile.asc");
	*/
	bool saveAsc(const char *filename) const;

    /**
	@PageName console_offscreen
	@FuncTitle Saving a console to a .apf file
	@FuncDesc You can save data from a console to Ascii Paint format with this function. The function returns false if it couldn't write the file. This is the only ASC function that works also with the root console !
	@Cpp bool TCODConsole::saveApf(const char *filename) const
	@C bool TCOD_console_save_apf(TCOD_console_t con, const char *filename)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param filename path to the .apf file to be created
	@CppEx
		console->saveApf("myfile.apf");
	@CEx
		TCOD_console_save_apf(console,"myfile.apf");
	*/
	bool saveApf(const char *filename) const;

	bool loadXp(const char *filename) {
		return TCOD_console_load_xp(data, filename) != 0;
	}
	bool saveXp(const char *filename, int compress_level) {
		return TCOD_console_save_xp(data, filename, compress_level) != 0;
	}

	/**
	@PageName console_offscreen
	@FuncTitle Blitting a console on another one
	@FuncDesc This function allows you to blit a rectangular area of the source console at a specific position on a destination console. It can also simulate alpha transparency with the fade parameter.
	@Cpp static void blit(const TCODConsole *src,int xSrc, int ySrc, int wSrc, int hSrc, TCODConsole *dst, int xDst, int yDst, float foregroundAlpha=1.0f, float backgroundAlpha=1.0f)
	@C void TCOD_console_blit(TCOD_console_t src,int xSrc, int ySrc, int wSrc, int hSrc, TCOD_console_t dst, int xDst, int yDst, float foreground_alpha, float background_alpha)
	@Py console_blit(src,xSrc,ySrc,xSrc,hSrc,dst,xDst,yDst,foregroundAlpha=1.0,backgroundAlpha=1.0)
	@C#
		static void TCODConsole::blit(TCODConsole src, int xSrc, int ySrc, int wSrc, int hSrc, TCODConsole dst, int xDst, int yDst)
		static void TCODConsole::blit(TCODConsole src, int xSrc, int ySrc, int wSrc, int hSrc, TCODConsole dst, int xDst, int yDst, float foreground_alpha)
		static void TCODConsole::blit(TCODConsole src, int xSrc, int ySrc, int wSrc, int hSrc, TCODConsole dst, int xDst, int yDst, float foreground_alpha, float background_alpha)
	@Lua
		tcod.console.blit(src, xSrc, ySrc, wSrc, hSrc, dst, xDst, yDst)
		tcod.console.blit(src, xSrc, ySrc, wSrc, hSrc, dst, xDst, yDst, foreground_alpha)
		tcod.console.blit(src, xSrc, ySrc, wSrc, hSrc, dst, xDst, yDst, foreground_alpha, background_alpha)
	@Param src The source console that must be blitted on another one.
	@Param xSrc,ySrc,wSrc,hSrc The rectangular area of the source console that will be blitted. If wSrc and/or hSrc == 0, the source console width/height are used
	@Param dst The destination console.
	@Param xDst,yDst Where to blit the upper-left corner of the source area in the destination console.
	@Param foregroundAlpha,backgroundAlpha Alpha transparency of the blitted console.
		0.0 => The source console is completely transparent. This function does nothing.
		1.0 => The source console is opaque. Its cells replace the destination cells.
		0 < fade < 1.0 => The source console is partially blitted, simulating real transparency.
	@CppEx
		// Cross-fading between two offscreen consoles. We use two offscreen consoles with the same size as the root console. We render a different screen on each offscreen console. When the user hits a key, we do a cross-fading from the first screen to the second screen.
		TCODConsole *off1 = new TCODConsole(80,50);
		TCODConsole *off2 = new TCODConsole(80,50);
		... print screen1 on off1
		... print screen2 of off2
		// render screen1 in the game window
		TCODConsole::blit(off1,0,0,80,50,TCODConsole::root,0,0);
		TCODConsole::flush();
		// wait or a keypress
		TCODConsole::waitForKeypress(true);
		// do a cross-fading from off1 to off2
		for (int i=1; i <= 255; i++) {
			TCODConsole::blit(off1,0,0,80,50,TCODConsole::root,0,0); // renders the first screen (opaque)
			TCODConsole::blit(off2,0,0,80,50,TCODConsole::root,0,0,i/255.0,i/255.0); // renders the second screen (transparent)
			TCODConsole::flush();
		}
	@CEx
		TCOD_console_t off1 = TCOD_console_new(80,50);
		TCOD_console_t off2 = TCOD_console_new(80,50);
		int i;
		... print screen1 on off1
		... print screen2 of off2
		// render screen1 in the game window
		TCOD_console_blit(off1,0,0,80,50,NULL,0,0,1.0,1.0);
		TCOD_console_flush();
		// wait or a keypress
		TCOD_console_wait_for_keypress(true);
		// do a cross-fading from off1 to off2
		for (i=1; i <= 255; i++) {
			TCOD_console_blit(off1,0,0,80,50,NULL,0,0,1.0,1.0); // renders the first screen (opaque)
			TCOD_console_blit(off2,0,0,80,50,NULL,0,0,i/255.0,i/255.0); // renders the second screen (transparent)
			TCOD_console_flush();
		}
	@PyEx
		off1 = libtcod.console_new(80,50)
		off2 = libtcod.console_new(80,50)
		... print screen1 on off1
		... print screen2 of off2
		# render screen1 in the game window
		libtcod.console_blit(off1,0,0,80,50,0,0,0)
		libtcod.console_flush()
		# wait or a keypress
		libtcod.console_wait_for_keypress(True)
		# do a cross-fading from off1 to off2
		for i in range(1,256) :
			litbcod.console_blit(off1,0,0,80,50,0,0,0) # renders the first screen (opaque)
			litbcod.console_blit(off2,0,0,80,50,0,0,0,i/255.0,i/255.0) # renders the second screen (transparent)
			litbcod.console_flush()
	@LuaEx
		-- Cross-fading between two offscreen consoles. We use two offscreen consoles with the same size as the root console. We render a different screen on each offscreen console. When the user hits a key, we do a cross-fading from the first screen to the second screen.
		off1 = tcod.Console(80,50)
		off2 = tcod.Console(80,50)
		... print screen1 on off1
		... print screen2 of off2
		-- render screen1 in the game window
		root=libtcod.TCODConsole_root
		tcod.console.blit(off1,0,0,80,50,root,0,0)
		tcod.console.flush()
		-- wait or a keypress
		tcod.console.waitForKeypress(true)
		-- do a cross-fading from off1 to off2
		for i=1,255,1 do
			tcod.console.blit(off1,0,0,80,50,root,0,0) -- renders the first screen (opaque)
			tcod.console.blit(off2,0,0,80,50,root,0,0,i/255,i/255) -- renders the second screen (transparent)
			tcod.console.flush()
		end
	*/
	static void blit(const TCODConsole *src,int xSrc, int ySrc, int wSrc, int hSrc, TCODConsole *dst, int xDst, int yDst, float foreground_alpha=1.0f, float background_alpha=1.0f);
	/**
	@PageName console_offscreen
	@FuncTitle Define a blit-transparent color
	@FuncDesc This function defines a transparent background color for an offscreen console. All cells with this background color are ignored by the blit operation. You can use it to blit only some parts of the console.
	@Cpp void TCODConsole::setKeyColor(const TCODColor &col)
	@C void TCOD_console_set_key_color(TCOD_console_t con,TCOD_color_t col)
	@Py console_set_key_color(con,col)
	@C# void TCODConsole::setKeyColor(TCODColor col)
	@Lua Console:setKeyColor(col)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param col the transparent background color
	*/
	void setKeyColor(const TCODColor &col);
	/**
	@PageName console_offscreen
	@FuncTitle Destroying an offscreen console
	@FuncDesc Use this function to destroy an offscreen console and release any resources allocated. Don't use it on the root console.
	@Cpp TCODConsole::~TCODConsole()
	@C void TCOD_console_delete(TCOD_console_t con)
	@Py console_delete(con)
	@C# void TCODConsole::Dispose()
	@Lua through garbage collector
	@Param con in the C and Python versions, the offscreen console handler
	@CppEx
		TCODConsole *off1 = new TCODConsole(80,50);
		... use off1
		delete off1; // destroy the offscreen console
	@CEx
		TCOD_console_t off1 = TCOD_console_new(80,50);
		... use off1
		TCOD_console_delete(off1); // destroy the offscreen console
	@PyEx
		off1 = libtcod.console_new(80,50)
		... use off1
		libtcod.console_delete(off1) # destroy the offscreen console
	@LuaEx
		off1 = tcod.Console(80,50)
		... use off1
		off1=nil -- release the reference
	*/
  TCOD_DEPRECATED("This function is a stub and will do nothing.")
  static void setKeyboardRepeat(int initialDelay,int interval);
  TCOD_DEPRECATED("This function is a stub and will do nothing.")
  static void disableKeyboardRepeat();

	virtual ~TCODConsole();

	void setDirty(int x, int y, int w, int h);


	TCODConsole(TCOD_console_t con) : data(con) {}

    // ctrl = TCOD_COLCTRL_1...TCOD_COLCTRL_5 or TCOD_COLCTRL_STOP
	static const char *getColorControlString( TCOD_colctrl_t ctrl );
	// ctrl = TCOD_COLCTRL_FORE_RGB or TCOD_COLCTRL_BACK_RGB
	static const char *getRGBColorControlString( TCOD_colctrl_t ctrl, const TCODColor & col );

protected :
	friend class TCODImage;
	friend class TCODZip;
	friend class TCODText;
  friend TCODLIB_API void tcod::console::init_root(
      int w, int h, const std::string& title,
      bool fullscreen, TCOD_renderer_t renderer, bool vsync);
	TCODConsole();
	TCOD_Console* data;
};

#endif /* TCOD_CONSOLE_SUPPORT */

#endif /* _TCOD_CONSOLE_HPP */
