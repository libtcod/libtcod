#ifndef _TCOD_SYS_HPP
#define _TCOD_SYS_HPP

class TCODLIB_API TCODSystem {
public :
	static uint32 getElapsedMilli();
	static float getElapsedSeconds();
	static void sleepMilli(uint32 val);
	static void saveScreenshot(const char *filename);
	static void forceFullscreenResolution(int width, int height);
	static void setFps(int val);
	static int getFps();
	static float getLastFrameLength();
	static void getCurrentResolution(int *w, int *h);
};

#endif
