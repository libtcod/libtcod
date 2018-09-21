
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
  virtual ~Display() = default;
  virtual void set_tileset(std::shared_ptr<Tileset> tileset) = 0;
  virtual void set_title(const std::string title) = 0;
  virtual std::string get_title() = 0;
  virtual void present(const TCOD_Console*) = 0;
};
/**
 *  Incomplete interface for subclasses which don't need an SDL2 window.
 */
class TerminalDisplay: public Display {
 public:
  virtual void set_tileset(std::shared_ptr<Tileset> tileset) override
  {}
  virtual void set_title(const std::string title) override
  {}
  virtual std::string get_title() override
  {
    return {};
  }
};
} // namespace sdl2
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_ENGINE_DISPLAY_H_


