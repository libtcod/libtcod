#include <libtcod.h>

int main() {
  auto* console = TCOD_console_new(1, 1);
  if (!console) return 1;
  TCOD_console_delete(console);
}
