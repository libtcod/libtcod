
#ifndef LIBTCOD_BACKEND_H_
#define LIBTCOD_BACKEND_H_

#ifdef __cplusplus
namespace tcod {
class Backend {
 public:
  virtual Activate(void) {}
  virtual Deactivate(void) {}
}
} /* namespace tcod */
#endif /* __cplusplus */
#endif /* LIBTCOD_BACKEND_H_ */
