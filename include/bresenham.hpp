/*
* libtcod 1.6.3
* Copyright (c) 2008,2009,2010,2012,2013,2016,2017 Jice & Mingos & rmtew
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
* THIS SOFTWARE IS PROVIDED BY JICE, MINGOS AND RMTEW ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE, MINGOS OR RMTEW BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _TCOD_BRESENHAM_HPP
#define _TCOD_BRESENHAM_HPP
class TCODLIB_API TCODLineListener {
public :
	virtual bool putPoint(int x,int y) = 0;
	virtual ~TCODLineListener() {}
};

class TCODLIB_API TCODLine {
public :
	/**
	@PageName line
	@PageCategory Base toolkits
	@PageTitle Line drawing toolkit
	@PageDesc This toolkit is a very simple and lightweight implementation of the bresenham line drawing algorithm. It allows you to follow straight paths on your map very easily.
	@FuncTitle Initializing the line
	@FuncDesc First, you have to initialize the toolkit with your starting and ending coordinates.
	@Cpp static void TCODLine::init (int xFrom, int yFrom, int xTo, int yTo)
	@C void TCOD_line_init (int xFrom, int yFrom, int xTo, int yTo)
	@Py line_init (xFrom, yFrom, xTo, yTo)
	@C# static void TCODLine::init(int xFrom, int yFrom, int xTo, int yTo)
	@Lua tcod.line.init(xFrom,yFrom, xTo,yTo)
	@Param xFrom,yFrom Coordinates of the line's starting point.
	@Param xTo,yTo Coordinates of the line's ending point.
	*/
	static void init(int xFrom, int yFrom, int xTo, int yTo);
	
	/**
	@PageName line
	@FuncTitle Walking the line
	@FuncDesc You can then step through each cell with this function. It returns true when you reach the line's ending point.
	@Cpp static bool TCODLine::step (int * xCur, int * yCur)
	@C bool TCOD_line_step (int * xCur, int * yCur)
	@Py line_step () # returns x,y or None,None if finished
	@C# static bool TCODLine::step(ref int xCur, ref int yCur)
	@Lua tcod.line.step(x,y) -- returns lineEnd,x,y
	@Param xCur,yCur the coordinates of the next cell on the line are stored here when the function returns
	@CppEx 
		// Going from point 5,8 to point 13,4
		int x = 5, y = 8;
		TCODLine::init(x,y,13,4);
		do {
		    // update cell x,y
		} while (!TCODLine::step(&x,&y));
	@CEx 
		int x = 5, y = 8;
		TCOD_line_init(x,y,13,4);
		do {
		    // update cell x,y 
		} while (!TCOD_line_step(&x,&y));
	@PyEx 
		libtcod.line_init(5,8,13,4)
		# update cell 5,8
		x,y=libtcod.line_step()
		while (not x is None) :
		    # update cell x,y
		x,y=libtcod.line_step()
	@LuaEx
		x=5
		y=8
		tcod.line.init(x,y,13,4)
		repeat
			-- update cell x,y
			lineEnd,x,y = tcod.line.step(x,y)
		until lineEnd		
	*/	
	static bool step(int *xCur, int *yCur);

	/**
	@PageName line
	@FuncTitle Callback-based function
	@FuncDesc The function returns false if the line has been interrupted by the callback (it returned false before the last point).
	@Cpp
		class TCODLIB_API TCODLineListener {
			virtual bool putPoint (int x, int y) = 0;
		};
		static bool TCODLine::line (int xFrom, int yFrom, int xTo, int yTo, TCODLineListener * listener)
	@C
		typedef bool (*TCOD_line_listener_t) (int x, int y);
		bool TCOD_line(int xFrom, int yFrom, int xTo, int yTo, TCOD_line_listener_t listener)
	@Py
		def line_listener(x,y) : # ...
		line(xFrom, yFrom, xTo, yTo, listener)
	@C# static bool line(int xFrom, int yFrom, int xTo, int yTo, TCODLineListener listener)
	@Param xFrom,yFrom	Coordinates of the line's starting point.
	@Param xTo,yTo	Coordinates of the line's ending point.
	@Param listener	Callback called for each line's point. The function stops if the callback returns false.
	@CppEx // Going from point 5,8 to point 13,4
class MyLineListener : public TCODLineListener {
    public:
    bool putPoint (int x,int y) {
        printf ("%d %d\n",x,y);
        return true;
    }
};
MyLineListener myListener;
TCODLine::line(5,8,13,4,&myListener);
	@CEx bool my_listener(int x,int y) {
    printf ("%d %d\n",x,y);
    return true;
}
TCOD_line_line(5,8,13,4,my_listener);
	@PyEx def my_listener(x,y):
    print x,y
    return True
libtcod.line_line(5,8,13,4,my_listener)	 
	*/
	static bool line(int xFrom, int yFrom, int xTo, int yTo, TCODLineListener *listener);
};

#endif
