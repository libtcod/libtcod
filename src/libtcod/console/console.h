#ifndef TCOD_CONSOLE_CONSOLE_H_
#define TCOD_CONSOLE_CONSOLE_H_

#include "../color/color.h"
/**
 *  \enum TCOD_bkgnd_flag_t
 *
 *  Background color blend modes.
 */
typedef enum {
  TCOD_BKGND_NONE,
  TCOD_BKGND_SET,
  TCOD_BKGND_MULTIPLY,
  TCOD_BKGND_LIGHTEN,
  TCOD_BKGND_DARKEN,
  TCOD_BKGND_SCREEN,
  TCOD_BKGND_COLOR_DODGE,
  TCOD_BKGND_COLOR_BURN,
  TCOD_BKGND_ADD,
  TCOD_BKGND_ADDA,
  TCOD_BKGND_BURN,
  TCOD_BKGND_OVERLAY,
  TCOD_BKGND_ALPH,
  TCOD_BKGND_DEFAULT
} TCOD_bkgnd_flag_t;
/**
 *  \enum TCOD_alignment_t
 *
 *  Print justification options.
 */
typedef enum {
  TCOD_LEFT,
  TCOD_RIGHT,
  TCOD_CENTER
} TCOD_alignment_t;
/**
 *  The libtcod console struct.
 *
 *  All attributes should be considered private.
 */
struct TCOD_Console {
  /** Character code array. */
  int *ch_array;
  /** Pointers to arrays of TCOD_color_t colors. */
  TCOD_color_t *fg_array, *bg_array;
  /** Console width and height (in characters, not pixels.) */
  int w,h;
  /** Default background operator for print & print_rect functions. */
  TCOD_bkgnd_flag_t bkgnd_flag;
  /** Default alignment for print & print_rect functions. */
  TCOD_alignment_t alignment;
  /** Foreground (text) and background colors. */
  TCOD_color_t fore, back;
  /** True if a key color is being used. */
  bool has_key_color;
  /** The current key color for this console. */
  TCOD_color_t key_color;
};
typedef struct TCOD_Console TCOD_Console;
typedef struct TCOD_Console *TCOD_console_t;
#endif // TCOD_CONSOLE_CONSOLE_H_
