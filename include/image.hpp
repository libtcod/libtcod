/*
* libtcod 1.5.0
* Copyright (c) 2008,2009,2010 Jice
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

class TCODLIB_API TCODImage {
public :
	TCODImage(const char *filename);
	TCODImage(int width, int height);
	TCODImage(const TCODConsole *console);
	TCODImage(TCOD_image_t img) : data(img), deleteData(false) {}
	virtual ~TCODImage();
	void clear(const TCODColor col);
	void getSize(int *w,int *h) const;
	TCODColor getPixel(int x, int y) const;
	int getAlpha(int x,int y) const;
	TCODColor getMipmapPixel(float x0,float y0, float x1, float y1);
	void putPixel(int x, int y, const TCODColor col);
	void blit(TCODConsole *console, float x, float y, TCOD_bkgnd_flag_t bkgnd_flag = TCOD_BKGND_SET, float scalex=1.0f, float scaley=1.0f, float angle=0.0f) const;
	void blitRect(TCODConsole *console, int x, int y, int w=-1, int h=-1, TCOD_bkgnd_flag_t bkgnd_flag = TCOD_BKGND_SET ) const;
	void blit2x(TCODConsole *dest, int dx, int dy, int sx=0, int sy=0, int w=-1, int h=-1) const;
	void save(const char *filename) const;
	void invert();
	void hflip();
	void vflip();
	void scale(int neww, int newh);
	void setKeyColor(const TCODColor keyColor);
	bool isPixelTransparent(int x, int y) const;
	void refreshConsole(const TCODConsole *console);

protected :
	friend class TCODLIB_API TCODSystem;
	friend class TCODLIB_API TCODZip;
	void *data;
	bool deleteData;
};

