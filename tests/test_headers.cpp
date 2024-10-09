
#include <libtcod-fov.h>

#if defined(MAX) || defined(MIN) || defined(ABS) || defined(CLAMP) || defined(LERP)
#error "utility/h macros must not be included in main header"
#endif
