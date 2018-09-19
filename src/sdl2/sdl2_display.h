
#ifndef LIBTCOD_SDL2_SDL2_DISPLAY_H_
#define LIBTCOD_SDL2_SDL2_DISPLAY_H_

#include "sdl2_renderer.h"
#include "../engine/display.h"

struct SDL_Window;
struct SDL_Renderer;
#ifdef __cplusplus
namespace tcod {
namespace sdl2 {
class SDL2Display: public engine::Display {
 public:
  SDL2Display(int width, int height, std::shared_ptr<Tileset> tileset,
              int fullscreen);
  ~SDL2Display();
  virtual void set_tileset(std::shared_ptr<Tileset> tileset) override;
  virtual void set_title(const std::string title) override;
  virtual void present(const TCOD_Console*) override;
 private:
  SDL_Window* window_;
  SDL_Renderer* renderer_;
  SDL2Renderer tcod_renderer_;
};
} // namespace sdl2
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_SDL2_SDL2_DISPLAY_H_
