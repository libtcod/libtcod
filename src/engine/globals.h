
#ifndef LIBTCOD_ENGINE_GLOBALS_H_
#define LIBTCOD_ENGINE_GLOBALS_H_

#ifdef __cplusplus
#include <memory>
#endif // __cplusplus

#include "backend.h"
#include "display.h"

#ifdef __cplusplus
namespace tcod {
namespace engine {
/**
 *  Replace the active backend with a new instance, can be nullptr.
 */
void set_backend(std::shared_ptr<Backend> backend);
/**
 *  Return a shared pointer to the active backend.  Might be nullptr.
 */
std::shared_ptr<Backend> get_backend();
/**
 *  Replace the active display with a new instance, can be nullptr.
 */
void set_display(std::shared_ptr<Display> display);
/**
 *  Return a shared pointer to the active display.  Might be nullptr.
 */
std::shared_ptr<Display> get_display();
} // namespace sdl2
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_ENGINE_GLOBALS_H_


