#ifndef _TCOD_BRESENHAM_H
#define _TCOD_BRESENHAM_H

TCODLIB_API void TCOD_line_init(int xFrom, int yFrom, int xTo, int yTo);
TCODLIB_API bool TCOD_line_step(int *xCur, int *yCur); // advance one step. returns true if we reach destination

#endif
