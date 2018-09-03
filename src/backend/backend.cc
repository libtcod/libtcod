
#include <utility>

#include "backend.h"
namespace tcod {
namespace backend {
/**
 *  Unique pointer to the active backend object.
 *
 *  This variable is private, use tcod::backend::get to get the active backend.
 */
std::unique_ptr<Backend> active_backend_;
/**
 *  Change the active backend to another Backend subclass.
 */
void set(std::unique_ptr<Backend>&& backend) {
  if (active_backend_) { active_backend_->deactivate(); }
  active_backend_ = std::move(backend);
  if (active_backend_) { active_backend_->activate(); }
}
/**
 *  Return the current backend interface.
 */
Backend& get() {
  if (!active_backend_) { set(std::make_unique<Backend>()); }
  return *active_backend_;
}
} // namespace backend
} // namespace tcod
