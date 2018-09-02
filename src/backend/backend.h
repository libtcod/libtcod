
#ifndef LIBTCOD_BACKEND_BACKEND_H_
#define LIBTCOD_BACKEND_BACKEND_H_

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
  virtual ~Backend() {}
  virtual void activate() {}
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
 *  Unique pointer to the active backend object.
 *
 *  This variable is private, use tcod::backend::get to get the active backend.
 */
extern std::unique_ptr<Backend> active_backend_;
/**
 *  Change the active backend to another Backend subclass.
 */
template <class T>
inline void change() {
  if (active_backend_) { active_backend_->deactivate(); }
  active_backend_ = std::make_unique<T>();
  if (active_backend_) { active_backend_->activate(); }
}
/**
 *  Return the current backend interface.
 */
inline Backend& get() {
  if (!active_backend_) { change<Backend>(); }
  return *active_backend_;
}
} // namespace backend
} // namespace tcod
#endif /* __cplusplus */
#endif /* LIBTCOD_BACKEND_BACKEND_H_ */
