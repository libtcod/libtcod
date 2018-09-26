
#ifndef LIBTCOD_SDL2_SDL2_DISPLAY_H_
#define LIBTCOD_SDL2_SDL2_DISPLAY_H_

#ifdef __cplusplus
#include <memory>
#include <utility>
#endif // __cplusplus

#include "sdl2_renderer.h"
#include "../engine/display.h"

struct SDL_Window;
struct SDL_Renderer;
#ifdef __cplusplus
namespace tcod {
namespace sdl2 {
/**
 *  Incomplete interface for subclasses expecting an SDL2 window.
 */
class WindowedDisplay: public engine::Display {
 public:
  WindowedDisplay(std::pair<int, int> window_size, int window_flags);
  ~WindowedDisplay();
  virtual void set_title(const std::string title) override;
  virtual std::string get_title() override;
  SDL_Window* get_window() { return window_.get(); }
 private:
  std::shared_ptr<SDL_Window> window_;
};
/**
 *  Display interface using a basic SDL2 renderer.
 */
class SDL2Display: public WindowedDisplay {
 public:
  SDL2Display(std::shared_ptr<Tileset> tileset,
              std::pair<int, int> window_size, int fullscreen);
  ~SDL2Display();
  virtual void set_tileset(std::shared_ptr<Tileset> tileset) override;
  virtual void present(const TCOD_Console*) override;
 private:
  std::shared_ptr<SDL_Renderer> renderer_;
  SDL2Renderer tcod_renderer_;
};
} // namespace sdl2
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_SDL2_SDL2_DISPLAY_H_
