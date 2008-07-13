#ifndef _TCOD_MOUSE_HPP
#define _TCOD_MOUSE_HPP

class TCODLIB_API TCODMouse {
public :
  static TCOD_mouse_t getStatus();
  static void showCursor(bool visible);
  static bool isCursorVisible();
  static void move(int x, int y);
};

#endif
