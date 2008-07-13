#ifndef _TCOD_CONSOLE_HPP
#define _TCOD_CONSOLE_HPP

class TCODImage;

class TCODLIB_API TCODConsole {
public :
	static TCODConsole *root;
	int getWidth() const;
	int getHeight() const;
	static void initRoot(int w, int h, const char * title, bool fullscreen = false);
	static void setCustomFont(const char *fontFile,int charWidth=8, int charHeight=8, int nbCharHoriz=16, int nbCharVertic=16, bool inRow=false,TCODColor keyColor=TCODColor::black);
	static void setWindowTitle(const char *title);
	static void setFullscreen(bool fullscreen);
	static bool isFullscreen();
	static bool isWindowClosed();
	
	void setBackgroundColor(TCODColor back);
	void setForegroundColor(TCODColor fore);
	void clear();
	void setBack(int x, int y, const TCODColor &col, TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET);
	void setFore(int x, int y, const TCODColor &col);
	void setChar(int x, int y, int c);
	void putChar(int x, int y, int c, TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET);

	void printLeft(int x, int y, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
	void printRight(int x, int y, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
	void printCenter(int x, int y, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
	int printLeftRect(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
	int printRightRect(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
	int printCenterRect(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 

	void rect(int x, int y, int w, int h, bool clear, TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET);
	void hline(int x,int y, int l, TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET);
	void vline(int x,int y, int l, TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET);
	void printFrame(int x,int y,int w,int h, bool empty, const char *fmt, ...);

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
	static void blit(const TCODConsole *src,int xSrc, int ySrc, int wSrc, int hSrc, TCODConsole *dst, int xDst, int yDst, int fade=255);
	virtual ~TCODConsole();
	
protected :
	friend class TCODImage;
	TCODConsole();
	TCOD_console_t data;
};

#endif
