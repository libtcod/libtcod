#ifndef _TCOD_BRESENHAM_HPP
#define _TCOD_BRESENHAM_HPP

class TCODLIB_API TCODLine {
public :
	static void init(int xFrom, int yFrom, int xTo, int yTo);
	static bool step(int *xCur, int *yCur);
};

#endif
