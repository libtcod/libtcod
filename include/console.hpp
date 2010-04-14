/*
* libtcod 1.5.1
* Copyright (c) 2008,2009,2010 Jice & Mingos
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

#ifndef _TCOD_CONSOLE_HPP
#define _TCOD_CONSOLE_HPP

#include "console_types.h"

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
			// ... draw on TCODConsole::root
			TCODConsole::flush();
			TCOD_key_t key = TCODConsole::checkForKeypress();
			updateWorld (key, TCODSystem::getLastFrameLength());
			// updateWorld(TCOD_key_t key, float elapsed) (using key if key.vk != TCODK_NONE)
			// use elapsed to scale any update that is time dependant.
		}
*/
/**
	@PageName console
	@FuncDesc Classic turn by turn game loop:
	@Cpp 
		TCODConsole::initRoot(80,50,"my game",false);
		while (!endGame && !TCODConsole::isWindowClosed()) {
			// ... draw on TCODConsole::root
			TCODConsole::flush();
			TCOD_key_t key = TCODConsole::waitForKeypress(true);
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
	@Cpp static void TCODConsole::initRoot (int w, int h, const char * title, bool fullscreen = false, TCOD_renderer_t renderer = TCOD_RENDERER_GLSL)
	@C void TCOD_console_init_root (int w, int h, const char * title, bool fullscreen, TCOD_renderer_t renderer)
	@Py console_init_root (w, h, title, fullscreen = False, renderer = RENDERER_GLSL)
	@Param w,h size of the console(in characters). The default font in libtcod (./terminal.png) uses 8x8 pixels characters.
		You can change the font by calling TCODConsole::setCustomFont before calling initRoot.
	@Param title title of the window. It's not visible when you are in fullscreen.
		Note 1 : you can dynamically change the window title with TCODConsole::setWindowTitle
	@Param fullscreen wether you start in windowed or fullscreen mode.
		Note 1 : you can dynamically change this mode with TCODConsole::setFullscreen
		Note 2 : you can get current mode with TCODConsole::isFullscreen
	@Param renderer which renderer to use. Possible values are :
    * TCOD_RENDERER_GLSL : works only on video cards with pixel shaders
    * TCOD_RENDERER_OPENGL : works on all video cards supporting OpenGL 1.4
    * TCOD_RENDERER_SDL : should work everywhere!
		Note 1: if you select a renderer that is not supported by the player's machine, libtcod scan the lower renderers until it finds a working one.
		Note 2: on recent video cards, GLSL results in up to 900% increase of framerates in the true color sample compared to SDL renderer.
		Note 3: whatever renderer you use, it can always be overriden by the player through the libtcod.cfg file.
		Note 4: you can dynamically change the renderer after calling initRoot with TCODSystem::setRenderer.
		Note 5: you can get current renderer with TCODSystem::getRenderer. It might be different from the one you set in initRoot in case it's not supported on the player's computer.
	@CppEx TCODConsole::initRoot(80, 50, "The Chronicles Of Doryen v0.1");
	@CEx TCOD_console_init_root(80, 50, "The Chronicles Of Doryen v0.1", false, TCOD_RENDERER_SDL);
	@PyEx libtcod.console_init_root(80, 50, 'The Chronicles Of Doryen v0.1')
	*/	
	static void initRoot(int w, int h, const char * title, bool fullscreen = false, TCOD_renderer_t renderer=TCOD_RENDERER_GLSL);

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
		
		<tr><td><img src='terminal.png' /></td><td><img src='terminal8x8_aa_as.png' /></td><td><img src='terminal8x8_gs_as.png' /></td></tr>
		</table>
		<ul>
		<li>standard : transparency is given by a key color automatically detected by looking at the color of the space character</li>
		<li>32 bits : transparency is given by the png alpha layer. The font color does not matter</li>
		<li>greyscale : transparency is given by the pixel value. You can use white characters on black background or black characters on white background. The background color is automatically detected by looking at the color of the space character</li>
		</ul>
		Examples of fonts can be found in libtcod's fonts directory. Check the Readme file there.
	@Cpp static void TCODConsole::setCustomFont(const char *fontFile, int flags=TCOD_FONT_LAYOUT_ASCII_INCOL,int nbCharHoriz=0, int nbCharVertic=0)
	@C void TCOD_console_set_custom_font(const char *fontFile, int flags,int nb_char_horiz, int nb_char_vertic)
	@Py console_set_custom_font(fontFile, flags=FONT_LAYOUT_ASCII_INCOL,nb_char_horiz=0, nb_char_vertic=0)
	@Param fontFile Name of a .bmp or .png file containing the font.
	@Param flags Used to define the characters layout in the bitmap and the font type :
		TCOD_FONT_LAYOUT_ASCII_INCOL : characters in ASCII order, code 0-15 in the first column
		TCOD_FONT_LAYOUT_ASCII_INROW : characters in ASCII order, code 0-15 in the first row
		TCOD_FONT_LAYOUT_TCOD : simplified layout. See examples below.
		TCOD_FONT_TYPE_GREYSCALE : create an anti-aliased font from a greyscale bitmap
		For python, remove TCOD _ :
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
	@Param fullscreen true to switch to fullscreen mode.
		false to switch to windowed mode.
	@CppEx 
		TCOD_key_t key=TCODConsole::checkForKeypress();
		if ( key.vk == TCODK_ENTER && key.lalt ) 
			TCODConsole::setFullscreen(!TCODConsole::isFullscreen());
	@CEx 
		TCOD_key_t key=TCOD_console_check_for_keypress();
		if ( key.vk == TCODK_ENTER && key.lalt ) 
			TCOD_console_set_fullscreen(!TCOD_console_is_fullscreen());
	@PyEx 
		key=libtcod.console_check_for_keypress()
		if key.vk == libtcod.KEY_ENTER and key.lalt : 
			libtcod.console_set_fullscreen(not libtcod.console_is_fullscreen())
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
	@Param title New title of the game window
	*/
	static void setWindowTitle(const char *title);

	/**
	@PageName console_window
	@FuncTitle Handling "close window" events
	@FuncDesc When you start the program, this returns false. Once a "close window" event has been sent by the window manager, it will allways return true. You're supposed to exit cleanly the game.
	@Cpp static bool TCODConsole::isWindowClosed()
	@C bool TCOD_console_is_window_closed()
	@Py console_is_window_closed()
	*/
	static bool isWindowClosed();

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
	*/
	static bool renderCredits(int x, int y, bool alpha);
	
	/**
	@PageName console_credits
	@FuncTitle Restart the credits animation
	@FuncDesc When using rederCredits, you can restart the credits animation from the begining before it's finished by calling this function.
	@Cpp static void TCODConsole::resetCredits()
	@C void TCOD_console_credits_reset()
	@Py console_credits_reset()
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
	@Cpp void TCODConsole::setBackgroundColor(TCODColor back)
	@C void TCOD_console_set_background_color(TCOD_console_t con,TCOD_color_t back)
	@Py console_set_background_color(con,back)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param back the new default background color for this console
	@CppEx TCODConsole::root->setBackgroundColor(myColor)
	@CEx TCOD_console_set_background_color(NULL, my_color)
	@PyEx litbcod.console_set_background_color(0, my_color)
	*/
	void setBackgroundColor(TCODColor back);

	/**
	@PageName console_draw_basic
	@FuncTitle Setting the default foreground color
	@FuncDesc This function changes the default foreground color for a console. The default foreground color is used by several drawing functions like clear, putChar, ...
	@Cpp void TCODConsole::setForegroundColor(TCODColor back)
	@C void TCOD_console_set_foreground_color(TCOD_console_t con,TCOD_color_t back)
	@Py console_set_foreground_color(con,back)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param back the new default foreground color for this console
	@CppEx TCODConsole::root->setForegroundColor(myColor)
	@CEx TCOD_console_set_foreground_color(NULL, my_color)
	@PyEx litbcod.console_set_foreground_color(0, my_color)
	*/
	void setForegroundColor(TCODColor fore);

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
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	*/
	void clear();

	/**
	@PageName console_draw_basic
	@FuncTitle Setting the background color of a cell
	@FuncDesc This function modifies the background color of a cell, leaving other properties (foreground color and ASCII code) unchanged.
	@Cpp void TCODConsole::setBack(int x, int y, const TCODColor &col, TCOD_bkgnd_flag_t flag = TCOD_BKGND_DEFAULT)
	@C void TCOD_console_set_back(TCOD_console_t con,int x, int y, TCOD_color_t col, TCOD_bkgnd_flag_t flag)
	@Py console_set_back(con, x,  y, col, flag=BKGND_DEFAULT)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinates of the cell in the console.
		0 <= x < console width
		0 <= y < console height
	@Param col the background color to use. You can use color constants
	@Param flag this flag defines how the cell's background color is modified. See <a href="console_bkgnd_flag_t.html">TCOD_bkgnd_flag_t</a>
	*/
	void setBack(int x, int y, const TCODColor &col, TCOD_bkgnd_flag_t flag = TCOD_BKGND_DEFAULT);
	/**
	@PageName console_draw_basic
	@FuncTitle Setting the foreground color of a cell
	@FuncDesc This function modifies the foreground color of a cell, leaving other properties (background color and ASCII code) unchanged.
	@Cpp void TCODConsole::setFore(int x, int y, const TCODColor &col)
	@C void TCOD_console_set_fore(TCOD_console_t con,int x, int y, TCOD_color_t col)
	@Py console_set_fore(con, x, y, col)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinates of the cell in the console.
		0 <= x < console width
		0 <= y < console height
	@Param col the foreground color to use. You can use color constants
	*/
	void setFore(int x, int y, const TCODColor &col);

	/**
	@PageName console_draw_basic
	@FuncTitle Setting the ASCII code of a cell
	@FuncDesc This function modifies the ASCII code of a cell, leaving other properties (background and foreground colors) unchanged.
		Note that since a clear console has both background and foreground colors set to black for every cell, using setchar will produce black characters on black background. Use putchar instead.
	@Cpp void TCODConsole::setChar(int x, int y, int c)
	@C void TCOD_console_set_char(TCOD_console_t con,int x, int y, int c)
	@Py console_set_char(con, x,  y, c)
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
		For python, remove TCOD_ : libtcod.BKGND_NONE 
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
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	*/
	TCOD_bkgnd_flag_t getBackgroundFlag() const;

	/**
	@PageName console_print
	@FuncTitle Setting the default alignment
	@FuncDesc This function defines the default alignment (see TCOD_alignment_t) for the console.
		This default alignment is used by several functions (print, printRect, ...).
		Values for alignment : TCOD_LEFT, TCOD_CENTER, TCOD_RIGHT (in python, remove TCOD_ : libtcod.LEFT).
	@Cpp void TCODConsole::setAlignment(TCOD_alignment_t alignment)
	@C void TCOD_console_set_alignment(TCOD_console_t con,TCOD_bkgnd_flag_t alignment)
	@Py console_set_alignment(con, alignment)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param alignment defines how the strings are printed on screen.	
	*/
	void setAlignment(TCOD_alignment_t alignment);

	/**
	@PageName console_print
	@FuncTitle Getting the default alignment
	@FuncDesc This function returns the default alignment (see TCOD_alignment_t) for the console.
		This default mode is used by several functions (print, printRect, ...).
		Values for alignment : TCOD_LEFT, TCOD_CENTER, TCOD_RIGHT (in python, remove TCOD_ : libtcod.LEFT).
	@Cpp TCOD_alignment_t TCODConsole::getAlignment() const
	@C TCOD_alignment_t TCOD_console_get_alignment(TCOD_console_t con)
	@Py console_get_alignment(con)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	*/
	TCOD_alignment_t getAlignment() const;	         

	/**
	@PageName console_print
	@FuncTitle Printing a string with default parameters
	@FuncDesc This function print a string at a specific position using current default alignment, background flag, foreground and background colors.
	@Cpp void TCODConsole::print(int x, int y, const char *fmt, ...)
	@C void TCOD_console_print(TCOD_console_t con,int x, int y, const char *fmt, ...)
	@Py console_print(con, x, y, fmt)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinate of the character in the console, depending on the alignment :
		* TCOD_LEFT : leftmost character of the string
		* TCOD_CENTER : center character of the string
		* TCOD_RIGHT : rightmost character of the string
	@Param fmt printf-like format string, eventually followed by parameters. You can use control codes to change the colors inside the string
	*/
	void print(int x, int y, const char *fmt, ...); 

	/**
	@PageName console_print
	@FuncTitle Printing a string with specific alignment and background mode
	@FuncDesc This function print a string at a specific position using specific alignment and background flag, but default foreground and background colors.
	@Cpp void TCODConsole::printEx(int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char *fmt, ...)
	@C void TCOD_console_print_ex(TCOD_console_t con,int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char *fmt, ...)
	@Py console_print_ex(con, x, y, flag, alignment, fmt)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinate of the character in the console, depending on the alignment :
		* TCOD_LEFT : leftmost character of the string
		* TCOD_CENTER : center character of the string
		* TCOD_RIGHT : rightmost character of the string
	@Param flag this flag defines how the cell's background color is modified. See TCOD_bkgnd_flag_t	
	@Param alignment defines how the strings are printed on screen.	
	@Param fmt printf-like format string, eventually followed by parameters. You can use control codes to change the colors inside the string
	*/
	void printEx(int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char *fmt, ...); 

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
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinate of the rectangle upper-left corner in the console
	@Param w,h size of the rectangle
		x <= x+w < console width
		y <= y+h < console height
	@Param fmt printf-like format string, eventually followed by parameters. You can use control codes to change the colors inside the string
	*/
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
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinate of the rectangle upper-left corner in the console
	@Param w,h size of the rectangle
		x <= x+w < console width
		y <= y+h < console height
	@Param flag this flag defines how the cell's background color is modified. See TCOD_bkgnd_flag_t	
	@Param alignment defines how the strings are printed on screen.	
	@Param fmt printf-like format string, eventually followed by parameters. You can use control codes to change the colors inside the string
	*/
	int printRectEx(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char *fmt, ...); 

	/**
	@PageName console_print
	@FuncTitle Compute the height of an autowrapped string
	@FuncDesc This function returns the expected height of an autowrapped string without actually printing the string with printRect or printRectEx
	@Cpp int TCODConsole::getHeightRect(int x, int y, int w, int h, const char *fmt, ...)
	@C int TCOD_console_get_height_rect(TCOD_console_t con,int x, int y, int w, int h, const char *fmt, ...)
	@Py console_get_height_rect(con, x, y, w, h, fmt)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinate of the rectangle upper-left corner in the console
	@Param w,h size of the rectangle
		x <= x+w < console width
		y <= y+h < console height
	@Param fmt printf-like format string, eventually followed by parameters. You can use control codes to change the colors inside the string
	*/
	int getHeightRect(int x, int y, int w, int h, const char *fmt, ...); 

	/**
	@PageName console_print
	@FuncTitle Changing the colors while printing a string
	@FuncDesc If you want to draw a string using different colors for each word, the basic solution is to call a string printing function several times, changing the default colors between each call.
		The TCOD library offers a simpler way to do this, allowing you to draw a string using different colors in a single call. For this, you have to insert color control codes in your string.
		A color control code is associated with a color set (a foreground color and a background color). If you insert this code in your string, the next characters will use the colors associated with the color control code.
		There are 5 predefined color control codes :
		For python, remove TCOD_ : libtcod.COLCTRL_1 
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
	*/
	static void setColorControl(TCOD_colctrl_t con, const TCODColor &fore, const TCODColor &back);

#ifndef NO_UNICODE
	/**
	@PageName console_print
	@FuncTitle Unicode functions
	@FuncDesc those functions are similar to their ASCII equivalent, but work with unicode strings (wchar_t in C/C++). 
		Note that unicode is not supported in the python wrapper.
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
	@Param fmt if NULL, the funtion only draws a rectangle.
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
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	*/
	int getHeight() const;

	/**
	@PageName console_read
	@FuncTitle Reading the default background color
	@FuncDesc This function returns the default background color of a console. 
	@Cpp TCODColor TCODConsole::getBackgroundColor() const
	@C TCOD_color_t TCOD_console_get_background_color(TCOD_console_t con)
	@Py console_get_background_color(con)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	*/
	TCODColor getBackgroundColor() const;

	/**
	@PageName console_read
	@FuncTitle Reading the default foreground color
	@FuncDesc This function returns the default foreground color of a console. 
	@Cpp TCODColor TCODConsole::getForegroundColor() const
	@C TCOD_color_t TCOD_console_get_foreground_color(TCOD_console_t con)
	@Py console_get_foreground_color(con)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	*/
	TCODColor getForegroundColor() const;

	/**
	@PageName console_read
	@FuncTitle Reading the background color of a cell
	@FuncDesc This function returns the background color of a cell. 
	@Cpp TCODColor TCODConsole::getBack(int x, int y) const
	@C TCOD_color_t TCOD_console_get_back(TCOD_console_t con,int x, int y)
	@Py console_get_back(con,x,y)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinates of the cell in the console.
		0 <= x < console width
		0 <= y < console height
	*/
	TCODColor getBack(int x, int y) const;

	/**
	@PageName console_read
	@FuncTitle Reading the foreground color of a cell
	@FuncDesc This function returns the foreground color of a cell. 
	@Cpp TCODColor TCODConsole::getFore(int x, int y) const
	@C TCOD_color_t TCOD_console_get_fore(TCOD_console_t con,int x, int y)
	@Py console_get_fore(con,x,y)
	@Param con in the C and Python versions, the offscreen console handler or NULL for the root console
	@Param x,y coordinates of the cell in the console.
		0 <= x < console width
		0 <= y < console height
	*/
	TCODColor getFore(int x, int y) const;

	/**
	@PageName console_read
	@FuncTitle Reading the ASCII code of a cell
	@FuncDesc This function returns the ASCII code of a cell. 
	@Cpp TCODColor TCODConsole::getChar(int x, int y) const
	@C TCOD_color_t TCOD_console_get_char(TCOD_console_t con,int x, int y)
	@Py console_get_char(con,x,y)
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
	@Cpp static void TCODConsole::setFade(uint8 fade, const TCODColor &fadingColor)
	@C void TCOD_console_set_fade(uint8 fade, TCOD_color_t fadingColor)
	@Py console_set_fade(fade, fadingColor)
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
	*/
	static void setFade(uint8 fade, const TCODColor &fadingColor);

	/**
	@PageName console_fading
	@FuncTitle Reading the fade amount
	@FuncDesc This function returns the current fade amount, previously defined by setFade.
	@Cpp static uint8 TCODConsole::getFade()
	@C uint8 TCOD_console_get_fade()
	@Py console_get_fade()
	*/
	static uint8 getFade();

	/**
	@PageName console_fading
	@FuncTitle Reading the fading color
	@FuncDesc This function returns the current fading color, previously defined by setFade.
	@Cpp static TCODColor TCODConsole::getFadingColor()
	@C TCODColor TCOD_console_get_fadingColor()
	@Py console_get_fading_color()
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
	*/
	static void flush();

	/**
	@PageName console_ascii
	@PageTitle ASCII constants
	@PageFather console_draw
	@FuncDesc Some useful graphic characters in the terminal.bmp font. For the python version, remove TCOD_ from the constants:
		Single line walls:
		TCOD_CHAR_HLINE=196
		TCOD_CHAR_VLINE=179
		TCOD_CHAR_NE=191
		TCOD_CHAR_NW=218
		TCOD_CHAR_SE=217
		TCOD_CHAR_SW=192
		
		Double lines walls:
		TCOD_CHAR_DHLINE=205
		TCOD_CHAR_DVLINE=186
		TCOD_CHAR_DNE=187
		TCOD_CHAR_DNW=201
		TCOD_CHAR_DSE=188
		TCOD_CHAR_DSW=200
		
		Single line vertical/horizontal junctions (T junctions):
		TCOD_CHAR_TEEW=180
		TCOD_CHAR_TEEE=195
		TCOD_CHAR_TEEN=193
		TCOD_CHAR_TEES=194
		
		Double line vertical/horizontal junctions (T junctions):
		TCOD_CHAR_DTEEW=185
		TCOD_CHAR_DTEEE=204
		TCOD_CHAR_DTEEN=202
		TCOD_CHAR_DTEES=203
		
		Block characters:
		TCOD_CHAR_BLOCK1=176
		TCOD_CHAR_BLOCK2=177
		TCOD_CHAR_BLOCK3=178
		
		Cross-junction between two single line walls:
		TCOD_CHAR_CROSS=197
		
		Arrows:
		TCOD_CHAR_ARROW_N=24
		TCOD_CHAR_ARROW_S=25
		TCOD_CHAR_ARROW_E=26
		TCOD_CHAR_ARROW_W=27
		
		Arrows without tail:
		TCOD_CHAR_ARROW2_N=30
		TCOD_CHAR_ARROW2_S=31
		TCOD_CHAR_ARROW2_E=16
		TCOD_CHAR_ARROW2_W=17
		
		Double arrows:
		TCOD_CHAR_DARROW_H=29
		TCOD_CHAR_ARROW_V=18
		
		GUI stuff:
		TCOD_CHAR_CHECKBOX_UNSET=224
		TCOD_CHAR_CHECKBOX_SET=225
		TCOD_CHAR_RADIO_UNSET=9
		TCOD_CHAR_RADIO_SET=10
		
		Sub-pixel resolution kit:
		TCOD_CHAR_SUBP_NW=226
		TCOD_CHAR_SUBP_NE=227
		TCOD_CHAR_SUBP_N=228
		TCOD_CHAR_SUBP_SE=229
		TCOD_CHAR_SUBP_DIAG=230
		TCOD_CHAR_SUBP_E=231
		TCOD_CHAR_SUBP_SW=232
		
		Miscellaneous characters:
		TCOD_CHAR_SMILY = 1
		TCOD_CHAR_SMILY_INV = 2
		TCOD_CHAR_HEART = 3
		TCOD_CHAR_DIAMOND = 4
		TCOD_CHAR_CLUB = 5
		TCOD_CHAR_SPADE = 6
		TCOD_CHAR_BULLET = 7
		TCOD_CHAR_BULLET_INV = 8
		TCOD_CHAR_MALE = 11
		TCOD_CHAR_FEMALE = 12
		TCOD_CHAR_NOTE = 13
		TCOD_CHAR_NOTE_DOUBLE = 14
		TCOD_CHAR_LIGHT = 15
		TCOD_CHAR_EXCLAM_DOUBLE = 19
		TCOD_CHAR_PILCROW = 20
		TCOD_CHAR_SECTION = 21
		TCOD_CHAR_POUND = 156
		TCOD_CHAR_MULTIPLICATION = 158
		TCOD_CHAR_FUNCTION = 159
		TCOD_CHAR_RESERVED = 169
		TCOD_CHAR_HALF = 171
		TCOD_CHAR_ONE_QUARTER = 172
		TCOD_CHAR_COPYRIGHT = 184
		TCOD_CHAR_CENT = 189
		TCOD_CHAR_YEN = 190
		TCOD_CHAR_CURRENCY = 207
		TCOD_CHAR_THREE_QUARTERS = 243
		TCOD_CHAR_DIVISION = 246
		TCOD_CHAR_GRADE = 248
		TCOD_CHAR_UMLAUT = 249
		TCOD_CHAR_POW1 = 251
		TCOD_CHAR_POW3 = 252
		TCOD_CHAR_POW2 = 253
		TCOD_CHAR_BULLET_SQUARE = 254
	*/

	/**
	@PageName console_input
	@PageTitle Handling keyboard input
	@PageDesc The keyboard handling functions allow you to get keyboard input from the user, either for turn by turn games (the function wait until the user press a key), or real time games (non blocking function).
	<b>WARNING : for proper redraw event handling, the keyboard functions should always be called just after TCODConsole::flush !</b>
	@PageFather console
	*/

	/**
	@PageName console_blocking_input
	@PageTitle Blocking keyboard input
	@PageFather console_input
	@FuncDesc This function waits for the user to press a key. It returns the code of the key pressed as well as the corresponding character. See TCOD_key_t.
		If the flush parameter is true, every pending keypress event is discarded, then the function wait for a new keypress.
		If flush is false, the function waits only if there are no pending keypress events, else it returns the first event in the keyboard buffer.
	@Cpp static TCOD_key_t TCODConsole::waitForKeypress(bool flush)
	@C TCOD_key_t TCOD_console_wait_for_keypress(bool flush)
	@Py console_wait_for_keypress(flush)
	@Param flush if true, all pending keypress events are flushed from the keyboard buffer. Else, return the first available event
	@CppEx 
		TCOD_key_t key = TCODConsole::waitForKeypress(true);
		if ( key.c == 'i' ) { ... open inventory ... }
	@CEx 
		TCOD_key_t key = TCOD_console_wait_for_keypress(true);
		if ( key.c == 'i' ) { ... open inventory ... }
	@PyEx 
		key = libtcod.console_wait_for_keypress(True)
		if key.c == ord('i') : # ... open inventory ...
	*/
	static TCOD_key_t waitForKeypress(bool flush);
	/**
	@PageName console_non_blocking_input
	@PageTitle Non blocking keyboard input
	@PageFather console_input
	@FuncDesc This function checks if the user has pressed a key. It returns the code of the key pressed as well as the corresponding character. See TCOD_key_t. If the user didn't press a key, this function returns the key code TCODK_NONE.
		<b>Note that key repeat only results in TCOD_KEY_PRESSED events.</b>
	@Cpp static TCOD_key_t TCODConsole::checkForKeypress(int flags=TCOD_KEY_RELEASED)
	@C TCOD_key_t TCOD_console_check_for_keypress(int flags)
	@Py console_check_for_keypress(flags=KEY_RELEASED)
	@Param flags A filter for key events :
		TCOD_KEY_PRESSED : only keypress events are returned
		TCOD_KEY_RELEASED : only key release events are returnes
		TCOD_KEY_PRESSED|TCOD_KEY_RELEASED : events of both types are returned.
	@CppEx 
		TCOD_key_t key = TCODConsole::checkForKeypress();
		if ( key.vk == TCODK_NONE ) return; // no key pressed
		if ( key.c == 'i' ) { ... open inventory ... }
	@C 
		TCOD_key_t key = TCOD_console_check_for_keypress(TCOD_KEY_PRESSED);
		if ( key.vk == TCODK_NONE ) return; // no key pressed
		if ( key.c == 'i' ) { ... open inventory ... }
	@Py 
		key = libtcod.console_check_for_keypress()
		if key.vk == libtcod.KEY_NONE return # no key pressed
		if key.c == ord('i') : # ... open inventory ...
	*/
	static TCOD_key_t checkForKeypress(int flags=TCOD_KEY_RELEASED);
	/**
	@PageName console_non_blocking_input
	@FuncDesc You can also get the status of any special key at any time with :
	@Cpp static bool TCODConsole::isKeyPressed(TCOD_keycode_t key)
	@C bool TCOD_console_is_key_pressed(TCOD_keycode_t key)
	@Py console_is_key_pressed(key)
	@Param key Any key code defined in keycode_t except TCODK_CHAR and TCODK_NONE
	*/
	static bool isKeyPressed(TCOD_keycode_t key);
	/**
	@PageName console_keyboard_repeat
	@PageTitle Changing keyboard repeat delay
	@PageFather console_input
	@FuncDesc This function changes the keyboard repeat times.
	@Cpp static void TCODConsole::setKeyboardRepeat(int initialDelay,int interval)
	@C void TCOD_console_set_keyboard_repeat(int initial_delay, int interval)
	@Py console_set_keyboard_repeat(initial_delay, interval)
	@Param initialDelay delay in millisecond between the time when a key is pressed, and keyboard repeat begins. If 0, keyboard repeat is disabled
	@Param interval interval in millisecond between keyboard repeat events
	*/
	static void setKeyboardRepeat(int initialDelay,int interval);
	/**
	@PageName console_keyboard_repeat
	@FuncDesc You can also disable the keyboard repeat feature with this function (it's equivalent to setKeyboardRepeat(0,0) ).
	@Cpp static void TCODConsole::disableKeyboardRepeat()
	@C void TCOD_console_disable_keyboard_repeat()
	@Py console_disable_keyboard_repeat()
	*/
	static void disableKeyboardRepeat();

	/**
	@PageName console_key_t
	@PageTitle 	Keyboard event structure
	@PageFather console_input
	@PageDesc This structure contains information about a key pressed/released by the user.
	@C
		typedef struct {
			TCOD_keycode_t vk; 
			char c;
			bool pressed;
			bool lalt;
			bool lctrl;
			bool ralt;
			bool rctrl;
			bool shift;
		} TCOD_key_t;
	@Param vk An arbitrary value representing the physical key on the keyboard. Possible values are stored in the TCOD_keycode_t enum. If no key was pressed, the value is TCODK_NONE
	@Param c If the key correspond to a printable character, the character is stored in this field. Else, this field contains 0.
	@Param pressed true if the event is a key pressed, or false for a key released.
	@Param lalt This field represents the status of the left Alt key : true => pressed, false => released.
	@Param lctrl This field represents the status of the left Control key : true => pressed, false => released.
	@Param ralt This field represents the status of the right Alt key : true => pressed, false => released.
	@Param rctrl This field represents the status of the right Control key : true => pressed, false => released.
	@Param shift This field represents the status of the shift key : true => pressed, false => released.
	*/

	/**
	@PageName console_keycode_t
	@PageTitle 	Key codes
	@PageFather console_input
	@PageDesc TCOD_keycode_t is a libtcod specific code representing a key on the keyboard.
		For python, replace TCODK by KEY: libtcod.KEY_NONE Possible values are :
		When no key was pressed (see checkForKeypress) : TCOD_NONE
		Special keys :
		TCODK_ESCAPE
		TCODK_BACKSPACE
		TCODK_TAB
		TCODK_ENTER
		TCODK_SHIFT
		TCODK_CONTROL
		TCODK_ALT
		TCODK_PAUSE
		TCODK_CAPSLOCK
		TCODK_PAGEUP
		TCODK_PAGEDOWN
		TCODK_END
		TCODK_HOME
		TCODK_UP
		TCODK_LEFT
		TCODK_RIGHT
		TCODK_DOWN
		TCODK_PRINTSCREEN
		TCODK_INSERT
		TCODK_DELETE
		TCODK_LWIN
		TCODK_RWIN
		TCODK_APPS
		TCODK_KPADD
		TCODK_KPSUB
		TCODK_KPDIV
		TCODK_KPMUL
		TCODK_KPDEC
		TCODK_KPENTER
		TCODK_F1
		TCODK_F2
		TCODK_F3
		TCODK_F4
		TCODK_F5
		TCODK_F6
		TCODK_F7
		TCODK_F8
		TCODK_F9
		TCODK_F10
		TCODK_F11
		TCODK_F12
		TCODK_NUMLOCK
		TCODK_SCROLLLOCK
		TCODK_SPACE
		
		numeric keys :
		
		TCODK_0
		TCODK_1
		TCODK_2
		TCODK_3
		TCODK_4
		TCODK_5
		TCODK_6
		TCODK_7
		TCODK_8
		TCODK_9
		TCODK_KP0
		TCODK_KP1
		TCODK_KP2
		TCODK_KP3
		TCODK_KP4
		TCODK_KP5
		TCODK_KP6
		TCODK_KP7
		TCODK_KP8
		TCODK_KP9
		
		Any other (printable) key :
		
		TCODK_CHAR
		
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
	@Param w,h the console size.
		0 < w
		0 < h
	@CppEx 
		// Creating a 40x20 offscreen console, filling it with red and blitting it on the root console at position 5,5
		TCODConsole *offscreenConsole = new TCODConsole(40,20);
		offscreenConsole->setBackgroundColor(TCODColor::red);
		offscreenConsole->clear();
		TCODConsole::blit(offscreenConsole,0,0,40,20,TCODConsole::root,5,5,255);
	@CEx 
		TCOD_console_t offscreen_console = TCOD_console_new(40,20);
		TCOD_console_set_background_color(offscreen_console,TCOD_red);
		TCOD_console_clear(offscreen_console);
		TCOD_console_blit(offscreen_console,0,0,40,20,NULL,5,5,255);
	@PyEx 
		offscreen_console = libtcod.console_new(40,20)
		libtcod.console_set_background_color(offscreen_console,libtcod.red)
		libtcod.console_clear(offscreen_console)
		libtcod.console_blit(offscreen_console,0,0,40,20,0,5,5,255)
	*/
	TCODConsole(int w, int h);

	/**
	@PageName console_offscreen
	@FuncTitle Blitting a console on another one
	@FuncDesc This function allows you to blit a rectangular area of the source console at a specific position on a destination console. It can also simulate alpha transparency with the fade parameter.
	@Cpp static void blit(const TCODConsole *src,int xSrc, int ySrc, int wSrc, int hSrc, TCODConsole *dst, int xDst, int yDst, float foregroundAlpha=1.0f, float backgroundAlpha=1.0f)
	@C void TCOD_console_blit(TCOD_console_t src,int xSrc, int ySrc, int wSrc, int hSrc, TCOD_console_t dst, int xDst, int yDst, float foreground_alpha, float background_alpha)
	@Py console_blit(src,xSrc,ySrc,xSrc,hSrc,dst,xDst,yDst,foregroundAlpha=1.0,backgroundAlpha=1.0)
	@Param src The source console that must be blitted on another one.
	@Param xSrc,ySrc,wSrc,hSrc The rectangular area of the source console that will be blitted.
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
	*/
	static void blit(const TCODConsole *src,int xSrc, int ySrc, int wSrc, int hSrc, TCODConsole *dst, int xDst, int yDst, float foreground_alpha=1.0f, float background_alpha=1.0f);
	/**
	@PageName console_offscreen
	@FuncTitle Define a blit-transparent color
	@FuncDesc This function defines a transparent background color for an offscreen console. All cells with this background color are ignored by the blit operation. You can use it to blit only some parts of the console.
	@Cpp void TCODConsole::setKeyColor(const TCODColor &col)
	@C void TCOD_console_set_key_color(TCOD_console_t con,TCOD_color_t col)
	@Py console_set_key_color(con,col)
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
	*/
	virtual ~TCODConsole();

	void setDirty(int x, int y, int w, int h);


	TCODConsole(TCOD_console_t con) : data(con) {}
	
protected :
	friend class TCODLIB_API TCODImage;
	friend class TCODLIB_API TCODZip;
	friend class TCODLIB_API TCODText;
	TCODConsole();
	TCOD_console_t data;
};

#endif
