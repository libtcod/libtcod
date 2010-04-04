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

enum TCOD_print_location_t {TCOD_PRINT_LEFT, TCOD_PRINT_RIGHT, TCOD_PRINT_CENTER };

class TCODLIB_API TCODConsole {
public :
	static TCODConsole *root;
	int getWidth() const;
	int getHeight() const;
	static void initRoot(int w, int h, const char * title, bool fullscreen = false, TCOD_renderer_t renderer=TCOD_RENDERER_GLSL);
	static void setWindowTitle(const char *title);
	static void setFullscreen(bool fullscreen);
	static bool isFullscreen();
	static bool isWindowClosed();

	static void setCustomFont(const char *fontFile, int flags=TCOD_FONT_LAYOUT_ASCII_INCOL,int nbCharHoriz=0, int nbCharVertic=0);
	static void mapAsciiCodeToFont(int asciiCode, int fontCharX, int fontCharY);
	static void mapAsciiCodesToFont(int firstAsciiCode, int nbCodes, int fontCharX, int fontCharY);
	static void mapStringToFont(const char *s, int fontCharX, int fontCharY);
	
	void setBackgroundColor(TCODColor back);
	void setForegroundColor(TCODColor fore);
	void clear();
	void setDirty(int x, int y, int w, int h);
	void setBack(int x, int y, const TCODColor &col, TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET);
	void setFore(int x, int y, const TCODColor &col);
	void setChar(int x, int y, int c);
	void putChar(int x, int y, int c, TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET);
	void putCharEx(int x, int y, int c, const TCODColor &fore, const TCODColor &back);

	void printLeft(int x, int y, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
	void printRight(int x, int y, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
	void printCenter(int x, int y, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
	void printLine(int x, int y, TCOD_bkgnd_flag_t flag, TCOD_print_location_t location, const char *fmt, ...); 

	int printLeftRect(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
	int printRightRect(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
	int printCenterRect(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...);
	int printRect(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, TCOD_print_location_t location, const char *fmt, ...); 

	int getHeightLeftRect(int x, int y, int w, int h, const char *fmt, ...); 
	int getHeightRightRect(int x, int y, int w, int h, const char *fmt, ...); 
	int getHeightCenterRect(int x, int y, int w, int h, const char *fmt, ...); 
	int getHeightRect(int x, int y, int w, int h, const char *fmt, ...); 

	void rect(int x, int y, int w, int h, bool clear, TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET);
	void hline(int x,int y, int l, TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET);
	void vline(int x,int y, int l, TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET);
	void printFrame(int x,int y,int w,int h, bool empty, TCOD_bkgnd_flag_t flag, const char *fmt, ...);

#ifndef NO_UNICODE
	static void mapStringToFont(const wchar_t *s, int fontCharX, int fontCharY);
	void printLeft(int x, int y, TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...); 
	void printRight(int x, int y, TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...); 
	void printCenter(int x, int y, TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...); 
	int printLeftRect(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...); 
	int printRightRect(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...); 
	int printCenterRect(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...); 
	int getHeightLeftRect(int x, int y, int w, int h, const wchar_t *fmt, ...); 
	int getHeightRightRect(int x, int y, int w, int h, const wchar_t *fmt, ...); 
	int getHeightCenterRect(int x, int y, int w, int h,const wchar_t *fmt, ...); 
#endif

	TCODColor getBackgroundColor() const;
	TCODColor getForegroundColor() const;
	TCODColor getBack(int x, int y) const;
	TCODColor getFore(int x, int y) const;
	int getChar(int x, int y) const;

	static void setFade(uint8 fade, const TCODColor &fadingColor);
	static uint8 getFade();
	static TCODColor getFadingColor();

	static void setColorControl(TCOD_colctrl_t con, const TCODColor &fore, const TCODColor &back);
	
	static void flush();
	
	static TCOD_key_t checkForKeypress(int flags=TCOD_KEY_RELEASED);
	static TCOD_key_t waitForKeypress(bool flush);
	static void setKeyboardRepeat(int initialDelay,int interval);
	static void disableKeyboardRepeat();
	static bool isKeyPressed(TCOD_keycode_t key);
	
	TCODConsole(int w, int h);
	TCODConsole(TCOD_console_t con) : data(con) {}
	static void blit(const TCODConsole *src,int xSrc, int ySrc, int wSrc, int hSrc, TCODConsole *dst, int xDst, int yDst, float foreground_alpha=1.0f, float background_alpha=1.0f);
	void setKeyColor(const TCODColor &col);
	virtual ~TCODConsole();
	
	static void credits();
	static void resetCredits();
	static bool renderCredits(int x, int y, bool alpha);

protected :
	friend class TCODLIB_API TCODImage;
	friend class TCODLIB_API TCODZip;
	friend class TCODLIB_API TCODText;
	TCODConsole();
	TCOD_console_t data;
};

#endif
