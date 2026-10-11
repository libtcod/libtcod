#include <libtcod.h>

int main() {
  TCOD_Console* console = TCOD_console_new(1, 1);
  if (!console) return 1;
  TCOD_console_delete(console);
  return 0;
}
