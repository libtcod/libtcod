
#ifndef _TCOD_TOUCH_TYPES_H
#define _TCOD_TOUCH_TYPES_H

#ifdef TCOD_TOUCH_INPUT

#include <SDL_touch.h>

typedef struct {
  int nupdates; /* how many updates have happened since the first finger was pressed. */
  SDL_FingerID finger_id; /* the last finger which was pressed. */
  int coords_prev[MAX_TOUCH_FINGERS][2]; /* absolute position of each finger. */
  int coords[MAX_TOUCH_FINGERS][2]; /* absolute position of each finger. */
  int coords_delta[MAX_TOUCH_FINGERS][2]; /* absolute position of each finger. */
  int consolecoords[MAX_TOUCH_FINGERS][2]; /* cell coordinates in the root console for each finger. */
  int consolecoords_delta[MAX_TOUCH_FINGERS][2]; /* cell coordinates in the root console for each finger. */
  int nfingers; /* number of unique fingers employed at any time during this. */
  int nfingerspressed; /* number of those fingers currently still pressed. */
  SDL_FingerID finger_ids[MAX_TOUCH_FINGERS];
  char fingerspressed[MAX_TOUCH_FINGERS];
} TCOD_touch_t;

#endif

#endif /* _TCOD_TOUCH_TYPES_H */
