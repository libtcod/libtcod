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

class TCODLIB_API TCODImage {
public :
	/**
	@PageName image
	@PageTitle Image toolkit
	@PageCategory Base toolkits
	@PageDesc This toolkit contains some image manipulation utilities.	
	*/	
	
	/**
	@PageName image_create
	@PageTitle Creating an image
	@PageFather image
	@FuncTitle Creating an empty image
	@FuncDesc You can create an image of any size, filled with black with this function.
	@Cpp TCODImage::TCODImage(int width, int height)
	@C TCOD_image_t TCOD_image_new(int width, int height)
	@Py image_new( width, height)
	@C# TCODImage::TCODImage(int width, int height)
	@Param width,height	Size of the image in pixels.
	@CppEx TCODImage *pix = new TCODImage(80,50);
	@CEx TCOD_image_t pix = TCOD_image_new(80,50);
	@PyEx pix = litbcod.image_new(80,50)	
	*/	
	TCODImage(int width, int height);

	/**
	@PageName image_create
	@FuncTitle Loading a .bmp or .png image
	@FuncDesc You can read data from a .bmp or .png file (for example to draw an image using the background color of the console cells).
		Note that only 24bits and 32bits PNG files are currently supported.
	@Cpp TCODImage::TCODImage(const char *filename)
	@C TCOD_image_t TCOD_image_load(const char *filename)
	@Py image_load(filename)
	@C# TCODImage::TCODImage(string filename)
	@Param filename Name of the .bmp or .png file to load.
	@CppEx TCODImage *pix = new TCODImage("mypic.bmp");
	@CEx TCOD_image_t pix = TCOD_image_load("mypic.bmp");
	@PyEx pix = libtcod.image_load("mypic.bmp")	
	*/	
	TCODImage(const char *filename);
	
	/**
	@PageName image_create
	@FuncTitle Creating an image from a console
	@FuncDesc You can create an image from any console (either the root console or an offscreen console).
		The image size will depend on the console size and the font characters size.
		You can then save the image to a file with the save function.
	@Cpp TCODImage::TCODImage(const TCODConsole *console)
	@C TCOD_image_t TCOD_image_from_console(TCOD_console_t console)
	@Py image_from_console(console)
	@C# TCODImage::TCODImage(TCODConsole console)
	@Param console The console to convert. In the C version, use NULL for the root console.
	@CppEx TCODImage *pix = new TCODImage(TCODConsole::root);
	@CEx TCOD_image_t pix = TCOD_image_from_console(NULL);
	@PyEx pix = libtcod.image_from_console(0)
	*/	
	TCODImage(const TCODConsole *console);
	
	/**
	@PageName image_create
	@FuncTitle refreshing an image created from a console
	@FuncDesc If you need to refresh the image with the console's new content, you don't have to delete it and create another one. Instead, use this function. Note that you must use the same console that was used in the TCOD_image_from_console call (or at least a console with the same size).
	@Cpp void TCODImage::refreshConsole(const TCODConsole *console)
	@C void TCOD_image_refresh_console(TCOD_image_t image, TCOD_console_t console)
	@Py image_refresh_console(image, console)
	@C# void TCODImage::refreshConsole(TCODConsole console)
	@Param image In the C version, the image created with TCOD_image_from_console.
	@Param console The console to capture. In the C version, use NULL for the root console.
	@CppEx 
		TCODImage *pix = new TCODImage(TCODConsole::root); // create an image from the root console
		// ... modify the console
		pix->refreshConsole(TCODConsole::root); // update the image with the console's new content
	@CEx 
		TCOD_image_t pix = TCOD_image_from_console(NULL);
		// ... modify the console .. 
		TCOD_image_refresh_console(pix,NULL);
	@PyEx 
		pix = libtcod.image_from_console(0)
		# ... modify the console .. 
		libtcod.image_refresh_console(pix,0)
	*/
	void refreshConsole(const TCODConsole *console);

	/**
	@PageName image_read
	@PageTitle Reading data from a TCODImage
	@PageFather image
	@FuncTitle Getting the size of an image
	@FuncDesc You can read the size of an image in pixels with this function.
	@Cpp void TCODImage::getSize(int *w,int *h) const
	@C void TCOD_image_get_size(TCOD_image_t image, int *w,int *h)
	@Py image_get_size(image) # returns w,h
	@C# void TCODImage::getSize(out int w, out int h)
	@Param image In the C version, the image handler, obtained with the load function.
	@Param w,h When the function returns, those variables contain the size of the image.
	@CppEx 
		TCODImage *pix = new TCODImage(80,50);
		int w,h;
		pix->getSize(&w,&h); // w = 80, h = 50
	@CEx 
		TCOD_image_t pix = TCOD_image_new(80,50);
		int w,h;
		TCOD_image_get_size(pix,&w,&h); // w = 80, h = 50
	@PyEx 
		pix = libtcod.image_new(80,50)
		w,h=libtcod.image_get_size(pix)
		# w = 80, h = 50 
	*/		
	void getSize(int *w,int *h) const;

	/**
	@PageName image_read
	@FuncTitle Getting the color of a pixel
	@FuncDesc You can read the colors from an image with this function.
	@Cpp TCODColor TCODImage::getPixel(int x, int y) const
	@C TCOD_color_t TCOD_image_get_pixel(TCOD_image_t image,int x, int y)
	@Py image_get_pixel(image, x, y)
	@C# TCODColor TCODImage::getPixel(int x, int y)
	@Param image In the C and python version, the image handler, obtained with the load function.
	@Param x,y The pixel coordinates inside the image.
		0 <= x < width
		0 <= y < height
	@CppEx 
		TCODImage *pix = new TCODImage(80,50);
		TCODColor col=pix->getPixel(40,25);
	@CEx 
		TCOD_image_t pix = TCOD_image_new(80,50);
		TCOD_color_t col=TCOD_image_get_pixel(pix,40,25);
	@PyEx 
		pix = litbcod.image_new(80,50)
		col=litbcod.image_get_pixel(pix,40,25)
	*/
	TCODColor getPixel(int x, int y) const;

	/**
	@PageName image_read
	@FuncTitle Getting the alpha value of a pixel
	@FuncDesc If you have set a key color for this image with setKeyColor, or if this image was created from a 32 bits PNG file (with alpha layer), you can get the pixel transparency with this function. This function returns a value between 0 (transparent pixel) and 255 (opaque pixel).
	@Cpp int TCODImage::getAlpha(int x, int y) const
	@C int TCOD_image_get_alpha(TCOD_image_t image, int x, int y)
	@Py image_get_alpha(image, x, y)
	@C# int TCODImage::getAlpha(int x, int y)
	@Param image In the C and python version, the image handler, obtained with the load function.
	@Param x,y The pixel coordinates inside the image.
		0 <= x < width
		0 <= y < height
	*/
	int getAlpha(int x,int y) const;

	/**
	@PageName image_read
	@FuncTitle Checking if a pixel is transparent
	@FuncDesc You can use this simpler version (for images with alpha layer, returns true only if alpha == 0) :
	@Cpp bool TCODImage::isPixelTransparent(int x,int y) const
	@C bool TCOD_image_is_pixel_transparent(TCOD_image_t image,int x, int y)
	@Py image_is_pixel_transparent(image, x, y)
	@C# bool TCODImage::isPixelTransparent(int x,int y)
	@Param image In the C and python version, the image handler, obtained with the load function.
	@Param x,y The pixel coordinates inside the image.
		0 <= x < width
		0 <= y < height
	*/
   	bool isPixelTransparent(int x, int y) const;

	/**
	@PageName image_read
	@FuncTitle Getting the average color of a part of the image
	@FuncDesc This method uses mipmaps to get the average color of an arbitrary rectangular region of the image.
		It can be used to draw a scaled-down version of the image. It's used by libtcod's blitting functions.
	@Cpp TCODColor TCODImage::getMipmapPixel(float x0,float y0, float x1, float y1)
	@C TCOD_color_t TCOD_image_get_mipmap_pixel(TCOD_image_t image,float x0,float y0, float x1, float y1)
	@Py image_get_mipmap_pixel(image,x0,y0, x1, y1)
	@C# TCODColor TCODImage::getMipmapPixel(float x0,float y0, float x1, float y1)
	@Param image	In the C version, the image handler, obtained with the load function.
	@Param x0,y0	Coordinates in pixels of the upper-left corner of the region.
		0.0 <= x0 < x1
		0.0 <= y0 < y1
	@Param x1,y1	Coordinates in pixels of the lower-right corner of the region.
		x0 < x1 < width
		y0 < y1 < height
	@CppEx 
		// Get the average color of a 5x5 "superpixel" in the center of the image.
		TCODImage *pix = new TCODImage(80,50);
		TCODColor col=pix->getMipMapPixel(37.5f, 22.5f, 42.5f, 28.5f);
	@CEx 
		TCOD_image_t pix = TCOD_image_new(80,50);
		TCOD_color_t col=TCOD_image_get_mipmap_pixel(pix,37.5f, 22.5f, 42.5f, 28.5f);
	@PyEx
		pix = libtcod.image_new(80,50)
		col=libtcod.image_get_mipmap_pixel(pix,37.5, 22.5, 42.5, 28.5)
	*/
   	TCODColor getMipmapPixel(float x0,float y0, float x1, float y1);

	/**
	@PageName image_update
	@PageTitle Updating an image
	@PageFather image
	@FuncTitle Filling an image with a color
	@FuncDesc You can fill the whole image with a color with :
	@Cpp void TCODImage::clear(const TCODColor color)
	@C void TCOD_image_clear(TCOD_image_t image, TCOD_color_t color)
	@Py image_clear(image,color)
	@C# void TCODImage::clear(TCODColor color)
	@Param image	In the C and python version, the image to fill.
	@Param color	The color to use.
	*/
	void clear(const TCODColor col);

	/**
	@PageName image_update
	@FuncTitle 	Changing the color of a pixel
	@Cpp TCODColor TCODImage::putPixel(int x, int y, const TCODColor col)
	@C void TCOD_image_put_pixel(TCOD_image_t image,int x, int y,TCOD_color_t col)
	@Py image_put_pixel(image,x, y,col)
	@C# TCODColor TCODImage::putPixel(int x, int y, TCODColor col)
	@Param image	In the C version, the image handler, obtained with the load function.
	@Param x,y	The pixel coordinates inside the image.
		0 <= x < width
		0 <= y < height
	@Param col	The new color of the pixel.
	*/
	void putPixel(int x, int y, const TCODColor col);
	
	/**
	@PageName image_update
	@FuncTitle Scaling an image
	@FuncDesc You can resize an image and scale its content. If neww < oldw or newh < oldh, supersampling is used to scale down the image. Else the image is scaled up using nearest neightbor.
	@Cpp void TCODImage::scale(int neww, int newh)
	@C void TCOD_image_scale(TCOD_image_t image,int neww, int newh)
	@Py image_scale(image, neww,newh)
	@C# void TCODImage::scale(int neww, int newh)
	@Param image	In the C and python version, the image handler, obtained with the load function.
	@Param neww,newh	The new size of the image.
	*/
	void scale(int neww, int newh);

	/**
	@PageName image_update
	@FuncTitle Flipping the image horizontally
	@Cpp void TCODImage::hflip()
	@C void TCOD_image_hflip(TCOD_image_t image)
	@Py image_hflip(image)
	@C# void TCODImage::hflip()
	@Param image	In the C and python version, the image handler, obtained with the load function.
	*/
	void hflip();

    /**
	@PageName image_update
	@FuncTitle Flipping the image vertically
	@Cpp void TCODImage::vflip()
	@C void TCOD_image_vflip(TCOD_image_t image)
	@Py image_vflip(image)
	@C# void TCODImage::vflip()
	@Param image	In the C and python version, the image handler, obtained with the load function.
	*/
	void vflip();

    /**
	@PageName image_update
	@FuncTitle Rotating the image clockwise
	@FuncDesc Rotate the image clockwise by increment of 90 degrees. 
	@Cpp void TCODImage::rotate90(int numRotations=1)
	@C void TCOD_image_rotate90(TCOD_image_t image, int numRotations)
	@Py image_rotate90(image, num=1)
	@C# void TCODImage::rotate90(int numRotations)
	@Param image	In the C and python version, the image handler, obtained with the load function.
	@Param numRotations	Number of 90 degrees rotations. Should be between 1 and 3.
	*/
	void rotate90(int numRotations=1);

    /**
	@PageName image_update
	@FuncTitle Inverting the colors of the image
	@Cpp void TCODImage::invert()
	@C void TCOD_image_invert(TCOD_image_t image)
	@Py image_invert(image)
	@C# void TCODImage::invert()
	@Param image	In the C and python version, the image handler, obtained with the load function.
	*/
	void invert();

	/**
	@PageName image_save
	@PageFather image
	@PageTitle Saving an image to a bmp or png file.
	@PageDesc You can save an image to a 24 bits .bmp or .png file.
	@Cpp void TCODImage::save(const char *filename)
	@C void TCOD_image_save(TCOD_image_t image, const char *filename)
	@Py image_save(image, filename)
	@C# void TCODImage::save(string filename)
	@Param image	In the C version, the image handler, obtained with any image creation function.
	@Param filename	Name of the .bmp or .png file.
	@CppEx 
		TCODImage *pix = new TCODImage(10,10);
		pix->save("mypic.bmp");
	@CEx 
		TCOD_image_t pix = TCOD_image_from_console(my_offscreen_console);
		TCOD_image_save(pix,"mypic.bmp");
	@PyEx 
		pix = libtcod.image_from_console(my_offscreen_console)
		libtcod.image_save(pix,"mypic.bmp")
	  */	
	void save(const char *filename) const;

	/**
	@PageName image_blit
	@PageFather image
	@PageTitle Blitting an image on a console
	@FuncTitle Standard blitting
	@FuncDesc This function blits a rectangular part of the image on a console without scaling it or rotating it. Each pixel of the image fills a console cell.
	@Cpp void TCODImage::blitRect(TCODConsole *console, int x, int y, int w=-1, int h=-1, TCOD_bkgnd_flag_t bkgnd_flag = TCOD_BKGND_SET ) const
	@C void TCOD_image_blit_rect(TCOD_image_t image, TCOD_console_t console, int x, int y, int w, int h, TCOD_bkgnd_flag_t bkgnd_flag)
	@Py image_blit_rect(image, console, x, y, w, h, bkgnd_flag)
	@C# 
		void TCODImage::blitRect(TCODConsole console, int x, int y) 
		void TCODImage::blitRect(TCODConsole console, int x, int y, int w)
		void TCODImage::blitRect(TCODConsole console, int x, int y, int w, int h)
		void TCODImage::blitRect(TCODConsole console, int x, int y, int w, int h, TCODBackgroundFlag bkgnd_flag)	
	@Param image	In the C version, the image handler, obtained with the load function.
	@Param console	The console on which the image will be drawn. In the C version, use NULL for the root console.
	@Param x,y	Coordinates in the console of the upper-left corner of the image.
	@Param w,h	Dimension of the image on the console. Use -1,-1 to use the image size.
	@Param flag	This flag defines how the cell's background color is modified. See TCOD_bkgnd_flag_t.	
	*/	
	void blitRect(TCODConsole *console, int x, int y, int w=-1, int h=-1, TCOD_bkgnd_flag_t bkgnd_flag = TCOD_BKGND_SET ) const;
	
	/**
	@PageName image_blit
	@FuncTitle Blitting with scaling and/or rotation
	@FuncDesc This function allows you to specify the floating point coordinates of the center
		of the image, its scale and its rotation angle.
	@Cpp void TCODImage::blit(TCODConsole *console, float x, float y, TCOD_bkgnd_flag_t bkgnd_flag = TCOD_BKGND_SET, float scalex=1.0f, float scaley=1.0f, float angle=0.0f) const
	@C void TCOD_image_blit(TCOD_image_t image, TCOD_console_t console, int x, int y, TCOD_bkgnd_flag_t bkgnd_flag, float scalex, float scaley, float angle)
	@Py image_blit(image, console, x, y, bkgnd_flag, scalex, scaley, angle)
	@C#
		void TCODImage::blit(TCODConsole console, float x, float y)
		void TCODImage::blit(TCODConsole console, float x, float y, TCODBackgroundFlag bkgnd_flag)
		void TCODImage::blit(TCODConsole console, float x, float y, TCODBackgroundFlag bkgnd_flag, float scalex)
		void TCODImage::blit(TCODConsole console, float x, float y, TCODBackgroundFlag bkgnd_flag, float scalex, float scaley)
		void TCODImage::blit(TCODConsole console, float x, float y, TCODBackgroundFlag bkgnd_flag, float scalex, float scaley, float angle)
	@Param image	In the C version, the image handler, obtained with the load function.
	@Param console	The console on which the image will be drawn. In the C version, use NULL for the root console.
	@Param x,y	Coordinates in the console of the center of the image.
	@Param flag	This flag defines how the cell's background color is modified. See TCOD_bkgnd_flag_t.
	@Param scalex,scaley	Scale coefficient. Must be > 0.0.
	@Param angle	Rotation angle in radians.
	*/	
	void blit(TCODConsole *console, float x, float y, TCOD_bkgnd_flag_t bkgnd_flag = TCOD_BKGND_SET, float scalex=1.0f, float scaley=1.0f, float angle=0.0f) const;
	/**
	@PageName image_blit
	@FuncTitle Blitting with a mask
	@FuncDesc When blitting an image, you can define a key color that will be ignored by the blitting function. This makes it possible to blit non rectangular images or images with transparent pixels.
	@Cpp void TCODImage::setKeyColor(const TCODColor keyColor)
	@C void TCOD_image_set_key_color(TCOD_image_t image, TCOD_color_t keyColor)
	@Py image_set_key_color(image, keyColor)
	@C# void TCODImage::setKeyColor(TCODColor keyColor)
	@Param image	In the C and python version, the image handler, obtained with the load function.
	@Param color	Pixels with this color will be skipped by blitting functions.
	@CppEx 
		TCODImage *pix = TCODImage("mypix.bmp");
		pix->setKeyColor(TCODColor::red);
		// blitting the image, omitting red pixels
		pix->blitRect(TCODConsole::root,40,25);
	@CEx 
		TCOD_image_t pix = TCOD_image_new(10,10);
		TCOD_image_set_key_color(pix,TCOD_red);
		TCOD_image_blit_rect(pix,NULL,40,25,5,5,TCOD_BKGND_SET);
	@PyEx 
		pix = libtcod.image_new(10,10)
		libtcod.image_set_key_color(pix,libtcod.red)
		libtcod.image_blit_rect(pix,0,40,25,5,5,libtcod.BKGND_SET)
	*/	
	void setKeyColor(const TCODColor keyColor);
	
	/**
	@PageName image_blit
	@FuncTitle Blitting with subcell resolution
	@FuncDesc Eventually, you can use some special characters in the libtcod fonts :
		<img src="subcell.png">
		to double the console resolution using this blitting function.
		<table><tr><td> 
		Comparison before/after subcell resolution in TCOD :<br /> 
		<img src="subcell_comp.png"></td><td> 
		Pyromancer ! screenshot, making full usage of subcell resolution :<br /> 
		<img src="subcell_pyro.png"></td></tr></table>
	@Cpp void TCODImage::blit2x(TCODConsole *dest, int dx, int dy, int sx=0, int sy=0, int w=-1, int h=-1 ) const;
	@C void TCOD_image_blit_2x(TCOD_image_t image, TCOD_console_t dest, int dx, int dy, int sx, int sy, int w, int h);
	@Py image_blit_2x(image, dest, dx, dy, sx=0, sy=0, w=-1, h=-1)
	@C#
		void TCODImage::blit2x(TCODConsole dest, int dx, int dy);
		void TCODImage::blit2x(TCODConsole dest, int dx, int dy, int sx);
		void TCODImage::blit2x(TCODConsole dest, int dx, int dy, int sx, int sy);
		void TCODImage::blit2x(TCODConsole dest, int dx, int dy, int sx, int sy, int w);
		void TCODImage::blit2x(TCODConsole dest, int dx, int dy, int sx, int sy, int w, int h);
	@Param image	In the C and python version, the image handler, obtained with the load function.
	@Param dest	The console of which the image will be blited. Foreground, background and character data will be overwritten.
	@Param dx,dy	Coordinate of the console cell where the upper left corner of the blitted image will be.
	@Param sx,sy,w,h	Part of the image to blit. Use -1 in w and h to blit the whole image.
	*/	
	void blit2x(TCODConsole *dest, int dx, int dy, int sx=0, int sy=0, int w=-1, int h=-1) const;

	TCODImage(TCOD_image_t img) : data(img), deleteData(false) {}
	virtual ~TCODImage();

protected :
	friend class TCODLIB_API TCODSystem;
	friend class TCODLIB_API TCODZip;
	void *data;
	bool deleteData;
};

