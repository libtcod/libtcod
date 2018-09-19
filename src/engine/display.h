
#ifndef LIBTCOD_ENGINE_DISPLAY_H_
#define LIBTCOD_ENGINE_DISPLAY_H_

#ifdef __cplusplus
#include <string>
#endif // __cplusplus

#include "../tileset/tileset.h"
#include "../console_types.h"

#ifdef __cplusplus
namespace tcod {
namespace engine {
using tcod::tileset::Tileset;
class Display {
 public:
  virtual void set_tileset(std::shared_ptr<Tileset> tileset) = 0;
  virtual void set_title(const std::string title) = 0;
  virtual void present(const TCOD_Console*) = 0;
};
} // namespace sdl2
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_ENGINE_DISPLAY_H_


