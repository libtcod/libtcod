#ifndef _TCOD_IMAGE_HPP
#define _TCOD_IMAGE_HPP

class TCODLIB_API TCODImage {
public :
	TCODImage(const char *filename);
	TCODImage(int width, int height);
	TCODImage(const TCODConsole *console);
	virtual ~TCODImage();
	void clear(const TCODColor col);
	void getSize(int *w,int *h) const;
	TCODColor getPixel(int x, int y) const;
	TCODColor getMipmapPixel(float x0,float y0, float x1, float y1);
	void putPixel(int x, int y, const TCODColor col);
	void blit(TCODConsole *console, float x, float y, TCOD_bkgnd_flag_t bkgnd_flag = TCOD_BKGND_SET, float scalex=1.0f, float scaley=1.0f, float angle=0.0f) const;
	void blitRect(TCODConsole *console, int x, int y, int w=-1, int h=-1, TCOD_bkgnd_flag_t bkgnd_flag = TCOD_BKGND_SET ) const;
	void save(const char *filename) const;
	void setKeyColor(const TCODColor keyColor);
	bool isPixelTransparent(int x, int y) const;

protected :
	void *data;
};

#endif
