
#ifndef LIBTCOD_ENGINE_BACKEND_H_
#define LIBTCOD_ENGINE_BACKEND_H_

#ifdef __cplusplus
#include <memory>
#endif /* __cplusplus */

#include "../console_types.h"
#include "../mouse_types.h"
#include "../sys.h"

#ifdef __cplusplus
namespace tcod {
namespace backend {
class Backend {
 public:
  virtual ~Backend() = default;
  /**
   *  Called when this Backend is setup.
   */
  virtual void activate() {}
  /**
   *  Called when this Backend is removed.
   */
  virtual void deactivate() {}
  /**
   *  Legacy wait for event virtual function.
   */
  virtual TCOD_event_t legacy_wait_for_event(
      int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse, bool flush) {
    return TCOD_EVENT_NONE;
  }
  /**
   *  Legacy check for event virtual function.
   */
  virtual TCOD_event_t legacy_check_for_event(
      int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse) {
    return TCOD_EVENT_NONE;
  }
  virtual void legacy_flush() { }
};
/**
 *  Change the active backend to another Backend instance.
 */
void set(std::unique_ptr<Backend>&& backend);
/**
 *  Change the active backend to another Backend class.
 */
template <class BackendType>
inline void set() {
  set(std::make_unique<BackendType>());
}
/**
 *  Return the current backend interface.
 */
Backend& get();
} // namespace backend
} // namespace tcod
#endif /* __cplusplus */
#endif /* LIBTCOD_ENGINE_BACKEND_H_ */
