#ifndef _TCOD_MOUSE_H
#define _TCOD_MOUSE_H

/* mouse data */
typedef struct {
  int x,y; /* absolute position */
  int dx,dy; /* movement since last update in pixels */
  int cx,cy; /* cell coordinates in the root console */
  int dcx,dcy; /* movement since last update in console cells */
  unsigned lbutton : 1; /* left button status */
  unsigned rbutton : 1; /* right button status */
  unsigned mbutton : 1; /* middle button status */
  unsigned lbutton_pressed : 1; /* left button pressed event */ 
  unsigned rbutton_pressed : 1; /* right button pressed event */ 
  unsigned mbutton_pressed : 1; /* middle button pressed event */ 
  unsigned wheel_up: 1; /* wheel up event */
  unsigned wheel_down: 1; /* wheel down event */
} TCOD_mouse_t;

/* mouse support */
TCODLIB_API TCOD_mouse_t TCOD_mouse_get_status();
TCODLIB_API void TCOD_mouse_show_cursor(bool visible);
TCODLIB_API bool TCOD_mouse_is_cursor_visible();
TCODLIB_API void TCOD_mouse_move(int x, int y);

#endif
