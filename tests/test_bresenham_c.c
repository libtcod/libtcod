#include <libtcod/bresenham.h>
#include <stddef.h>

/** Check the public C declaration and linkage independently of C++ callers. */
int test_bresenham_c(void) {
  int points[10] = {0};
  if (TCOD_bresenham(0, 0, 4, 4, 0, NULL) != 5) return 1;
  if (TCOD_bresenham(0, 0, 4, 4, 5, points) != 5) return 2;
  for (int i = 0; i < 5; ++i) {
    if (points[i * 2] != i || points[i * 2 + 1] != i) return 3;
  }
  return 0;
}
