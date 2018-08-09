
#ifndef LIBTCOD_BACKEND_BACKEND_H_
#define LIBTCOD_BACKEND_BACKEND_H_

#ifdef __cplusplus
namespace tcod {
class Backend {
 public:
  virtual activate() {}
  virtual deactivate() {}
 protected
  virtual ~Backend() {}
}
} /* namespace tcod */
#endif /* __cplusplus */
#endif /* LIBTCOD_BACKEND_BACKEND_H_ */
